#ifndef __GDIRECTX__
#define __GDIRECTX__

#include "GSDK.h"
#include "GMenu.h"
#include "GDraw.h"
#include "GConsole.h"

typedef HRESULT(WINAPI *Reset_t)(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
typedef HRESULT(WINAPI *EndScene_t)(LPDIRECT3DDEVICE9 pDevice);
typedef HRESULT(WINAPI *Present_t)(LPDIRECT3DDEVICE9 pDevice, RECT* pSourceRect, RECT* pDestRect, HWND hDestWindowOverride, RGNDATA* pDirtyRegion);
typedef HRESULT(WINAPI *DrawIndexedPrimitive_t)(LPDIRECT3DDEVICE9, D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount);
typedef HRESULT(WINAPI *DrawIndexedPrimitive_t2)(LPDIRECT3DDEVICE9, D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount);
typedef HRESULT(WINAPI *SetStreamSource_t)(LPDIRECT3DDEVICE9 pDevice, UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride);

class GDirectX {
public:
	GDirectX();
	bool Initialize(int w, int h, bool vsync, HWND hwnd, float depth, float near_);
	void Release();
	void Resize(int w, int h);
	void Render(HWND hwnd);
	void RenderLoop();

	void *DX9_Hook_VTable(const int index, void *func);
	DWORD DX9_VTableAddress(const int index);

	static HRESULT WINAPI hkPresent(LPDIRECT3DDEVICE9 pDevice, RECT* pSourceRect, RECT* pDestRect, HWND hDestWindowOverride, RGNDATA* pDirtyRegion);
	static HRESULT WINAPI hkEndScene(LPDIRECT3DDEVICE9 pDevice);
	static HRESULT WINAPI hkReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
	static HRESULT WINAPI hkDrawIndexedPrimitive(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount);
	static HRESULT WINAPI hkSetStreamSource(LPDIRECT3DDEVICE9 pDevice, UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride);

	DrawIndexedPrimitive_t DrawIndexedPrimitive;
	SetStreamSource_t SetStreamSource;
	Present_t Present;
	EndScene_t EndScene;
	Reset_t Reset;

	#ifndef USE_D3D9
		GDraw *Draw;
	#else
		CDX9Renderer *Draw;
	#endif

	GMenu *Menu;
private:
	IDXGISwapChain *swapchain;
	ID3D11Device *device;
	ID3D11DeviceContext *device_context;
	ID3D11RenderTargetView *renderTargetView;
	ID3D11Texture2D *depthStencilBuffer;
	ID3D11DepthStencilState *depthStencilState;
	ID3D11DepthStencilView *depthStencilView;
	ID3D11RasterizerState *rasterState;
	ID3D11DepthStencilState* depthDisabledStencilState;
	ID3D11BlendState* alphaEnableBlendingState;
	ID3D11BlendState* alphaDisableBlendingState;

	D3DXMATRIX projection_matrix;
	D3DXMATRIX world_matrix;
	D3DXMATRIX ortho_matrix;

	bool vsync_enabled;
	bool initialized;
};

#endif