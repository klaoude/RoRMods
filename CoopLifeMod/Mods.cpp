#include "Mods.h"
#include "Offsets.h"

Mods::Mods(HWND hWnd, int width, int height) : m_hWnd(hWnd), m_hook(new D3DHook(width, height))
{}

void Mods::Init()
{
	m_hook->setSolo(false);
	
	Player swag;
	Stats stat;

	stat.maxHealth = -1;
	std::vector<Player> moreSwag;
	moreSwag.push_back(swag);
	Data swagOverflow = { moreSwag };
	m_hook->setStat(stat);
	m_hook->setStats(swagOverflow);
	m_hook->setMod(1);
	m_hook->initD3D(m_hWnd);
	m_net = new Net(m_hook);

	m_mem.Open("Risk of Rain");

	m_hook->setPseudo(m_pseudo);
}

void Mods::Loop()
{
	if (m_isConnect)
	{	
		int pause = m_mem.GetDouble(pause_offsets);
		
		ShowHUD();

		Player p;
		p.stats = m_stats;
		p.pseudo = m_pseudo;

		if (!m_isServer)
		{			
			m_net->sendInfo(p);
			Data dt = m_net->recvData();
			m_hook->setStats(dt);
			m_hook->setpause(pause);
		}
		else
		{
			m_net->recvAllInfo();
			m_net->addInfo(p);
			m_net->broadcastData();
			m_hook->setStats(m_net->getData());
			m_hook->setpause(pause);
		}		

		if (GetAsyncKeyState(m_statup))
			m_hook->setpSel((m_hook->getpSel() - 1) % m_hook->getStats().players.size()); //select next player
		if (GetAsyncKeyState(m_statdown))
			m_hook->setpSel((m_hook->getpSel() + 1) % m_hook->getStats().players.size());	//select previous player
	}
	else
	{
		if (GetAsyncKeyState(m_host))
		{
			m_net->create(m_mem.GetDouble(portServerOffsets) + 1);
			m_isConnect = true;
			m_hook->setIsCon(m_isConnect);
			m_hook->setSolo(false);
			m_isServer = true;
		}
		else if (GetAsyncKeyState(m_join))
		{
			char* ip = m_mem.getChar(ipOffsets, 15);
			std::string ipStr = std::string(ip);
			ipStr.resize(15);
			m_net->conn(m_ipAddr, m_mem.GetDouble(portClientOffsets) + 1);
			m_isConnect = true;
			m_hook->setIsCon(m_isConnect);
			m_hook->setSolo(false);
		}		
	}

	if (GetAsyncKeyState(m_solo))
	{
		m_hook->setSolo(true);
		GetStats();
	}

	if (GetAsyncKeyState(m_toggle))
	{
		m_hook->getMod() ? m_hook->setMod(0) : m_hook->setMod(1);
	}

	if (m_hook->getSolo())
		ShowHUD();

	m_hook->render();
}

void Mods::Stop()
{
	delete m_net;
	delete m_hook;
}

void Mods::ShowHUD()
{
	GetStats();

	fixHealth();
	fixStat(m_maxHealths, m_stats.maxHealth);
	fixStat(m_damages, m_stats.damage);
	fixStat(m_regens, m_stats.regeneration);
	fixStat(m_crits, m_stats.critical);
	fixStat(m_attackSpeeds, m_stats.attackSpeed);
	fixStat(m_strengths, m_stats.strength);
	fixStat(m_levels, m_stats.level);
	fixStat(m_items, m_stats.item);
	fixStat(m_speeds, m_stats.speed);

	m_hook->setStat(m_stats);
	m_hook->setplStats(m_stats);
}

void Mods::GetStats()
{
	m_stats.health = m_mem.GetDouble(health_offsets);
	m_stats.maxHealth = m_mem.GetDouble(max_health_offsets);

	m_stats.damage = m_mem.GetDouble(damage_offsets);
	m_stats.attackSpeed = m_mem.GetDouble(attackSpeed_offsets);
	m_stats.strength = m_mem.GetDouble(resistance_offsets);
	m_stats.regeneration = m_mem.GetDouble(regeneration_offsets);
	m_stats.critical = m_mem.GetDouble(crit_offsets);
	m_stats.speed = m_mem.GetDouble(speed_offsets);

	m_stats.item = m_mem.GetDouble(item_offsets);
	m_stats.level = m_mem.GetDouble(level_offsets);
}

void Mods::fixHealth()
{
	if (m_stats.health >= 1 && m_stats.health < 100000)
		m_stableHealth = m_stats.health;
	else
		m_stats.health = m_stableHealth;
}

