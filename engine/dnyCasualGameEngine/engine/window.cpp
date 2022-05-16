#include "window.h"

/*
	Casual Game Engine (dnyCasualGameEngine) developed by Daniel Brendel

	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

namespace DxWindow {
	class CDxWindow* pDxWindowInstance = nullptr;

	LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return pDxWindowInstance->WndProc(hWnd, uMsg, wParam, lParam);
	}
}