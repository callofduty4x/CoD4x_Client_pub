/*
===========================================================================

Return to Castle Wolfenstein multiplayer GPL Source Code
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company. 

This file is part of the Return to Castle Wolfenstein multiplayer GPL Source Code (RTCW MP Source Code).  

RTCW MP Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

RTCW MP Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RTCW MP Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the RTCW MP Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the RTCW MP Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/


/*

packet header
-------------
4	outgoing sequence.  high bit will be set if this is a fragmented message
[2	qport (only for client to server)]
[4	fragment start byte]
[2	fragment length. if < FRAGMENT_SIZE, this is the last fragment]

if the sequence number is -1, the packet should be handled as an out-of-band
message instead of as part of a netchan.

All fragments will have the same sequence numbers.

The qport field is a workaround for bad address translating routers that
sometimes remap the client's source port on a packet during gameplay.

If the base part of the net address matches and the qport matches, then the
channel matches even if the IP port differs.  The IP port should be updated
to the new value before sending out any replies.

*/

#include "q_shared.h"
#include "server.h"
#include "win_sys.h"
#include "qcommon.h"

#include <stdlib.h>

#define MAX_PACKETLEN           1400        // max size of a network packet

#define FRAGMENT_SIZE           ( MAX_PACKETLEN - 100 )
#define PACKET_HEADER           10          // two ints and a short

#define FRAGMENT_BIT    ( 1 << 31 )

cvar_t      *showpackets;
cvar_t      *showdrop;
cvar_t      *qport;
cvar_t		*packetDebug;
cvar_t		*net_profile;
cvar_t		*net_showprofile;
cvar_t		*net_lanauthorize;
cvar_t		*msg_printEntityNums;
cvar_t		*msg_dumpEnts;
cvar_t		*msg_hudelemspew;

static char *netsrcString[2] = {
	"client",
	"server"
};

static void CopyCvar()
{
	*(cvar_t**)0x14bd894 = showpackets;
	*(cvar_t**)0x14e88b8 = showdrop;
	*(cvar_t**)0x14e88b4 = packetDebug;
	*(cvar_t**)0x14c88ac = net_profile;
	*(cvar_t**)0x14e88bc = net_showprofile;
	*(cvar_t**)0x14e88b0 = net_lanauthorize;
	*(cvar_t**)0x14e88c0 = msg_printEntityNums;
	*(cvar_t**)0x14c88a8 = msg_dumpEnts;
	*(cvar_t**)0x14e88c4 = msg_hudelemspew;	
}


/*
===============
Netchan_Init

===============
*/
void Netchan_Init( int port ) {
	port &= 0xffff;
	showpackets = Cvar_RegisterInt( "showpackets", 0, 0, 2, 0, "Show all sent and received packets");
	showdrop = Cvar_RegisterBool( "showdrop", qfalse, 0, "Show dropped packets");
	packetDebug = Cvar_RegisterBool("packetDebug", 0, 0, "Enable packet debugging information");
	net_profile = Cvar_RegisterInt("net_profile", 0, 0, 2, 0, "Profile network performance");
	net_showprofile = Cvar_RegisterInt("net_showprofile", 0, 0, 3, 0, "Show network profiling display");
	net_lanauthorize = Cvar_RegisterBool("net_lanauthorize", 0, 0, "Authorise CD keys when using a LAN");
	msg_printEntityNums = Cvar_RegisterBool("msg_printEntityNums", 0, 0, "Print entity numbers");
	msg_dumpEnts = Cvar_RegisterBool("msg_dumpEnts", 0, 0, "Print snapshot entity info");
	msg_hudelemspew = Cvar_RegisterBool("msg_hudelemspew", 0, 0, "Debug hudelem fields changing");
	qport = Cvar_RegisterInt( "net_qport", port, 0, 65535, CVAR_INIT, "The net_chan qport" );
	signed short *iqport = (signed short*)0xd5ec488;
	*iqport = (signed short)qport->integer;
	//Cmd_AddCommand("net_dumpprofile", 0x507bd0, &_ZZ12Netchan_InitiE21Net_DumpProfile_f_VAR);
	CopyCvar();
}

/*
==============
Netchan_Setup

called to open a channel to a remote system
==============
*/

