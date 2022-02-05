#include "GDirectX.h"
#include "GDraw.h"
#include "GPlayerManager.h"
#include "GESP.h"
#include "GMenu.h"
#include "GAimbot.h"
#include "GOverlay.h"
#include "GConsole.h"

GDirectX::GDirectX()
{
	swapchain = 0;
	device = 0;
	device_context = 0;
	renderTargetView = 0;
	depthStencilBuffer = 0;
	depthStencilState = 0;
	depthStencilView = 0;
	rasterState = 0;
	depthDisabledStencilState = 0;
	alphaEnableBlendingState = 0;
	alphaDisableBlendingState = 0;

	DrawIndexedPrimitive = NULL;
	SetStreamSource = NULL;
	Present = NULL;
	EndScene = NULL;
	Reset = NULL;

	#ifndef USE_D3D9
		Draw = new GDraw();
	#else
		Draw = new CDX9Renderer();
	#endif

	Menu = new GMenu(/*Haxx*/XorStr<0x4, 5, 0x09b379e>("\x4c\x64\x7e\x7f" + 0x09b379e).s, 100, 40, 135, false, true);
}

bool GDirectX::Initialize(int w, int h, bool vsync, HWND hwnd, float depth, float near_)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator;
	DXGI_MODE_DESC* displayModeList;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;
	float fieldOfView, screenAspect;
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	D3D11_BLEND_DESC blendStateDescription;

	// Store the vsync setting.
	vsync_enabled = vsync;

	// Create a DirectX graphics interface factory.
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
		return false;

	// Use the factory to create an adapter for the primary graphics interface (video card).
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
		return false;

	// Enumerate the primary adapter output (monitor).
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
		return false;

	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
		return false;

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
		return false;

	// Now fill the display mode list structures.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
		return false;

	// Now go through all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	for (i = 0; i<numModes; i++) {
		if (displayModeList[i].Width == (unsigned int)w) {

			if (displayModeList[i].Height == (unsigned int)h) {

				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// Release the display mode list.
	delete[] displayModeList;
	displayModeList = 0;

	// Release the adapter output.
	adapterOutput->Release();
	adapterOutput = 0;

	// Release the adapter.
	adapter->Release();
	adapter = 0;

	// Release the factory.
	factory->Release();
	factory = 0;

	// Initialize the swap chain description.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// Set to a single back buffer.
	swapChainDesc.BufferCount = 1;

	// Set the width and height of the back buffer.
	swapChainDesc.BufferDesc.Width = w;
	swapChainDesc.BufferDesc.Height = h;

	// Set regular 32-bit surface for the back buffer.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Set the refresh rate of the back buffer.
	if (vsync_enabled) {

		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else {

		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Set the usage of the back buffer.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the handle for the window to render to.
	swapChainDesc.OutputWindow = hwnd;

	// Turn multisampling off.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// Set to full screen or windowed mode.
	swapChainDesc.Windowed = true;

	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after presenting.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	swapChainDesc.Flags = 0;

	// Set the feature level to DirectX 11.
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// Create the swap chain, Direct3D device, and Direct3D device context.
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &swapchain, &device, NULL, &device_context);
	if (FAILED(result))
		return false;

	// Get the pointer to the back buffer.
	result = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
		return false;

	// Create the render target view with the back buffer pointer.
	result = device->CreateRenderTargetView(backBufferPtr, NULL, &renderTargetView);
	if (FAILED(result))
		return false;

	// Release pointer to the back buffer as we no longer need it.
	backBufferPtr->Release();
	backBufferPtr = 0;

	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = w;
	depthBufferDesc.Height = h;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	result = device->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilBuffer);
	if (FAILED(result))
		return false;

	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the depth stencil state.
	result = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
	if (FAILED(result))
		return false;

	// Set the depth stencil state.
	device_context->OMSetDepthStencilState(depthStencilState, 1);

	// Initialize the depth stencil view.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	result = device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView);
	if (FAILED(result))
		return false;

	// Bind the render target view and depth stencil buffer to the output render pipeline.
	device_context->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	result = device->CreateRasterizerState(&rasterDesc, &rasterState);
	if (FAILED(result))
		return false;

	// Now set the rasterizer state.
	device_context->RSSetState(rasterState);

	// Setup the viewport for rendering.
	viewport.Width = (float)w;
	viewport.Height = (float)h;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// Create the viewport.
	device_context->RSSetViewports(1, &viewport);

	// Setup the projection matrix.
	fieldOfView = (float)D3DX_PI / 4.0f;
	screenAspect = (float)w / (float)h;

	// Create the projection matrix for 3D rendering.
	D3DXMatrixPerspectiveFovLH(&projection_matrix, fieldOfView, screenAspect, near_, depth);

	// Initialize the world matrix to the identity matrix.
	D3DXMatrixIdentity(&world_matrix);

	// Create an orthographic projection matrix for 2D rendering.
	D3DXMatrixOrthoLH(&ortho_matrix, (float)w, (float)h, near_, depth);

	// Clear the second depth stencil state before setting the parameters.
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	// Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
	// that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the state using the device.
	result = device->CreateDepthStencilState(&depthDisabledStencilDesc, &depthDisabledStencilState);
	if (FAILED(result))
		return false;

	// Clear the blend state description.
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	// Create an alpha enabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// Create the blend state using the description.
	result = device->CreateBlendState(&blendStateDescription, &alphaEnableBlendingState);
	if (FAILED(result))
		return false;

	// Modify the description to create an alpha disabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;

	// Create the blend state using the description.
	result = device->CreateBlendState(&blendStateDescription, &alphaDisableBlendingState);
	if (FAILED(result))
		return false;

	if (!Draw->Initialized()) {
		Draw->Init(device, device_context, swapchain);
		Draw->InitFont();
	}

	initialized = true;

	return initialized;
}

void GDirectX::Resize(int w, int h)
{
	initialized = false;

	device_context->OMSetRenderTargets(0, 0, 0);
	device_context->ClearState();

	// Release all outstanding references to the swap chain's buffers.
	renderTargetView->Release();

	HRESULT hr;
	hr = swapchain->ResizeBuffers(2, w, h, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

	// Get buffer and create a render-target-view.
	ID3D11Texture2D* pBuffer;
	hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer);

	hr = device->CreateRenderTargetView(pBuffer, NULL, &renderTargetView);
	pBuffer->Release();

	device_context->OMSetRenderTargets(1, &renderTargetView, NULL);

	// Set up the viewport.
	D3D11_VIEWPORT vp;
	vp.Width = (float)w;
	vp.Height = (float)h;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	device_context->RSSetViewports(1, &vp);

	initialized = true;
}

void GDirectX::Release()
{
	if (alphaEnableBlendingState)
		alphaEnableBlendingState->Release();

	if (alphaDisableBlendingState)
		alphaDisableBlendingState->Release();

	if (rasterState)
		rasterState->Release();

	if (depthStencilView)
		depthStencilView->Release();

	if (depthDisabledStencilState)
		depthDisabledStencilState->Release();

	if (depthStencilState)
		depthStencilState->Release();

	if (depthStencilBuffer)
		depthStencilBuffer->Release();

	if (renderTargetView)
		renderTargetView->Release();

	if (device_context)
		device_context->Release();

	if (device)
		device->Release();

	if (swapchain)
		swapchain->Release();

	swapchain = 0;
	device = 0;
	device_context = 0;
	renderTargetView = 0;
	depthStencilBuffer = 0;
	depthStencilState = 0;
	depthStencilView = 0;
	rasterState = 0;
	depthDisabledStencilState = 0;
	alphaEnableBlendingState = 0;
	alphaDisableBlendingState = 0;

	initialized = false;

	Draw->Release();
	delete Draw;
}

void GDirectX::Render(HWND hwnd)
{
	if (!initialized)
		return;

	float color[4] = { 0 };
	float blendFactor[4] = { 0 };

	device_context->ClearRenderTargetView(renderTargetView, color);
	device_context->OMSetDepthStencilState(depthDisabledStencilState, 1);
	device_context->OMSetBlendState(alphaEnableBlendingState, blendFactor, 0xffffffff);

#ifndef USE_D3D9
	g_pTools->RestoreSteamOverlayHook();
#endif

	if (hwnd == GetForegroundWindow()) {

		if (!g_pTools->IsSteamOverlayEnabled()) {
			/*try {
				RenderLoop();
			}
			catch(...) {

			}*/
			RenderLoop();
		}
	}

	device_context->OMSetBlendState(alphaDisableBlendingState, blendFactor, 0xffffffff);
	device_context->OMSetDepthStencilState(depthStencilState, 1);

	if (vsync_enabled)
		swapchain->Present(1, 0); //Present1 --> better performance
	else
		swapchain->Present(0, 0);
}


const char *byte_to_binary(int x)
{
	static char b[17];
	b[0] = '\0';
	memset(b, 0, 16);

	int z;
	for (z = 128; z > 0; z >>= 1)
	{
		strcat(b, ((x & z) == z) ? "1" : "0");
	}

	return b;
}

extern char *GetState(CObject *obj);
extern char *GetWeaponState(CObject *obj);
/*
Standing:
normal	176		0010110000
vorne	303		0100101111
hinten	311		0100110111
links	309		0100110101
rechts	313		0100111001
rennen	451		0111000011
jump	116		0001110100

Kneeing:
normal	854		1101010110
vorne	351		0101011111
hinten	366		0101101110
links	364		0101101100
rechts	368		0101110000
jump	182		0010110110

Standing shift:
normal	176		0010110000
vorne	295		0100100111
hinten	299		0100101011
links	297		0100101001
rechts	301		0100101101
rennen	451		0111000011
jump	116		0001110100

prone
normal	228		0011100100
links	412		0110011100

links	309		0100110101
links	364		0101101100
links	412		0110011100
*/

void Console_CoderInfo(GConsole *con)
{
	con->Print(/*Console coded by Gordon` on 19.06.2014*/XorStr<0xd5, 39, 0x3384a0>("\x96\xb9\xb9\xab\xb6\xb6\xbe\xfc\xbe\xb1\xbb\x85\x85\xc2\x81\x9d\xc5\xa1\x88\x9a\x8d\x85\x85\x8c\xcd\x81\x81\xd0\xc0\xcb\xdd\xc4\xc3\xd8\xc5\xc8\xc8\xce" + 0x3384a0).s);
}

void Console_PrintLocalPlayerAddress(GConsole *con)
{
	CObject *obj = g_pPlayerMgr->GetLocalPlayer();
	con->Print(/*Localplayer Object: %08X*/XorStr<0x93, 25, 0x90f792c7>("\xdf\xfb\xf6\xf7\xfb\xe8\xf5\xfb\xe2\xf9\xef\xbe\xd0\xc2\xcb\xc7\xc0\xd0\x9f\x86\x82\x98\x91\xf2" + 0x90f792c7).s, obj);

	if (obj->weapon && obj->weapon->object) {

	}
}

void Console_Toggle(GConsole *con)
{
	//cvars.toggle_steal = !cvars.toggle_steal;

	//con->Print("bool toggled. now %d", (int)cvars.toggle_steal);
	//con->Print("Params: %s", cmd);
	//g_pPlayerMgr->ShowHint(const_cast<char*>(cmd));

	GConsole::console_cmd_args *buf = con->GetCmdArgs();

	for (int i = 0; i < buf->argc; i++) {
		con->PrintC("Argument ^3 %d^9: ^5%s", i, buf->argv[i].c_str());
	}
}

void Console_LogAddresses(GConsole *con)
{
	g_pTools->Log(/*Base: %08X*/XorStr<0xfc, 11, 0xfa60fb62>("\xbe\x9c\x8d\x9a\x3a\x21\x27\x33\x3c\x5d" + 0xfa60fb62).s, g_pPlayerMgr->GetBase());
	g_pTools->Log(/*NetworkMgr: %08X*/XorStr<0xb4, 17, 0xe218b319>("\xfa\xd0\xc2\xc0\xd7\xcb\xd1\xf6\xdb\xcf\x84\x9f\xe5\xf1\xfa\x9b" + 0xe218b319).s, g_pPlayerMgr->GetNetworkMgr());
	g_pTools->Log(/*Transformations: %08X*/XorStr<0x17, 22, 0xe249e37b>("\x43\x6a\x78\x74\x68\x7a\x72\x6c\x72\x41\x55\x4b\x4c\x4a\x56\x1c\x7\xd\x19\x12\x73" + 0xe249e37b).s, g_pPlayerMgr->transform);
	g_pTools->Log(/*Worldtime: %08X*/XorStr<0xd2, 16, 0x36d1373>("\x85\xbc\xa6\xb9\xb2\xa3\xb1\xb4\xbf\xe1\xfc\xf8\xee\xe7\xb8" + 0x36d1373).s, g_pPlayerMgr->GetTime());
	g_pTools->Log(/*D3D9 Device + Offset: %08X + %08X*/XorStr<0x3a, 34, 0x1ce694>("\x7e\x8\x78\x4\x1e\x7b\x25\x37\x2b\x20\x21\x65\x6d\x67\x7\x2f\x2c\x38\x29\x39\x74\x6f\x75\x61\x6a\xb\x74\x7e\x76\x72\x68\x61\x2" + 0x1ce694).s, g_pOverlay->DirectX->Draw->dwD3D9Address, g_pOverlay->DirectX->Draw->dwD3D9Offset);

	con->Print(color_red, /*content written to file.*/XorStr<0x9f, 25, 0x6619d4>("\xfc\xcf\xcf\xd6\xc6\xca\xd1\x86\xd0\xda\xc0\xde\xdf\xc9\xc3\x8e\xdb\xdf\x91\xd4\xda\xd8\xd0\x98" + 0x6619d4).s);
}


void GDirectX::RenderLoop()
{
	static bool init_d3d9_hook = false;
	static bool init = false;
	if (!init) {
		//Console->Render();
		g_pConsole->RegisterCommand(/*coderinfo*/XorStr<0xa, 10, 0x6a13da4>("\x69\x64\x68\x68\x7c\x66\x7e\x77\x7d" + 0x6a13da4).s, &Console_CoderInfo);
		g_pConsole->RegisterCommand(/*localplayer*/XorStr<0xf2, 12, 0xee20bb21>("\x9e\x9c\x97\x94\x9a\x87\x94\x98\x83\x9e\x8e" + 0xee20bb21).s, &Console_PrintLocalPlayerAddress);
		g_pConsole->RegisterCommand(/*logaddresses*/XorStr<0x40, 13, 0xa23ed571>("\x2c\x2e\x25\x22\x20\x21\x34\x22\x3b\x3a\x2f\x38" + 0xa23ed571).s, &Console_LogAddresses);
		g_pConsole->RegisterCommand(/*hint*/XorStr<0xfb, 5, 0xc35e2b60>("\x93\x95\x93\x8a" + 0xc35e2b60).s, &Console_Toggle);

		g_pConsole->Print(color_green, /*:: Console initialized © 2014, Gordon` ::*/XorStr<0x5c, 42, 0x8b58288f>("\x66\x67\x7e\x1c\xf\xf\x11\xc\x8\x0\x46\xe\x6\x0\x1e\x2\xd\x1\x7\x15\x15\x15\x52\xda\x54\x47\x46\x46\x4c\x55\x5a\x3c\x13\xf\x1a\x10\xee\xe1\xa2\xb9\xbe" + 0x8b58288f).s);
		g_pConsole->Print("\n");
		g_pConsole->Print(/*Base: %08X*/XorStr<0xfc, 11, 0xfa60fb62>("\xbe\x9c\x8d\x9a\x3a\x21\x27\x33\x3c\x5d" + 0xfa60fb62).s, g_pPlayerMgr->GetBase());
		g_pConsole->Print(/*NetworkMgr: %08X*/XorStr<0xb4, 17, 0xe218b319>("\xfa\xd0\xc2\xc0\xd7\xcb\xd1\xf6\xdb\xcf\x84\x9f\xe5\xf1\xfa\x9b" + 0xe218b319).s, g_pPlayerMgr->GetNetworkMgr());
		g_pConsole->Print(/*Transformations: %08X*/XorStr<0x17, 22, 0xe249e37b>("\x43\x6a\x78\x74\x68\x7a\x72\x6c\x72\x41\x55\x4b\x4c\x4a\x56\x1c\x7\xd\x19\x12\x73" + 0xe249e37b).s, g_pPlayerMgr->transform);
		g_pConsole->Print(/*Worldtime: %08X*/XorStr<0xd2, 16, 0x36d1373>("\x85\xbc\xa6\xb9\xb2\xa3\xb1\xb4\xbf\xe1\xfc\xf8\xee\xe7\xb8" + 0x36d1373).s, g_pPlayerMgr->GetTime());
		g_pConsole->Print(/*D3D9 Device + Offset: %08X + %08X*/XorStr<0x3a, 34, 0x1ce694>("\x7e\x8\x78\x4\x1e\x7b\x25\x37\x2b\x20\x21\x65\x6d\x67\x7\x2f\x2c\x38\x29\x39\x74\x6f\x75\x61\x6a\xb\x74\x7e\x76\x72\x68\x61\x2" + 0x1ce694).s, g_pOverlay->DirectX->Draw->dwD3D9Address, g_pOverlay->DirectX->Draw->dwD3D9Offset);
		g_pConsole->Print(color_darkgreen, /*Scanning finished*/XorStr<0x20, 18, 0x50e81d1e>("\x73\x42\x43\x4d\x4a\x4c\x48\x40\x8\x4f\x43\x45\x45\x5e\x46\x4a\x54" + 0x50e81d1e).s);
		g_pConsole->Print("\n");

		g_pConsole->PrintC(/*Hallo ^6ihr ^7Idioten ^4:-) ^8G^4o^8r^4d^8o^4n^8 ^2rockt^7. ^Ein ^1Wahnsinn ^3! ^4! ^5!*/XorStr<0xd4, 88, 0x638d339>("\x9c\xb4\xba\xbb\xb7\xf9\x84\xed\xb5\xb5\xac\xff\xbe\xd6\xab\x87\x8d\x8a\x92\x82\x86\xc9\xb4\xdf\xd6\xc0\xc7\xcf\xae\xc9\xb5\xad\xc0\x9a\xa8\xcf\x8a\xa7\xce\x9f\xa2\xc5\x91\xa1\x34\x6f\x5c\x3b\x24\x5b\x34\x75\x67\x6a\x61\x7f\x52\x3a\x20\x2f\x4e\x54\x7b\x7d\x34\x4b\x27\x40\x79\x71\x74\x68\x75\x73\x70\x3f\x7e\x12\x3\x3\x7a\x11\x7\x7\x76\x1c\xb" + 0x638d339).s);
		//con.PrintC("Silex natürlich ^5auch^9!");
		g_pConsole->Print("\n");
		g_pConsole->Print("Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. KK");
		//Console->ExecuteCommand("logaddresses");

		cvars.radar_x = 200;
		cvars.radar_y = 50;
		cvars.radar_range = 300;

		cvars.AIM_On = 1;
		cvars.AIM_Player = 1;
		cvars.AIM_Zombie = 1;
		cvars.AIM_Key = VK_LCONTROL;
		cvars.AIM_Lock = 1;

		cvars.ESP_Player = 1;
		cvars.ESP_Zombie = 1;
		cvars.ESP_Name = 1;
		cvars.ESP_Radar = 1;
		cvars.ESP_Weapon = 1;
		cvars.ESP_City = 1;
		cvars.ESP_Health = 0;
		cvars.ESP_ShowDead = 0;
		cvars.ESP_State = 1;
		cvars.ESP_Box = 0;
		cvars.ESP_Inventar = 1;

		cvars.ESP_Clothing = 1;
		cvars.ESP_Inventory = 1;
		//cvars.ESP_Magazine = 1;
		cvars.ESP_Weapons = 1;
		//cvars.ESP_Other = 1;
		cvars.ESP_Teleport = 4;
		cvars.ESP_Ruined = 1;

		cvars.ESP_Far = 1;
		cvars.ESP_Animals = 1;

		cvars.toggle_steal = 0;

		cvars.MISC_Healthbars = 1;
		cvars.MISC_ZombieWarning = 1;
		cvars.MISC_Statsbox = 1;
		cvars.MISC_Glitcher = 12;
		cvars.MISC_Crosshair = 3;
		//cvars.MISC_UnlimitedAmmo = 1;
		cvars.MISC_Removals = 1;
		cvars.MISC_Swim = 0;
		cvars.MISC_Worldtime = 15;

		cvars.console = 0;

		init = true;
	}

	//if ((cvars.D3D_Wallhack || cvars.D3D_Wireframe) && !init_d3d9_hook) {

	//	Hook_D3D9();
	//	init_d3d9_hook = true;
	//}

	//if ((!cvars.D3D_Wallhack && !cvars.D3D_Wireframe) && init_d3d9_hook) {

	//	Unhook_D3D9();
	//	init_d3d9_hook = false;
	//}

	//Draw->AddText(100, 40, color_white, FW1_LEFT, "Test1");
	//Draw->AddText(100, 60, color_white, FW1_LEFT, "Test2");
	//Draw->AddText(100, 80, color_white, FW1_LEFT, "Test3");
	//Draw->AddText(100, 100, color_white, FW1_LEFT, "Test4");

	//Draw->RenderText();

	if (GetAsyncKeyState(VK_DELETE) & 1) {
		cvars.console = !cvars.console;
	
		if (!cvars.console)
			g_pConsole->GetInput()->Clear();
	}

	if (cvars.console)
		g_pConsole->Render();

	static bool toggle = false;

	if (cvars.MISC_Fullscreen && !toggle) {

		g_pTools->EnterFakeFullscreen(g_pOverlay->GetAppWindow());
		toggle = true;
	}

	if (!cvars.MISC_Fullscreen && toggle) {

		g_pTools->EnterFakeFullscreen(g_pOverlay->GetAppWindow(), false);
		toggle = false;
	}

	if (!Draw->Initialized())
		return;

	/*CD3D9 *d = (CD3D9*)(*(DWORD*)Draw->dwD3D9Address);
	if (d) {

		D3DVIEWPORT9 g;
		d->dev->GetViewport(&g);
	}*/

	Draw->TextBg(7, 7, color_green, /*GordonDZ v1.0.0*/XorStr<0x66, 16, 0xC3D1F969>("\x21\x08\x1A\x0D\x05\x05\x28\x37\x4E\x19\x41\x5F\x42\x5D\x44" + 0xC3D1F969).s);

	if (cvars.MISC_Crosshair)
		Draw->Crosshair(cvars.MISC_Crosshair);

	if (cvars.ESP_Radar)
		g_pPlayerMgr->DrawRadar();

	g_pPlayerMgr->ESP->Reset();
	g_pPlayerMgr->ESP->Render();

	g_pPlayerMgr->Aimbot->Reset();

	for (register int i = 0; i < g_pPlayerMgr->GetMaxObjectIndex(); i++) {

		CObject *obj = g_pPlayerMgr->GetPlayerByIndex(i);

		g_pPlayerMgr->Aimbot->Collect(obj);
		g_pPlayerMgr->ESP->DoPlayer(i);
	}

	if (cvars.MISC_ZombieWarning)
		g_pPlayerMgr->ESP->DoZombieWarning();

	if (cvars.AIM_Animals) {

		for (register int i = 0; i < g_pPlayerMgr->GetMaxTableObjectIndex(OBJ_TABLE::ANIMALS); i++) {

			CObject* obj = g_pPlayerMgr->GetTableObjectByIndex(OBJ_TABLE::ANIMALS, i);
			g_pPlayerMgr->Aimbot->Collect(obj);
		}

		for (register int i = 0; i < g_pPlayerMgr->GetMaxTableObjectIndex(OBJ_TABLE::FAR_ANIMALS); i++) {

			CObject* obj = g_pPlayerMgr->GetTableObjectByIndex(OBJ_TABLE::FAR_ANIMALS, i);
			g_pPlayerMgr->Aimbot->Collect(obj);
		}
	}

	g_pPlayerMgr->Aimbot->Do();
	g_pPlayerMgr->ESP->DoWeaponSteal();

	if (cvars.MISC_Statsbox)
		g_pPlayerMgr->DrawStatsbox();

	if (cvars.MISC_UnlimitedAmmo)
			g_pPlayerMgr->RefillAmmo();

	/*
	+78 --> pointer ändert sich wenn man ins wasser geht
	*/

	CObject *pLocal = g_pPlayerMgr->GetLocalPlayer();
	if (pLocal) {

		//BYTE *v1 = (BYTE*)((DWORD)pLocal + 0xC29);
		//BYTE *v2 = (BYTE*)((DWORD)pLocal + 0xC2C);
		//BYTE *v3 = (BYTE*)((DWORD)pLocal + 0xC2D);
		//BYTE *v4 = (BYTE*)((DWORD)pLocal + 0xC2E);

		//*v1 = 1;
		//*v2 = 1;
		//*v3 = 1;
		//*v4 = 1;

		pLocal->allowDamage = 0;
		pLocal->invisble = 1;

	/*	if (Menu->bMenuActive) {

			CTransformationData *t = g_pPlayerMgr->GetTransformation();
			t->fov.x /= 2;
			t->fov.y /= 2;
		}*/

	//	if (pLocal->q)

		int state = 0;
		if (pLocal->weapon && pLocal->weapon->object) {
			state = reinterpret_cast<CObject*>(pLocal->weapon->object)->state;

			if (GetAsyncKeyState(VK_NUMPAD7) & 1) {
				g_pPlayerMgr->AskForDropItem(pLocal, NULL);
			}

			if (GetAsyncKeyState(VK_NUMPAD8) & 1) {
				g_pPlayerMgr->AskForDropItem(pLocal, (CItem*)pLocal->weapon->object);
			}
		}

		int j = 0;

		if (GetAsyncKeyState(VK_NUMPAD6) & 1) {
			for (int i = 0; i < 12; i++) {

				if (!g_pPlayerMgr->IsValid(pLocal->InventorySlot[i]))
					continue;

				Draw->TextBg(300, 50 + j, color_green, "%s", GItem(pLocal->InventorySlot[i]).GetClearName());
				g_pTools->Log("%08X %s", pLocal->InventorySlot[i], GItem(pLocal->InventorySlot[i]).GetClearName());
				j += 12;

				for (auto &it : g_pPlayerMgr->GetWeaponSlots(reinterpret_cast<CWeapon*>(pLocal->InventorySlot[i]))) {

					if (!g_pPlayerMgr->IsValid(it))
						continue;

					g_pTools->Log("  %08X %s", it, GItem(it).GetClearName());
					j += 12;
				}

				for (auto &it : g_pPlayerMgr->GetItemContainerContent(pLocal->InventorySlot[i])) {

					if (!g_pPlayerMgr->IsValid(it))
						continue;

					int max_ammo = 0;
					int cur_ammo = g_pPlayerMgr->GetMagazineAmmo((CObject*)it, &max_ammo);

					if (max_ammo && GItem(it).GetType() == GItem::ITEM_TYPE::T_MAGAZINE)
						Draw->TextBg(330, 50 + j, color_white, "%s [%d|%d]", GItem(it).GetClearName(), cur_ammo, max_ammo);
					else
						Draw->TextBg(330, 50 + j, color_white, "%s", GItem(it).GetClearName());

					g_pTools->Log("  %08X %s", it, GItem(it).GetClearName());
					j += 12;
				}
			}
		}

		//007431D8    8886 40180000   mov byte ptr ds:[esi+1840], al

		//BYTE *bla = reinterpret_cast<BYTE*>(g_pPlayerMgr->GetBase()->World + 0x1840);
		//*bla = 1;

		Draw->TextBg(300, 20, color_white, "%d %d %X", (int)pLocal->state, (int)pLocal->state2, (DWORD)&pLocal->state - (DWORD)pLocal);

		GVector view = g_pPlayerMgr->GetViewangles();
		GVector pos = pLocal->visualstate->pos;
		GVector weapon_pos;

		CWeapon *w = NULL;
		if (pLocal->weapon && pLocal->weapon->object) {
			w = pLocal->weapon->object;
			weapon_pos = ((CObject*)w)->visualstate->pos;
		}
		//if (GetAsyncKeyState(VK_NUMPAD2) & 1) {
		//	for (int i = 0; i < 12; i++) {

		//		if (pLocal->InventorySlot[i]) {
		//			for (auto &lol : g_pPlayerMgr->GetItemContainerContent(pLocal->InventorySlot[i])) {
		//				g_pPlayerMgr->DropItem(pLocal, lol);
		//			}
		//			g_pPlayerMgr->DropItem(pLocal, pLocal->InventorySlot[i]);
		//		}
		//	}
		//}

		GVector forward, up, right;
		g_pPlayerMgr->GetFixedView(forward, up, right);

		//GVector forward2 = pLocal->visualstate->forward;
		//GVector up2 = pLocal->visualstate->up;
		//GVector right2 = pLocal->visualstate->right;

		//GVector forward3 = pLocal->visualstate->forward_;
		//GVector up3;
		//GVector right3 = pLocal->visualstate->right_;

		//Draw->TextBg(300, 50, color_white, "F0: %.2f %.2f %.2f", forward.x, forward.y, forward.z);
		//Draw->TextBg(300, 65, color_white, "U0: %.2f %.2f %.2f", up.x, up.y, up.z);
		//Draw->TextBg(300, 80, color_white, "R0: %.2f %.2f %.2f", right.x, right.y, right.z);

		//Draw->TextBg(300, 110, color_white, "F1: %.2f %.2f %.2f", forward2.x, forward2.y, forward2.z);
		//Draw->TextBg(300, 125, color_white, "U1: %.2f %.2f %.2f", up2.x, up2.y, up2.z);
		//Draw->TextBg(300, 140, color_white, "R1: %.2f %.2f %.2f", right2.x, right2.y, right2.z);

		//Draw->TextBg(300, 170, color_white, "F2: %.2f %.2f %.2f", forward3.x, forward3.y, forward3.z);
		//Draw->TextBg(300, 185, color_white, "U2: %.2f %.2f %.2f", up3.x, up3.y, up3.z);
		//Draw->TextBg(300, 200, color_white, "R2: %.2f %.2f %.2f", right3.x, right3.y, right3.z);

		////F1 = R0
		////R1 = F0 F0.y = pitch

		//DWORD *bla = (DWORD*)((DWORD)pLocal + 0xA5C);

		//Draw->TextBg(300, 50, color_white, "%.2f %.2f %.2f", pos.x, pos.y, pos.z);
		//Draw->TextBg(300, 65, color_white, "%08X %08X", bla[0], bla[1]);


		//if (GetAsyncKeyState(VK_NUMPAD7) & 1) {

		//	for (int i = 11; i >= 0; i--) {
		//		if (pLocal->InventorySlot[i]) {

		//			g_pPlayerMgr->AskForDropItem(pLocal, pLocal->InventorySlot[i]);

		//			break;
		//		}
		//	}

		//}

		//if (GetAsyncKeyState(VK_NUMPAD8) & 1) {

		//	for (int i = 11; i >= 0; i--) {
		//		if (pLocal->InventorySlot[i]) {

		//			g_pPlayerMgr->MoveToGround(pLocal->InventorySlot[i]);

		//			break;
		//		}
		//	}

		//}


		if (cvars.MISC_Glitcher && !Menu->bMenuActive) {
			if (g_pInput->IsKeyDown(VK_SPACE))
				g_pPlayerMgr->FlyToView(pLocal->visualstate->pos);


			/*float Speed = (float)cvars.MISC_Glitcher / 100;
			if (g_pInput->IsKeyDown(VK_UP))
				pLocal->visualstate->pos += g_pPlayerMgr->GetForwardVector(Speed);
			if (g_pInput->IsKeyDown(VK_DOWN))
				pLocal->visualstate->pos -= g_pPlayerMgr->GetForwardVector(Speed);
			if (g_pInput->IsKeyDown(VK_RIGHT))
				pLocal->visualstate->pos += g_pPlayerMgr->GetRightVector(Speed);
			if (g_pInput->IsKeyDown(VK_LEFT))
				pLocal->visualstate->pos -= g_pPlayerMgr->GetRightVector(Speed);*/
		}

		if (cvars.MISC_Healthbars)
			g_pPlayerMgr->DrawHealthbars();

		if (cvars.MISC_Removals) {

			pLocal->recoil = 0;
			/*pLocal->recoilRandomX = 0;
			pLocal->recoilRandomY = 0;
			pLocal->recoilRandomZ = 0;*/
			pLocal->fatigue = 1;
			pLocal->fatigue_diff = 0;

			g_pPlayerMgr->RemoveFiringSpeed();
		}
		else
			pLocal->recoil = 1;

		if (cvars.MISC_Swim) {

			if (g_pInput->IsKeyDown(VK_MENU)) {
				pLocal->swim = 1;
				g_pPlayerMgr->FlyToView(pLocal->visualstate->pos);
			}
			else if (!pLocal->waterContact)
				pLocal->swim = 0;
		}

		if (cvars.MISC_Worldtime)
			g_pPlayerMgr->SetWorldTime(cvars.MISC_Worldtime);
	}

	Menu->Do();
}

DWORD GDirectX::DX9_VTableAddress(const int index)
{
	DWORD dwBase = *reinterpret_cast<DWORD*>(Draw->GetD3D9Device());
	if (dwBase == 0) 
		return 0;

	return dwBase + 4 * index;
}

void *GDirectX::DX9_Hook_VTable(const int index, void *func)
{
	DWORD table_address = DX9_VTableAddress(index);
	DWORD *table_address_ptr = reinterpret_cast<DWORD*>(table_address);

	if (!table_address || IsBadReadPtr((void*)table_address, 4))
		return NULL;

	table_address = *reinterpret_cast<DWORD*>(table_address);

	if (table_address != reinterpret_cast<DWORD>(func)) {

		void *orig_function_ptr = reinterpret_cast<void*>(table_address);

		*table_address_ptr = reinterpret_cast<DWORD>(func);

		return orig_function_ptr;
	}

	return NULL;
}

HRESULT WINAPI GDirectX::hkPresent(LPDIRECT3DDEVICE9 pDevice, RECT* pSourceRect, RECT* pDestRect, HWND hDestWindowOverride, RGNDATA* pDirtyRegion)
{
	__asm pushad

	__asm popad
	return g_pOverlay->DirectX->Present(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

extern "C" void * _ReturnAddress(void);
#pragma intrinsic(_ReturnAddress)

DWORD ret2 = 0;

HRESULT WINAPI GDirectX::hkEndScene(LPDIRECT3DDEVICE9 pDevice)
{
	//__asm mov ret2, esp
	//ret2 = *(DWORD*)ret2;

	HRESULT ret = g_pOverlay->DirectX->EndScene(pDevice);

	//if ((DWORD)_ReturnAddress() == 0xC09E5F)
	//	return ret;

	DWORD gameoverlayrenderer_start = g_pTools->GetModuleAddress(/*GameOverlayRenderer.dll*/XorStr<0x73, 24, 0x670a33d>("\x34\x15\x18\x13\x38\xe\x1c\x8\x17\x1d\x4\x2c\x1a\xee\xe5\xe7\xf1\xe1\xf7\xa8\xe3\xe4\xe5" + 0x670a33d).s);
	DWORD gameoverlayrenderer_end = gameoverlayrenderer_start + g_pTools->GetModuleSize(gameoverlayrenderer_start);

	if (!((DWORD)_ReturnAddress() >= gameoverlayrenderer_start && (DWORD)_ReturnAddress() < gameoverlayrenderer_end))
		return ret;

	__asm pushad

	//g_pTools->Log("Ret: %X", _ReturnAddress());

	g_pOverlay->DirectX->Draw->SetDevice(pDevice);
	g_pOverlay->DirectX->Draw->InitFont("Verdana", 12, FW_BOLD);
	g_pOverlay->DirectX->Draw->GetViewport(cvars.Screen.x, cvars.Screen.y);

	if (!g_pTools->IsSteamOverlayEnabled())
		g_pOverlay->DirectX->RenderLoop();

	__asm popad
	return ret;
}

HRESULT WINAPI GDirectX::hkReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	__asm pushad;

	g_pOverlay->DirectX->Draw->Release();

	__asm popad;

	return g_pOverlay->DirectX->Reset(pDevice, pPresentationParameters);
}

UINT STRIDE = 0;
HRESULT WINAPI GDirectX::hkDrawIndexedPrimitive(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
	__asm pushad

	if (cvars.D3D_Wallhack && (STRIDE == 32 || STRIDE == 36 || STRIDE == 40 || STRIDE == 60)) { //60 40 36 32
		pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	}

	if (cvars.D3D_Wireframe && (STRIDE == 20)) {//Numvertices 15 45 64 90 108 168 180 315 330 435 480 495 540 672 750 792 810 960 990

		DWORD fillmode = D3DFILL_WIREFRAME;
		if (cvars.D3D_Wireframe == 2)
			fillmode = D3DFILL_POINT;

		pDevice->SetRenderState(D3DRS_FILLMODE, fillmode); //primcount 
	}

	__asm popad
	return g_pOverlay->DirectX->DrawIndexedPrimitive(pDevice, PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}

HRESULT WINAPI GDirectX::hkSetStreamSource(LPDIRECT3DDEVICE9 pDevice, UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride)
{
	__asm pushad

	STRIDE = Stride;

	__asm popad
	return g_pOverlay->DirectX->SetStreamSource(pDevice, StreamNumber, pStreamData, OffsetInBytes, Stride);
}