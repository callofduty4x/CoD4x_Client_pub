/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#include "q_shared.h"
#include "qcommon.h"
#include "win_sys.h"

#include <stdlib.h>

#ifdef _WIN32
#	include <winsock2.h>
#	include <ws2tcpip.h>
/*
#	if WINVER < 0x501
#		ifdef __MINGW32__
			// wspiapi.h isn't available on MinGW, so if it's
			// present it's because the end user has added it
			// and we should look for it in our tree
#			include "wspiapi.h"
#		else
#			include <wspiapi.h>
#		endif
#	else
*/
#		include <ws2spi.h>
//#	endif

typedef int socklen_t;
#	ifdef ADDRESS_FAMILY
#		define sa_family_t	ADDRESS_FAMILY
#	else
typedef unsigned short sa_family_t;
#	endif

/* no epipe yet */
#ifndef WSAEPIPE
    #define WSAEPIPE       -12345
#endif
#	define EAGAIN					WSAEWOULDBLOCK
#	define EADDRNOTAVAIL	WSAEADDRNOTAVAIL
#	define EAFNOSUPPORT		WSAEAFNOSUPPORT
#	define ECONNRESET			WSAECONNRESET
#	define EINPROGRESS		WSAEINPROGRESS
#	define EINTR			WSAEINTR
# define EPIPE      WSAEPIPE
typedef u_long	ioctlarg_t;
#	define socketError		WSAGetLastError( )

#	define NET_NOSIGNAL 0x0

static WSADATA	winsockdata;
static qboolean	winsockInitialized = qfalse;

#	include "win_gqos.h"

int inet_pton(int af, const char *src, void *dst)
{
	struct sockaddr_storage sin;
	int addrSize = sizeof(sin);
	char address[256];
	strncpy(address, src, sizeof(address));

	int rc = WSAStringToAddressA( address, af, NULL, (SOCKADDR*)&sin, &addrSize ); 
	if(rc != 0)
	{
		return -1;
	}
	if(af == AF_INET)
	{
		*((struct in_addr *)dst) = ((struct sockaddr_in*)&sin)->sin_addr;
		return 1;
	}
	if(af == AF_INET6)
	{
		*((struct in_addr6 *)dst) = ((struct sockaddr_in6*)&sin)->sin6_addr;
		return 1;
	}
	return 0;
}

#else

#	if MAC_OS_X_VERSION_MIN_REQUIRED == 1020
		// needed for socklen_t on OSX 10.2
#		define _BSD_SOCKLEN_T_
#	endif

#	ifdef MACOS_X
#		define NET_NOSIGNAL SO_NOSIGPIPE
#	else
#		define NET_NOSIGNAL MSG_NOSIGNAL
#	endif

#	include <sys/socket.h>
#	include <errno.h>
#	include <netdb.h>
#	include <netinet/in.h>
#	include <arpa/inet.h>
#	include <net/if.h>
#	include <sys/ioctl.h>
#	include <sys/types.h>
#	include <sys/time.h>
#	include <unistd.h>
#	if !defined(__sun) && !defined(__sgi)
#		include <ifaddrs.h>
#	endif

#	ifdef __sun
#		include <sys/filio.h>
#	endif

typedef int SOCKET;
#	define INVALID_SOCKET		-1
#	define SOCKET_ERROR			-1
#	define closesocket			close
#	define ioctlsocket			ioctl
typedef int	ioctlarg_t;
#	define socketError			errno

#endif

static qboolean usingSocks = qfalse;
static int networkingEnabled = 0;

static cvar_t	*net_enabled;

static cvar_t	*net_socksEnabled;
static cvar_t	*net_socksServer;
static cvar_t	*net_socksPort;
static cvar_t	*net_socksUsername;
static cvar_t	*net_socksPassword;
#ifdef ENABLE_FAKELAG
static cvar_t	*net_fakelag;
#endif
cvar_t	*net_ip;
cvar_t	*net_ip6;
cvar_t	*net_port;
cvar_t	*net_port6;
static cvar_t	*net_mcast6addr;
static cvar_t	*net_mcast6iface;

static struct sockaddr	socksRelayAddr;

static SOCKET	ip_socket = INVALID_SOCKET;
static SOCKET	ip6_socket = INVALID_SOCKET;
static SOCKET	socks_socket = INVALID_SOCKET;
static SOCKET	multicast6_socket = INVALID_SOCKET;

// Keep track of currently joined multicast group.
static struct ipv6_mreq curgroup;
// And the currently bound address.
static struct sockaddr_in6 boundto;

#ifndef IF_NAMESIZE
  #define IF_NAMESIZE 16
#endif

// use an admin local address per default so that network admins can decide on how to handle quake3 traffic.
#define NET_MULTICAST_IP6 "ff04::696f:7175:616b:6533"

#define	MAX_IPS		32

typedef struct
{
	char ifname[IF_NAMESIZE];

	netadrtype_t type;
	sa_family_t family;
	struct sockaddr_storage addr;
	struct sockaddr_storage netmask;
} nip_localaddr_t;

static nip_localaddr_t localIP[MAX_IPS];
static int numIP;
static qboolean ipv6avail;

//=============================================================================


/*
====================
NET_ErrorString
====================
*/
char *NET_ErrorString( void ) {
#ifdef _WIN32
	//FIXME: replace with FormatMessage?
	switch( socketError ) {
		case WSAEINTR: return "WSAEINTR";
		case WSAEBADF: return "WSAEBADF";
		case WSAEACCES: return "WSAEACCES";
		case WSAEDISCON: return "WSAEDISCON";
		case WSAEFAULT: return "WSAEFAULT";
		case WSAEINVAL: return "WSAEINVAL";
		case WSAEMFILE: return "WSAEMFILE";
		case WSAEWOULDBLOCK: return "WSAEWOULDBLOCK";
		case WSAEINPROGRESS: return "WSAEINPROGRESS";
		case WSAEALREADY: return "WSAEALREADY";
		case WSAENOTSOCK: return "WSAENOTSOCK";
		case WSAEDESTADDRREQ: return "WSAEDESTADDRREQ";
		case WSAEMSGSIZE: return "WSAEMSGSIZE";
		case WSAEPROTOTYPE: return "WSAEPROTOTYPE";
		case WSAENOPROTOOPT: return "WSAENOPROTOOPT";
		case WSAEPROTONOSUPPORT: return "WSAEPROTONOSUPPORT";
		case WSAESOCKTNOSUPPORT: return "WSAESOCKTNOSUPPORT";
		case WSAEOPNOTSUPP: return "WSAEOPNOTSUPP";
		case WSAEPFNOSUPPORT: return "WSAEPFNOSUPPORT";
		case WSAEAFNOSUPPORT: return "WSAEAFNOSUPPORT";
		case WSAEADDRINUSE: return "WSAEADDRINUSE";
		case WSAEADDRNOTAVAIL: return "WSAEADDRNOTAVAIL";
		case WSAENETDOWN: return "WSAENETDOWN";
		case WSAENETUNREACH: return "WSAENETUNREACH";
		case WSAENETRESET: return "WSAENETRESET";
		case WSAECONNABORTED: return "WSWSAECONNABORTEDAEINTR";
		case WSAECONNRESET: return "WSAECONNRESET";
		case WSAENOBUFS: return "WSAENOBUFS";
		case WSAEISCONN: return "WSAEISCONN";
		case WSAENOTCONN: return "WSAENOTCONN";
		case WSAESHUTDOWN: return "WSAESHUTDOWN";
		case WSAETOOMANYREFS: return "WSAETOOMANYREFS";
		case WSAETIMEDOUT: return "WSAETIMEDOUT";
		case WSAECONNREFUSED: return "WSAECONNREFUSED";
		case WSAELOOP: return "WSAELOOP";
		case WSAENAMETOOLONG: return "WSAENAMETOOLONG";
		case WSAEHOSTDOWN: return "WSAEHOSTDOWN";
		case WSASYSNOTREADY: return "WSASYSNOTREADY";
		case WSAVERNOTSUPPORTED: return "WSAVERNOTSUPPORTED";
		case WSANOTINITIALISED: return "WSANOTINITIALISED";
		case WSAHOST_NOT_FOUND: return "WSAHOST_NOT_FOUND";
		case WSATRY_AGAIN: return "WSATRY_AGAIN";
		case WSANO_RECOVERY: return "WSANO_RECOVERY";
		case WSANO_DATA: return "WSANO_DATA";
		default: return "NO ERROR";
	}
#else
	return strerror(socketError);
#endif
}

void NET_UnmapIpv6v4(netadr_t* mapped, netadr_t* unmapped)
{
	int i;
	if(mapped->type != NA_IP6)
	{
		if(mapped->type == NA_IP)
		{
			*unmapped = *mapped;
		}else{
			unmapped->type = NA_BAD;
		}
		return;
	}
	unmapped->type = NA_IP;
	unmapped->scope_id = 0;
	unmapped->port = mapped->port;
	for(i = 0; i < 4; ++i)
	{
		unmapped->ip[i] = mapped->ip6[12 +i];
	}

}

qboolean NET_IsIPv6Mappedv4(netadr_t* adr)
{
	int i;

	for(i = 0; i < 10; ++i)
	{
		if(adr->ip6[i] != 0x00)
		{
			return qfalse;
		}
	}
	for( ; i < 12; ++i)
	{
		if(adr->ip6[i] != 0xff)
		{
			return qfalse;
		}
	}
	return qtrue;
}


