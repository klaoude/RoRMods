#pragma once
#include <windows.h>
#include <d3d9.h>
#include <iostream>
#include <d3dx9.h>
#include <Dwmapi.h> 
#include <TlHelp32.h>
#include <vector>

struct CUSTOMVERTEX { FLOAT X, Y, Z, RHW; DWORD COLOR; };
#define CUSTOMFVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

class D3DHook
{
public:
	D3DHook(int width, int height) { m_width = width; m_height = height; WIDTH = height / 35; LENGHT = WIDTH * 12; }

	void initD3D(HWND hWnd);

	void render(char* str, int life, int mlife);

	void DrawTextString(int x, int y, int h, int w, DWORD color, const char *str, LPD3DXFONT pfont, int align);
	void addRect(float x, float y, float l, float w, D3DCOLOR color);
	void drawString(int x, int y, DWORD color, LPD3DXFONT g_pFont, const char * fmt);

	void initFont();
	void vHUD();

private:
	LPDIRECT3DDEVICE9 m_d3ddev; 
	LPD3DXFONT m_pFont, m_pFontSmall, m_pFontDefault, m_pFontOut;
	LPDIRECT3D9 m_d3d;
	LPDIRECT3DVERTEXBUFFER9 m_vbuffer = NULL;
	int m_width, m_height;

	float WIDTH, LENGHT;
	float m_lmlife = LENGHT - 2* WIDTH / 8;
	float m_llife;
	std::vector<CUSTOMVERTEX> m_vertices;
};

