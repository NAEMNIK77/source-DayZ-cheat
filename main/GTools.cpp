//#include "GSDK.h"
#include "GDraw.h"
#include "GESP.h"
#include "GMenu.h"
#include "GPlayerManager.h"
#include "GTools.h"
#include "GOverlay.h"

GTools *g_pTools = new GTools;


void GTools::Log(const char* szText, ...)
{
	char szBuffer[1024];
	va_list args;
	va_start(args, szText);
	memset(szBuffer, 0, sizeof(szBuffer));
	_vsnprintf(szBuffer, sizeof(szBuffer)-1, szText, args);
	va_end(args);

	FILE* file = fopen("C:\\dayz_log.txt", "a+");
	fprintf(file, "%s\n", szBuffer);
	fclose(file);
}

std::wstring GTools::asciiDecode(const std::string &s)
{
	std::wostringstream    w;
	wchar_t                c;

	for (size_t i = 0; i < s.length(); i++) {
		mbtowc(&c, &s[i], 1);
		w << c;
	}
	return w.str();
}

std::string GTools::asciiEncode(const std::wstring &w)
{
	std::ostringstream  s;
	char *              c;
	int					r;

	c = new char[MB_CUR_MAX + 1];
	for (size_t i = 0; i < w.length(); i++) {
		r = wctomb(c, w[i]);
		c[r] = '\0';
		if (r <= 1 && c[0] > 0) {
			s << c;
		}
	}
	return s.str();
}

/*
6D550000

6D56CC20    55                   push ebp
6D56CC21    8BEC                 mov ebp, esp
6D56CC23    803D D0555E6D 00     cmp byte ptr ds:[6D5E55D0], 0                   ; ist der overlay aktiviert?
6D56CC2A    56                   push esi
6D56CC2B    8B75 0C              mov esi, dword ptr ss:[ebp+C]
6D56CC2E    74 1E                je short gameover.6D56CC4E
6D56CC30    81FE 00010000        cmp esi, 100
6D56CC36    75 16                jnz short gameover.6D56CC4E
6D56CC38    8B45 10              mov eax, dword ptr ss:[ebp+10]
6D56CC3B    56                   push esi
6D56CC3C    6A 00                push 0
6D56CC3E    50                   push eax
6D56CC3F    E8 DC520300          call gameover.6D5A1F20
6D56CC44    83C4 0C              add esp, 0C
6D56CC47    33C0                 xor eax, eax
6D56CC49    5E                   pop esi
6D56CC4A    5D                   pop ebp
6D56CC4B    C2 0C00              retn 0C
*/

bool GTools::IsSteamOverlayEnabled()
{
	static DWORD dwIsOverlayEnabled = 0;
	if (!dwIsOverlayEnabled) {

		DWORD gameoverlayrenderer = GetModuleAddress(/*GameOverlayRenderer.dll*/XorStr<0x73, 24, 0x670a33d>("\x34\x15\x18\x13\x38\xe\x1c\x8\x17\x1d\x4\x2c\x1a\xee\xe5\xe7\xf1\xe1\xf7\xa8\xe3\xe4\xe5" + 0x670a33d).s);
		if (!gameoverlayrenderer)
			return false;

		dwIsOverlayEnabled = FindPattern(gameoverlayrenderer, GetModuleSize(gameoverlayrenderer), reinterpret_cast<BYTE*>("\x55\x8B\xEC\x80\x3D"), /*xxxxx*/XorStr<0x8e, 6, 0x8a258b5c>("\xf6\xf7\xe8\xe9\xea" + 0x8a258b5c).s);
		if (dwIsOverlayEnabled)
			dwIsOverlayEnabled = *reinterpret_cast<DWORD*>(dwIsOverlayEnabled + 0x05);
		else
			return false;
	}

	return *reinterpret_cast<bool*>(dwIsOverlayEnabled);
}

void GTools::RestoreSteamOverlayHook()
{
#ifndef USE_D3D9
	try {
		DWORD **table = reinterpret_cast<DWORD**>(g_pOverlay->DirectX->Draw->GetSwapChain());
		DWORD present = table[0][8];

		if (present) {

			BYTE *check_jmp = reinterpret_cast<BYTE*>(present);

			if (*check_jmp == 0xE9)
				Patch(present, reinterpret_cast<BYTE*>("\x8B\xFF\x55\x8B\xEC"), 5);
		}
	}
	catch (...) {

	}
#endif
}

