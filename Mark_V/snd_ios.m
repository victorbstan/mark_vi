#ifndef CORE_SDL
#include "environment.h"
#ifdef PLATFORM_GUI_IOS

/*
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

//
//  snd_iphone.m
//  iPhone Quake
//
//  Kevin Arunski, October 2008
//

// Copyright (C) 2013-2014 Baker

*/
// snd_ios.m -- cd



#include "quakedef.h"
#include "iphonequake.h"
#include "q_sound.h"	// Courtesy

#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <OpenAL/oalStaticBufferExtension.h>


// The private data of the sound output engine
static ALCdevice * device = NULL;
static ALCcontext * context = NULL;
static const ALCint contextAttributes[] = 
{
	ALC_FREQUENCY, 11025,
	ALC_STEREO_SOURCES, 1,
	0, 0
};
static ALuint al_buffer = 0;
static ALuint al_source = 0;
#define AUDIO_BUFFER_SIZE 65536
static unsigned char audio_buffer[AUDIO_BUFFER_SIZE];
// for easier debugging I keep my own handle of the sound state
static volatile dma_t  *my_dma_handle = 0;
static const ALuint sampleBits = 8;
static const ALuint soundChannels = 2;
//static size_t playBuffer = 0;
static alBufferDataStaticProcPtr alBufferDataStatic;

// initializes cycling through a DMA buffer and returns information on it
cbool SNDDMA_Init(void)
{
	cbool success = 0;
	device = alcOpenDevice(NULL);
	if (device)
	{
		// OpenAL Buffer Static Data extensions allows us to define a 
		// buffer that will be played from directly, so no memory copies
		// into the AL.
		alBufferDataStatic = alGetProcAddress("alBufferDataStatic");
		memset(audio_buffer, 0, sizeof(audio_buffer));
		
		const ALCchar * deviceName = alcGetString(device, ALC_DEVICE_SPECIFIER);
		ALCint major = 0, minor = 0;
		alcGetIntegerv(device, ALC_MAJOR_VERSION, 1, &major);
		alcGetIntegerv(device, ALC_MINOR_VERSION, 1, &minor);
		
		Con_SafePrintLinef ("Using sound device: %s OpenAL %d.%d", deviceName, major, minor);
		
		context = alcCreateContext(device, contextAttributes);
		alcMakeContextCurrent(context);
		
		ALenum err = alGetError();
		alGenBuffers(1, &al_buffer);
		err = alGetError();
		if (err == AL_NO_ERROR)
		{
			// create an audio source.
			alGenSources(1, &al_source);
			err = alGetError();
			
			// Set audio source to loop over it's buffers...
			alSourcei(al_source, AL_LOOPING, AL_TRUE);
			err = alGetError();
			
			// attach OpenAL audio buffer ID to our static data buffer that
			// will be filled by Quake.
			ALuint frequency = contextAttributes[1];
			alBufferDataStatic(al_buffer,
							   AL_FORMAT_STEREO8, 
							   audio_buffer, 
							   sizeof(audio_buffer), 
							   frequency);
			err = alGetError();
			alSourcei(al_source, AL_BUFFER, al_buffer);
			err = alGetError();
			alSourcePlay(al_source);
			err = alGetError();
			alListenerf(AL_GAIN, 0.2);
			err = alGetError();
			
			my_dma_handle = shm = &sn;
			shm->splitbuffer = 0; // ?
			
			// set sample bits & speed
			shm->samplebits = sampleBits; // or 8
			shm->speed = contextAttributes[1]; //11025, 22051, 44100, 8000 (sample rate)
			
			shm->channels = soundChannels;  // 1 for mono or 2 for stereo
			
			shm->samples = AUDIO_BUFFER_SIZE / (shm->samplebits / 8);
			shm->submission_chunk = 1;
			
			// memory mapped dma buffer
			shm->buffer = audio_buffer; 
			
			shm->samplepos = 0;
			
			success = 1;
		}
	}
	
	return success; // return true if you actually get this to work
}

// gets the current DMA position
int SNDDMA_GetDMAPos(void)
{
	// The quake engine ensures that it does not fill the playback buffer
	// ahead of the playback position.
	
	// get source playback offset.
	ALint offset;
	alGetError();
	alGetSourcei(al_source, AL_BYTE_OFFSET, &offset);
	shm->samplepos = offset % AUDIO_BUFFER_SIZE;
	return offset;
}

// shutdown the DMA xfer. (close sound device)
void SNDDMA_Shutdown(void)
{
	if (al_source)
	{
		alDeleteSources(1, &al_source);
	}
	
	if (al_buffer)
	{
		alDeleteBuffers(1, &al_buffer);
	}
	
	alcMakeContextCurrent(NULL);
	if (context)
	{
		alcDestroyContext(context);
	}
	
	if (device)
	{
		alcCloseDevice(device);
	}
}

// submit sound
void SNDDMA_Submit(void)
{
	// the audio device just loops over the same buffer over and over, while
	// the quake engine keeps filling it up. 
}

//void S_Init (void)
//{
//}
//
//void S_AmbientOff (void)
//{
//}
//
//void S_AmbientOn (void)
//{
//}
//
//void S_Shutdown (void)
//{
//}
//
//void S_TouchSound (const char *sample)
//{
//}
//
//void S_ClearBuffer (void)
//{
//}
//
//void S_StaticSound (sfx_t *sfx, vec3_t origin, float vol, float attenuation)
//{
//}
//
//void S_StartSound (int entnum, int entchannel, sfx_t *sfx, const vec3_t origin, float fvol,  float attenuation)
//{
//}
//
//void S_StopSound (int entnum, int entchannel)
//{
//}
//
//sfx_t *S_PrecacheSound (const char *name, cbool *precached_ok)
//{
//	return NULL;
//}
//
//void S_ClearPrecache (void)
//{
//}
//
//void S_Update (const vec3_t origin, const vec3_t v_forward, const vec3_t v_right, const vec3_t v_up)
//{	
//}
//
//void S_StopAllSounds (cbool clear)
//{
//}
//
//void S_BeginPrecaching (void)
//{
//}
//
//void S_EndPrecaching (void)
//{
//}
//
//void S_ExtraUpdate (void)
//{
//}
//
//void S_LocalSound (const char *s)
//{
//}
//
//void S_Play2_f (void)
//{
//	
//}
//
//void S_PlayVol(void)
//{}
//
//void S_BlockSound () {}
//
//const char * S_Sound_ListExport () { return NULL; }
//
//void S_SoundInfo_f () {}
//
//void S_UnblockSound (void) {}
//
//void Sound_Toggle_Mute_f() {}
//
//void Neh_ResetSFX () {}
//
//
//void S_SoundList () {}
//
//void S_Play_f () {}
//
//void Neh_Reset_Sfx_Count (void) {};
//
//void S_StopAllSoundsC (void) {}
//
//
//
//void S_Snd_Speed_Notify_f (cvar_t *var)
//{
//}
//

void external_music_toggle_f (cvar_t *var)
{
}



#endif // PLATFORM_GUI_IOS
#endif // ! CORE_SDL
