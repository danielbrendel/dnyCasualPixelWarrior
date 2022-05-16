#pragma once

/*
	Casual Game Engine (dnyCasualGameEngine) developed by Daniel Brendel

	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

#include "shared.h"
#include <windowsx.h>

/* Window management component */
namespace DxWindow {
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	extern class CDxWindow* pDxWindowInstance;

	class CDxWindow {
	public:
		struct IWindowEvents {
			virtual void OnCreated(HWND hWnd) = 0;
			virtual LRESULT OnKeyEvent(int vKey, bool bDown, bool bCtrlHeld, bool bShiftHeld, bool bAltHeld) = 0;
			virtual LRESULT OnMouseEvent(int x, int y, int iMouseKey, bool bDown, bool bCtrlHeld, bool bShiftHeld, bool bAltHeld) = 0;
			virtual void OnMouseWheel(short wDistance, bool bForward) = 0;
			virtual void OnDraw(void) = 0;
			virtual void OnRelease(void) = 0;
		};
	private:
		bool m_bReady;
		std::wstring m_szClassName;
		ATOM m_hClass;
		HWND m_hWindow;
		RECT m_sRect;
		IWindowEvents* m_pEvents;

		friend LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			//Window procedure handling

			switch (uMsg) {
			case WM_CREATE:
				this->m_pEvents->OnCreated(hWnd);
				break;
			//Input handling:
			/*case WM_KEYUP: {
				bool bCtrlHeld = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
				bool bShiftHeld = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
				bool bAltHeld = (GetKeyState(VK_MENU) & 0x8000) != 0;
				return this->m_pEvents->OnKeyEvent((int)wParam, false, bCtrlHeld, bShiftHeld, bAltHeld);
				break;
			}
			case WM_KEYDOWN: {
				bool bCtrlHeld = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
				bool bShiftHeld = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
				bool bAltHeld = (GetKeyState(VK_MENU) & 0x8000) != 0;
				return this->m_pEvents->OnKeyEvent((int)wParam, true, bCtrlHeld, bShiftHeld, bAltHeld);
				break;
			}
			case WM_LBUTTONUP: {
				bool bCtrlHeld = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
				bool bShiftHeld = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
				bool bAltHeld = (GetKeyState(VK_MENU) & 0x8000) != 0;
				return this->m_pEvents->OnMouseEvent(-1, -1, VK_LBUTTON, false, bCtrlHeld, bShiftHeld, bAltHeld);
				break;
			}
			case WM_LBUTTONDOWN: {
				bool bCtrlHeld = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
				bool bShiftHeld = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
				bool bAltHeld = (GetKeyState(VK_MENU) & 0x8000) != 0;
				return this->m_pEvents->OnMouseEvent(-1, -1, VK_LBUTTON, true, bCtrlHeld, bShiftHeld, bAltHeld);
				break;
			}
			case WM_RBUTTONUP: {
				bool bCtrlHeld = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
				bool bShiftHeld = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
				bool bAltHeld = (GetKeyState(VK_MENU) & 0x8000) != 0;
				return this->m_pEvents->OnMouseEvent(-1, -1, VK_RBUTTON, false, bCtrlHeld, bShiftHeld, bAltHeld);
				break;
			}
			case WM_RBUTTONDOWN: {
				bool bCtrlHeld = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
				bool bShiftHeld = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
				bool bAltHeld = (GetKeyState(VK_MENU) & 0x8000) != 0;
				return this->m_pEvents->OnMouseEvent(-1, -1, VK_RBUTTON, true, bCtrlHeld, bShiftHeld, bAltHeld);
				break;
			}
			case WM_MBUTTONUP: {
				bool bCtrlHeld = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
				bool bShiftHeld = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
				bool bAltHeld = (GetKeyState(VK_MENU) & 0x8000) != 0;
				return this->m_pEvents->OnMouseEvent(-1, -1, VK_MBUTTON, false, bCtrlHeld, bShiftHeld, bAltHeld);
				break;
			}
			case WM_MBUTTONDOWN: {
				bool bCtrlHeld = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
				bool bShiftHeld = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
				bool bAltHeld = (GetKeyState(VK_MENU) & 0x8000) != 0;
				return this->m_pEvents->OnMouseEvent(-1, -1, VK_MBUTTON, true, bCtrlHeld, bShiftHeld, bAltHeld);
				break;
			}*/
			case WM_MOUSEWHEEL: {
				short wDistance = (short)HIWORD(wParam);
				bool bForward = wDistance >= 0;
				this->m_pEvents->OnMouseWheel(wDistance, bForward);
				break;
			}
			/*case WM_MOUSEMOVE:
				return this->m_pEvents->OnMouseEvent(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 0, false, false, false, false);
				break;*/
			case WM_PAINT: //Window shall paint
				this->m_pEvents->OnDraw();
				return 0;
				break;
			case WM_QUIT: //Window shall close
				this->m_pEvents->OnRelease();
				this->Release();
				return 0;
				break;
			case WM_DESTROY: { //Window gets destroyed
				this->m_pEvents->OnRelease();
				PostQuitMessage(EXIT_SUCCESS);
				this->Release();
				return 0;
			}
			default:
				break;
			}

			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	public:
		CDxWindow() : m_bReady(false), m_pEvents(nullptr) { pDxWindowInstance = this; }
		CDxWindow(const std::wstring& wszTitle, int w, int h, IWindowEvents* pEvents) : m_bReady(false) { pDxWindowInstance = this; this->Initialize(wszTitle, w, h, pEvents); }
		~CDxWindow() { this->Release(); }