void GTools::EnterFakeFullscreen(HWND hwnd, bool toggle)
{
	struct window_t {
		int x, y, w, h;
	} static window;

	if (toggle) {

		LONG style = GetWindowLong(hwnd, GWL_STYLE);

		if (~style & (WS_CAPTION | WS_SIZEBOX))
			return;

		style &= ~WS_CAPTION & ~WS_SIZEBOX;

		RECT rect;
		SystemParametersInfo(SPI_GETWORKAREA, 0, (void*)&rect, 0);
		int w = rect.right - rect.left;
		int h = rect.bottom - rect.top;

		GetWindowRect(hwnd, &rect);

		if (!window.w || !window.h) {
			window.x = rect.left;
			window.y = rect.top;
			window.w = rect.right - rect.left;
			window.h = rect.bottom - rect.top;
		}

		SetWindowLong(hwnd, GWL_STYLE, style);
		MoveWindow(hwnd, 0, 0, w, h, true);
	}
	else {

		LONG style = GetWindowLong(hwnd, GWL_STYLE);

		if (style & (WS_CAPTION | WS_SIZEBOX))
			return;

		style |= WS_CAPTION | WS_SIZEBOX;

		SetWindowLong(hwnd, GWL_STYLE, style);
		MoveWindow(hwnd, window.x, window.y, window.w, window.h, true);
	}
}

bool bDataCompare(const BYTE* pData, const BYTE* bMask, const char* szMask)
{
	for (; *szMask; ++szMask, ++pData, ++bMask)
	if (*szMask == 'x' && *pData != *bMask)
		return false;
	return (*szMask) == NULL;
}

DWORD GTools::FindPattern(const DWORD dwAddress, const DWORD dwLen, const BYTE *bMask, const char *szMask)
{
	for (DWORD i = 0; i < dwLen; i++)
	if (bDataCompare((BYTE*)(dwAddress + i), bMask, szMask))
		return (DWORD)(dwAddress + i);

	return 0;
}

DWORD GTools::GetModuleAddress(const char* module)
{
	return reinterpret_cast<DWORD>(GetModuleHandle(module));
}

DWORD GTools::GetModuleSize(const char* module)
{
	MODULEINFO moduleinfo;
	GetModuleInformation(GetCurrentProcess(), reinterpret_cast<HMODULE>(GetModuleAddress(module)), &moduleinfo, sizeof(moduleinfo));

	return moduleinfo.SizeOfImage;
}

DWORD GTools::GetModuleSize(const DWORD start_address)
{
	MODULEINFO moduleinfo;
	GetModuleInformation(GetCurrentProcess(), reinterpret_cast<HMODULE>(start_address), &moduleinfo, sizeof(moduleinfo));

	return moduleinfo.SizeOfImage;
}

DWORD GTools::GetFunctionAddress(const DWORD module, const char* func)
{
	return reinterpret_cast<DWORD>(GetProcAddress(reinterpret_cast<HMODULE>(module), func));
}

void GTools::Patch(const DWORD address, const BYTE *patch, const int size)
{
	DWORD protect;
	VirtualProtect(reinterpret_cast<void*>(address), size, PAGE_EXECUTE_READWRITE, &protect);

	memcpy(reinterpret_cast<void*>(address), patch, size);
	VirtualProtect(reinterpret_cast<void*>(address), size, protect, 0);
}

BOOL IsBadToRead(VOID *lp, UINT_PTR ucb)
{
	return IsBadReadPtr(lp, ucb);
	//return lp ? false : true;
}

float GTools::GetFramerate()
{
	static float fps = 0;
	static int count = 0;
	static GTimer fps_timer;

	float c = static_cast<float>(fps_timer.get_ms());

	if (c >= 1000) {
		fps = count / (c/1000);
		count = 0;
	}

	if (!count)
		fps_timer.start();

	count++;

	return fps;
}