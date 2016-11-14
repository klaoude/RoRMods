#include "D3DHook.h"


float width = 40; //global width
float lenght = width * 15; //global lenght



LPD3DXFONT pFont;

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

	lifebar();    // call the function to initialize the lifebar

	initFont(); //init font
}

void D3DHook::render(char* str, int life, int mlife)
{

	lmlife = lenght - 2 * width / 8;

	llife = life * lmlife / mlife;


	// clear the window alpha
	m_d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

	m_d3ddev->BeginScene();    // begins the 3D scene

	//drawString(10, 100, D3DCOLOR_ARGB(255, 255, 255, 0), m_pFont, str);

	// select which vertex format we are using
	m_d3ddev->SetFVF(CUSTOMFVF);

	// select the vertex buffer to display
	m_d3ddev->SetStreamSource(0, m_vbuffer, 0, sizeof(CUSTOMVERTEX));

	// copy the vertex buffer to the back buffer
	m_d3ddev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 8);

	//Draw lifeText
	DrawTextString(100, 100 + 2*width/8, width - 2*width/8, lenght, D3DCOLOR_XRGB(255, 255, 255), str);

	m_d3ddev->EndScene();    // ends the 3D scene

	m_d3ddev->Present(NULL, NULL, NULL, NULL);   // displays the created frame on the screen
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


void D3DHook::initFont()
{
	AddFontResourceEx("Resources/RiskofRainSquare.ttf", FR_PRIVATE, 0);
	D3DXCreateFont(d3ddev, 20, 0, FW_BOLD, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "RiskofRainSquare", &m_pFont);
}

void D3DHook::DrawTextString(int x, int y, int h, int w, DWORD color, const char *str)
{


	// set container of text
	RECT shit = { x, y, x + w, y + h };


	// Output the text, center aligned
	pFont->DrawText(NULL, str, -1, &shit, DT_CENTER, color);


}


void D3DHook::vHUD(void)
{
	
	
	// create the vertices using the CUSTOMVERTEX struct
	CUSTOMVERTEX vertices[] =
	{
		//FIRST OUTLINE
		{ 100.0f, 100.0f, 0.5f, 1.0f, D3DCOLOR_XRGB(51, 43, 60), }, 
		{ 100.0f + lenght, 100.0f, 0.5f, 1.0f, D3DCOLOR_XRGB(51, 43, 60), },
		{ 100.0f, 100.0f + width, 0.5f, 1.0f, D3DCOLOR_XRGB(51, 43, 60), },

		{ 100.0f + lenght, 100.0f , 0.5f, 1.0f, D3DCOLOR_XRGB(51, 43, 60), },
		{ 100.0f + lenght, 100.0f + width, 0.5f, 1.0f, D3DCOLOR_XRGB(51, 43, 60), },
		{ 100.0f, 100.0f + width, 0.5f, 1.0f, D3DCOLOR_XRGB(51, 43, 60), },


		//SECOND OUTLINE
		{ 100.0f + width / 8 , 100.0f + width / 8, 0.5f, 1.0f, D3DCOLOR_XRGB(64, 65, 87), },
		{ 100.0f + lenght - width / 8, 100.0f + width / 8, 0.5f, 1.0f, D3DCOLOR_XRGB(64, 65, 87), },
		{ 100.0f + width / 8, 100.0f + width - width / 8, 0.5f, 1.0f, D3DCOLOR_XRGB(64, 65, 87), },

		{ 100.0f + lenght - width / 8 , 100.0f + width / 8, 0.5f, 1.0f, D3DCOLOR_XRGB(64, 65, 87), },
		{ 100.0f + lenght - width / 8, 100.0f + width - width / 8 , 0.5f, 1.0f, D3DCOLOR_XRGB(64, 65, 87), },
		{ 100.0f + width / 8, 100.0f + width - width / 8, 0.5f, 1.0f, D3DCOLOR_XRGB(64, 65, 87), },


		//BACKGROUND (NO-LIFE)
		{ 100.0f + 2 * width / 8 , 100.0f + 2 * width / 8, 0.5f, 1.0f, D3DCOLOR_XRGB(26, 26, 26), },
		{ 100.0f + lenght - 2 * width / 8, 100.0f + 2 * width / 8, 0.5f, 1.0f, D3DCOLOR_XRGB(26, 26, 26), },
		{ 100.0f + 2 * width / 8, 100.0f + width - 2 * width / 8, 0.5f, 1.0f, D3DCOLOR_XRGB(26, 26, 26), },

		{ 100.0f + lenght - 2 * width / 8 , 100.0f + 2 * width / 8, 0.5f, 1.0f, D3DCOLOR_XRGB(26, 26, 26), },
		{ 100.0f + lenght - 2 * width / 8, 100.0f + width - 2 * width / 8 , 0.5f, 1.0f, D3DCOLOR_XRGB(26, 26, 26), },
		{ 100.0f + 2 * width / 8, 100.0f + width - 2 * width / 8, 0.5f, 1.0f, D3DCOLOR_XRGB(26, 26, 26), },


		//HEALTH (GREEN)
		{ 100.0f + 2 * width / 8 , 100.0f + 2 * width / 8, 0.5f, 1.0f, D3DCOLOR_XRGB(136, 211, 103), },
		{ 100.0f + llife, 100.0f + 2 * width / 8, 0.5f, 1.0f, D3DCOLOR_XRGB(136, 211, 103), },
		{ 100.0f + 2 * width / 8, 100.0f + width - 2 * width / 8, 0.5f, 1.0f, D3DCOLOR_XRGB(136, 211, 103), },

		{ 100.0f + llife , 100.0f + 2 * width / 8, 0.5f, 1.0f, D3DCOLOR_XRGB(136, 211, 103), },
		{ 100.0f + llife, 100.0f + width - 2 * width / 8 , 0.5f, 1.0f, D3DCOLOR_XRGB(136, 211, 103), },
		{ 100.0f + 2 * width / 8, 100.0f + width - 2 * width / 8, 0.5f, 1.0f, D3DCOLOR_XRGB(136, 211, 103), }


	};
	// create a vertex buffer interface called m_vbuffer
	d3ddev->CreateVertexBuffer(24 * sizeof(CUSTOMVERTEX),
		0,
		CUSTOMFVF,
		D3DPOOL_MANAGED,
		&m_vbuffer,
		NULL);

	VOID* pVoid;    // a void pointer

					// lock m_vbuffer and load the vertices into it
	m_vbuffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, vertices, sizeof(vertices));
	m_vbuffer->Unlock();
}