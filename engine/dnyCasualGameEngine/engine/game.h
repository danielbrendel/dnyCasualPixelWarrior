#pragma once

/*
	Casual Game Engine (dnyCasualGameEngine) developed by Daniel Brendel

	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

#pragma warning(disable : 4996)

#include "renderer.h"
#include "sound.h"
#include "window.h"
#include "vars.h"
#include "entity.h"
#include "menu.h"
#include "input.h"
#include <steam_api.h>
#include "workshop.h"

/* Game specific environment */
namespace Game {
	extern class CGame* pGame;

	//Window event handler component
	struct CWindowEvents : public DxWindow::CDxWindow::IWindowEvents {
		virtual void OnCreated(HWND hWnd);
		virtual LRESULT OnKeyEvent(int vKey, bool bDown, bool bCtrlHeld, bool bShiftHeld, bool bAltHeld);
		virtual LRESULT OnMouseEvent(int x, int y, int iMouseKey, bool bDown, bool bCtrlHeld, bool bShiftHeld, bool bAltHeld);
		virtual void OnMouseWheel(short wDistance, bool bForward);
		virtual void OnDraw(void);
		virtual void OnRelease(void);
	};

	const int C_MENU_BANNER_WIDTH = 768;

	extern CWindowEvents oDxWindowEvents;

	void UnknownExpressionHandler(const std::wstring& szCmdName);
	void AS_MessageCallback(const asSMessageInfo* msg, void* param);

	void Cmd_PackageName(void);
	void Cmd_PackageVersion(void);
	void Cmd_PackageAuthor(void);
	void Cmd_PackageContact(void);
	void Cmd_MapIndex(void);
	void Cmd_MapName(void);
	void Cmd_MapBackground(void);
	void Cmd_EnvSolidSprite(void);
	void Cmd_EntRequire(void);
	void Cmd_EntSpawn(void);
	void Cmd_EnvGoal(void);
	void Cmd_Bind(void);
	void Cmd_Echo(void);
	void Cmd_Exec(void);
	void Cmd_Restart(void);

	void OnHandleWorkshopItem(const std::wstring& wszItem);
	void HandlePackageUpload(const std::wstring& wszArgs);

	class CGame : public Input::CDxInput::IInputEvents {
	private:
		struct package_s {
			std::wstring wszPakName;
			std::wstring wszPakPath;
			std::wstring wszName;
			std::wstring wszVersion;
			std::wstring wszAuthor;
			std::wstring wszContact;
			std::wstring wszMapIndex;
		};

		struct map_s {
			std::wstring wszName;
			std::wstring wszFileName;
			std::wstring wszBackground;
			std::wstring wszBackgroundFullPath;
		};

		struct player_s {
			Entity::Vector pos;
		};

		struct entityscript_s {
			Scripting::HSISCRIPT hScript;
			std::wstring wszIdent;
		};

		bool m_bInit;
		bool m_bGameStarted;
		DxRenderer::HD3DSPRITE m_hBanner;
		Menu::CMenu m_oMenu;
		package_s m_sPackage;
		map_s m_sMap;
		player_s m_sPlayerSpawn;
		std::vector<Entity::CSolidSprite> m_vSolidSprites;
		std::vector<entityscript_s> m_vEntityScripts;
		Entity::CGoalEntity* m_pGoalEntity;
		bool m_bGamePause;
		bool m_bShowIntermission;
		Menu::CIntermissionMenu m_oIntermissionMenu;
		Menu::CGameOverMenu m_oGameOverMenu;
		Menu::CCursor m_oCursor;
		Workshop::CSteamDownload* pSteamDownloader;
		DxRenderer::HD3DSPRITE m_hLoadingScreen;
		bool m_bInGameLoadingProgress;
		std::wstring m_wszCurrentLoadingPackage;
		std::wstring m_wszCurrentLoadingFromPath;
		std::wstring m_wszCurrentLoadingMap;
		bool m_bGameOver;
		Entity::CSaveGameReader m_oSaveGameReader;
		bool m_bLoadSavedGame;
		std::wstring m_szQuickLoadFile;
		Entity::CHudInfoMessages m_oHudInfoMessages;
		Entity::CHud* m_pHud;
		bool m_bInAppRestart;
		LONGLONG m_lFrequency;
		LONGLONG m_lLastCount;
		LONGLONG m_ilCurCount;
		int m_iFrameRate;
		int m_iFrames;
		size_t m_uiSkipFrame;
		DxSound::HDXSOUND m_hMenuTheme;

