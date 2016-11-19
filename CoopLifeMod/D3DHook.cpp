#include "D3DHook.h"
//#include "dhFastFont9.h"

void D3DHook::initD3D(HWND hWnd)
{
	m_d3d = Direct3DCreate9(D3D_SDK_VERSION);    // create the Direct3D interface

	D3DPRESENT_PARAMETERS d3dpp;    // create a struct to hold various device information

	ZeroMemory(&d3dpp, sizeof(d3dpp));    // clear out the struct for use
	d3dpp.Windowed = TRUE;    // program windowed, not fullscreen
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;    // discard old frames
	d3dpp.hDeviceWindow = hWnd;    // set the window to be used by Direct3D
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;     // set the back buffer format to 32-bit
	d3dpp.BackBufferWidth = m_width;    // set the width of the buffer
	d3dpp.BackBufferHeight = m_height;    // set the height of the buffer

	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	// create a device class using this information and the info from the d3dpp stuct
	m_d3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&m_d3ddev);

	vHUD();    // call the function to initialize the lifebar
	
	initFont(); //init font	
	setlmlife();

	//ipBox("");
}

void D3DHook::render(char* str, int life, int mlife)
{
	m_llife = life * m_lmlife / mlife;

	refreshLife();
	// clear the window alpha
	m_d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

	m_d3ddev->BeginScene();    // begins the 3D scene

	// select which vertex format we are using
	m_d3ddev->SetFVF(CUSTOMFVF);

	// select the vertex buffer to display
	m_d3ddev->SetStreamSource(0, m_vbuffer, 0, sizeof(CUSTOMVERTEX));
	// copy the vertex buffer to the back buffer
	m_d3ddev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_vertices.size()/3);

	if (true) //draw howto if...
		DrawTextString(m_width / 2 - 100, m_height / 15, 100, 200, D3DCOLOR_ARGB(255, 255, 255, 255), "F1 to host | F2 to connect", m_pFontDefaultSmall, DT_CENTER);
		
	int shit;

	if (m_err_life > 0)
	{
		error();
		m_err_life--;
	}

	if (m_info_life > 0)
	{
		info();
		m_info_life--;
	}

	textHud(str);

	m_d3ddev->EndScene();    // ends the 3D scene

	m_d3ddev->Present(NULL, NULL, NULL, NULL);   // displays the created frame on the screen
}

void D3DHook::initFont()
{
	AddFontResourceEx("Resources/RiskofRainSquare.ttf", FR_PRIVATE, 0);
	AddFontResourceEx("Resources/RiskofRainFont.ttf", FR_PRIVATE, 0);

	D3DXCreateFont(m_d3ddev, 18, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "RiskofRainSquare", &m_pFont);
	D3DXCreateFont(m_d3ddev, 10, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "RiskofRainSquare", &m_pFontSmall);
	D3DXCreateFont(m_d3ddev, 10, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "RiskofRainFont", &m_pFontStat);

	D3DXCreateFont(m_d3ddev, 13, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &m_pFontDefaultSmall);
	D3DXCreateFont(m_d3ddev, 15, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &m_pFontDefault);
}

void D3DHook::DrawTextString(int x, int y, int h, int w, DWORD color, const char *str, LPD3DXFONT pfont, int align)
{
	// set container of text
	RECT container = { x, y, x + w, y + h };
	// Output the text, center aligned
	pfont->DrawText(NULL, str, -1, &container, align, color);
}

void D3DHook::vHUD()
{	
	addRect(10.0f, 60.0f, LENGHT, WIDTH, D3DCOLOR_ARGB(255, 51, 43, 60)); //EXTERNAL OUTLINE
	addRect(10.0f + WIDTH / 8, 60.0f + WIDTH / 8, LENGHT - 2.5 * WIDTH / 8, WIDTH - 2.5 * WIDTH / 8, D3DCOLOR_ARGB(255, 26, 26 , 26)); //HEALTH BACKGROUND
	
	addLifeRect(10.0f + WIDTH / 8, 60.0f + WIDTH / 8, WIDTH - 2.5 * WIDTH / 8, D3DCOLOR_ARGB(255, 136, 211, 103)); //HEALTH
	
	addRect(100, m_height / 5, 100, 9.8, D3DCOLOR_ARGB(255, 0, 0, 0));
	
	// create a vertex buffer interface called m_vbuffer
	m_d3ddev->CreateVertexBuffer(m_vertices.size() * sizeof(CUSTOMVERTEX), NULL, CUSTOMFVF, D3DPOOL_MANAGED, &m_vbuffer, NULL);

	VOID* pVoid;    // a void pointer

	CUSTOMVERTEX array[100];
	std::copy(m_vertices.begin(), m_vertices.end(), array);
	// lock m_vbuffer and load the vertices into it

	m_vbuffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, array, 20*m_vertices.size());
	m_vbuffer->Unlock();
}

