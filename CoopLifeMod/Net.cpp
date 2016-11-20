#include "Net.h"

void Net::ServerThread()
{
	Beep(1000, 1000);
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

	unsigned long iMode = 1;
	int iResult = ioctlsocket(m_server.socket, FIONBIO, &iMode);

	m_hook->setInfo("Waiting for conn...", 60);
	//m_hook->render();
	bind(m_server.socket, (SOCKADDR*)&m_server.addr, sizeof(m_server.addr));
	listen(m_server.socket, 0);

	int sizeof_csin = sizeof(m_client.addr);

	m_client.socket = accept(m_server.socket, (SOCKADDR*)&m_client.addr, &sizeof_csin);
	if (m_client.socket != NULL)
	{
		m_hook->setInfo("Client Connected...", 60);
		m_hook->render();
	}

	iMode = 0;
	iResult = ioctlsocket(m_server.socket, FIONBIO, &iMode);
	fd_set Write, Err;
	FD_ZERO(&Write);
	FD_ZERO(&Err);
	FD_SET(m_server.socket, &Write);
	FD_SET(m_server.socket, &Err);
}

void Net::conn(std::string ip, int port)
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);

	TIMEVAL Timeout;
	Timeout.tv_sec = 5;
	Timeout.tv_usec = 0;

	m_client.server = gethostbyname(ip.c_str());
	m_server.socket = socket(AF_INET, SOCK_STREAM, 0);

	if (m_client.server == NULL)
		printf("[CLIENT] [ERROR] error creating server link\n");

	m_server.addr.sin_family = AF_INET;
	m_server.addr.sin_addr.s_addr = inet_addr(ip.c_str());
	m_server.addr.sin_port = htons(port);

	connect(m_server.socket, (SOCKADDR*)&m_server.addr, sizeof(m_server.addr));
	m_hook->setErr("Connect failed !", 60);
	m_hook->render();

	m_isServer = false;
}

void Net::sendDouble(double val)
{
	char buffer[256];
	std::string valStr = std::to_string(val);
	strcpy(buffer, valStr.c_str());

	send(m_client.socket ? m_isServer : m_server.socket, buffer, sizeof(buffer), 0);
}

double Net::recvDouble()
{
	char buffer[256];
	
	recv(m_client.socket ? m_isServer : m_server.socket, buffer, sizeof(buffer), 0);

	return atof(buffer);
}

void Net::clear() 
{
	WSACleanup();
	for (auto &t : m_threads)
		t.join();
}