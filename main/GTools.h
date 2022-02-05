#ifndef __GTOOLS__
#define __GTOOLS__

#include "GSDK.h"
#include <psapi.h>

class GTools {
public:
	void Log(const char* szText, ...);

	std::wstring asciiDecode(const std::string &s);
	std::string asciiEncode(const std::wstring &w);
	bool IsSteamOverlayEnabled();
	void RestoreSteamOverlayHook();
	void EnterFakeFullscreen(HWND hwnd, bool toggle = true);
	DWORD FindPattern(const DWORD dwAddress, const DWORD dwLen, const BYTE *bMask, const char *szMask);
	DWORD GetModuleAddress(const char* module = 0);
	DWORD GetModuleSize(const char* module = 0);
	DWORD GetModuleSize(const DWORD start_address);
	DWORD GetFunctionAddress(const DWORD module, const char* func);
	void Patch(const DWORD address, const BYTE *patch, const int size);

	float GetFramerate();
private:
};

extern BOOL IsBadToRead(VOID *lp, UINT_PTR ucb);

extern GTools *g_pTools;

#endif