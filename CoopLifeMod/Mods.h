#pragma once

#include <map>

#include "Net.h"
#include "Memory.h"

typedef std::map<double, int> CounterMap;

class Mods
{
public:
	Mods(HWND hWnd, int width, int height);

	void Init();

	void Loop();

	void Stop();

	void setIP(std::string val) { m_ipAddr = val; }
	void setPseudo(std::string val) { m_pseudo = val; }

private:
	void ShowHUD();
	void GetStats();

	void fixHealth();
	void fixStat(std::vector<double>& vector, double& stats);
	void fixStat(std::vector<double>& vector, float& stats);
	void fixStat(std::vector<double>& vector, int& stats);

	HWND m_hWnd;
	D3DHook* m_hook;
	Memory m_mem;
	Net* m_net;

	Stats m_stats;

	//Fix
	std::vector<double> m_maxHealths;
	std::vector<double> m_attackSpeeds;
	std::vector<double> m_damages;
	std::vector<double> m_regens;
	std::vector<double> m_strengths;
	std::vector<double> m_crits;
	std::vector<double> m_levels;
	std::vector<double> m_items;
	CounterMap m_counts;
	CounterMap::iterator m_it;
	std::string m_ipAddr, m_pseudo;
	int m_stableHealth = 1;
	bool m_isConnect = false;
	bool m_isServer = false;
};

