#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>

#include "game.h"

static void OnLoad()
{
	if (LinkGame())
	{
		Msg("[RAZE] Game linked.\n");
	}
}

static void OnUnload()
{
	UnlinkGame();
}

int WINAPI DllMain(HINSTANCE hInstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)OnLoad, 0, 0, 0);
		break;
		case DLL_PROCESS_DETACH:
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)OnUnload, 0, 0, 0);
		break;
	}

	return 1;
}