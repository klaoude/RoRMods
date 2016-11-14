#pragma once
#include <windows.h>
#include <d3d9.h>
#include <iostream>
#include <d3dx9.h>
#include <Dwmapi.h> 
#include <TlHelp32.h>

struct CUSTOMVERTEX { FLOAT X, Y, Z, RHW; DWORD COLOR; };
#define CUSTOMFVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

const float WIDTH = 40; //global width
const float LENGHT = WIDTH * 15; //global lenght

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
	LPDIRECT3DVERTEXBUFFER9 m_vbuffer = NULL; 
	LPD3DXFONT m_pFont;
	LPDIRECT3D9 m_d3d;

	int m_width, m_height;
	float m_llife = LENGHT - 2 * WIDTH / 8, m_lmlife;
};

