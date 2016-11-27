#pragma once

#include <thread>

#include "D3DHook.h"

#pragma comment (lib, "Ws2_32.lib")

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
	Net(D3DHook* hook) : m_hook(hook) {}

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

	D3DHook* m_hook;
	Server m_server;

	bool m_isServer;

	std::vector<std::thread> m_threads;
	std::vector<Client> m_clients;

	Data m_data;
};