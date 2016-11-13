#include "D3DHook.h"

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

	D3DXCreateFont(m_d3ddev, 20, 0, FW_BOLD, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &m_pFont);

}

void D3DHook::render(char* str)
{
	// clear the window alpha
	m_d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

	m_d3ddev->BeginScene();    // begins the 3D scene

	drawString(10, 100, D3DCOLOR_ARGB(255, 255, 255, 0), m_pFont, str);

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