		friend void Cmd_PackageName(void);
		friend void Cmd_PackageVersion(void);
		friend void Cmd_PackageAuthor(void);
		friend void Cmd_PackageContact(void);
		friend void Cmd_MapIndex(void);
		friend void Cmd_MapName(void);
		friend void Cmd_MapBackground(void);
		friend void Cmd_EnvSolidSprite(void);
		friend void Cmd_EntSpawn(void);
		friend void Cmd_EntRequire(void);
		friend void Cmd_EnvGoal(void);
		friend void Cmd_Bind(void);
		friend void Cmd_Echo(void);
		friend void Cmd_Exec(void);
		friend void Cmd_Restart(void);

		bool LoadPackage(const std::wstring& wszPackage, const std::wstring& wszFromPath = L"", const std::wstring& wszFromMap = L"")
		{
			//Load package game

			std::wstring wszPackagePath = (!wszFromPath.length()) ? wszBasePath + L"packages\\" + wszPackage : wszFromPath;

			this->m_sPackage.wszPakName = wszPackage;
			this->m_sPackage.wszPakPath = wszPackagePath;

			oPackageLocaleMgr.SetLanguagePath(wszPackagePath + L"\\lang");
			oPackageLocaleMgr.SetLocale(pAppLang->szValue);
			
			//Check if package folder exists
			if (!Utils::DirExists(wszPackagePath)) {
				pConsole->AddLine(L"Package not found: " + wszPackage, Console::ConColor(255, 0, 0));
				return false;
			}
			
			//Check if package index config exists
			if (!Utils::FileExists(wszPackagePath + L"\\" + wszPackage + L".cfg")) {
				pConsole->AddLine(L"Package configuration script not found", Console::ConColor(255, 0, 0));
				return false;
			}

			//Execute package index config
			if (!pConfigMgr->Execute(wszPackagePath + L"\\" + wszPackage + L".cfg")) {
				pConsole->AddLine(L"Failed to execute package configuration script", Console::ConColor(255, 0, 0));
				return false;
			}

			pConsole->AddLine(L"Package " + this->m_sPackage.wszName + L" v" + this->m_sPackage.wszVersion + L" by " + this->m_sPackage.wszAuthor + L" (" + this->m_sPackage.wszContact + L")");

			//Overwrite index map if required
			if (wszFromMap.length() > 0) {
				pConsole->AddLine(L"wszFromMap: " + wszFromMap, Console::ConColor(0, 0, 150));
				this->m_sPackage.wszMapIndex = wszFromMap;
			}

			//Execute package index map file
			pConsole->AddLine(L"Executing: " + wszPackagePath + L"\\maps\\" + this->m_sPackage.wszMapIndex, Console::ConColor(255, 255, 255));
			if (!pConfigMgr->Execute(wszPackagePath + L"\\maps\\" + this->m_sPackage.wszMapIndex)) {
				pConsole->AddLine(L"Failed to execute package index map script", Console::ConColor(255, 0, 0));
				return false;
			}

			this->m_sMap.wszFileName = this->m_sPackage.wszMapIndex;

			//Set map background

			std::wstring wszBackgroundFile = wszPackagePath + L"\\gfx\\" + this->m_sMap.wszBackground;
			if (!Utils::FileExists(wszBackgroundFile)) {
				wszBackgroundFile = wszBasePath + L"packages\\.common\\gfx\\" + this->m_sMap.wszBackground;
			}

			this->m_sMap.wszBackgroundFullPath = wszBackgroundFile;

			pRenderer->SetBackgroundPicture(wszBackgroundFile);

			this->m_bGameStarted = true;

			return this->m_bGameStarted;
		}

