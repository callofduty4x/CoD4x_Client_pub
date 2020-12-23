#include "q_shared.h"
#include "qcommon.h"


#define sv_killserver *(byte*)0x185C410

void SV_KillLocalServer()
{
  if ( com_sv_running->boolean )
    sv_killserver = 1;
}

void SV_Say_f();
void SV_Tell_f();

void SV_AddDedicatedCommands()
{
  Cmd_RemoveCommand("say");
  Cmd_RemoveCommand("tell");
  Cmd_AddCommand("say", Cmd_Stub_f);
  Cmd_AddServerCommand("say", SV_Say_f); 
  Cmd_AddCommand("tell", Cmd_Stub_f);
  Cmd_AddServerCommand("tell", SV_Tell_f);
}