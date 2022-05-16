#pragma once

/*
	Casual Game Engine (dnyCasualGameEngine) developed by Daniel Brendel

	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#include "shared.h"
#include <dinput.h>

/* Key/mouse binding management */
namespace Input {
	/* Input binding manager */
	class CInputMgr {
	private:
		struct binding_s {
			std::wstring wszIdent;
			int vCode;
		};

		std::vector<binding_s> m_vBindings;
	public:
		CInputMgr() {}
		~CInputMgr() { this->m_vBindings.clear(); }

		void AddKeyBinding(const std::wstring& wszIdent, int vCode)
		{
			//Add key binding

			binding_s sBinding;
			sBinding.wszIdent = wszIdent;
			sBinding.vCode = vCode;

			this->m_vBindings.push_back(sBinding);
		}

		int GetKeyBindingCode(const std::wstring& wszIdent)
		{
			//Get key binding code

			for (size_t i = 0; i < this->m_vBindings.size(); i++) {
				if (this->m_vBindings[i].wszIdent == wszIdent) {
					return this->m_vBindings[i].vCode;
				}
			}

			return 0;
		}

		void SetKeyBindingCode(const std::wstring& wszIdent, int vCode)
		{
			//Set key binding code

			for (size_t i = 0; i < this->m_vBindings.size(); i++) {
				if (this->m_vBindings[i].wszIdent == wszIdent) {
					this->m_vBindings[i].vCode = vCode;
				}
			}
		}
	};

	/* DirectInput manager */
	class CDxInput {
	public:
		struct IInputEvents {
			virtual void OnKeyDown(int vKey) = 0;
			virtual void OnKeyUp(int vKey) = 0;
			virtual void OnMouseMove(int x, int y) = 0;
			virtual void OnMouseKeyDown(int vKey) = 0;
			virtual void OnMouseKeyUp(int vKey) = 0;
		};
	private:
		LPDIRECTINPUT8 m_pDirectInput;
		LPDIRECTINPUTDEVICE8 m_pInputKeyboard;
		LPDIRECTINPUTDEVICE8 m_pInputMouse;
		bool m_bLastKeyboardState[256];
		int m_iMousePos[2];
		int m_iWindowOffset[2];
		IInputEvents* m_pEvents;
		bool m_bLButtonDown;
		bool m_bRButtonDown;
	public:
		CDxInput() : m_pDirectInput(nullptr), m_pInputKeyboard(nullptr), m_pInputMouse(nullptr), m_bLButtonDown(false), m_bRButtonDown(false) {}
		~CDxInput() {}

		bool Initialize(HWND hWnd, IInputEvents* pEvents, int iWndOffsetX, int iWndOffsetY)
		{
			//Initialize DirectInput

			//Initialize DirectInput
			if (FAILED(DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&this->m_pDirectInput, nullptr))) {
				return false;
			}

			//Create keyboard device
			if (FAILED(this->m_pDirectInput->CreateDevice(GUID_SysKeyboard, &this->m_pInputKeyboard, nullptr))) {
				this->Release();
				return false;
			}

			//Create mouse device
			if (FAILED(this->m_pDirectInput->CreateDevice(GUID_SysMouse, &this->m_pInputMouse, nullptr))) {
				this->Release();
				return false;
			}

			//Set data format
			this->m_pInputKeyboard->SetDataFormat(&c_dfDIKeyboard);
			this->m_pInputMouse->SetDataFormat(&c_dfDIMouse);

			//Set cooperative level
			this->m_pInputKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
			this->m_pInputMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

			//Acquire keyboard access
			this->m_pInputKeyboard->Acquire();

			//Create input buffer for mouse
			DIPROPDWORD diProperties;
			memset(&diProperties, 0x00, sizeof(diProperties));
			diProperties.diph.dwSize = sizeof(DIPROPDWORD);
			diProperties.diph.dwHeaderSize = sizeof(DIPROPHEADER);
			diProperties.diph.dwObj = 0;
			diProperties.diph.dwHow = DIPH_DEVICE;
			diProperties.dwData = 16;
			this->m_pInputMouse->SetProperty(DIPROP_BUFFERSIZE, &diProperties.diph);

			//Store pointer
			this->m_pEvents = pEvents;

			//Store offsets
			this->m_iWindowOffset[0] = iWndOffsetX;
			this->m_iWindowOffset[1] = iWndOffsetY;