void D3DHook::refreshLife()
{
	for (int i = 0; i < 6; i++)
		m_vertices.pop_back();

	addLifeRect(10.0f + WIDTH / 8, 60.0f + WIDTH / 8, WIDTH - 2.5 * WIDTH / 8, D3DCOLOR_ARGB(255, 136, 211, 103)); //HEALTH
	// create a vertex buffer interface called m_vbuffer
	m_d3ddev->CreateVertexBuffer(m_vertices.size() * sizeof(CUSTOMVERTEX), NULL, CUSTOMFVF, D3DPOOL_MANAGED, &m_vbuffer, NULL);

	VOID* pVoid;    // a void pointer

	int size = m_vertices.size();

	CUSTOMVERTEX array[100];
	std::copy(m_vertices.begin(), m_vertices.end(), array);
	// lock m_vbuffer and load the vertices into it
	m_vbuffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, array, 20 * m_vertices.size());
	m_vbuffer->Unlock();
}


void D3DHook::addRect(float x, float y, float l, float w, D3DCOLOR color)
{
	m_vertices.push_back({ x, y, 0.0f, 0.0f, color });
	m_vertices.push_back({ x + l, y, 0.0f, 0.0f, color });
	m_vertices.push_back({ x, y + w, 0.0f, 0.0f, color });

	m_vertices.push_back({ x + l, y, 0.0f, 0.0f, color });
	m_vertices.push_back({ x + l, y + w, 0.0f, 0.0f, color });
	m_vertices.push_back({ x, y + w, 0.0f, 0.0f, color });	
}


void D3DHook::addLifeRect(float x, float y, float w, D3DCOLOR color)
{
	m_vertices.push_back({ x, y, 0.0f, 0.0f, color });
	m_vertices.push_back({ x + m_llife, y, 0.0f, 0.0f, color });
	m_vertices.push_back({ x + m_llife, y + w, 0.0f, 0.0f, color });

	m_vertices.push_back({ x + m_llife, y + w, 0.0f, 0.0f, color });
	m_vertices.push_back({ x, y + w, 0.0f, 0.0f, color });
	m_vertices.push_back({ x, y, 0.0f, 0.0f, color });
}

void D3DHook::drawString(int x, int y, DWORD color, LPD3DXFONT g_pFont, const char * fmt)
{
	RECT FontPos = { x, y, x + 120, y + 16 };
	char buf[1024] = { '\0' };
	va_list va_alist;

	va_start(va_alist, fmt);
	vsprintf_s(buf, fmt, va_alist);
	va_end(va_alist);
	g_pFont->DrawText(NULL, buf, -1, &FontPos, DT_NOCLIP, color);
}

float pixToShit(float pix)
{
	return 797 * pix / 1440 - 1;
}

void D3DHook::textHud(char * str)
{
	//LIFE & OUTLINE
	DrawTextString(9, 60 + 0.5 + WIDTH / 8, WIDTH, LENGHT, D3DCOLOR_ARGB(255, 26, 26, 26), str, m_pFontSmall, DT_CENTER);
	DrawTextString(11, 60 + 0.5 + WIDTH / 8, WIDTH, LENGHT, D3DCOLOR_ARGB(255, 26, 26, 26), str, m_pFontSmall, DT_CENTER);
	DrawTextString(10, 59 + 0.5 + WIDTH / 8, WIDTH, LENGHT, D3DCOLOR_ARGB(255, 26, 26, 26), str, m_pFontSmall, DT_CENTER);
	DrawTextString(10, 61 + 0.5 + WIDTH / 8, WIDTH, LENGHT, D3DCOLOR_ARGB(255, 26, 26, 26), str, m_pFontSmall, DT_CENTER);
	DrawTextString(10, 60 + 0.5 + WIDTH / 8, WIDTH, LENGHT, D3DCOLOR_ARGB(255, 255, 255, 255), str, m_pFontSmall, DT_CENTER);

	int yoff = 1;
	float height = 9.8;

	DrawTextString( 76*m_width/100, m_height/10 + 18 + yoff * height, height, m_width/5, D3DCOLOR_ARGB(255, 192, 192, 192), "Yolo",  m_pFontStat, DT_RIGHT);
	yoff++;
	DrawTextString(76 * m_width / 100, m_height / 10 + 18 + yoff * height, height, m_width / 5, D3DCOLOR_ARGB(255, 192, 192, 192), "Yolo", m_pFontStat, DT_RIGHT);
	yoff++;
	DrawTextString(76 * m_width / 100, m_height / 10 + 18 + yoff * height, height, m_width / 5, D3DCOLOR_ARGB(255, 192, 192, 192), "Yolo", m_pFontStat, DT_RIGHT);
	yoff+=2;
	DrawTextString(76 * m_width / 100, m_height / 10 + 18 + yoff * height, height, m_width / 5, D3DCOLOR_ARGB(255, 192, 192, 192), "Yolo", m_pFontStat, DT_RIGHT);
	yoff++;
	DrawTextString(76 * m_width / 100, m_height / 10 + 18 + yoff * height, height, m_width / 5, D3DCOLOR_ARGB(255, 192, 192, 192), "Yolo", m_pFontStat, DT_RIGHT);
}

