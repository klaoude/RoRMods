#include "Network.h"

int main(int argc, char** argv)
{
	Net net;
	bool s = false;
	
	net.conn("127.0.0.1", 1338);
	//net.create(1338);

	Stats stat;
	stat.attackSpeed = 1;
	stat.critical = 2;
	stat.damage = 3;
	stat.health = 1000;
	stat.item = 5;
	stat.level = 6;
	stat.maxHealth = 1000;
	stat.regeneration = 8;
	stat.strength = 9;

	Player p;
	p.stats = stat;
	p.isConnected = true;
	p.pseudo = "Bots qui nique des m�res";

	while (1)
	{
		if(!s)
		{
			net.sendInfo(p);
			Data dt;
			dt = net.recvData();
			for (auto i : dt.players)
				std::cout << "Health of " << i.pseudo << " = " << i.stats.health << std::endl;
		}
		else
		{
			net.recvAllInfo();
			net.addInfo(p);
			net.broadcastData();
		}

		Sleep(100);
	}

	system("Pause");
}