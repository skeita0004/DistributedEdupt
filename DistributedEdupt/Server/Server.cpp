#include <iostream>
#include <vector>
#include <string>
#include <conio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

const unsigned short SERVER_PORT = 8888;

struct ClientInfo
{
    SOCKET sock;
    std::string ip;
};

// サーバー自身のIPアドレスを取得する関数
void ShowServerIP() 
{
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0)
    {
        struct addrinfo hints = {}, * res = nullptr;
        hints.ai_family = AF_INET; // IPv4
        if (getaddrinfo(hostname, nullptr, &hints, &res) == 0)
        {
            std::cout << "   Server IP Addresses:" << std::endl;
            for (auto curr = res; curr != nullptr; curr = curr->ai_next) 
            {
                char ipStr[INET_ADDRSTRLEN];
                struct sockaddr_in* addr = (struct sockaddr_in*)curr->ai_addr;
                inet_ntop(AF_INET, &addr->sin_addr, ipStr, sizeof(ipStr));
                std::cout << "     >> " << ipStr << std::endl;
            }
            freeaddrinfo(res);
        }
    }
}

int main() {
    // 1. WinSock2.2 初期化
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed." << std::endl;
        return -1;
    }

    // 2. リスンソケットの作成
    SOCKET listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSock == INVALID_SOCKET) 
    {
        WSACleanup();
        return -1;
    }

    // ノンブロッキング設定
    unsigned long arg = 0x01;
    ioctlsocket(listenSock, FIONBIO, &arg);
    
    // アドレスの作成
    SOCKADDR_IN addr = { 0 };
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(listenSock, (SOCKADDR*)&addr, sizeof(addr));
    listen(listenSock, 5);

    std::vector<ClientInfo> connectedClients;

    // 初回画面
    auto DisplayStatus = [&]()
        {
            system("cls");
            std::cout << "--------------------------------------" << std::endl;
            std::cout << "   Rendering Server Mode: [WAITING]   " << std::endl;
            ShowServerIP(); // サーバーのIPアドレスを表示
            std::cout << "--------------------------------------" << std::endl;

            if (connectedClients.empty())
            {
                std::cout << "\n [Status]クライアントの接続を待っています..." << std::endl;
            }
            else
            {
                std::cout << "\n Connected Clients: " << connectedClients.size() << std::endl;
                std::cout << "   ------------------------------------------" << std::endl;
                for (size_t i = 0; i < connectedClients.size(); ++i)
                {
                    std::cout << "    [" << i << "] IP: " << connectedClients[i].ip << std::endl;
                }
            }
            std::cout << "\n   Press [Enter] to STOP accepting and START calculation..." << std::endl;
        };

    DisplayStatus();

    // --- クライアント接続受付フェーズ ---
    while (true) 
    {
        SOCKADDR_IN clientAddr;
        int addrLen = sizeof(clientAddr);
        SOCKET newSock = accept(listenSock, (SOCKADDR*)&clientAddr, &addrLen);

        if (newSock != INVALID_SOCKET) 
        {
            char ipStr[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &clientAddr.sin_addr, ipStr, sizeof(ipStr));

            ClientInfo info = { newSock, std::string(ipStr) };
            connectedClients.push_back(info);

            DisplayStatus(); // 接続があったので表示更新
        }

        if (_kbhit()) 
        {
            if (_getch() == 13) break;
        }

        Sleep(100);
    }

    std::cout << "\n[System] 接続を締め切りました。計算フェーズに移行します。" << std::endl;

    // --- 後処理（実際はここで計算命令を送信する） ---
    for (auto& c : connectedClients) closesocket(c.sock);
    closesocket(listenSock);
    WSACleanup();

    std::cout << "Press any key to exit." << std::endl;
    _getch();
    return 0;
}