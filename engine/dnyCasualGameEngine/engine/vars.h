#pragma once

/*
	Casual Game Engine (dnyCasualGameEngine) developed by Daniel Brendel

	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

#include "shared.h"
#include "renderer.h"
#include "sound.h"
#include "window.h"
#include "scriptint.h"
#include "configmgr.h"
#include "console.h"
#include "input.h"
#include "achievements.h"
#include "locale.h"

extern DxWindow::CDxWindow* pWindow;
extern DxRenderer::CDxRenderer* pRenderer;
extern DxSound::CDxSound* pSound;
extern Input::CDxInput* pInput;
extern Scripting::CScriptInt* pScriptingInt;
extern std::wstring wszBasePath;
extern ConfigMgr::CConfigInt* pConfigMgr;
extern Console::CConsole* pConsole;

extern ConfigMgr::CCVar::cvar_s* pAppName;
extern ConfigMgr::CCVar::cvar_s* pAppVersion;
extern ConfigMgr::CCVar::cvar_s* pAppAuthor;
extern ConfigMgr::CCVar::cvar_s* pAppContact;
extern ConfigMgr::CCVar::cvar_s* pAppSteamID;
extern ConfigMgr::CCVar::cvar_s* pAppLang;

extern ConfigMgr::CCVar::cvar_s* pGfxResolutionWidth;
extern ConfigMgr::CCVar::cvar_s* pGfxResolutionHeight;
extern ConfigMgr::CCVar::cvar_s* pGfxFullscreen;
extern ConfigMgr::CCVar::cvar_s* pSndVolume;
extern ConfigMgr::CCVar::cvar_s* pSndPlayMusic;

extern Input::CInputMgr g_oInputMgr;

extern Achievements::CSteamAchievements* pAchievements;

extern DxRenderer::d3dfont_s* pDefaultFont;
extern int iDefaultFontSize[2];

extern Localization::CLocalizationMgr oEngineLocaleMgr;
extern Localization::CLocalizationMgr oPackageLocaleMgr;
