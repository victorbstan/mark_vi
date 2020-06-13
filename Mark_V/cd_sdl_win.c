#if defined(CORE_SDL) && defined(_WIN32) // SDL + Windows

/*
Copyright (C) 1996-2001 Id Software, Inc.
Copyright (C) 2002-2009 John Fitzgibbons and others
Copyright (C) 2009-2014 Baker and others

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// Quake is a trademark of Id Software, Inc., (c) 1996 Id Software, Inc. All
// rights reserved.

// cd_sdl_win.c

#include "quakedef.h"
#include "Mmsystem.h" // Never needed this before
//#include "winquake.h"
#pragma comment (lib, "winmm.lib")		// timeBeginPeriod



//static cbool cdValid = false;
//static cbool	wasPlaying = false;

//static cbool	enabled = false;
//static cbool playLooping = false;
//
//static byte 	remap[MAX_MUSIC_MAPS_100];
//static byte		cdrom;
//static byte		playTrack;
//static byte		maxTrack;

#define MAX_TRACK_99 99

typedef struct {
	char	track_file[MAX_QPATH_64];
	byte 	remap[MAX_TRACK_99 + 1];
	int		playing;
	cbool	initialized;
	float	cdvolume;
	cbool	muted;
} msound_t;

msound_t muz;

float Muz_GetVolume_()
{
	if (vid.ActiveApp == false || vid.Minimized == true || muz.muted == true) return 0;
	return bgmvolume.value * sfxvolume.value;
}

// Really is that what this is?
#define CAP_1500		1500
#define PCT_MAX_VOL_3	3
int Muz_Decibals_ (float pct_volume)
{
	if (pct_volume <= 0)	return 0; //return -10000;

	if (pct_volume >= PCT_MAX_VOL_3)	return CAP_1500;

	return (int)(pct_volume * CAP_1500 / PCT_MAX_VOL_3);
}

// track01.mp3 ... track99.mp3
void CDAudio_Play (byte track, cbool looping)
{
	char track_file[MAX_QPATH_64];
	char *absolute_filename = NULL;

	CDAudio_Stop();

	if (!muz.initialized)
		return;

	if (!external_music.value)
		return; // User has it off

	c_snprintf1 (track_file, "music/track%02d.mp3", track); // Byte automatically converts to integer for varadic
	absolute_filename = COM_FindFile_NoPak (track_file);

	if (!absolute_filename) {
		Con_PrintLinef ("CDAudio: track %d not found", track);
		DEBUG_ASSERT (muz.playing == 0);
		return;
	}

	
	File_URL_Edit_SlashesBack_Like_Windows (absolute_filename); // Convert to windows format.

	{
		// Found track
		const char *cmdstring = va ("open " QUOTED_S " type mpegvideo alias song1", absolute_filename);
		int error_if_non_zero = mciSendString (cmdstring, /*string*/ NULL, /*length*/ 0, /*callback*/ NULL);
		if (error_if_non_zero) {
			Con_PrintLinef ("CDAudio: track %d is not audio", track);
			DEBUG_ASSERT (muz.playing == 0);
			return;
		}

		{
			float pctvol = Muz_GetVolume_();
			int db = Muz_Decibals_ (pctvol);
			char *cmdString = va("setaudio song1 volume to %d", db);

			error_if_non_zero = mciSendString (cmdString, /*string*/ NULL, /*length*/ 0, /*callback*/ NULL);
		}

		error_if_non_zero = mciSendString ("play song1 repeat", /*string*/ NULL, /*length*/ 0, /*callback*/ NULL);

		c_strlcpy (muz.track_file, track_file); // Copy it off.
		muz.playing = track;

	}
}


void CDAudio_Stop(void)
{
	if (!muz.playing)
		return; // Nothing to do.

	mciSendString("close song1", /*string*/ NULL, /*length*/ 0, /*callback*/ NULL);
	muz.playing = 0;
}


void CDAudio_Resume(void)
{
	if (!muz.playing)
		return; // Nothing to do.

	mciSendString("resume song1", /*string*/ NULL, /*length*/ 0, /*callback*/ NULL);
}



void CDAudio_Update(void)
{
	if (muz.playing) {
		static	float	old_effective_volume	= -1;
				float	effective_volume		= Muz_GetVolume_();
			
		if (effective_volume == old_effective_volume)
			return;

		{
			int db = Muz_Decibals_ (effective_volume);
			char *cmdString = va("setaudio song1 volume to %d", db);
			int error_if_non_zero = mciSendString (cmdString, /*string*/ NULL, /*length*/ 0, /*callback*/ NULL);
			//Con_PrintLinef ("Set volume to %d with code %d for effective_volume %2.4f", db, error_if_non_zero, effective_volume);
		}

		old_effective_volume = effective_volume;
	}
}






void CDAudio_Pause ()
{
	if (!muz.playing)
		return; // Not playing

	mciSendString ("pause song1", /*string*/ NULL, /*length*/ 0, /*callback*/ NULL);
}




void CDAudio_Shutdown(void)
{
	if (!muz.initialized)
		return;

#if 0
	CDAudio_Stop();
#endif
}


void CDAudio_Init(void)
{
	Cmd_AddCommands (CDAudio_Init);

	if (COM_CheckParm("-nosound") || COM_CheckParm("-nocdaudio")) {
		muz.initialized = false;
		Con_SafePrintLinef ("CD disabled by command line");
		return;
	}

	// Set up remap
	{ int n;  for (n = 0; n < ARRAY_COUNT(muz.remap); n++) {
		muz.remap[n] = n;
	}}

	muz.initialized = true;

	Con_SafePrintLinef ("%s", "MP3 Audio Initialized");
}

// Externs

void CD_f ()
{


}

// external_music
void external_music_toggle_f (cvar_t *var)
{
	if (!muz.initialized)
		return; // Command line disabled

	switch (CONVBOOL var->value) {
	default /*true*/:	if (cls.state == ca_connected) {
							CDAudio_Play ((byte)cl.cdtrack, true);
						}

	case_break false:	CDAudio_Stop ();	// Zero
	}
}


#endif // SDL + Windows