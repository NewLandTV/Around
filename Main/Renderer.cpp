#include "Renderer.h"
#include "resource.h"

ULONG_PTR Renderer::token = 0;

Gdiplus::Graphics* Renderer::graphics = nullptr;

Gdiplus::Image* Renderer::LoadImageFromResource(HINSTANCE hInstance, unsigned int imageID, const wchar_t* resourceType)
{
	IStream* stream = nullptr;

	Gdiplus::Image* image = nullptr;

	HRSRC resource = FindResource(hInstance, MAKEINTRESOURCE(imageID), resourceType);

	if (resource == nullptr || CreateStreamOnHGlobal(nullptr, true, &stream) != S_OK)
	{
		return nullptr;
	}

	LPVOID resourceData = LockResource(LoadResource(hInstance, resource));

	if (resourceData == nullptr)
	{
		return nullptr;
	}

	DWORD imageSize = SizeofResource(hInstance, resource);
	DWORD dwReadWrite = 0;

	stream->Write(resourceData, imageSize, &dwReadWrite);

	image = Gdiplus::Image::FromStream(stream);

	stream->Release();

	return image;
}

bool Renderer::Initialize()
{
	// Setup gdi+
	Gdiplus::GdiplusStartupInput input;

	Gdiplus::GdiplusStartup(&token, &input, nullptr);

	return true;
}

void Renderer::Release()
{
	// Memory
	delete graphics;

	// Gdi+ shutdown
	Gdiplus::GdiplusShutdown(token);
}

void Renderer::Image(HINSTANCE hInstance, HWND hWnd, HDC hdc, unsigned int imageID, unsigned int x, unsigned int y, int width, int height)
{
	Gdiplus::Image* image = LoadImageFromResource(hInstance, imageID, L"PNG");

	Gdiplus::Rect rect(x, y, width, height);

	// Check error
	if (image == nullptr || image->GetLastStatus() != Gdiplus::Ok)
	{
		return;
	}

	if (graphics == nullptr)
	{
		graphics = new Gdiplus::Graphics(hdc);
	}

	graphics->FromHDC(hdc)->DrawImage(&*image, rect);
}

void Renderer::Text(HWND hWnd, HDC hdc, const wchar_t* text, float x, float y, unsigned short fontSize, const wchar_t* fontFamilyName, Gdiplus::Color color)
{
	Gdiplus::FontFamily fontFamily(fontFamilyName);
	Gdiplus::Font font(&fontFamily, fontSize, Gdiplus::FontStyleBold, Gdiplus::UnitPoint);
	Gdiplus::PointF rect(x, y);
	Gdiplus::SolidBrush solidBrush(color);

	// Check error
	if (font.GetLastStatus() != Gdiplus::Ok || solidBrush.GetLastStatus() != Gdiplus::Ok)
	{
		return;
	}

	if (graphics == nullptr)
	{
		graphics = new Gdiplus::Graphics(hdc);
	}

	graphics->FromHDC(hdc)->DrawString(text, -1, &font, rect, &solidBrush);
}