void NetadrToSockadr( netadr_t *a, struct sockaddr *s ) {
	if( a->type == NA_BROADCAST ) {
		((struct sockaddr_in *)s)->sin_family = AF_INET;
		((struct sockaddr_in *)s)->sin_port = a->port;
		((struct sockaddr_in *)s)->sin_addr.s_addr = INADDR_BROADCAST;
	}
	else if( a->type == NA_IP ) {
		((struct sockaddr_in *)s)->sin_family = AF_INET;
		((struct sockaddr_in *)s)->sin_addr.s_addr = *(int *)&a->ip;
		((struct sockaddr_in *)s)->sin_port = a->port;
	}
	else if( a->type == NA_IP6 ) {
		if(NET_IsIPv6Mappedv4(a))
		{
			((struct sockaddr_in *)s)->sin_family = AF_INET;
			((struct sockaddr_in *)s)->sin_addr.s_addr = *(int *)&a->ip6[12];
			((struct sockaddr_in *)s)->sin_port = a->port;
		}else{
			((struct sockaddr_in6 *)s)->sin6_family = AF_INET6;
			((struct sockaddr_in6 *)s)->sin6_addr = * ((struct in6_addr *) &a->ip6);
			((struct sockaddr_in6 *)s)->sin6_port = a->port;
			((struct sockaddr_in6 *)s)->sin6_scope_id = a->scope_id;
		}
	}
	else if(a->type == NA_MULTICAST6)
	{
		((struct sockaddr_in6 *)s)->sin6_family = AF_INET6;
		((struct sockaddr_in6 *)s)->sin6_addr = curgroup.ipv6mr_multiaddr;
		((struct sockaddr_in6 *)s)->sin6_port = a->port;
	}
}


static void SockadrToNetadr( struct sockaddr *s, netadr_t *a ) {
	if (s->sa_family == AF_INET) {
		a->type = NA_IP;
		*(int *)&a->ip = ((struct sockaddr_in *)s)->sin_addr.s_addr;
		a->port = ((struct sockaddr_in *)s)->sin_port;
	}
	else if(s->sa_family == AF_INET6)
	{
		a->type = NA_IP6;
		memcpy(a->ip6, &((struct sockaddr_in6 *)s)->sin6_addr, sizeof(a->ip6));
		a->port = ((struct sockaddr_in6 *)s)->sin6_port;
		a->scope_id = ((struct sockaddr_in6 *)s)->sin6_scope_id;
	}
}


static struct addrinfo *SearchAddrInfo(struct addrinfo *hints, sa_family_t family)
{
	while(hints)
	{
		if(hints->ai_family == family)
			return hints;

		hints = hints->ai_next;
	}

	return NULL;
}


static qboolean Sys_StringToSockaddrNoDNS(const char* s, struct sockaddr *sadr, int sadr_len, sa_family_t family)
{
	char ptonaddr[32];
	char addressstring[128];
	char *ifstring;
	struct sockaddr_storage sadrstore;
	struct sockaddr_in *sin = (struct sockaddr_in *)&sadrstore;
	struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)&sadrstore;
	memset(sadr, '\0', sadr_len);
	memset(&sadrstore, '\0', sizeof(sadrstore));
	
	sa_family_t ptonfamily = family;

	Q_strncpyz(addressstring, s, sizeof(addressstring));

	if(ptonfamily == AF_UNSPEC)
	{
		 if(Q_CountChar(addressstring, ':') > 1)
		 {
			ptonfamily = AF_INET6;
		 }else{
			ptonfamily = AF_INET;
		 }
	}
	if(ptonfamily == AF_INET6)
	{
		ifstring = strchr(addressstring, '%');
		if(ifstring)
		{
			*ifstring = '\0';
			++ifstring;
		}
		if(Q_CountChar(addressstring, ':') < 2)
		{
			//Turn ipv4 into mapped ipv6 address
			char ipv4string[32];
			Q_strncpyz(ipv4string, addressstring, sizeof(ipv4string));
			Com_sprintf(addressstring, sizeof(addressstring), "::ffff:%s", ipv4string);
		}
	}

	if(inet_pton(ptonfamily, addressstring, &ptonaddr ) > 0)
	{
		if(ptonfamily == AF_INET6)
		{
			memcpy(&sin6->sin6_addr, ptonaddr, sizeof(sin6->sin6_addr));
			sin6->sin6_family = AF_INET6;
			if(ifstring)
			{
#ifdef _WIN32
				sin6->sin6_scope_id = atoi(ifstring);
#else
				sin6->sin6_scope_id = if_nametoindex(ifstring);
#endif
			}
		}else if(ptonfamily == AF_INET){
			memcpy(&sin->sin_addr, ptonaddr, sizeof(sin->sin_addr)); 
			sin->sin_family = AF_INET;
		}
		if(sadr_len > sizeof(sadrstore))
		{
			sadr_len = sizeof(sadrstore);
		}
		memcpy(sadr, &sadrstore, sadr_len);
		return qtrue;
	}
	return qfalse;
}


/*
=============
Sys_StringToSockaddr
=============
*/
static qboolean Sys_StringToSockaddr(const char *s, struct sockaddr *sadr, int sadr_len, sa_family_t family)
{
	struct addrinfo hints;
	struct addrinfo *res = NULL;
	struct addrinfo *search = NULL;
	struct addrinfo *hintsp;
	int retval;
	char ptonaddr[32];
	struct sockaddr_storage sadrstore;
	struct sockaddr_in *sin = (struct sockaddr_in *)&sadrstore;
	struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)&sadrstore;

	if(Sys_StringToSockaddrNoDNS(s, sadr, sadr_len, family))
	{
		return qtrue;
	}

	if(Q_stricmp(s, "localhost") == 0)
	{
		memset(ptonaddr, 0, sizeof(ptonaddr));
		
		if(family == AF_INET){
			ptonaddr[0] = 127;
			ptonaddr[3] = 1;
			memcpy(&sin->sin_addr, ptonaddr, sizeof(sin->sin_addr)); 
			sin->sin_family = AF_INET;
			return qtrue;
		}
		if(family == AF_INET6)
		{
			ptonaddr[15] = 1;
			memcpy(&sin6->sin6_addr, ptonaddr, sizeof(sin6->sin6_addr));
			sin6->sin6_family = AF_INET6;
			return qtrue;
		}
		return qfalse;
	}

	memset(sadr, '\0', sizeof(*sadr));
	memset(&hints, '\0', sizeof(hints));

	hintsp = &hints;
	hintsp->ai_family = family;
	hintsp->ai_socktype = SOCK_DGRAM;

	retval = getaddrinfo(s, NULL, hintsp, &res);

	if(!retval)
	{
		if(family == AF_UNSPEC)
		{
			// Decide here and now which protocol family to use
			if(net_enabled->integer & NET_PRIOV6)
			{
				if(net_enabled->integer & NET_ENABLEV6)
					search = SearchAddrInfo(res, AF_INET6);

				if(!search && (net_enabled->integer & NET_ENABLEV4))
					search = SearchAddrInfo(res, AF_INET);
			}
			else
			{
				if(net_enabled->integer & NET_ENABLEV4)
					search = SearchAddrInfo(res, AF_INET);

				if(!search && (net_enabled->integer & NET_ENABLEV6))
					search = SearchAddrInfo(res, AF_INET6);
			}
		}
		else
			search = SearchAddrInfo(res, family);

		if(search)
		{
			if(search->ai_addrlen > sadr_len)
				search->ai_addrlen = sadr_len;

			memcpy(sadr, search->ai_addr, search->ai_addrlen);
			freeaddrinfo(search);

			return qtrue;
		}
		else
			Com_Printf(CON_CHANNEL_NETWORK, "Sys_StringToSockaddr: Error resolving %s: No address of required type found.\n", s);
	}
	else
		Com_Printf(CON_CHANNEL_NETWORK, "Sys_StringToSockaddr: Error resolving %s: %s\n", s, gai_strerror(retval));

	if(res)
		freeaddrinfo(res);

	return qfalse;
}

/*
=============
Sys_SockaddrToString
=============
*/
static void Sys_SockaddrToString(char *dest, int destlen, struct sockaddr *input)
{
	socklen_t inputlen;

	if (input->sa_family == AF_INET6)
		inputlen = sizeof(struct sockaddr_in6);
	else
		inputlen = sizeof(struct sockaddr_in);

	if(getnameinfo(input, inputlen, dest, destlen, NULL, 0, NI_NUMERICHOST) && destlen > 0)
		*dest = '\0';
}

/*
=============
Sys_StringToAdr
=============
*/
qboolean Sys_StringToAdr( const char *s, netadr_t *a, netadrtype_t family ) {
	struct sockaddr_storage sadr;
	sa_family_t fam;

	switch(family)
	{
		case NA_IP:
			fam = AF_INET;
		break;
		case NA_IP6:
			fam = AF_INET6;
		break;
		default:
			fam = AF_UNSPEC;
		break;
	}

	if( !Sys_StringToSockaddr(s, (struct sockaddr *) &sadr, sizeof(sadr), fam ) ) {
		return qfalse;
	}

	SockadrToNetadr( (struct sockaddr *) &sadr, a );
	return qtrue;
}

