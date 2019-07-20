//==========================================================================
// Mouse Injector Plugin
//==========================================================================
// Copyright (C) 2016-2019 Carnivorous
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
#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <commctrl.h>
#include "global.h"
#include "maindll.h"
#include "device.h"
#include "discord.h"
#include "games/game.h"
#include "./ui/resource.h"
#include "vkey.h"

#define DLLEXPORT __declspec(dllexport)
#define CALL __cdecl

static HINSTANCE hInst = NULL;
static HANDLE injectthread = 0; // thread identifier
static wchar_t inifilepath[MAX_PATH]; // mouseinjector.ini filepath
static const char inifilepathdefault[MAX_PATH] = ".\\plugin\\mouseinjector.ini"; // mouseinjector.ini filepath (safe default char type)
static int lastinputbutton = 0; // used to check and see if user pressed button twice in a row (avoid loop for spacebar/enter/click)
static int currentplayer = PLAYER1;
static int defaultmouse = -1, defaultkeyboard = -1;
static CONTROL *ctrlptr = NULL;
static int currentlyconfiguring = 0;
static int changeratio = 0; // used to display different hoz fov for 4:3/16:9 ratio

const unsigned char **rdramptr = 0; // pointer to emulator's rdram table
const unsigned char **romptr = 0; // pointer to emulator's loaded rom
int stopthread = 1; // 1 to end inject thread
int mousetogglekey = 0x34; // default key is 4
int mousetoggle = 0; // mouse lock
int mouselockonfocus = 0; // lock mouse when 1964 is focused
int mouseunlockonloss = 1; // unlock mouse when 1964 is unfocused
HWND emulatorwindow = NULL;
int emuoverclock = 1; // is this emu overclocked?
int overridefov = 60; // fov override
int geshowcrosshair = 0; // inject the always show ge crosshair hack on start

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);
static int Init(const HWND hW);
static void End(void);
static void StartInjection(void);
static void StopInjection(void);
static BOOL CALLBACK GUI_Config(HWND hW, UINT uMsg, WPARAM wParam, LPARAM lParam);
static void GUI_Init(const HWND hW);
static void GUI_Refresh(const HWND hW, const int revertbtn);
static void GUI_ProcessKey(const HWND hW, const int buttonid, const int primflag);
static void GUI_DetectDevice(const HWND hW, const int buttonid);
static void INI_Load(const HWND hW, const int loadplayer);
static void INI_Save(const HWND hW);
static void INI_Reset(const int playerflag);
static void INI_SetConfig(const int playerflag, const int config);
static void UpdateControllerStatus(void);
DLLEXPORT void CALL CloseDLL(void);
DLLEXPORT void CALL ControllerCommand(int Control, BYTE *Command);
DLLEXPORT void CALL DllAbout(HWND hParent);
DLLEXPORT void CALL DllConfig(HWND hParent);
DLLEXPORT void CALL DllTest(HWND hParent);
DLLEXPORT void CALL GetDllInfo(PLUGIN_INFO *PluginInfo);
DLLEXPORT void CALL GetKeys(int Control, BUTTONS* Keys);
DLLEXPORT void CALL InitiateControllers(HWND hMainWindow, CONTROL Controls[4]);
DLLEXPORT void CALL ReadController(int Control, BYTE *Command);
DLLEXPORT void CALL RomClosed(void);
DLLEXPORT void CALL RomOpen(void);
DLLEXPORT void CALL WM_KeyDown(WPARAM wParam, LPARAM lParam);
DLLEXPORT void CALL WM_KeyUp(WPARAM wParam, LPARAM lParam);
DLLEXPORT void CALL HookRDRAM(DWORD *Mem, int OCFactor);
DLLEXPORT void CALL HookROM(DWORD *Rom);

