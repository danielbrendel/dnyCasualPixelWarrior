#include "entity.h"
#include "console.h"
#include "game.h"

/*
	Casual Game Engine (dnyCasualGameEngine) developed by Daniel Brendel

	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

namespace Entity {
	bool CScriptedEntsMgr::Spawn(const std::wstring& wszIdent, asIScriptObject* pObject, const Vector& vAtPos)
	{
		//Spawn new entity
		
		//Query script ident
		Scripting::HSISCRIPT hScript = Game::pGame->GetScriptHandleByIdent(wszIdent);
		if (hScript == SI_INVALID_ID)
			return false;
		
		//Instantiate entity object
		CScriptedEntity* pEntity = new CScriptedEntity(hScript, pObject);
		if (!pEntity)
			return false;
		
		//Check if ready
		if (!pEntity->IsReady()) {
			delete pEntity;
			return false;
		}
		
		//Call spawn function if required
		pEntity->OnSpawn(vAtPos);
		
		//Add to list
		this->m_vEnts.push_back(pEntity);

		//Handle special entity case: player
		if (wszIdent == L"player") {
			this->m_sPlayerEntity.hScript = hScript;
			this->m_sPlayerEntity.pObject = pObject;
		}

		return true;
	}

	void CScriptedEntsMgr::Process(void)
	{
		//Inform entities

		for (size_t i = 0; i < this->m_vEnts.size(); i++) {
			//Skip dormant entities
			if (this->m_vEnts[i]->CanBeDormant()) {
				if (this->IsEntityDormant(this->m_vEnts[i])) {
					continue;
				}
			}

			//Let entity process
			this->m_vEnts[i]->OnProcess();

			//Handle collisions
			bool isCollidable = this->m_vEnts[i]->IsCollidable(); //Query indicator value of entity
			if (isCollidable) {
				//Query model
				CModel* pModel = this->m_vEnts[i]->GetModel();
				if (pModel) {
					//Check for collisions with any other entities
					for (size_t j = 0; j < this->m_vEnts.size(); j++) {
						if ((j != i) && (this->m_vEnts[j]->IsCollidable())) {
							CModel* pRef = this->m_vEnts[j]->GetModel();
							if (pRef) {
								//Check if collided
								if (pModel->IsCollided(this->m_vEnts[i]->GetPosition(), this->m_vEnts[j]->GetPosition(), *pRef)) {
									//Inform both of being collided
									this->m_vEnts[i]->OnCollided(this->m_vEnts[j]->Object());
									this->m_vEnts[j]->OnCollided(this->m_vEnts[i]->Object());
								}
							}
						}
					}
				}
			}

			//Check for removal
			if (this->m_vEnts[i]->NeedsRemoval()) {
				if (this->m_vEnts[i]->GetName() == "player") {
					Game::pGame->ShowGameOver();
					return;
				}
				
				this->m_vEnts[i]->OnRelease();
				delete this->m_vEnts[i];
				this->m_vEnts.erase(this->m_vEnts.begin() + i);
			}
		}
	}

	CScriptedEntsMgr oScriptedEntMgr;

	namespace APIFuncs { //API functions usable in scripts
		void Print(const std::string& in)
		{
			pConsole->AddLine(Utils::ConvertToWideString(in));
		}

		void Print2(const std::string& in, const Console::ConColor& clr)
		{
			pConsole->AddLine(Utils::ConvertToWideString(in), clr);
		}

		void FatalError(const std::string& in)
		{
			MessageBox(GetForegroundWindow(), Utils::ConvertToWideString(in).c_str(), APP_NAME L" :: Fatal error in tool script", MB_ICONERROR);
			exit(EXIT_FAILURE);
		}

		DxRenderer::d3dfont_s* GetDefaultFont(void)
		{
			return pDefaultFont;
		}

		DxRenderer::d3dfont_s* LoadFont(const std::string& szFontName, byte ucFontSizeW, byte ucFontSizeH)
		{
			return pRenderer->LoadFont(Utils::ConvertToWideString(szFontName), ucFontSizeW, ucFontSizeH);
		}

		bool GetSpriteInfo(const std::string& szFile, SpriteInfo& out)
		{
			//Get sprite info

			SpriteInfo sInfo;
			D3DXIMAGE_INFO sImageInfo;

			if (!pRenderer->GetSpriteInfo(Utils::ConvertToWideString(szFile), sImageInfo))
				return false;

			sInfo.res = Vector((int)sImageInfo.Width, (int)sImageInfo.Height);
			sInfo.depth = sImageInfo.Depth;
			sInfo.format = (int)sImageInfo.Format;

			return true;
		}

		DxRenderer::HD3DSPRITE LoadSprite(const std::string& szTexture, int iFrameCount, int iFrameWidth, int iFrameHeight, int iFramesPerLine, const bool bForceCustomSize)
		{
			return pRenderer->LoadSprite(Utils::ConvertToWideString(szTexture), iFrameCount, iFrameWidth, iFrameHeight, iFramesPerLine, bForceCustomSize);
		}

		bool FreeSprite(DxRenderer::HD3DSPRITE hSprite)
		{
			return pRenderer->FreeSprite(hSprite);
		}

		bool DrawBox(const Vector& pos, const Vector& size, int iThickness, const Color& color)
		{
			return pRenderer->DrawBox(pos[0], pos[1], size[0], size[1], iThickness, color.r, color.g, color.b, color.a);
		}

		bool DrawFilledBox(const Vector& pos, const Vector& size, const Color& color)
		{
			return pRenderer->DrawFilledBox(pos[0], pos[1], size[0], size[1], color.r, color.g, color.b, color.a);
		}

		bool DrawLine(const Vector& start, const Vector& end, const Color& color)
		{
			return pRenderer->DrawLine(start[0], start[1], end[0], end[1], color.r, color.g, color.b, color.a);
		}

		bool DrawSprite(const DxRenderer::HD3DSPRITE hSprite, const Vector& pos, int iFrame, float fRotation, const Vector& vRotPos, float fScale1, float fScale2, const bool bUseCustomColorMask, const Color& color)
		{
			return pRenderer->DrawSprite(hSprite, pos[0], pos[1], iFrame, fRotation, vRotPos[0], vRotPos[1], fScale1, fScale2, bUseCustomColorMask, color.r, color.g, color.b, color.a);
		}

		bool DrawString(const DxRenderer::d3dfont_s* pFont, const std::string& szText, const Vector& pos, const Color& color)
		{
			return pRenderer->DrawString(pFont, Utils::ConvertToWideString(szText), pos[0], pos[1], color.r, color.g, color.b, color.a);
		}

		void AddHudMessage(const std::string& szMsg, HudMessageColor color, int iDuration = HUDMSG_DEFAULDURATION)
		{
			Game::pGame->AddHudInfoMessage(Utils::ConvertToWideString(szMsg), color, iDuration);
		}

		std::string QueryLangPhrase(const std::string& szIdent, const std::string& szDefault = "")
		{
			return Utils::ConvertToAnsiString(oPackageLocaleMgr.QueryPhrase(Utils::ConvertToWideString(szIdent), Utils::ConvertToWideString(szDefault)));
		}

		void LoadMap(const std::string& szMapName)
		{
			//Game::pGame->LoadMap(Utils::ConvertToWideString(szMapName) + L".cfg");
			Game::pGame->InitStartGame(Game::pGame->GetPackageName(), Game::pGame->GetPackagePath(), Utils::ConvertToWideString(szMapName) + L".cfg");
		}

		void TriggerGameSave(void)
		{
			Game::pGame->SaveGame();
		}

		bool ShouldDraw(const Vector& vMyPos, const Vector& vMySize)
		{
			//Determine whether this entity is in viewport so it should be drawn

			Vector* vecPlayerPos = nullptr;
			Vector* vecPlayerSize = nullptr;
			
			pScriptingInt->CallScriptMethod(oScriptedEntMgr.GetPlayerEntity().hScript, oScriptedEntMgr.GetPlayerEntity().pObject, "Vector& GetPosition()", nullptr, &vecPlayerPos, Scripting::FA_OBJECT);
			pScriptingInt->CallScriptMethod(oScriptedEntMgr.GetPlayerEntity().hScript, oScriptedEntMgr.GetPlayerEntity().pObject, "Vector& GetSize()", nullptr, &vecPlayerSize, Scripting::FA_OBJECT);

			if ((!vecPlayerPos) || (!vecPlayerSize)) {
				return false;
			}

			Vector vDistance;
			vDistance[0] = vMyPos[0] - (*vecPlayerPos)[0];
			vDistance[1] = vMyPos[1] - (*vecPlayerPos)[1];

			return (vDistance[0] + vMySize[0] > pRenderer->GetWindowWidth() / 2 * -1) && (vDistance[0] < pRenderer->GetWindowWidth() / 2 + vMySize[0]) &&
				(vDistance[1] + vMySize[1] > pRenderer->GetWindowHeight() / 2 * -1) && (vDistance[1] < pRenderer->GetWindowHeight() / 2 + vMySize[1]);
		}

		void GetDrawingPosition(const Vector& vMyPos, const Vector& vMySize, Vector& out)
		{
			//Convert world position to drawing position on screen
			//Calculate distance of the position and add the center to it for each coordinate
			
			Vector* vecPlayerPos;
			Vector* vecPlayerSize;
			pScriptingInt->CallScriptMethod(oScriptedEntMgr.GetPlayerEntity().hScript, oScriptedEntMgr.GetPlayerEntity().pObject, "Vector& GetPosition()", nullptr, &vecPlayerPos, Scripting::FA_OBJECT);
			pScriptingInt->CallScriptMethod(oScriptedEntMgr.GetPlayerEntity().hScript, oScriptedEntMgr.GetPlayerEntity().pObject, "Vector& GetSize()", nullptr, &vecPlayerSize, Scripting::FA_OBJECT);

			out[0] = (vMyPos[0] - (*vecPlayerPos)[0]) + pRenderer->GetWindowWidth() / 2 - vMySize[0] / 2;
			out[1] = (vMyPos[1] - (*vecPlayerPos)[1]) + pRenderer->GetWindowHeight() / 2 - vMySize[1] / 2;
		}

		DxSound::HDXSOUND QuerySound(const std::string& szSoundFile)
		{
			return pSound->QuerySound(Utils::ConvertToWideString(szSoundFile));
		}

		bool PlaySound_(DxSound::HDXSOUND hSound, long lVolume, bool bLoop = false)
		{
			if (!Game::pGame->IsGameStarted()) {
				return false;
			}

			return pSound->Play(hSound, lVolume, ((bLoop) ? DSBPLAY_LOOPING : 0));
		}

		bool StopSound_(DxSound::HDXSOUND hSound)
		{
			return pSound->StopSound(hSound);
		}

		int GetCurrentVolume(void)
		{
			return pSndVolume->iValue;
		}

		int GetWindowCenterX(void)
		{
			return pRenderer->GetWindowWidth() / 2;
		}

		int GetWindowCenterY(void)
		{
			return pRenderer->GetWindowHeight() / 2;
		}

		bool SpawnScriptedEntity(const std::string& szIdent, asIScriptObject* ref, const Vector& vPos)
		{
			return oScriptedEntMgr.Spawn(Utils::ConvertToWideString(szIdent), ref, vPos);
		}

		void Ent_Move(asIScriptObject* ref, float fSpeed, MovementDir dir)
		{
			//Move entity according to view

			CScriptedEntity* pEntity = oScriptedEntMgr.GetEntity(oScriptedEntMgr.GetEntityId(ref)); //Get entity class object pointer
			if (pEntity) {
				//Query position and rotation
				Vector vecPosition = pEntity->GetPosition();
				float fRotation = pEntity->GetRotation();
				Vector vecSize = pEntity->GetSize();
				
				//Calculate forward or backward vector according to dir
				if (dir == MOVE_FORWARD) {
					vecPosition[0] += (int)(sin(fRotation + 0.015) * fSpeed) / Game::pGame->GetCurrentFramerate();
					vecPosition[1] -= (int)(cos(fRotation + 0.015) * fSpeed) / Game::pGame->GetCurrentFramerate();
				} else if (dir == MOVE_BACKWARD) {
					vecPosition[0] -= (int)(sin(fRotation + 0.015) * fSpeed) / Game::pGame->GetCurrentFramerate();
					vecPosition[1] += (int)(cos(fRotation + 0.015) * fSpeed) / Game::pGame->GetCurrentFramerate();
				} else if (dir == MOVE_LEFT) {
					vecPosition[0] += (int)(sin(fRotation + 80.0) * fSpeed) / Game::pGame->GetCurrentFramerate();
					vecPosition[1] -= (int)(cos(fRotation + 80.0) * fSpeed) / Game::pGame->GetCurrentFramerate();
				} else if (dir == MOVE_RIGHT) {
					vecPosition[0] -= (int)(sin(fRotation + 80.0) * fSpeed) / Game::pGame->GetCurrentFramerate();
					vecPosition[1] += (int)(cos(fRotation + 80.0) * fSpeed) / Game::pGame->GetCurrentFramerate();
				} else if (dir == MOVE_NORTH) {
					vecPosition[1] -= (int)(cos(0.015) * fSpeed) / Game::pGame->GetCurrentFramerate();
				} else if (dir == MOVE_SOUTH) {
					vecPosition[1] += (int)(cos(0.015) * fSpeed) / Game::pGame->GetCurrentFramerate();
				} else if (dir == MOVE_WEST) {
					vecPosition[0] -= (int)(cos(0.015) * fSpeed) / Game::pGame->GetCurrentFramerate();
				} else if (dir == MOVE_EAST) {
					vecPosition[0] += (int)(cos(0.015) * fSpeed) / Game::pGame->GetCurrentFramerate();
				}

				//If not collided then move forward
				if (!Game::pGame->IsVectorFieldInsideWall(vecPosition, vecSize)) {
					pEntity->SetPosition(vecPosition);
				} else {
					pEntity->OnWallCollided();
				}
			}
		}

		void SetGoalActivationStatus(bool bStatus)
		{
			CGoalEntity* pGoalEntity = Game::pGame->GetGoalEntity();
			if (pGoalEntity) {
				Game::pGame->GetGoalEntity()->SetActivationStatus(bStatus);
			}
		}

		size_t GetEntityCount()
		{
			return oScriptedEntMgr.GetEntityCount();
		}

		size_t GetEntityNameCount(const std::string& szName)
		{
			return oScriptedEntMgr.GetEntityNameCount(szName);
		}

		asIScriptObject* GetEntityHandle(size_t uiEntityId)
		{
			return oScriptedEntMgr.GetEntityHandle(uiEntityId);
		}

		asIScriptObject* EntityTrace(const Vector& vStart, const Vector& vEnd, asIScriptObject* pIgnoredEnt)
		{
			CEntityTrace oEntityTrace(vStart, vEnd, pIgnoredEnt);
			return ((oEntityTrace.IsEmpty()) ? nullptr : oEntityTrace.EntityObject(0));
		}

		bool Ent_IsValid(asIScriptObject* pEntity)
		{
			return oScriptedEntMgr.IsValidEntity(pEntity);
		}

		size_t Ent_GetId(asIScriptObject* pEntity)
		{
			return oScriptedEntMgr.GetEntityId(pEntity);
		}

		asIScriptObject* GetPlayerEntity(void)
		{
			return oScriptedEntMgr.GetPlayerEntity().pObject;
		}

		bool ListFilesByExt(const std::string& szBaseDir, asIScriptFunction* pFunction, const char** pFileList, const size_t uiListLen)
		{
			//List files by extension list

			WIN32_FIND_DATAA sFindData = { 0 };
			HANDLE hFileSearch;
			std::vector<std::string> vNames;

			//Initiate file search
			hFileSearch = FindFirstFileA((szBaseDir + "\\*.*").c_str(), &sFindData);
			if (hFileSearch == INVALID_HANDLE_VALUE)
				return false;

			while (FindNextFileA(hFileSearch, &sFindData)) { //While next file found in list
				if (sFindData.cFileName[0] == '.') //Ignore special folder
					continue;

				if ((sFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) { //If not a directory
					//Extract file ext and check with extension list
					std::string szFileExt = Utils::ExtractFileExt(sFindData.cFileName);
					for (size_t i = 0; i < uiListLen; i++) {
						if (szFileExt == pFileList[i]) {
							vNames.push_back(sFindData.cFileName);
						}
					}
				}
			}

			FindClose(hFileSearch);

			//For each found file with given extension call the script callback function
			for (size_t i = 0; i < vNames.size(); i++) {
				bool bResult = true;
				BEGIN_PARAMS(vArgs);
				PUSH_OBJECT(&vNames[i]);

				pScriptingInt->CallScriptFunction(pFunction, &vArgs, &bResult, Scripting::FA_BYTE);

				END_PARAMS(vArgs);

				if (!bResult)
					break;
			}

			pFunction->Release();

			return true;
		}

		bool ListSprites(const std::string& szBaseDir, asIScriptFunction* pFunction)
		{
			static const char* szSpriteExt[] = {
				{"png"}, {"bmp"}, {"jpg"}, {"jpeg" }
			};

			return ListFilesByExt(szBaseDir, pFunction, szSpriteExt, _countof(szSpriteExt));
		}
		
		bool ListSounds(const std::string& szBaseDir, asIScriptFunction* pFunction)
		{
			static const char* szSoundExt[] = {
				{ "wav" },{ "ogg" }
			};

			return ListFilesByExt(szBaseDir, pFunction, szSoundExt, _countof(szSoundExt));
		}

		int Random(int start, int end)
		{
			//Generate and return a random number

			static bool bRndSeedOnce = false;
			if (!bRndSeedOnce) {
				srand((unsigned int)time(nullptr));
				bRndSeedOnce = true;
			}

			return (rand() % (end - start)) + start;
		}

		std::string StrReplace(const std::string& szString, const std::string& szFind, const std::string& szNew)
		{
			return Utils::ReplaceString(szString, szFind, szNew);
		}

		std::string GetPackageName(void)
		{
			return Utils::ConvertToAnsiString(Game::pGame->GetPackageName());
		}

		std::string GetCurrentMap(void)
		{
			return Utils::ConvertToAnsiString(Game::pGame->GetCurrentMap());
		}

		std::string GetPackagePath(void)
		{
			return Utils::ConvertToAnsiString(Game::pGame->GetPackagePath());
		}

		std::string GetCommonPath(void)
		{
			return Utils::ConvertToAnsiString(wszBasePath + L"packages\\.common\\");
		}

		int GetKeyBinding(const std::string& szIdent)
		{
			return Game::pGame->GetKeyBinding(Utils::ConvertToWideString(szIdent));
		}

		std::string CreateProperty(const std::string& szIdent, const std::string& szValue)
		{
			return szIdent + ":" + szValue + ";";
		}

		std::string ExtractValueFromProperties(const std::string& szProperties, const std::string& szIdent)
		{
			//Extract value from property string by ident

			size_t uiPos = szProperties.find(szIdent + ":"); //Find position of delimiter
			if (uiPos != std::string::npos) {
				std::string szValue = "";

				//Copy chars from char next to delimiter until end-delimiter char
				for (size_t i = uiPos + szIdent.length() + 1; i < szProperties.length(); i++) {
					if (szProperties[i] == ';') {
						break;
					}

					szValue += szProperties[i];
				}

				return szValue;
			}

			return "";
		}

		bool SavePropsFile(const std::string& szProperties, const std::string& szFileName)
		{
			//Save properties to file

			std::string szFullFileName = Utils::ConvertToAnsiString(Game::pGame->GetPackagePath()) +  "props\\" + szFileName;

			if (!Utils::DirExists(Game::pGame->GetPackagePath() + L"props")) {
				CreateDirectory((Game::pGame->GetPackagePath() + L"props").c_str(), nullptr);
			}

			std::ofstream hFile;
			hFile.open(szFullFileName, std::ofstream::out);
			if (!hFile.is_open()) {
				return false;
			}

			std::vector<std::string> vItems = Utils::Split(szProperties, ";");

			for (size_t i = 0; i < vItems.size(); i++) {
				std::string szLine = vItems[i] + "\n";
				hFile.write(szLine.c_str(), szLine.length());
			}

			hFile.close();

			return true;
		}

		std::string GetPropsFileContent(const std::string& szFileName)
		{
			//Get properties file content

			std::string szFullFileName = Utils::ConvertToAnsiString(Game::pGame->GetPackagePath()) + "props\\" + szFileName;
			
			std::ifstream hFile;
			hFile.open(szFullFileName, std::ifstream::in);
			if (!hFile.is_open()) {
				return "";
			}

			std::string szResult = "";

			while (!hFile.eof()) {
				char szLineBuffer[1024 * 4] = { 0 };
				hFile.getline(szLineBuffer, sizeof(szLineBuffer), '\n');

				szResult += std::string(szLineBuffer) + ";";
			}

			hFile.close();

			return szResult;
		}

		void SetSteamAchievement(const std::string& szName)
		{
			pAchievements->UnlockAchievement(szName.c_str());
		}

		void SetSteamStatInt(const std::string& szName, int iValue)
		{
			pAchievements->SetStat(szName.c_str(), iValue);
		}

		void SetSteamStatFloat(const std::string& szName, float fValue)
		{
			pAchievements->SetStat(szName.c_str(), fValue);
		}

		bool IsSteamAchievementUnlocked(const std::string& szName)
		{
			return pAchievements->IsAchievementUnlocked(szName.c_str());
		}

		int GetSteamStatInt(const std::string& szName)
		{
			return pAchievements->GetStatInt(szName.c_str());
		}

		float GetSteamStatFloat(const std::string& szName)
		{
			return pAchievements->GetStatFloat(szName.c_str());
		}

		ConfigMgr::CCVar::cvar_s* RegisterCVar(const std::string& szName, ConfigMgr::CCVar::cvar_type_e eType, const std::string& szInitial)
		{
			return pConfigMgr->CCVar::Add(Utils::ConvertToWideString(szName), eType, Utils::ConvertToWideString(szInitial));
		}

		bool GetCVarBool(const std::string& szName, bool bFallback)
		{
			ConfigMgr::CCVar::cvar_s* pCVar = pConfigMgr->CCVar::Find(Utils::ConvertToWideString(szName));
			if (!pCVar) {
				return bFallback;
			}

			return pCVar->bValue;
		}

		int GetCVarInt(const std::string& szName, int iFallback)
		{
			ConfigMgr::CCVar::cvar_s* pCVar = pConfigMgr->CCVar::Find(Utils::ConvertToWideString(szName));
			if (!pCVar) {
				return iFallback;
			}

			return pCVar->iValue;
		}

		float GetCVarFloat(const std::string& szName, float fFallback)
		{
			ConfigMgr::CCVar::cvar_s* pCVar = pConfigMgr->CCVar::Find(Utils::ConvertToWideString(szName));
			if (!pCVar) {
				return fFallback;
			}

			return pCVar->fValue;
		}

		std::string GetCVarString(const std::string& szName, const std::string& szFallback)
		{
			ConfigMgr::CCVar::cvar_s* pCVar = pConfigMgr->CCVar::Find(Utils::ConvertToWideString(szName));
			if (!pCVar) {
				return szFallback;
			}

			return Utils::ConvertToAnsiString(pCVar->szValue);
		}

		void SetCVarBool(const std::string& szName, bool value)
		{
			ConfigMgr::CCVar::cvar_s* pCVar = pConfigMgr->CCVar::Find(Utils::ConvertToWideString(szName));
			if (pCVar) {
				pCVar->bValue = value;
			}
		}

		void SetCVarInt(const std::string& szName, int value)
		{
			ConfigMgr::CCVar::cvar_s* pCVar = pConfigMgr->CCVar::Find(Utils::ConvertToWideString(szName));
			if (pCVar) {
				pCVar->iValue = value;
			}
		}

		void SetCVarFloat(const std::string& szName, float value)
		{
			ConfigMgr::CCVar::cvar_s* pCVar = pConfigMgr->CCVar::Find(Utils::ConvertToWideString(szName));
			if (pCVar) {
				pCVar->fValue = value;
			}
		}

		void SetCVarString(const std::string& szName, const std::string& value)
		{
			ConfigMgr::CCVar::cvar_s* pCVar = pConfigMgr->CCVar::Find(Utils::ConvertToWideString(szName));
			if (pCVar) {
				wcscpy(pCVar->szValue, Utils::ConvertToWideString(value).c_str());
			}
		}

		bool ExecConfig(const std::string& szFile)
		{
			return pConfigMgr->Execute(Game::pGame->GetPackagePath() + Utils::ConvertToWideString(szFile));
		}

		void SetHUDEnableStatus(bool value)
		{
			Game::pGame->GetHUD()->SetEnableStatus(value);
		}

		void UpdateHUDHealth(size_t value)
		{
			Game::pGame->GetHUD()->UpdateHealth(value);
		}

		void AddHUDAmmoItem(const std::string& szIdent, const std::string& szSprite)
		{
			Game::pGame->GetHUD()->AddAmmoItem(Utils::ConvertToWideString(szIdent), Utils::ConvertToWideString(szSprite));
		}

		void AddHUDCollectable(const std::string& szIdent, const std::string& szSprite, bool bDrawAlways)
		{
			Game::pGame->GetHUD()->AddCollectable(Utils::ConvertToWideString(szIdent), Utils::ConvertToWideString(szSprite), bDrawAlways);
		}

		void UpdateHUDAmmoItem(const std::string& szIdent, size_t uiCurAmmo, size_t uiMaxAmmo)
		{
			Game::pGame->GetHUD()->UpdateAmmoItem(Utils::ConvertToWideString(szIdent), uiCurAmmo, uiMaxAmmo);
		}

		void UpdateHUDCollectable(const std::string& szIdent, size_t uiCurCount)
		{
			Game::pGame->GetHUD()->UpdateCollectable(Utils::ConvertToWideString(szIdent), uiCurCount);
		}

		size_t GetHUDAmmoItemCurrent(const std::string& szIdent)
		{
			return Game::pGame->GetHUD()->GetAmmoItemCurrent(Utils::ConvertToWideString(szIdent));
		}

		size_t GetHUDAmmoItemMax(const std::string& szIdent)
		{
			return Game::pGame->GetHUD()->GetAmmoItemMax(Utils::ConvertToWideString(szIdent));
		}

		size_t GetHUDCollectableCount(const std::string& szIdent)
		{
			return Game::pGame->GetHUD()->GetCollectableCount(Utils::ConvertToWideString(szIdent));
		}

		void SetHUDAmmoDisplayItem(const std::string& szIdent)
		{
			Game::pGame->GetHUD()->SetAmmoDisplayItem(Utils::ConvertToWideString(szIdent));
		}

		bool IsHUDEnabled(void)
		{
			return Game::pGame->GetHUD()->IsEnabled();
		}
	}

	void CSolidSprite::Draw(void)
	{
		//Draw solid sprite

		for (size_t i = 0; i < this->m_vSprites.size(); i++) { //Loop through sprite list
			//Calculate absolute x and y positions for current iterated entry

			int xpos = this->m_vecPos[0];
			int ypos = this->m_vecPos[1];

			if (this->m_iDir == 0) {
				xpos += (int)i * this->m_vecSize[0];
			} else {
				ypos += (int)i * this->m_vecSize[1];
			}
			
			//Check if is inside screen
			if (!APIFuncs::ShouldDraw(Vector(xpos, ypos), this->m_vecSize))
				continue;

			//Calculate drawing position and draw sprite

			Vector vecOut;
			APIFuncs::GetDrawingPosition(Vector(xpos, ypos), this->m_vecSize, vecOut);

			pRenderer->DrawSprite(this->m_vSprites[i], vecOut[0], vecOut[1], 0, this->m_fRotation);
		}
	}

	void CSolidSprite::Process(void)
	{
		//Process solid entity
	}

	void CGoalEntity::Draw(void)
	{
		//Draw entity sprite

		if (!this->m_bActivated)
			return;

		//Check if is inside screen
		if (!APIFuncs::ShouldDraw(this->m_vecPosition, this->m_vecSize))
			return;

		//Calculate drawing position and draw sprite

		Vector vecOut;
		APIFuncs::GetDrawingPosition(this->m_vecPosition, this->m_vecSize, vecOut);

		pRenderer->DrawSprite(this->m_vSprites[this->m_iCurrentFrame], vecOut[0], vecOut[1], 0, 0.0f);
	}

	void CSaveGameWriter::EndSaveGame(void)
	{
		//Finish file writing

		this->Release();

		Game::pGame->GetMenu()->AddToSaveGameList(Utils::ConvertToWideString(*this->m_pszFileName));

		delete this->m_pszFileName;
		this->m_pszFileName = nullptr;
	}

	bool Initialize(void)
	{
		//Initialize entity scripting

		//Create default font
		iDefaultFontSize[0] = 7;
		iDefaultFontSize[1] = 15;
		pDefaultFont = pRenderer->LoadFont(L"Verdana", iDefaultFontSize[0], iDefaultFontSize[1]);
		if (!pDefaultFont)
			return false;

		//Registration macros
		#define REG_ENUM(n, h) h = pScriptingInt->RegisterEnumeration(n); if (h == SI_INVALID_ID) { return false; }
		#define ADD_ENUM(h, n, v) if (!pScriptingInt->AddEnumerationValue(h, n, v))  { return false; }
		#define REG_TYPEDEF(t, n) if (!pScriptingInt->RegisterTypeDef(t, n))  { return false; }
		#define REG_FUNCDEF(t) if (!pScriptingInt->RegisterFuncDef(t))  { return false; }
		#define REG_STRUCT(n, s, h) h = pScriptingInt->RegisterStructure(n, s); if (h == SI_INVALID_ID)  { return false; }
		#define ADD_STRUCT(n, o, h) if (!pScriptingInt->AddStructureMember(h, n, o))  { return false; }
		#define REG_CLASSV(n, s, h) h = pScriptingInt->RegisterClass_(n, s, asOBJ_VALUE | asOBJ_APP_CLASS); if (h == SI_INVALID_ID)  { return false; }
		#define REG_CLASSR(n, s, h) h = pScriptingInt->RegisterClass_(n, s, asOBJ_REF); if (h == SI_INVALID_ID)  { return false; }
		#define REG_CLASSRT(n, s, h) h = pScriptingInt->RegisterClass_(n, s, asOBJ_REF | asOBJ_TEMPLATE); if (h == SI_INVALID_ID)  { return false; }
		#define REG_CLASSRNC(n, s, h) h = pScriptingInt->RegisterClass_(n, s, asOBJ_REF | asOBJ_NOCOUNT); if (h == SI_INVALID_ID)  { return false; }
		#define ADD_CLASSM(n, o, h) if (!pScriptingInt->AddClassMember(h, n, o))  { return false; }
		#define ADD_CLASSF(n, p, h) if (!pScriptingInt->AddClassMethod(h, n, p))  { return false; }
		#define ADD_CLASSB(b, t, p, h) if (!pScriptingInt->AddClassBehaviour(h, b, t, p))  { return false; }
		#define REG_FUNC(d, p, c) if (!pScriptingInt->RegisterFunction(d, p, c))  {return false; }
		#define REG_VAR(d, p) if (!pScriptingInt->RegisterGlobalVariable(d, p))  { return false; }
		#define REG_IF(n) if (!pScriptingInt->RegisterInterface(n))  { return false; }
		#define REG_IFM(n, m) if (!pScriptingInt->RegisterInterfaceMethod(n, m))  { return false; }

		//Register typedefs
		REG_TYPEDEF("uint64", "size_t");
		REG_TYPEDEF("uint16", "HostVersion");
		REG_TYPEDEF("uint64", "SpriteHandle");
		REG_TYPEDEF("uint64", "FontHandle");
		REG_TYPEDEF("uint64", "SoundHandle");
		REG_TYPEDEF("uint64", "CVarHandle");

		//Register enum
		Scripting::HSIENUM hEnum;
		REG_ENUM("MovementDir", hEnum);
		ADD_ENUM(hEnum, "MOVE_FORWARD", MOVE_FORWARD);
		ADD_ENUM(hEnum, "MOVE_BACKWARD", MOVE_BACKWARD);
		ADD_ENUM(hEnum, "MOVE_LEFT", MOVE_LEFT);
		ADD_ENUM(hEnum, "MOVE_RIGHT", MOVE_RIGHT);
		ADD_ENUM(hEnum, "MOVE_NORTH", MOVE_NORTH);
		ADD_ENUM(hEnum, "MOVE_SOUTH", MOVE_SOUTH);
		ADD_ENUM(hEnum, "MOVE_WEST", MOVE_WEST);
		ADD_ENUM(hEnum, "MOVE_EAST", MOVE_EAST);
		REG_ENUM("FileSeekWay", hEnum);
		ADD_ENUM(hEnum, "SEEKW_BEGIN", CFileReader::SEEKW_BEGIN);
		ADD_ENUM(hEnum, "SEEKW_CURRENT", CFileReader::SEEKW_CURRENT);
		ADD_ENUM(hEnum, "SEEKW_END", CFileReader::SEEKW_END);
		REG_ENUM("HudInfoMessageColor", hEnum);
		ADD_ENUM(hEnum, "HUD_MSG_COLOR_DEFAULT", 0);
		ADD_ENUM(hEnum, "HUD_MSG_COLOR_GREEN", HudMessageColor::HM_GREEN);
		ADD_ENUM(hEnum, "HUD_MSG_COLOR_RED", HudMessageColor::HM_RED);
		ADD_ENUM(hEnum, "HUD_MSG_COLOR_YELLOW", HudMessageColor::HM_YELLOW);
		ADD_ENUM(hEnum, "HUD_MSG_COLOR_BLUE", HudMessageColor::HM_BLUE);
		REG_ENUM("CVarType", hEnum);
		ADD_ENUM(hEnum, "CVAR_TYPE_BOOL", ConfigMgr::CCVar::CVAR_TYPE_BOOL);
		ADD_ENUM(hEnum, "CVAR_TYPE_INT", ConfigMgr::CCVar::CVAR_TYPE_INT);
		ADD_ENUM(hEnum, "CVAR_TYPE_FLOAT", ConfigMgr::CCVar::CVAR_TYPE_FLOAT);
		ADD_ENUM(hEnum, "CVAR_TYPE_STRING", ConfigMgr::CCVar::CVAR_TYPE_STRING);

		//Register function def
		REG_FUNCDEF("bool FuncFileListing(const string& in)");

		//Register classes
		Scripting::HSICLASS hClasses;
		REG_CLASSV("Color", sizeof(Color), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f()", asMETHOD(Color, Construct), hClasses);
		ADD_CLASSB(asBEHAVE_DESTRUCT, "void f()", asMETHOD(Color, Destruct), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f(uint8 r, uint8 g, uint8 b, uint8 a)", asMETHOD(Color, Constr_Bytes), hClasses);
		ADD_CLASSF("void opAssign(const Color &in)", asMETHODPR(Color, operator=, (const Color&), void), hClasses);
		ADD_CLASSF("uint8 R()", asMETHOD(Color, GetR), hClasses);
		ADD_CLASSF("uint8 G()", asMETHOD(Color, GetG), hClasses);
		ADD_CLASSF("uint8 B()", asMETHOD(Color, GetB), hClasses);
		ADD_CLASSF("uint8 A()", asMETHOD(Color, GetA), hClasses);
		REG_CLASSV("ConColor", sizeof(Console::ConColor), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f()", asMETHOD(Console::ConColor, Construct), hClasses);
		ADD_CLASSB(asBEHAVE_DESTRUCT, "void f()", asMETHOD(Console::ConColor, Destruct), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f(uint8 r, uint8 g, uint8 b)", asMETHOD(Console::ConColor, Constr_Bytes), hClasses);
		ADD_CLASSF("uint8 R()", asMETHOD(Console::ConColor, GetR), hClasses);
		ADD_CLASSF("uint8 G()", asMETHOD(Console::ConColor, GetG), hClasses);
		ADD_CLASSF("uint8 B()", asMETHOD(Console::ConColor, GetB), hClasses);
		REG_CLASSV("Vector", sizeof(Vector), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f()", asMETHOD(Vector, Construct), hClasses);
		ADD_CLASSB(asBEHAVE_DESTRUCT, "void f()", asMETHOD(Vector, Destruct), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f(Vector &in)", asMETHOD(Vector, Constr_Class), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f(int x, int y)", asMETHOD(Vector, Constr_Ints), hClasses);
		ADD_CLASSF("int opIndex(int) const", asMETHODPR(Vector, operator[], (int) const, int), hClasses);
		ADD_CLASSF("int& opIndex(int)", asMETHODPR(Vector, operator[], (int), int&), hClasses);
		ADD_CLASSF("void opAssign(const Vector &in)", asMETHODPR(Vector, operator=, (const Vector&), void), hClasses);
		ADD_CLASSF("bool opEquals(const Vector &in)", asMETHODPR(Vector, operator==, (const Vector&), bool), hClasses);
		ADD_CLASSF("void opPostInc(int)", asMETHODPR(Vector, operator++, (int), void), hClasses);
		ADD_CLASSF("void opPostDec(int)", asMETHODPR(Vector, operator--, (int), void), hClasses);
		ADD_CLASSF("Vector opAdd(const Vector &in)", asMETHODPR(Vector, operator+, (const Vector&), Vector), hClasses);
		ADD_CLASSF("Vector opSub(const Vector &in)", asMETHODPR(Vector, operator-, (const Vector&), Vector), hClasses);
		ADD_CLASSF("Vector opMul(const Vector &in)", asMETHODPR(Vector, operator*, (const Vector&), Vector), hClasses);
		ADD_CLASSF("Vector opDiv(const Vector &in)", asMETHODPR(Vector, operator/, (const Vector&), Vector), hClasses);
		ADD_CLASSF("int GetX() const", asMETHOD(Vector, GetX), hClasses);
		ADD_CLASSF("int GetY() const", asMETHOD(Vector, GetY), hClasses);
		ADD_CLASSF("int Distance(const Vector &in)", asMETHOD(Vector, Distance), hClasses);
		ADD_CLASSF("void Zero()", asMETHOD(Vector, Zero), hClasses);
		REG_CLASSV("SpriteInfo", sizeof(SpriteInfo), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f()", asMETHOD(SpriteInfo, Construct), hClasses);
		ADD_CLASSB(asBEHAVE_DESTRUCT, "void f()", asMETHOD(SpriteInfo, Destruct), hClasses);
		ADD_CLASSF("Vector& GetResolution()", asMETHOD(SpriteInfo, GetResolution), hClasses);
		ADD_CLASSF("int GetDepth()", asMETHOD(SpriteInfo, GetDepth), hClasses);
		ADD_CLASSF("int GetFormat()", asMETHOD(SpriteInfo, GetFormat), hClasses);
		REG_CLASSV("BoundingBox", sizeof(CBoundingBox), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f()", asMETHOD(CBoundingBox, Construct), hClasses);
		ADD_CLASSB(asBEHAVE_DESTRUCT, "void f()", asMETHOD(CBoundingBox, Destruct), hClasses);
		ADD_CLASSF("void opAssign(const BoundingBox &in)", asMETHODPR(CBoundingBox, operator=, (const CBoundingBox&), void), hClasses);
		ADD_CLASSF("bool Alloc()", asMETHOD(CBoundingBox, Alloc), hClasses);
		ADD_CLASSF("void AddBBoxItem(const Vector& in, const Vector& in)", asMETHOD(CBoundingBox, AddBBoxItem), hClasses);
		ADD_CLASSF("bool IsCollided(const Vector& in, const Vector& in, const BoundingBox& in)", asMETHOD(CBoundingBox, IsCollided), hClasses);
		ADD_CLASSF("bool IsInside(const Vector &in, const Vector &in)", asMETHOD(CBoundingBox, IsInside), hClasses);
		ADD_CLASSF("bool IsEmpty()", asMETHOD(CBoundingBox, IsEmpty), hClasses);
		ADD_CLASSF("void Clear()", asMETHOD(CBoundingBox, Clear), hClasses);
		REG_CLASSV("TempSprite", sizeof(CTempSprite), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f()", asMETHOD(CTempSprite, Construct), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f(const string &in, uint32 dwSwitchDelay, bool bInfinite, int iFrameCount, int iFrameWidth, int iFrameHeight, int iFramesPerLine, bool bForceCustomSize)", asMETHOD(CTempSprite, Constr_Init), hClasses);
		ADD_CLASSB(asBEHAVE_DESTRUCT, "void f()", asMETHOD(CTempSprite, Destruct), hClasses);
		ADD_CLASSF("bool Initialize(const string &in, uint32 dwSwitchDelay, bool bInfinite, int iFrameCount, int iFrameWidth, int iFrameHeight, int iFramesPerLine, bool bForceCustomSize)", asMETHOD(CTempSprite, Initialize), hClasses);
		ADD_CLASSF("void Draw(const Vector &in, float fRotation)", asMETHOD(CTempSprite, Draw), hClasses);
		ADD_CLASSF("void Release()", asMETHOD(CTempSprite, Release), hClasses);
		REG_CLASSV("Model", sizeof(CModel), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f()", asMETHOD(CModel, Construct), hClasses);
		ADD_CLASSB(asBEHAVE_DESTRUCT, "void f()", asMETHOD(CModel, Destruct), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f(const string &in)", asMETHOD(CModel, Constr_IntputStr), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f(const string &in, bool b)", asMETHOD(CModel, Constr_IntputStrBool), hClasses);
		ADD_CLASSF("void opAssign(const Model &in)", asMETHODPR(CModel, operator=, (const CModel&), void), hClasses);
		ADD_CLASSF("bool Initialize(const string&in szMdlFile, bool bForceCustomSize)", asMETHOD(CModel, Initialize), hClasses);
		ADD_CLASSF("bool Initialize2(const BoundingBox& in, SpriteHandle hSprite)", asMETHOD(CModel, Initialize2), hClasses);
		ADD_CLASSF("void Release()", asMETHOD(CModel, Release), hClasses);
		ADD_CLASSF("bool IsCollided(const Vector& in mypos, const Vector& in refpos, const Model& in mdl)", asMETHOD(CModel, IsCollided), hClasses);
		ADD_CLASSF("bool IsValid()", asMETHOD(CModel, IsValid), hClasses);
		ADD_CLASSF("bool Alloc()", asMETHOD(CModel, Alloc), hClasses);
		ADD_CLASSF("void SetCenter(const Vector &in)", asMETHOD(CModel, SetCenter), hClasses);
		ADD_CLASSF("SpriteHandle Handle()", asMETHOD(CModel, Sprite), hClasses);
		ADD_CLASSF("const Vector& GetCenter() const", asMETHOD(CModel, Center), hClasses);
		ADD_CLASSF("BoundingBox& GetBBox()", asMETHOD(CModel, BBox), hClasses);
		REG_CLASSV("Timer", sizeof(CTimer), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f()", asMETHOD(CTimer, Construct), hClasses);
		ADD_CLASSB(asBEHAVE_DESTRUCT, "void f()", asMETHOD(CTimer, Destruct), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f(uint32 delay)", asMETHOD(CTimer, Constr_Delay), hClasses);
		ADD_CLASSF("void Reset()", asMETHOD(CTimer, Reset), hClasses);
		ADD_CLASSF("void Update()", asMETHOD(CTimer, Update), hClasses);
		ADD_CLASSF("void SetActive(bool bStatus)", asMETHOD(CTimer, SetActive), hClasses);
		ADD_CLASSF("void SetDelay(uint32 delay)", asMETHOD(CTimer, SetDelay), hClasses);
		ADD_CLASSF("bool IsActive()", asMETHOD(CTimer, Started), hClasses);
		ADD_CLASSF("uint32 GetDelay()", asMETHOD(CTimer, Delay), hClasses);
		ADD_CLASSF("bool IsElapsed()", asMETHOD(CTimer, Elapsed), hClasses);
		REG_CLASSV("FileReader", sizeof(CFileReader), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f()", asMETHOD(CFileReader, Construct), hClasses);
		ADD_CLASSB(asBEHAVE_DESTRUCT, "void f()", asMETHOD(CFileReader, Destruct), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f(const string& in)", asMETHOD(CFileReader, Constr_Init), hClasses);
		ADD_CLASSF("bool Open(const string &in)", asMETHOD(CFileReader, Open), hClasses);
		ADD_CLASSF("bool IsOpen()", asMETHOD(CFileReader, IsOpen), hClasses);
		ADD_CLASSF("bool Eof()", asMETHOD(CFileReader, Eof), hClasses);
		ADD_CLASSF("void Seek(FileSeekWay from, int offset)", asMETHOD(CFileReader, Seek), hClasses);
		ADD_CLASSF("string GetLine()", asMETHOD(CFileReader, GetLine), hClasses);
		ADD_CLASSF("string GetEntireContent(bool bSkipNLChar = false)", asMETHOD(CFileReader, GetEntireContent), hClasses);
		ADD_CLASSF("void Close()", asMETHOD(CFileReader, Close), hClasses);
		REG_CLASSV("FileWriter", sizeof(CFileWriter), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f()", asMETHOD(CFileWriter, Construct), hClasses);
		ADD_CLASSB(asBEHAVE_DESTRUCT, "void f()", asMETHOD(CFileWriter, Destruct), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f(const string& in)", asMETHOD(CFileWriter, Constr_Init), hClasses);
		ADD_CLASSF("bool Open(const string &in, bool bAppend = true)", asMETHOD(CFileWriter, Open), hClasses);
		ADD_CLASSF("bool IsOpen()", asMETHOD(CFileWriter, IsOpen), hClasses);
		ADD_CLASSF("bool Eof()", asMETHOD(CFileWriter, Eof), hClasses);
		ADD_CLASSF("void Seek(FileSeekWay from, int offset)", asMETHOD(CFileWriter, Seek), hClasses);
		ADD_CLASSF("void Write(const string &in)", asMETHOD(CFileWriter, Write), hClasses);
		ADD_CLASSF("void WriteLine(const string &in)", asMETHOD(CFileWriter, WriteLine), hClasses);
		ADD_CLASSF("void Close()", asMETHOD(CFileWriter, Close), hClasses);
		REG_CLASSV("SaveGameWriter", sizeof(CSaveGameWriter), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f()", asMETHOD(CSaveGameWriter, Construct), hClasses);
		ADD_CLASSB(asBEHAVE_DESTRUCT, "void f()", asMETHOD(CSaveGameWriter, Destruct), hClasses);
		ADD_CLASSF("bool BeginSaveGame()", asMETHOD(CSaveGameWriter, BeginSaveGame), hClasses);
		ADD_CLASSF("bool WritePackage(const string &in szPackage)", asMETHOD(CSaveGameWriter, WritePackage), hClasses);
		ADD_CLASSF("bool WriteFromPath(const string &in szFromPath)", asMETHOD(CSaveGameWriter, WriteFromPath), hClasses);
		ADD_CLASSF("bool WriteMap(const string &in szMap)", asMETHOD(CSaveGameWriter, WriteMap), hClasses);
		ADD_CLASSF("bool WriteAttribute(const string &in szName, const string &in szValue)", asMETHOD(CSaveGameWriter, WriteAttribute), hClasses);
		ADD_CLASSF("void EndSaveGame()", asMETHOD(CSaveGameWriter, EndSaveGame), hClasses);
		REG_CLASSV("SaveGameReader", sizeof(CSaveGameReader), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f()", asMETHOD(CSaveGameReader, Construct), hClasses);
		ADD_CLASSB(asBEHAVE_DESTRUCT, "void f()", asMETHOD(CSaveGameReader, Destruct), hClasses);
		ADD_CLASSF("bool OpenSaveGameFile(const string &in szFile)", asMETHOD(CSaveGameReader, OpenSaveGameFile), hClasses);
		ADD_CLASSF("void AcquireSaveGameData()", asMETHOD(CSaveGameReader, AcquireSaveGameData), hClasses);
		ADD_CLASSF("string GetDataItem(const string &in szIdent)", asMETHOD(CSaveGameReader, GetDataItem), hClasses);
		ADD_CLASSF("void Close()", asMETHOD(CSaveGameReader, Close), hClasses);

		//Register interfaces

		REG_IF("IScriptedEntity");
		REG_IFM("IScriptedEntity", "void OnSpawn(const Vector& in vec)");
		REG_IFM("IScriptedEntity", "void OnRelease()");
		REG_IFM("IScriptedEntity", "void OnProcess()");
		REG_IFM("IScriptedEntity", "void OnDraw()");
		REG_IFM("IScriptedEntity", "void OnDrawOnTop()");
		REG_IFM("IScriptedEntity", "void OnWallCollided()");
		REG_IFM("IScriptedEntity", "bool IsCollidable()");
		REG_IFM("IScriptedEntity", "void OnCollided(IScriptedEntity@)");
		REG_IFM("IScriptedEntity", "Model& GetModel()");
		REG_IFM("IScriptedEntity", "Vector& GetPosition()");
		REG_IFM("IScriptedEntity", "void SetPosition(const Vector &in)");
		REG_IFM("IScriptedEntity", "Vector& GetSize()");
		REG_IFM("IScriptedEntity", "float GetRotation()");
		REG_IFM("IScriptedEntity", "void SetRotation(float)");
		REG_IFM("IScriptedEntity", "void OnDamage(uint32)");
		REG_IFM("IScriptedEntity", "bool NeedsRemoval()");
		REG_IFM("IScriptedEntity", "bool CanBeDormant()");
		REG_IFM("IScriptedEntity", "string GetName()");
		REG_IFM("IScriptedEntity", "string GetSaveGameProperties()");

		REG_IF("IPlayerEntity");
		REG_IFM("IPlayerEntity", "void OnKeyPress(int vKey, bool bDown)");
		REG_IFM("IPlayerEntity", "void OnMousePress(int key, bool bDown)");
		REG_IFM("IPlayerEntity", "void OnUpdateCursor(const Vector &in pos)");
		REG_IFM("IPlayerEntity", "void AddPlayerScore(int amount)");
		REG_IFM("IPlayerEntity", "int GetPlayerScore()");

		REG_IF("ICollectingEntity");
		REG_IFM("ICollectingEntity", "void AddHealth(uint health)");
		REG_IFM("ICollectingEntity", "void AddAmmo(const string &in ident, uint amount)");

		//Register scripting API

		struct script_api_func_s {
			std::string szDefinition;
			void* pFunction;
		} sGameAPIFunctions[] = {
			{ "string GetPackageName()", &APIFuncs::GetPackageName },
			{ "string GetCurrentMap()", &APIFuncs::GetCurrentMap },
			{ "string GetPackagePath()", &APIFuncs::GetPackagePath },
			{ "string GetCommonPath()", &APIFuncs::GetCommonPath },
			{ "void Print(const string& in)", &APIFuncs::Print },
			{ "void PrintClr(const string& in, const ConColor &in)", &APIFuncs::Print2 },
			{ "int GetKeyBinding(const string &in)", &APIFuncs::GetKeyBinding },
			{ "FontHandle R_LoadFont(const string& in, uint8 ucFontSizeW, uint8 ucFontSizeH)", &APIFuncs::LoadFont },
			{ "bool R_GetSpriteInfo(const string &in, SpriteInfo &out)", &APIFuncs::GetSpriteInfo },
			{ "SpriteHandle R_LoadSprite(const string& in szFile, int iFrameCount, int iFrameWidth, int iFrameHeight, int iFramesPerLine, bool bForceCustomSize)", &APIFuncs::LoadSprite },
			{ "bool R_FreeSprite(SpriteHandle hSprite)", &APIFuncs::FreeSprite },
			{ "bool R_DrawBox(const Vector& in pos, const Vector&in size, int iThickness, const Color&in color)", &APIFuncs::DrawBox },
			{ "bool R_DrawFilledBox(const Vector&in pos, const Vector&in size, const Color&in color)", &APIFuncs::DrawFilledBox },
			{ "bool R_DrawLine(const Vector&in start, const Vector&in end, const Color&in color)", &APIFuncs::DrawLine },
			{ "bool R_DrawSprite(const SpriteHandle hSprite, const Vector&in pos, int iFrame, float fRotation, const Vector &in vRotPos, float fScale1, float fScale2, bool bUseCustomColorMask, const Color&in color)", &APIFuncs::DrawSprite },
			{ "bool R_DrawString(const FontHandle font, const string&in szText, const Vector&in pos, const Color&in color)", &APIFuncs::DrawString },
			{ "bool R_ShouldDraw(const Vector &in vMyPos, const Vector &in vMySize)", APIFuncs::ShouldDraw },
			{ "void R_GetDrawingPosition(const Vector &in vMyPos, const Vector &in vMySize, Vector &out)", &APIFuncs::GetDrawingPosition },
			{ "FontHandle R_GetDefaultFont()", &APIFuncs::GetDefaultFont },
			{ "SoundHandle S_QuerySound(const string&in szSoundFile)", &APIFuncs::QuerySound },
			{ "bool S_PlaySound(SoundHandle hSound, int32 lVolume, bool bLoop = false)", &APIFuncs::PlaySound_ },
			{ "bool S_StopSound(SoundHandle hSound)", &APIFuncs::StopSound_ },
			{ "int S_GetCurrentVolume()", &APIFuncs::GetCurrentVolume },
			{ "int Wnd_GetWindowCenterX()", &APIFuncs::GetWindowCenterX },
			{ "int Wnd_GetWindowCenterY()", &APIFuncs::GetWindowCenterY },
			{ "bool Ent_SpawnEntity(const string &in, IScriptedEntity @obj, const Vector& in, bool bSpawn = true)", &APIFuncs::SpawnScriptedEntity },
			{ "size_t Ent_GetEntityCount()", &APIFuncs::GetEntityCount },
			{ "size_t Ent_GetEntityNameCount(const string &in szName)", &APIFuncs::GetEntityNameCount },
			{ "IScriptedEntity@+ Ent_GetEntityHandle(size_t uiEntityId)", &APIFuncs::GetEntityHandle },
			{ "IScriptedEntity@+ Ent_GetPlayerEntity()", &APIFuncs::GetPlayerEntity },
			{ "IScriptedEntity@+ Ent_TraceLine(const Vector&in vStart, const Vector&in vEnd, IScriptedEntity@+ pIgnoredEnt)", &APIFuncs::EntityTrace },
			{ "bool Ent_IsValid(IScriptedEntity@ pEntity)", &APIFuncs::Ent_IsValid },
			{ "size_t Ent_GetId(IScriptedEntity@ pEntity)", &APIFuncs::Ent_GetId },
			{ "void Ent_Move(IScriptedEntity@ pThis, float fSpeed, MovementDir dir)", &APIFuncs::Ent_Move },
			{ "void Ent_SetGoalActivationStatus(bool bStatus)", &APIFuncs::SetGoalActivationStatus },
			{ "bool Util_ListSprites(const string& in, FuncFileListing @cb)", &APIFuncs::ListSprites },
			{ "bool Util_ListSounds(const string& in, FuncFileListing @cb)", &APIFuncs::ListSounds },
			{ "int Util_Random(int start, int end)", &APIFuncs::Random },
			{ "string Util_StrReplace(const string& in szSource, const string &in szTarget, const string &in szNew)", &APIFuncs::StrReplace },
			{ "string Props_CreateProperty(const string &in ident, const string &in value)", &APIFuncs::CreateProperty },
			{ "string Props_ExtractValue(const string &in properties, const string &in ident)", &APIFuncs::ExtractValueFromProperties },
			{ "bool Props_SaveToFile(const string &in properties, const string &in fileName)", &APIFuncs::SavePropsFile },
			{ "string Props_GetFromFile(const string &in fileName)", &APIFuncs::GetPropsFileContent },
			{ "void Steam_SetAchievement(const string &in szName)", &APIFuncs::SetSteamAchievement },
			{ "void Steam_SetStat(const string &in szName, int iValue)", &APIFuncs::SetSteamStatInt },
			{ "void Steam_SetStat(const string &in szName, float fValue)", &APIFuncs::SetSteamStatFloat },
			{ "bool Steam_IsAchievementUnlocked(const string &in szName)", &APIFuncs::IsSteamAchievementUnlocked },
			{ "int Steam_GetStatInt(const string &in szName)", &APIFuncs::GetSteamStatInt },
			{ "float Steam_GetStatFloat(const string &in szName)", &APIFuncs::GetSteamStatFloat },
			{ "CVarHandle CVar_Register(const string &in szName, CVarType eType, const string &in szInitial)", &APIFuncs::RegisterCVar },
			{ "bool CVar_GetBool(const string &in szName, bool fallback)", APIFuncs::GetCVarBool },
			{ "int CVar_GetInt(const string &in szName, int fallback)", APIFuncs::GetCVarInt },
			{ "float CVar_GetFloat(const string &in szName, float fallback)", APIFuncs::GetCVarFloat },
			{ "string CVar_GetString(const string &in szName, const string &in fallback)", APIFuncs::GetCVarString },
			{ "void CVar_SetBool(const string &in szName, bool value)", APIFuncs::SetCVarBool },
			{ "void CVar_SetInt(const string &in szName, int value)", APIFuncs::SetCVarInt },
			{ "void CVar_SetFloat(const string &in szName, float value)", APIFuncs::SetCVarFloat },
			{ "void CVar_SetString(const string &in szName, const string &in value)", APIFuncs::SetCVarString },
			{ "bool ExecConfig(const string &in szFile)", APIFuncs::ExecConfig },
			{ "void HUD_SetEnableStatus(bool value)", APIFuncs::SetHUDEnableStatus },
			{ "void HUD_UpdateHealth(size_t value)", APIFuncs::UpdateHUDHealth },
			{ "void HUD_AddAmmoItem(const string &in szIdent, const string &in szSprite)", APIFuncs::AddHUDAmmoItem },
			{ "void HUD_AddCollectable(const string &in szIdent, const string &in szSprite, bool bDrawAlways)", APIFuncs::AddHUDCollectable },
			{ "void HUD_UpdateAmmoItem(const string &in szIdent, size_t uiCurAmmo, size_t uiMaxAmmo)", APIFuncs::UpdateHUDAmmoItem },
			{ "void HUD_UpdateCollectable(const string &in szIdent, size_t uiCurCount)", APIFuncs::UpdateHUDCollectable },
			{ "size_t HUD_GetAmmoItemCurrent(const string &in szIdent)", APIFuncs::GetHUDAmmoItemCurrent },
			{ "size_t HUD_GetAmmoItemMax(const string &in szIdent)", APIFuncs::GetHUDAmmoItemMax },
			{ "size_t HUD_GetCollectableCount(const string &in szIdent)", APIFuncs::GetHUDCollectableCount },
			{ "void HUD_SetAmmoDisplayItem(const string &in szIdent)", APIFuncs::SetHUDAmmoDisplayItem },
			{ "bool HUD_IsEnabled()", APIFuncs::IsHUDEnabled },
			{ "void HUD_AddMessage(const string &in msg, HudInfoMessageColor color, int duration = 3000)", &APIFuncs::AddHudMessage },
			{ "string Lang_QueryPhrase(const string &in szIdent, const string &in szDefault = \"\")", &APIFuncs::QueryLangPhrase },
			{ "string _(const string &in szIdent, const string &in szDefault = \"\")", &APIFuncs::QueryLangPhrase },
			{ "void LoadMap(const string &in map)", &APIFuncs::LoadMap },
			{ "void TriggerGameSave()", &APIFuncs::TriggerGameSave }
		};

		for (size_t i = 0; i < _countof(sGameAPIFunctions); i++) {
			if (!pScriptingInt->RegisterFunction(sGameAPIFunctions[i].szDefinition, sGameAPIFunctions[i].pFunction))
				return false;
		}
		
		return true;
	}
}