/*
===================
NET_CompareBaseAdrMask

Compare without port, and up to the bit number given in netmask.
===================
*/
qboolean NET_CompareBaseAdrMask(netadr_t *a, netadr_t *b, int netmask)
{
	byte cmpmask, *addra, *addrb;
	int curbyte;
	netadr_t as;
	netadr_t bs;

	if (a->type != b->type)
	{
		if(a->type == NA_IP6 && NET_IsIPv6Mappedv4(a))
		{
			NET_UnmapIpv6v4(a, &as);
			a = &as;
		}
		if(b->type == NA_IP6 && NET_IsIPv6Mappedv4(b))
		{
			NET_UnmapIpv6v4(b, &bs);
			b = &bs;
		}		
		if (a->type != b->type)
		{
			return qfalse;
		}
	}
	if (a->type == NA_LOOPBACK)
		return qtrue;

	if(a->type == NA_IP)
	{
		addra = (byte *) &a->ip;
		addrb = (byte *) &b->ip;

		if(netmask < 0 || netmask > 32)
			netmask = 32;
	}
	else if(a->type == NA_IP6)
	{
		addra = (byte *) &a->ip6;
		addrb = (byte *) &b->ip6;

		if(netmask < 0 || netmask > 128)
			netmask = 128;
	}
	else
	{
		Com_Printf(CON_CHANNEL_NETWORK, "NET_CompareBaseAdr: bad address type\n");
		return qfalse;
	}

	curbyte = netmask >> 3;

	if(curbyte && memcmp(addra, addrb, curbyte))
			return qfalse;

	netmask &= 0x07;
	if(netmask)
	{
		cmpmask = (1 << netmask) - 1;
		cmpmask <<= 8 - netmask;

		if((addra[curbyte] & cmpmask) == (addrb[curbyte] & cmpmask))
			return qtrue;
	}
	else
		return qtrue;

	return qfalse;
}


/*
===================
NET_CompareBaseAdr

Compares without the port
===================
*/
qboolean NET_CompareBaseAdr (netadr_t *a, netadr_t *b)
{
	return NET_CompareBaseAdrMask(a, b, -1);
}

const char	*NET_AdrToStringShort (netadr_t* a)
{
	static	char	s[NET_ADDRSTRMAXLEN];

	if(a == NULL)
		return "(null)";

	if (a->type == NA_LOOPBACK)
		Com_sprintf (s, sizeof(s), "loopback");
	else if (a->type == NA_BOT)
		Com_sprintf (s, sizeof(s), "bot");
	else if (a->type == NA_IP || a->type == NA_IP6)
	{
		struct sockaddr_storage sadr;

		memset(&sadr, 0, sizeof(sadr));
		NetadrToSockadr(a, (struct sockaddr *) &sadr);
		Sys_SockaddrToString(s, sizeof(s), (struct sockaddr *) &sadr);
	}
	return s;
}

const char	*NET_AdrToString(netadr_t* a)
{
	static	char	s[NET_ADDRSTRMAXLEN];
	char		t[NET_ADDRSTRMAXLEN];
	struct 		sockaddr_storage sadr;

	if(a == NULL)
		return "(null)";

	if (a->type == NA_LOOPBACK){
		Com_sprintf (s, sizeof(s), "loopback");
	}else if (a->type == NA_BOT){
		Com_sprintf (s, sizeof(s), "bot");
	}else if(a->type == NA_IP){
		memset(&sadr, 0, sizeof(sadr));
		NetadrToSockadr(a, (struct sockaddr *) &sadr);
		Sys_SockaddrToString(t, sizeof(t), (struct sockaddr *) &sadr);
		Com_sprintf(s, sizeof(s), "%s:%hu", t, ntohs(a->port));

	}else if(a->type == NA_IP6){
		memset(&sadr, 0, sizeof(sadr));
		NetadrToSockadr(a, (struct sockaddr *) &sadr);
		Sys_SockaddrToString(t, sizeof(t), (struct sockaddr *) &sadr);
		Com_sprintf(s, sizeof(s), "[%s]:%hu", t, ntohs(a->port));
    }
	return s;
}

qboolean	NET_CompareAdr (netadr_t *a, netadr_t *b)
{
	if(!NET_CompareBaseAdr(a, b))
		return qfalse;

	if (a->type == NA_IP || a->type == NA_IP6)
	{
		if (a->port == b->port)
			return qtrue;
	}
	else
		return qtrue;

	return qfalse;
}


/*
===================
NET_CompareAdrSigned

Compare with port, IPv4 and IPv6 addresses and return if the are greater or smaller.
===================
*/

int NET_CompareAdrSigned(netadr_t *a, netadr_t *b)
{
	int i;

	if(a->type == NA_IP && b->type == NA_IP)
	{
		for(i = 0; i < 4; i++)
		{
			if(a->ip[i] < b->ip[i])
				return -1;
			else if(a->ip[i] > b->ip[i])
				return 1;
		}
		if(a->port < b->port)
			return -1;
		if(a->port > b->port)
			return 1;
		return 0;
	}

	if(a->type == NA_IP6 && b->type == NA_IP6)
	{
		for(i = 0; i < 16; i++)
		{
			if(a->ip6[i] < b->ip6[i])
				return -1;
			else if(a->ip6[i] > b->ip6[i])
				return 1;
		}
		if(a->port < b->port)
			return -1;
		if(a->port > b->port)
			return 1;
		return 0;
	}

	/* NA_IP6 is always greater than NA_IP. Port does not matter here */
	if(a->type == NA_IP && b->type == NA_IP6)
	{
		for(i = 0; i < 4; i++)
		{
			if(a->ip[i] < b->ip6[i])
				return -1;
			else if(a->ip[i] > b->ip6[i])
				return 1;
		}
		return -1;
	}

	if(a->type == NA_IP6 && b->type == NA_IP)
	{
		for(i = 0; i < 4; i++)
		{
			if(a->ip6[i] < b->ip[i])
				return -1;
			else if(a->ip6[i] > b->ip[i])
				return 1;
		}
		return 1;
	}

	Com_PrintError(CON_CHANNEL_ERROR,  "NET_CompareAdrSigned: bad address type\n");
    return 0;
}

qboolean	NET_IsLocalAddress( netadr_t adr ) {
	return adr.type == NA_LOOPBACK;
}

//=============================================================================

/*
==================
NET_GetPacket

Receive one packet
==================
*/
#ifdef _DEBUG
int	recvfromCount;
#endif

qboolean NET_GetPacketInternal(netadr_t *net_from, msg_t *net_message, netadrtype_t type)
{
	int 	ret;
	struct sockaddr_storage from;
	socklen_t	fromlen;
	int		err;

#ifdef _DEBUG
	recvfromCount++;		// performance check
#endif

	if(type == NA_IP && ip_socket != INVALID_SOCKET)
	{
		fromlen = sizeof(from);
		ret = recvfrom( ip_socket, (void *)net_message->data, net_message->maxsize, 0, (struct sockaddr *) &from, &fromlen );

		if (ret == SOCKET_ERROR)
		{
			err = socketError;

			if( err != EAGAIN && err != ECONNRESET )
				Com_PrintError(CON_CHANNEL_NETWORK, "NET_GetPacket: %s\n", NET_ErrorString() );
		}
		else
		{

			memset( ((struct sockaddr_in *)&from)->sin_zero, 0, 8 );

			if ( usingSocks && memcmp( &from, &socksRelayAddr, fromlen ) == 0 ) {
				if ( ret < 10 || net_message->data[0] != 0 || net_message->data[1] != 0 || net_message->data[2] != 0 || net_message->data[3] != 1 ) {
					return qfalse;
				}
				net_from->type = NA_IP;
				net_from->ip[0] = net_message->data[4];
				net_from->ip[1] = net_message->data[5];
				net_from->ip[2] = net_message->data[6];
				net_from->ip[3] = net_message->data[7];
				net_from->port = *(short *)&net_message->data[8];
				net_message->readcount = 10;
			}
			else {
				SockadrToNetadr( (struct sockaddr *) &from, net_from );
				net_message->readcount = 0;
			}

			if( ret >= net_message->maxsize ) {
				Com_PrintError(CON_CHANNEL_NETWORK,  "Oversize packet from %s\n", NET_AdrToString ( net_from ) );
				return qfalse;
			}

			net_message->cursize = ret;
			return qtrue;
		}
	}else if(type == NA_IP6 && ip6_socket != INVALID_SOCKET){
		fromlen = sizeof(from);
		ret = recvfrom(ip6_socket, (void *)net_message->data, net_message->maxsize, 0, (struct sockaddr *) &from, &fromlen);

		if (ret == SOCKET_ERROR)
		{
			err = socketError;

			if( err != EAGAIN && err != ECONNRESET )
				Com_PrintError(CON_CHANNEL_NETWORK, "NET_GetPacket: %s\n", NET_ErrorString() );
		}
		else
		{
			SockadrToNetadr((struct sockaddr *) &from, net_from);
			net_message->readcount = 0;

			if(ret >= net_message->maxsize)
			{
				Com_PrintError(CON_CHANNEL_NETWORK,  "Oversize packet from %s\n", NET_AdrToString ( net_from ) );
				return qfalse;
			}

			net_message->cursize = ret;
			return qtrue;
		}

	}else if(type == NA_MULTICAST6 && multicast6_socket != INVALID_SOCKET){

		fromlen = sizeof(from);
		ret = recvfrom(multicast6_socket, (void *)net_message->data, net_message->maxsize, 0, (struct sockaddr *) &from, &fromlen);

		if (ret == SOCKET_ERROR)
		{
			err = socketError;

			if( err != EAGAIN && err != ECONNRESET )
				Com_PrintError(CON_CHANNEL_ERROR,  "NET_GetPacket: %s\n", NET_ErrorString() );
		}
		else
		{
			SockadrToNetadr((struct sockaddr *) &from, net_from);
			net_message->readcount = 0;

			if(ret >= net_message->maxsize)
			{
				Com_PrintError(CON_CHANNEL_NETWORK,  "Oversize packet from %s\n", NET_AdrToString ( net_from ) );
				return qfalse;
			}

			net_message->cursize = ret;
			return qtrue;
		}
	}
	return qfalse;
}

