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
extern int windowactive;

extern int DEV_Init(void);
extern void DEV_Quit(void);
extern DWORD WINAPI DEV_InjectThread();
extern int DEV_ReturnKey(void);
extern int DEV_ReturnDeviceID(const int devicetype);
extern const char *DEV_Name(const int id);
extern int DEV_Type(const int id);
extern int DEV_TypeIndex(const int id);
extern int DEV_TypeID(const int id, const int devicetype);