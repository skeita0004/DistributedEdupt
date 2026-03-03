#include <WinSock2.h>

#include "Client.h"

int main(int argc, char* argv[])
{
	//WinSock初期化
	WSADATA wsaData{};
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		int errorCode{WSAGetLastError()};
		std::cerr << "WSAStartup() failed." << std::endl;
		std::cerr << "Error code : " << errorCode << std::endl;
		WSACleanup();
		return -1;
	}


	// メインループ
	while (true)
	{
		// クライアント初期化
		Client  client{};
		if (client.Initialize() != 0)
		{
			std::cerr << "Initialize failed." << std::endl;
			return -1;
		}

		if (client.Run(argc, argv) != 0)
		{
			std::cerr << "ERROR! client.Run() failed." << std::endl;
			return -1;
		}
	}

	// WSA終了
	if (WSACleanup() != 0)
	{
		int errorCode{WSAGetLastError()};
		std::cerr << "WSACleanup() failed." << std::endl;
		std::cerr << "Error code : " << errorCode << std::endl;
		return -1;
	}
	
	return 0;
}