		bool SpawnEntity(const std::wstring& wszName, int x, int y, float rot, const std::wstring& wszProps = L"")
		{
			//Spawn entity into world

			std::wstring wszFullScriptPath = this->m_sPackage.wszPakPath + L"\\entities\\" + wszName + L".as";
			if (!Utils::FileExists(wszFullScriptPath)) {
				wszFullScriptPath = wszBasePath + L"packages\\.common\\entities\\" + wszName + L".as";
			}

			//Check if entity script exists
			if (!Utils::FileExists(wszFullScriptPath)) {
				pConsole->AddLine(L"Entity script does not exist", Console::ConColor(255, 0, 0));
				return false;
			}

			//Load entity script if not already loaded
			Scripting::HSISCRIPT hScript;
			size_t uiScriptListId = this->FindScript(wszName);
			if (uiScriptListId == std::string::npos) {
				hScript = pScriptingInt->LoadScript(Utils::ConvertToAnsiString(wszFullScriptPath));
				if (hScript == SI_INVALID_ID) {
					pConsole->AddLine(L"Failed to load entity script: " + wszFullScriptPath, Console::ConColor(255, 0, 0));
					return false;
				}

				//Store data
				entityscript_s sEntScript;
				sEntScript.hScript = hScript;
				sEntScript.wszIdent = wszName;
				this->m_vEntityScripts.push_back(sEntScript);
				uiScriptListId = this->m_vEntityScripts.size() - 1;
			} else {
				hScript = this->m_vEntityScripts[uiScriptListId].hScript;
			}

			//Call CreateEntity() script function to let the entity be created

			Entity::Vector vecPos(x, y);
			std::string szIdent = Utils::ConvertToAnsiString(wszName);
			std::string szPath = Utils::ConvertToAnsiString(this->m_sPackage.wszPakPath + L"\\");
			std::string szProps = Utils::ConvertToAnsiString(wszProps);

			BEGIN_PARAMS(vArgs);
			PUSH_OBJECT(&vecPos);
			PUSH_FLOAT(rot);
			PUSH_OBJECT(&szIdent);
			PUSH_OBJECT(&szPath);
			PUSH_OBJECT(&szProps);
			
			bool bResult = pScriptingInt->CallScriptFunction(hScript, true, "CreateEntity", &vArgs, nullptr, Scripting::FA_VOID);

			END_PARAMS(vArgs);

			if (!bResult) {
				pConsole->AddLine(L"Failed to call CreateEntity() in script", Console::ConColor(255, 0, 0));
				pScriptingInt->UnloadScript(hScript);
				this->m_vEntityScripts.erase(this->m_vEntityScripts.begin() + uiScriptListId);
				return false;
			}

			return true;
		}

		bool RequireEntityScript(const std::wstring& wszName)
		{
			//Load entity script
			
			std::wstring wszFullFilePath = this->m_sPackage.wszPakPath + L"\\entities\\" + wszName + L".as";
			if (!Utils::FileExists(wszFullFilePath)) {
				wszFullFilePath = wszBasePath + L"packages\\.common\\entities\\" + wszName + L".as";
			}

			//Check if entity script exists
			if (!Utils::FileExists(wszFullFilePath)) {
				pConsole->AddLine(L"Entity script does not exist", Console::ConColor(255, 0, 0));
				return false;
			}

			//Load entity script if not already loaded
			Scripting::HSISCRIPT hScript;
			size_t uiScriptListId = this->FindScript(wszName);
			if (uiScriptListId == std::string::npos) {
				hScript = pScriptingInt->LoadScript(Utils::ConvertToAnsiString(wszFullFilePath));
				if (hScript == SI_INVALID_ID) {
					pConsole->AddLine(L"Failed to load entity script: " + wszFullFilePath, Console::ConColor(255, 0, 0));
					return false;
				}

				//Store data
				entityscript_s sEntScript;
				sEntScript.hScript = hScript;
				sEntScript.wszIdent = wszName;
				this->m_vEntityScripts.push_back(sEntScript);
			}

			return true;
		}

		void SpawnGoal(int x, int y, const std::wstring& wszGoal)
		{
			//Spawn goal entity

			//Free previous goal entity if was spawned
			if (this->m_pGoalEntity) {
				delete this->m_pGoalEntity;
				this->m_pGoalEntity = nullptr;
			}

			this->m_pGoalEntity = new Entity::CGoalEntity();
			this->m_pGoalEntity->SetPosition(x, y);
			this->m_pGoalEntity->SetGoal(wszGoal);
		}

		size_t FindScript(const std::wstring& wszName)
		{
			//Find script by name if loaded

			for (size_t i = 0; i < this->m_vEntityScripts.size(); i++) {
				if (this->m_vEntityScripts[i].wszIdent == wszName) {
					return i;
				}
			}

			return std::string::npos;
		}

		// Input events for DirectInput

		virtual void OnKeyDown(int vKey)
		{
			oDxWindowEvents.OnKeyEvent(vKey, true, false, false, false);
		}

		virtual void OnKeyUp(int vKey)
		{
			oDxWindowEvents.OnKeyEvent(vKey, false, false, false, false);
		}