//==========================================================================
// Purpose: first called upon launch
// Changed Globals: hInst, inifilepath
//==========================================================================
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch(fdwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
			hInst = hinstDLL;
			wchar_t filepath[MAX_PATH], directory[MAX_PATH];
			GetModuleFileNameW(hInst, filepath, MAX_PATH);
			if(filepath != NULL)
			{
				const wchar_t slash[] = L"\\";
				wchar_t *dllname;
				unsigned int dllnamelength = 19;
				dllname = wcspbrk(filepath, slash);
				while(dllname != NULL) // find the last filename in full filepath and set filename length to dllnamelength (skip to slash every loop until last filename is found)
				{
					dllnamelength = wcslen(dllname);
					dllname = wcspbrk(dllname + 1, slash);
				}
				wcsncpy(directory, filepath, wcslen(filepath) - dllnamelength + 1); // remove dll filename from filepath string to get directory path
				directory[wcslen(filepath) - dllnamelength + 1] = L'\0'; // string needs terminator so add zero character to end
				wcsncpy(inifilepath, directory, MAX_PATH); // copy directory to inifilepath
				wcscat(inifilepath, L"mouseinjector.ini"); // add mouseinjector.ini to inifilepath, to get complete filepath to mouseinjector.ini
			}
			break;
		}
		default:
			break;
	}
	return TRUE;
}
//==========================================================================
// Purpose: init manymouse and get first keyboard/mouse instance
// Changed Globals: defaultmouse, defaultkeyboard
//==========================================================================
static int Init(const HWND hW)
{
	if(!DEV_Init()) // if devices are not detected, return 0
		return 0;
	for(int connectedindex = 0; connectedindex < DEV_Init(); connectedindex++) // get default devices
	{
		if(DEV_Type(connectedindex) == MOUSETYPE && defaultmouse == -1)
			defaultmouse = connectedindex;
		else if(DEV_Type(connectedindex) == KEYBOARDTYPE && defaultkeyboard == -1)
			defaultkeyboard = connectedindex;
		if(defaultmouse != -1 && defaultkeyboard != -1)
			break;
	}
	INI_Load(hW, ALLPLAYERS); // inform user if mouseinjector.ini is corrupted/missing
	UpdateControllerStatus();
	return 1;
}
//==========================================================================
// Purpose: close plugin
// Changed Globals: currentlyconfiguring, mousetoggle, lastinputbutton, rdramptr, romptr, ctrlptr
//==========================================================================
static void End(void)
{
	StopInjection(); // stop device/injection thread
	DEV_Quit(); // shutdown manymouse
	DRP_Quit(); // shutdown discord rich presence
	currentlyconfiguring = 0;
	mousetoggle = 0;
	lastinputbutton = 0;
	rdramptr = 0;
	romptr = 0;
	ctrlptr = NULL;
}
//==========================================================================
// Purpose: start device injection thread
// Changed Globals: stopthread, injectthread
//==========================================================================
static void StartInjection(void)
{
	if(stopthread) // check if thread isn't running already
	{
		stopthread = 0;
		injectthread = CreateThread(NULL, 0, DEV_InjectThread, NULL, 0, NULL); // start thread, return thread identifier
	}
}
//==========================================================================
// Purpose: stop device injection thread
// Changed Globals: stopthread, injectthread
//==========================================================================
static void StopInjection(void)
{
	if(!stopthread) // check if thread is running
	{
		stopthread = 1;
		CloseHandle(injectthread);
	}
}
//==========================================================================
// Purpose: manage config window
// Changed Globals: lastinputbutton, PROFILE.SETTINGS, currentplayer, overridefov, changeratio, geshowcrosshair, mouselockonfocus, mouseunlockonloss
//==========================================================================
static BOOL CALLBACK GUI_Config(HWND hW, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_INITDIALOG:
			GUI_Init(hW); // set defaults/add items
			GUI_Refresh(hW, 0); // refresh the interface
			lastinputbutton = 0; // reset lastinputbutton
			return TRUE;
		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case IDC_CONFIGBOX:
					if(PROFILE[currentplayer].SETTINGS[CONFIG] != SendMessage(GetDlgItem(hW, IDC_CONFIGBOX), CB_GETCURSEL, 0, 0)) // if player's profile was changed, refresh gui
					{
						PROFILE[currentplayer].SETTINGS[CONFIG] = SendMessage(GetDlgItem(hW, IDC_CONFIGBOX), CB_GETCURSEL, 0, 0); // get profile setting
						if(PROFILE[currentplayer].SETTINGS[CONFIG] == WASD || PROFILE[currentplayer].SETTINGS[CONFIG] == ESDF) // if profile set to WASD/ESDF
							INI_SetConfig(currentplayer, PROFILE[currentplayer].SETTINGS[CONFIG]);
						GUI_Refresh(hW, 1);
					}
					break;
				case IDC_OK:
					INI_Save(hW);
					EndDialog(hW, FALSE);
					return TRUE;
				case IDC_HELPPOPUP:
					MessageBox(hW, "\tIf you are having issues, please read the file\n\tBUNDLE_README.txt located in the 1964 directory.\n\n\tMouse Injector for GE/PD, Copyright (C) "__CURRENTYEAR__" Carnivorous\n\tMouse Injector comes with ABSOLUTELY NO WARRANTY;\n\tThis is free software, and you are welcome to redistribute it\n\tunder the terms of the GNU General Public License.\n\n\tThis plugin is powered by ManyMouse input library,\n\tCopyright (C) 2005-2012 Ryan C. Gordon <icculus.org>", "Mouse Injector - Help", MB_ICONINFORMATION | MB_OK);
					break;
				case IDC_CANCEL:
					INI_Load(hW, ALLPLAYERS); // reload all player settings from file
					EndDialog(hW, FALSE);
					return TRUE;
				case IDC_MOUSESELECT:
				case IDC_KEYBOARDSELECT:
					PROFILE[currentplayer].SETTINGS[MOUSE] = DEV_TypeID(SendMessage(GetDlgItem(hW, IDC_MOUSESELECT), CB_GETCURSEL, 0, 0), 0);
					PROFILE[currentplayer].SETTINGS[KEYBOARD] = DEV_TypeID(SendMessage(GetDlgItem(hW, IDC_KEYBOARDSELECT), CB_GETCURSEL, 0, 0), 1);
					break;
				case IDC_CLEAR:
					INI_Reset(currentplayer);
					GUI_Refresh(hW, 1); // refresh and set revert button's state to true
					break;
				case IDC_REVERT:
					INI_Load(hW, currentplayer); // reload settings for current player
					GUI_Refresh(hW, 0);
					break;
				case IDC_PLAYER1:
				case IDC_PLAYER2:
				case IDC_PLAYER3:
				case IDC_PLAYER4:
					lastinputbutton = 0;
					currentplayer = LOWORD(wParam) - IDC_PLAYER1; // update currentplayer to new selected player
					GUI_Refresh(hW, 0);
					break;
				case IDC_DETECTDEVICE:
					GUI_DetectDevice(hW, LOWORD(wParam));
					break;
				case IDC_PRIMARY00:
				case IDC_PRIMARY01:
				case IDC_PRIMARY02:
				case IDC_PRIMARY03:
				case IDC_PRIMARY04:
				case IDC_PRIMARY05:
				case IDC_PRIMARY06:
				case IDC_PRIMARY07:
				case IDC_PRIMARY08:
				case IDC_PRIMARY09:
				case IDC_PRIMARY10:
				case IDC_PRIMARY11:
				case IDC_PRIMARY12:
				case IDC_PRIMARY13:
				case IDC_PRIMARY14:
				case IDC_PRIMARY15:
					GUI_ProcessKey(hW, LOWORD(wParam), 0);
					break;
				case IDC_SECONDARY00:
				case IDC_SECONDARY01:
				case IDC_SECONDARY02:
				case IDC_SECONDARY03:
				case IDC_SECONDARY04:
				case IDC_SECONDARY05:
				case IDC_SECONDARY06:
				case IDC_SECONDARY07:
				case IDC_SECONDARY08:
				case IDC_SECONDARY09:
				case IDC_SECONDARY10:
				case IDC_SECONDARY11:
				case IDC_SECONDARY12:
				case IDC_SECONDARY13:
				case IDC_SECONDARY14:
				case IDC_SECONDARY15:
					GUI_ProcessKey(hW, LOWORD(wParam), 1);
					break;
				case IDC_INVERTPITCH:
				case IDC_CROUCHTOGGLE:
				case IDC_GECURSORAIMING:
				case IDC_PDCURSORAIMING:
					PROFILE[currentplayer].SETTINGS[LOWORD(wParam) - IDC_INVERTPITCH + INVERTPITCH] = SendMessage(GetDlgItem(hW, LOWORD(wParam)), BM_GETCHECK, 0, 0);
					EnableWindow(GetDlgItem(hW, IDC_REVERT), 1);
					break;
				case IDC_RESETFOV:
					overridefov = 60;
					GUI_Refresh(hW, 2); // fov is a global option that applies to all players, send ignore revert button's state and refresh labels and slider
					break;
				case IDC_FOV_DEGREES: // if clicked this change the hor fov ratio calculation
				case IDC_FOV_NOTE:
					if(stopthread) // do this if game isn't running
					{
						changeratio = !changeratio;
						GUI_Refresh(hW, 2);
						SetDlgItemText(hW, IDC_FOV_NOTE, changeratio ? "FOV - 4:3 Ratio" : "FOV - 16:9 Ratio");
					}
					break;
				case IDC_GESHOWCROSSHAIR:
					geshowcrosshair = SendMessage(GetDlgItem(hW, LOWORD(wParam)), BM_GETCHECK, 0, 0);
					break;
				case IDC_LOCK:
					GUI_ProcessKey(hW, LOWORD(wParam), 2);
					break;
				case IDC_LOCKONFOCUS:
					mouselockonfocus = SendMessage(GetDlgItem(hW, LOWORD(wParam)), BM_GETCHECK, 0, 0);
					break;
				case IDC_UNLOCKONWINLOSS:
					mouseunlockonloss = SendMessage(GetDlgItem(hW, LOWORD(wParam)), BM_GETCHECK, 0, 0);
					break;
				default:
					break;
			}
			break;
		}
		case WM_HSCROLL:
		{
			if(overridefov != SendMessage(GetDlgItem(hW, IDC_FOV), TBM_GETPOS, 0, 0)) // if fov slider moved
			{
				overridefov = SendMessage(GetDlgItem(hW, IDC_FOV), TBM_GETPOS, 0, 0);
				GUI_Refresh(hW, 2); // refresh and ignore revert button's state (fov slider is a global option for all players, ignore revert button because player's profile didn't change)
			}
			if(PROFILE[currentplayer].SETTINGS[SENSITIVITY] != SendMessage(GetDlgItem(hW, IDC_SLIDER00), TBM_GETPOS, 0, 0) || PROFILE[currentplayer].SETTINGS[ACCELERATION] != SendMessage(GetDlgItem(hW, IDC_SLIDER01), TBM_GETPOS, 0, 0) || PROFILE[currentplayer].SETTINGS[CROSSHAIR] != SendMessage(GetDlgItem(hW, IDC_SLIDER02), TBM_GETPOS, 0, 0)) // if profile sliders have moved
			{
				PROFILE[currentplayer].SETTINGS[SENSITIVITY] = SendMessage(GetDlgItem(hW, IDC_SLIDER00), TBM_GETPOS, 0, 0);
				PROFILE[currentplayer].SETTINGS[ACCELERATION] = SendMessage(GetDlgItem(hW, IDC_SLIDER01), TBM_GETPOS, 0, 0);
				PROFILE[currentplayer].SETTINGS[CROSSHAIR] = SendMessage(GetDlgItem(hW, IDC_SLIDER02), TBM_GETPOS, 0, 0);
				GUI_Refresh(hW, 1); // refresh and enable revert
			}
			break;
		}
		case WM_CLOSE:
		case WM_DESTROY:
			EndDialog(hW, FALSE);
			return TRUE;
		default:
			break;
	}
	return FALSE;
}
//==========================================================================
// Purpose: load the interface
//==========================================================================
static void GUI_Init(const HWND hW)
{
	CheckRadioButton(hW, IDC_PLAYER1, IDC_PLAYER4, IDC_PLAYER1 + currentplayer); // check current player's radio button
	for(int connectedindex = 0; connectedindex < DEV_Init(); connectedindex++) // add devices to combobox
	{
		char devicename[256];
		sprintf(devicename, "%d: %s", DEV_TypeIndex(connectedindex), DEV_Name(connectedindex)); // create string for combobox
		SendMessage(GetDlgItem(hW, DEV_Type(connectedindex) == MOUSETYPE ? IDC_MOUSESELECT : IDC_KEYBOARDSELECT), CB_ADDSTRING, 0, (LPARAM)devicename); // add device to appropriate combobox
	}
	SendMessage(GetDlgItem(hW, IDC_CONFIGBOX), CB_ADDSTRING, 0, (LPARAM)"Disabled"); // add default configs
	SendMessage(GetDlgItem(hW, IDC_CONFIGBOX), CB_ADDSTRING, 0, (LPARAM)"WASD");
	SendMessage(GetDlgItem(hW, IDC_CONFIGBOX), CB_ADDSTRING, 0, (LPARAM)"ESDF");
	SendMessage(GetDlgItem(hW, IDC_CONFIGBOX), CB_ADDSTRING, 0, (LPARAM)"Custom");
	SendMessage(GetDlgItem(hW, IDC_SLIDER00), TBM_SETRANGEMIN, 0, 0); // set trackbar stats
	SendMessage(GetDlgItem(hW, IDC_SLIDER00), TBM_SETRANGEMAX, 0, 100);
	SendMessage(GetDlgItem(hW, IDC_SLIDER01), TBM_SETRANGEMIN, 0, 0);
	SendMessage(GetDlgItem(hW, IDC_SLIDER01), TBM_SETRANGEMAX, 0, 5);
	SendMessage(GetDlgItem(hW, IDC_SLIDER02), TBM_SETRANGEMIN, 0, 0);
	SendMessage(GetDlgItem(hW, IDC_SLIDER02), TBM_SETRANGEMAX, 0, 18);
	SendMessage(GetDlgItem(hW, IDC_FOV), TBM_SETRANGEMIN, 0, 40);
	SendMessage(GetDlgItem(hW, IDC_FOV), TBM_SETRANGEMAX, 0, 120);
#ifdef SPEEDRUN_BUILD // replace info box with details about the speedrun build
	SetDlgItemText(hW, IDC_INFO, "The speedrun build removes the FOV zoom speed adjustment for GE. This is to ensure all players will have the same watch/weapon zoom speed regardless of their FOV setting.\n\nIt also removes Y axis pickup threshold adjustment for GE/PD so it's the original value (-45 degrees).");
#endif
}
//==========================================================================
// Purpose: refresh the interface and display current player's settings
//==========================================================================
static void GUI_Refresh(const HWND hW, const int revertbtn)
{
	// set radio buttons
	for(int radiobtn = PLAYER1; radiobtn < ALLPLAYERS; radiobtn++) // uncheck other player's radio buttons
		if(currentplayer != radiobtn)
			CheckDlgButton(hW, IDC_PLAYER1 + radiobtn, BST_UNCHECKED);
	// set config profile combobox
	SendMessage(GetDlgItem(hW, IDC_CONFIGBOX), CB_SETCURSEL, PROFILE[currentplayer].SETTINGS[CONFIG], 0); // set DISABLED/WASD/ESDF/CUSTOM config combobox
	// load buttons from current player's profile
	for(int button = 0; button < 16; button++) // load buttons from player struct and set input button statuses (setting to disabled/enabled)
	{
		SetDlgItemText(hW, IDC_PRIMARY00 + button, GetKeyName(PROFILE[currentplayer].BUTTONPRIM[button])); // get key
		EnableWindow(GetDlgItem(hW, IDC_PRIMARY00 + button), PROFILE[currentplayer].SETTINGS[CONFIG] != DISABLED); // set status
		SetDlgItemText(hW, IDC_SECONDARY00 + button, GetKeyName(PROFILE[currentplayer].BUTTONSEC[button]));
		EnableWindow(GetDlgItem(hW, IDC_SECONDARY00 + button), PROFILE[currentplayer].SETTINGS[CONFIG] != DISABLED);
	}
	// set keyboard/mouse to id stored in player's profile (only if they are valid)
	if(ONLY1PLAYERACTIVE) // we accept all device input if there is only one player active, so disable all device related comboboxes/button
	{
		ShowWindow(GetDlgItem(hW, IDC_DETECTDEVICE), 0); // hide detect devices button
		EnableWindow(GetDlgItem(hW, IDC_DETECTDEVICE), 0); // disable detect devices button
		EnableWindow(GetDlgItem(hW, IDC_MOUSESELECT), 0); // disable select combobox
		EnableWindow(GetDlgItem(hW, IDC_KEYBOARDSELECT), 0); // disable select combobox
	}
	else // multiple players are active
	{
		ShowWindow(GetDlgItem(hW, IDC_DETECTDEVICE), PROFILE[currentplayer].SETTINGS[CONFIG] != DISABLED && DEV_Init() > 2); // if profile is active and multiple devices connected, show detect devices button
		EnableWindow(GetDlgItem(hW, IDC_DETECTDEVICE), PROFILE[currentplayer].SETTINGS[CONFIG] != DISABLED && DEV_Init() > 2); // if profile is active and multiple devices connected, enable detect devices button
		EnableWindow(GetDlgItem(hW, IDC_MOUSESELECT), PROFILE[currentplayer].SETTINGS[CONFIG] != DISABLED && DEV_Init() > 2); // if profile is active and multiple devices connected, enable mouse select combobox
		EnableWindow(GetDlgItem(hW, IDC_KEYBOARDSELECT), PROFILE[currentplayer].SETTINGS[CONFIG] != DISABLED && DEV_Init() > 2); // if profile is active and multiple devices connected, enable keyboard select combobox
	}
	SendMessage(GetDlgItem(hW, IDC_MOUSESELECT), CB_SETCURSEL, DEV_TypeIndex(PROFILE[currentplayer].SETTINGS[MOUSE]), 0); // set mouse to saved device id from settings
	SendMessage(GetDlgItem(hW, IDC_KEYBOARDSELECT), CB_SETCURSEL, DEV_TypeIndex(PROFILE[currentplayer].SETTINGS[KEYBOARD]), 0); // set keyboard to saved device id from settings
	// set slider positions
	SendMessage(GetDlgItem(hW, IDC_SLIDER00), TBM_SETPOS, 1, PROFILE[currentplayer].SETTINGS[SENSITIVITY]);
	SendMessage(GetDlgItem(hW, IDC_SLIDER01), TBM_SETPOS, 1, PROFILE[currentplayer].SETTINGS[ACCELERATION]);
	SendMessage(GetDlgItem(hW, IDC_SLIDER02), TBM_SETPOS, 1, PROFILE[currentplayer].SETTINGS[CROSSHAIR]);
	SendMessage(GetDlgItem(hW, IDC_FOV), TBM_SETPOS, 1, overridefov); // set pos for fov trackbar
	// set slider labels
	char label[64];
	if(PROFILE[currentplayer].SETTINGS[SENSITIVITY]) // set percentage for sensitivity
		sprintf(label, "%d%%", PROFILE[currentplayer].SETTINGS[SENSITIVITY] * 5); // set percentage for sensitivity
	else
		sprintf(label, "None"); // replace 0% with none
	SetDlgItemText(hW, IDC_SLIDER_DISPLAY00, label);
	if(PROFILE[currentplayer].SETTINGS[ACCELERATION]) // set mouse acceleration
		sprintf(label, "%dx", PROFILE[currentplayer].SETTINGS[ACCELERATION]);
	else
		sprintf(label, "None"); // replace 0x with none
	SetDlgItemText(hW, IDC_SLIDER_DISPLAY01, label);
	if(PROFILE[currentplayer].SETTINGS[CROSSHAIR]) // set percentage for crosshair movement
		sprintf(label, "%d%%", PROFILE[currentplayer].SETTINGS[CROSSHAIR] * 100 / 6);
	else
		sprintf(label, "Locked"); // replace 0% with locked
	SetDlgItemText(hW, IDC_SLIDER_DISPLAY02, label);
	// set fov label
	if(stopthread) // if game isn't running
	{
		if(overridefov < 60)
			SetDlgItemText(hW, IDC_FOV_NOTE, "Below Default");
		else if(overridefov == 60)
			SetDlgItemText(hW, IDC_FOV_NOTE, "Default FOV");
		else if(overridefov <= 80)
			SetDlgItemText(hW, IDC_FOV_NOTE, "Above Default");
		else if(overridefov <= 90)
			SetDlgItemText(hW, IDC_FOV_NOTE, "Breaks ViewModels");
		else
			SetDlgItemText(hW, IDC_FOV_NOTE, "Breaks ViewModels\\LOD");
	}
	else
		SetDlgItemText(hW, IDC_FOV_NOTE, "Locked - Stop to Edit"); // fov can only be set at boot, tell user to stop emulating if they want to change fov
	// calculate and set fov (ge/pd format is vertical fov, convert to hor fov)
	const double fovtorad = (double)overridefov * (3.1415 / 180.f);
	const double setfov = 2.f * atan((tan(fovtorad / 2.f) / (0.75))) * (180.f / 3.1415);
	const double aspect = changeratio ? 4.f / 3.f : 16.f / 9.f;
	const double hfov = 2.f * atan((tan(setfov / 2.f * (3.1415 / 180.f))) * (aspect * 0.75)) * (180.f / 3.1415);
	sprintf(label, "Vertical FOV:  %d (Hor %d)", overridefov, (int)hfov); // set degrees for fov
	SetDlgItemText(hW, IDC_FOV_DEGREES, label);
	// set checkboxes from current player's profile (invert aiming, crouch toggle, ge cursor aiming, pd radial navigation)
	for(int index = 0; index < 4; index++) // set checkbox from player struct
		SendMessage(GetDlgItem(hW, index + IDC_INVERTPITCH), BM_SETCHECK, PROFILE[currentplayer].SETTINGS[index + INVERTPITCH] ? BST_CHECKED : BST_UNCHECKED, 0);
	// disable/enable sensitivity and checkboxes according to player status
	for(int trackbar = IDC_SLIDER00; trackbar <= IDC_PDCURSORAIMING; trackbar++) // set trackbar and checkbox statuses
		EnableWindow(GetDlgItem(hW, trackbar), PROFILE[currentplayer].SETTINGS[CONFIG] != DISABLED);
	// enable/disable fov adjustment when game is running
	EnableWindow(GetDlgItem(hW, IDC_RESETFOV), stopthread && overridefov != 60); // disable/enable fov reset button depending if fov is default or not and if game isn't running
	for(int fovbuttons = IDC_FOV_DEGREES; fovbuttons <= IDC_GESHOWCROSSHAIR; fovbuttons++)
		EnableWindow(GetDlgItem(hW, fovbuttons), stopthread); // if stopthread is 0 it means game is running
	SendMessage(GetDlgItem(hW, IDC_GESHOWCROSSHAIR), BM_SETCHECK, geshowcrosshair ? BST_CHECKED : BST_UNCHECKED, 0); // set checkbox for show crosshair
	// revert button
	if(revertbtn != 2) // 2 is ignore flag
		EnableWindow(GetDlgItem(hW, IDC_REVERT), revertbtn); // set revert button status
	// enable/disable clear button depending if buttons have been cleared
	int allbuttonchecksum = 0;
	for(int buttonindex = 0; buttonindex < 16; buttonindex++) // add button sum to allbuttonchecksum (used to check if clear button should be enabled)
	{
		allbuttonchecksum += PROFILE[currentplayer].BUTTONPRIM[buttonindex];
		allbuttonchecksum += PROFILE[currentplayer].BUTTONSEC[buttonindex];
	}
	EnableWindow(GetDlgItem(hW, IDC_CLEAR), allbuttonchecksum > 0); // set clear button status
	SetDlgItemText(hW, IDC_LOCK, GetKeyName(mousetogglekey)); // set mouse toggle text
	SendMessage(GetDlgItem(hW, IDC_LOCKONFOCUS), BM_SETCHECK, mouselockonfocus ? BST_CHECKED : BST_UNCHECKED, 0); // set mouse lock checkbox
	SendMessage(GetDlgItem(hW, IDC_UNLOCKONWINLOSS), BM_SETCHECK, mouseunlockonloss ? BST_CHECKED : BST_UNCHECKED, 0); // set mouse unlock checkbox
}
//==========================================================================
// Purpose: process input binding
// Changed Globals: lastinputbutton, PROFILE.BUTTONPRIM, PROFILE.BUTTONSEC, mousetogglekey
//==========================================================================
static void GUI_ProcessKey(const HWND hW, const int buttonid, const int primflag)
{
	if(lastinputbutton == buttonid) // button pressed twice (usually by accident or spacebar)
	{
		lastinputbutton = 0;
		return;
	}
	lastinputbutton = buttonid;
	PROFILE[currentplayer].SETTINGS[CONFIG] = CUSTOM;
	SendMessage(GetDlgItem(hW, IDC_CONFIGBOX), CB_SETCURSEL, CUSTOM, 0);
	if(primflag != 2) // don't enable revert if pressed mouse toggle button
		EnableWindow(GetDlgItem(hW, IDC_REVERT), 1);
	SetDlgItemText(hW, buttonid, "...");
	int key = 0, tick = 0;
	while(!key) // search for first key press
	{
		Sleep(40); // don't repeat this loop too quickly
		tick++;
		if(tick > 3) // wait 3 ticks before accepting input
			key = DEV_ReturnKey();
		else
			DEV_ReturnKey(); // flush input
		if(tick == 10)
			SetDlgItemText(hW, buttonid, "..5..");
		else if(tick == 35)
			SetDlgItemText(hW, buttonid, "..4..");
		else if(tick == 60)
			SetDlgItemText(hW, buttonid, "..3..");
		else if(tick == 85)
			SetDlgItemText(hW, buttonid, "..2..");
		else if(tick == 110)
			SetDlgItemText(hW, buttonid, "..1..");
		if(tick >= 135 || key == VK_ESCAPE || primflag == 2 && (key >= VK_LBUTTON && key <= VK_XBUTTON2 || key == VK_WHEELUP || key == VK_WHEELDOWN || key == VK_WHEELRIGHT || key == VK_WHEELLEFT)) // user didn't enter anything in or pressed VK_ESCAPE, or set mouse toggle button to a mouse button
		{
			key = primflag < 2 ? 0x00 : 0x34; // if regular input button set to none, if mouse toggle button set to default key (4)
			lastinputbutton = 0;
			break;
		}
	}
	SetDlgItemText(hW, buttonid, GetKeyName(key));
	if(primflag == 0)
		PROFILE[currentplayer].BUTTONPRIM[buttonid - IDC_PRIMARY00] = key;
	else if(primflag == 1)
		PROFILE[currentplayer].BUTTONSEC[buttonid - IDC_SECONDARY00] = key;
	else
		mousetogglekey = key;
	int allbuttonchecksum = 0; // enable/disable clear button depending if buttons have been cleared
	for(int buttonindex = 0; buttonindex < 16; buttonindex++) // add button sum to allbuttonchecksum (used to check if clear button should be enabled)
	{
		allbuttonchecksum += PROFILE[currentplayer].BUTTONPRIM[buttonindex];
		allbuttonchecksum += PROFILE[currentplayer].BUTTONSEC[buttonindex];
	}
	EnableWindow(GetDlgItem(hW, IDC_CLEAR), allbuttonchecksum > 0); // set clear button status
}
//==========================================================================
// Purpose: detect keyboard and mice for profile
// Changed Globals: lastinputbutton, PROFILE.SETTINGS
//==========================================================================
static void GUI_DetectDevice(const HWND hW, const int buttonid)
{
	if(lastinputbutton == buttonid) // button pressed twice (usually by accident or spacebar)
	{
		lastinputbutton = 0;
		return;
	}
	lastinputbutton = buttonid;
	EnableWindow(GetDlgItem(hW, buttonid), 0); // disable button while detecting devices
	int kb = -1, ms = -1, tick = 0;
	while(ms == -1) // search for mouse
	{
		Sleep(30); // don't repeat this loop too quickly
		tick++;
		if(tick > 5) // wait 5 ticks before accepting device id
			ms = DEV_ReturnDeviceID(MOUSETYPE);
		else
			DEV_ReturnDeviceID(KEYBOARDTYPE); // flush keyboard input
		if(tick == 10)
			SetDlgItemText(hW, buttonid, "..Click Mouse..5..");
		else if(tick == 35)
			SetDlgItemText(hW, buttonid, "..Click Mouse..4..");
		else if(tick == 60)
			SetDlgItemText(hW, buttonid, "..Click Mouse..3..");
		else if(tick == 85)
			SetDlgItemText(hW, buttonid, "..Click Mouse..2..");
		else if(tick == 110)
			SetDlgItemText(hW, buttonid, "..Click Mouse..1..");
		else if(tick == 135) // didn't detect mouse
			break;
	}
	tick = 0;
	while(kb == -1) // search for keyboard
	{
		Sleep(30); // don't repeat this loop too quickly
		tick++;
		if(tick > 5) // wait 5 ticks before accepting device id
			kb = DEV_ReturnDeviceID(KEYBOARDTYPE);
		else
			DEV_ReturnDeviceID(MOUSETYPE); // flush mouse input
		if(tick == 10)
			SetDlgItemText(hW, buttonid, "..Press Any Key..5.."); // we're assuming the user knows where the any key is...
		else if(tick == 35)
			SetDlgItemText(hW, buttonid, "..Press Any Key..4..");
		else if(tick == 60)
			SetDlgItemText(hW, buttonid, "..Press Any Key..3..");
		else if(tick == 85)
			SetDlgItemText(hW, buttonid, "..Press Any Key..2..");
		else if(tick == 110)
			SetDlgItemText(hW, buttonid, "..Press Any Key..1..");
		else if(tick == 135) // didn't detect keyboard
			break;
	}
	SetDlgItemText(hW, buttonid, "Detect Input Devices");
	EnableWindow(GetDlgItem(hW, buttonid), 1); // enable detect devices button
	EnableWindow(GetDlgItem(hW, IDC_REVERT), 1); // set revert button status to true
	if(kb == -1 && ms == -1)
		return;
	if(ms != -1)
		PROFILE[currentplayer].SETTINGS[MOUSE] = ms;
	if(kb != -1)
		PROFILE[currentplayer].SETTINGS[KEYBOARD] = kb;
	SendMessage(GetDlgItem(hW, IDC_MOUSESELECT), CB_SETCURSEL, DEV_TypeIndex(PROFILE[currentplayer].SETTINGS[MOUSE]), 0); // set mouse combobox to new device
	SendMessage(GetDlgItem(hW, IDC_KEYBOARDSELECT), CB_SETCURSEL, DEV_TypeIndex(PROFILE[currentplayer].SETTINGS[KEYBOARD]), 0); // set keyboard combobox to new device
}
//==========================================================================
// Purpose: load profile settings (i'm really sorry about this mess)
// Changed Globals: PROFILE, overridefov, geshowcrosshair, mouselockonfocus, mouseunlockonloss, mousetogglekey
//==========================================================================
static void INI_Load(const HWND hW, const int loadplayer)
{
	#define PRIMBTNBLKSIZE 4 * 16 // 4 PLAYERS * BUTTONPRIM
	#define BUTTONBLKSIZE 4 * (16 + 16) // 4 PLAYERS * (BUTTONPRIM + BUTTONSEC)
	#define SETTINGBLKSIZE 4 * 10 // 4 PLAYERS * SETTINGS
	#define TOTALLINES BUTTONBLKSIZE + SETTINGBLKSIZE + 5 // profile struct[all players] + overridefov + geshowcrosshair + mouselockonfocus + mouseunlockonloss + mousetogglekey
	FILE *fileptr; // file pointer for mouseinjector.ini
	if((fileptr = fopen(inifilepathdefault, "r")) == NULL) // if INI file was not found
		fileptr = _wfopen(inifilepath, L"r"); // reattempt to load INI file using wide character filepath
	if(fileptr != NULL) // if INI file was found
	{
		char line[256][5]; // char array used for file to write to
		char lines[5]; // maximum lines read size
		int counter = 0; // used to assign each line to a array
		while(fgets(lines, sizeof(lines), fileptr) != NULL && counter < 256) // read the first 256 lines
		{
			strcpy(line[counter], lines); // read file lines and assign value to line array
			counter++; // add 1 to counter, so the next line can be read
		}
		fclose(fileptr); // close the file stream
		if(counter == TOTALLINES) // check mouseinjector.ini if it has the correct new lines
		{
			const int safesettings[2][10] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {3, 100, 5, 18, 1, 1, 1, 1, 16, 16}}; // safe min/max values
			int everythingisfine = 1; // for now...
			for(int player = PLAYER1; player < ALLPLAYERS; player++) // load settings block first because if using WASD/ESDF config don't bother loading custom keys (settings are stored at end of file)
			{
				for(int index = 0; index < 10; index++)
					if(everythingisfine && atoi(line[BUTTONBLKSIZE + (player * 10) + index]) >= safesettings[0][index] && atoi(line[BUTTONBLKSIZE + (player * 10) + index]) <= safesettings[1][index]) // if everything is fine
					{
						if(loadplayer == ALLPLAYERS || player == loadplayer) // load everything if given ALLPLAYERS flag or filter loading to current player
							PROFILE[player].SETTINGS[index] = atoi(line[BUTTONBLKSIZE + (player * 10) + index]);
					}
					else // invalid settings, abort (this isn't fine)
						everythingisfine = 0;
			}
			if(everythingisfine) // if settings block is OK
			{
				if(loadplayer == ALLPLAYERS) // only load if given ALLPLAYERS flag
				{
					overridefov = ClampInt(atoi(line[TOTALLINES - 5]), 40, 120); // load overridefov
					geshowcrosshair = !(!atoi(line[TOTALLINES - 4])); // load geshowcrosshair
					mouselockonfocus = !(!atoi(line[TOTALLINES - 3])); // load mouselockonfocus
					mouseunlockonloss = !(!atoi(line[TOTALLINES - 2])); // load mouseunlockonloss
					mousetogglekey = ClampInt(atoi(line[TOTALLINES - 1]), 0x00, 0xFF); // load mousetogglekey
					if(!mousetogglekey || mousetogglekey == 0xFF || mousetogglekey == VK_ESCAPE || mousetogglekey >= VK_LBUTTON && mousetogglekey <= VK_XBUTTON2 || mousetogglekey == VK_WHEELUP || mousetogglekey == VK_WHEELDOWN || mousetogglekey == VK_WHEELRIGHT || mousetogglekey == VK_WHEELLEFT) // if mousetogglekey is set to none/escape/mouse button, reset to default key
						mousetogglekey = 0x34;
				}
				for(int player = PLAYER1; player < ALLPLAYERS; player++)
				{
					if(loadplayer == ALLPLAYERS || player == loadplayer) // load everything if given ALLPLAYERS flag or filter loading to current player
					{
						if(PROFILE[player].SETTINGS[CONFIG] == DISABLED || PROFILE[player].SETTINGS[CONFIG] == CUSTOM) // only load keys if profile is disabled/custom, else skip
						{
							for(int button = 0; button < 16; button++)
							{
								PROFILE[player].BUTTONPRIM[button] = ClampInt(atoi(line[player * 16 + button]), 0x00, 0xFF);
								PROFILE[player].BUTTONSEC[button] = ClampInt(atoi(line[PRIMBTNBLKSIZE + (player * 16) + button]), 0x00, 0xFF);
								if(PROFILE[player].BUTTONPRIM[button] == VK_ESCAPE || PROFILE[player].BUTTONPRIM[button] == 0xFF) // set to none if escape/0xFF (escape can't be used for keys)
									PROFILE[player].BUTTONPRIM[button] = 0;
								if(PROFILE[player].BUTTONSEC[button] == VK_ESCAPE || PROFILE[player].BUTTONSEC[button] == 0xFF)
									PROFILE[player].BUTTONSEC[button] = 0;
							}
						}
						else
							INI_SetConfig(player, PROFILE[player].SETTINGS[CONFIG]); // player is not using custom config, assign keys from function
						if(DEV_Name(PROFILE[player].SETTINGS[MOUSE]) == NULL || DEV_Type(PROFILE[player].SETTINGS[MOUSE]) == KEYBOARDTYPE) // device not connected or id is used by a keyboard
							PROFILE[player].SETTINGS[MOUSE] = defaultmouse;
						if(DEV_Name(PROFILE[player].SETTINGS[KEYBOARD]) == NULL || DEV_Type(PROFILE[player].SETTINGS[KEYBOARD]) == MOUSETYPE) // device not connected or id is used by a mouse
							PROFILE[player].SETTINGS[KEYBOARD] = defaultkeyboard;
					}
				}
				return; // we're done
			}
		}
		MessageBox(hW, "Loading mouseinjector.ini failed!\n\nInvalid settings detected, resetting to default...", "Mouse Injector - Error", MB_ICONERROR | MB_OK); // tell the user loading mouseinjector.ini failed
	}
	else
		MessageBox(hW, "Loading mouseinjector.ini failed!\n\nCould not find mouseinjector.ini file, creating mouseinjector.ini...", "Mouse Injector - Error", MB_ICONERROR | MB_OK); // tell the user loading mouseinjector.ini failed
	INI_Reset(ALLPLAYERS);
	INI_SetConfig(PLAYER1, WASD);
	INI_Save(hW); // create/overwrite mouseinjector.ini with default values
}
//==========================================================================
// Purpose: save profile settings
//==========================================================================
static void INI_Save(const HWND hW)
{
	FILE *fileptr; // create a file pointer and open mouseinjector.ini from same dir as our plugin
	if((fileptr = fopen(inifilepathdefault, "w")) == NULL) // if INI file was not found
		fileptr = _wfopen(inifilepath, L"w"); // reattempt to write INI file using wide character filepath
	if(fileptr != NULL) // if INI file was found
	{
		for(int player = PLAYER1; player < ALLPLAYERS; player++)
			for(int button = 0; button < 16; button++)
				fprintf(fileptr, "%d\n", ClampInt(PROFILE[player].BUTTONPRIM[button], 0x00, 0xFF)); // sanitize save
		for(int player = PLAYER1; player < ALLPLAYERS; player++)
			for(int button = 0; button < 16; button++)
				fprintf(fileptr, "%d\n", ClampInt(PROFILE[player].BUTTONSEC[button], 0x00, 0xFF));
		for(int player = PLAYER1; player < ALLPLAYERS; player++)
			for(int index = 0; index < 10; index++)
				fprintf(fileptr, "%d\n", ClampInt(PROFILE[player].SETTINGS[index], 0, 100));
		fprintf(fileptr, "%d\n%d\n%d\n%d\n%d", overridefov, geshowcrosshair, mouselockonfocus, mouseunlockonloss, mousetogglekey);
		fclose(fileptr); // close the file stream
	}
	else // if saving file failed (could be set to read only, antivirus is preventing file writing or filepath is invalid)
		MessageBox(hW, "Saving mouseinjector.ini failed!\n\nCould not write mouseinjector.ini file...", "Mouse Injector - Error", MB_ICONERROR | MB_OK); // tell the user saving mouseinjector.ini failed
}
//==========================================================================
// Purpose: reset a player struct or all players
// Changed Globals: PROFILE, overridefov, geshowcrosshair, mouselockonfocus, mouseunlockonloss, mousetogglekey, lastinputbutton
//==========================================================================
static void INI_Reset(const int playerflag)
{
	const int defaultsetting[10] = {DISABLED, 20, 0, 3, 0, 0, 1, 1, defaultmouse, defaultkeyboard};
	if(playerflag == ALLPLAYERS)
	{
		for(int player = PLAYER1; player < ALLPLAYERS; player++)
		{
			for(int buttons = 0; buttons < 16; buttons++)
			{
				PROFILE[player].BUTTONPRIM[buttons] = 0;
				PROFILE[player].BUTTONSEC[buttons] = 0;
			}
			for(int index = 0; index < 10; index++)
				PROFILE[player].SETTINGS[index] = defaultsetting[index];
		}
		overridefov = 60, geshowcrosshair = 0, mouselockonfocus = 0, mouseunlockonloss = 1, mousetogglekey = 0x34;
	}
	else
	{
		for(int buttons = 0; buttons < 16; buttons++)
		{
			PROFILE[playerflag].BUTTONPRIM[buttons] = 0;
			PROFILE[playerflag].BUTTONSEC[buttons] = 0;
		}
		if(PROFILE[playerflag].SETTINGS[CONFIG] != DISABLED) // hitting clear when current player's profile is active (WASD/ESDF/CUSTOM) will set config to custom
			PROFILE[playerflag].SETTINGS[CONFIG] = CUSTOM;
	}
	lastinputbutton = 0;
}
//==========================================================================
// Purpose: set a player's config to WASD/ESDF (does not support ALLPLAYERS)
// Changed Globals: PROFILE, lastinputbutton
//==========================================================================
static void INI_SetConfig(const int playerflag, const int config)
{
	const int defaultbuttons[2][16] = {{87, 83, 65, 68, 1, 2, 81, 69, 13, 17, 10, 11, 38, 40, 37, 39}, {69, 68, 83, 70, 1, 2, 87, 82, 13, 65, 10, 11, 38, 40, 37, 39}}; // WASD/ESDF
	for(int buttons = 0; buttons < 16; buttons++)
	{
		PROFILE[playerflag].BUTTONPRIM[buttons] = defaultbuttons[config - 1][buttons];
		PROFILE[playerflag].BUTTONSEC[buttons] = 0;
	}
	PROFILE[playerflag].SETTINGS[CONFIG] = config;
	lastinputbutton = 0;
}
//==========================================================================
// Purpose: set the controller status
// Changed Globals: ctrlptr
//==========================================================================
static void UpdateControllerStatus(void)
{
	if(ctrlptr == NULL)
		return;
	for(int player = PLAYER1; player < ALLPLAYERS; player++)
	{
		ctrlptr[player].Present = PROFILE[player].SETTINGS[CONFIG] == DISABLED ? FALSE : TRUE;
		ctrlptr[player].RawData = FALSE;
		ctrlptr[player].Plugin = player == PLAYER1 ? PLUGIN_MEMPAK : PLUGIN_NONE; // set player 1's mempak to present and disable other players
	}
}
//==========================================================================
// Purpose: called when the emulator is closing down allowing the DLL to de-initialise
//==========================================================================
DLLEXPORT void CALL CloseDLL(void)
{
	End();
}
//==========================================================================
// Purpose: To process the raw data that has just been sent to a specific controller
// Input: Controller Number (0 to 3) and -1 signaling end of processing the pif ram. Pointer of data to be processed.
// Note: This function is only needed if the DLL is allowing raw data
// The data that is being processed looks like this
// initialize controller: 01 03 00 FF FF FF
// read controller:       01 04 01 FF FF FF FF
//==========================================================================
DLLEXPORT void CALL ControllerCommand(int Control, BYTE *Command)
{
	return;
}
//==========================================================================
// Purpose: Optional function that is provided to give further information about the DLL
// Input: A handle to the window that calls this function
//==========================================================================
DLLEXPORT void CALL DllAbout(HWND hParent)
{
	MessageBox(hParent, "Mouse Injector for GE/PD "__MOUSE_INECTOR_VERSION__" (Build: "__DATE__")\nCopyright (C) "__CURRENTYEAR__", Carnivorous", "Mouse Injector - About", MB_ICONINFORMATION | MB_OK);
}
//==========================================================================
// Purpose: Optional function that is provided to allow the user to configure the DLL
// Input: A handle to the window that calls this function
// Changed Globals: currentlyconfiguring, mousetoggle, lastinputbutton, windowactive
//==========================================================================
DLLEXPORT void CALL DllConfig(HWND hParent)
{
	if(Init(hParent))
	{
		int laststate = mousetoggle;
		currentlyconfiguring = 1, mousetoggle = 0, lastinputbutton = 0;
		DialogBox(hInst, MAKEINTRESOURCE(IDC_CONFIGWINDOW), hParent, (DLGPROC)GUI_Config);
		UpdateControllerStatus();
		currentlyconfiguring = 0, mousetoggle = laststate, windowactive = 1;
	}
	else
		MessageBox(hParent, "Mouse Injector could not find Mouse and Keyboard\n\nPlease connect devices and restart Emulator..." , "Mouse Injector - Error", MB_ICONERROR | MB_OK);
}
//==========================================================================
// Purpose: Optional function that is provided to allow the user to test the DLL
// input: A handle to the window that calls this function
//==========================================================================
DLLEXPORT void CALL DllTest(HWND hParent)
{
	MessageBox(hParent, DEV_Init() ? "Mouse Injector detects Mouse and Keyboard" : "Mouse Injector could not find Mouse and Keyboard", "Mouse Injector - Testing", MB_ICONINFORMATION | MB_OK);
}
//==========================================================================
// Purpose: Allows the emulator to gather information about the DLL by filling in the PluginInfo structure
// Input: A pointer to a PLUGIN_INFO structure that needs to be filled by the function (see def above)
//==========================================================================
DLLEXPORT void CALL GetDllInfo(PLUGIN_INFO *PluginInfo)
{
	PluginInfo->Version = 0xFBAD; // no emulator supports this other than my disgusting version of 1964 (awful hack that i created because plugins are not complicated enough and i don't know what the f**k i am doing as evident from the code i've written)
	PluginInfo->Type = PLUGIN_TYPE_CONTROLLER;
	sprintf(PluginInfo->Name, "Mouse Injector for GE/PD "__MOUSE_INECTOR_VERSION__"");
#ifdef SPEEDRUN_BUILD
	sprintf(PluginInfo->Name, "%s (Speedrun Build)", PluginInfo->Name);
#endif
}
//==========================================================================
// Purpose: Get the current state of the controllers buttons
// Input: Controller Number (0 to 3) - A pointer to a BUTTONS structure to be filled with the controller state
//==========================================================================
DLLEXPORT void CALL GetKeys(int Control, BUTTONS *Keys)
{
	if(Keys == NULL)
		return;
	Keys->Value = !currentlyconfiguring ? CONTROLLER[Control].Value : 0; // only send input if user is not currently configuring plugin
}
//==========================================================================
// Purpose: Initializes how each of the controllers should be handled
// Input: The handle to the main window - A controller structure that needs to be filled for the emulator to know how to handle each controller
// Changed Globals: ctrlptr, mousetoggle, PROFILE.SETTINGS
//==========================================================================
DLLEXPORT void CALL InitiateControllers(HWND hMainWindow, CONTROL Controls[4])
{
	ctrlptr = Controls;
	mousetoggle = 0;
	if(!Init(hMainWindow)) // mouse & keyboard isn't detected, disable controllers
	{
		for(int player = PLAYER1; player < ALLPLAYERS; player++)
			PROFILE[player].SETTINGS[CONFIG] = DISABLED;
		UpdateControllerStatus(); // set controls to disabled
		MessageBox(hMainWindow, "Mouse Injector could not find Mouse and Keyboard\n\nPlease connect devices and restart Emulator..." , "Mouse Injector - Error", MB_ICONERROR | MB_OK);
	}
}
//==========================================================================
// Purpose: Initializes how each of the controllers should be handled
// Input: Controller Number (0 to 3) and -1 signaling end of processing the pif ram - Pointer of data to be processed
// Note: This function is only needed if the DLL is allowing raw data
//==========================================================================
DLLEXPORT void CALL ReadController(int Control, BYTE *Command)
{
	return;
}
//==========================================================================
// Purpose: Called when a ROM is closed
// Changed Globals: mousetoggle
//==========================================================================
DLLEXPORT void CALL RomClosed(void)
{
	mousetoggle = 0;
	StopInjection();
	GAME_Quit();
}
//==========================================================================
// Purpose: Called when a ROM is open (from the emulation thread)
// Changed Globals: emulatorwindow
//==========================================================================
DLLEXPORT void CALL RomOpen(void)
{
	emulatorwindow = GetForegroundWindow();
	StartInjection();
}
//==========================================================================
// Purpose: To pass the WM_KeyDown message from the emulator to the plugin
// Input: wParam and lParam of the WM_KEYDOWN message
// Changed Globals: emulatorwindow, windowactive
//==========================================================================
DLLEXPORT void CALL WM_KeyDown(WPARAM wParam, LPARAM lParam)
{
	if(!windowactive) // update emulatorwindow if windowactive is disabled (prevent rare case where foreground window is incorrectly set eg: a different program window was focused when 1964 finishes loading a ROM)
		emulatorwindow = GetForegroundWindow();
	windowactive = 1; // emulator window is active on key press
}
//==========================================================================
// Purpose: To pass the WM_KEYUP message from the emulator to the plugin
// Input: wParam and lParam of the WM_KeyUp message
//==========================================================================
DLLEXPORT void CALL WM_KeyUp(WPARAM wParam, LPARAM lParam)
{
	return;
}
//==========================================================================
// Purpose: Give rdram pointer to the plugin (called every second)
// Input: pointer to emulator's rdram and overclock factor
// Changed Globals: rdramptr, emuoverclock
//==========================================================================
DLLEXPORT void CALL HookRDRAM(DWORD *Mem, int OCFactor)
{
	rdramptr = (const unsigned char **)Mem;
	emuoverclock = OCFactor >= 3; // an overclock above 3 is guaranteed to be 60fps, so set to 0 if below 3 times overclock
	DRP_Update(); // init and update discord rich presence (discord will limit update rate to once every 15 seconds)
}
//==========================================================================
// Purpose: Give rom file pointer to the plugin on boot (for patching fov)
// Input: pointer to emulator's loaded rom
// Changed Globals: romptr
//==========================================================================
DLLEXPORT void CALL HookROM(DWORD *Rom)
{
	romptr = (const unsigned char **)Rom;
}