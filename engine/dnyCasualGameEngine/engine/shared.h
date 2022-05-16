#pragma once

/*
	Casual Game Engine (dnyCasualGameEngine) developed by Daniel Brendel

	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

#define APP_NAME L"Casual Game Engine"
#define APP_VERSION L"1.0"
#define APP_AUTHOR L"Daniel Brendel"
#define APP_CONTACT L"dbrendel1988<at>gmail<dot>com"
#define APP_STEAMID 0

#include <cstdlib>
#include <cstdio>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <codecvt>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <strsafe.h>
#include <Windows.h>
#include "resource.h"

#define FREE(p) if (p) { delete p; p = nullptr; }
