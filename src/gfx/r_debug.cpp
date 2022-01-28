#include "win_debugcon.h"
#include "r_debug.h"

RDebugConsole *dbgCon = nullptr;

RDebugConsole& R_CreateDebugConsole()
{
    HINSTANCE h = GetModuleHandle(NULL);

    static RDebugConsole dbgcon(h, "D3D Debug Console", 800, 600);
    dbgcon.Show(1, true);
    dbgcon.Print("Test message\n");
    return dbgcon;
}



extern "C" void R_InitDebugSystems()
{
   dbgCon = &R_CreateDebugConsole();

}