		virtual void OnMouseMove(int x, int y)
		{
			oDxWindowEvents.OnMouseEvent(x, y, 0, false, false, false, false);
		}

		virtual void OnMouseKeyDown(int vKey)
		{
			oDxWindowEvents.OnMouseEvent(0, 0, vKey, true, false, false, false);
		}

		virtual void OnMouseKeyUp(int vKey)
		{
			oDxWindowEvents.OnMouseEvent(0, 0, vKey, false, false, false, false);
		}
	public:
		CGame() : m_bInit(false), m_bGameStarted(false), m_bGamePause(false), m_bShowIntermission(false), pSteamDownloader(nullptr), m_bInGameLoadingProgress(false), m_bGameOver(false), m_bLoadSavedGame(false), m_pHud(nullptr), m_bInAppRestart(false), m_iFrames(100), m_iFrameRate(100) { pGame = this; }
		~CGame() { pGame = nullptr; }

		bool Initialize(const std::wstring& wszPackage = L"", const std::wstring& wszMap = L"")
		{
			//Initialize game

			if (this->m_bInit) {
				return true;
			}

			//Get base game path

			wchar_t wszAppPath[2080];
			GetModuleFileName(0, wszAppPath, sizeof(wszAppPath));
			for (size_t i = wcslen(wszAppPath); i >= 0; i--) {
				if (wszAppPath[i] == '\\') {
					break;
				}

				wszAppPath[i] = 0;
			}
			
			wszBasePath = wszAppPath;

			//Initialize config manager

			pConfigMgr = new ConfigMgr::CConfigInt();
			if (!pConfigMgr) {
				this->Release();
				return false;
			}
			
			pConfigMgr->SetUnknownExpressionInformer(&UnknownExpressionHandler);

			//Add CVars
			pAppName = pConfigMgr->CCVar::Add(L"app_name", ConfigMgr::CCVar::CVAR_TYPE_STRING, APP_NAME);
			pAppVersion = pConfigMgr->CCVar::Add(L"app_version", ConfigMgr::CCVar::CVAR_TYPE_STRING, APP_VERSION);
			pAppAuthor = pConfigMgr->CCVar::Add(L"app_author", ConfigMgr::CCVar::CVAR_TYPE_STRING, APP_AUTHOR);
			pAppContact = pConfigMgr->CCVar::Add(L"app_contact", ConfigMgr::CCVar::CVAR_TYPE_STRING, APP_CONTACT);
			pAppSteamID = pConfigMgr->CCVar::Add(L"app_steamid", ConfigMgr::CCVar::CVAR_TYPE_INT, std::to_wstring(APP_STEAMID));
			pAppLang = pConfigMgr->CCVar::Add(L"app_language", ConfigMgr::CCVar::CVAR_TYPE_STRING, L"en");
			pGfxResolutionWidth = pConfigMgr->CCVar::Add(L"gfx_resolution_width", ConfigMgr::CCVar::CVAR_TYPE_INT, L"1024");
			pGfxResolutionHeight = pConfigMgr->CCVar::Add(L"gfx_resolution_height", ConfigMgr::CCVar::CVAR_TYPE_INT, L"768");
			pGfxFullscreen = pConfigMgr->CCVar::Add(L"gfx_fullscreen", ConfigMgr::CCVar::CVAR_TYPE_BOOL, L"1");
			pSndVolume = pConfigMgr->CCVar::Add(L"snd_volume", ConfigMgr::CCVar::CVAR_TYPE_INT, L"10");
			pSndPlayMusic = pConfigMgr->CCVar::Add(L"snd_playmusic", ConfigMgr::CCVar::CVAR_TYPE_BOOL, L"1");
			
			//Add commands
			pConfigMgr->CCommand::Add(L"exec", L"Execute a script file", &Cmd_Exec);
			pConfigMgr->CCommand::Add(L"bind", L"Bind command to key", &Cmd_Bind);
			pConfigMgr->CCommand::Add(L"echo", L"Print text to console", &Cmd_Echo);
			pConfigMgr->CCommand::Add(L"restart", L"Restart application", &Cmd_Restart);
			pConfigMgr->CCommand::Add(L"package_name", L"Package name", &Cmd_PackageName);
			pConfigMgr->CCommand::Add(L"package_version", L"Package version", &Cmd_PackageVersion);
			pConfigMgr->CCommand::Add(L"package_author", L"Package author", &Cmd_PackageAuthor);
			pConfigMgr->CCommand::Add(L"package_contact", L"Package contact information", &Cmd_PackageContact);
			pConfigMgr->CCommand::Add(L"map_index", L"Map to begin game with", &Cmd_MapIndex);
			pConfigMgr->CCommand::Add(L"map_name", L"Map name", &Cmd_MapName);
			pConfigMgr->CCommand::Add(L"map_background", L"Map background", &Cmd_MapBackground);
			pConfigMgr->CCommand::Add(L"env_solidsprite", L"Solid sprite placement", &Cmd_EnvSolidSprite);
			pConfigMgr->CCommand::Add(L"ent_spawn", L"Spawn scripted entity", &Cmd_EntSpawn);
			pConfigMgr->CCommand::Add(L"ent_require", L"Require entity script", &Cmd_EntRequire);
			pConfigMgr->CCommand::Add(L"env_goal", L"Spawn goal entity", &Cmd_EnvGoal);
			
			//Execute configuration scripts
			pConfigMgr->Execute(wszBasePath + L"app.cfg");
			pConfigMgr->Execute(wszBasePath + L"config.cfg");
			
			//Check special resolution vars
			if ((pGfxResolutionWidth->iValue == 0) && (pGfxResolutionHeight->iValue == 0)) {
				RECT sWindowRect;

				GetWindowRect(GetDesktopWindow(), &sWindowRect);
				pGfxResolutionWidth->iValue = sWindowRect.right;
				pGfxResolutionHeight->iValue = sWindowRect.bottom;
			}

			//Link with Steam

			if (pAppSteamID->iValue != 0) {
				if (SteamAPI_RestartAppIfNecessary(pAppSteamID->iValue)) {
					this->Release();
					return true;
				}

				if (!SteamAPI_Init()) {
					this->Release();
					return false;
				}
			}

			//Instantiate window manager
			pWindow = new DxWindow::CDxWindow();
			if (!pWindow) {
				this->Release();
				return false;
			}
			
			//Instantiate renderer
			pRenderer = new DxRenderer::CDxRenderer();
			if (!pRenderer) {
				this->Release();
				return false;
			}

			//Instantiate sound manager
			pSound = new DxSound::CDxSound();
			if (!pSound) {
				this->Release();
				return false;
			}

			//Instantiate input manager
			pInput = new Input::CDxInput();
			if (!pInput) {
				this->Release();
				return false;
			}
			
			//Initialize game window
			if (!pWindow->Initialize(pAppName->szValue, pGfxResolutionWidth->iValue, pGfxResolutionHeight->iValue, &oDxWindowEvents)) {
				this->Release();
				return false;
			}
			
			//Initialize renderer
			if (!pRenderer->Initialize(pWindow->GetHandle(), !pGfxFullscreen->bValue, pWindow->GetResolutionX(), pWindow->GetResolutionY(), 0, 0, 0, 255)) {
				this->Release();
				return false;
			}

			//Initialize sound
			if (!pSound->Initialize(pWindow->GetHandle())) {
				this->Release();
				return false;
			}

			//Initialize input
			if (!pInput->Initialize(pWindow->GetHandle(), this, pWindow->GetCurrentRect().left, pWindow->GetCurrentRect().top)) {
				this->Release();
				return false;
			}

			//Instantiate console manager
			pConsole = new Console::CConsole();
			if (!pConsole) {
				this->Release();
				return false;
			}
			
			//Initialize console
			if (!pConsole->Initialize(pRenderer, pGfxResolutionWidth->iValue, 500, CON_DEFAULT_MAXHISTORY, Console::ConColor(100, 100, 50))) {
				this->Release();
				return false;
			}

			//Instantiate and initialize AngelScript scripting interface
			pScriptingInt = new Scripting::CScriptInt("", &AS_MessageCallback);
			if (!pScriptingInt) {
				this->Release();
				return false;
			}
			
			//Initialize entity environment
			if (!Entity::Initialize()) {
				this->Release();
				return false;
			}

			//Initialize engine localization
			oEngineLocaleMgr.SetLanguagePath(wszBasePath + L"lang");
			oEngineLocaleMgr.SetLocale(pAppLang->szValue);

			//Load color palette
			Menu::LoadPalette();
			
			//Initialize menu
			if (!this->m_oMenu.Initialize(pGfxResolutionWidth->iValue, pGfxResolutionHeight->iValue, &this->m_bGameStarted)) {
				this->Release();
				return false;
			}

			if (pAppSteamID->iValue != 0) {
				//Instantiate Steam Workshop downloader object
				pSteamDownloader = new Workshop::CSteamDownload(&OnHandleWorkshopItem);
				if (!pSteamDownloader) {
					this->Release();
					return false;
				}

				//Process subscribed Workshop items
				if (!pSteamDownloader->ProcessSubscribedItems()) {
					this->Release();
					return false;
				}

				//Initialize Steam Achievements manager
				pAchievements = new Achievements::CSteamAchievements();
				if (!pAchievements) {
					this->Release();
					return false;
				}
			}

			//Load banner
			this->m_hBanner = pRenderer->LoadSprite(wszBasePath + L"media\\gfx\\banner.png", 1, 768, 150, 1, false);
			
			//Set game menu background and open the menu
			pRenderer->SetBackgroundPicture(wszBasePath + L"media\\gfx\\background.jpg");
			this->m_oMenu.SetOpenStatus(true);

			//Initialize intermission menu
			this->m_oIntermissionMenu.Initialize(400, 250, &this->m_bGameStarted);
			this->m_oIntermissionMenu.SetPosition(Entity::Vector(pWindow->GetResolutionX() / 2 - 200, pWindow->GetResolutionY() / 2 - 125));

			//Initialize game over menu
			this->m_oGameOverMenu.Initialize(400, 250, &this->m_bGameStarted);
			this->m_oGameOverMenu.SetPosition(Entity::Vector(pWindow->GetResolutionX() / 2 - 200, pWindow->GetResolutionY() / 2 - 125));

			//Initialize and activate cursor
			this->m_oCursor.Initialize();
			this->m_oCursor.SetActiveStatus(true);

			//Load loading screen image
			this->m_hLoadingScreen = pRenderer->LoadSprite(wszBasePath + L"media\\gfx\\game_loading.png", 1, pWindow->GetResolutionX(), pWindow->GetResolutionY(), 1, true);
			if (this->m_hLoadingScreen == GFX_INVALID_SPRITE_ID) {
				this->Release();
				return false;
			}

			//Initialize HUD message component
			this->m_oHudInfoMessages.Initialize();

			//Initialize game HUD
			this->m_pHud = new Entity::CHud();
			if (!this->m_pHud) {
				this->Release();
				return false;
			}

			//Handle menu theme
			
			this->m_hMenuTheme = pSound->QuerySound(wszBasePath + L"media\\sound\\menu.wav");

			if (pSndPlayMusic->bValue) {
				pSound->Play(this->m_hMenuTheme, pSndVolume->iValue, DSBPLAY_LOOPING);
			}

			//Create restart script
			Utils::CreateRestartScript();

			//Add info text
			pConsole->AddLine(std::wstring(pAppName->szValue) + L" v" + std::wstring(pAppVersion->szValue) + L" developed by " + std::wstring(pAppAuthor->szValue) + L" (" + std::wstring(pAppContact->szValue) + L")", Console::ConColor(100, 215, 255));
			pConsole->AddLine("Powered by " APP_NAME L" v" APP_VERSION L" developed by " APP_AUTHOR L" (" APP_CONTACT L")", Console::ConColor(200, 200, 200));
			pConsole->AddLine(L"");

			this->m_bInit = true;
			this->m_bGameStarted = false;
			this->m_bInGameLoadingProgress = false;
			
			//Load map of package if provided
			if ((wszPackage.length() > 0) && (wszMap.length() > 0)) {
				this->StartGame(wszPackage);
				this->LoadMap(wszMap);
			}

			return this->m_bInit;
		}

