#include "Network.h"

void split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss;
	ss.str(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
}

std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

void Packer(std::stringstream& ss, Player p)
{
	ss << p.isConnected;
	ss << "|";
	ss << p.stats.health;
	ss << "|";
	ss << p.stats.maxHealth;
	ss << "|";
	ss << p.stats.damage;
	ss << "|";
	ss << p.stats.attackSpeed;
	ss << "|";
	ss << p.stats.critical;
	ss << "|";
	ss << p.stats.regeneration;
	ss << "|";
	ss << p.stats.strength;
	ss << "|";
	ss << p.stats.level;
	ss << "|";
	ss << p.stats.item;
	ss << "|";
	ss << p.pseudo;
	ss << "|";
	ss << p.stats.speed;
}

void dePackerize(Player& player, std::vector<std::string> infoSplited)
{
	player.isConnected = true;//? infoSplited[0] == "204" : false;
	player.stats.health = atof(infoSplited[1].c_str());
	player.stats.maxHealth = atof(infoSplited[2].c_str());
	player.stats.damage = atof(infoSplited[3].c_str());
	player.stats.attackSpeed = atof(infoSplited[4].c_str());
	player.stats.critical = atof(infoSplited[5].c_str());
	player.stats.regeneration = atof(infoSplited[6].c_str());
	player.stats.strength = atof(infoSplited[7].c_str());
	player.stats.level = atof(infoSplited[8].c_str());
	player.stats.item = atof(infoSplited[9].c_str());
	player.pseudo = infoSplited[10];
	player.stats.speed = atof(infoSplited[11].c_str());
}

void Net::create(int port)
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);

	m_server.socket = socket(AF_INET, SOCK_STREAM, 0);
	m_server.addr.sin_addr.s_addr = htonl(INADDR_ANY);
	m_server.addr.sin_family = AF_INET;
	m_server.addr.sin_port = htons(port);
	m_isServer = true;

	std::stringstream ss;
	ss << "Server created ! (port:";
	ss << port;
	ss << ")";
	std::string str = ss.str();
	str.resize(29);

	m_threads.push_back(std::thread(&Net::ServerThread, this));
	//m_log->write("---[Net]Create ended---");
}

void Net::ServerThread()
{
	while (1)
	{
		bind(m_server.socket, (SOCKADDR*)&m_server.addr, sizeof(m_server.addr));
		listen(m_server.socket, 0);

		Client cl;
		int sizeof_csin = sizeof(cl.addr);

		cl.socket = accept(m_server.socket, (SOCKADDR*)&cl.addr, &sizeof_csin);
		if (cl.socket != INVALID_SOCKET)
		{
			m_clients.push_back(cl);
		}
	}
}

void Net::conn(std::string ip, int port)
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);

	TIMEVAL Timeout;
	Timeout.tv_sec = 5;
	Timeout.tv_usec = 0;

	m_server.socket = socket(AF_INET, SOCK_STREAM, 0);

	m_server.addr.sin_family = AF_INET;
	m_server.addr.sin_addr.s_addr = inet_addr(ip.c_str());
	m_server.addr.sin_port = htons(port);

	if (connect(m_server.socket, (SOCKADDR*)&m_server.addr, sizeof(m_server.addr)) == SOCKET_ERROR)
	{
		return;
	}

	m_isServer = false;
	//m_log->write("---[Net]Conn ended---");
}

void Net::sendInfo(Player p)
{
	char buffer[1024];

	std::string dataStr;
	std::stringstream ss;

	Packer(ss, p);

	dataStr = ss.str();

	strcpy(buffer, dataStr.c_str());

	send(m_server.socket, buffer, sizeof(buffer), 0);
}

void Net::recvAllInfo()
{
	m_data.players.clear();
	for (auto i = 0; i < m_clients.size(); i++)
	{
		Player p = recvInfo(m_clients[i]);
		if (p.isConnected)
			m_data.players.push_back(p);
		else
			m_clients.erase(m_clients.begin() + i);
	}
}

Player Net::recvInfo(Client c)
{
	char buffer[1024];

	if (recv(c.socket, buffer, sizeof(buffer), 0) < 0)
	{
		Player p;
		p.isConnected = false;
		return p;
	}
	else
	{
		Player player;

		std::string dataStr = buffer;

		std::vector<std::string> infoSplited = split(dataStr, '|');
		dePackerize(player, infoSplited);

		return player;
	}
}

void Net::broadcastData()
{
	char buffer[1024];

	std::string dataStr;
	std::stringstream ss;

	for (auto i : m_data.players)
	{
		ss << ">";
		Packer(ss, i);
	}

	dataStr = ss.str();

	strcpy(buffer, dataStr.c_str());

	for (auto c : m_clients)
		send(c.socket, buffer, sizeof(buffer), 0);
}

Data Net::recvData()
{
	char buffer[1024];

	recv(m_server.socket, buffer, sizeof(buffer), 0);

	Data data;

	std::string dataStr = buffer;
	std::vector<std::string>splited = split(dataStr, '>');

	for (auto i = 1; i < splited.size(); i++)
	{
		Player player;
		std::vector<std::string> infoSplited = split(splited[i], '|');
		dePackerize(player, infoSplited);
		data.players.push_back(player);
	}

	return data;
}

void Net::addInfo(Player p)
{
	m_data.players.push_back(p);
}

void Net::clear()
{
	WSACleanup();
	for (auto &t : m_threads)
		t.join();
}