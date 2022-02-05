#include "GSDK.h"
#include "GDraw.h"
#include "GESP.h"
#include "GMenu.h"
#include "GPlayerManager.h"
#include "GTools.h"
#include "GColor.h"
#include "GOverlay.h"


GDraw::GDraw()
{

}

void GDraw::InitFont(char *fontname, int fontheight, int fontflags)
{
	strcpy(font.name, fontname);
	font.height = fontheight;
	font.flags = fontflags;

	if (FAILED(FW1CreateFactory(FW1_VERSION, &factory)))
		return;

	if (FAILED(factory->CreateFontWrapper(GetDevice(), g_pTools->asciiDecode(font.name).c_str(), &fontwrapper)))
		return;

	factory->Release();
}

void GDraw::Text(int x, int y, GColor color, char *szText, ...)
{
	if (!Initialized())
		return;

	if (!GetDevice() || !GetDeviceContext())
		return;

	va_list va_alist;
	char logbuf[256];

	va_start(va_alist, szText);
	_vsnprintf(logbuf, sizeof(logbuf), szText, va_alist);
	va_end(va_alist);

	fontwrapper->DrawString(GetDeviceContext(), g_pTools->asciiDecode(logbuf).c_str(), font.height, x, y, color.Get(), FW1_ALIASED);
}

void GDraw::TextCentered(int x, int y, GColor color, char *szText, ...)
{
	if (!Initialized())
		return;

	if (!GetDevice() || !GetDeviceContext())
		return;

	va_list va_alist;
	char logbuf[256];

	va_start(va_alist, szText);
	_vsnprintf(logbuf, sizeof(logbuf), szText, va_alist);
	va_end(va_alist);

	fontwrapper->DrawString(GetDeviceContext(), g_pTools->asciiDecode(logbuf).c_str(), font.height, x, y, color.Get(), FW1_CENTER | FW1_ALIASED);
}

void GDraw::TextFlags(int x, int y, GColor color, DWORD flags, char *szText, ...)
{
	if (!Initialized())
		return;

	if (!GetDevice() || !GetDeviceContext())
		return;

	va_list va_alist;
	char logbuf[256];

	va_start(va_alist, szText);
	_vsnprintf(logbuf, sizeof(logbuf), szText, va_alist);
	va_end(va_alist);

	if (flags == DT_CENTER)
		flags = FW1_CENTER;

	fontwrapper->DrawString(GetDeviceContext(), g_pTools->asciiDecode(logbuf).c_str(), font.height, x, y, color.Get(), flags | FW1_ALIASED);
}

void GDraw::AddText(int x, int y, GColor color, DWORD flags, char *szText, ...)
{
	va_list va_alist;
	char logbuf[256];

	va_start(va_alist, szText);
	_vsnprintf(logbuf, sizeof(logbuf), szText, va_alist);
	va_end(va_alist);

	_stack_text buf = { 0 };

	buf.x = x;
	buf.y = y;
	buf.color = color;
	buf.flags = flags;
	strcpy_s(buf.text, 100, logbuf);

	stack_text.push_front(buf);
}

std::list<_stack_text> GDraw::GetText()
{
	return stack_text;
}

void GDraw::RenderText()
{
	for (auto &it : stack_text)
		TextFlags(it.x, it.y, it.color, it.flags, it.text);

	stack_text.clear();
}

void GDraw::GetTextLength(char *text, textsize *size)
{
	//funktioniert leider nicht
	/*FW1_RECTF layoutRect;

	layoutRect.Left = layoutRect.Right = size->w;
	layoutRect.Top = layoutRect.Bottom = size->h;

	FW1_RECTF rect = fontwrapper->MeasureString(
		g_pTools->asciiDecode(text).c_str(), 
		g_pTools->asciiDecode(font.name).c_str(), 
		font.height, &layoutRect, FW1_ANALYZEONLY);

	size->h = rect.Right - rect.Left;
	size->w = rect.Bottom - rect.Top;*/

	/*IDWriteFactory *pDWriteFactory;
	fontwrapper->GetDWriteFactory(&pDWriteFactory);

	IDWriteTextFormat *pTextFormat;
	pDWriteFactory->CreateTextFormat(g_pTools->asciiDecode(font.name).c_str(), NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_OBLIQUE, DWRITE_FONT_STRETCH_NORMAL, font.height, L"", &pTextFormat);

	IDWriteTextLayout *pTextLayout;
	pDWriteFactory->CreateTextLayout(g_pTools->asciiDecode(text).c_str(), strlen(text), pTextFormat, 0.0f, 0.0f, &pTextLayout);
	pTextLayout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

	DWRITE_OVERHANG_METRICS overhangMetrics;
	pTextLayout->GetOverhangMetrics(&overhangMetrics);

	pTextLayout->Release();
	pTextFormat->Release();
	pDWriteFactory->Release();

	size->w = overhangMetrics.right - overhangMetrics.left;
	size->h = overhangMetrics.bottom - overhangMetrics.top;*/
	size->w = strlen(text) * 6.7;
	size->h = font.height + 1;
}