void Netchan_Setup( netsrc_t sock, netchan_t *chan, netadr_t *adr, int qport , byte* unsentBuffer, int unsentBufferSize, byte* fragmentBuffer, int fragmentBufferSize){

	memset( chan, 0, sizeof( netchan_t ) );

	chan->sock = sock;
	chan->remoteAddress = *adr;
	chan->qport = qport;
	chan->incomingSequence = 0;
	chan->outgoingSequence = 1;
	chan->unsentBuffer = unsentBuffer;
	chan->unsentBufferSize = unsentBufferSize;
	chan->fragmentBuffer = fragmentBuffer;
	chan->fragmentBufferSize = fragmentBufferSize;

}

/*
=================
Netchan_Process

Returns qfalse if the message should not be processed due to being
out of order or a fragment.

Msg must be large enough to hold MAX_MSGLEN, because if this is the
final fragment of a multi-part message, the entire thing will be
copied out.
=================
*/
qboolean Netchan_Process( netchan_t *chan, msg_t *msg ) {
	int sequence;
	int fragmentStart, fragmentLength;
	qboolean fragmented;
	// get sequence numbers
	MSG_BeginReading( msg );
	sequence = MSG_ReadLong( msg );

	// check for fragment information
	if ( sequence & FRAGMENT_BIT ) {
		sequence &= ~FRAGMENT_BIT;
		fragmented = qtrue;
	} else {
		fragmented = qfalse;
	}

	// read the qport if we are a server
	if ( chan->sock == NS_SERVER ) {
		/*qport = */
		MSG_ReadShort( msg );
	}

	// read the fragment information
	if ( fragmented ) {
		fragmentStart = MSG_ReadLong( msg );
		fragmentLength = MSG_ReadShort( msg );

	} else {
		fragmentStart = 0;      // stop warning message
		fragmentLength = 0;
	}

	if ( showpackets->integer ) {
		if ( fragmented ) {
			Com_Printf(CON_CHANNEL_SYSTEM, "%s recv %4i : s=%i fragment=%i,%i\n"
						, netsrcString[ chan->sock ]
						, msg->cursize
						, sequence
						, fragmentStart, fragmentLength );
		} else {
			Com_Printf(CON_CHANNEL_SYSTEM, "%s recv %4i : s=%i\n"
						, netsrcString[ chan->sock ]
						, msg->cursize
						, sequence );
		}
	}

	//
	// discard out of order or duplicated packets
	//
	if ( sequence <= chan->incomingSequence ) {
		if ( showdrop->boolean || showpackets->integer ) {
			Com_Printf(CON_CHANNEL_SYSTEM, "%s:Out of order packet %i at %i\n"
						, NET_AdrToString( &chan->remoteAddress )
						,  sequence
						, chan->incomingSequence );
		}
		return qfalse;
	}

	//
	// dropped packets don't keep the message from being used
	//
	chan->dropped = sequence - ( chan->incomingSequence + 1 );
	if ( chan->dropped > 0 ) {
		if ( showdrop->boolean || showpackets->integer ) {
			Com_Printf(CON_CHANNEL_SYSTEM, "%s:Dropped %i packets at %i\n"
						, NET_AdrToString( &chan->remoteAddress )
						, chan->dropped
						, sequence );
		}
	}


	//
	// if this is the final framgent of a reliable message,
	// bump incoming_reliable_sequence
	//
	if ( fragmented ) {
		// TTimo
		// make sure we add the fragments in correct order
		// either a packet was dropped, or we received this one too soon
		// we don't reconstruct the fragments. we will wait till this fragment gets to us again
		// (NOTE: we could probably try to rebuild by out of order chunks if needed)

		if ( sequence != chan->fragmentSequence ) {
			chan->fragmentSequence = sequence;
			chan->fragmentLength = 0;
		}

		// if we missed a fragment, dump the message
		if ( fragmentStart != chan->fragmentLength ) {
			if ( showdrop->boolean || showpackets->integer ) {
				Com_Printf(CON_CHANNEL_SYSTEM, "%s:Dropped a message fragment %d\n"
							, NET_AdrToString( &chan->remoteAddress )
							, sequence );
			}
			// we can still keep the part that we have so far,
			// so we don't need to clear chan->fragmentLength
			return qfalse;
		}

		// copy the fragment to the fragment buffer
		if ( fragmentLength < 0 || msg->readcount + fragmentLength > msg->cursize 
			|| chan->fragmentLength + fragmentLength > chan->fragmentBufferSize) {

			if ( showdrop->boolean || showpackets->integer ) {
				Com_Printf(CON_CHANNEL_SYSTEM, "%s:illegal fragment length\n"
							, NET_AdrToString( &chan->remoteAddress ) );
			}
			return qfalse;
		}

		memcpy( chan->fragmentBuffer + chan->fragmentLength,	msg->data + msg->readcount, fragmentLength );

		chan->fragmentLength += fragmentLength;

		// if this wasn't the last fragment, don't process anything
		if ( fragmentLength == FRAGMENT_SIZE ) {
			return qfalse;
		}

		if ( chan->fragmentLength > msg->maxsize ) {
			Com_Printf(CON_CHANNEL_SYSTEM, "%s:fragmentLength %i > msg->maxsize\n"
					, NET_AdrToString( &chan->remoteAddress ),
						chan->fragmentLength );
			return qfalse;
		}

		// copy the full message over the partial fragment

		// make sure the sequence number is still there
		*(int *)msg->data = LittleLong( sequence );

		memcpy( msg->data + 4, chan->fragmentBuffer, chan->fragmentLength );
		msg->cursize = chan->fragmentLength + 4;
		chan->fragmentLength = 0;
		msg->readcount = 4; // past the sequence number
		msg->bit = 32;  // past the sequence number


		// TTimo
		// clients were not acking fragmented messages
		chan->incomingSequence = sequence;

		return qtrue;
	}

	//
	// the message can now be read from the current message pointer
	//
	chan->incomingSequence = sequence;

	return qtrue;
}


