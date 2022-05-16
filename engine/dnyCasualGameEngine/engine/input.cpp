#include "input.h"

/*
	Casual Game Engine (dnyCasualGameEngine) developed by Daniel Brendel

	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

namespace Input {
	std::wstring GetKeyCharFromCode(int vKey)
	{
		//Get key character name from code

		std::wstring wszKeyMap[256] = {
			{L"ESCAPE"}, //0x01
			{L"1"}, //0x02
			{L"2"}, //0x03
			{L"3"}, //0x04
			{L"4"}, //0x05
			{L"5"}, //0x06
			{L"6"}, //0x07
			{L"7"}, //0x08
			{L"8"}, //0x09
			{L"9"}, //0x0A
			{L"0"}, //0x0B
			{L"MINUS"}, //0x0C/* - on main keyboard */
			{L"EQUALS"}, //0x0D
			{L"BACK"}, //0x0E/* backspace */
			{L"TAB"}, //0x0F
			{L"Q"}, //0x10
			{L"W"}, //0x11
			{L"E"}, //0x12
			{L"R"}, //0x13
			{L"T"}, //0x14
			{L"Y"}, //0x15
			{L"U"}, //0x16
			{L"I"}, //0x17
			{L"O"}, //0x18
			{L"P"}, //0x19
			{L"LBRACKET"}, //0x1A
			{L"RBRACKET"}, //0x1B
			{L"RETURN"}, //0x1C/* Enter on main keyboard */
			{L"LCONTROL"}, //0x1D
			{L"A"}, //0x1E
			{L"S"}, //0x1F
			{L"D"}, //0x20
			{L"F"}, //0x21
			{L"G"}, //0x22
			{L"H"}, //0x23
			{L"J"}, //0x24
			{L"K"}, //0x25
			{L"L"}, //0x26
			{L"SEMICOLON"}, //0x27
			{L"APOSTROPHE"}, //0x28
			{L"GRAVE"}, //0x29/* accent grave */
			{L"LSHIFT"}, //0x2A
			{L"BACKSLASH"}, //0x2B
			{L"Z"}, //0x2C
			{L"X"}, //0x2D
			{L"C"}, //0x2E
			{L"V"}, //0x2F
			{L"B"}, //0x30
			{L"N"}, //0x31
			{L"M"}, //0x32
			{L"COMMA"}, //0x33
			{L"PERIOD"}, //0x34/* . on main keyboard */
			{L"SLASH"}, //0x35/* / on main keyboard */
			{L"RSHIFT"}, //0x36
			{L"MULTIPLY"}, //0x37/* * on numeric keypad */
			{L"LMENU"}, //0x38/* left Alt */
			{L"SPACE"}, //0x39
			{L"CAPITAL"}, //0x3A
			{L"F1"}, //0x3B
			{L"F2"}, //0x3C
			{L"F3"}, //0x3D
			{L"F4"}, //0x3E
			{L"F5"}, //0x3F
			{L"F6"}, //0x40
			{L"F7"}, //0x41
			{L"F8"}, //0x42
			{L"F9"}, //0x43
			{L"F10"}, //0x44
			{L"NUMLOCK"}, //0x45
			{L"SCROLL"}, //0x46/* Scroll Lock */
			{L"NUMPAD7"}, //0x47
			{L"NUMPAD8"}, //0x48
			{L"NUMPAD9"}, //0x49
			{L"SUBTRACT"}, //0x4A/* - on numeric keypad */
			{L"NUMPAD4"}, //0x4B
			{L"NUMPAD5"}, //0x4C
			{L"NUMPAD6"}, //0x4D
			{L"ADD"}, //0x4E/* + on numeric keypad */
			{L"NUMPAD1"}, //0x4F
			{L"NUMPAD2"}, //0x50
			{L"NUMPAD3"}, //0x51
			{L"NUMPAD0"}, //0x52
			{L"DECIMAL"}, //0x53/* . on numeric keypad */
			{L"OEM_102"}, //0x56/* <> or \| on RT 102-key keyboard (Non-U.S.) */
			{L"F11"}, //0x57
			{L"F12"}, //0x58
			{L"F13"}, //0x64/* (NEC PC98) */
			{L"F14"}, //0x65/* (NEC PC98) */
			{L"F15"}, //0x66/* (NEC PC98) */
			{L"KANA"}, //0x70/* (Japanese keyboard)*/
			{L"ABNT_C1"}, //0x73/* /? on Brazilian keyboard */
			{L"CONVERT"}, //0x79/* (Japanese keyboard)*/
			{L"NOCONVERT"}, //0x7B/* (Japanese keyboard)*/
			{L"YEN"}, //0x7D/* (Japanese keyboard)*/
			{L"ABNT_C2"}, //0x7E/* Numpad . on Brazilian keyboard */
			{L"NUMPADEQUALS"}, //0x8D/* = on numeric keypad (NEC PC98) */
			{L"PREVTRACK"}, //0x90/* Previous Track (DIK_CIRCUMFLEX on Japanese keyboard) */
			{L"AT"}, //0x91/* (NEC PC98) */
			{L"COLON"}, //0x92/* (NEC PC98) */
			{L"UNDERLINE"}, //0x93/* (NEC PC98) */
			{L"KANJI"}, //0x94/* (Japanese keyboard)*/
			{L"STOP"}, //0x95/* (NEC PC98) */
			{L"AX"}, //0x96/* (Japan AX) */
			{L"UNLABELED"}, //0x97/*(J3100) */
			{L"NEXTTRACK"}, //0x99/* Next Track */
			{L"NUMPADENTER"}, //0x9C/* Enter on numeric keypad */
			{L"RCONTROL"}, //0x9D
			{L"MUTE"}, //0xA0/* Mute */
			{L"CALCULATOR"}, //0xA1/* Calculator */
			{L"PLAYPAUSE"}, //0xA2/* Play / Pause */
			{L"MEDIASTOP"}, //0xA4/* Media Stop */
			{L"VOLUMEDOWN"}, //0xAE/* Volume - */
			{L"VOLUMEUP"}, //0xB0/* Volume + */
			{L"WEBHOME"}, //0xB2/* Web home */
			{L"NUMPADCOMMA"}, //0xB3/* , on numeric keypad (NEC PC98) */
			{L"DIVIDE"}, //0xB5/* / on numeric keypad */
			{L"SYSRQ"}, //0xB7
			{L"RMENU"}, //0xB8/* right Alt */
			{L"PAUSE"}, //0xC5/* Pause */
			{L"HOME"}, //0xC7/* Home on arrow keypad */
			{L"UP"}, //0xC8/* UpArrow on arrow keypad */
			{L"PRIOR"}, //0xC9/* PgUp on arrow keypad */
			{L"LEFT"}, //0xCB/* LeftArrow on arrow keypad */
			{L"RIGHT"}, //0xCD/* RightArrow on arrow keypad */
			{L"END"}, //0xCF/* End on arrow keypad */
			{L"DOWN"}, //0xD0/* DownArrow on arrow keypad */
			{L"NEXT"}, //0xD1/* PgDn on arrow keypad */
			{L"INSERT"}, //0xD2/* Insert on arrow keypad */
			{L"DELETE"}, //0xD3/* Delete on arrow keypad */
			{L"LWIN"}, //0xDB/* Left Windows key */
			{L"RWIN"}, //0xDC/* Right Windows key */
			{L"APPS"}, //0xDD/* AppMenu key */
			{L"POWER"}, //0xDE/* System Power */
			{L"SLEEP"}, //0xDF/* System Sleep */
			{L"WAKE"}, //0xE3/* System Wake */
			{L"WEBSEARCH"}, //0xE5/* Web Search */
			{L"WEBFAVORITES"}, //0xE6/* Web Favorites */
			{L"WEBREFRESH"}, //0xE7/* Web Refresh */
			{L"WEBSTOP"}, //0xE8/* Web Stop */
			{L"WEBFORWARD"}, //0xE9/* Web Forward */
			{L"WEBBACK"}, //0xEA/* Web Back */
			{L"MYCOMPUTER"}, //0xEB/* My Computer */
			{L"MAIL"}, //0xEC/* Mail */
			{L"MEDIASELECT"}, //0xED/* Media Select */
		};

		return wszKeyMap[vKey - 1];
	}
}