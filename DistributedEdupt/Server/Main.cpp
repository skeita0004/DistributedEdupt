#define WIN32_LEAN_AND_MEAN
#include "Server.h"
#include <conio.h>

int main(int argc, char** argv)
{
	// 引数チェック(解像度、スーパーサンプル数、サンプル数)
	if(argc != 5)
	{
		std::cerr << "Invalid argment." << std::endl;
		std::cerr << "Example: ./DEduptSV.exe width height supersample sample" << std::endl;
		return -1;
	}

	Server server{};
	if(server.Initialize(argv) != 0)
	{
		std::cerr << "Initialize failed." << std::endl;
		return -1;
	}

	// --- クライアント接続受付フェーズ ---
	std::cout << "Waiting Client..." << std::endl;
	while(true)
	{
		server.JoinClient();
		
		if(_kbhit())
		{
			if(_getch() == 13)
			{
				break;
			}
		}

		Sleep(100);
	}
	std::cout << "\nStop Accept!" << std::endl;
	std::cout << "Transitioning to the calculation phase..." << std::endl;

	server.PreparationSendData();
	server.SendData();

	// 受信(待機)、画像合成、形式変換(ffmpeg)
	while(true)
	{

	}

	server.Release();

	std::cout << "Press any key to exit." << std::endl;
	_getch();

	return 0;
}