		bool Initialize(const std::wstring& wszTitle, int w, int h, IWindowEvents* pEvents)
		{
			//Initialize window
			
			if (this->m_bReady)
				return true;

			//Save/Set data
			this->m_szClassName = wszTitle;
			this->m_pEvents = pEvents;

			//Setup window class data struct
			WNDCLASSEXW sWndClass = { 0 };
			sWndClass.cbSize = sizeof(WNDCLASSEXW);
			sWndClass.style = CS_VREDRAW | CS_HREDRAW;
			sWndClass.hInstance = (HINSTANCE)GetCurrentProcess();
			sWndClass.lpszClassName = this->m_szClassName.c_str();
			sWndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
			sWndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
			sWndClass.lpfnWndProc = &WindowProc;
			
			//Register window class
			this->m_hClass = RegisterClassEx(&sWndClass);
			if (!this->m_hClass)
				return false;
			
			//Create the window
			this->m_hWindow = CreateWindowEx(0, this->m_szClassName.c_str(), wszTitle.c_str(), WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, w, h, 0, 0, (HINSTANCE)GetCurrentProcess(), NULL);
			if (!this->m_hWindow)
				return false;
			
			//Center window to screen
			RECT sDesktopRect;
			GetWindowRect(GetDesktopWindow(), &sDesktopRect);
			SetWindowPos(this->m_hWindow, 0, sDesktopRect.right / 2 - w / 2, sDesktopRect.bottom / 2 - h / 2, w, h, SWP_NOSIZE | SWP_NOZORDER);

			//Save window rect data
			if (!GetWindowRect(this->m_hWindow, &this->m_sRect)) {
				this->Release();
				return false;
			}
			
			//Update the window initiallly
			UpdateWindow(this->m_hWindow);

			//Hide cursor
			ShowCursor(FALSE);

			return this->m_bReady = true;
		}

		void Release(void)
		{
			//Release resources

			if (!this->m_bReady)
				return;

			//Release window and unregister class
			DestroyWindow(this->m_hWindow);
			UnregisterClass(this->m_szClassName.c_str(), (HINSTANCE)GetCurrentProcess());

			//Clear data
			this->m_hWindow = 0;
			this->m_hClass = 0;

			this->m_bReady = false;
		}

		bool Process(void)
		{
			//Perform main loop

			MSG sMsg;

			//Handle message queue
			if (PeekMessage(&sMsg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&sMsg);
				DispatchMessage(&sMsg);
			}

			//Update window
			UpdateWindow(this->m_hWindow);

			//Return indicator whether window does still exist or not
			return IsWindow(this->m_hWindow) == TRUE;
		}

		//Getters
		inline const bool IsReady(void) const { return this->m_bReady; }
		inline const HWND GetHandle(void) const { return this->m_hWindow; }
		int GetResolutionX(void) { return this->m_sRect.right - this->m_sRect.left; }
		int GetResolutionY(void) { return this->m_sRect.bottom - this->m_sRect.top; }
		const RECT& GetCurrentRect(void) const { return this->m_sRect; }
	};
}