int GDraw::GetFontHeight()
{
	return font.height + 1;
}

void GDraw::TextBg(int x, int y, GColor color, char *szText, ...)
{
	va_list va_alist;
	char logbuf[256];

	va_start(va_alist, szText);
	_vsnprintf(logbuf, sizeof(logbuf), szText, va_alist);
	va_end(va_alist);

	textsize size = { x, y };
	GetTextLength(logbuf, &size);

	FilledQuad(x, y, size.w, size.h, GColor(0, 0, 0, 165));
	Text(x, y, color, logbuf);
}

void GDraw::TextCenteredBg(int x, int y, GColor color, char *szText, ...)
{
	va_list va_alist;
	char logbuf[256];

	va_start(va_alist, szText);
	_vsnprintf(logbuf, sizeof(logbuf), szText, va_alist);
	va_end(va_alist);

	textsize size = { x, y };
	GetTextLength(logbuf, &size);

	FilledQuad(x - size.w / 2, y, size.w, size.h, GColor(0, 0, 0, 165));
	TextCentered(x, y, color, logbuf);
}

void GDraw::Crosshair(int type)
{
	int w, h;
	GetViewport(w, h);

	int centerX = w / 2;
	int centerY = h / 2;

	if (type == 1) {

		FilledQuad(centerX - 14, centerY, 9, 1, color_white);
		FilledQuad(centerX + 5, centerY, 9, 1, color_white);
		FilledQuad(centerX, centerY - 14, 1, 9, color_white);
		FilledQuad(centerX, centerY + 5, 1, 9, color_white);
		FilledQuad(centerX, centerY, 1, 1, color_red);
	}
	else if (type == 2) {

		FilledQuad(centerX - 14, centerY, 9, 2, color_white);
		FilledQuad(centerX + 5, centerY, 9, 2, color_white);
		FilledQuad(centerX, centerY - 14, 2, 9, color_white);
		FilledQuad(centerX, centerY + 5, 2, 9, color_white);
		FilledQuad(centerX, centerY, 1, 2, color_red);
	}
	else if (type == 3) {

		FilledQuad(centerX - 25, centerY, 50, 1, color_white);
		FilledQuad(centerX - 5, centerY, 10, 1, color_red);
		FilledQuad(centerX, centerY - 25, 1, 50, color_white);
		FilledQuad(centerX, centerY - 5, 1, 10, color_red);
	}
	else if (type == 4) {

		FilledQuad(centerX - 25, centerY, 50, 2, color_white);
		FilledQuad(centerX - 5, centerY, 10, 2, color_red);
		FilledQuad(centerX, centerY - 25, 2, 50, color_white);
		FilledQuad(centerX, centerY - 5, 2, 10, color_red);
	}
	else if (type == 5) {

		Quad(centerX, 0, 0, centerY * 2, 1, color_white);
		Quad(0, centerY, centerX * 2, 1, 1, color_white);
		FilledQuad(centerX, centerY - 5, 1, 10, color_red);
		FilledQuad(centerX - 5, centerY, 10, 1, color_red);
	}
}

void GDraw::Cross(int x, int y, GColor color, int cross_radius)
{
	int fix = cross_radius == 2 ? 1 : cross_radius % 2;

	Line(x - cross_radius, y - cross_radius, x + cross_radius + fix, y + cross_radius + fix, color);
	Line(x + cross_radius, y - cross_radius, x - cross_radius - fix, y + cross_radius + fix, color);
}

