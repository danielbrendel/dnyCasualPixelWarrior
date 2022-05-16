#pragma once

/*
	Casual Game Engine (dnyCasualGameEngine) developed by Daniel Brendel

	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

#include "shared.h"
#include <mmsystem.h>
#include <dsound.h>
#include <DxErr.h>

/* Sound management component */
namespace DxSound {
	#define SND_INVALID_HANDLE_VALUE std::wstring::npos

	typedef size_t HDXSOUND;
	
	class CDxSound {
	private:
		struct wave_header_s {
			char chunkId[4];
			unsigned long chunkSize;
			char format[4];
			char subChunkId[4];
			unsigned long subChunkSize;
			unsigned short audioFormat;
			unsigned short numChannels;
			unsigned long sampleRate;
			unsigned long bytesPerSecond;
			unsigned short blockAlign;
			unsigned short bitsPerSample;
			char dataChunkId[4];
			unsigned long dataSize;
		};

		struct soundfile_s {
			std::wstring wszName;
			void* pData;
			IDirectSoundBuffer8* pSoundBuffer;
			wave_header_s sWaveHeader;
		};

		LPDIRECTSOUND8 m_pSoundDevice;
		std::vector<soundfile_s> m_vSounds;
		long m_lGlobalVolume;

		void* LoadFile(const std::wstring& wszSoundFile, wave_header_s& sWaveHeader)
		{
			//Load file into memory

			if (!wszSoundFile.length())
				return nullptr;

			//Open file in read-mode
			HANDLE hFile = CreateFile(wszSoundFile.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
			if (hFile == INVALID_HANDLE_VALUE)
				return nullptr;

			DWORD dwBytesRead;

			//Read Wave file header
			if ((!ReadFile(hFile, &sWaveHeader, sizeof(wave_header_s), &dwBytesRead, nullptr)) && (dwBytesRead != sizeof(wave_header_s))) {
				CloseHandle(hFile);
				return nullptr;
			}

			//Validate wave file header
			if (!this->IsValidWaveFile(sWaveHeader)) {
				CloseHandle(hFile);
				return nullptr;
			}

			//Allocate memory for wave sound data
			void* pData = new unsigned char[sWaveHeader.dataSize];
			if (!pData) {
				CloseHandle(hFile);
				return nullptr;
			}

			//Read sound data to buffer
			if ((!ReadFile(hFile, pData, sWaveHeader.dataSize, &dwBytesRead, nullptr)) && (dwBytesRead != sWaveHeader.dataSize)) {
				delete[] pData;
				CloseHandle(hFile);
				return nullptr;
			}

			//Close file
			CloseHandle(hFile);

			return pData;
		}

		bool IsValidWaveFile(const wave_header_s& sWaveHeader)
		{
			//Check if the wave header contains valid data

			return (!memcmp(sWaveHeader.chunkId, "RIFF", sizeof(DWORD))) && //Check for RIFF signature
				(!memcmp(sWaveHeader.format, "WAVE", sizeof(DWORD))) && //Check for WAVE format signature
				(!memcmp(sWaveHeader.subChunkId, "fmt ", sizeof(DWORD))) && //Check for fmt signature
				(sWaveHeader.audioFormat == WAVE_FORMAT_PCM) /*&& //Check if it's a PCM wave file
															 (sWaveHeader.sampleRate == 44100)*/; //Check if recorded in stereo mode
		}

		inline bool IsValidHandle(HDXSOUND hSound) { return (hSound != SND_INVALID_HANDLE_VALUE) && (hSound < this->m_vSounds.size()); }

		HDXSOUND LoadSound(const std::wstring& wszSoundFile)
		{
			//Load sound from file

			if (!wszSoundFile.length())
				return SND_INVALID_HANDLE_VALUE;

			soundfile_s sSoundFile;

			//Load file into memory
			void* pMemData = this->LoadFile(wszSoundFile, sSoundFile.sWaveHeader);
			if (!pMemData)
				return SND_INVALID_HANDLE_VALUE;

			//Save further data
			sSoundFile.pData = pMemData;
			sSoundFile.wszName = wszSoundFile;

			//Setup wave format data
			WAVEFORMATEX sWaveFmt = { 0 };
			sWaveFmt.wFormatTag = WAVE_FORMAT_PCM;
			sWaveFmt.nChannels = sSoundFile.sWaveHeader.numChannels;
			sWaveFmt.nSamplesPerSec = sSoundFile.sWaveHeader.sampleRate;
			sWaveFmt.wBitsPerSample = sSoundFile.sWaveHeader.bitsPerSample;
			sWaveFmt.nBlockAlign = (sWaveFmt.wBitsPerSample / 8) * sWaveFmt.nChannels;
			sWaveFmt.nAvgBytesPerSec = sWaveFmt.nSamplesPerSec * sWaveFmt.nBlockAlign;

			//Setup buffer description data
			DSBUFFERDESC sBufferDesc = { 0 };
			sBufferDesc.dwSize = sizeof(DSBUFFERDESC);
			sBufferDesc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
			sBufferDesc.dwBufferBytes = sSoundFile.sWaveHeader.dataSize;
			sBufferDesc.lpwfxFormat = &sWaveFmt;
			sBufferDesc.guid3DAlgorithm = GUID_NULL;

			LPDIRECTSOUNDBUFFER pTempSoundBuffer = nullptr;

			//Create temporary interface sound buffer
			if (FAILED(this->m_pSoundDevice->CreateSoundBuffer(&sBufferDesc, &pTempSoundBuffer, nullptr))) {
				delete[] sSoundFile.pData;
				return false;
			}

			//Create new secondary sound buffer
			if (FAILED(pTempSoundBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&sSoundFile.pSoundBuffer))) {
				delete[] sSoundFile.pData;
				pTempSoundBuffer->Release();
				return false;
			}

			//Release temporary sound buffer
			pTempSoundBuffer->Release();

			LPVOID lpvDataPtr1 = nullptr, lpvDataPtr2 = nullptr;
			DWORD dwDataSize1 = 0, dwDataSize2 = 0;

			//Acquire buffer memory area to write file data and lock it
			if (FAILED(sSoundFile.pSoundBuffer->Lock(0, sSoundFile.sWaveHeader.dataSize, &lpvDataPtr1, &dwDataSize1, &lpvDataPtr2, &dwDataSize2, 0)))
				return false;

			//Copy file data to buffers memory area
			memcpy(lpvDataPtr1, sSoundFile.pData, sSoundFile.sWaveHeader.dataSize);

			//Unlock buffer memory area again
			if (FAILED(sSoundFile.pSoundBuffer->Unlock(lpvDataPtr1, dwDataSize1, lpvDataPtr2, dwDataSize2)))
				return false;

			//Add to list
			this->m_vSounds.push_back(sSoundFile);

			return this->m_vSounds.size() - 1; //Return sound ID
		}