//==============================================================================

/*
=================
Netchan_TransmitNextFragment

Send one fragment of the current message
=================
*/
qboolean Netchan_TransmitNextFragment( netchan_t *chan ) {
	msg_t send;
	qboolean sendsucc;
	byte send_buf[MAX_PACKETLEN];
	int fragmentLength;
	qboolean var_01 = qfalse;
	// write the packet header
	MSG_Init( &send, send_buf, sizeof( send_buf ) );                // <-- only do the oob here

	MSG_WriteLong( &send, chan->outgoingSequence | FRAGMENT_BIT );

	// send the qport if we are a client
	if ( chan->sock == NS_CLIENT ) {
		MSG_WriteShort( &send, qport->integer );
	}

	// copy the reliable message to the packet first
	fragmentLength = FRAGMENT_SIZE;
	if ( chan->unsentFragmentStart  + fragmentLength > chan->unsentLength ) {
		fragmentLength = chan->unsentLength - chan->unsentFragmentStart;
		var_01 = qtrue;
	}

	MSG_WriteLong(&send, chan->unsentFragmentStart);
	MSG_WriteShort( &send, fragmentLength );
	MSG_WriteData( &send, chan->unsentBuffer + chan->unsentFragmentStart, fragmentLength );

	// send the datagram
	sendsucc = NET_SendPacket( chan->sock, send.cursize, send.data, &chan->remoteAddress );
	if ( showpackets->integer ) {
		Com_Printf(CON_CHANNEL_SYSTEM, "%s send %4i : s=%i fragment=%i,%i\n"
					, netsrcString[ chan->sock ]
					, send.cursize
					, chan->outgoingSequence
					, chan->unsentFragmentStart, fragmentLength );
	}

	chan->unsentFragmentStart += fragmentLength;

	// this exit condition is a little tricky, because a packet
	// that is exactly the fragment length still needs to send
	// a second packet of zero length so that the other side
	// can tell there aren't more to follow
	if ( chan->unsentFragmentStart == chan->unsentLength && var_01 ) {
		chan->outgoingSequence++;
		chan->unsentFragments = qfalse;
	}
	return sendsucc;
}




