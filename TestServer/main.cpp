#include "Network.h"

int main(int argc, char** argv)
{
	Net net;
	bool s = false;
	if (argc == 1 )//&& std::string(argv[1]) == "s")
	{
		net.create(1337);
		s = true;
	}
	else if(argc == 2 && std::string(argv[1]) == "c")
		net.conn("127.0.0.1", 1337);

	Stats stat;
	stat.attackSpeed = 1;
	stat.critical = 2;
	stat.damage = 3;
	stat.health = 4;
	stat.item = 5;
	stat.level = 6;
	stat.maxHealth = 7;
	stat.regeneration = 8;
	stat.strength = 9;

	Player p;
	p.stats = stat;
	p.isConnected = true;
	p.pseudo = "ElOne";

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

		Sleep(100);
	}

	system("Pause");
}