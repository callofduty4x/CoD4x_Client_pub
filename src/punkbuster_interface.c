
#include "q_shared.h"
#include "qcommon.h"
#include "client.h"
#include <windows.h>

#pragma pack(push, 1)

struct pbsv_object_t
{
  int field_0;
  int field_4;
  HMODULE hLibModule;
  int field_C;
  int field_10;
  char messageprefix[32];
  char pbPath[256];
  int field_134;
  int wantdisable;
  void *PbSvGameCommand;
  void *PbSvGameQuery;
  void *PbSvGameMsg;
  void *PbSvSendToClient;		
  void (__cdecl *sb)(struct pbsv_object_t*, int commandtype, int clientNum, int stringlen, const char* cmdstring, int);
  void *sa;
  void *PbSVSendToAddrPort;
  int (__cdecl *dword_D8B2EC0)(struct pbsv_object_t*, int, int);
  int dword_D8B2EC4;
  int CapturConsole;
  int dword_D8B2ECC;
};

struct pbcl_object_t
{
  int field_0;
  int dword_D8B2BC4;
  HMODULE hLibModule;
  int dword_D8B2BCC;
  int wantdisable;
  char gap_14[33];
  char messageprefix[32];
  char pbpath[256];
  char gap_155[35];
  void *PbClGameCommand;
  void *PbClGameQuery;
  void *PbClGameMsg;
  void *PbClSendToServer;
  void (__cdecl *cb)(struct pbcl_object_t *, signed int, int, const CHAR *, int);
  void *ca;
  void *PbClSendToAddrPort;
  int dword_D8B2D54;
  int (__cdecl *PbClEventConnecting)(struct pbcl_object_t *, int, const char*, int*);
  int (__cdecl *dword_D8B2D5C)(struct pbcl_object_t *, int);
  int dword_D8B2D60;
  struct pbsv_object_t *object;
};


#pragma pack(pop)

int PbClLoad();
int PbSvLoad();
int PbClUnload();
int PbSvUnload();
void DisablePbCl();
void EnablePbCl();
qboolean isPbClEnabled();

void Sys_PBSendUdpPacket(char* netadrstring, unsigned short netport, int len, char* data)
{
	netadr_t dest;
	char adrportasstring[256];
	
	Com_sprintf( adrportasstring, sizeof(adrportasstring), "%s:%d", netadrstring, netport);
	if(NET_StringToAdr(adrportasstring, &dest, NA_IP) == 1)
	{
		Sys_SendPacket( len, data, &dest );
	}
}

#define pbcl (*((struct pbcl_object_t*)(0xd8b2bc0)))
#define pbsv (*((struct pbsv_object_t*)(0xd8b2d68)))

void PbClientConnecting(int arg_1, char* string, int* size){

	if(!pbcl.PbClEventConnecting)
		return;
	
	pbcl.PbClEventConnecting(&pbcl, arg_1, string, size);
}

void PbClAddEvent(int commandtype, int stringlen, const char* cmdstring)
{
  if ( pbcl.PbClGameCommand == NULL)
	return;
  
  if ( !pbcl.wantdisable || !pbcl.hLibModule)
  {
    if ( pbcl.hLibModule || !PbClLoad() )
    {
      pbcl.cb(&pbcl, commandtype, stringlen, cmdstring, 0);
    }
  }
  else
	PbClUnload();
}

void PbSvAddEvent(int commandtype, int clientNum ,int stringlen, const char* cmdstring)
{
  if ( pbsv.PbSvGameCommand == NULL)
	return;
  
  if ( !pbsv.wantdisable || !pbsv.hLibModule)
  {
    if ( pbsv.hLibModule || !PbSvLoad() )
    {
	    pbsv.sb(&pbsv, commandtype, clientNum, stringlen, cmdstring, 0);
    }
  }
  else
	PbSvUnload();
}

const char *__cdecl PB_Q_Serveraddr()
{
  if ( !clientUIActives.unk3 )
    return "bot";
  if ( !clientUIActives.state )
  {
    Com_PrintError(CON_CHANNEL_ERROR, "Trying to get server address for PB but connection state is 'disconnected'\n");
    return "bot";
  }

  return NET_AdrToString( &clc.serverAddress );
}



void CL_SendPbPacket(int len, void *data)
{
  if ( clientUIActives.unk3 )
  {
    if ( !clientUIActives.state )
    {
      Com_PrintError(CON_CHANNEL_ERROR, "Trying to send PB Packet but connection state is 'disconnected'\n");
      return;
    }
    if ( clc.serverAddress.type == NA_LOOPBACK )
    {
		PbSvAddEvent(13, 0 , len, data);
		return;
	}
    NET_OutOfBandData(NS_CLIENT, &clc.serverAddress, data, len);
  }
}


void CLUI_SetPbClStatus(qboolean enable)
{
    if ( enable )
    {
		EnablePbCl();
		if ( !isPbClEnabled() )
			Com_SetErrorMessage("MPUI_NOPUNKBUSTER");
			
    }
    else
    {
		DisablePbCl();
    }
}




void __cdecl PBSV_GlobalConstructor()
{
  strcpy(pbsv.messageprefix, "PunkBuster Server");
  pbsv.hLibModule = 0;
  pbsv.PbSvGameCommand = 0;
  pbsv.PbSvGameQuery = 0;
  pbsv.PbSvGameMsg = 0;
  pbsv.PbSvSendToClient = 0;
  pbsv.field_4 = 0;
  pbsv.sb = 0;
  pbsv.sa = 0;
  pbsv.PbSVSendToAddrPort = 0;
  pbsv.dword_D8B2EC0 = 0;
  pbsv.dword_D8B2EC4 = 0;
  pbsv.CapturConsole = 0;
  pbsv.field_0 = 0x357AFE32;
  pbsv.wantdisable = 1;
}

void __cdecl PBCL_GlobalConstructor()
{
  memset(&pbcl, 0, 0x1A8u);
  strcpy(pbcl.messageprefix, "PunkBuster Client");
  pbcl.dword_D8B2BCC = 0;
  pbcl.hLibModule = 0;
  pbcl.PbClGameCommand = 0;
  pbcl.PbClGameQuery = 0;
  pbcl.PbClGameMsg = 0;
  pbcl.PbClSendToServer = 0;
  pbcl.dword_D8B2BC4 = 0;
  pbcl.cb = 0;
  pbcl.ca = 0;
  pbcl.PbClSendToAddrPort = 0;
  pbcl.dword_D8B2D54 = 0;
  pbcl.PbClEventConnecting = 0;
  pbcl.dword_D8B2D5C = 0;
  pbcl.dword_D8B2D60 = 0;
  pbcl.field_0 = 0x264B8BBD;
  pbcl.wantdisable = 1;
}
