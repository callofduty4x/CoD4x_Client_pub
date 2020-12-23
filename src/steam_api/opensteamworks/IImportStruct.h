#ifndef __IIMPORTSTRUCT_H__
#define __IIMPORTSTRUCT_H__

#include "../isteamclient.h"

typedef struct
{
	const char* name;
	int index;
}safeimportaccess_t;


struct InterfaceDesc_t
{
	void* vtable;
	const char* classname;
	HSteamUser hSteamUser;
	HSteamPipe hSteamPipe;
};


typedef uint32_t** classptr_t;

#define CPPCALL __attribute__((thiscall))

#endif