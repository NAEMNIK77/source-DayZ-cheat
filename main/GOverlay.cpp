#include "GOverlay.h"
#include "GDirectX.h"
#include "GTools.h"

GOverlay *g_pOverlay = new GOverlay();
GInput *g_pInput = new GInput();

GOverlay::GOverlay()
{
	instance = 0;
	app_window = 0;
	overlay_window = 0;

	DirectX = new GDirectX();
}

LRESULT CALLBACK GOverlay::WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message) {

	case WM_SIZE:
	case WM_MOVE:
		g_pOverlay->CorrectOverlayWindow();
		g_pOverlay->DirectX->Resize(cvars.Screen.x, cvars.Screen.y);
		break;

	case WM_CREATE:
		DwmExtendFrameIntoClientArea(hWnd, &Margin);
		break;

	case WM_DESTROY:
		PostQuitMessage(1);
		return 0;
	case WM_KEYDOWN:
		//g_pInput->OnKeyDown((int)wParam);
		break;
	case WM_KEYUP:
		//g_pInput->OnKeyUp((int)wParam);
		//MessageBox(0, 0, 0, 0);
		break;
	}

	return DefWindowProc(hWnd, Message, wParam, lParam);
}

WNDPROC window;

#define HIWORDS(l) ((short)((DWORD_PTR)(l)>>16))
LRESULT CALLBACK GOverlay::AppWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_CLOSE) {

		return CallWindowProc(window, hwnd, msg, wParam, lParam);
	}

	if (msg == WM_KEYDOWN) {

		g_pOverlay->DirectX->Menu->KeyHandling(wParam);
		g_pConsole->GetInput()->OnKeyPressDown((int)wParam);
		//g_pInput->OnKeyDown(wParam);
	}

	if (msg == WM_LBUTTONDOWN) {
		g_pOverlay->DirectX->Menu->KeyHandling(VK_LBUTTON);
		//g_pInput->OnKeyDown(VK_LBUTTON);
	}

	if (msg == WM_RBUTTONDOWN) {
		g_pOverlay->DirectX->Menu->KeyHandling(VK_RBUTTON);
		//g_pInput->OnKeyDown(VK_RBUTTON);
	}

	else if (msg == WM_KEYUP) {
			
		//g_pInput->OnKeyUp(wParam);
		g_pConsole->GetInput()->OnKeyPressUp((int)wParam);
	}
	else if (msg == WM_MOUSEWHEEL) {

		if (HIWORDS(wParam) > 0) 
			g_pOverlay->DirectX->Menu->KeyHandling(VK_UP);
		else 
			g_pOverlay->DirectX->Menu->KeyHandling(VK_DOWN);
	}
	//return DefWindowProc(hwnd, msg, wParam, lParam); 

	return CallWindowProc(g_pOverlay->DirectX->Menu->window, hwnd, msg, wParam, lParam);
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM)
{
	DWORD pid = 0;
	char window_title[256] = { 0 };

	if (!hWnd)
		return TRUE;

	GetWindowThreadProcessId(hWnd, &pid);
	if (pid != GetCurrentProcessId())
		return TRUE;

	if (GetParent(hWnd) || !IsWindow(hWnd) || !IsWindowVisible(hWnd))
		return TRUE;

	if (!GetWindowText(hWnd, window_title, 255))
		return TRUE;

	if (strstr(window_title, "DayZ"))
			g_pOverlay->SetAppWindow(hWnd);

	return TRUE;
}