		bool StartGame(const std::wstring& wszPackage, const std::wstring& wszFromPath = L"", const std::wstring& wszFromMap = L"")
		{
			//Start a new game from package

			pConsole->AddLine(L"Starting new game using package \"" + wszPackage + L"\"...");

			if (wszFromPath.length() > 0) {
				pConsole->AddLine(L"Loading game from path: " + wszFromPath);
			}

			//Stop current game if running any
			if (this->m_bGameStarted) {
				this->StopGame();
				pConsole->AddLine(L"Stopped previous game");
			}

			//Stop menu theme
			pSound->StopSound(this->m_hMenuTheme);

			if (!this->m_bInit) {
				return false;
			}

			this->m_bInGameLoadingProgress = true;
			this->m_bGamePause = false;
			this->m_bGameOver = false;

			this->m_oMenu.SetOpenStatus(false);
			this->m_oCursor.SetActiveStatus(false);

			//Load package
			bool bResult = this->LoadPackage(wszPackage, wszFromPath, wszFromMap);
			if (!bResult) {
				this->m_oMenu.SetOpenStatus(true);
				this->m_oCursor.SetActiveStatus(true);

				this->AddHudInfoMessage(L"Failed to load game", Entity::HudMessageColor::HM_RED, 5000);

				return false;
			}

			this->m_bInGameLoadingProgress = false;

			return true;
		}

