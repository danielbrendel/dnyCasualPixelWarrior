#pragma once

/*
	Casual Game Engine (dnyCasualGameEngine) developed by Daniel Brendel

	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

#pragma warning(disable : 4996)

#include "shared.h"
#include "utils.h"
#include <steam_api.h>

namespace Workshop {
	struct workshop_item_info_s {
		std::wstring wszPackageName;
		std::wstring wszTitle;
		std::wstring wszDescription;
		bool bVisibility;
		std::wstring wszTags;
		std::wstring wszContent;
		std::wstring wszPreview;
		std::wstring wszChangeNote;
	};

	bool LoadWorkshopInfoData(const std::wstring& wszPackage, workshop_item_info_s& out);

	class CSteamUpload {
	private:
		std::wstring m_wszPath;
		ISteamUGC* m_pSteamUGC;
		bool m_bReady;
		bool m_bJobDone;
		bool m_bSuccess;
		std::wstring m_wszTargetPackage;

		PublishedFileId_t GetWorkshopId(const std::wstring& wszFile)
		{
			//Get workshop Id

			std::wifstream hFile;
			hFile.open(wszFile.c_str(), std::wifstream::in);
			if (hFile.is_open()) {
				wchar_t wcWsId[MAX_PATH];

				hFile.getline(wcWsId, sizeof(wcWsId));
				
				return _wcstoui64(wcWsId, nullptr, 10);
			}

			return 0;
		}

		bool StoreWorkshopId(const std::wstring& wszFile, const std::wstring& wszId)
		{
			//Store workshop item Id

			std::wofstream hFile;
			hFile.open(wszFile, std::wofstream::out);
			if (hFile.is_open()) {
				hFile << wszId << std::endl;

				return true;
			}

			return false;
		}

		/*void SplitTagString(const std::wstring& wszString, SteamParamStringArray_t& out)
		{
			//Split tags into Steam param string array

			std::wstring tmp = L"";
			int cnt = 0;
			std::vector<std::string> vTags;

			for (size_t i = 0; i < wszString.length(); i++) {
				if (wszString[i] == ' ')
					continue;

				if (wszString[i] == ',') {
					vTags.push_back(Utils::ConvertToAnsiString(tmp));
					cnt++;
					tmp.clear();
				}

				tmp += wszString[i];
			}

			out.m_nNumStrings = cnt;
			out.m_ppStrings = new char*[cnt];
			for (int i = 0; i < cnt; i++) {
				out.m_ppStrings[i] = new char[255];
				strcpy((char*)out.m_ppStrings[i], vTags[i].c_str());
			}
		}*/

		void OnCreateItemResult(CreateItemResult_t* pCallback, bool bIOFailure);
		CCallResult<CSteamUpload, CreateItemResult_t> m_CreateItemCallResult;

		void OnSubmitItemUpdate(SubmitItemUpdateResult_t* pCallback, bool bIOFailure);
		CCallResult<CSteamUpload, SubmitItemUpdateResult_t> m_SubmitItemUpdateCallResult;
	public:
		CSteamUpload()
		{
			//Construct object

			this->m_bReady = false;

			wchar_t wszPath[MAX_PATH];
			if (!GetModuleFileNameW(0, wszPath, sizeof(wszPath)))
				return;

			for (size_t i = wcslen(wszPath); i > 0; i--) {
				if (wszPath[i] == '\\')
					break;
				wszPath[i] = 0;
			}

			this->m_wszPath = wszPath;

			//if (!SteamAPI_Init())
			//	return;

			this->m_pSteamUGC = SteamUGC();
			if (!this->m_pSteamUGC)
				return;

			this->m_bReady = true;
		}

		~CSteamUpload()
		{
			//Release resources

			//SteamAPI_Shutdown();
			this->m_pSteamUGC = nullptr;
			this->m_bReady = false;
		}

		bool InitWorkshopUpdate(const std::wstring& wszPackage)
		{
			//Initialize workshop update of package

			this->m_bSuccess = false;
			this->m_bJobDone = false;
			this->m_wszTargetPackage = this->m_wszPath + L"packages\\" + wszPackage;

			if (Utils::FileExists(this->m_wszPath + L"packages\\" + wszPackage + L"\\workshop_id.txt")) { //Check if Workshop item shall be updated
				if (!this->StartItemUpdate(this->GetWorkshopId(this->m_wszPath + L"packages\\" + wszPackage + L"\\workshop_id.txt"))) { //Start item update
					this->m_bJobDone = true;
					this->m_bSuccess = false;
					return false;
				}
			} else { //Item shall be created
				this->m_bJobDone = false;
				
				SteamAPICall_t hCreateItem = this->m_pSteamUGC->CreateItem(SteamUtils()->GetAppID(), k_EWorkshopFileTypeCommunity); //Create new item
				if (!hCreateItem)
					return false;
				
				this->m_CreateItemCallResult.Set(hCreateItem, this, &CSteamUpload::OnCreateItemResult); //Listen for event
			}

			return true;
		}

		bool StartItemUpdate(PublishedFileId_t workshopId)
		{
			//Start update of item
			
			UGCUpdateHandle_t hUpdate = this->m_pSteamUGC->StartItemUpdate(SteamUtils()->GetAppID(), workshopId); //Start item update
			if (hUpdate) {
				workshop_item_info_s sData;
				if (!LoadWorkshopInfoData(this->m_wszTargetPackage, sData)) //Load Workshop data
					return false;
				
				//Set preview path
				if (sData.wszPreview == L"$FROM_PATH") {
					wchar_t wszAppPath[2080];
					GetModuleFileName(0, wszAppPath, sizeof(wszAppPath));
					for (size_t i = wcslen(wszAppPath); i >= 0; i--) {
						if (wszAppPath[i] == '\\') {
							break;
						}

						wszAppPath[i] = 0;
					}

					if (Utils::FileExists(std::wstring(wszAppPath) + L"packages\\" + sData.wszPackageName + L"\\preview.jpg")) {
						sData.wszPreview = std::wstring(wszAppPath) + L"packages\\" + sData.wszPackageName + L"\\preview.jpg";
					} else {
						sData.wszPreview = std::wstring(wszAppPath) + L"packages\\" + sData.wszPackageName + L"\\preview.png";
					}
				}

				//Set item data
				this->m_pSteamUGC->SetItemTitle(hUpdate, Utils::ConvertToAnsiString(sData.wszTitle).c_str());
				this->m_pSteamUGC->SetItemDescription(hUpdate, Utils::ConvertToAnsiString(sData.wszDescription).c_str());
				this->m_pSteamUGC->SetItemVisibility(hUpdate, (sData.bVisibility == true ? k_ERemoteStoragePublishedFileVisibilityPublic : k_ERemoteStoragePublishedFileVisibilityPrivate));
				this->m_pSteamUGC->SetItemContent(hUpdate, Utils::ConvertToAnsiString(sData.wszContent).c_str());
				this->m_pSteamUGC->SetItemPreview(hUpdate, Utils::ConvertToAnsiString(sData.wszPreview).c_str());

				//Submit item update
				SteamAPICall_t hSubmitUpdate = this->m_pSteamUGC->SubmitItemUpdate(hUpdate, Utils::ConvertToAnsiString(sData.wszChangeNote).c_str());
				if (!hSubmitUpdate) {
					this->m_bJobDone = true;
					this->m_bSuccess = false;
				}

				//Listen for event
				this->m_SubmitItemUpdateCallResult.Set(hSubmitUpdate, this, &CSteamUpload::OnSubmitItemUpdate);
				
				return true;
			}

			return false;
		}

		bool IsFinished()
		{
			//Whether job of workshop item upload is finished

			return this->m_bJobDone;
		}

		bool GetResult()
		{
			//Return result

			return this->m_bSuccess;
		}
	};

	class CSteamDownload {
	public:
		typedef void(*TpfnOnHandleWorkshopItem)(const std::wstring& wszItem);
	private:
		std::wstring m_wszPath;
		ISteamUGC* m_pSteamUGC;
		bool m_bReady;
		TpfnOnHandleWorkshopItem m_pfnEventFunc;
	public:
		CSteamDownload() {}
		CSteamDownload(TpfnOnHandleWorkshopItem pfnEvent)
		{
			//Construct object

			this->m_bReady = false;
			this->m_pfnEventFunc = pfnEvent;

			wchar_t wszPath[MAX_PATH];
			if (!GetModuleFileNameW(0, wszPath, sizeof(wszPath)))
				return;

			for (size_t i = wcslen(wszPath); i > 0; i--) {
				if (wszPath[i] == '\\')
					break;
				wszPath[i] = 0;
			}

			this->m_wszPath = wszPath;

			//if (!SteamAPI_Init())
			//	return;

			this->m_pSteamUGC = SteamUGC();
			if (!this->m_pSteamUGC)
				return;

			this->m_bReady = true;
		}

		~CSteamDownload()
		{
			//Release resources

			//SteamAPI_Shutdown();
			this->m_bReady = false;
		}

		bool ProcessSubscribedItems(void);
	};
}