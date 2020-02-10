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
#define TICKRATE (emuoverclock ? 2 : 4)
#define TIMESTEP TICKRATE / 1000

extern const unsigned char **rdramptr;
extern const unsigned char **romptr;
extern int stopthread;
extern int mousetogglekey;
extern int mousetoggle;
extern int mouselockonfocus;
extern int mouseunlockonloss;
extern HWND emulatorwindow;
extern int emuoverclock;
extern int overridefov;
extern int geshowcrosshair;