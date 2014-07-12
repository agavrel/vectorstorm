/*
 *  VS_System.h
 *  VectorStorm
 *
 *  Created by Trevor Powell on 27/12/07.
 *  Copyright 2007 Trevor Powell. All rights reserved.
 *
 */

#ifndef VS_SYSTEM_H
#define VS_SYSTEM_H

#include "Utils/VS_Singleton.h"

class vsMaterialManager;
class vsPreferences;
class vsPreferenceObject;
class vsSystemPreferences;
class vsScreen;
class vsTextureManager;

enum Weekday
{
	Sunday,
	Monday,
	Tuesday,
	Wednesday,
	Thursday,
	Friday,
	Saturday
};

struct vsTime
{
	int second;
	int	minute;
	int hour;
	int day;
	int month;
	int year;
	Weekday wday;
};

enum Orientation
{
	Orientation_Normal,
	Orientation_Three,
	Orientation_Six,
	Orientation_Nine
};

class vsSystem
{
	static vsSystem *	s_instance;

	bool				m_showCursor;
	bool				m_showCursorOverridden;
	bool				m_focused;
	bool				m_exitGameKeyEnabled;
	bool				m_exitApplicationKeyEnabled;

	Orientation			m_orientation;

	vsTextureManager *	m_textureManager;
	vsMaterialManager *	m_materialManager;

	vsScreen *			m_screen;

	vsSystemPreferences *m_preferences;

public:

	static vsSystem *	Instance() { return s_instance; }

	vsSystem( size_t totalMemoryBytes = 1024*1024*64 );
	~vsSystem();

	void Init();
	void Deinit();

	void		InitGameData();	// game has exitted, kill anything else we know about that it might have been using.
	void		DeinitGameData();	// game has exitted, kill anything else we know about that it might have been using.

	time_t		GetTime();
	void		MakeVsTime( vsTime *t, time_t time );	// correct for local time zone
	void		MakeVsTime_UTC( vsTime *t, time_t time );	// give time in UTC
	vsString	MakeTimeString(uint32_t time);

	void UpdateVideoMode();
	void UpdateVideoMode(int width, int height);
	void CheckVideoMode();

	void	ShowCursor(bool show);
	void	HideCursor() { ShowCursor(false); }
	void	SetWindowCaption(const vsString &caption);
	bool	AppHasFocus() { return m_focused; }
	void	SetAppHasFocus( bool focus ) { m_focused = focus; }

	void	SetOrientation( Orientation orient ) { m_orientation = orient; }
	Orientation	GetOrientation() { return m_orientation; }

	void	Launch( const vsString &string );
	int		GetNumberOfCores();		// estimates the number of cores on this computer.

	static vsScreen *		GetScreen() { return Instance()->m_screen; }
	vsSystemPreferences *	GetPreferences() { return m_preferences; }

	// if set 'true', then the default "exit game" key ('q') will be enabled,
	// and will cause execution to return to the main game.  See
	// CORE_Registry.h for details.
	void EnableExitGameKey(bool enable) { m_exitGameKeyEnabled = enable; }
	// if set 'true', then the default "quit" key ('ESC') will be enabled,
	// and will cause the whole application to exit.  If false, you need
	// to handle exiting the application explicitly in game code.  Call
	// core::Exit() when you wish to trigger the application to exit.
	void EnableExitApplicationKey(bool enable) { m_exitApplicationKeyEnabled = enable; }

	bool IsExitGameKeyEnabled() const { return m_exitGameKeyEnabled; }
	bool IsExitApplicationKeyEnabled() const { return m_exitApplicationKeyEnabled; }
};


struct Resolution
{
	int width;
	int height;
};


	/** vsSystemPreferences
	 *		Accessor class to provide managed access to the global preferences data.
	 */
class vsSystemPreferences
{
	vsPreferences *	m_preferences;

	vsPreferenceObject *	m_resolution;
	vsPreferenceObject *	m_resolutionX;
	vsPreferenceObject *	m_resolutionY;
	vsPreferenceObject *	m_fullscreen;
	vsPreferenceObject *	m_vsync;
	vsPreferenceObject *	m_bloom;

	vsPreferenceObject *	m_effectVolume;
	vsPreferenceObject *	m_musicVolume;

	Resolution*	m_supportedResolution;
	int			m_supportedResolutionCount;


public:

	vsSystemPreferences();
	~vsSystemPreferences();

	void			Save();

	void			CheckResolutions();
	int				GetSupportedResolutionCount() { return m_supportedResolutionCount; }
	Resolution *	GetSupportedResolutions() { return m_supportedResolution; }

	// Video preferences
	Resolution *	GetResolution();
	int				GetResolutionId();
	void			SetResolutionId(int val);

	bool			GetFullscreen();
	void			SetFullscreen(bool fullscreen);

	bool			GetVSync();
	void			SetVSync(bool vsync);

	bool			GetBloom();
	void			SetBloom(bool enabled);

	// Sound preferences
	int				GetEffectVolume();
	void			SetEffectVolume(int volume);

	int				GetMusicVolume();
	void			SetMusicVolume(int volume);
};

#endif // VS_SYSTEM_H