void GDraw::CircleCross(int x, int y, GColor color)
{
	int radius = 4;
	Circle(x, y, radius, color);
	Cross(x, y, color, radius-1);
}

void GDraw::FilledQuad(int x, int y, int w, int h, GColor color)
{
	DrawRectInternal(x, y, x + w, y + h, color);
}

void GDraw::Line(int x, int y, int x1, int y1, GColor color)
{
	DrawLineInternal(x, y, x1, y1, color);
}

void GDraw::Circle(int x, int y, int radius, GColor color)
{
	DrawCircleInternal(x, y, radius, color);
}

void GDraw::Quad(int x, int y, int w, int h, int px, GColor BorderColor)
{
	FilledQuad(x, (y + h - px), w, px, BorderColor);
	FilledQuad(x, y, px, h, BorderColor);
	FilledQuad(x, y, w, px, BorderColor);
	FilledQuad((x + w - px), y, px, h, BorderColor);
}

void GDraw::Pixel(int x, int y, GColor color)
{
	FilledQuad(x, y, 1, 1, color);
}

GDX11Renderer::GDX11Renderer()
{
	init = false;
}

bool GDX11Renderer::Initialized()
{
	return init;
}

void GDX11Renderer::Init(ID3D11Device *dev, ID3D11DeviceContext *dev_context, IDXGISwapChain *swap_chain)
{
	device = dev;
	devicecontext = dev_context;
	swapchain = swap_chain;

	if (!dev || !dev_context)
		return;

	if (init)
		return;

	ID3D10Blob *compiledFX = NULL, *errorMsgs = NULL;

	if (FAILED(D3DX11CompileFromMemory(shaderrow, strlen(shaderrow), "FillTechFx", NULL, NULL,
		"FillTech", "fx_5_0", NULL, NULL, NULL, &compiledFX, &errorMsgs, NULL)))
		return;

	if (FAILED(D3DX11CreateEffectFromMemory(compiledFX->GetBufferPointer(), compiledFX->GetBufferSize(), 0, dev, &effect)))
		return;

	compiledFX->Release();

	technique = effect->GetTechniqueByName("FillTech");
	if (!technique)
		return;

	D3D11_INPUT_ELEMENT_DESC lineRectLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	D3DX11_PASS_DESC passDesc;

	if (FAILED(technique->GetPassByIndex(0)->GetDesc(&passDesc)))
		return;

	if (FAILED(dev->CreateInputLayout(lineRectLayout, sizeof(lineRectLayout) / sizeof(lineRectLayout[0]), passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &inputlayout)))
		return;

	D3D11_BUFFER_DESC bufferDesc;

	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = MAX_VERTEX_COUNT * sizeof(COLOR_VERTEXX);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;

	if (FAILED(dev->CreateBuffer(&bufferDesc, NULL, &vertexbuffer)))
		return;

	init = true;
}

void GDX11Renderer::Release()
{
	vertexbuffer->Release();
	inputlayout->Release();
	effect->Release();
}

ID3D11Device *GDX11Renderer::GetDevice()
{
	return device;
}

ID3D11DeviceContext *GDX11Renderer::GetDeviceContext()
{
	return devicecontext;
}

IDXGISwapChain *GDX11Renderer::GetSwapChain()
{
	return swapchain;
}

IDirect3DDevice9 *GDX11Renderer::GetD3D9Device()
{
	if (!dwD3D9Address || !dwD3D9Offset)
		return NULL;

	DWORD buf = *(DWORD*)dwD3D9Address;
	if (!buf)
		return NULL;

	buf += dwD3D9Offset;

	if (IsBadReadPtr((void*)buf, 4))
		return NULL;

	buf = *(DWORD*)buf;

	return (IDirect3DDevice9*)buf;
}

