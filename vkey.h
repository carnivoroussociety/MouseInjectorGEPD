//==========================================================================
// Mouse Injector Plugin
//==========================================================================
// Copyright (C) 2016-2021 Carnivorous
// All rights reserved.
//
// Mouse Injector is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, visit http://www.gnu.org/licenses/gpl-2.0.html
//==========================================================================
// Virtual Keys, Standard Set
#define VK_LBUTTON 0x01
#define VK_RBUTTON 0x02
#define VK_CANCEL 0x03
#define VK_MBUTTON 0x04 // NOT contiguous with L & RBUTTON

#if(_WIN32_WINNT >= 0x0500)
	#define VK_XBUTTON1 0x05 // NOT contiguous with L & RBUTTON
	#define VK_XBUTTON2 0x06 // NOT contiguous with L & RBUTTON
#endif

// 0x07 : unassigned

#define VK_BACK 0x08
#define VK_TAB 0x09

// 0x0A - 0x0B : mouse wheel
#define VK_WHEELUP 0x0A // THERE IS NO SUCH THING AS VK_WHEELUP - I INVENTED THIS BECAUSE I'M LAZY - TERRIBLE HACK
#define VK_WHEELDOWN 0x0B // THERE IS NO SUCH THING AS VK_WHEELDOWN - I INVENTED THIS BECAUSE I'M LAZY - TERRIBLE HACK

#define VK_CLEAR 0x0C
#define VK_RETURN 0x0D

#define VK_WHEELLEFT 0x0F // THERE IS NO SUCH THING AS VK_WHEELLEFT - I INVENTED THIS BECAUSE I'M LAZY - TERRIBLE HACK
#define VK_WHEELRIGHT 0x0E // THERE IS NO SUCH THING AS VK_WHEELRIGHT - I INVENTED THIS BECAUSE I'M LAZY - TERRIBLE HACK

#define VK_SHIFT 0x10
#define VK_CONTROL 0x11
#define VK_MENU 0x12
#define VK_PAUSE 0x13
#define VK_CAPITAL 0x14

#define VK_KANA 0x15
#define VK_HANGEUL 0x15 // old name - should be here for compatibility
#define VK_HANGUL 0x15
#define VK_JUNJA 0x17
#define VK_FINAL 0x18
#define VK_HANJA 0x19
#define VK_KANJI 0x19

#define VK_ESCAPE 0x1B

#define VK_CONVERT 0x1C
#define VK_NONCONVERT 0x1D
#define VK_ACCEPT 0x1E
#define VK_MODECHANGE 0x1F

#define VK_SPACE 0x20
#define VK_PRIOR 0x21
#define VK_NEXT 0x22
#define VK_END 0x23
#define VK_HOME 0x24
#define VK_LEFT 0x25
#define VK_UP 0x26
#define VK_RIGHT 0x27
#define VK_DOWN 0x28
#define VK_SELECT 0x29
#define VK_PRINT 0x2A
#define VK_EXECUTE 0x2B
#define VK_SNAPSHOT 0x2C
#define VK_INSERT 0x2D
#define VK_DELETE 0x2E
#define VK_HELP 0x2F

// VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
// 0x40 : unassigned
// VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)

#define VK_LWIN 0x5B
#define VK_RWIN 0x5C
#define VK_APPS 0x5D

// 0x5E : reserved

#define VK_SLEEP 0x5F

