#include "Memory.h"

bool Memory::Open(LPCSTR windowName)
{
	m_window = FindWindow(0, windowName);
	if (m_window == 0)
	{
		std::cerr << "[Memory] [ERROR] Can't find window with name : " << windowName << std::endl;
		return false;
	}

	GetWindowThreadProcessId(m_window, &m_pID);
	std::wcout << "[Memory] [INFO] " << windowName << " pID is : " << m_pID << std::endl;

	m_handle = OpenProcess(PROCESS_ALL_ACCESS, 0, m_pID);
	if (m_handle == NULL)
	{
		std::cerr << "[Memory] [ERROR] Can't open pID : " << m_pID << std::endl;
		return false;
	}

	return true;
}

HMODULE Memory::GetProcessAddr()
{
	DWORD ret = UNINITIALIZED;
	HANDLE moduleSnapshotHandle_ = INVALID_HANDLE_VALUE;
	MODULEENTRY32 moduleEntry_;

	moduleSnapshotHandle_ = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_pID);

	if (moduleSnapshotHandle_ == INVALID_HANDLE_VALUE)
	{
		std::cout << "Module Snapshot error" << std::endl;
		return (HMODULE)ret;
	}

	moduleEntry_.dwSize = sizeof(MODULEENTRY32);

	if (!Module32First(moduleSnapshotHandle_, &moduleEntry_))
	{
		std::cout << "First module not found" << std::endl;
		CloseHandle(moduleSnapshotHandle_);
		return (HMODULE)ret;
	}

	while (ret == UNINITIALIZED)
	{
		do
		{
			if (!strcmp(moduleEntry_.szModule, "Risk of Rain.exe"))
			{
				ret = (unsigned int)moduleEntry_.modBaseAddr;
				break;
			}
		} while (Module32Next(moduleSnapshotHandle_, &moduleEntry_));
	}

	CloseHandle(moduleSnapshotHandle_);

	return (HMODULE)ret;

	/*HMODULE *lphModule;
	int count = 1;
	DWORD cb = sizeof(HMODULE) * count;
	DWORD cbNeeded;

	do
	{
		lphModule = new HMODULE [++count];
		cb = sizeof(HMODULE) * count;
		EnumProcessModulesEx(m_handle, lphModule, cb, &cbNeeded, LIST_MODULES_ALL);
	} while (cbNeeded > cb);

	HMODULE baseAddress = lphModule[0];

	delete[] lphModule;

	std::cout << "[Memory] [INFO] Process Address = 0x" << (LPVOID)baseAddress << std::endl;

	return baseAddress;*/
}

LPVOID Memory::GetBasePointer(LPVOID offset/* = 0x0*/)
{
	LPVOID procAddr = GetProcessAddr();
	if (procAddr == NULL)
		return NULL;

	LPVOID buffer;
	ReadProcessMemory(m_handle, addLPVOID(procAddr, offset), &buffer, sizeof(LPVOID), 0);
	std::cout << "[Memory] [INFO] 0x" << addLPVOID(procAddr, offset) << " -> 0x" << buffer << std::endl;

	return buffer;
}

double Memory::GetDouble(std::vector<LPVOID> offsets)
{
	LPVOID tmpBuffer;
	double ret;

	LPVOID bp = (LPVOID)GetBasePointer(offsets[0]);

	for (auto i = 1; i < offsets.size() - 1; i++)
	{
		ReadProcessMemory(m_handle, addLPVOID((i == 1 ? bp : tmpBuffer), offsets[i]), &tmpBuffer, sizeof(offsets[i]), 0);
		std::cout << "[Memory] [INFO] 0x" << addLPVOID((i == 1 ? bp : tmpBuffer), offsets[i]) << " -> 0x" << tmpBuffer << std::endl;
	}

	ReadProcessMemory(m_handle, addLPVOID(tmpBuffer, offsets[offsets.size() - 1]), &ret, sizeof(double), 0);
	std::cout << "[Memory] [INFO] 0x" << offsets[offsets.size() - 1] << " -> " << ret << std::endl;

	return ret;
}

double Memory::GetDouble(LPVOID addr)
{
	double ret; 
	ReadProcessMemory(m_handle, addr, &ret, sizeof(ret), 0);
	return ret;
}

char* Memory::getChar(LPVOID addr, int size)
{
	char* ret = new char[size];
	ReadProcessMemory(m_handle, addr, &ret[0], 2, 0);
	return ret;
}

char* Memory::getChar(std::vector<LPVOID> offsets, int size)
{
	LPVOID tmpBuffer = (LPVOID)UNINITIALIZED;
	char* ret = new char[size];

	LPVOID bp = (LPVOID)GetBasePointer(offsets[0]);

	ReadProcessMemory(m_handle, addLPVOID(GetProcessAddr(), offsets[0]), &ret[0], 2, 0);

	return (char*)addLPVOID(GetProcessAddr(), offsets[0]);

	/*for (auto i = 1; i < offsets.size() - 1; i++)
	{
		ReadProcessMemory(m_handle, addLPVOID((i == 1 ? bp : tmpBuffer), offsets[i]), &tmpBuffer, sizeof(offsets[i]), 0);
		std::cout << "[Memory] [INFO] 0x" << addLPVOID((i == 1 ? bp : tmpBuffer), offsets[i]) << " -> 0x" << tmpBuffer << std::endl;
	}

	ReadProcessMemory(m_handle, addLPVOID(bp, offsets[offsets.size() - 1]), &ret[0], 2, 0);
	std::cout << "[Memory] [INFO] 0x" << offsets[offsets.size() - 1] << " -> " << ret << std::endl;*/

	return ret;
}

LPVOID Memory::GetAddr(std::vector<LPVOID> offsets)
{
	LPVOID tmpBuffer;
	double ret;

	LPVOID bp = (LPVOID)GetBasePointer(offsets[0]);

	for (auto i = 1; i < offsets.size() - 1; i++)
	{
		ReadProcessMemory(m_handle, addLPVOID((i == 1 ? bp : tmpBuffer), offsets[i]), &tmpBuffer, sizeof(offsets[i]), 0);
		std::cout << "[Memory] [INFO] 0x" << addLPVOID((i == 1 ? bp : tmpBuffer), offsets[i]) << " -> 0x" << tmpBuffer << std::endl;
	}

	return tmpBuffer;
}

void Memory::WriteMem(double val, std::vector<LPVOID> offsets)
{
	double ret;
	WriteProcessMemory(m_handle, GetAddr(offsets), &ret, sizeof(ret), NULL);
}