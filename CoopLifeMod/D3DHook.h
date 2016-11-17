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
	D3DHook(int width, int height) { m_width = width; m_height = height; WIDTH = height / 40; LENGHT = WIDTH * 5; }

	void initD3D(HWND hWnd);
	void setlmlife() {m_lmlife = LENGHT - 2.5 * WIDTH / 8; }
	void render(char* str, int life, int mlife);

	void DrawTextString(int x, int y, int h, int w, DWORD color, const char *str, LPD3DXFONT pfont, int align);
	void addRect(float x, float y, float l, float w, D3DCOLOR color);
	void addLifeRect(float x, float y, float w, D3DCOLOR color);
	void refreshLife();
	void drawString(int x, int y, DWORD color, LPD3DXFONT g_pFont, const char * fmt);
	void textHud(char * str);
	
	void error(const char *str);
	void info(const char *str);

	void ipBox(const char *str);

	void initFont();
	void vHUD();
	
	void setErr_life(int val) { m_err_life = val; }
	void setInfo_life(int val) { m_info_life = val; }

private:
	LPDIRECT3DDEVICE9 m_d3ddev; 
	LPD3DXFONT m_pFont, m_pFontSmall, m_pFontDefault, m_pFontStat;
	LPDIRECT3D9 m_d3d;
	LPDIRECT3DVERTEXBUFFER9 m_vbuffer = NULL;
	int m_width, m_height;

	int m_err_life, m_info_life;
	float WIDTH, LENGHT;
	float m_lmlife;
	float m_llife;
	std::vector<CUSTOMVERTEX> m_vertices;
};

