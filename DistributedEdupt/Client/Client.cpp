#include "Client.h"

Client::Client()
{
}

Client::~Client()
{
}

int Client::Initialize()
{
	//WinSock初期化
	if (WSAStartup(MAKEWORD(2, 2), &wsaData_) != 0)
	{
		return 1;
	}

	//ソケット作成
	sock_ = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_ == INVALID_SOCKET)
	{
		std::cout << "ソケット作成失敗" << std::endl;
		WSACleanup();
		return 1;
	}
}

int Client::Release()
{
}

bool Client::ConnectServer(int _argc, char** _argv)
{
	//argv[0]がコマンド名、argv[1]がIP、argv[2]がポート番号の想定
	//引数があるか、2回目以降の再試行かで分岐
	if (_argc == 3)
	{
		//初回かつ引数がある場合
		serverIP_ = _argv[1];
		serverPort_ = (uint16_t)atoi(_argv[2]);
		std::cout << "コマンドライン引数を使用します: " << serverIP_ << ":" << serverPort_ << std::endl;
	}
	else
	{
		//引数がない、または引数での接続に失敗した場合は手入力
		ShowMyIPAddresses();
		std::cout << "\n接続先サーバーIP: ";
		std::cin >> serverIP_;

		std::string portStr{};
		std::cout << "ポート番号: ";
		std::cin >> portStr;

		serverPort_ = (uint16_t)stoi(portStr);
	}

	//接続設定
	SOCKADDR_IN addr = {0};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(serverPort_);
	inet_pton(AF_INET, serverIP_.c_str(), &addr.sin_addr.s_addr);

	//接続実行
	if (connect(sock_, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		std::cout << "接続失敗 IP: " << serverIP_ << " Port: " << serverPort_ << " に繋げませんでした。" << std::endl;
		std::cout << "コマンドライン引数、もしくは、接続先IPアドレス・ポート番号が誤っている可能性があります。" << std::endl;
		closesocket(sock_);

		// ループの最初（入力待ち）に戻る
		return false;
	}

	//接続成功して初めてこのメッセージを出す
	std::cout << "Connected!  サーバーからのタスクを待機します。" << std::endl;
	//接続できたので入力ループを脱出

	return true;
}

void Client::RecvData()
{
	// 受信データのサイズを取得する
// 受信バッファのサイズを取得したサイズでリサイズする

// 受信データ格納用
	char recvRawData[sizeof(JobData)];

	int recvRet = recv(sock_, recvRawData, sizeof(JobData), 0);
	if (recvRet > 0)
	{
		// 解析用ポインタ
		const char* _p = recvRawData;

		JobData tmp{};
		int index{0};

		//ポインタをずらしながら3つ分取り出す
		memcpy(&tmp.mySize, &_p[index], sizeof(tmp.mySize));
		index += sizeof(tmp.mySize);

		memcpy(&tmp.status, &_p[index], sizeof(tmp.status));
		index += sizeof(tmp.status);

		memcpy(&tmp.tile, &_p[index], sizeof(tmp.tile));

		//ネットワークオーダーからホストオーダー変換
		tmp.mySize = ntohl(tmp.mySize);
		tmp.tile.ChangeEndianNtoH();

		cout << "タスク #" << tmp.tile.id << " 受信完了。キューへ追加（現在：" << taskQueue.size() + 1 << "件）" << endl;

		if (tmp.status == STATE_COMPLETE_SEND)
		{
			break;
		}

		taskQueue.push(tmp);
	}
	else if (recvRet == 0)
	{
		cout << "サーバーが接続を閉じました。" << endl;
		break;
	}
	else
	{
		//何も届いていない場合はループの先頭に戻って待機
		continue;
	}

}

void Client::SendData()
{

}

void Client::ShowMyIPAddresses()
{
	char hostname[256];
	if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR)
	{
		return;
	}

	ADDRINFOA hints, * res = NULL;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;

	if (getaddrinfo(hostname, NULL, &hints, &res) == 0)
	{
		std::cout << "このPCのIPアドレス:" << std::endl;
		for (ADDRINFOA* p = res; p != NULL; p = p->ai_next)
		{
			char ipStr[INET_ADDRSTRLEN];
			struct sockaddr_in* addr = (struct sockaddr_in*)p->ai_addr;
			inet_ntop(AF_INET, &addr->sin_addr, ipStr, sizeof(ipStr));
			std::cout << " >> " << ipStr << std::endl;
		}
		freeaddrinfo(res);
	}
}