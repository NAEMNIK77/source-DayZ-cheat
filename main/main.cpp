#include "GOverlay.h"
#include "GPlayerManager.h"

sVars cvars;

DWORD WINAPI InitializeOverlayWindow(LPVOID)
{
	g_pPlayerMgr->Init();

	#ifndef USE_D3D9
		g_pOverlay->Initialize();
	#else
		g_pOverlay->InitializeDX9_Hook();
	#endif

	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID)
{
	if (dwReason == DLL_PROCESS_ATTACH) {

		g_pOverlay->SetInstance(hModule);
		DisableThreadLibraryCalls(hModule);

		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)InitializeOverlayWindow, 0, 0, 0);
	}

	return TRUE;
}