#define VK_NUMPAD0 0x60
#define VK_NUMPAD1 0x61
#define VK_NUMPAD2 0x62
#define VK_NUMPAD3 0x63
#define VK_NUMPAD4 0x64
#define VK_NUMPAD5 0x65
#define VK_NUMPAD6 0x66
#define VK_NUMPAD7 0x67
#define VK_NUMPAD8 0x68
#define VK_NUMPAD9 0x69
#define VK_MULTIPLY 0x6A
#define VK_ADD 0x6B
#define VK_SEPARATOR 0x6C
#define VK_SUBTRACT 0x6D
#define VK_DECIMAL 0x6E
#define VK_DIVIDE 0x6F
#define VK_F1 0x70
#define VK_F2 0x71
#define VK_F3 0x72
#define VK_F4 0x73
#define VK_F5 0x74
#define VK_F6 0x75
#define VK_F7 0x76
#define VK_F8 0x77
#define VK_F9 0x78
#define VK_F10 0x79
#define VK_F11 0x7A
#define VK_F12 0x7B
#define VK_F13 0x7C
#define VK_F14 0x7D
#define VK_F15 0x7E
#define VK_F16 0x7F
#define VK_F17 0x80
#define VK_F18 0x81
#define VK_F19 0x82
#define VK_F20 0x83
#define VK_F21 0x84
#define VK_F22 0x85
#define VK_F23 0x86
#define VK_F24 0x87

// 0x88 - 0x8F : unassigned

#define VK_NUMLOCK 0x90
#define VK_SCROLL 0x91

// NEC PC-9800 kbd definitions
#define VK_OEM_NEC_EQUAL 0x92 // '=' key on numpad

// Fujitsu/OASYS kbd definitions
#define VK_OEM_FJ_JISHO 0x92 // 'Dictionary' key
#define VK_OEM_FJ_MASSHOU 0x93 // 'Unregister word' key
#define VK_OEM_FJ_TOUROKU 0x94 // 'Register word' key
#define VK_OEM_FJ_LOYA 0x95 // 'Left OYAYUBI' key
#define VK_OEM_FJ_ROYA 0x96 // 'Right OYAYUBI' key

// 0x97 - 0x9F : unassigned

// VK_L* & VK_R* - left and right Alt, Ctrl and Shift virtual keys.
// Used only as parameters to GetAsyncKeyState() and GetKeyState().
// No other API or message will distinguish left and right keys in this way.
#define VK_LSHIFT 0xA0
#define VK_RSHIFT 0xA1
#define VK_LCONTROL 0xA2
#define VK_RCONTROL 0xA3
#define VK_LMENU 0xA4
#define VK_RMENU 0xA5

#if(_WIN32_WINNT >= 0x0500)
#define VK_BROWSER_BACK 0xA6
#define VK_BROWSER_FORWARD 0xA7
#define VK_BROWSER_REFRESH 0xA8
#define VK_BROWSER_STOP 0xA9
#define VK_BROWSER_SEARCH 0xAA
#define VK_BROWSER_FAVORITES 0xAB
#define VK_BROWSER_HOME 0xAC

#define VK_VOLUME_MUTE 0xAD
#define VK_VOLUME_DOWN 0xAE
#define VK_VOLUME_UP 0xAF
#define VK_MEDIA_NEXT_TRACK 0xB0
#define VK_MEDIA_PREV_TRACK 0xB1
#define VK_MEDIA_STOP 0xB2
#define VK_MEDIA_PLAY_PAUSE 0xB3
#define VK_LAUNCH_MAIL 0xB4
#define VK_LAUNCH_MEDIA_SELECT 0xB5
#define VK_LAUNCH_APP1 0xB6
#define VK_LAUNCH_APP2 0xB7

#endif

// 0xB8 - 0xB9 : reserved

#define VK_OEM_1 0xBA // ';:' for US
#define VK_OEM_PLUS 0xBB // '+' any country
#define VK_OEM_COMMA 0xBC // ',' any country
#define VK_OEM_MINUS 0xBD // '-' any country
#define VK_OEM_PERIOD 0xBE // '.' any country
#define VK_OEM_2 0xBF // '/?' for US
#define VK_OEM_3 0xC0 // '`~' for US


// 0xC1 - 0xD7 : reserved

// 0xD8 - 0xDA : unassigned

#define VK_OEM_4 0xDB // '[{' for US
#define VK_OEM_5 0xDC // '\|' for US
#define VK_OEM_6 0xDD // ']}' for US
#define VK_OEM_7 0xDE // ''"' for US
#define VK_OEM_8 0xDF