void GDX11Renderer::DrawRectInternal(int x0, int y0, int x1, int y1, GColor color)
{
	if (!init)
		return;

	if (!device || !devicecontext)
		return;

	UINT viewportNumber = 1;

	D3D11_VIEWPORT vp;

	devicecontext->RSGetViewports(&viewportNumber, &vp);

	float xx0 = 2.0f * (x0 - 0.5f) / vp.Width - 1.0f;
	float yy0 = 1.0f - 2.0f * (y0 - 0.5f) / vp.Height;
	float xx1 = 2.0f * (x1 - 0.5f) / vp.Width - 1.0f;
	float yy1 = 1.0f - 2.0f * (y1 - 0.5f) / vp.Height;

	COLOR_VERTEXX* v = NULL;

	D3D11_MAPPED_SUBRESOURCE mapData;

	if (FAILED(devicecontext->Map(vertexbuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapData)))
		return;

	v = (COLOR_VERTEXX*)mapData.pData;

	int r = color.R();
	int g = color.G();
	int b = color.B();
	int a = color.A();

	v[0].Position.x = xx0;
	v[0].Position.y = yy0;
	v[0].Position.z = 0;
	v[0].Color.r = ((FLOAT)r / 255.0f);
	v[0].Color.g = ((FLOAT)g / 255.0f);
	v[0].Color.b = ((FLOAT)b / 255.0f);
	v[0].Color.a = ((FLOAT)a / 255.0f);

	v[1].Position.x = xx1;
	v[1].Position.y = yy0;
	v[1].Position.z = 0;
	v[1].Color.r = ((FLOAT)r / 255.0f);
	v[1].Color.g = ((FLOAT)g / 255.0f);
	v[1].Color.b = ((FLOAT)b / 255.0f);
	v[1].Color.a = ((FLOAT)a / 255.0f);

	v[2].Position.x = xx0;
	v[2].Position.y = yy1;
	v[2].Position.z = 0;
	v[2].Color.r = ((FLOAT)r / 255.0f);
	v[2].Color.g = ((FLOAT)g / 255.0f);
	v[2].Color.b = ((FLOAT)b / 255.0f);
	v[2].Color.a = ((FLOAT)a / 255.0f);

	v[3].Position.x = xx1;
	v[3].Position.y = yy1;
	v[3].Position.z = 0;
	v[3].Color.r = ((FLOAT)r / 255.0f);
	v[3].Color.g = ((FLOAT)g / 255.0f);
	v[3].Color.b = ((FLOAT)b / 255.0f);
	v[3].Color.a = ((FLOAT)a / 255.0f);

	devicecontext->Unmap(vertexbuffer, NULL);
	devicecontext->IASetInputLayout(inputlayout);

	UINT Stride = sizeof(COLOR_VERTEXX);
	UINT Offset = 0;

	devicecontext->IASetVertexBuffers(0, 1, &vertexbuffer, &Stride, &Offset);
	devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	D3DX11_TECHNIQUE_DESC techDesc;

	if (FAILED(technique->GetDesc(&techDesc)))
		return;

	for (UINT p = 0; p < techDesc.Passes; ++p) {

		technique->GetPassByIndex(p)->Apply(0, devicecontext);
		devicecontext->Draw(4, 0);
	}
}

void GDX11Renderer::DrawLineInternal(int x0, int y0, int x1, int y1, GColor color)
{
	if (!init)
		return;

	if (!device || !devicecontext)
		return;

	UINT viewportNumber = 1;

	D3D11_VIEWPORT vp;

	devicecontext->RSGetViewports(&viewportNumber, &vp);

	float xx0 = 2.0f * (x0 - 0.5f) / vp.Width - 1.0f;
	float yy0 = 1.0f - 2.0f * (y0 - 0.5f) / vp.Height;
	float xx1 = 2.0f * (x1 - 0.5f) / vp.Width - 1.0f;
	float yy1 = 1.0f - 2.0f * (y1 - 0.5f) / vp.Height;

	COLOR_VERTEXX* v = NULL;

	D3D11_MAPPED_SUBRESOURCE mapData;

	if (FAILED(devicecontext->Map(vertexbuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapData)))
		return;

	v = (COLOR_VERTEXX*)mapData.pData;

	int r = color.R();
	int g = color.G();
	int b = color.B();
	int a = color.A();

	v[0].Position.x = xx0;
	v[0].Position.y = yy0;
	v[0].Position.z = 0;
	v[0].Color.r = ((FLOAT)r / 255.0f);
	v[0].Color.g = ((FLOAT)g / 255.0f);
	v[0].Color.b = ((FLOAT)b / 255.0f);
	v[0].Color.a = ((FLOAT)a / 255.0f);

	v[1].Position.x = xx1;
	v[1].Position.y = yy1;
	v[1].Position.z = 0;
	v[1].Color.r = ((FLOAT)r / 255.0f);
	v[1].Color.g = ((FLOAT)g / 255.0f);
	v[1].Color.b = ((FLOAT)b / 255.0f);
	v[1].Color.a = ((FLOAT)a / 255.0f);

	devicecontext->Unmap(vertexbuffer, NULL);
	devicecontext->IASetInputLayout(inputlayout);

	UINT Stride = sizeof(COLOR_VERTEXX);
	UINT Offset = 0;

	devicecontext->IASetVertexBuffers(0, 1, &vertexbuffer, &Stride, &Offset);
	devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	D3DX11_TECHNIQUE_DESC techDesc;

	if (FAILED(technique->GetDesc(&techDesc)))
		return;

	for (UINT p = 0; p < techDesc.Passes; ++p) {

		technique->GetPassByIndex(p)->Apply(0, devicecontext);
		devicecontext->Draw(2, 0);
	}
}

