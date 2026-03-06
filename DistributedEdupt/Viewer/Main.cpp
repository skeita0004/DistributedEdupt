#include <DxLib.h>

#include <vector>
#include <string>
#include <sstream>

bool LoadTiles(int _tileNumX, int _tileNumY, std::vector<int>& _handles);
void ReLoadTiles(int _tileNumX, int _tileNumY, std::vector<int>& _handles);

int APIENTRY WinMain(_In_ HINSTANCE hInstance,
					 _In_opt_ HINSTANCE hPrevInstance,
					 _In_ LPSTR lpCmdLine,
					 _In_ int nCmdShow)
{
	const int SCREEN_WIDTH{1366};
	const int SCREEN_HEIGHT{768};

	ChangeWindowMode(TRUE);
	SetGraphMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32);
	SetOutApplicationLogValidFlag(TRUE);

	SetMainWindowText("RenderResultViewer");
	//SetWindowSizeExtendRate(1.0f);

	if (DxLib_Init() == -1)
	{
		return -1;
	}

	int imageWidth{256};
	int imageHeight{256};

	int tileSize{64};

	// 画像ハンドル配列用
	int tileNumX{4}; // 横方向タイル数
	int tileNumY{4}; // 縦方向タイル数

	int argc{};

	std::vector<std::string> cmdLineArgs{};
	std::string cmdLineRawArgs{GetCommandLineA()};
	std::istringstream iss(cmdLineRawArgs);

	std::string arg{};
	while (iss >> arg)
	{
		cmdLineArgs.push_back(arg);
	}

	imageWidth  = std::stoi(cmdLineArgs[1]);
	imageHeight = std::stoi(cmdLineArgs[2]);
	tileNumX    = std::stoi(cmdLineArgs[3]);
	tileNumY    = std::stoi(cmdLineArgs[4]);
	tileSize    = std::stoi(cmdLineArgs[5]);

	std::vector<int> handles{};

	// 総タイル分の画像を読み込めるまで
	while (true)
	{
		if (LoadTiles(tileNumX, tileNumY, handles))
		{
			break;
		}
	}

	int tileMerged{MakeScreen(tileSize * tileNumX, tileSize * tileNumY)};
	int rotateAndCropped{MakeScreen(imageWidth, imageHeight)};

	while (true)
	{
		ClearDrawScreen();

		SetDrawScreen(tileMerged);

		for (int y = 0; y < tileNumY; y++)
		{
			for (int x = 0; x < tileNumX; x++)
			{
				DrawGraph(x * tileSize, y * tileSize, handles[tileNumX * y + x], FALSE);
			}
		}

		SetDrawScreen(rotateAndCropped);

		int drawOriginX{imageWidth / 2};
		int drawOriginY{imageHeight / 2};

		int srcX{(tileNumX * tileSize) - imageWidth};
		int srcY{(tileNumY * tileSize) - imageHeight};
		DrawRectRotaGraph(drawOriginX, drawOriginY, srcX, srcY, imageWidth, imageHeight, 1.0, 0.0, tileMerged, FALSE, TRUE, TRUE);

		SetDrawScreen(DX_SCREEN_BACK);
		DrawExtendGraph(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, rotateAndCropped, FALSE);

		ReLoadTiles(tileNumX, tileNumY, handles);
		ScreenFlip();
		WaitTimer(16);
		if (ProcessMessage() == -1 or CheckHitKey(KEY_INPUT_ESCAPE) == TRUE)
		{
			break;
		}
	}

	DxLib_End();
	return 0;
}

bool LoadTiles(int _tileNumX, int _tileNumY, std::vector<int>& _handles)
{
	int length = _tileNumX * _tileNumY;
	_handles.resize(length);
	for (int i = 0; i < length; i++)
	{
		std::string path{".\\out" + std::to_string(i) + ".png"};
		_handles[i] = LoadGraph(path.c_str());
	}

	int succsessCounter{0};
	for (auto& handle : _handles)
	{
		if (handle != 0)
		{
			succsessCounter++;
		}
	}
	if (succsessCounter == _handles.size())
	{
		return true;
	}

	return false;
}

void ReLoadTiles(int _tileNumX, int _tileNumY, std::vector<int>& _handles)
{
	int length = _tileNumX * _tileNumY;
	for (int i = 0; i < length; i++)
	{
		std::string path{".\\out" + std::to_string(i) + ".png"};
		ReloadGraph(path.c_str(), _handles[i]);
	}
}
