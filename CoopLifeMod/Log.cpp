#include "Log.h"

Log::Log(std::string filePath)
{
	m_path = filePath;
	m_file.open(filePath);
	m_file.close();
}

Log::~Log()
{
	m_file.close();
}

void Log::write(std::string s)
{
	m_file.open(m_path.c_str(), std::ios::out | std::ios::app);
	m_file << s << std::endl;
	m_file.close();	
}

void Log::write(std::stringstream ss)
{
	write(ss.str());
}