void GDX11Renderer::DrawCircleInternal(int x0, int y0, int radius, GColor color)
{
	if (!init)
		return;

	if (!device || !devicecontext)
		return;

	const int NUMPOINTS = CIRCLE_NUMPOINTS;
	UINT viewportNumber = 1;

	D3D11_VIEWPORT vp;

	devicecontext->RSGetViewports(&viewportNumber, &vp);

	COLOR_VERTEXX* v = NULL;

	D3D11_MAPPED_SUBRESOURCE mapData;

	if (FAILED(devicecontext->Map(vertexbuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapData)))
		return;

	v = (COLOR_VERTEXX*)mapData.pData;

	int r = color.R();
	int g = color.G();
	int b = color.B();
	int a = color.A();

	float WedgeAngle = (float)((2 * D3DX_PI) / NUMPOINTS);

	for (int i = 0; i <= NUMPOINTS; i++) {

		float Theta = (float)(i * WedgeAngle);
		float x = (float)(x0 + radius * cos(Theta));
		float y = (float)(y0 - radius * sin(Theta));

		v[i].Position.x = 2.0f * (x - 0.5f) / vp.Width - 1.0f;
		v[i].Position.y = 1.0f - 2.0f * (y - 0.5f) / vp.Height;
		v[i].Position.z = 0.0f;
		v[i].Color.r = ((FLOAT)r / 255.0f);
		v[i].Color.g = ((FLOAT)g / 255.0f);
		v[i].Color.b = ((FLOAT)b / 255.0f);
		v[i].Color.a = ((FLOAT)a / 255.0f);
	}

	devicecontext->Unmap(vertexbuffer, NULL);
	devicecontext->IASetInputLayout(inputlayout);

	UINT Stride = sizeof(COLOR_VERTEXX);
	UINT Offset = 0;

	devicecontext->IASetVertexBuffers(0, 1, &vertexbuffer, &Stride, &Offset);

	devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	//devicecontext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	D3DX11_TECHNIQUE_DESC techDesc;

	if (FAILED(technique->GetDesc(&techDesc)))
		return;

	for (UINT p = 0; p < techDesc.Passes; ++p) {

		technique->GetPassByIndex(p)->Apply(0, devicecontext);
		devicecontext->Draw(NUMPOINTS + 1, 0);
	}
}

void GDX11Renderer::GetViewport(int &w, int &h)
{
	UINT viewportNumber = 1;

	D3D11_VIEWPORT vp;
	GetDeviceContext()->RSGetViewports(&viewportNumber, &vp);

	w = (int)vp.Width;
	h = (int)vp.Height;
}

CDX9Renderer::CDX9Renderer()
{
	font = NULL;
}

void CDX9Renderer::Init()
{
	init = true;
}

