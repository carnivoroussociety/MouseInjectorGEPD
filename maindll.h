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
#define TICKRATE_OC 2 // 2ms (500 Hz) for overclocked
#define TICKRATE_STOCK 4 // 4ms (250 Hz) for stock speed
#define TICKRATE (emuoverclock ? TICKRATE_OC : TICKRATE_STOCK)
#define TIMESTEP TICKRATE / 1000
#ifdef SPEEDRUN_BUILD // fov/ratio hacks gives unfair advantage, always use default values for speedrun build
#define RATIOFACTOR 1.f
#define OVERRIDEFOV 60
#else
#define RATIOFACTOR (((float)overrideratiowidth / (float)overrideratioheight) / (16.f / 9.f))
#define OVERRIDEFOV overridefov
#endif

extern BUTTONS CONTROLLER[4];
extern struct PROFILE_STRUCT PROFILE[4];
extern struct DEVICE_STRUCT DEVICE[4];

extern const unsigned char **rdramptr;
extern const unsigned char **romptr;
extern int stopthread;
extern int mousetogglekey;
extern int mousetoggle;
extern int mouselockonfocus;
extern int mouseunlockonloss;
extern int configdialogopen;
extern HWND emulatorwindow;
extern int emuoverclock;
extern int overridefov;
extern int overrideratiowidth, overrideratioheight;
extern int geshowcrosshair;
extern int bypassviewmodelfovtweak;