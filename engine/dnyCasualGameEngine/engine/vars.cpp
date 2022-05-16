#include "vars.h"

/*
	Casual Game Engine (dnyCasualGameEngine) developed by Daniel Brendel

	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

DxWindow::CDxWindow* pWindow = nullptr;
DxRenderer::CDxRenderer* pRenderer = nullptr;
DxSound::CDxSound* pSound = nullptr;
Input::CDxInput* pInput = nullptr;
Scripting::CScriptInt* pScriptingInt = nullptr;
std::wstring wszBasePath = L"";
ConfigMgr::CConfigInt* pConfigMgr = nullptr;
Console::CConsole* pConsole = nullptr;

ConfigMgr::CCVar::cvar_s* pAppName = nullptr;
ConfigMgr::CCVar::cvar_s* pAppVersion = nullptr;
ConfigMgr::CCVar::cvar_s* pAppAuthor = nullptr;
ConfigMgr::CCVar::cvar_s* pAppContact = nullptr;
ConfigMgr::CCVar::cvar_s* pAppSteamID = nullptr;
ConfigMgr::CCVar::cvar_s* pAppLang = nullptr;

ConfigMgr::CCVar::cvar_s* pGfxResolutionWidth = nullptr;
ConfigMgr::CCVar::cvar_s* pGfxResolutionHeight = nullptr;
ConfigMgr::CCVar::cvar_s* pGfxFullscreen = nullptr;
ConfigMgr::CCVar::cvar_s* pSndVolume = nullptr;
ConfigMgr::CCVar::cvar_s* pSndPlayMusic = nullptr;

Input::CInputMgr g_oInputMgr;

Achievements::CSteamAchievements* pAchievements = nullptr;

DxRenderer::d3dfont_s* pDefaultFont = nullptr;
int iDefaultFontSize[2];

Localization::CLocalizationMgr oEngineLocaleMgr;
Localization::CLocalizationMgr oPackageLocaleMgr;