void D3DHook::error()
{
	RECT Rect = { 10, m_height - 20,0,0 };

	m_pFontDefault->DrawText(NULL, m_error, -1, &Rect, DT_CALCRECT, 0);
	m_pFontDefault->DrawText(NULL, m_error, -1, &Rect, DT_LEFT, D3DCOLOR_ARGB(255, 255, 0, 0));
}

void D3DHook::info()
{
	RECT Rect = { 10, m_height - 20,0,0 };

	m_pFontDefault->DrawText(NULL, m_info, -1, &Rect, DT_CALCRECT, 0);
	m_pFontDefault->DrawText(NULL, m_info, -1, &Rect, DT_LEFT, D3DCOLOR_ARGB(255, 0, 255, 255));
}

/*CUSTOMVERTEX vertices[] =
{
	//FIRST OUTLINE
	{ 10.0f, 100.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(51, 43, 60) },
	{ 10.0f + LENGHT, 100.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(51, 43, 60) },
	{ 10.0f, 100.0f + WIDTH, 0.0f, 0.0f, D3DCOLOR_XRGB(51, 43, 60) },
	{ 10.0f + LENGHT, 100.0f + WIDTH, 0.0f, 0.0f, D3DCOLOR_XRGB(51, 43, 60) },

	{ 10.0f, 100.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(51, 43, 60) },
	{ 10.0f + LENGHT, 100.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(51, 43, 60) },
	{ 10.0f, 100.0f + WIDTH, 0.0f, 0.0f, D3DCOLOR_XRGB(51, 43, 60) },
	{ 10.0f + LENGHT, 100.0f + WIDTH, 0.0f, 0.0f, D3DCOLOR_XRGB(51, 43, 60) },

	{ 10.0f, 100.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(51, 43, 60) },
	{ 10.0f + LENGHT, 100.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(51, 43, 60) },
	{ 10.0f, 100.0f + WIDTH, 0.0f, 0.0f, D3DCOLOR_XRGB(51, 43, 60) },
	{ 10.0f + LENGHT, 100.0f + WIDTH, 0.0f, 0.0f, D3DCOLOR_XRGB(51, 43, 60) },

	{ 10.0f + LENGHT, 100.0f , 0.0f, 0.0f, D3DCOLOR_XRGB(51, 43, 60) },
	{ 10.0f + LENGHT, 100.0f + WIDTH, 0.0f, 0.0f, D3DCOLOR_XRGB(51, 43, 60) },
	{ 10.0f, 100.0f + WIDTH, 0.0f, 0.0f, D3DCOLOR_XRGB(51, 43, 60) },

	//SECOND OUTLINE
	{ 10.0f + WIDTH / 16 , 100.0f + WIDTH / 16, 0.0f, 0.0f, D3DCOLOR_XRGB(64, 65, 87) },
	{ 10.0f + LENGHT - WIDTH / 16, 100.0f + WIDTH / 16, 0.0f, 0.0f, D3DCOLOR_XRGB(64, 65, 87) },
	{ 10.0f + WIDTH / 16, 100.0f + WIDTH - WIDTH / 16, 0.0f, 0.0f, D3DCOLOR_XRGB(64, 65, 87) },

	{ 10.0f + LENGHT - WIDTH / 16 , 100.0f + WIDTH / 16, 0.0f, 0.0f, D3DCOLOR_XRGB(64, 65, 87) },
	{ 10.0f + LENGHT - WIDTH / 16, 100.0f + WIDTH - WIDTH / 16 , 0.0f, 0.0f, D3DCOLOR_XRGB(64, 65, 87) },
	{ 10.0f + WIDTH / 16, 100.0f + WIDTH - WIDTH / 16, 0.0f, 0.0f, D3DCOLOR_XRGB(64, 65, 87) },

	//BACKGROUND (NO-LIFE)
	{ 10.0f + 2 * WIDTH / 16 , 100.0f + 2 * WIDTH / 16, 0.0f, 0.0f, D3DCOLOR_XRGB(26, 26, 26) },
	{ 10.0f + LENGHT - 2 * WIDTH / 16, 100.0f + 2 * WIDTH / 16, 0.0f, 0.0f, D3DCOLOR_XRGB(26, 26, 26) },
	{ 10.0f + 2 * WIDTH / 16, 100.0f + WIDTH - 2 * WIDTH / 16, 0.0f, 0.0f, D3DCOLOR_XRGB(26, 26, 26) },

	{ 10.0f + LENGHT - 2 * WIDTH / 16 , 100.0f + 2 * WIDTH / 16, 0.0f, 0.0f, D3DCOLOR_XRGB(26, 26, 26) },
	{ 10.0f + LENGHT - 2 * WIDTH / 16, 100.0f + WIDTH - 2 * WIDTH / 16 , 0.0f, 0.0f, D3DCOLOR_XRGB(26, 26, 26) },
	{ 10.0f + 2 * WIDTH / 16, 100.0f + WIDTH - 2 * WIDTH / 16, 0.0f, 0.0f, D3DCOLOR_XRGB(26, 26, 26) },

	//HEALTH (GREEN)
	{ 10.0f + 2 * WIDTH / 16 , 100.0f + 2 * WIDTH / 16, 0.0f, 0.0f, D3DCOLOR_XRGB(136, 211, 103) },
	{ 10.0f + m_llife, 100.0f + 2 * WIDTH / 16, 0.0f, 0.0f, D3DCOLOR_XRGB(136, 211, 103) },
	{ 10.0f + 2 * WIDTH / 16, 100.0f + WIDTH - 2 * WIDTH / 16, 0.0f, 0.0f, D3DCOLOR_XRGB(136, 211, 103) },

	{ 10.0f + m_llife , 100.0f + 2 * WIDTH / 16, 0.0f, 0.0f, D3DCOLOR_XRGB(136, 211, 103) },
	{ 10.0f + m_llife, 100.0f + WIDTH - 2 * WIDTH / 16 , 0.0f, 0.0f, D3DCOLOR_XRGB(136, 211, 103) },
	{ 10.0f + 2 * WIDTH / 16, 100.0f + WIDTH - 2 * WIDTH / 16, 0.0f, 0.0f, D3DCOLOR_XRGB(136, 211, 103) },

};*/

