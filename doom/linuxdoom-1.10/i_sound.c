// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log:$
//
// DESCRIPTION:
//	System interface for sound.
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id: i_unix.c,v 1.5 1997/02/03 22:45:10 b1 Exp $";

#include <stdio.h>
#include <string.h>

#include "z_zone.h"
#include "w_wad.h"

#include "i_sound.h"
#include "i_music.h"
#include "sounds.h"


// UNIX hack, to be removed.
#ifdef SNDSERV
char*  sndserver_filename = "./sndserver ";
#endif


//
// The browser pulls audio with I_MixAudio, so instead of a sound
// device this keeps a few channels of 8 bit DMX samples and mixes
// them over the music when asked.
//

#define MIX_CHANNELS		16
#define MIX_CHUNK		512

typedef struct
{
    const byte*	data;
    unsigned	length;
    unsigned	rate;
} sample_t;

typedef struct
{
    int		id;
    int		handle;
    const byte*	data;
    unsigned	length;
    unsigned	rate;
    // Position in the sample, in 1/65536ths of a sample.
    unsigned long long	position;
    int		left;
    int		right;
} mix_channel_t;

static sample_t		samples[NUMSFX];
static mix_channel_t	mix_channels[MIX_CHANNELS];
static int		next_handle = 1;


// Reads a DMX sound lump: a format 3 header with the sample rate and
// length, then 8 bit unsigned samples.
static void LoadSample(sample_t* sample, const byte* lump, int lumplen)
{
    unsigned	rate;
    unsigned	length;

    if (lumplen < 8 || lump[0] != 3 || lump[1] != 0)
	return;

    rate = lump[2] | (lump[3] << 8);
    length = lump[4] | (lump[5] << 8) | (lump[6] << 16) | (lump[7] << 24);

    // DMX skips the 16 bytes of padding at each end, and ignores
    // sounds that are too short to have any audio.
    if (length > (unsigned) lumplen - 8 || length <= 48)
	return;

    sample->data = lump + 8 + 16;
    sample->length = length - 32;
    sample->rate = rate;
}

static mix_channel_t* FindChannel(int handle)
{
    int i;

    for (i=0 ; i<MIX_CHANNELS ; i++)
	if (mix_channels[i].data && mix_channels[i].handle == handle)
	    return &mix_channels[i];

    return NULL;
}

// vol is the game's 0 to 15 volume, and sep pans from 1 (left) to 255.
static void SetChannelParams(mix_channel_t* channel, int vol, int sep)
{
    vol *= 8;
    if (vol > 127)
	vol = 127;
    if (vol < 0)
	vol = 0;

    // linuxdoom's panning curve.
    sep += 1;
    channel->left = vol - ((vol*sep*sep) >> 16);
    sep -= 257;
    channel->right = vol - ((vol*sep*sep) >> 16);

    if (channel->left < 0)
	channel->left = 0;
    if (channel->right < 0)
	channel->right = 0;
}


void I_SetChannels(){}

void I_SetSfxVolume(int volume){}

// volume is 0 to 15. S_SetMusicVolume also sends 127 first.
void I_SetMusicVolume(int volume)
{
    volume *= 8;
    if (volume > 127)
	volume = 127;
    I_MUS_SetVolume(volume);
}

int I_GetSfxLumpNum(sfxinfo_t* sfx)
{
    char namebuf[9];
    int lump;

    sprintf(namebuf, "ds%s", sfx->name);
    lump = W_CheckNumForName(namebuf);

    // The shareware IWAD lacks the registered game's sounds.
    if (lump == -1)
	lump = W_GetNumForName("dspistol");

    return lump;
}

int
I_StartSound
( int		id,
  int		vol,
  int		sep,
  int		pitch,
  int		priority )
{
    mix_channel_t*	channel = NULL;
    int			i;

    if (!samples[id].data)
	return 0;

    // These sounds play one at a time.
    if (id == sfx_sawup || id == sfx_sawidl || id == sfx_sawful
	|| id == sfx_sawhit || id == sfx_stnmov || id == sfx_pistol)
    {
	for (i=0 ; i<MIX_CHANNELS ; i++)
	    if (mix_channels[i].id == id)
		mix_channels[i].data = NULL;
    }

    // Use a free channel, or else replace the oldest sound.
    for (i=0 ; i<MIX_CHANNELS ; i++)
    {
	mix_channel_t* c = &mix_channels[i];

	if (!c->data)
	{
	    channel = c;
	    break;
	}
	if (!channel || c->handle < channel->handle)
	    channel = c;
    }

    // Doom 1.9 dropped linuxdoom's random pitch shifts, so pitch is
    // ignored.
    channel->id = id;
    channel->handle = next_handle++;
    channel->data = samples[id].data;
    channel->length = samples[id].length;
    channel->rate = samples[id].rate;
    channel->position = 0;
    SetChannelParams(channel, vol, sep);

    return channel->handle;
}

