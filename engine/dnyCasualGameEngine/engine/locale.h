#pragma once

/*
	Casual Game Engine (dnyCasualGameEngine) developed by Daniel Brendel

	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

#include "shared.h"

/* Localization environment */
namespace Localization {
	class CLocalizationMgr {
	private:
		struct phrase_s {
			std::wstring wszIdent;
			std::wstring wszPhrase;
		};

		struct langfile_s {
			std::wstring wszFile;
			std::vector<phrase_s> vPhrases;
		};

		struct locale_s {
			std::wstring wszLocale;
			std::vector<langfile_s> vFiles;
		};

		std::wstring m_wszLocale;
		std::wstring m_wszLangPath;
		std::vector<locale_s> m_vLocales;

		size_t FindLocale(const std::wstring& wszLocale)
		{
			//Find locale and return ID if found

			for (size_t i = 0; i < this->m_vLocales.size(); i++) {
				if (this->m_vLocales[i].wszLocale == wszLocale) {
					return i;
				}
			}

			return std::string::npos;
		}

		size_t FindLocaleEx(const std::wstring& wszLocale, const std::wstring& wszIdent)
		{
			//More specific locale finding

			for (size_t i = 0; i < this->m_vLocales.size(); i++) {
				if (this->m_vLocales[i].wszLocale == wszLocale) {
					//Find position of delimiter char
					size_t uiDelim = wszIdent.find(L".");
					if (uiDelim == std::string::npos) {
						return std::string::npos;
					}

					//Extract file ident
					std::wstring wszFile = wszIdent.substr(0, uiDelim);

					//Search for specified file
					for (size_t j = 0; j < this->m_vLocales[i].vFiles.size(); j++) {
						if (this->m_vLocales[i].vFiles[j].wszFile == wszFile) {
							return i;
						}
					}
				}
			}

			return std::string::npos;
		}

		std::wstring FindLocalePhrase(const std::wstring& wszIdent, const std::wstring& wszDefault = L"")
		{
			//Find locale phrase of current locale

			//Find locale entry in locale list
			size_t uiLocaleId = this->FindLocale(this->m_wszLocale);
			if (uiLocaleId == std::string::npos) {
				return wszDefault;
			}

			//Find position of delimiter char
			size_t uiDelim = wszIdent.find(L".");
			if (uiDelim == std::string::npos) {
				return wszDefault;
			}

			//Extract file and phrase ident
			std::wstring wszFile = wszIdent.substr(0, uiDelim);
			std::wstring wszPhraseIdent = wszIdent.substr(uiDelim + 1);
			
			//Loop trough phrases of locale file and search for ident
			for (size_t i = 0; i < this->m_vLocales[uiLocaleId].vFiles.size(); i++) {
				if (this->m_vLocales[uiLocaleId].vFiles[i].wszFile == wszFile) {
					for (size_t j = 0; j < this->m_vLocales[uiLocaleId].vFiles[i].vPhrases.size(); j++) {
						if (this->m_vLocales[uiLocaleId].vFiles[i].vPhrases[j].wszIdent == wszPhraseIdent) {
							return this->m_vLocales[uiLocaleId].vFiles[i].vPhrases[j].wszPhrase;
						}
					}
				}
			}

			return wszDefault;
		}

		bool ExtractIdentAndPhrase(const std::wstring& wszBuffer, phrase_s& rOut)
		{
			//Extract ident and phrase from buffer

			size_t uiDelim = wszBuffer.find(L":");
			if (uiDelim == std::string::npos) {
				return false;
			}

			rOut.wszIdent = wszBuffer.substr(0, uiDelim);
			rOut.wszPhrase = wszBuffer.substr(uiDelim + 1);

			return true;
		}

		bool AddLocaleFile(const std::wstring& wszIdent)
		{
			//Add locale file

			//Get current locale ID
			size_t uiLocaleId = this->FindLocale(this->m_wszLocale);

			//Find position of delimiter char
			size_t uiDelim = wszIdent.find(L".");
			if (uiDelim == std::string::npos) {
				return false;
			}

			//Extract file ident
			std::wstring wszFile = wszIdent.substr(0, uiDelim);

			//Open file for reading
			std::wifstream hFile;
			hFile.open(this->m_wszLangPath + L"\\" + this->m_wszLocale + L"\\" + wszFile + L".lng", std::wifstream::in);
			if (hFile.is_open()) {
				langfile_s sLangData;
				sLangData.wszFile = wszFile;

				while (!hFile.eof()) {
					wchar_t wszLineBuffer[4096] = { 0 };
					hFile.getline(wszLineBuffer, sizeof(wszLineBuffer), '\n');

					if ((wszLineBuffer[0] == '#') || (wcslen(wszLineBuffer) == 0)) {
						continue;
					}

					phrase_s sPhrase;
					this->ExtractIdentAndPhrase(wszLineBuffer, sPhrase);
					sLangData.vPhrases.push_back(sPhrase);
				}

				if (uiLocaleId == std::string::npos) {
					locale_s sLocale;
					sLocale.wszLocale = this->m_wszLocale;
					sLocale.vFiles.push_back(sLangData);
					this->m_vLocales.push_back(sLocale);
				} else {
					this->m_vLocales[uiLocaleId].vFiles.push_back(sLangData);
				}

				hFile.close();

				return true;
			}

			return false;
		}
	public:
		CLocalizationMgr() : m_wszLocale(L"en") {}
		~CLocalizationMgr() {}

		std::wstring QueryPhrase(const std::wstring& wszIdent, const std::wstring& wszDefault = L"")
		{
			//Query phrase of locale

			size_t uiLocaleId = this->FindLocaleEx(this->GetLocale(), wszIdent);
			if (uiLocaleId == std::string::npos) {
				this->AddLocaleFile(wszIdent);
			} 
			
			return this->FindLocalePhrase(wszIdent, wszDefault);
		}

		void SetLocale(const std::wstring& wszLocale)
		{
			//Set current locale

			this->m_wszLocale = wszLocale;
		}

		void SetLanguagePath(const std::wstring& wszPath)
		{
			//Set path to language folder

			this->m_wszLangPath = wszPath;
		}

		//Getters
		std::wstring GetLocale(void) { return this->m_wszLocale; }
		std::wstring GetLanguagePath(void) { return this->m_wszLangPath; }
	};
}