void Mods::fixStat(std::vector<double>& vector, double& stats)
{
	if (vector.size() > 20)
		vector.erase(vector.begin());

	if (stats > 0)
		vector.push_back(stats);

	if (vector.size() > 19)
	{
		m_counts.clear();
		for (int i = 0; i < vector.size(); ++i)
		{
			m_it = m_counts.find(vector[i]);

			if (m_it != m_counts.end())
				m_it->second++;
			else
				m_counts[vector[i]] = 1;
		}

		int max = 0;
		int pos = 0;
		int i = 0;
		for (auto it = m_counts.begin(); it != m_counts.end(); it++)
		{
			if (it->second > max)
			{
				max = it->second;
				pos = i;
			}
			i++;
		}

		m_it = m_counts.begin();
		for (auto j = 0; j < pos; j++)
			m_it++;
		stats = m_it->first;
		return;
	}
	if (stats == m_stats.strength)
		stats = 0;
}

void Mods::fixStat(std::vector<double>& vector, float& stats)
{
	if (vector.size() > 20)
		vector.erase(vector.begin());

	if (stats > 0)
		vector.push_back(stats);

	if (vector.size() > 19)
	{
		m_counts.clear();
		for (int i = 0; i < vector.size(); ++i)
		{
			m_it = m_counts.find(vector[i]);

			if (m_it != m_counts.end())
				m_it->second++;
			else
				m_counts[vector[i]] = 1;
		}

		int max = 0;
		int pos = 0;
		int i = 0;
		for (auto it = m_counts.begin(); it != m_counts.end(); it++)
		{
			if (it->second > max)
			{
				max = it->second;
				pos = i;
			}
			i++;
		}

		m_it = m_counts.begin();
		for (auto j = 0; j < pos; j++)
			m_it++;
		stats = m_it->first;
	}
}

void Mods::fixStat(std::vector<double>& vector, int& stats)
{
	if (vector.size() > 20)
		vector.erase(vector.begin());

	if (stats == m_stats.item)
		if(stats >= -1 || stats >=1)
			vector.push_back(stats+1);
		else
			vector.push_back(stats);
	else
		if (stats > 0)
			vector.push_back(stats);

	if (vector.size() > 19)
	{
		m_counts.clear();
		for (int i = 0; i < vector.size(); ++i)
		{
			m_it = m_counts.find(vector[i]);

			if (m_it != m_counts.end())
				m_it->second++;
			else
				m_counts[vector[i]] = 1;
		}

		int max = 0;
		int pos = 0;
		int i = 0;
		for (auto it = m_counts.begin(); it != m_counts.end(); it++)
		{
			if (it->second > max)
			{
				max = it->second;
				pos = i;
			}
			i++;
		}

		m_it = m_counts.begin();
		for (auto j = 0; j < pos; j++)
			m_it++;
		stats = m_it->first;
	}
}


void Mods::setkeys(int host, int join, int solo, int toggle, int statup, int statdown)
{
	m_host = host;
	m_join = join;
	m_solo = solo;
	m_toggle = toggle;
	m_statup = statup;
	m_statdown = statdown;
}


int Mods::stk(std::string input)
{

	if (input == "F1")
		return VK_F1;


	else if (input == "F2")
		return VK_F2;


	else if (input == "F3")
		return VK_F3;


	else if (input == "F4")
		return VK_F4;


	else if (input == "F5")
		return VK_F5;


	else if (input == "F6")
		return VK_F6;


	else if (input == "F7")
		return VK_F7;


	else if (input == "F8")
		return VK_F8;


	else if (input == "F9")
		return VK_F9;


	else if (input == "F10")
		return VK_F10;


	else if (input == "F11")
		return VK_F11;


	else if (input == "F12")
		return VK_F12;


	else if (input == "PGUP")
		return VK_PRIOR;


	else if (input == "PGDOWN")
		return VK_NEXT;


	else if (input == "NUM1")
		return VK_NUMPAD1;

	else if (input == "NUM2")
		return VK_NUMPAD2;

	else if (input == "NUM3")
		return VK_NUMPAD3;

	else if (input == "NUM4")
		return VK_NUMPAD4;

	else if (input == "NUM5")
		return VK_NUMPAD5;

	else if (input == "NUM6")
		return VK_NUMPAD6;

	else if (input == "NUM7")
		return VK_NUMPAD7;

	else if (input == "NUM8")
		return VK_NUMPAD8;

	else if (input == "NUM9")
		return VK_NUMPAD9;

	else if (input == "NUM0")
		return VK_NUMPAD0;


	else if (input == "TAB")
		return VK_TAB;


	else
		return int(input[0]);

}

void Mods::setDec(int dmg, int rate, int crit, int regen, int strength, int speed, int leaf)
{
	m_hook->setDec(dmg, rate, crit, regen, strength, speed, leaf);
}

void Mods::setId(int dmg, int rate, int crit, int regen, int strength, int speed, int leaf)
{
	m_hook->setId(dmg, rate, crit, regen, strength, speed, leaf);
}