void CDX9Renderer::InitFont(char *fontname, int height, int flags)
{
	if (!font || IsBadReadPtr((void*)font, 4)) {

		#define _createfont /*D3DXCreateFontA*/XorStr<0xA5,16,0x28E45D31>("\xE1\x95\xE3\xF0\xEA\xD8\xCE\xCD\xD9\xCB\xE9\xDF\xDF\xC6\xF2"+0x28E45D31).s
		#define _d3dx9 /*d3dx9_41.dll*/XorStr<0x59,13,0x21bc58bf>("\x3d\x69\x3f\x24\x64\x1\x6b\x51\x4f\x6\xf\x8"+0x21bc58bf).s
		typedef HRESULT(D3DAPI *D3DXCreateFont_t)(LPDIRECT3DDEVICE9 pDevice, INT Height, UINT Width, UINT Weight, UINT MipLevels, BOOL Italic, DWORD CharSet, DWORD OutputPrecision, DWORD Quality, DWORD PitchAndFamily, LPCTSTR pFacename, LPD3DXFONT* ppFont);

		D3DXCreateFont_t pD3DXCreateFont = (D3DXCreateFont_t)g_pTools->GetFunctionAddress(g_pTools->GetModuleAddress(_d3dx9), _createfont);

		pD3DXCreateFont(device, height, 0, flags, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE, fontname, &font);
	}
}

void CDX9Renderer::Release()
{
	if (font)
		font->Release();

	font = NULL;
}

void CDX9Renderer::SetDevice(IDirect3DDevice9 *dev)
{
	device = dev;
}

IDirect3DDevice9 *CDX9Renderer::GetDevice()
{
	return device;
}

void CDX9Renderer::DrawRectInternal(int x0, int y0, int x1, int y1, GColor color)
{
	const DWORD D3D_FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;

	DWORD c = D3DCOLOR_ARGB(color.A(), color.R(), color.G(), color.B());

	QuadVertex qV[4] = {
		{ (float)x0, (float)y1, 0.0f, 0.0f, c },
		{ (float)x0, (float)y0, 0.0f, 0.0f, c },
		{ (float)x1, (float)y1, 0.0f, 0.0f, c },
		{ (float)x1, (float)y0, 0.0f, 0.0f, c }
	};

	device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	device->SetRenderState(D3DRS_FOGENABLE, false);

	device->SetFVF(D3D_FVF);
	device->SetTexture(0, NULL);
	device->SetPixelShader(NULL);
	device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, qV, sizeof(QuadVertex));
}

void CDX9Renderer::DrawLineInternal(int x0, int y0, int x1, int y1, GColor color)
{
	const DWORD D3D_FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;

	DWORD c = D3DCOLOR_ARGB(color.A(), color.R(), color.G(), color.B());

	QuadVertex qV[2] = {
		{ (float)x0, (float)y0, 0.0f, 1.0f, c },
		{ (float)x1, (float)y1, 1.0f, 1.0f, c }
	};

	device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	device->SetRenderState(D3DRS_FOGENABLE, false);

	device->SetFVF(D3D_FVF);
	device->SetTexture(0, NULL);
	device->SetPixelShader(NULL);
	device->DrawPrimitiveUP(D3DPT_LINELIST, 2, qV, sizeof(QuadVertex));
}

void CDX9Renderer::DrawCircleInternal(int x0, int y0, int radius, GColor color)
{

	const DWORD D3D_FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
	QuadVertex Circle[31] = { 0 };

	DWORD c = D3DCOLOR_ARGB(color.A(), color.R(), color.G(), color.B());

	float angle = (float)((2 * D3DX_PI) / 30);

	for (int i = 0; i <= 30; i++) {

		float theta = angle * i;
		float X = (float)(x0 + radius * cos(theta));
		float Y = (float)(y0 - radius * sin(theta));

		Circle[i].x = X;
		Circle[i].y = Y;
		Circle[i].z = 0;
		Circle[i].rhw = 1;
		Circle[i].dwColor = c;
	}

	device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	device->SetRenderState(D3DRS_FOGENABLE, false);

	device->SetFVF(D3D_FVF);
	device->SetTexture(0, NULL);
	device->SetVertexShader(NULL);
	device->DrawPrimitiveUP(D3DPT_LINESTRIP, 30, Circle, sizeof(QuadVertex));
}

void CDX9Renderer::Cross(int x, int y, GColor color, int cross_radius)
{
	int fix = cross_radius == 2 ? 1 : cross_radius % 2;

	Line(x - cross_radius, y - cross_radius, x + cross_radius + fix, y + cross_radius + fix, color);
	Line(x + cross_radius, y - cross_radius, x - cross_radius - fix, y + cross_radius + fix, color);
}

void CDX9Renderer::CircleCross(int x, int y, GColor color)
{
	int radius = 4;
	Circle(x, y, radius, color);
	Cross(x, y, color, radius - 1);
}

