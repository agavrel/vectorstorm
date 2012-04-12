/*
 *  SND_System.cpp
 *  VectorStorm
 *
 *  Created by Trevor Powell on 8/04/07.
 *  Copyright 2007 Trevor Powell. All rights reserved.
 *
 */

#include "VS_SoundSystem.h"
#include "VS_Music.h"
#include "VS_SoundSample.h"

#include "VS_System.h"

#if !TARGET_OS_IPHONE
#if __APPLE_CC__
#include <SDL_mixer/SDL_mixer.h>
#else
#include "SDL_mixer.h"
#endif
#endif

vsSoundSystem *	vsSoundSystem::s_instance = NULL;

vsSoundSystem::vsSoundSystem()
{
	s_instance = this;

	printf(" ++ Initialising mixer\n");
//	if ( Mix_OpenAudio( MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024 ) )
#if !TARGET_OS_IPHONE
	if ( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 1024 ) )
		printf(" !! Mix_OpenAudio: %s\n", Mix_GetError());

	int numtimesopened, frequency, channels;
	Uint16 format;
	numtimesopened=Mix_QuerySpec(&frequency, &format, &channels);
	if(!numtimesopened) {
		printf(" !! Mix_QuerySpec: %s\n",Mix_GetError());
	}
	else {
		const char *format_str="Unknown";
		switch(format) {
			case AUDIO_U8: format_str="U8"; break;
			case AUDIO_S8: format_str="S8"; break;
			case AUDIO_U16LSB: format_str="U16LSB"; break;
			case AUDIO_S16LSB: format_str="S16LSB"; break;
			case AUDIO_U16MSB: format_str="U16MSB"; break;
			case AUDIO_S16MSB: format_str="S16MSB"; break;
		}
		printf(" ++ audio frequency=%dHz  format=%s  channels=%d\n",
			   frequency, format_str, channels);
	}

	const char * soundDriver = SDL_AudioDriverName(NULL, 0);
	if ( soundDriver )
		printf(" ++ Sound playing using %s.\n", soundDriver);
	else
		printf(" ?? No sound driver reported by SDL_AudioDriverName.\n");

	m_channelCount = Mix_AllocateChannels(32);	// get ourselves 32 channels for sound effects.  That's pretty realistic for a SNES-era console.
	m_maxChannelsInUse = 0;
	m_channelsInUse = 0;

	Mix_ChannelFinished( &vsSoundSystem::ChannelFinished );
#endif
}

vsSoundSystem::~vsSoundSystem()
{
#if !TARGET_OS_IPHONE
	Mix_CloseAudio();
#endif

	s_instance = NULL;
}

void
vsSoundSystem::Init()
{
	for ( int i = 0; i < MAX_DEFERRED_SAMPLES; i++ )
		m_deferredSample[i].m_sample = NULL;

	InitVolume();
}

void
vsSoundSystem::InitVolume()
{
#if !TARGET_OS_IPHONE
	vsSystemPreferences *p = vsSystem::Instance()->GetPreferences();
	float effectVolumeFraction = p->GetEffectVolume() / 10.f;
	float musicVolumeFraction = p->GetMusicVolume() / 10.f;

	for ( int i = 0; i < m_maxChannelsInUse; i++ )
		Mix_Volume(i, int(MIX_MAX_VOLUME * effectVolumeFraction));

	Mix_VolumeMusic(int(MIX_MAX_VOLUME * musicVolumeFraction));
#endif
}

void
vsSoundSystem::Deinit()
{
	StopMusic();
#if !TARGET_OS_IPHONE
	Mix_HaltChannel(-1);
#endif
	printf(" ++ Sound channels in use: %d\n", m_channelsInUse);
	printf(" ++ Max sound channels in use: %d\n", m_maxChannelsInUse);
}

void
vsSoundSystem::Update( float timeStep )
{
	for ( int i = 0; i < MAX_DEFERRED_SAMPLES; i++ )
	{
		if ( m_deferredSample[i].m_sample )
		{
			m_deferredSample[i].m_fuse -= timeStep;

			if ( m_deferredSample[i].m_fuse <= 0.f )
			{
				PlaySound( m_deferredSample[i].m_sample );
				m_deferredSample[i].m_sample = NULL;
			}
		}
	}
}

void
vsSoundSystem::ChannelFinished( int channel )
{
	UNUSED(channel);
	//printf("Channel %d finished playing.\n", channel);
	vsSoundSystem::Instance()->m_channelsInUse--;
}

void
vsSoundSystem::PlayMusic( vsMusic * music )
{
#if !TARGET_OS_IPHONE
	if ( music )
	{
		if(Mix_PlayMusic(music->m_music, -1)==-1) {
			printf("Mix_PlayMusic: %s\n", Mix_GetError());
			// well, there's no music, but most games don't break without music...
		}
	}
	else
		StopMusic();
#endif
}

void
vsSoundSystem::StopMusic()
{
#if !TARGET_OS_IPHONE
	Mix_HaltMusic();
#endif
}

int
vsSoundSystem::PlaySound( vsSoundSample *sound )
{
#if !TARGET_OS_IPHONE
	int channelUsed = Mix_PlayChannel( -1, sound->m_chunk, 0 );
	if ( channelUsed != -1 )
	{
		// successfully started the sound on channel 'channelUsed'

		m_channelsInUse++;

		if ( m_channelsInUse > m_maxChannelsInUse )
			m_maxChannelsInUse = m_channelsInUse;
	}

	return channelUsed;
#else
	return 0;
#endif
}

void
vsSoundSystem::StopChannel( int channel )
{
#if !TARGET_OS_IPHONE
	if ( channel != -1 )
		Mix_FadeOutChannel( channel, 500 );
#endif
}

void
vsSoundSystem::PlaySoundDeferred( vsSoundSample *sound, float fuse )
{
	for ( int i = 0; i < MAX_DEFERRED_SAMPLES; i++ )
	{
		if ( m_deferredSample[i].m_sample == NULL )
		{
			m_deferredSample[i].m_sample = sound;
			m_deferredSample[i].m_fuse = fuse;
			return;
		}
	}
}

void
vsSoundSystem::CancelDeferredSounds( vsSoundSample *sound )
{
	for ( int i = 0; i < MAX_DEFERRED_SAMPLES; i++ )
	{
		if ( m_deferredSample[i].m_sample == sound )
			m_deferredSample[i].m_sample = NULL;
	}
}
