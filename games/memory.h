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
#define LOAD_SHALF_PARAM(param) *(short *) ((unsigned char *)rdramptr[(((param) >> 12))] + (((param) & 0x00000FFF) ^ 2))
#define LOAD_SWORD_PARAM(param) *(int *)((unsigned char *)rdramptr[(((param) >> 12))] + (((param) & 0x00000FFF)))
#define LOAD_UWORD_PARAM(param) *(unsigned int *)((unsigned char *)rdramptr[(((param) >> 12))] + (((param) & 0x00000FFF)))

#define WITHINRANGE(param) ((rdramptr != 0) && ((param & 0xFF800000U) == 0x80000000U))
#define GARBAGESHORT (short)0xABAD
#define GARBAGEINT (int)0xABADC0DE

//==========================================================================
// Purpose: read short from memory
// Parameter: address
//==========================================================================
static inline short EMU_ReadShort(const unsigned int addr)
{
	if(WITHINRANGE(addr))
		return LOAD_SHALF_PARAM(addr);
	return GARBAGESHORT;
}
//==========================================================================
// Purpose: write short to memory location
// Parameter: address, value
//==========================================================================
static inline void EMU_WriteShort(const unsigned int addr, const short value)
{
	if(WITHINRANGE(addr))
		LOAD_SHALF_PARAM(addr) = value;
}
//==========================================================================
// Purpose: read int from memory
// Parameter: address
//==========================================================================
static inline int EMU_ReadInt(const unsigned int addr)
{
	if(WITHINRANGE(addr))
		return LOAD_SWORD_PARAM(addr);
	return GARBAGEINT;
}
//==========================================================================
// Purpose: write int to memory location
// Parameter: address, value
//==========================================================================
static inline void EMU_WriteInt(const unsigned int addr, const int value)
{
	if(WITHINRANGE(addr))
		LOAD_SWORD_PARAM(addr) = value;
}
//==========================================================================
// Purpose: read float from memory
// Parameter: address
//==========================================================================
static inline float EMU_ReadFloat(const unsigned int addr)
{
	if(WITHINRANGE(addr))
		return *((float *)&LOAD_UWORD_PARAM(addr));
	return GARBAGEINT;
}
//==========================================================================
// Purpose: write float to memory location
// Parameter: address, value
//==========================================================================
static inline void EMU_WriteFloat(const unsigned int addr, const float value)
{
	if(WITHINRANGE(addr))
		LOAD_UWORD_PARAM(addr) = *(unsigned int *)(&value);
}
//==========================================================================
// Purpose: read int from emulator's copy of rom
// Parameter: address
//==========================================================================
static inline unsigned EMU_ReadROM(const unsigned int addr)
{
	return (unsigned)romptr[addr / 0x4];
}
//==========================================================================
// Purpose: write to emulator's copy of rom
// Parameter: address, value
//==========================================================================
static inline void EMU_WriteROM(const unsigned int addr, const unsigned int value)
{
	if(romptr != 0)
		romptr[addr / 0x4] = (void *)value;
}