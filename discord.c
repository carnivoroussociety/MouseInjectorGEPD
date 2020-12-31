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
#ifdef ADD_DISCORD_PRESENCE
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "global.h"
#include "discord.h"
#include "maindll.h"
#include "games/game.h"
#include "games/memory.h"
#include "./discord/discord_rpc.h"

#define ROMCRC1 0x10
#define ROMCRC2 0x14
#define ROMNAME 0x28
// GOLDENEYE ADDRESSES
#define GE_stageid 0x8002A8F4
#define GE_difficulty 0x8002A8FC
#define GE_camera 0x80036494
#define GE_pause 0x80048370
#define GE_menupage 0x8002A8C0
#define GE_sptime 0x80079A20
#define GE_mptime 0x80048394
#define GE_gamemode 0x8002A8F0
#define GECRC1 0xDCBC50D1
#define GECRC2 0x09FD1AA3
// PERFECT DARK ADDRESSES
#define PD_stageid 0x800624E4
#define PD_difficulty 0x80084020
#define PD_camera 0x8009A26C
#define PD_pause 0x80084014
#define PD_mppause 0x800ACBA6
#define PD_joannadata 0x8009A024
#define PD_sptime 0x801BD21C - 0x801BB6A0
#define PD_mptime 0x80084024
#define PDCRC1 0x41F2B98F
#define PDCRC2 0xB458B466

static const char gestagenames[0x3A][0x0C] = {"", "", "", "", "", "", "", "", "", "Bunker I", "", "", "", "", "", "", "", "", "", "", "Silo", "", "Statue Park", "Control", "Archives", "Train", "Frigate", "Bunker II", "Aztec", "Streets", "Depot", "Complex", "Egypt", "Dam", "Facility", "Runway", "Surface I", "Jungle", "Temple", "Caverns", "Citadel", "Cradle", "", "Surface II", "", "Basement", "Stack", "", "Library", "", "Caves", "", "", "", "Credits", "", "", ""}; // stage names
static const char gedifficulty[0x04][0x0D] = {"Agent", "Secret Agent", "00 Agent", "007"}; // difficulty string
static const char pdstagenames[0x51][0x17] = {"", "", "", "", "", "", "", "", "", "Maian SOS", "", "", "", "", "", "", "", "", "", "", "", "", "WAR!", "Ravine", "", "A51 Escape", "", "", "Crash Site", "Chicago", "G5 Building", "Complex", "G5 Building", "Pelagic II", "dataDyne Extraction", "", "", "Temple", "Carrington Institute", "Air Base", "", "Pipes", "Skedar Ruins", "", "Carrington Villa", "Carrington Institute", "", "A51 Infiltration", "dataDyne Defection", "Air Force One", "Skedar", "dataDyne Investigation", "Attack Ship", "A51 Rescue", "", "Mr. Blonde's Revenge", "Deep Sea", "Base", "", "Area 52", "Warehouse", "Car Park", "", "", "", "Ruins", "Sewers", "Felicity", "Fortress", "Villa", "", "Grid", "", "", "", "", "", "", "", "Duel"}; // stage names
static const char pddifficulty[0x03][0x0E] = {"Agent", "Secret Agent", "Perfect Agent"}; // difficulty string
static const int pdmpstageids[0x10] = {0x32, 0x29, 0x17, 0x20, 0x42, 0x3C, 0x47, 0x41, 0x3B, 0x39, 0x44, 0x45, 0x3D, 0x25, 0x1F, 0x43}; // multiplayer stage ids
static DiscordRichPresence presence; // presence struct for update function
static int alreadyexec = 0; // has init already exec?
static char thumbnailid[0x5]; // holds thumbnail id

static void DRP_Init(void);
void DRP_Update(void);
void DRP_Quit(void);

