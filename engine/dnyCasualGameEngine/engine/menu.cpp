#include "menu.h"
#include "game.h"

/*
	Casual Game Engine (dnyCasualGameEngine) developed by Daniel Brendel

	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

namespace Menu {
	std::vector<palette_s> vMenuPaletteItems;

	void LoadPalette(void)
	{
		//Load palette from disk

		std::wifstream hFile;
		hFile.open(wszBasePath + L"palette.txt");
		if (hFile.is_open()) {
			wchar_t wszInputBuffer[2048] = { 0 };

			while (!hFile.eof()) {
				hFile.getline(wszInputBuffer, sizeof(wszInputBuffer), '\n');

				if ((wszInputBuffer[0] == '#') || (wcslen(wszInputBuffer) == 0)) {
					continue;
				}

				std::vector<std::wstring> vSplitData = Utils::SplitW(wszInputBuffer, L" ");
				if (vSplitData.size() == 5) {
					palette_s sData;
					sData.wszItem = vSplitData[0];
					sData.sColor = Entity::Color(_wtoi(vSplitData[1].c_str()), _wtoi(vSplitData[2].c_str()), _wtoi(vSplitData[3].c_str()), _wtoi(vSplitData[4].c_str()));
					vMenuPaletteItems.push_back(sData);
				}
			}

			hFile.close();
		}
	}

	Entity::Color GetPaletteItem(const std::wstring& wszItem, Entity::Color colDefault)
	{
		//Get palette color data

		for (size_t i = 0; i < vMenuPaletteItems.size(); i++) {
			if (vMenuPaletteItems[i].wszItem == wszItem) {
				return vMenuPaletteItems[i].sColor;
			}
		}

		return colDefault;
	}

	void CPlayMenu::OnButtonClick(class CButton* pButton)
	{
		if (pButton == &this->m_oPlay) {
			Game::pGame->InitStartGame(L"game");
		} /*else if (pButton == &this->m_oLoad) {
			if (this->m_oSaveGames.GetSelectedItem() != std::string::npos) {
				Game::pGame->LoadSavedGameState(this->m_oSaveGames.GetItem(this->m_oSaveGames.GetSelectedItem()));
			} else {
				Game::pGame->AddHudInfoMessage(oEngineLocaleMgr.QueryPhrase(L"app.playmenu.info.selectgamestate", L"Please select a saved game state"), Entity::HudMessageColor::HM_RED, 5000);
			}
		} else if (pButton == &this->m_oDelete) {
			if (this->m_oSaveGames.GetSelectedItem() != std::string::npos) {
				if (Game::pGame->DeleteSavedGameState(this->m_oSaveGames.GetItem(this->m_oSaveGames.GetSelectedItem()))) {
					this->m_oSaveGames.RemoveItem(this->m_oSaveGames.GetSelectedItem());
					Game::pGame->AddHudInfoMessage(oEngineLocaleMgr.QueryPhrase(L"app.playmenu.info.gamestatedeleted", L"The saved game state has been deleted successfully"), Entity::HudMessageColor::HM_GREEN, 5000);
				} else {
					Game::pGame->AddHudInfoMessage(oEngineLocaleMgr.QueryPhrase(L"app.playmenu.info.itemremovalfailure", L"Failed to delete the saved game state"), Entity::HudMessageColor::HM_RED, 5000);
				}
			} else {
				Game::pGame->AddHudInfoMessage(oEngineLocaleMgr.QueryPhrase(L"app.playmenu.info.selectgamestate", L"Please select a saved game state"), Entity::HudMessageColor::HM_RED, 5000);
			}
		}*/
	}

	void CPlayMenu::OnListBoxSelectEvent(class CListBox* pListBox, size_t uiItem)
	{
		pSound->Play(this->m_hSelect, pSndVolume->iValue, 0);
	}

	void CPackageMenu::OnMouseEvent(int x, int y, int iMouseKey, bool bDown, bool bCtrlHeld, bool bShiftHeld, bool bAltHeld)
	{
		if (!iMouseKey) {
			this->m_vecMousePos = Entity::Vector(x, y);
		}

		this->m_oButton.OnMouseEvent(x, y, iMouseKey, bDown, bCtrlHeld, bShiftHeld, bAltHeld);
		this->m_oImageListView.OnMouseEvent(x, y, iMouseKey, bDown, bCtrlHeld, bShiftHeld, bAltHeld);

		if (pAppSteamID->iValue != 0) {
			this->m_oBrowse.OnMouseEvent(x, y, iMouseKey, bDown, bCtrlHeld, bShiftHeld, bAltHeld);
		}

		if (this->m_vPackages.size() > 0) {
			if ((iMouseKey == 1) && (!bDown)) {
				if ((this->m_vecMousePos[0] > 350) && (this->m_vecMousePos[0] < 350 + 50) && (this->m_vecMousePos[1] > 200 + 45 + (int)this->m_vPackages[this->m_uiSelectedPackage].vAboutContent.size() * iDefaultFontSize[1] + 350) && (this->m_vecMousePos[1] < 200 + 45 + (int)this->m_vPackages[this->m_uiSelectedPackage].vAboutContent.size() * iDefaultFontSize[1] + 350 + 50)) {
					this->m_oImageListView.ScrollDown();
				}

				if ((this->m_vecMousePos[0] > 400) && (this->m_vecMousePos[0] < 400 + 50) && (this->m_vecMousePos[1] > 200 + 45 + (int)this->m_vPackages[this->m_uiSelectedPackage].vAboutContent.size() * iDefaultFontSize[1] + 350) && (this->m_vecMousePos[1] < 200 + 45 + (int)this->m_vPackages[this->m_uiSelectedPackage].vAboutContent.size() * iDefaultFontSize[1] + 350 + 50)) {
					this->m_oImageListView.ScrollUp();
				}
			}
		}
	}

	void CPackageMenu::OnButtonClick(class CButton* pButton)
	{
		if (pButton == &this->m_oButton) {
			if ((this->m_uiSelectedPackage >= 0) && (this->m_uiSelectedPackage < this->m_vPackages.size())) {
				Game::pGame->InitStartGame(this->m_vPackages[this->m_uiSelectedPackage].wszIdent, this->m_vPackages[this->m_uiSelectedPackage].wszPath);
				this->m_uiSelectedPackage = std::string::npos;
			}
		} else if (pButton == &this->m_oBrowse) {
			SteamFriends()->ActivateGameOverlayToWebPage(std::string("https://steamcommunity.com/app/" + std::to_string(pAppSteamID->iValue - 1) + "/workshop/").c_str());
		}
	}

	void CPackageMenu::OnImageSelected(CImageListView* pImageListView, size_t uiItemId)
	{
		const CImageListView::viewitem_s& viewItem = pImageListView->GetItem(uiItemId);

		for (size_t i = 0; i < this->m_vPackages.size(); i++) {
			if (this->m_vPackages[i].wszIdent == viewItem.wszIdent) {
				this->m_uiSelectedPackage = i;
				break;
			}
		}
	}

	void CSettingsKeys::OnButtonClick(class CButton* pButton)
	{
		if (pButton == &this->m_btnSave) {
			this->SaveBindings();
			Game::pGame->AddHudInfoMessage(oEngineLocaleMgr.QueryPhrase(L"app.settingsmenu.keys.info.saved", L"Settings saved!"), Entity::HudMessageColor::HM_GREEN);
		} else if (pButton == &this->m_btnRestoreDefaults) {
			this->RestoreDefaults();
			this->SaveBindings();
			Game::pGame->AddHudInfoMessage(oEngineLocaleMgr.QueryPhrase(L"app.settingsmenu.keys.info.defaultrestored", L"Default key settings restored!"), Entity::HudMessageColor::HM_GREEN);
		}
	}

	void CSettingsGfx::OnButtonClick(class CButton* pButton)
	{
		if (pButton == &this->m_btnSave) {
			this->SaveGfxSettings();
			this->SaveUISettings();
			Game::pGame->AddHudInfoMessage(oEngineLocaleMgr.QueryPhrase(L"app.settingsmenu.gfx.info.saved", L"Settings saved!"), Entity::HudMessageColor::HM_GREEN);
			pConfigMgr->Parse(L"restart");
		}
	}

	void CSettingsSnd::SaveSoundSettings(void)
	{
		//Save sound settings

		pSndVolume->iValue = this->m_oVolume.GetValue();
		pSndPlayMusic->bValue = this->m_oPlayMusic.IsChecked();

		std::wofstream hFile;
		hFile.open(wszBasePath + L"sound.cfg", std::wofstream::out);
		if (hFile.is_open()) {
			hFile << L"# Sound settings configuration file" << std::endl << std::endl;
			hFile << L"snd_volume " + std::to_wstring(this->m_oVolume.GetValue()) << std::endl;
			hFile << L"snd_playmusic " + std::to_wstring((int)this->m_oPlayMusic.IsChecked()) << std::endl;

			hFile.close();
		}

		Game::pGame->ToggleMenuTheme(pSndPlayMusic->bValue);
	}

	void CSettingsSnd::OnButtonClick(class CButton* pButton)
	{
		if (pButton == &this->m_btnSave) {
			this->SaveSoundSettings();
			Game::pGame->AddHudInfoMessage(oEngineLocaleMgr.QueryPhrase(L"app.settingsmenu.snd.info.saved", L"Settings saved!"), Entity::HudMessageColor::HM_GREEN);
		}
	}

	void CMenu::SetOpenStatus(bool status)
	{
		this->m_bOpen = status;

		if (status) {
			this->m_oMainMenu.OnToggleGameActiveMenuItems();

			pRenderer->SetBackgroundPicture(wszBasePath + L"media\\gfx\\background.jpg");
		} else {
			pRenderer->SetBackgroundPicture(Game::pGame->GetFullBackgroundFileName());
		}
	}

	void MainMenu_OnResumeGame(class CMenu* pMenu)
	{
		Game::pGame->ResumeGame();
	}

	void MainMenu_OnSaveGame(class CMenu* pMenu)
	{
		Game::pGame->SaveGame();
		Game::pGame->ResumeGame();
	}

	void MainMenu_OnStopGame(class CMenu* pMenu)
	{
		Game::pGame->StopGame();
	}

	void MainMenu_OnOpenPlay(class CMenu* pMenu)
	{
		pMenu->OpenPlayMenu();
	}

	void MainMenu_OnOpenPackages(class CMenu* pMenu)
	{
		pMenu->OpenPackageMenu();
	}

	void MainMenu_OnOpenSettings(class CMenu* pMenu)
	{
		pMenu->OpenSettingsMenu();
	}

	void MainMenu_OnQuitGame(class CMenu* pMenu)
	{
		Game::pGame->Release();
	}

	void CIntermissionMenu::OnButtonClick(class CButton* pButton)
	{
		Game::pGame->GetCursor()->SetActiveStatus(false);
		
		if (this->m_bGameFinished) {
			pConsole->AddLine(L"Game has finished!");
			Game::pGame->StopGame();
		} else {
			Game::pGame->LoadMap(Game::pGame->GetGoalEntity()->GetGoal() + L".cfg");
		}
	}

	void CGameOverMenu::OnButtonClick(class CButton* pButton)
	{
		Game::pGame->GetCursor()->SetActiveStatus(false);

		if (pButton == &this->m_oBtnRestart) {
			Game::pGame->InitRestartGame();
		} else if (pButton == &this->m_oBtnLoadLastSavedGame) {
			Game::pGame->QuickLoad();
		} else if (pButton == &this->m_oBtnReturnToMainMenu) {
			Game::pGame->StopGame();
		}
	}
}