void CDX9Renderer::FilledQuad(int x, int y, int w, int h, GColor color)
{
	DrawRectInternal(x, y, x + w, y + h, color);
}

void CDX9Renderer::Line(int x, int y, int x1, int y1, GColor color)
{
	DrawLineInternal(x, y, x1, y1, color);
}

void CDX9Renderer::Circle(int x, int y, int radius, GColor color)
{
	DrawCircleInternal(x, y, radius, color);
}

void CDX9Renderer::Quad(int x, int y, int w, int h, int px, GColor BorderColor)
{
	FilledQuad(x, (y + h - px), w, px, BorderColor);
	FilledQuad(x, y, px, h, BorderColor);
	FilledQuad(x, y, w, px, BorderColor);
	FilledQuad((x + w - px), y, px, h, BorderColor);
}

void CDX9Renderer::GetViewport(int &w, int &h)
{
	D3DVIEWPORT9 v;
	device->GetViewport(&v);

	w = v.Width;
	h = v.Height;
}

IDirect3DDevice9 *CDX9Renderer::GetD3D9Device()
{
	if (!dwD3D9Address || !dwD3D9Offset)
		return NULL;

	DWORD buf = *(DWORD*)dwD3D9Address;
	if (!buf)
		return NULL;

	buf += dwD3D9Offset;

	if (IsBadReadPtr((void*)buf, 4))
		return NULL;

	buf = *(DWORD*)buf;

	return (IDirect3DDevice9*)buf;
}

bool CDX9Renderer::Initialized()
{
	return init;
}

void CDX9Renderer::Crosshair(int type)
{
	int w, h;
	GetViewport(w, h);

	int centerX = w / 2;
	int centerY = h / 2;

	if (type == 1) {

		FilledQuad(centerX - 14, centerY, 9, 1, color_white);
		FilledQuad(centerX + 5, centerY, 9, 1, color_white);
		FilledQuad(centerX, centerY - 14, 1, 9, color_white);
		FilledQuad(centerX, centerY + 5, 1, 9, color_white);
		FilledQuad(centerX, centerY, 1, 1, color_red);
	}
	else if (type == 2) {

		FilledQuad(centerX - 14, centerY, 9, 2, color_white);
		FilledQuad(centerX + 5, centerY, 9, 2, color_white);
		FilledQuad(centerX, centerY - 14, 2, 9, color_white);
		FilledQuad(centerX, centerY + 5, 2, 9, color_white);
		FilledQuad(centerX, centerY, 1, 2, color_red);
	}
	else if (type == 3) {

		FilledQuad(centerX - 25, centerY, 50, 1, color_white);
		FilledQuad(centerX - 5, centerY, 10, 1, color_red);
		FilledQuad(centerX, centerY - 25, 1, 50, color_white);
		FilledQuad(centerX, centerY - 5, 1, 10, color_red);
	}
	else if (type == 4) {

		FilledQuad(centerX - 25, centerY, 50, 2, color_white);
		FilledQuad(centerX - 5, centerY, 10, 2, color_red);
		FilledQuad(centerX, centerY - 25, 2, 50, color_white);
		FilledQuad(centerX, centerY - 5, 2, 10, color_red);
	}
	else if (type == 5) {

		Quad(centerX, 0, 0, centerY * 2, 1, color_white);
		Quad(0, centerY, centerX * 2, 1, 1, color_white);
		FilledQuad(centerX, centerY - 5, 1, 10, color_red);
		FilledQuad(centerX - 5, centerY, 10, 1, color_red);
	}
}

void CDX9Renderer::Text(int x, int y, GColor color, char *szText, ...)
{
	if (!Initialized())
		return;

	if (!GetDevice())
		return;

	if (!font || IsBadReadPtr((void*)font, 4))
		return;

	va_list va_alist;
	char logbuf[256];

	va_start(va_alist, szText);
	_vsnprintf(logbuf, sizeof(logbuf), szText, va_alist);
	va_end(va_alist);

	DWORD c = D3DCOLOR_ARGB(color.A(), color.R(), color.G(), color.B());

	RECT font_rect;
	font_rect.top = y;
	font_rect.left = x;
	font_rect.right = x - 0.001;
	font_rect.bottom = y - 0.001;

	font->DrawTextA(NULL, logbuf, strlen(logbuf), &font_rect, DT_NOCLIP, c);
}