// 0xE0 : reserved


// Various extended or enhanced keyboards

#define VK_OEM_AX 0xE1 // 'AX' key on Japanese AX kbd
#define VK_OEM_102 0xE2 // "<>" or "\|" on RT 102-key kbd.
#define VK_ICO_HELP 0xE3 // Help key on ICO
#define VK_ICO_00 0xE4 // 00 key on ICO

#if(WINVER >= 0x0400)
#define VK_PROCESSKEY 0xE5
#endif

#define VK_ICO_CLEAR 0xE6


#if(_WIN32_WINNT >= 0x0500)
#define VK_PACKET 0xE7
#endif

// 0xE8 : unassigned


// Nokia/Ericsson definitions
#define VK_OEM_RESET 0xE9
#define VK_OEM_JUMP 0xEA
#define VK_OEM_PA1 0xEB
#define VK_OEM_PA2 0xEC
#define VK_OEM_PA3 0xED
#define VK_OEM_WSCTRL 0xEE
#define VK_OEM_CUSEL 0xEF
#define VK_OEM_ATTN 0xF0
#define VK_OEM_FINISH 0xF1
#define VK_OEM_COPY 0xF2
#define VK_OEM_AUTO 0xF3
#define VK_OEM_ENLW 0xF4
#define VK_OEM_BACKTAB 0xF5

#define VK_ATTN 0xF6
#define VK_CRSEL 0xF7
#define VK_EXSEL 0xF8
#define VK_EREOF 0xF9
#define VK_PLAY 0xFA
#define VK_ZOOM 0xFB
#define VK_NONAME 0xFC
#define VK_PA1 0xFD
#define VK_OEM_CLEAR 0xFE

// 0xFF : reserved

struct VKeyInfo
{
	unsigned int VKey;
	LPCSTR VKname;
};

#define AddVKey(VK, VKName) {(VK), (VKName)}

