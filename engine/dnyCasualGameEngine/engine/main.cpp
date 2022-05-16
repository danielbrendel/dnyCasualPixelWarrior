#include "shared.h"
#include "game.h"

/*
	Casual Game Engine (dnyCasualGameEngine) developed by Daniel Brendel

	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Game::CGame* pGame = new Game::CGame();
	
	std::wstring wszArgs = Utils::ConvertToWideString(lpCmdLine);

	if (wszArgs.find(L"-workshop_item_update") != -1) {
		Game::HandlePackageUpload(wszArgs);

		return 0;
	}

	std::wstring wszPackage = L"";
	std::wstring wszMap = L"";

	if (wszArgs.find(L"-run") != -1) {
		std::wstring wszRunArg = wszArgs.substr(wszArgs.find(L" ") + 1);
		if (wszRunArg.length() > 0) {
			std::vector<std::wstring> vParams = Utils::SplitW(wszRunArg, L":");
			if (vParams.size() == 2) {
				wszPackage = vParams[0];
				wszMap = vParams[1];
			}
		}
	}

	pGame->Initialize(wszPackage, wszMap);
	pGame->Process();
	pGame->Release();

	delete pGame;

	return 0;
}