/*
void D3DHook::ipBox(const char *str)
{

float x = m_width/2, y = m_height / 15, l = 100, w = 13;
x -= l / 2;
D3DCOLOR color = D3DCOLOR_ARGB(255, 0, 0, 0);

m_ipvertices.push_back({ x, y, 0.0f, 0.0f, color });
m_ipvertices.push_back({ x + l, y, 0.0f, 0.0f, color });
m_ipvertices.push_back({ x, y + w, 0.0f, 0.0f, color });

m_ipvertices.push_back({ x + l, y, 0.0f, 0.0f, color });
m_ipvertices.push_back({ x + l, y + w, 0.0f, 0.0f, color });
m_ipvertices.push_back({ x, y + w, 0.0f, 0.0f, color });








// create a vertex buffer interface called m_vbuffer
m_d3ddev->CreateVertexBuffer(m_ipvertices.size() * sizeof(CUSTOMVERTEX), NULL, CUSTOMFVF, D3DPOOL_MANAGED, &m_ipvbuffer, NULL);

VOID* pipVoid;    // a void pointer

CUSTOMVERTEX iparray[100];
std::copy(m_ipvertices.begin(), m_ipvertices.end(), iparray);
// lock m_vbuffer and load the vertices into it

m_ipvbuffer->Lock(0, 0, (void**)&pipVoid, 0);
memcpy(pipVoid, iparray, 20 * m_ipvertices.size());
m_ipvbuffer->Unlock();
}
*/
