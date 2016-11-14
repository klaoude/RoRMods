#pragma once

#include <windows.h>
#include <d3d9.h>
#include <iostream>
#include <d3dx9.h>
#include <Dwmapi.h> 
#include <TlHelp32.h>

#define CENTERX (GetSystemMetrics(SM_CXSCREEN)/2)-(s_width/2)
#define CENTERY (GetSystemMetrics(SM_CYSCREEN)/2)-(s_height/2)

class D3DHook
{
public:
	D3DHook(int width, int height) { m_width = width; m_height = height; }

	void initD3D(HWND hWnd);

	void render(char* str, int life, int mlife);
	void drawString(int x, int y, DWORD color, LPD3DXFONT g_pFont, const char *fmt);

	void DrawTextString(int x, int y, int h, int w, DWORD color, const char *str);
	void initFont();
	void vHUD();

private:
	LPDIRECT3DDEVICE9 m_d3ddev;
	LPDIRECT3DVERTEXBUFFER9 m_vbuffer; 
	LPD3DXFONT m_pFont;
	LPDIRECT3D9 m_d3d;

	int m_width, m_height;
};