//==========================================================================
// Purpose: init discord rich presence
//==========================================================================
static void DRP_Init(void)
{
	Discord_Initialize("454158757262262272", NULL, 1, NULL);
	alreadyexec = 1;
}
//==========================================================================
// Purpose: updates discord rich presence
// Changed Globals: presence struct
//==========================================================================
void DRP_Update(void)
{
	if(!alreadyexec)
		DRP_Init();
	memset(&presence, 0, sizeof(presence)); // set presence struct to 0
	if(GAME_Name() != NULL)
	{
		if(EMU_ReadROM(ROMCRC1) == GECRC1 && EMU_ReadROM(ROMCRC2) == GECRC2) // check if official GE rom
		{
			presence.largeImageKey = "ge"; // large default icon (not in-game)
			presence.largeImageText = "GoldenEye 007";
			if(EMU_ReadInt(GE_menupage) == 11) // if in-game
			{
				presence.smallImageKey = "geingame"; // set small icon
				presence.details = gestagenames[ClampInt(EMU_ReadInt(GE_stageid), 0x00, 0x39)]; // set details to map name (after clamping)
				int mpflag = EMU_ReadInt(GE_gamemode) == 1; // set to 1 if gamemode is multiplayer (0 = sp, 1 = mp, 2 = cheats)
				presence.state = !mpflag ? gedifficulty[ClampInt(EMU_ReadInt(GE_difficulty), 0, 3)] : "Multiplayer"; // set difficulty/multiplayer to state
				if(EMU_ReadInt(GE_pause) == 0 && (EMU_ReadInt(GE_camera) == 4 || EMU_ReadInt(GE_camera) == 0)) // if in gameplay mode (not intro swirl) and not paused, calculate time
				{
					time_t currenttime = time(NULL); // get current time from OS
					presence.startTimestamp = currenttime - (EMU_ReadInt(!mpflag ? GE_sptime : GE_mptime) / 60); // convert in-game time (60 Hz) to seconds and subtract from current time to get starting time for current map
				}
				sprintf(thumbnailid, "ge%1.2x", EMU_ReadInt(GE_stageid)); // set thumbnail id
				presence.largeImageKey = thumbnailid;
			}
			else // not in-game
			{
				if(EMU_ReadInt(GE_menupage) >= -1 && EMU_ReadInt(GE_menupage) <= 4) // intro range
					presence.details = "Intro";
				else if(EMU_ReadInt(GE_menupage) == 24) // character credits
					presence.details = "Credits";
				else // in menu
					presence.details = "In Main Menu";
			}
		}
		else if(EMU_ReadROM(ROMCRC1) == PDCRC1 && EMU_ReadROM(ROMCRC2) == PDCRC2) // check if official PD rom
		{
			presence.largeImageKey = "pd"; // large default icon (not in-game)
			presence.largeImageText = "Perfect Dark";
			if(EMU_ReadInt(PD_stageid) >= 0x09 && EMU_ReadInt(PD_stageid) < 0x51) // if stage id is within char array
			{
				presence.smallImageKey = "pdingame"; // set small icon
				presence.details = pdstagenames[EMU_ReadInt(PD_stageid)]; // set details to map name
				if(EMU_ReadInt(PD_stageid) != 0x26) // if level isn't carrington institute
				{
					int mpflag = 0;
					for(int index = 0; index < 0x10; index++) // compare current stage id to list of mp stage ids
						if(pdmpstageids[index] == EMU_ReadInt(PD_stageid))
							mpflag = 1;
					presence.state = !mpflag ? pddifficulty[ClampInt(EMU_ReadInt(PD_difficulty), 0, 2)] : "Combat Simulator"; // set difficulty/combat simulator to state
					if(EMU_ReadInt(PD_pause) == 0 && (EMU_ReadShort(PD_mppause) & 0xFF00) == 0 && (EMU_ReadInt(PD_camera) == 1 || EMU_ReadInt(PD_camera) == 7)) // if in gameplay mode (not cutscene) and not paused, calculate time
					{
						time_t currenttime = time(NULL); // get current time from OS
						int gametime;
						if(!mpflag)
							gametime = EMU_ReadInt(EMU_ReadInt(PD_joannadata) + PD_sptime) / 60; // convert in-game SP time (60 Hz) to seconds (SP time is part of player struct)
						else
							gametime = EMU_ReadInt(PD_mptime) / 60; // convert in-game MP time (60 Hz) to seconds
						presence.startTimestamp = currenttime - gametime; // subtract from current time to get starting time for current map
					}
				}
				sprintf(thumbnailid, "pd%1.2x", EMU_ReadInt(PD_stageid)); // set thumbnail id
				presence.largeImageKey = thumbnailid;
			}
			else if(EMU_ReadInt(PD_stageid) == -1)
				presence.details = "Intro";
			else if(EMU_ReadInt(PD_stageid) == 0x5C) // if credits
				presence.state = "Credits";
		}
		else // user is running a rom hack
		{
			switch(EMU_ReadROM(ROMNAME))
			{
				case 0x45522020: // if GF rom hack
					presence.largeImageKey = "gfcustom";
					presence.largeImageText = "Goldfinger 64";
					break;
				case 0x65205820: // if GEX rom hack
					presence.largeImageKey = "gexcustom";
					presence.largeImageText = "GoldenEye X";
					break;
				case 0x45202020: // if GE rom hack
					presence.largeImageKey = "custom";
					presence.largeImageText = "GoldenEye 007 (ROM Hack)";
					break;
				default: // assume PD rom hack
					presence.largeImageKey = "pdcustom";
					presence.largeImageText = "Perfect Dark (ROM Hack)";
					break;
			}
			presence.details = presence.largeImageText;
		}
	}
	else
	{
		presence.largeImageKey = "logo"; // default 1964 icon
		presence.largeImageText = "N64 Emulator for GE/PD";
		presence.details = "Not In-Game";
	}
	Discord_UpdatePresence(&presence);
}
//==========================================================================
// Purpose: safely shutdown discord rich presence
// Changed Globals: alreadyexec
//==========================================================================
void DRP_Quit(void)
{
	Discord_ClearPresence();
	Discord_Shutdown();
	alreadyexec = 0;
}
#else
//==========================================================================
// Purpose: dummy functions for non-drp build
//==========================================================================
void DRP_Update(void)
{
	return;
}
void DRP_Quit(void)
{
	return;
}
#endif