		HDXSOUND FindSound(const std::wstring& wszSoundFile)
		{
			//Find sound by name

			for (HDXSOUND i = 0; i < this->m_vSounds.size(); i++) {
				if (this->m_vSounds[i].wszName == wszSoundFile)
					return i;
			}

			return SND_INVALID_HANDLE_VALUE;
		}

		bool FreeSound(HDXSOUND hSound)
		{
			//Free sound resource

			//Validate handle
			if (!this->IsValidHandle(hSound))
				return false;

			//Release sound buffer
			this->m_vSounds[hSound].pSoundBuffer->Release();

			//Free wave memory area
			delete this->m_vSounds[hSound].pData;

			//Remove from list
			this->m_vSounds.erase(this->m_vSounds.begin() + hSound);

			return true;
		}
	public:
		CDxSound() : m_pSoundDevice(nullptr), m_lGlobalVolume(-1) {}
		CDxSound(HWND hWindow) : CDxSound() { this->Initialize(hWindow); }
		~CDxSound() { this->Release();  }

		bool Initialize(HWND hWindow)
		{
			//Initialize sound manager
			
			//Check window handle
			if (!IsWindow(hWindow))
				return false;
			
			//Create sound device
			if (FAILED(DirectSoundCreate8(nullptr, &this->m_pSoundDevice, nullptr)))
				return false;
			
			//Set cooperative level
			if (FAILED(this->m_pSoundDevice->SetCooperativeLevel(hWindow, DSSCL_PRIORITY))) {
				this->Release();
				return false;
			}
			
			return true;
		}

		void Release(void)
		{
			//Release resources

			//Release secondary buffers, free memory and clear list
			if (this->m_vSounds.size()) {
				for (size_t i = 0; i < this->m_vSounds.size(); i++) {
					this->m_vSounds[i].pSoundBuffer->Release();
					delete[] this->m_vSounds[i].pData;
				}

				this->m_vSounds.clear();
			}

			//Release device if exists
			if (this->m_pSoundDevice) {
				this->m_pSoundDevice->Release();
				this->m_pSoundDevice = nullptr;
			}
		}

		HDXSOUND QuerySound(const std::wstring& wszSoundFile)
		{
			//Query a sound

			if (!wszSoundFile.length())
				return SND_INVALID_HANDLE_VALUE;

			//Attempt to find sound
			HDXSOUND hSound = this->FindSound(wszSoundFile);
			if (hSound == SND_INVALID_HANDLE_VALUE) {
				//Sound has not yet been loaded, attempt to load it
				return this->LoadSound(wszSoundFile);
			}

			return hSound;
		}

		bool Play(HDXSOUND hSound, const long iVolume, const DWORD dwFlags, const bool bOnPreviousPosition = false)
		{
			//Play sound

			//Validate handle
			if (!this->IsValidHandle(hSound))
				return false;

			DWORD dwPlayPos = 0;

			//Get position if desired
			if (bOnPreviousPosition) {
				this->m_vSounds[hSound].pSoundBuffer->GetCurrentPosition(&dwPlayPos, nullptr);
			}

			//Set position
			if (FAILED(this->m_vSounds[hSound].pSoundBuffer->SetCurrentPosition(dwPlayPos)))
				return false;

			//Set volume
			//if (FAILED(this->m_vSounds[hSound].pSoundBuffer->SetVolume(iVolume)))
			//	return false;
			#define SND_MAX_ATTENUATION 10000
			#define SND_TO_ATTENUATION(indicator) (indicator * 10 * SND_MAX_ATTENUATION / 100 - SND_MAX_ATTENUATION)
			this->m_vSounds[hSound].pSoundBuffer->SetVolume(SND_TO_ATTENUATION(((this->m_lGlobalVolume != -1) ? this->m_lGlobalVolume : iVolume)));

			//Play sound
			return SUCCEEDED(this->m_vSounds[hSound].pSoundBuffer->Play(0, 0, dwFlags));
		}

		bool StopSound(HDXSOUND hSound)
		{
			//Stop playing given sound

			//Validate handle
			if (!this->IsValidHandle(hSound))
				return false;

			return SUCCEEDED(this->m_vSounds[hSound].pSoundBuffer->Stop());
		}

		void StopAll(void)
		{
			//Stop all sounds

			for (size_t i = 0; i < this->m_vSounds.size(); i++) {
				this->StopSound(i);
			}
		}

		void SetGlobalVolume(const long iVolume)
		{
			//Set global volume

			this->m_lGlobalVolume = iVolume;
		}

		const long GetGlobalVolume(void) const { return this->m_lGlobalVolume; } //Getter
	};
}

