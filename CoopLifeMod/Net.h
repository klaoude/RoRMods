#pragma once

#include <winsock2.h>
#include <iostream>
#include <string>

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
	void create(int port);
	void conn(std::string ip, int port);

	void sendDouble(double val);
	double recvDouble();

	void clear();

private:
	Client m_client;
	Server m_server;

	bool m_isServer;
};