//=============================================================================



/*
==================
Sys_SendPacket
==================
*/
qboolean Sys_SendPacket( int length, const void *data, netadr_t* to ) {
	int				ret = SOCKET_ERROR;
	struct sockaddr_storage	addr;
	char socksBuf[4096];

	if( to->type != NA_BROADCAST && to->type != NA_IP && to->type != NA_IP6 && to->type != NA_MULTICAST6)
	{
		Com_Error( ERR_FATAL, "Sys_SendPacket: bad address type" );
		return qfalse;
	}

	if( (ip_socket == INVALID_SOCKET && to->type == NA_IP) ||
		(ip6_socket == INVALID_SOCKET && to->type == NA_IP6) ||
		(ip6_socket == INVALID_SOCKET && to->type == NA_MULTICAST6) )
		return qfalse;

	if(to->type == NA_MULTICAST6 && (net_enabled->integer & NET_DISABLEMCAST))
		return qfalse;

	memset(&addr, 0, sizeof(addr));
	NetadrToSockadr( to, (struct sockaddr *) &addr );

	if( usingSocks && to->type == NA_IP ) {
		socksBuf[0] = 0;	// reserved
		socksBuf[1] = 0;
		socksBuf[2] = 0;	// fragment (not fragmented)
		socksBuf[3] = 1;	// address type: IPV4
		*(int *)&socksBuf[4] = ((struct sockaddr_in *)&addr)->sin_addr.s_addr;
		*(short *)&socksBuf[8] = ((struct sockaddr_in *)&addr)->sin_port;
		memcpy( &socksBuf[10], data, length );
		ret = sendto( ip_socket, socksBuf, length+10, 0, &socksRelayAddr, sizeof(socksRelayAddr) );

	} else {

		if(addr.ss_family == AF_INET)
		{
			ret = sendto( ip_socket, data, length, 0, (struct sockaddr *) &addr, sizeof(struct sockaddr_in) );

		}else if(addr.ss_family == AF_INET6){
				ret = sendto( ip6_socket, data, length, 0, (struct sockaddr *) &addr, sizeof(struct sockaddr_in6) );
		}
	}

	if( ret == SOCKET_ERROR ) {
		int err = socketError;

		// wouldblock is silent
		if( err == EAGAIN ) {
			return qfalse;
		}

		// some PPP links do not allow broadcasts and return an error
		if( ( err == EADDRNOTAVAIL ) && ( ( to->type == NA_BROADCAST ) ) ) {
			return qfalse;
		}
		Sys_EnterCriticalSection(20);
		Com_Printf(CON_CHANNEL_NETWORK,  "NET_SendPacket: %s\n", NET_ErrorString() );
		Sys_LeaveCriticalSection(20);
		return qfalse;
	}
	return qtrue;
}


//=============================================================================

/*
==================
Sys_IsLANAddress

LAN clients will have their rate var ignored
==================
*/
qboolean Sys_IsLANAddress( netadr_t adr ) {
	int		index, run, addrsize;
	qboolean differed;
	byte *compareadr, *comparemask, *compareip;

	if( adr.type == NA_LOOPBACK ) {
		return qtrue;
	}

	if( adr.type == NA_IP )
	{
		// RFC1918:
		// 10.0.0.0        -   10.255.255.255  (10/8 prefix)
		// 172.16.0.0      -   172.31.255.255  (172.16/12 prefix)
		// 192.168.0.0     -   192.168.255.255 (192.168/16 prefix)
		if(adr.ip[0] == 10)
			return qtrue;
		if(adr.ip[0] == 172 && (adr.ip[1]&0xf0) == 16)
			return qtrue;
		if(adr.ip[0] == 192 && adr.ip[1] == 168)
			return qtrue;

		if(adr.ip[0] == 127)
			return qtrue;
	}
	else if(adr.type == NA_IP6)
	{
		if(adr.ip6[0] == 0xfe && (adr.ip6[1] & 0xc0) == 0x80)
			return qtrue;
		if((adr.ip6[0] & 0xfe) == 0xfc)
			return qtrue;
	}

	// Now compare against the networks this computer is member of.
	for(index = 0; index < numIP; index++)
	{
		if(localIP[index].type == adr.type)
		{
			if(adr.type == NA_IP)
			{
				compareip = (byte *) &((struct sockaddr_in *) &localIP[index].addr)->sin_addr.s_addr;
				comparemask = (byte *) &((struct sockaddr_in *) &localIP[index].netmask)->sin_addr.s_addr;
				compareadr = adr.ip;

				addrsize = sizeof(adr.ip);
			}
			else
			{
				// TODO? should we check the scope_id here?

				compareip = (byte *) &((struct sockaddr_in6 *) &localIP[index].addr)->sin6_addr;
				comparemask = (byte *) &((struct sockaddr_in6 *) &localIP[index].netmask)->sin6_addr;
				compareadr = adr.ip6;

				addrsize = sizeof(adr.ip6);
			}

			differed = qfalse;
			for(run = 0; run < addrsize; run++)
			{
				if((compareip[run] & comparemask[run]) != (compareadr[run] & comparemask[run]))
				{
					differed = qtrue;
					break;
				}
			}

			if(!differed)
				return qtrue;

		}
	}

	return qfalse;
}

/*
==================
Sys_ShowIP
==================
*/
void Sys_ShowIP(void) {
	int i;
	char addrbuf[NET_ADDRSTRMAXLEN];

	for(i = 0; i < numIP; i++)
	{
		Sys_SockaddrToString(addrbuf, sizeof(addrbuf), (struct sockaddr *) &localIP[i].addr);

		if(localIP[i].type == NA_IP)
			Com_Printf(CON_CHANNEL_NETWORK,  "IP: %s\n", addrbuf);
		else if(localIP[i].type == NA_IP6)
			Com_Printf(CON_CHANNEL_NETWORK,  "IP6: %s\n", addrbuf);
	}
}

//Autodetection of internet version 6 addresses which do not change. Required as players should learn only permanent server addresses and not storing temporarly addresses in favorites
qboolean IsStaticIP6Addr(nip_localaddr_t* localaddr)
{
	int z;
	struct sockaddr_in6* t6;
				
	if(localaddr->type != NA_IP6)
	{
		return qfalse;
	}
	t6 = (struct sockaddr_in6*)&localaddr->addr;
	byte* baddr6 = (byte*)t6->sin6_addr.s6_addr;

	//Ignore site local, link local or multicast
	if(baddr6[0] >= 0xfc)
	{
		return qfalse;
	}
	for(z = 0; z < 4; ++z)
	{
		if(baddr6[z] != 0)
		{
			break;
		}
	}
	if(z == 4)
	{
		//Address is ::/64  (localhost or mapped IPv4 address)
		return qfalse;
	}
	return qtrue;
}



void Sys_CheckLocalAddressForIPv6()
{
	int i;
	for(i = 0; i < numIP; i++)
	{
		if(localIP[i].type != NA_IP6)
		{
			continue;
		}
		if(IsStaticIP6Addr(&localIP[i]))
		{
			ipv6avail = qtrue;
			return;
		}
	}
}


qboolean NET_IsIPv6Active()
{
	return ipv6avail;
}

//=============================================================================


/*
====================
NET_IPSocket
====================
*/
int NET_IPSocket( char *net_interface, int port, int *err ) {
	SOCKET				newsocket;
	struct sockaddr_in	address;
	ioctlarg_t			_true = 1;
	int					i;

	*err = 0;

	if( net_interface ) {
		Com_Printf(CON_CHANNEL_NETWORK,  "Opening IP socket: %s:%i\n", net_interface, port );
	}
	else {
		Com_Printf(CON_CHANNEL_NETWORK,  "Opening IP socket: 0.0.0.0:%i\n", port );
	}

	if( ( newsocket = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP ) ) == INVALID_SOCKET ) {
		*err = socketError;
		Com_Printf(CON_CHANNEL_NETWORK,  "WARNING: NET_IPSocket: socket: %s\n", NET_ErrorString() );
		return newsocket;
	}
	// make it non-blocking
	if( ioctlsocket( newsocket, FIONBIO, &_true ) == SOCKET_ERROR ) {
		Com_Printf(CON_CHANNEL_NETWORK,  "WARNING: NET_IPSocket: ioctl FIONBIO: %s\n", NET_ErrorString() );
		*err = socketError;
		closesocket(newsocket);
		return INVALID_SOCKET;
	}

	// make it broadcast capable
	i = 1;

	if( setsockopt( newsocket, SOL_SOCKET, SO_BROADCAST, (char *) &i, sizeof(i) ) == SOCKET_ERROR ) {
		Com_PrintError(CON_CHANNEL_NETWORK,  "NET_IPSocket: setsockopt SO_BROADCAST: %s\n", NET_ErrorString() );
	}
	// set the sendbuffersize to 64Kb
	i = 0x10000;

	if( setsockopt( newsocket, SOL_SOCKET, SO_SNDBUF, (char *) &i, sizeof(i) ) == SOCKET_ERROR ) {
		Com_PrintError(CON_CHANNEL_NETWORK,  "NET_IPSocket: setsockopt SO_SNDBUF: %s\n", NET_ErrorString() );
	}
	// set the receivebuffersize to 256Kb
	i = 0x40000;

	if( setsockopt( newsocket, SOL_SOCKET, SO_RCVBUF, (char *) &i, sizeof(i) ) == SOCKET_ERROR ) {
		Com_PrintError(CON_CHANNEL_NETWORK,  "NET_IPSocket: setsockopt SO_RCVBUF: %s\n", NET_ErrorString() );
	}
	// All outgoing packets have the QoS EF-Flag set
	i = 0xB8;

	if( setsockopt( newsocket, IPPROTO_IP, IP_TOS, (char *) &i, sizeof(i) ) == SOCKET_ERROR ) {
		Com_PrintError(CON_CHANNEL_NETWORK,  "NET_IPSocket: setsockopt IP_TOS: %s\n", NET_ErrorString() );
	}


	if( !net_interface || !net_interface[0]) {
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
	}
	else
	{
		if(!Sys_StringToSockaddr( net_interface, (struct sockaddr *)&address, sizeof(address), AF_INET))
		{
			closesocket(newsocket);
			return INVALID_SOCKET;
		}
	}

	if( port == PORT_ANY ) {
		address.sin_port = 0;
	}
	else {
		address.sin_port = htons( (short)port );
	}

	if( bind( newsocket, (void *)&address, sizeof(address) ) == SOCKET_ERROR ) {
		Com_Printf(CON_CHANNEL_NETWORK,  "WARNING: NET_IPSocket: bind: %s\n", NET_ErrorString() );
		*err = socketError;
		closesocket( newsocket );
		return INVALID_SOCKET;
	}

	return newsocket;
}

