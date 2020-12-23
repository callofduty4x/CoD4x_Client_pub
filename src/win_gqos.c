
#include "qcommon.h"

#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0600

#include <winsock2.h>
#include <Ws2tcpip.h>
#include "Qos2.h"


#define	QOSCLOSEHANDLE(aname) WINBOOL WINAPI (*aname)(HANDLE QOSHandle)
#define QOSCREATEHANDLE(aname) WINBOOL WINAPI (*aname)(PQOS_VERSION Version, PHANDLE QOSHandle)
#define QOSADDSOCKETTOFLOW(aname) WINBOOL WINAPI (*aname)(HANDLE QOSHandle, SOCKET Socket, PSOCKADDR DestAddr, QOS_TRAFFIC_TYPE TrafficType, DWORD Flags, QOS_FLOWID *FlowId)
#define QOSREMOVESOCKETFROMFLOW(aname) WINBOOL WINAPI (*aname)(HANDLE QOSHandle, SOCKET Socket, QOS_FLOWID FlowId, DWORD Flags)

HMODULE qWAVELib;
HANDLE QoSHandle;
QOS_FLOWID QoSFlowId;

QOSCLOSEHANDLE(ZQOSCloseHandle);
QOSCREATEHANDLE(ZQOSCreateHandle);
QOSADDSOCKETTOFLOW(ZQOSAddSocketToFlow);
QOSREMOVESOCKETFROMFLOW(ZQOSRemoveSocketFromFlow);

void NetadrToSockadr( netadr_t *a, struct sockaddr *s );

void QoS2_CloseHandle()
{
	if(QoSHandle)
	{
		ZQOSCloseHandle(QoSHandle);
	}
	QoSHandle = NULL;
	QoSFlowId = 0;
}

BOOL QoS2_ImportDLL()
{
	if(qWAVELib != NULL)
	{
		return TRUE;
	}
	
	qWAVELib = LoadSystemLibraryA("qwave.dll");
	if(qWAVELib == NULL)
	{
		Com_Printf(CON_CHANNEL_SYSTEM, "LoadLibraryA() qwave.dll is not available on this platform\n");
		return FALSE;
	}
	
	ZQOSCloseHandle = GetProcAddress(qWAVELib, "QOSCloseHandle");
	ZQOSCreateHandle = GetProcAddress(qWAVELib, "QOSCreateHandle");	
	ZQOSAddSocketToFlow = GetProcAddress(qWAVELib, "QOSAddSocketToFlow");
	ZQOSRemoveSocketFromFlow = GetProcAddress(qWAVELib, "QOSRemoveSocketFromFlow");
	
	if( ZQOSCloseHandle && ZQOSCreateHandle && ZQOSAddSocketToFlow && ZQOSRemoveSocketFromFlow )
	{
		return TRUE;
	}
	Com_Printf(CON_CHANNEL_SYSTEM, "GetProcAddress() qwave.dll is not available on this platform\n");
	FreeLibrary(qWAVELib);
	return FALSE;
}


void QoS2_Init(int socket, netadr_t* remote)
{
	QOS_VERSION Version;
	DWORD QoSResult = FALSE;
	struct sockaddr_storage sadr;
	
	char displayMessageBuf[1024];
	
	if(QoS2_ImportDLL() == FALSE){
		return;
	}
	
	NetadrToSockadr( remote, (struct sockaddr *) &sadr );

	
	QoS2_CloseHandle();

	// Initialize the QoS version parameter.
	Version.MajorVersion = 1;
	Version.MinorVersion = 0;

	// Get a handle to the QoS subsystem.
	QoSResult = ZQOSCreateHandle(&Version, &QoSHandle );

	if (QoSResult != TRUE)
	{
		FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
						0, WSAGetLastError(), 0x400, displayMessageBuf, sizeof(displayMessageBuf), 0);

		Com_Printf(CON_CHANNEL_SYSTEM, "QOSCreateHandle failed. Error: %s\n", displayMessageBuf);
		return;
	}
	
	QOS_TRAFFIC_TYPE QoSOutgoingTrafficType = QOSTrafficTypeControl;
	
	/*
	QoSResult = QOSSetFlow( QoSHandle, FlowId, QOSSetTrafficType, sizeof(QoSOutgoingTrafficType), &QoSOutgoingTrafficType, 0, NULL);
	if(QoSResult == 0 )
	{
		Com_Printf("QOSSetFlow failed. Error: %d\n", WSAGetLastError());
		return;
	}
	*/

	QoSResult = ZQOSAddSocketToFlow(QoSHandle, socket, (struct sockaddr *) &sadr, QoSOutgoingTrafficType, QOS_NON_ADAPTIVE_FLOW, &QoSFlowId);
	if (QoSResult != TRUE)
	{
		
		FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
						0, WSAGetLastError(), 0x400, displayMessageBuf, sizeof(displayMessageBuf), 0);

		
		Com_Printf(CON_CHANNEL_SYSTEM, "QOSAddSocketToFlow failed to add a flow. Error: %s\n", displayMessageBuf);
		return;
	}
	
	
	
}



void QoS2_Shutdown()
{
	QoS2_CloseHandle( );
}