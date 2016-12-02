#pragma once

#include <winsock2.h>
#include <windows.h>
#include <d3d9.h>
#include <iostream>
#include <d3dx9.h>
#include <Dwmapi.h> 
#include <TlHelp32.h>
#include <vector>
#include <sstream>
#include <iomanip>
#include <stdio.h>
#include <fstream>

class Log
{
public:
	Log() {}
	Log(std::string filePath);
	~Log();

	void write(std::stringstream ss);
	void write(std::string s);

private:
	std::ofstream m_file;

	std::string m_path;
};