/*
====================
NET_IP6Socket
====================
*/
int NET_IP6Socket( char *net_interface, int port, struct sockaddr_in6 *bindto, int *err ) {
	SOCKET				newsocket;
	struct sockaddr_in6	address;
	ioctlarg_t			_true = 1;

	*err = 0;

	if( net_interface )
	{
		// Print the name in brackets if there is a colon:
		if(Q_CountChar(net_interface, ':'))
			Com_Printf(CON_CHANNEL_NETWORK,  "Opening IP6 socket: [%s]:%i\n", net_interface, port );
		else
			Com_Printf(CON_CHANNEL_NETWORK,  "Opening IP6 socket: %s:%i\n", net_interface, port );
	}
	else
		Com_Printf(CON_CHANNEL_NETWORK,  "Opening IP6 socket: [::]:%i\n", port );

	if( ( newsocket = socket( PF_INET6, SOCK_DGRAM, IPPROTO_UDP ) ) == INVALID_SOCKET ) {
		*err = socketError;
		Com_Printf(CON_CHANNEL_NETWORK,  "WARNING: NET_IP6Socket: socket: %s\n", NET_ErrorString() );
		return newsocket;
	}

	// make it non-blocking
	if( ioctlsocket( newsocket, FIONBIO, &_true ) == SOCKET_ERROR ) {
		Com_Printf(CON_CHANNEL_NETWORK,  "WARNING: NET_IP6Socket: ioctl FIONBIO: %s\n", NET_ErrorString() );
		*err = socketError;
		closesocket(newsocket);
		return INVALID_SOCKET;
	}

#ifdef IPV6_V6ONLY
	{
		int i = 1;

		// ipv4 addresses should not be allowed to connect via this socket.
		if(setsockopt(newsocket, IPPROTO_IPV6, IPV6_V6ONLY, (char *) &i, sizeof(i)) == SOCKET_ERROR)
		{
			// win32 systems don't seem to support this anyways.
			Com_DPrintf(CON_CHANNEL_NETWORK, "WARNING: NET_IP6Socket: setsockopt IPV6_V6ONLY: %s\n", NET_ErrorString());
		}
	}
#endif

	if( !net_interface || !net_interface[0]) {
		address.sin6_family = AF_INET6;
		address.sin6_addr = in6addr_any;
	}
	else
	{
		if(!Sys_StringToSockaddr( net_interface, (struct sockaddr *)&address, sizeof(address), AF_INET6))
		{
			closesocket(newsocket);
			return INVALID_SOCKET;
		}
	}

	if( port == PORT_ANY ) {
		address.sin6_port = 0;
	}
	else {
		address.sin6_port = htons( (short)port );
	}

	if( bind( newsocket, (void *)&address, sizeof(address) ) == SOCKET_ERROR ) {
		Com_Printf(CON_CHANNEL_NETWORK,  "WARNING: NET_IP6Socket: bind: %s\n", NET_ErrorString() );
		*err = socketError;
		closesocket( newsocket );
		return INVALID_SOCKET;
	}

	if(bindto)
		*bindto = address;

	return newsocket;
}

/*
====================
NET_SetMulticast
Set the current multicast group
====================
*/
void NET_SetMulticast6(void)
{
	struct sockaddr_in6 addr;

	if(!*net_mcast6addr->string || !Sys_StringToSockaddr(net_mcast6addr->string, (struct sockaddr *) &addr, sizeof(addr), AF_INET6))
	{
		Com_Printf(CON_CHANNEL_NETWORK, "WARNING: NET_JoinMulticast6: Incorrect multicast address given, "
			   "please set cvar %s to a sane value.\n", net_mcast6addr->name);

		Cvar_SetInt(net_enabled, net_enabled->integer | NET_DISABLEMCAST);

		return;
	}

	memcpy(&curgroup.ipv6mr_multiaddr, &addr.sin6_addr, sizeof(curgroup.ipv6mr_multiaddr));

#ifdef _WIN32
	if(net_mcast6iface->integer)
#else
	if(*net_mcast6iface->string)
#endif
	{

#ifdef _WIN32
		curgroup.ipv6mr_interface = net_mcast6iface->integer;
#else
		curgroup.ipv6mr_interface = if_nametoindex(net_mcast6iface->string);
#endif
	}
	else
		curgroup.ipv6mr_interface = 0;
}

/*
====================
NET_JoinMulticast
Join an ipv6 multicast group
====================
*/
void NET_JoinMulticast6(void)
{
	int err;

	if(ip6_socket == INVALID_SOCKET || multicast6_socket != INVALID_SOCKET || (net_enabled->integer & NET_DISABLEMCAST))
		return;

	if(IN6_IS_ADDR_MULTICAST(&boundto.sin6_addr) || IN6_IS_ADDR_UNSPECIFIED(&boundto.sin6_addr))
	{
		// The way the socket was bound does not prohibit receiving multi-cast packets. So we don't need to open a new one.
		multicast6_socket = ip6_socket;
	}
	else
	{
		if((multicast6_socket = NET_IP6Socket(net_mcast6addr->string, ntohs(boundto.sin6_port), NULL, &err)) == INVALID_SOCKET)
		{
			// If the OS does not support binding to multicast addresses, like WinXP, at least try with the normal file descriptor.
			multicast6_socket = ip6_socket;
		}
	}

	if(curgroup.ipv6mr_interface)
	{
		if (setsockopt(multicast6_socket, IPPROTO_IPV6, IPV6_MULTICAST_IF,
					(char *) &curgroup.ipv6mr_interface, sizeof(curgroup.ipv6mr_interface)) < 0)
		{
			Com_Printf(CON_CHANNEL_NETWORK, "NET_JoinMulticast6: Couldn't set scope on multicast socket: %s\n", NET_ErrorString());

			if(multicast6_socket != ip6_socket)
			{
				closesocket(multicast6_socket);
				multicast6_socket = INVALID_SOCKET;
				return;
			}
		}
	}

	if (setsockopt(multicast6_socket, IPPROTO_IPV6, IPV6_JOIN_GROUP, (char *) &curgroup, sizeof(curgroup)))
	{
		Com_Printf(CON_CHANNEL_NETWORK, "NET_JoinMulticast6: Couldn't join multicast group: %s\n", NET_ErrorString());

		if(multicast6_socket != ip6_socket)
		{
			closesocket(multicast6_socket);
			multicast6_socket = INVALID_SOCKET;
			return;
		}
	}
}

void NET_LeaveMulticast6()
{
	if(multicast6_socket != INVALID_SOCKET)
	{
		if(multicast6_socket != ip6_socket)
			closesocket(multicast6_socket);
		else
			setsockopt(multicast6_socket, IPPROTO_IPV6, IPV6_LEAVE_GROUP, (char *) &curgroup, sizeof(curgroup));

		multicast6_socket = INVALID_SOCKET;
	}
}

