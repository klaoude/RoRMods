#pragma once
#include <Windows.h>
#include <iostream>
#include <tchar.h>
#include <psapi.h>
#include <vector>
#include <TlHelp32.h>

#define UNINITIALIZED 0xFFFFFFFF

class Memory
{
public:
	bool Open(LPCSTR windowName);
	HMODULE GetProcessAddr();
	LPVOID GetBasePointer(LPVOID offset = 0x0);

	double GetDouble(std::vector<LPVOID> offsets);
	double GetDouble(LPVOID addr);

	char* getChar(LPVOID addr, int size);
	char* getChar(std::vector<LPVOID> offsets, int size);

	LPVOID GetAddr(std::vector<LPVOID> offsets);

	void WriteMem(double val, std::vector<LPVOID> offsets);

	LPVOID addLPVOID(LPVOID nb1, LPVOID nb2) { return (LPVOID)((int)nb1 + (int)nb2); }
private:
	HWND m_window;
	DWORD m_pID;
	HANDLE m_handle;
};