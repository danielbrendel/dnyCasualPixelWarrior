#include "game.h"

/*
	Casual Game Engine (dnyCasualGameEngine) developed by Daniel Brendel

	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

namespace Game {
	class CGame* pGame = nullptr;
	CWindowEvents oDxWindowEvents;

	bool CGame::LoadMap(const std::wstring& wszMap)
	{
		//Load package map file

		pConsole->AddLine(L"Loading map: " + wszMap);

		this->m_bShowIntermission = false;
		this->m_bGamePause = false;

		//Free old resources

		for (size_t i = 0; i < this->m_vSolidSprites.size(); i++) {
			this->m_vSolidSprites[i].Release();
		}

		this->m_vSolidSprites.clear();

		Entity::oScriptedEntMgr.Release();

		//Execute package map file
		if (!pConfigMgr->Execute(this->GetPackagePath() + L"maps\\" + wszMap)) {
			pConsole->AddLine(L"Failed to execute package map script");
			return false;
		}

		this->m_sMap.wszFileName = wszMap;

		//Publish current achievements
		if (pAppSteamID->iValue != 0) {
			pAchievements->PublishAchievementAndStatProgress();
		}

		std::wstring wszBackgroundFile = this->m_sPackage.wszPakPath + L"\\gfx\\" + this->m_sMap.wszBackground;
		if (!Utils::FileExists(wszBackgroundFile)) {
			wszBackgroundFile = wszBasePath + L"packages\\.common\\gfx\\" + this->m_sMap.wszBackground;
		}

		//Set map background
		return pRenderer->SetBackgroundPicture(wszBackgroundFile);
	}

	void CGame::Process(void)
	{
		//Process game

		//Query performance frequency
		QueryPerformanceFrequency((LARGE_INTEGER*)&this->m_lFrequency);

		//Get initial performance counter value
		QueryPerformanceCounter((LARGE_INTEGER*)&this->m_lLastCount);

		while (this->m_bInit) {
			//Query current performance counter
			QueryPerformanceCounter((LARGE_INTEGER*)&this->m_ilCurCount);

			if (this->m_ilCurCount - this->m_lLastCount > this->m_lFrequency) { //If a second has elapsed
				this->m_lLastCount = this->m_ilCurCount; //Update last count value
				this->m_iFrameRate = this->m_iFrames; //Store rate for this second
				this->m_iFrames = 0; //Clear to start counting again
			} else {
				this->m_iFrames++; //Increment frames
			}

			//Perform window processing
			pWindow->Process();

			//Process input
			if (pInput) {
				pInput->ProcessKeyboard();
				pInput->ProcessMouse();
			}

			//Process Steam callbacks
			if (pAppSteamID->iValue != 0) {
				SteamAPI_RunCallbacks();
			}

			//Perform game loading if indicated
			if (this->m_bInGameLoadingProgress) {
				this->m_uiSkipFrame++; 
				
				if (this->m_uiSkipFrame >= 2) {
					if (pAppSteamID->iValue != 0) {
						pAchievements->PublishAchievementAndStatProgress();
					}

					if (!this->StartGame(this->m_wszCurrentLoadingPackage, this->m_wszCurrentLoadingFromPath, this->m_wszCurrentLoadingMap)) {
						pConsole->AddLine(L"Failed to start new game", Console::ConColor(250, 0, 0));
					}

					if (this->m_bLoadSavedGame) { //Handle load saved game state case
						this->LoadMap(Utils::ConvertToWideString(this->m_oSaveGameReader.GetDataItem("map")));

						const std::vector<Entity::CSaveGameReader::save_game_entry_s>& vList = this->m_oSaveGameReader.GetDataVector();
						for (size_t i = 0; i < vList.size(); i++) {
							if ((vList[i].szIdent == "package") || (vList[i].szIdent == "frompath") || (vList[i].szIdent == "map")) {
								continue;
							}

							Scripting::HSISCRIPT hScript = this->FindScript(L"player");

							std::string szIdent = vList[i].szIdent;
							std::string szValue = vList[i].szValue;

							BEGIN_PARAMS(vArgs);
							PUSH_OBJECT(&szIdent);
							PUSH_OBJECT(&szValue);

							pScriptingInt->CallScriptFunction(hScript, true, "RestoreState", &vArgs, nullptr, Scripting::FA_VOID);

							END_PARAMS(vArgs);
						}

						this->m_oSaveGameReader.Close();
						this->m_bLoadSavedGame = false;
					}

					this->m_bInGameLoadingProgress = false;
				}
			}

			if ((this->m_bGameStarted) && (!this->m_bGamePause)) {
				//Process scripted entities
				Entity::oScriptedEntMgr.Process();

				//Process goal entity
				if (this->m_pGoalEntity) {
					this->m_pGoalEntity->Process();

					//Handle if game goal reached
					if (this->m_pGoalEntity->IsGoalReached()) {
						if (this->m_pGoalEntity->GetGoal() == L"#finished") { //Package game has finished
							this->m_oIntermissionMenu.SetGameFinishState(true);
						} else { //Current map has finished and a next map is following
							this->m_oIntermissionMenu.SetGameFinishState(false);
						}

						this->m_oIntermissionMenu.SetGameScore(this->GetLocalPlayerScore());
						this->m_bShowIntermission = true;
						this->m_bGamePause = true;
						this->m_oCursor.SetActiveStatus(true);
					}
				}
			}

			//Process HUD info messages
			this->m_oHudInfoMessages.Process();

			Sleep(1);
		}
	}

	void CGame::Draw(void)
	{
		if (this->m_bGameStarted) {
			if (!this->m_oMenu.IsOpen()) {
				//Draw solid sprites
				for (size_t i = 0; i < this->m_vSolidSprites.size(); i++) {
					this->m_vSolidSprites[i].Draw();
				}

				//Draw scripted entities
				Entity::oScriptedEntMgr.Draw();
				Entity::oScriptedEntMgr.DrawOnTop();

				//Draw goal entity
				if (this->m_pGoalEntity) {
					this->m_pGoalEntity->Draw();
				}

				//Draw intermission menu if active
				if (this->m_bShowIntermission) {
					this->m_oIntermissionMenu.Draw();
				}

				//Draw game over menu if active
				if (this->m_bGameOver) {
					this->m_oGameOverMenu.Draw();
				}
			}

			//Draw game HUD
			if ((this->m_pHud) && (!this->m_oMenu.IsOpen())) {
				this->m_pHud->Draw();
			}
		}

		//Draw menu if opened
		if (this->m_oMenu.IsOpen()) {
			pRenderer->DrawSprite(this->m_hBanner, pGfxResolutionWidth->iValue / 2 - C_MENU_BANNER_WIDTH / 2, 10, 0, 0.0f);

			this->m_oMenu.Draw();
		}

		//Draw HUD info messages
		this->m_oHudInfoMessages.Draw();

		if (this->m_bInGameLoadingProgress) {
			pRenderer->DrawSprite(this->m_hLoadingScreen, 0, 0, 0, 0.0f);
		}

		//Draw console if opened
		if (pConsole) {
			pConsole->Draw();
		}

		//Draw cursor
		this->m_oCursor.Draw();
	}

	void CGame::StopGame(void)
	{
		//Stop game and cleanup

		//Indicate game not started anymore
		this->m_bGameStarted = false;

		//Release solid sprites
		for (size_t i = 0; i < this->m_vSolidSprites.size(); i++) {
			this->m_vSolidSprites[i].Release();
		}

		//Release scripted entities
		Entity::oScriptedEntMgr.Release();

		//Unload entity scripts
		for (size_t i = 0; i < this->m_vEntityScripts.size(); i++) {
			pScriptingInt->UnloadScript(this->m_vEntityScripts[i].hScript);
		}

		//Clear lists
		this->m_vEntityScripts.clear();
		this->m_vSolidSprites.clear();

		//Free memory
		if (this->m_pGoalEntity) {
			delete this->m_pGoalEntity;
			this->m_pGoalEntity = nullptr;
		}

		//Reset indicators
		this->m_bShowIntermission = false;
		this->m_bGameOver = false;
		this->m_bInGameLoadingProgress = false;
		this->m_bLoadSavedGame = false;

		//Inform menu
		this->m_oMenu.OnStopGame();
		//this->m_oMenu.OnCloseAll();
		this->m_oMenu.SetOpenStatus(true);

		//Show cursor
		this->m_oCursor.SetActiveStatus(true);

		//Publish current achievements
		if (pAppSteamID->iValue != 0) {
			pAchievements->PublishAchievementAndStatProgress();
		}

		//Restore background image
		pRenderer->SetBackgroundPicture(wszBasePath + L"media\\gfx\\background.jpg");

		//Play main menu theme if loaded
		pSound->Play(this->m_hMenuTheme, pSndVolume->iValue, DSBPLAY_LOOPING);
	}

	void CGame::OnMouseEvent(int x, int y, int iMouseKey, bool bDown, bool bCtrlHeld, bool bShiftHeld, bool bAltHeld)
	{
		//Called for mouse events
		
		this->m_oCursor.OnMouseEvent(x, y, iMouseKey, bDown, bCtrlHeld, bShiftHeld, bAltHeld);

		if (!this->m_oMenu.IsOpen()) {
			//Inform player entity
			if (!iMouseKey) {
				const Entity::CScriptedEntsMgr::playerentity_s& playerEntity = Entity::oScriptedEntMgr.GetPlayerEntity();
				Entity::Vector vPos(x, y);
				BEGIN_PARAMS(vArgs);
				PUSH_OBJECT(&vPos);
				pScriptingInt->CallScriptMethod(playerEntity.hScript, playerEntity.pObject, "void OnUpdateCursor(const Vector &in pos)", &vArgs, nullptr, Scripting::FA_VOID);
			} else {
				const Entity::CScriptedEntsMgr::playerentity_s& playerEntity = Entity::oScriptedEntMgr.GetPlayerEntity();
				BEGIN_PARAMS(vArgs);
				PUSH_DWORD(iMouseKey);
				PUSH_BYTE(bDown);
				pScriptingInt->CallScriptMethod(playerEntity.hScript, playerEntity.pObject, "void OnMousePress(int key, bool bDown)", &vArgs, nullptr, Scripting::FA_VOID);
			}
		} else {
			//Pass to menu
			this->m_oMenu.OnMouseEvent(x, y, iMouseKey, bDown, bCtrlHeld, bShiftHeld, bAltHeld);
			return;
		}

		if (this->m_bShowIntermission) { //Inform intermission menu
			this->m_oIntermissionMenu.OnMouseEvent(x, y, iMouseKey, bDown, bCtrlHeld, bShiftHeld, bAltHeld);
			return;
		}

		if (this->m_bGameOver) { //Inform game over menu
			this->m_oGameOverMenu.OnMouseEvent(x, y, iMouseKey, bDown, bCtrlHeld, bShiftHeld, bAltHeld);
			return;
		}
	}

	void CGame::OnMouseWheel(short wDistance, bool bForward)
	{
		//Handle mouse wheel event

		if (pConsole->IsVisible()) {
			if (bForward) {
				pConsole->ScrollUp();
			} else {
				pConsole->ScrollDown();
			}

			return;
		}

		if (this->m_oMenu.IsOpen()) {
			this->m_oMenu.OnMouseWheel(wDistance, bForward);
		}
	}

	void CGame::OnKeyEvent(int vKey, bool bDown, bool bCtrlHeld, bool bShiftHeld, bool bAltHeld)
	{
		//Called for key events
		
		if (vKey == g_oInputMgr.GetKeyBindingCode(L"MENU")) {
			if (!bDown) {
				if (this->m_bGameStarted) {
					this->m_oMenu.SetOpenStatus(!this->m_oMenu.IsOpen());
					this->m_oCursor.SetActiveStatus(this->m_oMenu.IsOpen());
				}

				this->m_bGamePause = this->m_oMenu.IsOpen();

				return;
			}
		}
		
		if ((vKey == g_oInputMgr.GetKeyBindingCode(L"SAVEGAME")) && (!bDown)) {
			this->SaveGame();
		}

		if (!this->m_oMenu.IsOpen()) {
			const Entity::CScriptedEntsMgr::playerentity_s& playerEntity = Entity::oScriptedEntMgr.GetPlayerEntity();

			BEGIN_PARAMS(vArgs);
			PUSH_DWORD(vKey);
			PUSH_BYTE(bDown);
			
			pScriptingInt->CallScriptMethod(playerEntity.hScript, playerEntity.pObject, "void OnKeyPress(int vKey, bool bDown)", &vArgs, nullptr, Scripting::FA_VOID);
		} else {
			this->m_oMenu.OnKeyEvent(vKey, bDown, bCtrlHeld, bShiftHeld, bAltHeld);
		}
	}

	void CWindowEvents::OnCreated(HWND hWnd)
	{
	}

	LRESULT CWindowEvents::OnKeyEvent(int vKey, bool bDown, bool bCtrlHeld, bool bShiftHeld, bool bAltHeld)
	{
		//Handle key input

		if ((vKey == g_oInputMgr.GetKeyBindingCode(L"CONSOLE")) && (bDown)) {
			pConsole->Toggle();
		}

		pGame->OnKeyEvent(vKey, bDown, bCtrlHeld, bShiftHeld, bAltHeld);

		return 0;
	}

	LRESULT CWindowEvents::OnMouseEvent(int x, int y, int iMouseKey, bool bDown, bool bCtrlHeld, bool bShiftHeld, bool bAltHeld)
	{
		//Handle mouse input

		pGame->OnMouseEvent(x, y, iMouseKey, bDown, bCtrlHeld, bShiftHeld, bAltHeld);

		return 0;
	}

	void CWindowEvents::OnMouseWheel(short wDistance, bool bForward)
	{
		pGame->OnMouseWheel(wDistance, bForward);
	}

	void CWindowEvents::OnDraw(void)
	{
		pRenderer->DrawBegin();
		pGame->Draw();
		pRenderer->DrawEnd();
	}

	void CWindowEvents::OnRelease(void)
	{
		pGame->Release();
	}

	void OnHandleWorkshopItem(const std::wstring& wszItem)
	{
		//Fired for handling Workshop items

		Workshop::workshop_item_info_s sInfo;
		
		if (Workshop::LoadWorkshopInfoData(wszItem, sInfo)) {
			Game::pGame->AddPackage(sInfo.wszPackageName, wszItem);
		}
	}

	void HandlePackageUpload(const std::wstring& wszArgs)
	{
		//Handle package upload process

		std::wstring wszItem = wszArgs.substr(wszArgs.find(L"-workshop_item_update") + wcslen(L"-workshop_item_update") + 1);

		if (MessageBox(0, (L"Attempting to upload game package: " + wszItem + L"\n\nNote: This might take a while. You will be informed when the process has finished.\n\nDo you want to continue?").c_str(), APP_NAME, MB_ICONINFORMATION | MB_YESNO) == IDNO)
			return;

		SteamAPI_Init();

		Workshop::CSteamUpload* pWorkShop = new Workshop::CSteamUpload();

		pWorkShop->InitWorkshopUpdate(wszItem);

		while (!pWorkShop->IsFinished()) {
			SteamAPI_RunCallbacks();
			Sleep(1);
		}

		(pWorkShop->GetResult()) ? MessageBox(0, L"Your Workshop item has been stored!", APP_NAME, MB_ICONINFORMATION) : MessageBox(0, L"Failed to store Workshop item", APP_NAME, MB_ICONEXCLAMATION);

		delete pWorkShop;

		SteamAPI_Shutdown();
	}

	void UnknownExpressionHandler(const std::wstring& szCmdName)
	{
		//Handler for unknown expressions

		if ((pConsole) && (szCmdName.length())) {
			pConsole->AddLine(L"Unknown command: \"" + szCmdName + L"\"");
		}
	}

	void AS_MessageCallback(const asSMessageInfo* msg, void* param)
	{
		//Callback function for AngelScript message outputs

		Console::ConColor sColor(200, 200, 200);

		std::string szMsgType("Info");
		if (msg->type == asMSGTYPE_WARNING) {
			szMsgType = "Warning";
			sColor = Console::ConColor(150, 150, 0);
		}
		else if (msg->type == asMSGTYPE_ERROR) {
			szMsgType = "Error";
			sColor = Console::ConColor(250, 0, 0);
		}

		std::string szErrMsg = "[AngelScript][" + szMsgType + "] " + Utils::ExtractFileNameA(std::string(msg->section)) + " (" + std::to_string(msg->row) + ":" + std::to_string(msg->col) + "): " + msg->message + "\n";

		if (pConsole) {
			pConsole->AddLine(Utils::ConvertToWideString(szErrMsg), sColor);
		}
	}

	void Cmd_Exec(void)
	{
		std::wstring wszText = pConfigMgr->ExpressionItemValue(1);

		pConfigMgr->Execute(wszBasePath + wszText);
	}

	void Cmd_Restart(void)
	{
		pGame->m_bInAppRestart = true;
		pGame->Release();

		ShellExecute(0, L"open", (wszBasePath + L"restarter.bat").c_str(), L"", wszBasePath.c_str(), SW_HIDE);
	}

	void Cmd_Bind(void)
	{
		int vCode = _wtoi(pConfigMgr->ExpressionItemValue(1).c_str());
		std::wstring wszIdent = pConfigMgr->ExpressionItemValue(2);

		g_oInputMgr.AddKeyBinding(wszIdent, vCode);
	}

	void Cmd_Echo(void)
	{
		std::wstring wszText = pConfigMgr->ExpressionItemValue(1);

		if (pConsole) {
			pConsole->AddLine(wszText);
		}
	}

	void Cmd_PackageName(void)
	{
		pGame->m_sPackage.wszName = pConfigMgr->ExpressionItemValue(1);
	}

	void Cmd_PackageVersion(void)
	{
		pGame->m_sPackage.wszVersion = pConfigMgr->ExpressionItemValue(1);
	}

	void Cmd_PackageAuthor(void)
	{
		pGame->m_sPackage.wszAuthor = pConfigMgr->ExpressionItemValue(1);
	}

	void Cmd_PackageContact(void)
	{
		pGame->m_sPackage.wszContact = pConfigMgr->ExpressionItemValue(1);
	}

	void Cmd_MapIndex(void)
	{
		pGame->m_sPackage.wszMapIndex = pConfigMgr->ExpressionItemValue(1);
	}

	void Cmd_MapName(void)
	{
		pGame->m_sMap.wszName = pConfigMgr->ExpressionItemValue(1);
	}

	void Cmd_MapBackground(void)
	{
		pGame->m_sMap.wszBackground = pConfigMgr->ExpressionItemValue(1);
	}

	void Cmd_EnvSolidSprite(void)
	{
		std::wstring wszFile = pConfigMgr->ExpressionItemValue(1);
		int x = _wtoi(pConfigMgr->ExpressionItemValue(2).c_str());
		int y = _wtoi(pConfigMgr->ExpressionItemValue(3).c_str());
		int w = _wtoi(pConfigMgr->ExpressionItemValue(4).c_str());
		int h = _wtoi(pConfigMgr->ExpressionItemValue(5).c_str());
		float rot = (float)_wtof(pConfigMgr->ExpressionItemValue(6).c_str());
		int repeat = _wtoi(pConfigMgr->ExpressionItemValue(7).c_str());
		int dir = _wtoi(pConfigMgr->ExpressionItemValue(8).c_str());
		bool wall = pConfigMgr->ExpressionItemValue(9) == L"true";

		std::wstring wszFullFilePath = pGame->m_sPackage.wszPakPath + L"\\gfx\\" + wszFile;
		if (!Utils::FileExists(wszFullFilePath)) {
			wszFullFilePath = wszBasePath + L"packages\\.common\\gfx\\" + wszFile;
		}
		
		Entity::CSolidSprite oSprite;
		oSprite.Initialize(x, y, w, h, wszFullFilePath, repeat, dir, rot, wall);

		pGame->m_vSolidSprites.push_back(oSprite);
	}

	void Cmd_EntRequire(void)
	{
		std::wstring wszEntScript = pConfigMgr->ExpressionItemValue(1);

		pGame->RequireEntityScript(wszEntScript);
	}

	void Cmd_EntSpawn(void)
	{
		std::wstring wszEntityName = pConfigMgr->ExpressionItemValue(1);
		int x = _wtoi(pConfigMgr->ExpressionItemValue(2).c_str());
		int y = _wtoi(pConfigMgr->ExpressionItemValue(3).c_str());
		float rot = (float)_wtof(pConfigMgr->ExpressionItemValue(4).c_str());
		std::wstring wszProps = pConfigMgr->ExpressionItemValue(5);

		pGame->SpawnEntity(wszEntityName, x, y, rot, wszProps);
	}

	void Cmd_EnvGoal(void)
	{
		int x = _wtoi(pConfigMgr->ExpressionItemValue(1).c_str());
		int y = _wtoi(pConfigMgr->ExpressionItemValue(2).c_str());
		std::wstring wszGoal = pConfigMgr->ExpressionItemValue(3);

		pGame->SpawnGoal(x, y, wszGoal);
	}
}