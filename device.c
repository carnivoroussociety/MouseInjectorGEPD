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
#include <stdlib.h>
#include <windows.h>
#include "global.h"
#include "device.h"
#include "maindll.h"
#include "./manymouse/manymouse.h"
#include "./games/game.h"

#define VK_LBUTTON 0x01 // IDs from vkey.h
#define VK_RBUTTON 0x02
#define VK_MBUTTON 0x04
#define VK_XBUTTON1 0x05
#define VK_XBUTTON2 0x06
#define VK_WHEELUP 0x0A
#define VK_WHEELDOWN 0x0B
#define VK_WHEELLEFT 0x0E
#define VK_WHEELRIGHT 0x0F

static int alreadyexec = 0; // has init already exec?
static int connected = 0; // number of devices connected
static POINT lockpos = {0}; // coords for mouse lock position

int windowactive = 1; // is emulator window active?

int DEV_Init(void);
void DEV_Quit(void);
DWORD WINAPI DEV_InjectThread();
int DEV_ReturnKey(void);
int DEV_ReturnDeviceID(const int devicetype);
const char *DEV_Name(const int id);
int DEV_Type(const int id);
int DEV_TypeIndex(const int id);
int DEV_TypeID(const int id, const int devicetype);

//==========================================================================
// Purpose: First called upon device launch, returns total connected ms/kb
// Changes Globals: alreadyexec, connected
//==========================================================================
int DEV_Init(void)
{
	if(!alreadyexec)
	{
		alreadyexec = 1;
		connected = ManyMouse_Init();
		int activemice = 0, activekeyboards = 0;
		for(int index = 0; index < connected; index++)
		{
			if(DEV_Type(index) == MOUSETYPE)
				activemice = 1; 
			else
				activekeyboards = 1;
		}
		if(!activemice || !activekeyboards) // fail if mouse or keyboard not detected (mouse and keyboard are required for this plugin)
			connected = 0;
	}
	if(connected >= 2) // if at least one mouse and keyboard are detected
		return connected;
	return 0;
}
//==========================================================================
// Purpose: Safely close ManyMouse
// Changes Globals: alreadyexec, connected
//==========================================================================
void DEV_Quit(void)
{
	ManyMouse_Quit();
	alreadyexec = 0;
	connected = 0;
}
//==========================================================================
// Purpose: Polls ManyMouse for input and injects into game
// Changes Globals: a lot
//==========================================================================
DWORD WINAPI DEV_InjectThread()
{
	ManyMouseEvent event; // hold current input event (movement, buttons, ect)
	memset(&DEVICE, 0, sizeof(DEVICE)); // clear device struct
	int checkwindowtick = 0; // check if emulator window is in focus
	int togglebuffer = 0; // buffer cool down for mouse toggle
	int acceptalldevices = 0; // accept all device input (used if only 1 player is active)
	unsigned char lockmousecounter = 0; // limit SetCursorPos execution
	while(ManyMouse_PollEvent(&event)) // flush message pump (max messages is 1024)
	{
		continue;
	}
	while(!stopthread)
	{
		if(mousetoggle && lockmousecounter % (emuoverclock ? TICKRATE_STOCK : TICKRATE_OC) == 0) // don't execute every tick
			SetCursorPos(lockpos.x, lockpos.y); // set mouse position to lock position
		lockmousecounter++; // overflow pseudo-counter
		if(togglebuffer > 0)
			togglebuffer--;
		for(int player = PLAYER1; player < ALLPLAYERS; player++)
		{
			DEVICE[player].XPOS = 0, DEVICE[player].YPOS = 0; // reset mouse input
			if(DEVICE[player].WHEEL <= 1) // mouse wheel is not an instant key - treat with cooldown and turn button off once cooled off
			{
				DEVICE[player].WHEEL = 0;
				for(int button = 0; button < TOTALBUTTONS; button++) // reset wheel scroll once cooled down
				{
					if(PROFILE[player].BUTTONPRIM[button] == VK_WHEELUP || PROFILE[player].BUTTONPRIM[button] == VK_WHEELDOWN || PROFILE[player].BUTTONPRIM[button] == VK_WHEELLEFT || PROFILE[player].BUTTONPRIM[button] == VK_WHEELRIGHT)
						DEVICE[player].BUTTONPRIM[button] = 0;
					if(PROFILE[player].BUTTONSEC[button] == VK_WHEELUP || PROFILE[player].BUTTONSEC[button] == VK_WHEELDOWN || PROFILE[player].BUTTONSEC[button] == VK_WHEELLEFT || PROFILE[player].BUTTONSEC[button] == VK_WHEELRIGHT)
						DEVICE[player].BUTTONSEC[button] = 0;
				}
			}
			else
				DEVICE[player].WHEEL--;
		}
		while(ManyMouse_PollEvent(&event))
		{
			for(int player = PLAYER1; player < ALLPLAYERS; player++)
			{
				if(PROFILE[player].SETTINGS[CONFIG] == DISABLED) // don't check for disabled players
					continue;
				acceptalldevices = ONLY1PLAYERACTIVE; // do not filter devices if only one player is active
				if(PROFILE[player].SETTINGS[MOUSE] == (int)event.device || acceptalldevices) // mouse movement
				{
					if(event.type == MANYMOUSE_EVENT_RELMOTION && mousetoggle)
					{
						if(event.item == 0)
							DEVICE[player].XPOS += event.value;
						else
							DEVICE[player].YPOS += event.value;
					}
					if(event.type == MANYMOUSE_EVENT_BUTTON) // key presses
					{
						for(int button = 0; button < TOTALBUTTONS; button++)
						{
							int offset = event.item > 1 ? 2 : 1; // mouse button needs offset to sync with VK table (left click + offset = 0x01 aka VK_LBUTTON and right click + offset = 0x02 aka VK_RBUTTON)
							if(PROFILE[player].BUTTONPRIM[button] == (int)event.item + offset)
								DEVICE[player].BUTTONPRIM[button] = event.value;
							if(PROFILE[player].BUTTONSEC[button] == (int)event.item + offset)
								DEVICE[player].BUTTONSEC[button] = event.value;
						}
					}
					if(event.type == MANYMOUSE_EVENT_SCROLL) // mouse wheel
					{
						for(int button = 0; button < TOTALBUTTONS; button++)
						{
							if(event.item == 0) // if VK_WHEELUP/VK_WHEELDOWN
							{
								if(event.value > 0 && PROFILE[player].BUTTONPRIM[button] == VK_WHEELUP || event.value < 0 && PROFILE[player].BUTTONPRIM[button] == VK_WHEELDOWN)
									DEVICE[player].BUTTONPRIM[button] = event.value != 0;
								if(event.value > 0 && PROFILE[player].BUTTONSEC[button] == VK_WHEELUP || event.value < 0 && PROFILE[player].BUTTONSEC[button] == VK_WHEELDOWN)
									DEVICE[player].BUTTONSEC[button] = event.value != 0;
							}
							else // if VK_WHEELRIGHT/VK_WHEELLEFT
							{
								if(event.value > 0 && PROFILE[player].BUTTONPRIM[button] == VK_WHEELLEFT || event.value < 0 && PROFILE[player].BUTTONPRIM[button] == VK_WHEELRIGHT)
									DEVICE[player].BUTTONPRIM[button] = event.value != 0;
								if(event.value > 0 && PROFILE[player].BUTTONSEC[button] == VK_WHEELLEFT || event.value < 0 && PROFILE[player].BUTTONSEC[button] == VK_WHEELRIGHT)
									DEVICE[player].BUTTONSEC[button] = event.value != 0;
							}
						}
						DEVICE[player].WHEEL = 16; // hold button down for 32ms/64ms (non-oc emu need longer buffer because they run at lower framerate)
					}
				}
				if(event.type == MANYMOUSE_EVENT_KEYBOARD && (PROFILE[player].SETTINGS[KEYBOARD] == (int)event.device || acceptalldevices))
				{
					for(int button = 0; button < TOTALBUTTONS; button++) // check for key presses
					{
						if(PROFILE[player].BUTTONPRIM[button] == (int)event.item)
							DEVICE[player].BUTTONPRIM[button] = event.value;
						if(PROFILE[player].BUTTONSEC[button] == (int)event.item)
							DEVICE[player].BUTTONSEC[button] = event.value;
					}
					if(mousetogglekey == (int)event.item && event.value && !togglebuffer && windowactive) // only toggle on press and if window is active
					{
						mousetoggle = !mousetoggle;
						GetCursorPos(&lockpos);
						togglebuffer = 100 / TICKRATE; // buffer for 100ms
						if(mouselockonfocus) // if user presses mouse toggle key while lock on focus is on, increase buffer size before mouse is relocked so user can temporarily control mouse to close game/change settings
							togglebuffer *= 15;
					}
				}
			}
		}
		if(checkwindowtick > (500 / TICKRATE)) // poll every 500ms
		{
			checkwindowtick = 0;
			if((mouseunlockonloss || !mousetoggle) && emulatorwindow != GetForegroundWindow()) // window is inactive
			{
				memset(&DEVICE, 0, sizeof(DEVICE)); // reset player input
				GAME_Inject(); // ship empty input to game
				windowactive = 0, mousetoggle = 0, togglebuffer = 0;
			}
			else // emu window is in focus
			{
				windowactive = 1;
				if(mouselockonfocus && !mousetoggle && !togglebuffer) // if mouselockonfocus is on and window was refocused/mouse toggle is off, turn on mouse toggle
				{
					mousetoggle = 1;
					GetCursorPos(&lockpos);
				}
			}
		}
		else
			checkwindowtick++;
		if(windowactive && GAME_Status() && !configdialogopen) // if emulator is focused, game is valid and config dialog isn't open
			GAME_Inject(); // send input to game driver
		Sleep(TICKRATE);
	}
	GAME_Quit(); // reset game driver's global variables
	return 0;
}
//==========================================================================
// Purpose: Returns a single key
//==========================================================================
int DEV_ReturnKey(void)
{
	int key = 0;
	ManyMouseEvent event; // hold current input event (movement, buttons, ect)
	while(ManyMouse_PollEvent(&event))
	{
		if(event.type == MANYMOUSE_EVENT_BUTTON)
		{
			if(event.item < 2)
				key = event.item ? VK_RBUTTON : VK_LBUTTON;
			else if(event.item == 2)
				key = VK_MBUTTON;
			else if(event.item == 3)
				key = VK_XBUTTON1;
			else
				key = VK_XBUTTON2;
		}
		else if(event.type == MANYMOUSE_EVENT_SCROLL)
		{
			if(event.item == 0)
				key = event.value > 0 ? VK_WHEELUP : VK_WHEELDOWN;
			else
				key = event.value > 0 ? VK_WHEELRIGHT : VK_WHEELLEFT;
		}
		else if(event.type == MANYMOUSE_EVENT_KEYBOARD)
			key = event.item;
	}
	return ClampInt(key, 0, 0xFF); // sanity check key return
}
//==========================================================================
// Purpose: Returns device id of detected input
//==========================================================================
int DEV_ReturnDeviceID(const int devicetype)
{
	ManyMouseEvent event; // hold current input event (movement, buttons, ect)
	while(ManyMouse_PollEvent(&event))
		if(devicetype == MOUSETYPE && (event.type == MANYMOUSE_EVENT_BUTTON || event.type == MANYMOUSE_EVENT_SCROLL) || devicetype == KEYBOARDTYPE && event.type == MANYMOUSE_EVENT_KEYBOARD)
			return event.device;
	return -1;
}
//==========================================================================
// Purpose: Returns the device name given a type
//==========================================================================
const char *DEV_Name(const int id)
{
	if(ManyMouse_DeviceName(id, MOUSETYPE) != NULL)
		return ManyMouse_DeviceName(id, MOUSETYPE);
	if(ManyMouse_DeviceName(id, KEYBOARDTYPE) != NULL)
		return ManyMouse_DeviceName(id, KEYBOARDTYPE);
	return NULL;
}
//==========================================================================
// Purpose: Wraps ManyMouse device name and returns the device type
//==========================================================================
int DEV_Type(const int id)
{
	return ManyMouse_DeviceName(id, MOUSETYPE) != NULL ? MOUSETYPE : KEYBOARDTYPE; // 0 mouse : 1 keyboard
}
//==========================================================================
// Purpose: Returns the index id within a type (for converting to device combobox)
//==========================================================================
int DEV_TypeIndex(const int id)
{
	int kbcount = -1, mscount = -1;
	for(int index = 0; index < DEV_Init(); index++)
	{
		if(DEV_Type(index) == MOUSETYPE)
			mscount++;
		else
			kbcount++;
		if(index == id)
			return DEV_Type(index) == MOUSETYPE ? mscount : kbcount;
	}
	return 0;
}
//==========================================================================
// Purpose: Converts a type index id to its real id (for converting from device combobox)
//==========================================================================
int DEV_TypeID(const int id, const int devicetype)
{
	int kbcount = -1, mscount = -1;
	for(int index = 0; index < DEV_Init(); index++)
	{
		if(DEV_Type(index) == MOUSETYPE)
			mscount++;
		else
			kbcount++;
		if(id == mscount && devicetype == MOUSETYPE || id == kbcount && devicetype == KEYBOARDTYPE)
			return index;
	}
	return 0;
}