static struct VKeyInfo vkis[] =
{
	AddVKey(0x00, "None"), // THERE IS NO SUCH THING AS 0x00 - I INVENTED THIS BECAUSE I'M LAZY - TERRIBLE HACK
	AddVKey(VK_LBUTTON, "Left Mouse Button"),
	AddVKey(VK_RBUTTON, "Right Mouse Button"),
	AddVKey(VK_CANCEL, "Control-Break"),
	AddVKey(0x04, "Middle Mouse Button"),
	AddVKey(0x05, "X1 Mouse Button"),
	AddVKey(0x06, "X2 Mouse Button"),
	AddVKey(0x07, "Undefined"),
	AddVKey(VK_BACK, "Backspace"),
	AddVKey(VK_TAB, "Tab"),
	AddVKey(0x0A, "Mouse Wheel Up"), // THERE IS NO SUCH THING AS VK_WHEELUP - I INVENTED THIS BECAUSE I'M LAZY - TERRIBLE HACK
	AddVKey(0x0B, "Mouse Wheel Down"), // THERE IS NO SUCH THING AS VK_WHEELDOWN - I INVENTED THIS BECAUSE I'M LAZY - TERRIBLE HACK
	AddVKey(VK_CLEAR, "Clear"),
	AddVKey(VK_RETURN, "Enter"),
	AddVKey(0x0E, "Mouse Wheel Left"), // THERE IS NO SUCH THING AS VK_WHEELLEFT - I INVENTED THIS BECAUSE I'M LAZY - TERRIBLE HACK
	AddVKey(0x0F, "Mouse Wheel Right"), // THERE IS NO SUCH THING AS VK_WHEELRIGHT - I INVENTED THIS BECAUSE I'M LAZY - TERRIBLE HACK
	AddVKey(VK_SHIFT, "Shift"),
	AddVKey(VK_CONTROL, "Control"),
	AddVKey(VK_MENU, "Alt"),
	AddVKey(VK_PAUSE, "Pause"),
	AddVKey(VK_CAPITAL, "Caps Lock"),
	AddVKey(VK_KANA, "IME Kana"), // Input Method Editor (IME) Kana mode
	AddVKey(VK_HANGUL, "IME Hangul"),
	AddVKey(0x16, "Undefined"),
	AddVKey(VK_JUNJA, "IME Junja"),
	AddVKey(VK_FINAL, "IME final"),
	AddVKey(VK_HANJA, "IME Hanja"),
	AddVKey(VK_KANJI, "IME Kanji"),
	AddVKey(0x1A, "Undefined"),
	AddVKey(VK_ESCAPE, "Escape"),
	AddVKey(VK_CONVERT, "IME Convert"),
	AddVKey(VK_NONCONVERT, "IME Non-Convert"),
	AddVKey(VK_ACCEPT, "IME Accept"),
	AddVKey(VK_MODECHANGE, "IME Mode Change"),
	AddVKey(VK_SPACE, "Spacebar"),
	AddVKey(VK_PRIOR, "Page Up"),
	AddVKey(VK_NEXT, "Page Down"),
	AddVKey(VK_END, "End"),
	AddVKey(VK_HOME, "Home"),
	AddVKey(VK_LEFT, "Left Arrow"),
	AddVKey(VK_UP, "Up Arrow"),
	AddVKey(VK_RIGHT, "Right Arrow"),
	AddVKey(VK_DOWN, "Down Arrow"),
	AddVKey(VK_SELECT, "Select"),
	AddVKey(VK_PRINT, "Print"),
	AddVKey(VK_EXECUTE, "Execute"),
	AddVKey(VK_SNAPSHOT, "Print Screen"),
	AddVKey(VK_INSERT, "Insert"),
	AddVKey(VK_DELETE, "Delete"),
	AddVKey(VK_HELP, "Help"),
	AddVKey(0x30, "0"),
	AddVKey(0x31, "1"),
	AddVKey(0x32, "2"),
	AddVKey(0x33, "3"),
	AddVKey(0x34, "4"),
	AddVKey(0x35, "5"),
	AddVKey(0x36, "6"),
	AddVKey(0x37, "7"),
	AddVKey(0x38, "8"),
	AddVKey(0x39, "9"),
	AddVKey(0x3A, "Undefined"),
	AddVKey(0x3B, "Undefined"),
	AddVKey(0x3C, "Undefined"),
	AddVKey(0x3D, "Undefined"),
	AddVKey(0x3E, "Undefined"),
	AddVKey(0x3F, "Undefined"),
	AddVKey(0x40, "Undefined"),
	AddVKey(0x41, "A"),
	AddVKey(0x42, "B"),
	AddVKey(0x43, "C"),
	AddVKey(0x44, "D"),
	AddVKey(0x45, "E"),
	AddVKey(0x46, "F"),
	AddVKey(0x47, "G"),
	AddVKey(0x48, "H"),
	AddVKey(0x49, "I"),
	AddVKey(0x4A, "J"),
	AddVKey(0x4B, "K"),
	AddVKey(0x4C, "L"),
	AddVKey(0x4D, "M"),
	AddVKey(0x4E, "N"),
	AddVKey(0x4F, "O"),
	AddVKey(0x50, "P"),
	AddVKey(0x51, "Q"),
	AddVKey(0x52, "R"),
	AddVKey(0x53, "S"),
	AddVKey(0x54, "T"),
	AddVKey(0x55, "U"),
	AddVKey(0x56, "V"),
	AddVKey(0x57, "W"),
	AddVKey(0x58, "X"),
	AddVKey(0x59, "Y"),
	AddVKey(0x5A, "Z"),