/*
===============
Netchan_Transmit

Sends a message to a connection, fragmenting if necessary
A 0 length will still generate a packet.
================
*/
qboolean Netchan_Transmit( netchan_t *chan, int length, const byte *data ) {
	msg_t send;
	qboolean sendsucc;
	byte send_buf[MAX_PACKETLEN];

	if ( length > chan->unsentBufferSize ) {
		Com_Error( ERR_DROP, "Netchan_Transmit: length = %i buffer length = %i", length, chan->unsentBufferSize);
	}
	chan->unsentFragmentStart = 0;
	
	// fragment large reliable messages
	if ( length >= FRAGMENT_SIZE ) {
		chan->unsentFragments = qtrue;
		chan->unsentLength = length;
		Com_Memcpy( chan->unsentBuffer, data, length );
		// only send the first fragment now
		Netchan_TransmitNextFragment( chan );
		return qtrue;
	}

	// write the packet header
	MSG_Init( &send, send_buf, sizeof( send_buf ) );

	MSG_WriteLong( &send, chan->outgoingSequence );
	chan->outgoingSequence++;

	// send the qport if we are a client
	if ( chan->sock == NS_CLIENT ) {
		MSG_WriteShort( &send, qport->integer );
	}

	MSG_WriteData( &send, data, length );

	// send the datagram
	sendsucc = NET_SendPacket( chan->sock, send.cursize, send.data, &chan->remoteAddress );
	if ( showpackets->integer ) {
		Com_Printf(CON_CHANNEL_SYSTEM, "%s send %4i : s=%i ack=%i\n"
					, netsrcString[ chan->sock ]
					, send.cursize
					, chan->outgoingSequence - 1
					, chan->incomingSequence );
	}
	return sendsucc;
}




/*
=============================================================================

LOOPBACK BUFFERS FOR LOCAL PLAYER

=============================================================================
*/

// there needs to be enough loopback messages to hold a complete
// gamestate of maximum size
#define	MAX_LOOPBACK	16

typedef struct {
	byte	data[MAX_PACKETLEN];
	int		datalen;
} loopmsg_t;

typedef struct {
	loopmsg_t	msgs[MAX_LOOPBACK];
	int			get, send;
} loopback_t;

loopback_t	loopbacks[2];


qboolean	NET_GetLoopPacket (netsrc_t sock, netadr_t *net_from, msg_t *net_message)
{
	int		i;
	loopback_t	*loop;

	loop = &loopbacks[sock];

	if (loop->send - loop->get > MAX_LOOPBACK)
		loop->get = loop->send - MAX_LOOPBACK;

	if (loop->get >= loop->send)
		return qfalse;

	i = loop->get & (MAX_LOOPBACK-1);
	loop->get++;

	Com_Memcpy (net_message->data, loop->msgs[i].data, loop->msgs[i].datalen);
	net_message->cursize = loop->msgs[i].datalen;
	Com_Memset (net_from, 0, sizeof(*net_from));
	net_from->type = NA_LOOPBACK;
	return qtrue;

}


void NET_SendLoopPacket (netsrc_t sock, int length, const void *data, netadr_t *to)
{
	int		i;
	loopback_t	*loop;

	loop = &loopbacks[sock^1];

	i = loop->send & (MAX_LOOPBACK-1);
	loop->send++;

	Com_Memcpy (loop->msgs[i].data, data, length);
	loop->msgs[i].datalen = length;
}

//=============================================================================

qboolean NET_SendPacket( netsrc_t sock, int length, const void *data, netadr_t *to ) {

	// sequenced packets are shown in netchan, so just show oob
	if ( showpackets->integer && *(int *)data == -1 )	{
		Com_Printf (CON_CHANNEL_SYSTEM, "send packet %4i\n", length);
	}
	if ( to->type == NA_LOOPBACK ) {
		NET_SendLoopPacket (sock, length, data, to);
		return qtrue;
	}
	if ( to->type == NA_BOT ) {
		return qfalse;		//???????????????????????????????????????????????????
	}
	if ( to->type == NA_BAD ) {
		return qfalse;
	}
	return Sys_SendPacket( length, data, to );
}

qboolean NET_SendPacketWrapper( netsrc_t sock, int length, const void *data, legacy_netadr_t *to )
{
	netadr_t adr;
	switch(to->type)
	{
		case LEGACY_NA_LOOPBACK:
			adr.type = NA_LOOPBACK;
			adr.port = to->port;
			return NET_SendPacket( sock, length, data, &adr );
		case LEGACY_NA_BROADCAST:
			adr.type = NA_BROADCAST;
			adr.port = to->port;
			adr.scope_id = 0;
			adr.ip[0] = to->ip[0];
			adr.ip[1] = to->ip[1];
			adr.ip[2] = to->ip[2];
			adr.ip[3] = to->ip[3];
			return NET_SendPacket( sock, length, data, &adr );
		case LEGACY_NA_IP:
			adr.type = NA_IP;
			adr.port = to->port;
			adr.scope_id = 0;
			adr.ip[0] = to->ip[0];
			adr.ip[1] = to->ip[1];
			adr.ip[2] = to->ip[2];
			adr.ip[3] = to->ip[3];
			return NET_SendPacket( sock, length, data, &adr );
		default:
			return qfalse;
	}
}