bool GOverlay::Initialize()
{
	while (!GetAppWindow()) {
		
		EnumWindows(EnumWindowsProc, NULL);
		Sleep(100);
	}

	char windowtitle[512] = "";
	GetWindowText(GetAppWindow(), windowtitle, 512);

	WNDCLASSEX wClass;
	wClass.cbClsExtra = NULL;
	wClass.cbSize = sizeof(WNDCLASSEX);
	wClass.cbWndExtra = NULL;
	wClass.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
	wClass.hCursor = LoadCursor(0, IDC_ARROW);
	wClass.hIcon = LoadIcon(0, IDI_APPLICATION);
	wClass.hIconSm = LoadIcon(0, IDI_APPLICATION);
	wClass.hInstance = GetInstance();;
	wClass.lpfnWndProc = WinProc;
	wClass.lpszClassName = windowtitle;
	wClass.lpszMenuName = windowtitle;
	wClass.style = CS_VREDRAW | CS_HREDRAW;

	if (!RegisterClassEx(&wClass))
		return false;

	RECT rect;
	GetWindowRect(GetAppWindow(), &rect);
	POINT pt = { rect.left, rect.top };

	cvars.Screen.x = rect.right - rect.left;
	cvars.Screen.y = rect.bottom - rect.top;

	HWND hwnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_NOACTIVATE,
		wClass.lpszMenuName, wClass.lpszClassName,
		WS_POPUP,
		pt.x, pt.y, cvars.Screen.x, cvars.Screen.y,
		0, 0, wClass.hInstance, 0);

	SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
	SetLayeredWindowAttributes(hwnd, 0, RGB(0, 0, 0), LWA_COLORKEY);

	SetOverlayWindow(hwnd);

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
	EnableWindow(hwnd, false);

	CorrectOverlayWindow();

	//DWORD tid1 = GetWindowThreadProcessId(GetAppWindow(), NULL);
	//DWORD tid2 = GetWindowThreadProcessId(GetOverlayWindow(), NULL);

	//AttachThreadInput(tid2, tid1, true);

	g_pOverlay->DirectX->Menu->window = (WNDPROC)SetWindowLong(GetAppWindow(), GWL_WNDPROC, (LONG)AppWinProc);

	DirectX->Initialize(cvars.Screen.x, cvars.Screen.y, true, hwnd, 1000.0f, 0.1f);

	DoMessageLoop();

	return true;
}

bool GOverlay::InitializeDX9_Hook()
{
	while (!GetAppWindow()) {

		EnumWindows(EnumWindowsProc, NULL);
		Sleep(100);
	}

	while (!DirectX->Draw->GetD3D9Device())
		Sleep(100);

	g_pOverlay->DirectX->Menu->window = (WNDPROC)SetWindowLong(GetAppWindow(), GWL_WNDPROC, (LONG)AppWinProc);

	//41 => BeginScene
	DirectX->EndScene = (EndScene_t)DirectX->DX9_Hook_VTable(41, &GDirectX::hkEndScene);
	//g_pTools->Log("Bla: %X", DirectX->EndScene);

	if (!DirectX->Draw->Initialized()) 
		DirectX->Draw->Init();

	while (msg.message != WM_QUIT) {

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		DirectX->DX9_Hook_VTable(41, &GDirectX::hkEndScene);
		Sleep(100);
	}

	DirectX->Release();
	delete DirectX;

	delete g_pTools;

	return true;
}

bool GOverlay::DoMessageLoop()
{
	while (msg.message != WM_QUIT) {

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		CorrectOverlayWindow();
		DirectX->Render(GetAppWindow());
	}

	DirectX->Release();
	delete DirectX;

	delete g_pTools;

	return true;
}

void GOverlay::CorrectOverlayWindow()
{
	if (!GetAppWindow())
		return;

	RECT rect;
	GetWindowRect(GetAppWindow(), &rect);
	POINT pt = {
		rect.right - rect.left,
		rect.bottom - rect.top
	};

	LONG style = GetWindowLong(GetAppWindow(), GWL_STYLE);

	if (style & (WS_CAPTION | WS_SIZEBOX)) {
		int size_frame_x = GetSystemMetrics(SM_CXSIZEFRAME);
		int size_frame_y = GetSystemMetrics(SM_CYSIZEFRAME);
		int size_caption = GetSystemMetrics(SM_CYCAPTION) + size_frame_y;

		rect.top += size_caption;
		rect.left += size_frame_x;
		pt.x -= size_frame_x * 2;
		pt.y -= size_caption + size_frame_y;
	}

	if (cvars.Screen.x != pt.x || cvars.Screen.y != pt.y) {
		cvars.Screen.x = pt.x;
		cvars.Screen.y = pt.y;
	}

	if (!g_pTools->IsSteamOverlayEnabled()) {
		MoveWindow(GetOverlayWindow(), rect.left, rect.top, cvars.Screen.x, cvars.Screen.y, true);
		//SetActiveWindow(GetOverlayWindow());
	}
}

void GOverlay::SetInstance(HINSTANCE hinst)
{
	instance = hinst;
}

HINSTANCE GOverlay::GetInstance()
{
	return instance;
}

void GOverlay::SetAppWindow(HWND hwnd)
{
	app_window = hwnd;
}

void GOverlay::SetOverlayWindow(HWND hwnd)
{
	overlay_window = hwnd;
}

HWND GOverlay::GetAppWindow()
{
	return app_window;
}

HWND GOverlay::GetOverlayWindow()
{
	return overlay_window;
}