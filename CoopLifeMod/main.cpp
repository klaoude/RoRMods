#include "D3DHook.h"
#include "Memory.h"
#include <sstream>

int s_width = 800;
int s_height = 600;
HWND hWnd;
const MARGINS  margin = { 0,0,s_width,s_height };

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

	int health = 0, maxHealth = 0;
	std::vector<LPVOID> health_offsets;
	health_offsets.push_back((LPVOID)0x005A9B38);
	health_offsets.push_back((LPVOID)0x2F0);
	health_offsets.push_back((LPVOID)0x4);
	health_offsets.push_back((LPVOID)0x3A0);
	//health_offsets.push_back((LPVOID)0x3A0);

	std::vector<LPVOID> max_health_offsets;
	max_health_offsets.push_back((LPVOID)0x005AE468);
	max_health_offsets.push_back((LPVOID)0x0);
	max_health_offsets.push_back((LPVOID)0x0);
	max_health_offsets.push_back((LPVOID)0x4);
	max_health_offsets.push_back((LPVOID)0x390);

	int frame = 0;
	int fps = 10;
	std::stringstream s;
	std::string healthStr;
	int stableHealth = 1;
	int oldMax = 1;

	while (TRUE)
	{				
		//mem.WriteMem(90, offsets);
		oldMax = maxHealth;
		//health = (int)mem.GetDouble(health_offsets);
		//maxHealth = (int)mem.GetDouble(max_health_offsets);
		health = (int)mem.GetDouble((LPVOID)0x03F306C8);

		if (maxHealth < oldMax)
			maxHealth = oldMax;

		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

		if (!FindWindow(NULL, value))
			ExitProcess(1337);
			
		s.str("");

		if (health >= 1 && health < 10000)
		{
			s << health << "/" << maxHealth;
			stableHealth = health;
		}
		else
			s << stableHealth << "/" << maxHealth;


		if (frame >= fps)
		{
			hook.render((char*)(s.str().c_str()), health, maxHealth);
			frame = 0;
		}	
		
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			exit(0);

		frame++;
		Sleep(10);
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