		void InitStartGame(const std::wstring& wszPackage, const std::wstring& wszFromPath = L"", const std::wstring& wszFromMap = L"")
		{
			//Init game start

			if (this->m_bGameStarted) {
				this->StopGame();
			}

			this->m_bInGameLoadingProgress = true;
			this->m_wszCurrentLoadingPackage = wszPackage;
			this->m_wszCurrentLoadingFromPath = wszFromPath;
			this->m_wszCurrentLoadingMap = wszFromMap;
			
			this->m_uiSkipFrame = 0;
		}

		void InitRestartGame(void)
		{
			//Init restart game

			if (this->m_bGameStarted) {
				this->StopGame();
			}

			this->m_wszCurrentLoadingMap = this->GetCurrentMap();

			this->InitStartGame(this->m_wszCurrentLoadingPackage, this->m_wszCurrentLoadingFromPath, this->m_wszCurrentLoadingMap);
		}

		void ShowGameOver(void)
		{
			//Activate game over menu

			this->m_bGameOver = true;
			this->m_bGamePause = true;

			this->m_oCursor.SetActiveStatus(true);
		}

		bool LoadMap(const std::wstring& wszMap);

		void StopGame(void);

		void LoadSavedGameState(const std::wstring& wszFile)
		{
			//Init loading saved game state from disk

			this->m_bLoadSavedGame = true;
			this->m_szQuickLoadFile = wszFile;

			this->m_oSaveGameReader.OpenSaveGameFile(Utils::ConvertToAnsiString(wszFile));
			this->m_oSaveGameReader.AcquireSaveGameData();

			std::string szPackage = this->m_oSaveGameReader.GetDataItem("package");
			std::string szFromPath = this->m_oSaveGameReader.GetDataItem("frompath");

			this->InitStartGame(Utils::ConvertToWideString(szPackage), Utils::ConvertToWideString(szFromPath));
		}