/*
====================
NET_OpenSocks
====================
*/
void NET_OpenSocks( int port ) {
	struct sockaddr_in	address;
	struct hostent		*h;
	int					len;
	qboolean			rfc1929;
	unsigned char		buf[64];

	usingSocks = qfalse;

	Com_Printf(CON_CHANNEL_NETWORK,  "Opening connection to SOCKS server.\n" );

	if ( ( socks_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ) ) == INVALID_SOCKET ) {
		Com_Printf(CON_CHANNEL_NETWORK,  "WARNING: NET_OpenSocks: socket: %s\n", NET_ErrorString() );
		return;
	}

	h = gethostbyname( net_socksServer->string );
	if ( h == NULL ) {
		Com_Printf(CON_CHANNEL_NETWORK,  "WARNING: NET_OpenSocks: gethostbyname: %s\n", NET_ErrorString() );
		return;
	}
	if ( h->h_addrtype != AF_INET ) {
		Com_Printf(CON_CHANNEL_NETWORK,  "WARNING: NET_OpenSocks: gethostbyname: address type was not AF_INET\n" );
		return;
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = *(int *)h->h_addr_list[0];
	address.sin_port = htons( (short)net_socksPort->integer );

	if ( connect( socks_socket, (struct sockaddr *)&address, sizeof( address ) ) == SOCKET_ERROR ) {
		Com_Printf(CON_CHANNEL_NETWORK,  "NET_OpenSocks: connect: %s\n", NET_ErrorString() );
		return;
	}

	// send socks authentication handshake
	if ( *net_socksUsername->string || *net_socksPassword->string ) {
		rfc1929 = qtrue;
	}
	else {
		rfc1929 = qfalse;
	}

	buf[0] = 5;		// SOCKS version
	// method count
	if ( rfc1929 ) {
		buf[1] = 2;
		len = 4;
	}
	else {
		buf[1] = 1;
		len = 3;
	}
	buf[2] = 0;		// method #1 - method id #00: no authentication
	if ( rfc1929 ) {
		buf[2] = 2;		// method #2 - method id #02: username/password
	}
	if ( send( socks_socket, (void *)buf, len, 0 ) == SOCKET_ERROR ) {
		Com_Printf(CON_CHANNEL_NETWORK,  "NET_OpenSocks: send: %s\n", NET_ErrorString() );
		return;
	}

	// get the response
	len = recv( socks_socket, (void *)buf, 64, 0 );
	if ( len == SOCKET_ERROR ) {
		Com_Printf(CON_CHANNEL_NETWORK,  "NET_OpenSocks: recv: %s\n", NET_ErrorString() );
		return;
	}
	if ( len != 2 || buf[0] != 5 ) {
		Com_Printf(CON_CHANNEL_NETWORK,  "NET_OpenSocks: bad response\n" );
		return;
	}
	switch( buf[1] ) {
	case 0:	// no authentication
		break;
	case 2: // username/password authentication
		break;
	default:
		Com_Printf(CON_CHANNEL_NETWORK,  "NET_OpenSocks: request denied\n" );
		return;
	}

	// do username/password authentication if needed
	if ( buf[1] == 2 ) {
		int		ulen;
		int		plen;

		// build the request
		ulen = strlen( net_socksUsername->string );
		plen = strlen( net_socksPassword->string );

		buf[0] = 1;		// username/password authentication version
		buf[1] = ulen;
		if ( ulen ) {
			memcpy( &buf[2], net_socksUsername->string, ulen );
		}
		buf[2 + ulen] = plen;
		if ( plen ) {
			memcpy( &buf[3 + ulen], net_socksPassword->string, plen );
		}

		// send it
		if ( send( socks_socket, (void *)buf, 3 + ulen + plen, 0 ) == SOCKET_ERROR ) {
			Com_Printf(CON_CHANNEL_NETWORK,  "NET_OpenSocks: send: %s\n", NET_ErrorString() );
			return;
		}

		// get the response
		len = recv( socks_socket, (void *)buf, 64, 0 );
		if ( len == SOCKET_ERROR ) {
			Com_Printf(CON_CHANNEL_NETWORK,  "NET_OpenSocks: recv: %s\n", NET_ErrorString() );
			return;
		}
		if ( len != 2 || buf[0] != 1 ) {
			Com_Printf(CON_CHANNEL_NETWORK,  "NET_OpenSocks: bad response\n" );
			return;
		}
		if ( buf[1] != 0 ) {
			Com_Printf(CON_CHANNEL_NETWORK,  "NET_OpenSocks: authentication failed\n" );
			return;
		}
	}

	// send the UDP associate request
	buf[0] = 5;		// SOCKS version
	buf[1] = 3;		// command: UDP associate
	buf[2] = 0;		// reserved
	buf[3] = 1;		// address type: IPV4
	*(int *)&buf[4] = INADDR_ANY;
	*(short *)&buf[8] = htons( (short)port );		// port
	if ( send( socks_socket, (void *)buf, 10, 0 ) == SOCKET_ERROR ) {
		Com_Printf(CON_CHANNEL_NETWORK,  "NET_OpenSocks: send: %s\n", NET_ErrorString() );
		return;
	}

	// get the response
	len = recv( socks_socket, (void *)buf, 64, 0 );
	if( len == SOCKET_ERROR ) {
		Com_Printf(CON_CHANNEL_NETWORK,  "NET_OpenSocks: recv: %s\n", NET_ErrorString() );
		return;
	}
	if( len < 2 || buf[0] != 5 ) {
		Com_Printf(CON_CHANNEL_NETWORK,  "NET_OpenSocks: bad response\n" );
		return;
	}
	// check completion code
	if( buf[1] != 0 ) {
		Com_Printf(CON_CHANNEL_NETWORK,  "NET_OpenSocks: request denied: %i\n", buf[1] );
		return;
	}
	if( buf[3] != 1 ) {
		Com_Printf(CON_CHANNEL_NETWORK,  "NET_OpenSocks: relay address is not IPV4: %i\n", buf[3] );
		return;
	}
	((struct sockaddr_in *)&socksRelayAddr)->sin_family = AF_INET;
	((struct sockaddr_in *)&socksRelayAddr)->sin_addr.s_addr = *(int *)&buf[4];
	((struct sockaddr_in *)&socksRelayAddr)->sin_port = *(short *)&buf[8];
	memset( ((struct sockaddr_in *)&socksRelayAddr)->sin_zero, 0, 8 );

	usingSocks = qtrue;
}


/*
=====================
NET_AddLocalAddress
=====================
*/
static void NET_AddLocalAddress(char *ifname, struct sockaddr *addr, struct sockaddr *netmask)
{
	int addrlen;
	sa_family_t family;

	// only add addresses that have all required info.
	if(!addr || !netmask || !ifname)
		return;

	family = addr->sa_family;

	if(numIP < MAX_IPS)
	{
		if(family == AF_INET)
		{
			addrlen = sizeof(struct sockaddr_in);
			localIP[numIP].type = NA_IP;
		}
		else if(family == AF_INET6)
		{
			addrlen = sizeof(struct sockaddr_in6);
			localIP[numIP].type = NA_IP6;
		}
		else
			return;

		Q_strncpyz(localIP[numIP].ifname, ifname, sizeof(localIP[numIP].ifname));

		localIP[numIP].family = family;

		memcpy(&localIP[numIP].addr, addr, addrlen);
		memcpy(&localIP[numIP].netmask, netmask, addrlen);

		numIP++;
	}
}

#if defined(__linux__) || defined(MACOSX) || defined(__BSD__)
static void NET_GetLocalAddress(void)
{
	struct ifaddrs *ifap, *search;

	numIP = 0;

	if(getifaddrs(&ifap))
		Com_Printf(CON_CHANNEL_NETWORK, "NET_GetLocalAddress: Unable to get list of network interfaces: %s\n", NET_ErrorString());
	else
	{
		for(search = ifap; search; search = search->ifa_next)
		{
			// Only add interfaces that are up.
			if(ifap->ifa_flags & IFF_UP)
				NET_AddLocalAddress(search->ifa_name, search->ifa_addr, search->ifa_netmask);
		}

		freeifaddrs(ifap);

		Sys_ShowIP();
	}
}
#else
static void NET_GetLocalAddress( void ) {
	char				hostname[256];
	struct addrinfo	hint;
	struct addrinfo	*res = NULL;

	numIP = 0;

	if(gethostname( hostname, 256 ) == SOCKET_ERROR)
		return;

	Com_Printf(CON_CHANNEL_NETWORK,  "Hostname: %s\n", hostname );

	memset(&hint, 0, sizeof(hint));

	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_DGRAM;

	if(!getaddrinfo(hostname, NULL, &hint, &res))
	{
		struct sockaddr_in mask4;
		struct sockaddr_in6 mask6;
		struct addrinfo *search;

		/* On operating systems where it's more difficult to find out the configured interfaces, we'll just assume a
		 * netmask with all bits set. */

		memset(&mask4, 0, sizeof(mask4));
		memset(&mask6, 0, sizeof(mask6));
		mask4.sin_family = AF_INET;
		memset(&mask4.sin_addr.s_addr, 0xFF, sizeof(mask4.sin_addr.s_addr));
		mask6.sin6_family = AF_INET6;
		memset(&mask6.sin6_addr, 0xFF, sizeof(mask6.sin6_addr));

		// add all IPs from returned list.
		for(search = res; search; search = search->ai_next)
		{
			if(search->ai_family == AF_INET)
				NET_AddLocalAddress("", search->ai_addr, (struct sockaddr *) &mask4);
			else if(search->ai_family == AF_INET6)
				NET_AddLocalAddress("", search->ai_addr, (struct sockaddr *) &mask6);
		}

		Sys_ShowIP();
	}

	if(res)
		freeaddrinfo(res);
}
#endif

/*
====================
NET_OpenIP
====================
*/
void NET_OpenIP( void ) {
	int		i;
	int		err;
	int		port;
	int		port6;

	port = net_port->integer;
	port6 = net_port6->integer;

	NET_GetLocalAddress();

	Sys_CheckLocalAddressForIPv6();

	// automatically scan for a valid port, so multiple
	// dedicated servers can be started without requiring
	// a different net_port for each one

	if(net_enabled->integer & NET_ENABLEV6)
	{
		for( i = 0 ; i < 10 ; i++ )
		{
			ip6_socket = NET_IP6Socket(net_ip6->string, port6 + i, &boundto, &err);
			if (ip6_socket != INVALID_SOCKET)
			{
				Cvar_SetInt( net_port6, port6 + i );
				break;
			}
			else
			{
				if(err == EAFNOSUPPORT)
					break;
			}
		}
		if(ip6_socket == INVALID_SOCKET)
		{
			Com_Printf(CON_CHANNEL_NETWORK,  "WARNING: Couldn't bind to a v6 ip address.\n");
			ipv6avail = qfalse;
		}
	}else{
		ipv6avail = qfalse;
	}

	if(net_enabled->integer & NET_ENABLEV4)
	{
		for( i = 0 ; i < 10 ; i++ ) {
			ip_socket = NET_IPSocket( net_ip->string, port + i, &err );
			if (ip_socket != INVALID_SOCKET) {
				Cvar_SetInt( net_port, port + i );

				if (net_socksEnabled->integer)
					NET_OpenSocks( port + i );

				break;
			}
			else
			{
				if(err == EAFNOSUPPORT)
					break;
			}
		}

		if(ip_socket == INVALID_SOCKET)
		{
			Com_Printf(CON_CHANNEL_NETWORK,  "WARNING: Couldn't bind to a v4 ip address.\n");
		}

	}
}

