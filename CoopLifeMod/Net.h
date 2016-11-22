#pragma once

#include <winsock2.h>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include "D3DHook.h"

#pragma comment (lib, "Ws2_32.lib")

const int BUFFER_LENGHT = 256;

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
	Net(D3DHook* hook) { m_hook = hook; }
	void create(int port);
	void conn(std::string ip, int port);

	void setErr(const char *str, int val) { m_hook->setErr(str, val); };
	void setInfo(const char *str, int val) { m_hook->setInfo(str, val); };

	void sendDouble(double val);
	double recvDouble();

	void sendData(Data data);
	Data recvData();

	void clear();

private:
	void ServerThread();

	D3DHook* m_hook;
	Client m_client;
	Server m_server;

	bool m_isServer;

	std::vector<std::thread> m_threads;
};