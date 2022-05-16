#include "workshop.h"
#include "vars.h"

/*
	Casual Game Engine (dnyCasualGameEngine) developed by Daniel Brendel

	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

namespace Workshop {
	bool LoadWorkshopInfoData(const std::wstring& wszPackage, workshop_item_info_s& out)
	{
		//Store workshop info data into struct

		std::wifstream hFile;
		hFile.open((wszPackage + L"\\workshop_info.txt").c_str(), std::wifstream::in);
		if (hFile.is_open()) {
			wchar_t wszLine[1024];

			out.wszContent = wszPackage;

			while (!hFile.eof()) {
				hFile.getline(wszLine, sizeof(wszLine));
				std::wstring wsz = wszLine;

				if (wsz.find(L"packagename") != -1) {
					std::wstring wszArg = wsz.substr(wsz.find(L"packagename") + wcslen(L"packagename") + 1);
					out.wszPackageName = wszArg;
				} else if (wsz.find(L"title") != -1) {
					std::wstring wszArg = wsz.substr(wsz.find(L"title") + wcslen(L"title") + 1);
					out.wszTitle = wszArg;
				} else if (wsz.find(L"description") != -1) {
					std::wstring wszArg = wsz.substr(wsz.find(L"description") + wcslen(L"description") + 1);
					out.wszDescription = wszArg;
				} else if (wsz.find(L"visibility") != -1) {
					std::wstring wszArg = wsz.substr(wsz.find(L"visibility") + wcslen(L"visibility") + 1);
					out.bVisibility = (wszArg == L"true") || (wszArg == L"1");
				} else if (wsz.find(L"tags") != -1) {
					std::wstring wszArg = wsz.substr(wsz.find(L"tags") + wcslen(L"tags") + 1);
					out.wszTags = wszArg;
				} else if (wsz.find(L"preview") != -1) {
					std::wstring wszArg = wsz.substr(wsz.find(L"preview") + wcslen(L"preview") + 1);
					out.wszPreview = wszArg;
				} else if (wsz.find(L"changenote") != -1) {
					std::wstring wszArg = wsz.substr(wsz.find(L"changenote") + wcslen(L"changenote") + 1);
					out.wszChangeNote = wszArg;
				}
			}

			return true;
		}

		return false;
	}

	void CSteamUpload::OnCreateItemResult(CreateItemResult_t* pCallback, bool bIOFailure)
	{
		//Fired when item has been created

		if (pCallback) {
			if (pCallback->m_eResult != k_EResultOK) {
				this->m_bJobDone = true;
				this->m_bSuccess = false;
			} else {
				this->StoreWorkshopId(this->m_wszTargetPackage + L"\\workshop_id.txt", std::to_wstring(pCallback->m_nPublishedFileId));
				if (!this->StartItemUpdate(pCallback->m_nPublishedFileId)) {
					this->m_bJobDone = true;
					this->m_bSuccess = false;
				}
			}
		}
	}

	void CSteamUpload::OnSubmitItemUpdate(SubmitItemUpdateResult_t* pCallback, bool bIOFailure)
	{
		//Fired when item upload is complete

		if (pCallback) {
			this->m_bJobDone = true;
			this->m_bSuccess = pCallback->m_eResult == k_EResultOK;
			
			if (this->m_bSuccess) {
				//SteamFriends()->ActivateGameOverlayToWebPage(("steam://url/CommunityFilePage/" + std::to_string(pCallback->m_nPublishedFileId)).c_str());
				ShellExecute(0, L"open", (L"steam://url/CommunityFilePage/" + std::to_wstring(pCallback->m_nPublishedFileId)).c_str(), L"", L"", SW_SHOWNORMAL);
			}
		}
	}

	bool CSteamDownload::ProcessSubscribedItems(void)
	{
		//Process all subscribed Workshop items

		if (!this->m_bReady)
			return false;
		
		//Allocate array
		PublishedFileId_t* pArray = new PublishedFileId_t[this->m_pSteamUGC->GetNumSubscribedItems()];
		if (!pArray)
			return false;
		
		//Get all subscribed items
		uint32 uiPopulatedItems = this->m_pSteamUGC->GetSubscribedItems(pArray, this->m_pSteamUGC->GetNumSubscribedItems());
		if (!uiPopulatedItems) {
			delete[] pArray;
			return true;
		}

		//Iterate through items
		char pszFolder[MAX_PATH * 2];
		uint64 uiSizeOnDisk;
		uint32 uiTimeStamp;
		for (uint32 i = 0; i < uiPopulatedItems; i++) {
			if (this->m_pSteamUGC->GetItemInstallInfo(pArray[i], &uiSizeOnDisk, pszFolder, sizeof(pszFolder), &uiTimeStamp)) { //Get install info
				this->m_pfnEventFunc(Utils::ConvertToWideString(pszFolder));
			}
		}

		delete[] pArray;

		return true;
	}
}