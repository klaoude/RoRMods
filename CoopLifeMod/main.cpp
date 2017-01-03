#include "Mods.h"

int s_width = 800;
int s_height = 600;
HWND hWnd;
const MARGINS  margin = { 0,0,s_width,s_height };
const char* value = "Risk of Rain";

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void WinApiInit(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

char* ReadString(char* szSection, char* szKey, const char* szDefaultValue)
{
	char* szResult = new char[255];
	memset(szResult, 0x00, 255);
	GetPrivateProfileString(szSection, szKey,
		szDefaultValue, szResult, 255, "./Resources/config.ini");
	return szResult;
}

int ReadInt(char* szSection, char* szKey, int iDefaultValue)
{
	int iResult = GetPrivateProfileInt(szSection, szKey, iDefaultValue, "./Resources/config.ini");
	return iResult;
}

int WINAPI WinMain(HINSTANCE hInstance,	HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WinApiInit(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

	MSG msg;
	Log log("log.log");
	Mods mod(&log, hWnd, s_width, s_height);

	log.write("---Initialisation---");
	log.write("--Reading value in config.ini--");

	std::string pseudo, ip;
	pseudo = ReadString("options", "nickname", "no nickname"); //read pseudo from .ini
	ip = ReadString("options", "ip", "ip not defined"); //read ip from .ini

	log.write("Pseudo = " + pseudo);
	log.write("ip = " + ip);

	std::string host, join, solo, toggle, statup, statdown, quit, cyclelifetext, font;
	host = ReadString("keys", "host", "F1");
	join = ReadString("keys", "join", "F2");
	solo = ReadString("keys", "solo", "F3");
	toggle = ReadString("keys", "toggle", "F8");
	statup = ReadString("keys", "statup", "PGUP");
	statdown = ReadString("keys", "statdown", "PGDOWN");
	quit = ReadString("keys", "quit", "F9");
	cyclelifetext = ReadString("keys", "cyclelifetext", "F5");
	font = ReadString("keys", "font", "F6");

	log.write("Reading Keys");

	int decdmg, decrate, deccrit, decregen, decstrength, decspeed, decleaf;
	decdmg = ReadInt("decimals", "damage", 2);
	decrate = ReadInt("decimals", "attackSpeed", 2);
	deccrit = ReadInt("decimals", "crit", 2);
	decregen = ReadInt("decimals", "regen", 2);
	decstrength = ReadInt("decimals", "strength", 2);
	decspeed = ReadInt("decimals", "speed", 2);
	decleaf = ReadInt("decimals", "cloverChance", 2);

	log.write("Reading decimals");

	int iddmg, idrate, idcrit, idregen, idstrength, idspeed, idleaf;
	iddmg = ReadInt("index", "damage", 0);
	idrate = ReadInt("index", "attackSpeed", 0);
	idcrit = ReadInt("index", "crit", 0);
	idregen = ReadInt("index", "regen", 0);
	idstrength = ReadInt("index", "strength", 0);
	idspeed = ReadInt("index", "speed", 0);
	idleaf = ReadInt("index", "cloverChance", 0);

	log.write("Reading ids");
	log.write("--Reading Done--");
	log.write("--Setting config--");
	
	mod.setkeys(mod.stk(host), mod.stk(join), mod.stk(solo), mod.stk(toggle), mod.stk(statup), mod.stk(statdown), mod.stk(quit), mod.stk(cyclelifetext), mod.stk(font));

	mod.setDec(decdmg, decrate, deccrit, decregen, decstrength, decspeed, decleaf);
	mod.setId(iddmg, idrate, idcrit, idregen, idstrength, idspeed, idleaf);

	mod.setIP(ip);
	mod.setPseudo(pseudo);
	
	log.write("--Setting Done--");

	mod.Init();

	while (TRUE)
	{			
		mod.Loop();

		RECT rc;
		HWND wnd = FindWindow("YYGameMakerYY", value);
		if (wnd != NULL && wnd == GetForegroundWindow())
		{
			GetWindowRect(wnd, &rc);

			mod.getHook()->setHeight(rc.bottom - rc.top);
			mod.getHook()->setWidth(rc.right - rc.left);

			mod.getHook()->setMod(true);
		} 
		else if(wnd != GetForegroundWindow())
		{
			GetWindowRect(wnd, &rc);

			mod.getHook()->setMod(false);
		}

		SetWindowPos(hWnd, HWND_TOPMOST, rc.left, rc.top, 0, 0, SWP_NOSIZE);
		
		if (!FindWindow("YYGameMakerYY", value))
			ExitProcess(1337);	
		
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			mod.Stop();
			exit(0);
		}			

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

void WinApiInit(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	RECT rc;

	HWND newhwnd = FindWindow("YYGameMakerYY", value);
	if (newhwnd != NULL)
	{
		GetWindowRect(newhwnd, &rc);
		s_width = rc.right - rc.left;
		s_height = rc.bottom - rc.top;
	}
	else
		ExitProcess(42);
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
	wc.lpszClassName = "WindowClass";

	RegisterClassEx(&wc);

	/*hWnd = CreateWindowEx(0,
		"WindowClass",
		"",
		WS_EX_TOPMOST | WS_POPUP,
		rc.left, rc.top,
		s_width, s_height,
		NULL,
		NULL,
		hInstance,
		NULL);*/

	hWnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW, "WindowClass", "", WS_POPUP, rc.left, rc.top, s_width, s_height, 0, 0, 0, 0);

	SetLayeredWindowAttributes(hWnd, 0, 1.0f, LWA_ALPHA);
	SetLayeredWindowAttributes(hWnd, 0, RGB(0, 0, 0), LWA_COLORKEY);

	SetWindowLong(hWnd, GWL_EXSTYLE, (int)GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
	//SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 255, ULW_COLORKEY);
	//SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);

	ShowWindow(hWnd, nCmdShow);

	::SetWindowPos(FindWindow("YYGameMakerYY", value), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	SetForegroundWindow(FindWindow("YYGameMakerYY", value));
}