		bool DeleteSavedGameState(const std::wstring& wszFile)
		{
			//Delete a saved game state

			return DeleteFile((wszBasePath + L"saves\\" + wszFile).c_str()) == TRUE;
		}

		void QuickLoad(void)
		{
			//Init quick load
			
			this->StopGame();
			this->LoadSavedGameState(this->m_szQuickLoadFile);
		}

		void Process(void);
		void Draw(void);

		Scripting::HSISCRIPT GetScriptHandleByIdent(const std::wstring& wszIdent)
		{
			//Get script handle by ident

			for (size_t i = 0; i < this->m_vEntityScripts.size(); i++) {
				if (this->m_vEntityScripts[i].wszIdent == wszIdent) {
					return this->m_vEntityScripts[i].hScript;
				}
			}

			return SI_INVALID_ID;
		}

		bool IsVectorFieldInsideWall(const Entity::Vector& vecPos, const Entity::Vector& vecSize)
		{
			//Check if vector is inside wall

			for (size_t i = 0; i < this->m_vSolidSprites.size(); i++) {
				if (!this->m_vSolidSprites[i].IsWall()) {
					continue;
				}

				if (this->m_vSolidSprites[i].IsVectorFieldCollided(vecPos, vecSize)) {
					return true;
				}
			}

			return false;
		}

		int GetLocalPlayerScore(void)
		{
			//Get local player score

			const Entity::CScriptedEntsMgr::playerentity_s& playerEntity = Entity::oScriptedEntMgr.GetPlayerEntity();

			int iScore;

			pScriptingInt->CallScriptMethod(playerEntity.hScript, playerEntity.pObject, "int GetPlayerScore()", nullptr, &iScore, Scripting::FA_DWORD);

			return iScore;
		}

