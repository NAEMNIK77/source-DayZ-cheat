#ifndef __GDRAW__
#define __GDRAW__

#include "GSDK.h"

struct _stack_text {
	int x, y;
	GColor color;
	DWORD flags;
	char text[100];
};

class CDX9Renderer {
public:
	CDX9Renderer();
	void Init();
	void Init(ID3D11Device *dev, ID3D11DeviceContext *dev_context, IDXGISwapChain *swap_chain) {};
	void InitFont(char *fontname = "Verdana", int height = 10, int flags = FW_BOLD);
	void Release();
	bool Initialized();

	void SetDevice(IDirect3DDevice9 *dev);
	IDirect3DDevice9 *GetDevice();
	IDirect3DDevice9 *GetD3D9Device();

	void DrawRectInternal(int x0, int y0, int x1, int y1, GColor color);
	void DrawLineInternal(int x0, int y0, int x1, int y1, GColor color);
	void DrawCircleInternal(int x0, int y0, int radius, GColor color);

	void FilledQuad(int x, int y, int w, int h, GColor color);
	void Line(int x, int y, int x1, int y1, GColor color);
	void Circle(int x, int y, int radius, GColor color);
	void Quad(int x, int y, int w, int h, int px, GColor BorderColor);
	void Pixel(int x, int y, GColor color);
	void Cross(int x, int y, GColor color, int cross_radius = 2);
	void CircleCross(int x, int y, GColor color);
	void Crosshair(int type);

	void Text(int x, int y, GColor color, char *szText, ...);
	void TextFlags(int x, int y, GColor color, DWORD flags, char *szText, ...);
	void TextCentered(int x, int y, GColor color, char *szText, ...);
	void TextBg(int x, int y, GColor color, char *szText, ...);
	void TextCenteredBg(int x, int y, GColor color, char *szText, ...);

	void AddText(int x, int y, GColor color, DWORD flags, char *szText, ...);
	std::list<struct _stack_text> GetText();
	void RenderText();

	void GetTextLength(char* text, textsize *size);
	int GetFontHeight();

	void GetViewport(int &w, int &h);

	DWORD dwD3D9Address;
	DWORD dwD3D9Offset;
private:
	IDirect3DDevice9 *device;
	ID3DXFont *font;

	struct QuadVertex
	{
		float x, y, z, rhw;
		DWORD dwColor;
	};

	bool init;

	struct GFont {
		char name[64];
		int height;
		int flags;
	} fonts;

	std::list<_stack_text>stack_text;

	const int CIRCLE_NUMPOINTS = 30;
	const int MAX_VERTEX_COUNT = CIRCLE_NUMPOINTS + 1;
};

class GDX11Renderer {
public:
	GDX11Renderer();
	void Init(ID3D11Device *dev, ID3D11DeviceContext *dev_context, IDXGISwapChain *swap_chain);

	void Init() {};
	void SetDevice(IDirect3DDevice9 *dev) {};

	ID3D11Device *GetDevice();
	ID3D11DeviceContext *GetDeviceContext();
	IDXGISwapChain *GetSwapChain();
	IDirect3DDevice9 *GetD3D9Device();
	bool Initialized();
	void Release();

	void DrawRectInternal(int x0, int y0, int x1, int y1, GColor color);
	void DrawLineInternal(int x0, int y0, int x1, int y1, GColor color);
	void DrawCircleInternal(int x0, int y0, int radius, GColor color);

	void GetViewport(int &w, int &h);

	DWORD dwD3D9Address;
	DWORD dwD3D9Offset;

private:
	ID3D11Device *device;
	ID3D11DeviceContext *devicecontext;
	ID3DX11Effect *effect;
	ID3DX11EffectTechnique *technique;
	ID3D11InputLayout *inputlayout;
	ID3D11Buffer *vertexbuffer;
	IDXGISwapChain *swapchain;
	IDirect3DDevice9 *d3d9_device;

	bool init;

	struct COLOR_VERTEXX {
		D3DXVECTOR3 Position;
		D3DXCOLOR Color;
	};

	const int CIRCLE_NUMPOINTS = 30;
	const int MAX_VERTEX_COUNT = CIRCLE_NUMPOINTS + 1;

	friend class GDraw;
};

class GDraw : public GDX11Renderer {
public:
	GDraw();
	void InitFont(char *fontname="Courier New", int height=11, int flags=0);

	void FilledQuad(int x, int y, int w, int h, GColor color);
	void Line(int x, int y, int x1, int y1, GColor color);
	void Circle(int x, int y, int radius, GColor color);
	void Quad(int x, int y, int w, int h, int px, GColor BorderColor);
	void Pixel(int x, int y, GColor color);

	void Text(int x, int y, GColor color, char *szText, ...);
	void TextFlags(int x, int y, GColor color, DWORD flags, char *szText, ...);
	void TextCentered(int x, int y, GColor color, char *szText, ...);

	void AddText(int x, int y, GColor color, DWORD flags, char *szText, ...);
	std::list<struct _stack_text> GetText();
	void RenderText();

	void GetTextLength(char* text, textsize *size);
	int GetFontHeight();

	void TextBg(int x, int y, GColor color, char *szText, ...);
	void TextCenteredBg(int x, int y, GColor color, char *szText, ...);

	void Crosshair(int type);
	void Cross(int x, int y, GColor color, int cross_radius = 2);
	void CircleCross(int x, int y, GColor color);

private:
	IFW1Factory *factory;
	IFW1FontWrapper *fontwrapper;

	struct GFont {
		char name[64];
		int height;
		int flags;
	} font;

	std::list<_stack_text>stack_text;
};

const char shaderrow[] = "BlendState FillTechBlend\n"
"{\n"
"    AlphaToCoverageEnable = FALSE;\n"
"    BlendEnable[0] = TRUE;\n"
"    SrcBlend = SRC_ALPHA;\n"
"    DestBlend = INV_SRC_ALPHA;\n"
"    BlendOp = ADD;\n"
"    SrcBlendAlpha = ONE;\n"
"    DestBlendAlpha = ZERO;\n"
"    BlendOpAlpha = ADD;\n"
"    RenderTargetWriteMask[0] = 0x0F;\n"
"};\n\n"
"DepthStencilState FillTechDepth\n"
"{\n"
"    DepthEnable = false;\n"
"};\n\n"
"struct VSOut\n"
"{\n"
"    float4 Col : COLOR;\n"
"    float4 Pos : SV_POSITION;\n"
"};\n\n"
"VSOut VS(float4 Col : COLOR, float4 Pos : POSITION)\n"
"{\n"
"    VSOut Output;\n"
"    Output.Pos = Pos;\n"
"    Output.Col = Col;\n"
"    return Output;\n"
"}\n\n"
"float4 PS(float4 Col : COLOR) : SV_TARGET\n"
"{\n"
"    return Col;\n"
"}\n\n"
"technique10 FillTech\n"
"{\n"
"    pass Pass_0\n"
"    {\n"
"        SetVertexShader(CompileShader(vs_4_0, VS()));\n"
"        SetGeometryShader(NULL);\n"
"        SetPixelShader(CompileShader(ps_4_0, PS()));\n"
"        SetDepthStencilState(FillTechDepth, 0);\n"
"        SetBlendState(FillTechBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);\n"
"    }\n"
"}\n";

#endif