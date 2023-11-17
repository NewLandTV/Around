#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <Windows.h>
#include <gdiplus.h>

#pragma comment(lib, "gdiplus.lib")

static class Renderer
{
private:
	static ULONG_PTR token;

	static Gdiplus::Graphics* graphics;

	static Gdiplus::Image* LoadImageFromResource(HINSTANCE hInstance, unsigned int imageID, const wchar_t* resourceType);

public:
	static bool Initialize();
	static void Release();

	static void Image(HINSTANCE hInstance, HWND hWnd, HDC hdc, unsigned int imageID, unsigned int x, unsigned int y, int width, int height);
	static void Text(HWND hWnd, HDC hdc, const wchar_t* text, float x, float y, unsigned short fontSize, const wchar_t* fontFamilyName, Gdiplus::Color color);
};

#endif