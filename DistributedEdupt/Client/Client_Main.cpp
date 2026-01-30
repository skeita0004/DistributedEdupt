#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment( lib, "ws2_32.lib" )

using namespace std;

int main()
{
	// WinSock2.2初期化
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		cout << "Error :WSAStartup" << endl;
		return 1;
	}

	SOCKET sock = INVALID_SOCKET;

	//接続ループ開始 
	while (true)
	{
		string ipAddress;
		string portStr; // 入力エラー対策のため一旦stringで受ける
		int port;

		cout << "\n Connection Setup " << endl;
		cout << "接続先IPアドレス: ";
		cin >> ipAddress;
		cout << "ポート番号: ";
		cin >> portStr;

		// ポート番号が数値かどうか変換チェック
		try {
			port = stoi(portStr);
		}
		catch (...) {
			cout << "Invalid port number. Please enter a numeric value." << endl;
			continue; // ループの最初に戻る
		}

		// ソケットの作成
		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == INVALID_SOCKET)
		{
			cout << "Error :socket creation failed." << endl;
			WSACleanup();
			return 1;
		}

		// サーバのアドレス設定
		SOCKADDR_IN serverSocketAddress;
		memset(&serverSocketAddress, 0, sizeof(serverSocketAddress));
		serverSocketAddress.sin_family = AF_INET;
		serverSocketAddress.sin_port = htons((unsigned short)port);

		// IPアドレスのフォーマットチェック
		if (inet_pton(AF_INET, ipAddress.c_str(), &serverSocketAddress.sin_addr.s_addr) <= 0)
		{
			cout << "Invalid IP Address format. Try again." << endl;
			closesocket(sock); // 失敗したので一旦閉じる
			continue; // ループの最初に戻る
		}

		// 接続要求
		cout << "Connecting to " << ipAddress << ":" << port << "..." << endl;
		if (connect(sock, (SOCKADDR*)&serverSocketAddress, sizeof(serverSocketAddress)) == SOCKET_ERROR)
		{
			cout << "Connection failed (Error code: " << WSAGetLastError() << "). Check your IP/Port and try again." << endl;
			closesocket(sock); // 失敗したので一旦閉じる
			continue; // ループの最初に戻る
		}

		// 接続成功
		cout << "Connection OK!" << endl;
		break; // 接続できたので入力ループを抜ける
	}
	//接続ループ終了 

	// メッセージ送信ループ
	while (true)
	{
		char buff[256];
		cout << "input message (type 'exit' to quit): ";
		cin >> buff;

		if (strcmp(buff, "exit") == 0) break;

		int ret = send(sock, buff, (int)strlen(buff), 0);
		if (ret == SOCKET_ERROR)
		{
			cout << "Error :send failed." << endl;
			break;
		}
	}

	closesocket(sock);
	WSACleanup();
	cout << "Closed connection. Success :WSACleanup" << endl;

	return 0;
}