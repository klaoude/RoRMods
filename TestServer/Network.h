#pragma once

#include <sys/types.h>
#include <winsock2.h>
#include <Windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <sstream>

#pragma comment (lib, "Ws2_32.lib")

struct Stats
{
	float health;
	float maxHealth;

	double damage;
	double attackSpeed;
	double strength;
	double regeneration;
	double critical;
	double speed;

	int level;
	int item;
};


struct Player //send les float avec 2 decimales mini please, thx.
{
	bool isConnected;
	std::string pseudo;

	Stats stats;
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
	Net() {}

	void create(int port);
	void conn(std::string ip, int port);

	//Client fonctions
	void sendInfo(Player p);
	Data recvData();

	//Server fonctions
	void recvAllInfo();
	void broadcastData();
	void addInfo(Player p);

	Data getData() { return m_data; }

	void clear();

private:
	void ServerThread();
	Player recvInfo(Client client);

	Server m_server;

	bool m_isServer;

	std::vector<std::thread> m_threads;
	std::vector<Client> m_clients;

	Data m_data;
};