void NET_ShutdownQoS()
{
	QoS2_Shutdown();
}

void NET_EnableQoSForHost(netadr_t* host)
{

	if(ip_socket == INVALID_SOCKET || host->type != NA_IP)
	{
		Com_Printf(CON_CHANNEL_NETWORK,  "NET_EnableQoSForHost: No IPv4 socket or remote host.\n");
	}else{
		QoS2_Init(ip_socket, host);
	}
}
//===================================================================


/*
====================
NET_GetCvars
====================
*/
static qboolean NET_GetCvars( void ) {
	int modified;

	if(com_dedicated->integer){
	// I want server owners to explicitly turn on ipv6 support.
		net_enabled = Cvar_RegisterInt( "net_enabled", 1, 0, 8, CVAR_LATCH | CVAR_ARCHIVE, "Enables / Disables Network");
	}else{
		/* End users have it enabled so they can connect to ipv6-only hosts, but ipv4 will be
		* used if available due to ping */
		net_enabled = Cvar_RegisterInt( "net_enabled", 3, 0, 8, CVAR_LATCH | CVAR_ARCHIVE, "Enables / Disables Network");
	}
	modified = net_enabled->modified;
	net_enabled->modified = qfalse;

	net_ip = Cvar_RegisterString( "net_ip", "0.0.0.0", CVAR_LATCH, "Default IPv4 Network Address");
	if(Q_stricmp(net_ip->string, "localhost") == 0)
	{
		Com_Printf(CON_CHANNEL_NETWORK, "If you really attempt to bind to \"localhost\" use for \"net_ip\" 127.0.0.1 instead\n");
		Cvar_SetString(net_ip, "0.0.0.0");
	}
	modified += net_ip->modified;
	net_ip->modified = qfalse;

	net_ip6 = Cvar_RegisterString( "net_ip6", "::", CVAR_LATCH , "Default IPv6 Network Address");
	modified += net_ip6->modified;
	net_ip6->modified = qfalse;

	net_port = Cvar_RegisterInt( "net_port", PORT_SERVER, 0, 65535, CVAR_LATCH, "Network Port Server will listen on");
	modified += net_port->modified;
	net_port->modified = qfalse;

	net_port6 = Cvar_RegisterInt( "net_port6", PORT_SERVER, 0, 65535, CVAR_LATCH, "IPv6 Network Port Server will listen on" );
	modified += net_port6->modified;
	net_port6->modified = qfalse;

	// Some cvars for configuring multicast options which facilitates scanning for servers on local subnets.
	net_mcast6addr = Cvar_RegisterString( "net_mcast6addr", NET_MULTICAST_IP6, CVAR_LATCH | CVAR_ARCHIVE,  "IPv6 Network multicast address");
	modified += net_mcast6addr->modified;
	net_mcast6addr->modified = qfalse;

#ifdef _WIN32
	net_mcast6iface = Cvar_RegisterInt( "net_mcast6iface", 0, 0, 65535, CVAR_LATCH | CVAR_ARCHIVE ,  "IPv6 Network multicast interface");
#else
	net_mcast6iface = Cvar_RegisterString( "net_mcast6iface", "", CVAR_LATCH | CVAR_ARCHIVE ,  "IPv6 Network multicast interface");
#endif
	modified += net_mcast6iface->modified;
	net_mcast6iface->modified = qfalse;

	net_socksEnabled = Cvar_RegisterInt( "net_socksEnabled", 0, 0, 1, CVAR_LATCH | CVAR_ARCHIVE, "Net enable socks proxy support");
	modified += net_socksEnabled->modified;
	net_socksEnabled->modified = qfalse;

	net_socksServer = Cvar_RegisterString( "net_socksServer", "", CVAR_LATCH | CVAR_ARCHIVE, "Net socks proxyserver address");
	modified += net_socksServer->modified;
	net_socksServer->modified = qfalse;

	net_socksPort = Cvar_RegisterInt( "net_socksPort", 1080, 0, 65535, CVAR_LATCH | CVAR_ARCHIVE, "Net socks proxyserver port");
	modified += net_socksPort->modified;
	net_socksPort->modified = qfalse;

	net_socksUsername = Cvar_RegisterString( "net_socksUsername", "", CVAR_LATCH | CVAR_ARCHIVE , "Net socks proxyserver username");
	modified += net_socksUsername->modified;
	net_socksUsername->modified = qfalse;

	net_socksPassword = Cvar_RegisterString( "net_socksPassword", "", CVAR_LATCH | CVAR_ARCHIVE , "Net socks proxyserver password");
	modified += net_socksPassword->modified;
	net_socksPassword->modified = qfalse;

#ifdef ENABLE_FAKELAG
	net_fakelag = Cvar_RegisterInt( "net_fakelag", 0, 0, 1023, 0, "Add artificial network input lag to gameclient for testing purpose. Value is in msec.");
#endif

	return modified ? qtrue : qfalse;
}


/*
====================
NET_Config
====================
*/
void NET_Config( qboolean enableNetworking ) {
	qboolean	modified;
	qboolean	stop;
	qboolean	start;

	// get any latched changes to cvars
	modified = NET_GetCvars();

	if( !net_enabled->integer ) {
		enableNetworking = 0;
	}

	// if enable state is the same and no cvars were modified, we have nothing to do
	if( enableNetworking == networkingEnabled && !modified ) {
		return;
	}

	if( enableNetworking == networkingEnabled ) {
		if( enableNetworking ) {
			stop = qtrue;
			start = qtrue;
		}
		else {
			stop = qfalse;
			start = qfalse;
		}
	}
	else {
		if( enableNetworking ) {
			stop = qfalse;
			start = qtrue;
		}
		else {
			stop = qtrue;
			start = qfalse;
		}
		networkingEnabled = enableNetworking;
	}

	if( stop ) {
		if ( ip_socket != INVALID_SOCKET ) {
			closesocket( ip_socket );
			ip_socket = INVALID_SOCKET;
		}

		if(multicast6_socket)
		{
			if(multicast6_socket != ip6_socket)
				closesocket(multicast6_socket);

			multicast6_socket = INVALID_SOCKET;
		}

		if ( ip6_socket != INVALID_SOCKET ) {
			closesocket( ip6_socket );
			ip6_socket = INVALID_SOCKET;
		}

		if ( socks_socket != INVALID_SOCKET ) {
			closesocket( socks_socket );
			socks_socket = INVALID_SOCKET;
		}

	}

	if( start )
	{
		if (net_enabled->integer)
		{
			NET_OpenIP();
			NET_SetMulticast6();
		}
	}
}


/*
====================
NET_Init
====================
*/
void NET_Init( void ) {
#ifdef _WIN32
	int		r;

	r = WSAStartup( MAKEWORD( 2, 2 ), &winsockdata );
	if( r ) {
		Com_Printf(CON_CHANNEL_NETWORK,  "WARNING: Winsock initialization failed, returned %d\n", r );
		return;
	}

	winsockInitialized = qtrue;
	Com_Printf(CON_CHANNEL_NETWORK,  "Winsock Initialized\n" );
#endif

	NET_Config( qtrue );

	Cmd_AddCommand ("net_restart", NET_Restart_f);
}


/*
====================
NET_Shutdown
====================
*/
void NET_Shutdown( void ) {
	if ( !networkingEnabled ) {
		return;
	}

	NET_Config( qfalse );

#ifdef _WIN32
	WSACleanup();
	winsockInitialized = qfalse;
#endif
}

/*
====================
NET_Sleep

Sleeps msec or until something happens on the network
====================
*/
/*
void NET_Sleep(int msec)
{
	struct timeval timeout;
	fd_set fdr;
	int highestfd = -1, retval;

	if(msec < 0)
		msec = 0;

	FD_ZERO(&fdr);

	if(ip_socket != INVALID_SOCKET)
	{
		FD_SET(ip_socket, &fdr);

		highestfd = ip_socket;
	}
	if(ip6_socket != INVALID_SOCKET)
	{
		FD_SET(ip6_socket, &fdr);

		if(ip6_socket > highestfd)
			highestfd = ip6_socket;
	}

	timeout.tv_sec = msec/1000;
	timeout.tv_usec = (msec%1000)*1000;

#ifdef _WIN32
	if(highestfd < 0)
	{
		// windows ain't happy when select is called without valid FDs
		SleepEx(msec, 0);
		return;
	}
#endif

	retval = select(highestfd + 1, &fdr, NULL, NULL, &timeout);

	if(retval < 0)
		Com_Printf(CON_CHANNEL_NETWORK, "Warning: select() syscall failed: %s\n", NET_ErrorString());
	else if(retval > 0)
		NET_Event(&fdr);
}
*/
/*
====================
NET_Restart_f
====================
*/
void NET_Restart_f( void ) {
	NET_Config( networkingEnabled );
}


/*
===============
NET_TcpCloseSocket

This function should be able to close all types of open TCP sockets
===============
*/

void NET_TcpCloseSocket(int socket)
{
	if(socket == INVALID_SOCKET)
		return;

	//Close the socket
	closesocket(socket);

}



