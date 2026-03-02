#include "LocalClient.h"
#include <Windows.h>
#include <iostream>

LocalClient::LocalClient() :
	procInfo_()
{}

bool LocalClient::Launch(const std::string& _exePath,
						 const std::string& _ip,
						 const std::string& _port)
{
	std::string cmdLine{_exePath + " " + _ip + " " + _port};

	STARTUPINFOA startUpInfo{};
	startUpInfo.cb = sizeof(startUpInfo);

	DWORD creationFlags{CREATE_NEW_CONSOLE};

	BOOL result{};
	result = CreateProcessA(nullptr,
							cmdLine.data(),
							nullptr,
							nullptr, 
							FALSE,
							creationFlags,
							nullptr,
							nullptr,
							&startUpInfo,
							&procInfo_);

	if(result == FALSE)
	{
		DWORD err = GetLastError();
		std::cerr << "CreateProcessA failed. ERROR = " << err << std::endl;
	}

	return result;
}

void LocalClient::Terminate()
{
	if(CloseHandle(procInfo_.hProcess) == FALSE)
	{
		std::cerr << "FAILED TO CloseHandle(hThread) " << "line:" << __LINE__ << std::endl;
	}

	if(CloseHandle(procInfo_.hThread) == FALSE)
	{
		std::cerr << "FAILED TO CloseHandle(hThread) " << "line:" << __LINE__ << std::endl;
	}
}
