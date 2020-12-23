
#include "q_shared.h"
#include "qcommon.h"
#include "client.h"



/*
==============
CL_Netchan_Decode

==============
*/
void CL_Netchan_Decode_Default(byte *data, int length)
{

	int i, index;
	byte key, *string;

	string = (byte *)clc.reliableCommands[ clc.reliableAcknowledge & ( MAX_RELIABLE_COMMANDS - 1 ) ].command;
	 
	key = clc.challenge ^ clc.serverMessageSequence;

	for ( i=0, index=0; i < length; i++ )
	{
	 
		if ( !string[index] )
		{
			index = 0;
		}
		 
		// modify the key with the last sent and acknowledged server command
		key ^= string[index] << ( i & 1 );
		data[i] ^= key;
		 
		index++;
	}

}

/*
==============
CL_Netchan_Encode

==============
*/

void CL_Netchan_Encode_Default( byte *data, int cursize ) {

	int i, index;
	byte key, *string, serverid;
	serverid = cl.serverId;
	string = (byte *)clc.serverCommands[clc.serverCommandSequence & ( MAX_RELIABLE_COMMANDS - 1 )];
	key = serverid ^ clc.serverMessageSequence ^ clc.challenge;
	index = 0;

	for ( i = 0; i < cursize; i++ ) {

		if ( !string[index] ) {
			index = 0;
		}

		// modify the key with the last sent and acknowledged server command
		key ^= string[index] << ( i & 1 );
		data[i] ^= key;

		index++;
	}

}


void (*CL_Netchan_Encode)(byte *data, int cursize) = CL_Netchan_Encode_Default;
void (*CL_Netchan_Decode)(byte *data, int length) = CL_Netchan_Decode_Default;

/*
==============
CL_Netchan_Transmit

==============
*/

qboolean CL_Netchan_Transmit( netchan_t *chan, int outlen, byte* outdata )
{
	int encryptionstart;
	
	if(Com_IsLegacyServer())
	{
		encryptionstart = 9;
	}else{
		encryptionstart = 16;
	}
	CL_Netchan_Encode(outdata + encryptionstart, outlen - encryptionstart);
	return Netchan_Transmit( chan, outlen, outdata );
}

/*
==============
CL_Netchan_TransmitNextFragment

==============
*/

qboolean CL_Netchan_TransmitNextFragment( netchan_t *chan )
{
	return Netchan_TransmitNextFragment( chan );
}