void I_StopSound (int handle)
{
    mix_channel_t* channel = FindChannel(handle);

    if (channel)
	channel->data = NULL;
}


int I_SoundIsPlaying(int handle)
{
    return FindChannel(handle) != NULL;
}

// Sound is mixed when the browser asks for it, in I_MixAudio.
void I_UpdateSound( void ){}

void I_SubmitSound(void){}



void
I_UpdateSoundParams
( int	handle,
  int	vol,
  int	sep,
  int	pitch)
{
    mix_channel_t* channel = FindChannel(handle);

    if (channel)
	SetChannelParams(channel, vol, sep);
}

void I_ShutdownSound(void){}

void I_InitSound()
{
    int		i;
    int		lump;

    // Cache every sound up front, as linuxdoom did.
    for (i=1 ; i<NUMSFX ; i++)
    {
	if (S_sfx[i].link)
	    continue;

	lump = I_GetSfxLumpNum(&S_sfx[i]);
	S_sfx[i].data = W_CacheLumpNum(lump, PU_STATIC);
	LoadSample(&samples[i], S_sfx[i].data, W_LumpLength(lump));
    }

    for (i=1 ; i<NUMSFX ; i++)
    {
	if (S_sfx[i].link)
	{
	    S_sfx[i].data = S_sfx[i].link->data;
	    samples[i] = samples[S_sfx[i].link - S_sfx];
	}
    }

    fprintf(stderr, "I_InitSound: sound effects cached.\n");

    I_InitMusic();
}

void I_InitMusic(void)
{
    if (W_CheckNumForName("GENMIDI") == -1)
	return;

    I_MUS_Init(W_CacheLumpName("GENMIDI", PU_STATIC));
    fprintf(stderr, "I_InitMusic: OPL music ready.\n");
}

void I_ShutdownMusic(void)
{
    I_MUS_Stop();
}

void I_PlaySong(int handle, int looping)
{
    I_MUS_Play(looping);
}

void I_PauseSong (int handle)
{
    I_MUS_Pause(true);
}

void I_ResumeSong (int handle)
{
    I_MUS_Pause(false);
}

void I_StopSong(int handle)
{
    I_MUS_Stop();
}

void I_UnRegisterSong(int handle)
{
    I_MUS_Unregister();
}

int I_RegisterSong(void* data)
{
    return I_MUS_Register(data) ? 1 : 0;
}


static void MixChannel(mix_channel_t* channel, int* mix, int frames,
		       int rate)
{
    unsigned long long	step;
    unsigned long long	end;
    unsigned		index;
    unsigned		fraction;
    int			s0;
    int			s1;
    int			sample;
    int			i;

    step = ((unsigned long long) channel->rate << 16) / rate;
    end = (unsigned long long) channel->length << 16;

    for (i=0 ; i<frames ; i++)
    {
	if (channel->position >= end)
	{
	    channel->data = NULL;
	    return;
	}

	// Interpolate, since the samples are 11 kHz.
	index = channel->position >> 16;
	fraction = channel->position & 0xffff;
	s0 = channel->data[index] - 128;
	s1 = index + 1 < channel->length ? channel->data[index + 1] - 128 : s0;
	sample = (s0 << 8) + (((s1 - s0) * (int) fraction) >> 8);

	mix[i*2] += sample * channel->left / 127;
	mix[i*2+1] += sample * channel->right / 127;

	channel->position += step;
    }
}

//
// Fills out with frames interleaved stereo 16 bit samples at rate Hz:
// the music with the sound effects on top.
//
void I_MixAudio(short* out, int frames, int rate)
{
    static int	mix[MIX_CHUNK*2];
    int		n;
    int		i;

    while (frames > 0)
    {
	n = frames < MIX_CHUNK ? frames : MIX_CHUNK;

	I_MUS_Render(out, n, rate);

	for (i=0 ; i<n*2 ; i++)
	    mix[i] = out[i];

	for (i=0 ; i<MIX_CHANNELS ; i++)
	    if (mix_channels[i].data)
		MixChannel(&mix_channels[i], mix, n, rate);

	for (i=0 ; i<n*2 ; i++)
	{
	    if (mix[i] > 32767)
		out[i] = 32767;
	    else if (mix[i] < -32768)
		out[i] = -32768;
	    else
		out[i] = mix[i];
	}

	out += n*2;
	frames -= n;
    }
}
