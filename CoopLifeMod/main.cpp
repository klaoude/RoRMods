#include "D3DHook.h"
#include "Memory.h"
#include <sstream>

int s_width = 800;
int s_height = 600;
#define CENTERX (GetSystemMetrics(SM_CXSCREEN)/2)-(s_width/2)
#define CENTERY (GetSystemMetrics(SM_CYSCREEN)/2)-(s_height/2)
HWND hWnd;
const MARGINS  margin = { 0,0,s_width,s_height };
LPD3DXFONT pFont;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	D3DHook hook(s_width, s_height);
	char * value = "Risk of Rain";

	RECT rc;

	HWND newhwnd = FindWindow(NULL, value);
	if (newhwnd != NULL)
	{
		GetWindowRect(newhwnd, &rc);
		s_width = rc.right - rc.left;
		s_height = rc.bottom - rc.top;
	}
	else
		ExitProcess(0);
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)RGB(0, 0, 0);
	wc.lpszClassName = "WindowClass";

	RegisterClassEx(&wc);

	hWnd = CreateWindowEx(0,
		"WindowClass",
		"",
		WS_EX_TOPMOST | WS_POPUP,
		rc.left, rc.top,
		s_width, s_height,
		NULL,
		NULL,
		hInstance,
		NULL);

	SetWindowLong(hWnd, GWL_EXSTYLE, (int)GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT);
	SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 0, ULW_COLORKEY);
	SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);

	ShowWindow(hWnd, nCmdShow);


	hook.initD3D(hWnd);
	MSG msg;
	::SetWindowPos(FindWindow(NULL, value), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	Memory mem;
	mem.Open("Risk of Rain");

	double health;
	std::vector<LPVOID> offsets;
	offsets.push_back((LPVOID)0x005AE468);
	offsets.push_back((LPVOID)0x0);
	offsets.push_back((LPVOID)0x0);
	offsets.push_back((LPVOID)0x4);
	offsets.push_back((LPVOID)0x3A0);

	int frame = 0;
	int fps = 60;
	std::string str = "MyLife: ";

	while (TRUE)
	{		
		health = mem.GetDouble(offsets);
		::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

		if (!FindWindow(NULL, value))
			ExitProcess(1337);
		
		std::stringstream s;
		std::string healthStr = std::to_string(health);				

		if (health > 0 && health < 10000)
		{				
			s << str << healthStr;
		}

		if (GetActiveWindow() == FindWindow(NULL, value))
		{
			if (frame >= fps)
			{
				hook.render((char*)(s.str().c_str()));
				frame = 0;
			}
		}		
		
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			exit(0);

		frame++;
	}

	return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		DwmExtendFrameIntoClientArea(hWnd, &margin);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}