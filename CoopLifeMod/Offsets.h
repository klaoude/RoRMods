#pragma once
#include <vector>
#include <Windows.h>

std::vector<LPVOID> health_offsets = 
{ 
	(LPVOID)0x005AE468,
	(LPVOID)0x0,
	(LPVOID)0x0,
	(LPVOID)0x4,
	(LPVOID)0x3A0
};

std::vector<LPVOID> max_health_offsets = 
{
	(LPVOID)0x005AE468,
	(LPVOID)0x0,
	(LPVOID)0x0,
	(LPVOID)0x4,
	(LPVOID)0x390
};

std::vector<LPVOID> portClientOffsets = 
{
	(LPVOID)0x0034E464,
	(LPVOID)0x26C,
	(LPVOID)0xC,
	(LPVOID)0xC,
	(LPVOID)0x4,
	(LPVOID)0xC0,
};

std::vector<LPVOID> portServerOffsets = 
{
	(LPVOID)0x0034E464,
	(LPVOID)0x26C,
	(LPVOID)0xC,
	(LPVOID)0xC,
	(LPVOID)0x4,
	(LPVOID)0x930
};

std::vector<LPVOID> ipOffsets = 
{
	(LPVOID)0x0034E464,
	(LPVOID)0x26C,
	(LPVOID)0xC,
	(LPVOID)0xC,
	(LPVOID)0x4,
	(LPVOID)-0x3DE3E54
};