//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 2005-2014 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// DESCRIPTION:
//	MUS music played on an emulated OPL2 (AdLib) chip.
//

#ifndef __I_MUSIC__
#define __I_MUSIC__

// genmidi is the GENMIDI lump with the OPL instrument bank.
void I_MUS_Init(void* genmidi);

// Returns false if data is not a MUS lump.
int I_MUS_Register(void* data);
void I_MUS_Unregister(void);
void I_MUS_Play(int looping);
void I_MUS_Stop(void);
void I_MUS_Pause(int paused);

// volume is 0 to 127.
void I_MUS_SetVolume(int volume);

// Writes frames interleaved stereo samples at rate Hz to out.
void I_MUS_Render(short* out, int frames, int rate);

#endif
