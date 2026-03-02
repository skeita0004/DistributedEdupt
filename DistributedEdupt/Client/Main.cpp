#define _CRT_SECURE_NO_WARNINGS

#include "Client.h"

int main(int argc, char* argv[])
{
	Client client{};

	//接続ループ
	while (true)
	{
		// サーバに接続できるまでループ
		if (client.ConnectServer(argc, argv))
		{
			break;
		}
	}

	while (true)
	{
		while (true)
		{
		}

		//計算・報告フェーズ
		//キューに溜まったタスクを順次処理
		while (!taskQueue.empty())
		{
			JobData current = taskQueue.front();
			taskQueue.pop();
			const int IMAGE_ARRAY_SIZE{current.tile.renderData.tileWidth * current.tile.renderData.tileHeight};

			// レンダリング処理
			cout << "タスク #" << current.tile.id << " を処理中..." << endl;
			std::vector<edupt::Color> image{};
			image.reserve(IMAGE_ARRAY_SIZE);
			image.resize(IMAGE_ARRAY_SIZE);
			edupt::render(current.tile.renderData, image);
			cout << "タスク #" << current.tile.id << " の処理が完了" << endl;


			// 割り当て確認の返信(ACK)
			// いらないかもしれない

			//Store工程、送信データの作成
			const size_t SEND_BUF_SIZE{sizeof(edupt::Color) * IMAGE_ARRAY_SIZE + sizeof(uint32_t) + sizeof(uint32_t)};

			std::vector<char> sendBuf{};
			sendBuf.reserve(SEND_BUF_SIZE);
			sendBuf.resize(SEND_BUF_SIZE);

			// エンディアン変換
			uint32_t sendBufSize{htonl(static_cast<uint32_t>(SEND_BUF_SIZE - sizeof(uint32_t)))};

			uint32_t id = htonl(current.tile.id);

			std::vector<edupt::NetVec> image_int{};
			image_int.reserve(IMAGE_ARRAY_SIZE);
			image_int.resize(IMAGE_ARRAY_SIZE);

			if (not(image.empty()))
			{
				for (int i = 0; i < IMAGE_ARRAY_SIZE; i++)
				{
					image_int[i] = image[i].ChangeEndianHtoN();
				}
			}

			////memcpyで詰め込み、ポインタをずらす
			//memcpy(_sendP, &sSize, sizeof(int));  _sendP += sizeof(int);
			//memcpy(_sendP, &sState, sizeof(int)); _sendP += sizeof(int);
			int offset{0};
			memcpy(sendBuf.data(), &sendBufSize, sizeof(sendBufSize));
			offset += sizeof(sendBufSize);
			memcpy(sendBuf.data() + offset, &id, sizeof(id));
			offset += sizeof(id);
			memcpy(sendBuf.data() + offset, image_int.data(),
				   sizeof(edupt::NetVec) * image_int.size());

			//最後に計算データ本体をコピー
			//memcpy(_sendP, lineData.data(), pixelDataSize);

			//送信実行
			int ret = send(sock, sendBuf.data(), sendBuf.size(), 0);
			cout << "タスク #" << current.tile.id << " の計算結果を送信しました。" << endl;

			#ifdef _DEBUG
			cout << "サイズ：" << ret << endl;
			#endif
		}

		shutdown(sock, SD_BOTH);
		closesocket(sock);
		cout << "-------------------------------------------" << endl;
		cout << "割り当てられた全ての計算が完了したため、サーバーとの通信を終了しました。" << endl;
		cout << "別のサーバーに接続するか、再試行する場合はIPを入力してください。" << endl;
	}
	WSACleanup();
	return 0;
}