	AddVKey(VK_LWIN, "Left Windows"), // Left Windows key (Microsoft Natural keyboard)
	AddVKey(VK_RWIN, "Right Windows"), // Right Windows key (Natural keyboard)
	AddVKey(VK_APPS, "App"), // Applications key (Natural keyboard)
	AddVKey(0x5E, "Reserved"),
	AddVKey(VK_SLEEP, "Sleep"),
	AddVKey(VK_NUMPAD0, "Numpad #0"),
	AddVKey(VK_NUMPAD1, "Numpad #1"),
	AddVKey(VK_NUMPAD2, "Numpad #2"),
	AddVKey(VK_NUMPAD3, "Numpad #3"),
	AddVKey(VK_NUMPAD4, "Numpad #4"),
	AddVKey(VK_NUMPAD5, "Numpad #5"),
	AddVKey(VK_NUMPAD6, "Numpad #6"),
	AddVKey(VK_NUMPAD7, "Numpad #7"),
	AddVKey(VK_NUMPAD8, "Numpad #8"),
	AddVKey(VK_NUMPAD9, "Numpad #9"),
	AddVKey(VK_MULTIPLY, "Numpad *"),
	AddVKey(VK_ADD, "Numpad +"),
	AddVKey(VK_SEPARATOR, "Separator"),
	AddVKey(VK_SUBTRACT, "Numpad -"),
	AddVKey(VK_DECIMAL, "Numpad ."),
	AddVKey(VK_DIVIDE, "Numpad /"),
	AddVKey(VK_F1, "F1"),
	AddVKey(VK_F2, "F2"),
	AddVKey(VK_F3, "F3"),
	AddVKey(VK_F4, "F4"),
	AddVKey(VK_F5, "F5"),
	AddVKey(VK_F6, "F6"),
	AddVKey(VK_F7, "F7"),
	AddVKey(VK_F8, "F8"),
	AddVKey(VK_F9, "F9"),
	AddVKey(VK_F10, "F10"),
	AddVKey(VK_F11, "F11"),
	AddVKey(VK_F12, "F12"),
	AddVKey(VK_F13, "F13"),
	AddVKey(VK_F14, "F14"),
	AddVKey(VK_F15, "F15"),
	AddVKey(VK_F16, "F16"),
	AddVKey(VK_F17, "F17"),
	AddVKey(VK_F18, "F18"),
	AddVKey(VK_F19, "F19"),
	AddVKey(VK_F20, "F20"),
	AddVKey(VK_F21, "F21"),
	AddVKey(VK_F22, "F22"),
	AddVKey(VK_F23, "F23"),
	AddVKey(VK_F24, "F24"),
	AddVKey(0x88, "Unassigned"),
	AddVKey(0x89, "Unassigned"),
	AddVKey(0x8A, "Unassigned"),
	AddVKey(0x8B, "Unassigned"),
	AddVKey(0x8C, "Unassigned"),
	AddVKey(0x8D, "Unassigned"),
	AddVKey(0x8E, "Unassigned"),
	AddVKey(0x8F, "Unassigned"),
	AddVKey(VK_NUMLOCK, "Num Lock"),
	AddVKey(VK_SCROLL, "Scroll Lock"),
	AddVKey(0x92, "OEM specific"),
	AddVKey(0x93, "OEM specific"),
	AddVKey(0x94, "OEM specific"),
	AddVKey(0x95, "OEM specific"),
	AddVKey(0x96, "OEM specific"),
	AddVKey(0x97, "Unassigned"),
	AddVKey(0x98, "Unassigned"),
	AddVKey(0x99, "Unassigned"),
	AddVKey(0x9A, "Unassigned"),
	AddVKey(0x9B, "Unassigned"),
	AddVKey(0x9C, "Unassigned"),
	AddVKey(0x9D, "Unassigned"),
	AddVKey(0x9E, "Unassigned"),
	AddVKey(0x9F, "Unassigned"),
	AddVKey(VK_LSHIFT, "Left Shift"),
	AddVKey(VK_RSHIFT, "Right Shift"),
	AddVKey(VK_LCONTROL, "Left Control"),
	AddVKey(VK_RCONTROL, "Right Control"),
	AddVKey(VK_LMENU, "Left Alt"),
	AddVKey(VK_RMENU, "Right Alt"),
	AddVKey(0xA6, "Browser Back"), // Windows 2000/XP: Browser Back key
	AddVKey(0xA7, "Browser Forward"), // Windows 2000/XP: Browser Forward key
	AddVKey(0xA8, "Browser Refresh"), // Windows 2000/XP: Browser Refresh key
	AddVKey(0xA9, "Browser Stop"), // Windows 2000/XP: Browser Stop key
	AddVKey(0xAA, "Browser Search"), // Windows 2000/XP: Browser Search key
	AddVKey(0xAB, "Browser Favorites"), // Windows 2000/XP: Browser Favorites key
	AddVKey(0xAC, "Browser Start/Home"), // Windows 2000/XP: Browser Start and Home key
	AddVKey(0xAD, "Volume Mute"), // Windows 2000/XP: Volume Mute key
	AddVKey(0xAE, "Volume Down"), // Windows 2000/XP: Volume Down key
	AddVKey(0xAF, "Volume Up"), // Windows 2000/XP: Volume Up key
	AddVKey(0xB0, "Next Track"), // Windows 2000/XP: Next Track key
	AddVKey(0xB1, "Previous Track"), // Windows 2000/XP: Previous Track key
	AddVKey(0xB2, "Stop Track"), // Windows 2000/XP: Stop Media key
	AddVKey(0xB3, "Play/Pause Track"), // Windows 2000/XP: Play/Pause Media key
	AddVKey(0xB4, "E-Mail"), // Windows 2000/XP: Start Mail key
	AddVKey(0xB5, "Select Track"), // Windows 2000/XP: Select Media key
	AddVKey(0xB6, "Start App 1"), // Windows 2000/XP: Start Application 1 key
	AddVKey(0xB7, "Start App 2"), // Windows 2000/XP: Start Application 2 key
	AddVKey(0xB8, "Reserved"),
	AddVKey(0xB9, "Reserved"),
	AddVKey(VK_OEM_1, ":"), // Used for miscellaneous characters; it can vary by keyboard." "Windows 2000/XP: For the US standard keyboard, the \';:\' key
	AddVKey(VK_OEM_PLUS, "+"), // Windows 2000/XP: For any country/region, the \'+\' key
	AddVKey(VK_OEM_COMMA, ","), // Windows 2000/XP: For any country/region, the \',\' key
	AddVKey(VK_OEM_MINUS, "-"), // Windows 2000/XP: For any country/region, the \'-\' key
	AddVKey(VK_OEM_PERIOD, "."), // Windows 2000/XP: For any country/region, the \'.\' key
	AddVKey(VK_OEM_2, "?"), // Used for miscellaneous characters; it can vary by keyboard." "Windows 2000/XP: For the US standard keyboard, the \'/?\' key
	AddVKey(VK_OEM_3, "~"), // Used for miscellaneous characters; it can vary by keyboard." "Windows 2000/XP: For the US standard keyboard, the \'`~\' key
	AddVKey(0xC1, "Reserved"),
	AddVKey(0xC2, "Reserved"),
	AddVKey(0xC3, "Reserved"),
	AddVKey(0xC4, "Reserved"),
	AddVKey(0xC5, "Reserved"),
	AddVKey(0xC6, "Reserved"),
	AddVKey(0xC7, "Reserved"),
	AddVKey(0xC8, "Reserved"),
	AddVKey(0xC9, "Reserved"),
	AddVKey(0xCA, "Reserved"),
	AddVKey(0xCB, "Reserved"),
	AddVKey(0xCC, "Reserved"),
	AddVKey(0xCD, "Reserved"),
	AddVKey(0xCE, "Reserved"),
	AddVKey(0xCF, "Reserved"),
	AddVKey(0xD0, "Reserved"),
	AddVKey(0xD1, "Reserved"),
	AddVKey(0xD2, "Reserved"),
	AddVKey(0xD3, "Reserved"),
	AddVKey(0xD4, "Reserved"),
	AddVKey(0xD5, "Reserved"),
	AddVKey(0xD6, "Reserved"),
	AddVKey(0xD7, "Reserved"),
	AddVKey(0xD8, "Unassigned"),
	AddVKey(0xD9, "Unassigned"),
	AddVKey(0xDA, "Unassigned"),
	AddVKey(VK_OEM_4, "["), // Used for miscellaneous characters; it can vary by keyboard." "Windows 2000/XP: For the US standard keyboard, the \'[{\' key
	AddVKey(VK_OEM_5, "\\"), // Used for miscellaneous characters; it can vary by keyboard." "Windows 2000/XP: For the US standard keyboard, the \'\\|\' key
	AddVKey(VK_OEM_6, "]"), // Used for miscellaneous characters; it can vary by keyboard." "Windows 2000/XP: For the US standard keyboard, the \']}\' key
	AddVKey(VK_OEM_7, "'"), // Used for miscellaneous characters; it can vary by keyboard." "Windows 2000/XP: For the US standard keyboard, the \'single-quote/double-quote\' key

