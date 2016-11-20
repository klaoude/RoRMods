#pragma once
#include <windows.h>
#include <d3d9.h>
#include <iostream>
#include <d3dx9.h>
#include <Dwmapi.h> 
#include <TlHelp32.h>
#include <vector>
#include <sstream>
#include <iomanip>

struct CUSTOMVERTEX { FLOAT X, Y, Z, RHW; DWORD COLOR; };
#define CUSTOMFVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

class D3DHook
{
public:
	D3DHook(int width, int height) { m_width = width; m_height = height; WIDTH = height / 40.0f; LENGHT = WIDTH * 5.0f; }
	
	void render();
	void initD3D(HWND hWnd);
	
	void initFont();
	

	void vHUD();
	void addRect(float x, float y, float l, float w, D3DCOLOR color);
	void addLifeRect(float x, float y, float w, D3DCOLOR color);
	void refreshLife();

	

	void textHud();
	void DrawTextString(int x, int y, int h, int w, DWORD color, const char *str, LPD3DXFONT pfont, int align);
	void DrawOutline(int x, int y, int h, int w, DWORD color, const char * str, LPD3DXFONT pfont, int align, RECT container);
	void error();
	void info();
	//void drawString(int x, int y, DWORD color, LPD3DXFONT g_pFont, const char * fmt);


	void setErr(const char* str, int val) { m_error = str; m_err_life = val; }
	void setInfo(const char* str, int val) { m_info = str;  m_info_life = val; }

	void setdmg(float dmg) { m_dmg = dmg;  }
	void setrate(float firerate) { m_firerate = firerate; }
	void setcrit(float crit) { m_crit = crit; }
	void setregen(float regen) { m_regen = regen; }
	void setstrength(float strength) { m_strength = strength; }
	void setitem(int nb) { m_item = nb; }
	void setlvl(int lvl) { m_lvl = lvl; }

	void setlife(float life) { m_life = life; }
	void setmlife(float mlife) { m_mlife = mlife; }

	void setlmlife() {m_lmlife = LENGHT - 2.5 * WIDTH / 8; }


	



	


	//void ipBox(const char *str);
	//void setIp(char const* str) { m_ip = str; }

	
	
	
	


private:
	LPDIRECT3DDEVICE9 m_d3ddev; 
	LPD3DXFONT m_pFont, m_pFontSmall, m_pFontDefault, m_pFontStat, m_pFontDefaultSmall;
	LPDIRECT3D9 m_d3d;
	LPDIRECT3DVERTEXBUFFER9 m_vbuffer = NULL, m_ipvbuffer = NULL;
	int m_width, m_height;

	int m_err_life, m_info_life;
	float WIDTH, LENGHT;
	float m_lmlife;
	float m_llife;

	float m_dmg, m_firerate, m_crit, m_regen, m_strength, m_life, m_mlife, m_lvl;
	int m_item;
	const char* m_error, *m_info, *m_ip;

	std::vector<CUSTOMVERTEX> m_vertices, m_ipvertices;
};