		void OnMouseEvent(int x, int y, int iMouseKey, bool bDown, bool bCtrlHeld, bool bShiftHeld, bool bAltHeld);
		void OnKeyEvent(int vKey, bool bDown, bool bCtrlHeld, bool bShiftHeld, bool bAltHeld);
		void OnMouseWheel(short wDistance, bool bForward);

		void ResumeGame(void)
		{
			//Resume game

			if (!this->m_bGameStarted) {
				return;
			}

			this->m_oMenu.SetOpenStatus(false);
			this->m_bGamePause = false;
		}

		void SaveGame(void)
		{
			if (GetFileAttributes((wszBasePath + L"saves").c_str()) == INVALID_FILE_ATTRIBUTES) {
				CreateDirectory((wszBasePath + L"saves").c_str(), nullptr);
			}

			size_t uiPlayerScript = this->FindScript(L"player");
			bool bResult = false;

			pScriptingInt->CallScriptFunction(this->m_vEntityScripts[uiPlayerScript].hScript, true, "SaveGame", nullptr, &bResult, Scripting::FA_BYTE);

			if (bResult) {
				this->m_oHudInfoMessages.AddMessage(oEngineLocaleMgr.QueryPhrase(L"app.savegame.success", L"Game saved!"), Entity::HudMessageColor::HM_GREEN);
			} else {
				this->m_oHudInfoMessages.AddMessage(oEngineLocaleMgr.QueryPhrase(L"app.savegame.failure", L"Failed to save game"), Entity::HudMessageColor::HM_RED);
			}
		}

		void Release(void)
		{
			//Release game component

			if (!this->m_bInit)
				return;

			//Clear indicators
			this->m_bGameStarted = false;
			this->m_bInit = false;

			//Stop current game
			this->StopGame();

			//Unlink from Steam
			SteamAPI_Shutdown();

			//Free cursor
			this->m_oCursor.Release();

			//Free memory
			FREE(this->m_pHud);
			FREE(pConsole);
			FREE(pInput);
			FREE(pSound);
			FREE(pRenderer);
			FREE(pWindow);
			FREE(pScriptingInt);
			FREE(pConfigMgr);
			FREE(pSteamDownloader);
			FREE(pAchievements);

			//Delete restarter script if not in restart progress
			if (!this->m_bInAppRestart) {
				DeleteFile((wszBasePath + L"restarter.bat").c_str());
			}
		}

		void AddPackage(const std::wstring& wszName, const std::wstring& wszPath)
		{
			this->m_oMenu.AddPackage(wszName, wszPath);
		}

		void AddHudInfoMessage(const std::wstring& wszMessage, const Entity::HudMessageColor eColor, int iDuration = Entity::HUDMSG_DEFAULDURATION)
		{
			this->m_oHudInfoMessages.AddMessage(wszMessage, eColor, iDuration);
		}

		void ToggleMenuTheme(bool value)
		{
			if (value) {
				if (!this->m_bGameStarted) {
					pSound->Play(this->m_hMenuTheme, pSndVolume->iValue, DSBPLAY_LOOPING);
				}
			} else {
				pSound->StopAll();
			}
		}

		//Return package name
		std::wstring GetPackageName(void) { return this->m_sPackage.wszPakName; }
		//Return current map name
		std::wstring GetCurrentMap(void) { return this->m_sMap.wszFileName; }
		//Return package path
		std::wstring GetPackagePath(void) { return (!this->m_sPackage.wszPakPath.length()) ? wszBasePath + L"packages\\" + this->m_sPackage.wszPakName + L"\\" : this->m_sPackage.wszPakPath + L"\\"; }
		//Return key binding
		int GetKeyBinding(const std::wstring& wszIdent) { return g_oInputMgr.GetKeyBindingCode(wszIdent); }
		//Get goal entity
		Entity::CGoalEntity* GetGoalEntity(void) { return this->m_pGoalEntity; }
		//Get cursor
		Menu::CCursor* GetCursor(void) { return &this->m_oCursor; }
		//Get main menu
		Menu::CMenu* GetMenu(void) { return &this->m_oMenu; }
		//Get HUD object
		Entity::CHud* GetHUD(void) { return this->m_pHud; }
		//Indicate if game is started
		bool IsGameStarted(void) { return this->m_bGameStarted; }
		//Get current frame rate
		int GetCurrentFramerate(void) { return this->m_iFrameRate; }
		//Get current full background file name
		std::wstring GetFullBackgroundFileName(void) { return this->m_sMap.wszBackgroundFullPath; }
	};
}