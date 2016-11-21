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

struct CUSTOMVERTEX { FLOAT X, Y, Z, RHW; DWORD COLOR; }; //Vertex structure for rectangles
#define CUSTOMFVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

class D3DHook
{
public:
	D3DHook(int width, int height) { m_width = width; m_height = height; WIDTH = height / 40.0f; LENGHT = WIDTH * 5.0f; }
	
	void render(); //draw everything
	void initD3D(HWND hWnd);
	
	void initFont(); //create fonts
	

	void vHUD(); //draw rectangles
	void addRect(float x, float y, float l, float w, D3DCOLOR color); //ad rectangle to draw
	void addLifeRect(float x, float y, float w, D3DCOLOR color); //add lifebar
	void refreshLife(); //refresh lifebar lenght

	

	void textHud(); //draw all texts
	void DrawTextString(int x, int y, int h, int w, DWORD color, const char *str, LPD3DXFONT pfont, int align); //draw text
	void DrawOutline(int x, int y, int h, int w, DWORD color, const char * str, LPD3DXFONT pfont, int align, RECT container); //draw outline of a text
	void error(); //draw error text
	void info(); //draw info text




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

	void setpause(int pause) { m_pause = pause; }
	void setscale(int scale) { m_scale = scale; }

	void setlmlife() {m_lmlife = LENGHT - 2.5 * WIDTH / 8; }

	void setpSel(int psel) { m_pSel = psel; }
	int getpSel() { return m_pSel; }
	void setnbP(int nbp) { m_nbP = nbp; }
	int getnbP() { return m_nbP; }


	



	

	//void drawString(int x, int y, DWORD color, LPD3DXFONT g_pFont, const char * fmt);
	//void ipBox(const char *str);
	//void setIp(char const* str) { m_ip = str; }

	
	
	
	


private:
	LPDIRECT3DDEVICE9 m_d3ddev; 
	LPD3DXFONT m_pFont, m_pFontSmall, m_pFontDefault, m_pFontStat, m_pFontDefaultSmall;	//fonts
	LPDIRECT3D9 m_d3d;
	LPDIRECT3DVERTEXBUFFER9 m_vbuffer = NULL; //vertex buffer
	int m_width, m_height; //height & width of window

	int m_err_life, m_info_life;
	float WIDTH, LENGHT; //width & lenght of lifebar (outline not health value)
	float m_lmlife; //max lenght of health value
	float m_llife; //lenght of current health value

	int m_pSel;
	int m_nbP = 1;

	float m_dmg, m_firerate, m_crit, m_regen, m_strength, m_life, m_mlife, m_lvl;
	int m_item, m_pause, m_scale;
	const char* m_error, *m_info, *m_ip;

	std::vector<CUSTOMVERTEX> m_vertices, m_ipvertices; //vertices' vector
};

