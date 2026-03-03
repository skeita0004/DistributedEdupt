#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <queue> 

#include <ws2tcpip.h>

#include "render.h"

#pragma comment(lib, "ws2_32.lib")


class Client
{
public:
	enum State : uint8_t
	{
		STATE_NONE,
		STATE_QUOTA,
		STATE_COMPLETE_SEND,
		STATE_COMPLETION,
		STATE_MAX
	};

	// 内部管理用
	struct Tile
	{
		Tile() :
			id(0),
			renderData()
		{
		}

		Tile(int _id, edupt::RenderData _renderData) :
			id(_id),
			renderData(_renderData)
		{
		}

		void ChangeEndianNtoH()
		{
			id = ntohl(id);
			renderData = renderData.Load();
		}

		void ChangeEndianHtoN()
		{

		}

		int id;
		edupt::RenderData renderData;
	};

	struct JobData
	{
		JobData() :
			mySize(0),
			status(STATE_NONE),
			tile()
		{
		}

		int64_t mySize;
		State status;
		Tile tile;
	};

	struct RenderResult
	{
		uint32_t id;
		edupt::Color* image;
	};

	Client();
	~Client();

	int Initialize();
	bool Run(const int _argc, const char** _argv);
	int Release();

	bool ConnectServer(const int _argc, const char** _argv);

	int RecvData();
	int SendData();

private:
	void ShowMyIPAddresses();

	/// @brief クライアントのソケット
	SOCKET  sock_;

	/// @brief 届いたタスクを貯めておくキュー
	std::queue<JobData> taskQueue_;

	/// @brief サーバIPアドレス
	std::string serverIP_;

	/// @brief サーバポート番号
	uint16_t    serverPort_;

	/// @brief サーバへの接続を試みるのは初めてか
	bool firstTry_;
};