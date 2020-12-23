#include "server.h"
#include "sys_patch.h"
#include "qcommon.h"

#include "cg_weapons.h"

void SV_SetMapCenterInSVSHeader(float* center)
{
	svsHeader.mapCenter[0] = center[0];
	svsHeader.mapCenter[1] = center[1];
	svsHeader.mapCenter[2] = center[2];
}

void SV_GetMapCenterFromSVSHeader(float* center)
{
	center[0] = svsHeader.mapCenter[0];
	center[1] = svsHeader.mapCenter[1];
	center[2] = svsHeader.mapCenter[2];
}

void SaveRegisteredItems()
{
	byte bits[MAX_ITEMS / 8];
	char itemstring[MAX_ITEMS];
	int i, bitpos;

	*((int*)0x13EE494) = 0;


	Com_Memset(bits, 0 , sizeof(bits));

	for(i = 0; i < MAX_ITEMS; ++i)
	{
		if(itemRegistered[i])
		{
			bitpos = i & 3;
			if((i & 7) > 3)
			{
				bits[i / 8] |= 1 << bitpos;
			}else{
				bits[i / 8] |= 1 << (bitpos + 4);
			}


		}
	}

	for(i = 0; i < (MAX_ITEMS / 8); i++)
	{
		sprintf(&itemstring[2*i], "%02x", bits[i]);
	}
	itemstring[2*i] = '\0';

//	Com_Printf("ItemString: %s\n", itemstring);

	SV_SetConfigString(2314, itemstring);
}


int SV_GetPredirectedOriginAndTimeForClientNum(int clientNum, float *origin)
{
	client_t* client = &svs.clients[clientNum];
	origin[0] = client->predictedOrigin[0];
	origin[1] = client->predictedOrigin[1];
	origin[2] = client->predictedOrigin[2];
	return client->predictedOriginServerTime;
}


void SV_Patches()
{
	WriteSymbol(0x4bcbfa, itemRegistered);
	WriteSymbol(0x4bcca6, itemRegistered);
	WriteSymbol(0x4c298b, itemRegistered);
	WriteSymbol(0x4e87d4, itemRegistered);
	WriteSymbol(0x4ea41c, itemRegistered);
	WriteSymbol(0x4ea434, itemRegistered);

	SetJump(0x4BCBE0, SaveRegisteredItems);
}


int __regparm1 __MSG_ReadBitsCompress_Server(const byte* input, byte* outputBuf, int readsize)
{
	return MSG_ReadBitsCompress(input, readsize, outputBuf, 0x800);
}