void CDX9Renderer::GetTextLength(char *text, textsize *size)
{
	if (!font || IsBadReadPtr((void*)font, 4))
		return;
	/*size->w = strlen(text) * 6.7;
	size->h = font.height + 1;*/
	SIZE s;
	GetTextExtentPoint32(font->GetDC(), text, strlen(text), &s);
	size->w = s.cx;
	size->h = s.cy - 2;
}

int CDX9Renderer::GetFontHeight()
{
	textsize size;
	GetTextLength("ABCDEF", &size);
	return size.h;
}

void CDX9Renderer::TextFlags(int x, int y, GColor color, DWORD flags, char *szText, ...)
{
	if (!Initialized())
		return;

	if (!GetDevice())
		return;

	if (!font || IsBadReadPtr((void*)font, 4))
		return;

	va_list va_alist;
	char logbuf[256];

	va_start(va_alist, szText);
	_vsnprintf(logbuf, sizeof(logbuf), szText, va_alist);
	va_end(va_alist);

	DWORD c = D3DCOLOR_ARGB(color.A(), color.R(), color.G(), color.B());

	if (flags == FW1_RIGHT)
		flags = DT_RIGHT;
	else if (flags == FW1_CENTER)
		flags = DT_CENTER;

	RECT font_rect;
	font_rect.top = y;
	font_rect.left = x;
	font_rect.right = x - 0.001;
	font_rect.bottom = y - 0.001;

	font->DrawTextA(NULL, logbuf, strlen(logbuf), &font_rect, DT_NOCLIP | flags, c);
}

void CDX9Renderer::TextCentered(int x, int y, GColor color, char *szText, ...)
{
	if (!Initialized())
		return;

	if (!GetDevice())
		return;

	if (!font || IsBadReadPtr((void*)font, 4))
		return;

	va_list va_alist;
	char logbuf[256];

	va_start(va_alist, szText);
	_vsnprintf(logbuf, sizeof(logbuf), szText, va_alist);
	va_end(va_alist);

	DWORD c = D3DCOLOR_ARGB(color.A(), color.R(), color.G(), color.B());

	RECT font_rect;
	font_rect.top = y;
	font_rect.left = x;
	font_rect.right = x - 0.001;
	font_rect.bottom = y - 0.001;

	font->DrawTextA(NULL, logbuf, strlen(logbuf), &font_rect, DT_NOCLIP | DT_CENTER, c);
}

void CDX9Renderer::TextBg(int x, int y, GColor color, char *szText, ...)
{
	va_list va_alist;
	char logbuf[256];

	va_start(va_alist, szText);
	_vsnprintf(logbuf, sizeof(logbuf), szText, va_alist);
	va_end(va_alist);

	textsize size;
	GetTextLength(logbuf, &size);

	FilledQuad(x, y + 1, size.w, size.h, GColor(0, 0, 0, 165));
	Text(x, y, color, logbuf);
}

void CDX9Renderer::TextCenteredBg(int x, int y, GColor color, char *szText, ...)
{
	va_list va_alist;
	char logbuf[256];

	va_start(va_alist, szText);
	_vsnprintf(logbuf, sizeof(logbuf), szText, va_alist);
	va_end(va_alist);

	textsize size = { x, y };
	GetTextLength(logbuf, &size);

	FilledQuad(x - size.w / 2 - 1, y + 1, size.w, size.h, GColor(0, 0, 0, 165));
	TextCentered(x, y, color, logbuf);
}

void CDX9Renderer::AddText(int x, int y, GColor color, DWORD flags, char *szText, ...)
{
	va_list va_alist;
	char logbuf[256];

	va_start(va_alist, szText);
	_vsnprintf(logbuf, sizeof(logbuf), szText, va_alist);
	va_end(va_alist);

	_stack_text buf = { 0 };

	buf.x = x;
	buf.y = y;
	buf.color = color;
	buf.flags = flags;
	strcpy_s(buf.text, 100, logbuf);

	stack_text.push_front(buf);
}

std::list<_stack_text> CDX9Renderer::GetText()
{
	return stack_text;
}

void CDX9Renderer::RenderText()
{
	for (auto &it : stack_text)
		TextFlags(it.x, it.y, it.color, it.flags, it.text);

	stack_text.clear();
}