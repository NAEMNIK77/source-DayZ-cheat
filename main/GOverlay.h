#ifndef __GOVERLAY__
#define __GOVERLAY__

#include "GDirectX.h"

const MARGINS Margin = { 0, 0, 800, 600 };

class GOverlay {
public:
	GOverlay();
	bool Initialize();
	bool InitializeDX9_Hook();
	bool DoMessageLoop();

	void SetInstance(HINSTANCE hinst);
	HINSTANCE GetInstance();
	void SetAppWindow(HWND hwnd);
	void SetOverlayWindow(HWND hwnd);
	HWND GetAppWindow();
	HWND GetOverlayWindow();

	void CorrectOverlayWindow();

	static LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK AppWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	GDirectX *DirectX;
private:
	HINSTANCE instance;
	HWND app_window;
	HWND overlay_window;
	MSG msg;
};

class GInput {
public:
	GInput() {
		memset(&keys, 0, sizeof(keys));
	}
	SHORT IsKeyDown(int key) {
		return GetAsyncKeyState(key);
	}
	bool IsKeyDown2(int key) {
		return keys[key] >> 1;
	}
	/*bool IsKeyUp(int key) {
		return keys[key];
	}*/
	//void OnKeyDown(int key) {
	//	keys[key] = true;
	//}
	//void OnKeyUp(int key) {
	//	keys[key] = false;
	//}
	void Capture() {
		GetKeyboardState(keys);
	}
private:
	BYTE keys[256];
};

extern GOverlay *g_pOverlay;
extern GInput *g_pInput;

#endif