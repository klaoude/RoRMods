#include <sys/types.h>
#include <winsock2.h>
#include <Windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <sstream>

#pragma comment (lib, "Ws2_32.lib")

const int BUFFER_LENGHT = 256;

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

struct Player //send les float avec 2 decimales mini please, thx.
{
	bool isConnected = false;

	float health;
	float maxHealth;

	float dmg;
	float rate;
	float crit;
	float regen;
	float strength;

	int level; //je sais plus si tu l'a get celle là, si oui tant mieux.

	std::string pseudo;
};

struct Data
{
	std::vector<Player> players;
};

struct Client
{
	sockaddr_in addr;
	int clilen = sizeof(sockaddr_in);
	int socket;

	hostent* server;
};

struct Server
{
	sockaddr_in addr;
	int port;
	int socket;
};

class Net
{
public:
	void create(int port);
	void conn(std::string ip, int port);

	void sendDouble(double val);
	double recvDouble();

	void sendData(Data data);
	Data recvData();

	void clear();

private:
	void ServerThread();

	Client m_client;
	Server m_server;

	bool m_isServer;

	std::vector<std::thread> m_threads;
};

void Net::sendData(Data data)
{
	int sock;
	if (m_isServer)
		sock = m_client.socket;
	else
		sock = m_server.socket;

	char buffer[1024];

	std::string dataStr;
	std::stringstream ss;

	for (auto i : data.players)
	{
		ss << ">";
		ss << i.isConnected;
		ss << "|";
		ss << i.health;
		ss << "|";
		ss << i.maxHealth;
		ss << "|";
		ss << i.dmg;
		ss << "|";
		ss << i.rate;
		ss << "|";
		ss << i.crit;
		ss << "|";
		ss << i.strength;
		ss << "|";
		ss << i.level;
		ss << "|";
		ss << i.pseudo;
	}

	dataStr = ss.str();

	strcpy(buffer, dataStr.c_str());

	send(sock, buffer, sizeof(buffer), 0);
}

Data Net::recvData()
{
	char buffer[1024];

	int sock;
	if (m_isServer)
		sock = m_client.socket;
	else
		sock = m_server.socket;

	recv(sock, buffer, sizeof(buffer), 0);

	std::cout << "Buffer : " << buffer << std::endl;

	Data data;

	std::string dataStr = buffer;
	std::vector<std::string>splited = split(dataStr, '>');

	for (auto i = 1; i < splited.size(); i++)
	{
		std::cout << "[1] " << splited[i] << std::endl;
		Player player;
		std::vector<std::string> infoSplited = split(splited[i], '|');
		std::cout << "[2] " << infoSplited[0] << std::endl;
		player.isConnected = true ? infoSplited[0] == "1" : false;
		player.health = atof(infoSplited[1].c_str());
		player.maxHealth = atof(infoSplited[2].c_str());
		player.dmg = atof(infoSplited[3].c_str());
		player.rate = atof(infoSplited[4].c_str());
		player.crit = atof(infoSplited[5].c_str());
		player.regen = atof(infoSplited[6].c_str());
		player.strength = atof(infoSplited[7].c_str());
		/*player.level = atof(infoSplited[8].c_str());
		player.pseudo = infoSplited[9];*/
		data.players.push_back(player);
	}

	return data;
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

	printf("Waiting for conn...\n");
	bind(m_server.socket, (SOCKADDR*)&m_server.addr, sizeof(m_server.addr));
	listen(m_server.socket, 0);

	int sizeof_csin = sizeof(m_client.addr);

	m_client.socket = accept(m_server.socket, (SOCKADDR*)&m_client.addr, &sizeof_csin);
	if (m_client.socket != INVALID_SOCKET)
	{
		printf("Client Connected !\n");
	}
}

void Net::conn(std::string ip, int port)
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);

	TIMEVAL Timeout;
	Timeout.tv_sec = 5;
	Timeout.tv_usec = 0;

	m_client.server = (hostent*)gethostbyname(ip.c_str());
	m_server.socket = socket(AF_INET, SOCK_STREAM, 0);

	if (m_client.server == NULL)
		printf("[CLIENT] [ERROR] error creating server link\n");

	m_server.addr.sin_family = AF_INET;
	m_server.addr.sin_addr.s_addr = inet_addr(ip.c_str());
	m_server.addr.sin_port = htons(port);

	if (connect(m_server.socket, (SOCKADDR*)&m_server.addr, sizeof(m_server.addr)) == SOCKET_ERROR)
		printf("Connection failed\n");

	m_isServer = false;
}

void Net::sendDouble(double val)
{
	char buffer[256];
	std::string valStr = std::to_string(val);
	strcpy(buffer, valStr.c_str());

	int sock;
	if (m_isServer)
		sock = m_client.socket;
	else
		sock = m_server.socket;

	send(sock, buffer, sizeof(buffer), 0);
}

double Net::recvDouble()
{
	char buffer[256];

	int sock;
	if (m_isServer)
		sock = m_client.socket;
	else
		sock = m_server.socket;

	recv(sock, buffer, sizeof(buffer), 0);

	return atof(buffer);
}

void Net::clear()
{
	WSACleanup();
	for (auto &t : m_threads)
		t.join();
}

int main(int argc, char** argv)
{
	Net net;
	if (argc == 2 && std::string(argv[1]) == "s")
		net.create(1337);
	else
		net.conn("127.0.0.1", 1337);

	while (1)
	{
		Data data = net.recvData();
		std::cout << "[Health] " <<  data.players[0].health << std::endl;
	}

	system("Pause");
}