#pragma once
#include <string>
#include <Windows.h>

class LocalClient
{
public:
	LocalClient();

	/// @brief ローカルクライアントの起動
	/// @return 成功:true, 失敗:false
	bool Launch(const std::string& _exePath,
				const std::string& _ip,
				const std::string& _port);

	/// @brief ローカルクライアントの終了
	void Terminate();

private:
	PROCESS_INFORMATION procInfo_;
};