/*
====================
NET_TcpClientConnect
====================
*/
int NET_TcpClientConnectInternal( const char *remoteAdr, int family ) {
	SOCKET			newsocket;
	struct sockaddr_storage	address;
	netadr_t remoteadr;
	int err = 0;
	int retval;
	fd_set fdr;
	struct timeval timeout;
	const char* iptypestr = "";

	Com_Printf(CON_CHANNEL_NETWORK,  "Connecting to: %s\n", remoteAdr);

	netadrtype_t type = NA_UNSPEC;

	if(family == PF_INET)
	{
		iptypestr = "(IPv4)";
		type = NA_IP;
	}
	if(family == PF_INET6)
	{
		iptypestr = "(IPv6)";
		type = NA_IP6;
	}

	if(NET_StringToAdr(remoteAdr, &remoteadr, type))
	{
		Com_Printf(CON_CHANNEL_NETWORK,  "Resolved %s to: %s\n", remoteAdr, NET_AdrToString(&remoteadr));
	}else{
		Com_PrintError(CON_CHANNEL_NETWORK,  "Couldn't resolve: %s %s\n", remoteAdr, iptypestr);
		return INVALID_SOCKET;
	}

	NetadrToSockadr( &remoteadr, (struct sockaddr *)&address);

	if( ( newsocket = socket( family, SOCK_STREAM, IPPROTO_TCP ) ) == INVALID_SOCKET ) {
		Com_PrintError(CON_CHANNEL_NETWORK,  "NET_TCPConnect: socket: %s\n", NET_ErrorString() );
		return INVALID_SOCKET;
	}
	// make it non-blocking
	ioctlarg_t	_true = 1;
	if( ioctlsocket( newsocket, FIONBIO, &_true ) == SOCKET_ERROR ) {
		Com_PrintError(CON_CHANNEL_NETWORK,  "NET_TCPIPSocket: ioctl FIONBIO: %s\n", NET_ErrorString() );
		closesocket(newsocket);
		return INVALID_SOCKET;
	}

	if( connect( newsocket, (void *)&address, sizeof(address) ) == SOCKET_ERROR ) {

		err = socketError;
		if(err == EINPROGRESS
#ifdef _WIN32
			|| err == WSAEWOULDBLOCK
#endif
		){

			FD_ZERO(&fdr);
			FD_SET(newsocket, &fdr);
			timeout.tv_sec = 2;
			timeout.tv_usec = 0;

			retval = select(newsocket +1, NULL, &fdr, NULL, &timeout);

			if(retval < 0){
				Com_PrintError(CON_CHANNEL_NETWORK, "NET_TcpConnect: select() syscall failed: %s\n", NET_ErrorString());
				closesocket( newsocket );
				return INVALID_SOCKET;
			}else if(retval > 0){

				socklen_t so_len = sizeof(err);

				if(getsockopt(newsocket, SOL_SOCKET, SO_ERROR, (char*) &err, &so_len) == 0)
				{
					return newsocket;
				}

			}else{
				Com_PrintError(CON_CHANNEL_NETWORK, "NET_TcpConnect: Connecting to: %s timed out\n", remoteAdr);
				closesocket( newsocket );
				return INVALID_SOCKET;
			}
		}
		Com_PrintError(CON_CHANNEL_NETWORK,  "NET_TCPOpenConnection: connect: %s\n", NET_ErrorString() );
		closesocket( newsocket );
		return INVALID_SOCKET;
	}
	return newsocket;
}


int NET_TcpClientConnect( const char *remoteAdr )
{
	int s = INVALID_SOCKET;
	if(NET_IsIPv6Active())
	{
		s = NET_TcpClientConnectInternal( remoteAdr, PF_INET6 );
	}
	if(s == INVALID_SOCKET)
	{
		s = NET_TcpClientConnectInternal( remoteAdr, PF_INET );
	}
	return s;
}

/*
==================
NET_TcpSendData
Only for Stream sockets (TCP)
Return -1 if an fatal error happened on this socket otherwise 0
==================
*/
int NET_TcpSendData( int sock, const void *data, int length, char* errormsg, int maxerrorlen ) {

	int state, err;

	if(sock < 1)
		return -1;

	state = send( sock, data, length, NET_NOSIGNAL); // FIX: flag NOSIGNAL prevents SIGPIPE in case of connection problems

	if(state == SOCKET_ERROR)
	{
			err = socketError;

			if(err == EAGAIN || err == EINTR)
			{
				return NET_WANT_WRITE;
			}
      if(errormsg)
      {
        Q_strncpyz(errormsg, NET_ErrorString(), maxerrorlen);
      }
      if(err == EPIPE || err == ECONNRESET){
        return NET_CONNRESET;
			}
			return -1;
	}
	return state;
}


/*
====================
NET_TcpClientGetData
returns number of read bytes.
Or returns: NET_WANT_READ -> Call this again soon. Nothing here yet.
Or NET_CONNRESET close the socket. Remote host aborted connection connection. [Writes readable message to errormsg if not NULL]
Or NET_ERROR close the socket. An error happened. [Writes readable message to errormsg if not NULL]
Or 0 close the socket. Action completed in most cases.
====================
*/
int NET_TcpClientGetData(int sock, void* buf, int buflen, char* errormsg, int maxerrorlen)
{

	int err;
	int ret;

	if(sock < 1)
		return -1;

	ret = recv(sock, buf, buflen, 0);

	if(ret == SOCKET_ERROR)
  {
			err = socketError;
			if(err == EAGAIN || err == EINTR)
      {
				return NET_WANT_READ; //Nothing more to read left or interupted system call
			}
      if(errormsg)
      {
        Q_strncpyz(errormsg, NET_ErrorString(), maxerrorlen);
      }
      if(ret == ECONNRESET || ret == EPIPE)
      {
        return NET_CONNRESET;
			}
      return NET_ERROR;
	}
	return ret;
}

#ifdef ENABLE_FAKELAG

struct cachedPacket_s
{
	netadr_t remote;
	int len;
	byte data[1500];
};

struct PacketCacheFrame_s{ 
	int packetcount;
	struct cachedPacket_s packets[];
};

struct PacketCache_s{
	struct PacketCacheFrame_s *cachedPackets[1024]; //index = msec - lagdelay
	int lastread; //
};

struct PacketCache_s packetCache;

void NET_CachePacket(netadr_t *net_from, msg_t *net_message)
{
	int now = Sys_Milliseconds();
	struct PacketCacheFrame_s* newframe = NULL;
	struct PacketCacheFrame_s* frame = packetCache.cachedPackets[now % 1024];
	
	int n = 1;

	if(frame != NULL)
	{	
		n = frame->packetcount + 1;
	}
	
	newframe = (struct PacketCacheFrame_s*)malloc(sizeof(struct PacketCacheFrame_s) + n*sizeof(struct cachedPacket_s));

	if(newframe == NULL)
	{
		Com_Error(ERR_FATAL, "NET_CachePacket(): Out of memory!");
		return;
	}

	newframe->packetcount = 0; //set a valid initial value

	if(frame != NULL)
	{
		memcpy(newframe, frame, sizeof(struct PacketCacheFrame_s) + frame->packetcount* sizeof(struct cachedPacket_s));
		free(frame);
	}

	frame = newframe;

	frame->packets[frame->packetcount].len = net_message->cursize;
	if(frame->packets[frame->packetcount].len > 1500)
	{
		frame->packets[frame->packetcount].len = 1500; //truncate it
	}
	frame->packets[frame->packetcount].remote = *net_from;

	memcpy(&frame->packets[frame->packetcount].data, net_message->data, frame->packets[frame->packetcount].len);

	++frame->packetcount;

	packetCache.cachedPackets[now % 1024] = frame;
}

qboolean NET_GetCachedPacket(netadr_t *net_from, msg_t *net_message)
{
	int latest = Sys_Milliseconds() - net_fakelag->integer;
	int oldtime = packetCache.lastread;

	struct PacketCacheFrame_s* frame = NULL;
	while(oldtime <= latest)
	{
		frame = packetCache.cachedPackets[oldtime % 1024];
		if(frame != NULL)
		{
			break;
		}
		++oldtime;
	}
	
	if(frame == NULL)
	{
		packetCache.lastread = latest;
		return qfalse;
	}

	assert(frame->packetcount > 0);

	MSG_WriteData(net_message, frame->packets[0].data, frame->packets[0].len);
	*net_from = frame->packets[0].remote;

	--frame->packetcount;

	if(frame->packetcount == 0)
	{
		free(frame);
		packetCache.cachedPackets[oldtime % 1024] = NULL;
		packetCache.lastread = oldtime + 1;
	}else{
		memmove(&frame->packets[0], &frame->packets[1], frame->packetcount * sizeof(struct cachedPacket_s));
	}
	return qtrue;
}

qboolean NET_GetPacket(netadr_t *net_from, msg_t *net_message, netadrtype_t type)
{
	msg_t msg;
	netadr_t from;
	byte buffer[1500];

	if(net_fakelag->integer == 0)
	{
		return NET_GetPacketInternal(net_from, net_message, type);
	}

	MSG_Init(&msg, buffer, sizeof(buffer));

	while(NET_GetPacketInternal(&from, &msg, type))
	{
		NET_CachePacket(&from, &msg);
	}

	return NET_GetCachedPacket(net_from, net_message);
}

#else
qboolean NET_GetPacket(netadr_t *net_from, msg_t *net_message, netadrtype_t type)
{
	return NET_GetPacketInternal(net_from, net_message, type);
}
#endif
