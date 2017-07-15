#pragma once

#include <map>

#include "Net.h"
#include "Memory.h"

typedef std::map<double, int> CounterMap;

class Mods
{
public:
	Mods(Log* log, HWND hWnd, int width, int height);

	void Init();

	void Loop();

	void Stop();

	void setIP(std::string val) { m_ipAddr = val; }
	void setPseudo(std::string val) { m_pseudo = val; }
	int stk(std::string input); //string to key
	void setkeys(int host, int join, int solo, int toggle, int statup, int statdown, int quit, int cyclelifetext, int font);
	void setDec(int dmg, int rate, int crit, int regen, int strength, int speed, int leaf);
	void setId(int dmg, int rate, int crit, int regen, int strength, int speed, int leaf);

	D3DHook* getHook() { return m_hook; }

private:
	void ShowHUD();
	void GetStats();

	void fixHealth();
	void fixStat(std::vector<double>& vector, double& stats);
	void fixStat(std::vector<double>& vector, float& stats);
	void fixStat(std::vector<double>& vector, int& stats);
	
	Log* m_log;
	HWND m_hWnd;
	D3DHook* m_hook;
	Memory m_mem;
	Net* m_net;

	Stats m_stats;

	int m_leaf;

	//Fix
	std::vector<double> m_maxHealths;
	std::vector<double> m_attackSpeeds;
	std::vector<double> m_damages;
	std::vector<double> m_regens;
	std::vector<double> m_strengths;
	std::vector<double> m_crits;
	std::vector<double> m_speeds;
	std::vector<double> m_levels;
	std::vector<double> m_items;
	std::vector<double> m_leafs;
	CounterMap m_counts;
	CounterMap::iterator m_it;
	std::string m_ipAddr, m_pseudo;
	int m_stableHealth = 1;
	bool m_isConnect = false;
	bool m_isServer = false;

	int m_host, m_join, m_solo, m_toggle, m_statup, m_statdown, m_quit, m_cyclelifetext, m_font;
};

