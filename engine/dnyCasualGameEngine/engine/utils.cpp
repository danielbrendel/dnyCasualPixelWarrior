#include "utils.h"
#include "vars.h"

/*
	Casual Game Engine (dnyCasualGameEngine) developed by Daniel Brendel

	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

namespace Utils {
	std::wstring ConvertToWideString(const std::string& szString)
	{
		//Convert ansi string to wide string
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> oConverter;
		return oConverter.from_bytes(szString);
	}

	std::string ConvertToAnsiString(const std::wstring& wszString)
	{
		//Convert wide string to ansi string
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> oConverter;
		return oConverter.to_bytes(wszString);
	}

	std::string ExtractFileExt(const std::string& szFile)
	{
		//Extract file extension

		std::string szExt;
		std::string szResult;

		for (size_t i = szFile.length() - 1; i >= 0; i--) {
			if (szFile[i] == '.') break;
			szExt += szFile[i];
		}

		for (size_t i = 0; i < szExt.length(); i++) {
			szResult += szExt[(szExt.length() - 1) - i];
		}

		return szResult;
	}

	std::wstring ExtractFileExt(const std::wstring& wszFile)
	{
		//Extract file extension

		std::wstring wszExt;
		std::wstring wszResult;

		for (size_t i = wszFile.length() - 1; i >= 0; i--) {
			if (wszFile[i] == '.') break;
			wszExt += wszFile[i];
		}

		for (size_t i = 0; i < wszExt.length(); i++) {
			wszResult += wszExt[(wszExt.length() - 1) - i];
		}

		return wszResult;
	}

	std::wstring ExtractFileName(const std::wstring& wszFile)
	{
		//Extract file name

		std::wstring wszName;
		std::wstring wszResult;

		for (size_t i = wszFile.length() - 1; i >= 0; i--) {
			if ((wszFile[i] == '/') || (wszFile[i] == '\\')) break;
			wszName += wszFile[i];
		}

		for (size_t i = 0; i < wszName.length(); i++) {
			wszResult += wszName[(wszName.length() - 1) - i];
		}

		return wszResult;
	}

	std::string ExtractFileNameA(const std::string& szFile)
	{
		//Extract file name (Ansi strings)

		std::string szName;
		std::string szResult;

		for (size_t i = szFile.length() - 1; i >= 0; i--) {
			if ((szFile[i] == '/') || (szFile[i] == '\\')) break;
			szName += szFile[i];
		}

		for (size_t i = 0; i < szName.length(); i++) {
			szResult += szName[(szName.length() - 1) - i];
		}

		return szResult;
	}

	std::wstring ExtractFilePath(const std::wstring& wszFile)
	{
		//Extract file path

		wchar_t wszFullFileName[MAX_PATH * 2];
		lstrcpyW(wszFullFileName, wszFile.c_str());

		for (size_t i = wcslen(wszFullFileName); i > 0; i--) {
			if (wszFullFileName[i] == '\\')
				break;
			wszFullFileName[i] = 0;
		}

		return wszFullFileName;
	}

	std::wstring GetExeName(void)
	{
		//Get exe name

		wchar_t wszFullFileName[2048] = { 0 };

		GetModuleFileName(0, wszFullFileName, sizeof(wszFullFileName));

		return ExtractFileName(wszFullFileName);
	}

	bool FileExists(const std::wstring& wszFileName)
	{
		//Indicate whether a file exists or not

		HANDLE hFile = CreateFile(wszFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (hFile != INVALID_HANDLE_VALUE) {
			CloseHandle(hFile);
			return true;
		}

		return false;
	}

	bool DirExists(const std::wstring& wszDirName)
	{
		//Indicate whether a folder exists or not

		DWORD dwAttributes = GetFileAttributes(wszDirName.c_str());

		return ((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
	}

	inline void CharReplace(std::string& szStr, const char old, const char _new)
	{
		//Replace all char occurences
		std::replace(szStr.begin(), szStr.end(), old, _new);
	}

	byte* ReadEntireFile(const std::wstring& wszFile, size_t& uiSizeOut, bool bTreatAsString)
	{
		//Read entire file content into stream

		if (!wszFile.length())
			return nullptr;

		//Open existing file in read-mode
		HANDLE hFile = CreateFile(wszFile.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (hFile == INVALID_HANDLE_VALUE)
			return nullptr;

		//Obtain file size
		DWORD dwFileSize = GetFileSize(hFile, nullptr);
		if (!dwFileSize) {
			CloseHandle(hFile);
			return nullptr;
		}

		//Allocate buffer with file-size in bytes
		byte* pBuffer = (byte*)malloc(dwFileSize + ((bTreatAsString) ? sizeof(wchar_t) : 0));
		if (!pBuffer) {
			CloseHandle(hFile);
			return nullptr;
		}

		//Read entire file content into buffer
		DWORD dwBytesRead;
		if ((!ReadFile(hFile, pBuffer, dwFileSize, &dwBytesRead, nullptr)) || (dwBytesRead != dwFileSize)) {
			free(pBuffer);
			CloseHandle(hFile);
			return nullptr;
		}

		//Set output size and close file handle
		uiSizeOut = (size_t)dwFileSize;
		CloseHandle(hFile);

		//Zero terminate if string
		if (bTreatAsString) pBuffer[dwFileSize] = 0;

		return pBuffer;
	}

	std::vector<std::wstring> ReadFileLines(const std::wstring& wszFile)
	{
		//Read all lines of a file

		std::vector<std::wstring> vResult;

		std::wifstream hFile(wszFile);
		if (hFile.is_open()) {
			wchar_t wszLine[2048] = { 0 };

			while (!hFile.eof()) {
				hFile.getline(wszLine, sizeof(wszLine), '\n');

				vResult.push_back(wszLine);
			}

			hFile.close();
		}

		return vResult;
	}

	std::vector<std::string> Split(const std::string& szInput, const std::string& szSplit)
	{
		//Split string by delimiter

		std::vector<std::string> vResult;

		if ((!szInput.length()) || (!szSplit.length()))
			return vResult;

		std::string szTemp;

		for (size_t i = 0; i <= szInput.length(); i++) { //Iterate through all chars
			std::string szSubStr = "";
			if (i < szInput.length()) szSubStr = szInput.substr(i, szSplit.length()); //Get substring at current pos with delimiter length if not end of string reached

			if ((szSubStr == szSplit) || (szInput[i] == 0)) { //If current substring equals the delimiter
				vResult.push_back(szTemp); //Add current item to list
				szTemp.clear(); //Clear temp buffer
				i += szSplit.length() - 1; //Ignore further delim chars
				continue;
			}

			//Add current char to temp buffer
			szTemp += szInput[i];
		}

		return vResult;
	}

	std::vector<std::wstring> SplitW(const std::wstring& wszInput, const std::wstring& wszSplit)
	{
		//Split string by delimiter

		std::vector<std::wstring> vResult;

		if ((!wszInput.length()) || (!wszSplit.length()))
			return vResult;

		std::wstring wszTemp;

		for (size_t i = 0; i <= wszInput.length(); i++) { //Iterate through all chars
			std::wstring wszSubStr = L"";
			if (i < wszInput.length()) wszSubStr = wszInput.substr(i, wszSplit.length()); //Get substring at current pos with delimiter length if not end of string reached

			if ((wszSubStr == wszSplit) || (wszInput[i] == 0)) { //If current substring equals the delimiter
				vResult.push_back(wszTemp); //Add current item to list
				wszTemp.clear(); //Clear temp buffer
				i += wszSplit.length() - 1; //Ignore further delim chars
				continue;
			}

			//Add current char to temp buffer
			wszTemp += wszInput[i];
		}

		return vResult;
	}

	std::string ReplaceString(const std::string& szString, const std::string& szFind, const std::string& szNew)
	{
		//Replace all occurences of a string

		std::string szResult = szString;

		//Find position of occurence in string
		size_t uiSubstrPos = szResult.find(szFind);
		while (uiSubstrPos != std::string::npos) {
			//Replace var identifier with value
			szResult = szResult.replace(szResult.begin() + uiSubstrPos, szResult.begin() + uiSubstrPos + szFind.length(), szNew);

			//Update position value
			uiSubstrPos = szResult.find(szFind);
		}

		return szResult;
	}

	std::wstring ReplaceStringW(const std::wstring& wszString, const std::wstring& wszFind, const std::wstring& wszNew)
	{
		//Replace all occurences of a string

		std::wstring wszResult = wszString;

		//Find position of occurence in string
		size_t uiSubstrPos = wszResult.find(wszFind);
		while (uiSubstrPos != std::wstring::npos) {
			//Replace var identifier with value
			wszResult = wszResult.replace(wszResult.begin() + uiSubstrPos, wszResult.begin() + uiSubstrPos + wszFind.length(), wszNew);

			//Update position value
			uiSubstrPos = wszResult.find(wszFind);
		}

		return wszResult;
	}

	std::vector<std::wstring> ListFilesByExt(const std::wstring& wszBaseDir, const wchar_t** pFileList, const size_t uiListLen)
	{
		WIN32_FIND_DATA sFindData = { 0 };
		HANDLE hFileSearch;
		std::vector<std::wstring> vNames;

		hFileSearch = FindFirstFile((wszBaseDir + L"\\*.*").c_str(), &sFindData);
		if (hFileSearch == INVALID_HANDLE_VALUE)
			return vNames;

		while (FindNextFile(hFileSearch, &sFindData)) {
			if (sFindData.cFileName[0] == '.')
				continue;

			if ((sFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) {
				std::wstring wszFileExt = Utils::ExtractFileExt(sFindData.cFileName);
				for (size_t i = 0; i < uiListLen; i++) {
					if (wszFileExt == pFileList[i]) {
						vNames.push_back(sFindData.cFileName);
					}
				}
			}
		}

		FindClose(hFileSearch);

		return vNames;
	}

	bool RemoveEntireDirectory(const std::wstring& wszDirectory)
	{
		//Delete entire folder from disk

		if (!wszDirectory.length())
			return false;

		wchar_t* pwszNameObject = new wchar_t[wszDirectory.length() + 2];
		memset(pwszNameObject, 0x00, wszDirectory.length() * 2 + 4);
		lstrcpyW(pwszNameObject, wszDirectory.c_str());

		SHFILEOPSTRUCTW sFileOpStruct;
		memset(&sFileOpStruct, 0x00, sizeof(sFileOpStruct));
		sFileOpStruct.wFunc = FO_DELETE;
		sFileOpStruct.fFlags = FOF_NO_UI;
		sFileOpStruct.pFrom = (PCZZWSTR)pwszNameObject;

		bool bSuccess = SHFileOperationW(&sFileOpStruct) == 0;

		delete[] pwszNameObject;

		return bSuccess;
	}

	bool CopyEntireDirectory(const std::wstring& wszFrom, const std::wstring& wszTo)
	{
		//Copy an entire directory

		if ((!wszFrom.length()) || (!wszTo.length()))
			return false;

		wchar_t pwszFrom[MAX_PATH];
		lstrcpyW(pwszFrom, wszFrom.c_str());
		lstrcatW(pwszFrom, L"\\*");

		wchar_t pwszTo[MAX_PATH];
		lstrcpyW(pwszTo, wszTo.c_str());

		SHFILEOPSTRUCTW sFileOpStruct;
		memset(&sFileOpStruct, 0x00, sizeof(sFileOpStruct));
		sFileOpStruct.wFunc = FO_COPY;
		sFileOpStruct.fFlags = FOF_SILENT;
		sFileOpStruct.pFrom = pwszFrom;
		sFileOpStruct.pTo = pwszTo;

		return SHFileOperation(&sFileOpStruct) == 0;
	}

	bool CreateRestartScript(void)
	{
		//Create a restarter script

		std::wstring wszBatchScript = L"@echo off\ncls\ntaskkill /PID " + std::to_wstring(GetCurrentProcessId()) + L"\nstart \"\" \"" + GetExeName() + L"\"\n";
		std::wstring wszBatchFile = wszBasePath + L"restarter.bat";

		if (FileExists(wszBatchFile)) {
			DeleteFile(wszBatchFile.c_str());
		}

		std::wofstream hFile;
		hFile.open(wszBasePath + L"restarter.bat", std::wifstream::out);
		if (hFile.is_open()) {
			hFile << wszBatchScript;
			hFile.close();

			return true;
		}

		return false;
	}
}