			//Clear as initial state
			memset(&this->m_bLastKeyboardState, 0x00, sizeof(this->m_bLastKeyboardState));

			return true;
		}

		void ProcessKeyboard(void)
		{
			//Process keyboard input

			bool bKeyboard[256];
			
			//Get keyboard state
			if (FAILED(this->m_pInputKeyboard->GetDeviceState(sizeof(bKeyboard), &bKeyboard))) {
				this->m_pInputKeyboard->Acquire(); //Acquire if lost access
			}

			//Check for each key if it is pressed or released
			for (int i = 0; i < sizeof(bKeyboard); i++) {
				if (bKeyboard[i]) {
					if (!this->m_bLastKeyboardState[i]) {
						this->m_pEvents->OnKeyDown(i);
					}
				} else {
					if (this->m_bLastKeyboardState[i]) {
						this->m_pEvents->OnKeyUp(i);
					}
				}
			}

			//Save last state
			memcpy(&this->m_bLastKeyboardState, &bKeyboard, sizeof(bKeyboard));
		}

		void ProcessMouse(void)
		{
			//Process mouse input

			/*DWORD dwNumElements = 1;
			DIDEVICEOBJECTDATA data;
			memset(&data, 0x00, sizeof(data));

			//Get device data
			if (FAILED(this->m_pInputMouse->GetDeviceData(sizeof(data), &data, &dwNumElements, 0))) {
				this->m_pInputMouse->Acquire(); //Acquire if lost access
			}

			//Handle data
			switch (data.dwOfs) {
			case DIMOFS_X:
				this->m_iMousePos[0] += data.dwData;
				this->m_pEvents->OnMouseMove(this->m_iMousePos[0], this->m_iMousePos[1]);
				break;
			case DIMOFS_Y:
				this->m_iMousePos[1] += data.dwData;
				this->m_pEvents->OnMouseMove(this->m_iMousePos[0], this->m_iMousePos[1]);
				break;
			case DIMOFS_BUTTON0:
				if (data.dwData) {
					this->m_pEvents->OnMouseKeyDown(1);
				} else {
					this->m_pEvents->OnMouseKeyUp(1);
				}
				break;
			case DIMOFS_BUTTON1:
				if (data.dwData) {
					this->m_pEvents->OnMouseKeyDown(2);
				} else {
					this->m_pEvents->OnMouseKeyUp(2);
				}
				break;
			case DIMOFS_BUTTON3:
				if (data.dwData) {
					this->m_pEvents->OnMouseKeyDown(3);
				} else {
					this->m_pEvents->OnMouseKeyUp(3);
				}
				break;
			default:
				break;
			}*/

			//Get cursor position
			POINT sPoint;
			GetCursorPos(&sPoint);

			//Store result
			this->m_iMousePos[0] = sPoint.x - this->m_iWindowOffset[0];
			this->m_iMousePos[1] = sPoint.y - this->m_iWindowOffset[1];

			//Call event method
			this->m_pEvents->OnMouseMove(this->m_iMousePos[0], this->m_iMousePos[1]);

			//Handle left mouse button
			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
				if (!this->m_bLButtonDown) {
					this->m_pEvents->OnMouseKeyDown(VK_LBUTTON);
					this->m_bLButtonDown = true;
				}
			} else {
				if (this->m_bLButtonDown) {
					this->m_pEvents->OnMouseKeyUp(VK_LBUTTON);
					this->m_bLButtonDown = false;
				}
			}

			//Handle right mouse button
			if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
				if (!this->m_bRButtonDown) {
					this->m_pEvents->OnMouseKeyDown(VK_RBUTTON);
					this->m_bRButtonDown = true;
				}
			} else {
				if (this->m_bRButtonDown) {
					this->m_pEvents->OnMouseKeyUp(VK_RBUTTON);
					this->m_bRButtonDown = false;
				}
			}
		}

		void Release(void)
		{
			//Release resources

			if (this->m_pInputMouse) {
				this->m_pInputMouse->Unacquire();
				this->m_pInputMouse->Release();
				this->m_pInputMouse = nullptr;
			}

			if (this->m_pInputKeyboard) {
				this->m_pInputKeyboard->Unacquire();
				this->m_pInputKeyboard->Release();
				this->m_pInputKeyboard = nullptr;
			}

			if (this->m_pDirectInput) {
				this->m_pDirectInput->Release();
				this->m_pDirectInput = nullptr;
			}
		}
	};

	std::wstring GetKeyCharFromCode(int vKey);
}