void SV_PacketEventWrapper(netadr_t *from, msg_t *netmsg)
{
	legacy_netadr_t adr;
	switch(from->type)
	{
		case NA_LOOPBACK:
			adr.type = LEGACY_NA_LOOPBACK;
			adr.port = from->port;
			SV_PacketEvent(adr, netmsg);
			return;
		case NA_BROADCAST:
			adr.type = LEGACY_NA_BROADCAST;
			adr.port = from->port;
			adr.ip[0] = from->ip[0];
			adr.ip[1] = from->ip[1];
			adr.ip[2] = from->ip[2];
			adr.ip[3] = from->ip[3];
			SV_PacketEvent(adr, netmsg);			
			return;
		case NA_IP:
			adr.type = LEGACY_NA_IP;
			adr.port = from->port;
			adr.ip[0] = from->ip[0];
			adr.ip[1] = from->ip[1];
			adr.ip[2] = from->ip[2];
			adr.ip[3] = from->ip[3];
			SV_PacketEvent(adr, netmsg);			
			return;
		default:
			return;
	}	

}
/*
===============
NET_OutOfBandPrint

Sends a text message in an out-of-band datagram
================
*/
void NET_OutOfBandPrint( netsrc_t sock, netadr_t *adr, const char *format, ... )
{
	va_list		argptr;
	char		string[FRAGMENT_SIZE];
	
	// set the header
	string[0] = -1;
	string[1] = -1;
	string[2] = -1;
	string[3] = -1;

	va_start( argptr, format );
	Q_vsnprintf( string+4, sizeof(string)-4, format, argptr );
	va_end( argptr );

	// send the datagram
	NET_SendPacket( sock, strlen( string ), string, adr );
}


/*
===============
NET_OutOfBandData

Sends a data message in an out-of-band datagram (only used for "PbSvSendToClient")
================
*/
void NET_OutOfBandData( netsrc_t sock, netadr_t *adr, byte *format, int len ) {
	byte string[FRAGMENT_SIZE];
	int i;
//	msg_t mbuf;

	// set the header
	string[0] = 0xff;
	string[1] = 0xff;
	string[2] = 0xff;
	string[3] = 0xff;

	if(len +5 >= sizeof(string)){
	    Com_PrintWarning(CON_CHANNEL_SYSTEM, "Buffer Overflow in NET_OutOfBandData %i bytes\n", len);
	    return;
	}

	for ( i = 0; i < len ; i++ ) {
		string[i+4] = format[i];
	}
//	string[i+1] = 0;
	//mbuf.data = string;
	//mbuf.cursize = len + 4;
	//Huff_Compress( &mbuf, 12 );
	// send the datagram
	//NET_SendPacket( sock, mbuf.cursize, mbuf.data, adr );
	NET_SendPacket( sock, i+4, string, adr );
}


/*
=============
NET_StringToAdr

Traps "localhost" for loopback, passes everything else to system
return 0 on address not found, 1 on address found with port, 2 on address found without port.
=============
*/
int NET_StringToAdr( const char *s, netadr_t *a, netadrtype_t family )
{
	char	base[MAX_STRING_CHARS], *search;
	char	*port = NULL;

	if (!strcmp (s, "localhost")) {
		Com_Memset (a, 0, sizeof(*a));
		a->type = NA_LOOPBACK;
// as NA_LOOPBACK doesn't require ports report port was given.
		return 1;
	}

	Q_strncpyz( base, s, sizeof( base ) );
	
	if(*base == '[' || Q_CountChar(base, ':') > 1)
	{
		// This is an ipv6 address, handle it specially.
		search = strchr(base, ']');
		if(search)
		{
			*search = '\0';
			search++;

			if(*search == ':')
				port = search + 1;
		}
		
		if(*base == '[')
			search = base + 1;
		else
			search = base;
	}
	else
	{
		// look for a port number
		port = strchr( base, ':' );
		
		if ( port ) {
			*port = '\0';
			port++;
		}
		
		search = base;
	}

	if(!Sys_StringToAdr(search, a, family))
	{
		a->type = NA_BAD;
		return 0;
	}

	if(port)
	{
		a->port = BigShort((short) atoi(port));
		return 1;
	}
	else
	{
		a->port = BigShort(PORT_SERVER);
		return 2;
	}
}