	AddVKey(VK_OEM_8, "OEM Misc"), // Used for miscellaneous characters; it can vary by keyboard.
	AddVKey(0xE0, "Reserved"),
	AddVKey(0xE1, "OEM Specific"),
	AddVKey(VK_OEM_102, "Angle Bracket"), // Windows 2000/XP: Either the angle bracket key or the backslash key on the RT 102-key keyboard
	AddVKey(0xE3, "OEM Specific"),
	AddVKey(0xE4, "OEM Specific"),
	AddVKey(VK_PROCESSKEY, "IME Process"), // Windows 95/98/Me, Windows NT 4.0, Windows 2000/XP: IME PROCESS key
	AddVKey(0xE6, "OEM Specific"),
	AddVKey(0xE7, "Packet"), // Windows 2000/XP: Used to pass Unicode characters as if they were keystrokes. The VK_PACKET key is the low word of a 32-bit Virtual Key value used for non-keyboard input methods. For more information, see Remark in KEYBDINPUT, SendInput, WM_KEYDOWN, and WM_KEYUP
	AddVKey(0xE8, "Unassigned"),
	AddVKey(0xE9, "OEM Specific"),
	AddVKey(0xEA, "OEM Specific"),
	AddVKey(0xEB, "OEM Specific"),
	AddVKey(0xEC, "OEM Specific"),
	AddVKey(0xED, "OEM Specific"),
	AddVKey(0xEF, "OEM Specific"),
	AddVKey(0xF0, "OEM Specific"),
	AddVKey(0xF1, "OEM Specific"),
	AddVKey(0xF2, "OEM Specific"),
	AddVKey(0xF3, "OEM Specific"),
	AddVKey(0xF4, "OEM Specific"),
	AddVKey(0xF5, "OEM Specific"),
	AddVKey(VK_ATTN, "Attn"),
	AddVKey(VK_CRSEL, "CrSel"),
	AddVKey(VK_EXSEL, "ExSel"),
	AddVKey(VK_EREOF, "Erase"),
	AddVKey(VK_PLAY, "Play"),
	AddVKey(VK_ZOOM, "Zoom"),
	AddVKey(VK_NONAME, "No Name"),
	AddVKey(VK_PA1, "Pa1"),
	AddVKey(VK_OEM_CLEAR, "Clear"),
	AddVKey(0xFF, "Invalid Key")
};

static LPCSTR GetKeyName(unsigned int VKey)
{
	for(unsigned int i = 0; i < ARRAY_SIZE(vkis); i++)
	{
		if(VKey == vkis[i].VKey)
			return vkis[i].VKname;
	}
	return vkis[0xFF].VKname; // return "Invalid Key"
}