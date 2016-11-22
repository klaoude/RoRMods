#pragma once

#include <winsock2.h>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include "D3DHook.h"

#pragma comment (lib, "Ws2_32.lib")

const int BUFFER_LENGHT = 256;


/*
J'ai eu une idée, on sait jamais ça peut ptet t'aider:
-un player reçoit un tableau (size max 4) contenant les structures player des autres joueurs.
-le joueur qui reçoit ça rajoute ses propres stat (dans une structure), dans la case suivante du tableau (forcement vide du coup)

si t'arrive pas a faire ça osef, tant qu'a la fin j'ai un tableau de structures Player ça me va.

PS: jpense que ça compile pas parce que D3DHook doit pas connaitre la struct Player
*/

struct Player //send les float avec 2 decimales mini please, thx.
{
	bool isConnected;
	std::string pseudo;

	float health;
	float maxHealth;

	float dmg;
	float rate;
	float crit;
	float regen;
	float strength;

	int level; //je sais plus si tu l'a get celle là, si oui tant mieux.
};

struct Data
{
	std::vector<Player> vec;
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