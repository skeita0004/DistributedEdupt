#pragma once

#include <iostream>
#include <vector>
#include <string>
//#include <conio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <cstdlib>
#include "render_data.h"
#include "LocalClient.h"

#pragma comment(lib, "ws2_32.lib")

class LocalClient;

class Server
{
public:
	Server();
	~Server();
	
	/// @brief サーバの初期化
	/// @param _argv main関数が受け取ったコマンドライン引数
	/// @return 正常終了: 0, 異常終了: -1
	int Initialize(const char** _argv);

	/// @brief 解放処理
	/// @return 正常終了: 0, 異常終了: -1
	int Release();

	void JoinClient();
	void PreparationSendData();
	void SendData();

	void SendDataStab();

private:
	enum State: uint8_t
	{
		STATE_NONE,
		STATE_QUOTA,
		STATE_COMPLETION,
		STATE_MAX
	};

	struct ClientInfo
	{
		SOCKET sock;
		std::string ip;
	};

	// 内部管理用
	struct Tile
	{
		Tile(int _id,edupt::RenderData _renderData):
			id(_id),
			renderData(_renderData)
		{}

		int id;
		edupt::RenderData renderData;
	};

	// 送信用
	struct JobData
	{
		int64_t mySize;
		State status;
		Tile tile;
	};

	// 受信用
	struct RecvData
	{
		int id;
		edupt::Color renderResult;
	};

	/// @brief テスト用スタブ
	struct RenderTask
	{
		int taskId;
		int startY;
		int endY;
		State status;
		std::string ip;
	};

	void AcceptLocalClient();

	// サーバー自身のIPアドレスを取得する関数
	void ShowServerIP();

	void DisplayMessage(const std::vector<ClientInfo>& clients);

	void GetCommandLineArgs(const char** _argv);


	inline static const uint16_t PORT_{8888};

	int imageWidth_;
	int imageHeight_;
	int superSampleNum_;
	int sampleNum_;

	WSADATA wsaData_;
	SOCKET listenSock_;
	SOCKADDR_IN serverAddr_;

	std::vector<ClientInfo> connectedClients_;
	std::vector<Tile> renderData_;
	LocalClient* localClient_;
};