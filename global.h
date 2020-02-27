//==========================================================================
// Mouse Injector Plugin
//==========================================================================
// Copyright (C) 2016-2020 Carnivorous
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
#define __MOUSE_INECTOR_VERSION__ "V2.0"
#define __CURRENTYEAR__ "2020"
#define CONSOLE { AllocConsole(); AttachConsole(GetCurrentProcessId()); freopen("CON", "w", stdout); }
#define ONLY1PLAYERACTIVE \
PROFILE[PLAYER1].SETTINGS[CONFIG] != DISABLED && PROFILE[PLAYER2].SETTINGS[CONFIG] == DISABLED && PROFILE[PLAYER3].SETTINGS[CONFIG] == DISABLED && PROFILE[PLAYER4].SETTINGS[CONFIG] == DISABLED || \
PROFILE[PLAYER1].SETTINGS[CONFIG] == DISABLED && PROFILE[PLAYER2].SETTINGS[CONFIG] != DISABLED && PROFILE[PLAYER3].SETTINGS[CONFIG] == DISABLED && PROFILE[PLAYER4].SETTINGS[CONFIG] == DISABLED || \
PROFILE[PLAYER1].SETTINGS[CONFIG] == DISABLED && PROFILE[PLAYER2].SETTINGS[CONFIG] == DISABLED && PROFILE[PLAYER3].SETTINGS[CONFIG] != DISABLED && PROFILE[PLAYER4].SETTINGS[CONFIG] == DISABLED || \
PROFILE[PLAYER1].SETTINGS[CONFIG] == DISABLED && PROFILE[PLAYER2].SETTINGS[CONFIG] == DISABLED && PROFILE[PLAYER3].SETTINGS[CONFIG] == DISABLED && PROFILE[PLAYER4].SETTINGS[CONFIG] != DISABLED
#if _MSC_VER && !__INTEL_COMPILER // here because some MSVC versions only support __inline :/
#define inline __inline
#endif
#ifdef SPEEDRUN_BUILD // gives unfair advantage, use smaller fov range for speedrun build
#define FOV_MIN 60
#define FOV_MAX 75
#else
#define FOV_MIN 40
#define FOV_MAX 120
#endif

inline float ClampFloat(const float value, const float min, const float max)
{
	const float test = value < min ? min : value;
	return test > max ? max : test;
}

inline int ClampInt(const int value, const int min, const int max)
{
	const int test = value < min ? min : value;
	return test > max ? max : test;
}

// profile struct
enum CONTROLLERENUM {FORWARDS = 0, BACKWARDS, STRAFELEFT, STRAFERIGHT, FIRE, AIM, ACCEPT, CANCEL, START, CROUCH, PREVIOUSWEAPON, NEXTWEAPON, UP, DOWN, LEFT, RIGHT};
enum CONFIGENUM {CONFIG = 0, SENSITIVITY, ACCELERATION, CROSSHAIR, INVERTPITCH, CROUCHTOGGLE, GEAIMMODE, PDAIMMODE, MOUSE, KEYBOARD};
enum QUICKCONFIGENUM {DISABLED = 0, WASD, ESDF, CUSTOM};
enum PLAYERSENUM {PLAYER1 = 0, PLAYER2, PLAYER3, PLAYER4, ALLPLAYERS};
enum DEVICETYPE {MOUSETYPE = 0, KEYBOARDTYPE};

struct
{
	int BUTTONPRIM[16];
	int BUTTONSEC[16];
	int SETTINGS[10];
} PROFILE[4];

// device struct (used for all game input)
struct
{
	int XPOS, YPOS; // mouse input
	int WHEEL; // mouse wheel buffer cool down
	int BUTTONPRIM[16]; // button values for primary set (0-1)
	int BUTTONSEC[16]; // button values for secondary set (0-1)
	int ARROW[4]; // arrow information
} DEVICE[4];

// plugin spec
#define PLUGIN_TYPE_CONTROLLER 4

#define PLUGIN_NONE 1
#define PLUGIN_MEMPAK 2
#define PLUGIN_RUMBLE_PAK 3 // not implemeted for non raw data
#define PLUGIN_TANSFER_PAK 4 // not implemeted for non raw data

// Structures (Note: BOOL, BYTE, WORD, DWORD, TRUE, FALSE are defined in windows.h)
#ifndef _WINDOWS_
	typedef unsigned short WORD;
	typedef int BOOL;
	typedef unsigned long DWORD;
	typedef void * HWND;
#endif

typedef struct
{
	WORD Version; // Should be set to 0x0100
	WORD Type; // Set to PLUGIN_TYPE_CONTROLLER
	char Name[100]; // Name of the DLL
	BOOL Reserved1;
	BOOL Reserved2;
} PLUGIN_INFO;

typedef struct
{
	BOOL Present;
	BOOL RawData;
	int Plugin;
} CONTROL;

typedef union
{
	DWORD Value;
	struct
	{
		unsigned R_DPAD: 1;
		unsigned L_DPAD: 1;
		unsigned D_DPAD: 1;
		unsigned U_DPAD: 1;
		unsigned START_BUTTON: 1;
		unsigned Z_TRIG: 1;
		unsigned B_BUTTON: 1;
		unsigned A_BUTTON: 1;

		unsigned R_CBUTTON: 1;
		unsigned L_CBUTTON: 1;
		unsigned D_CBUTTON: 1;
		unsigned U_CBUTTON: 1;
		unsigned R_TRIG: 1;
		unsigned L_TRIG: 1;
		unsigned Reserved1: 1;
		unsigned Reserved2: 1;

		signed Y_AXIS: 8;

		signed X_AXIS: 8;
	};
} BUTTONS;

BUTTONS CONTROLLER[4]; // end of plugin spec