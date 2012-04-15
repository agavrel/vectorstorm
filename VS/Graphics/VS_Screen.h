/*
 *  VS_Screen.h
 *  VectorStorm
 *
 *  Created by Trevor Powell on 12/03/07.
 *  Copyright 2007 Trevor Powell. All rights reserved.
 *
 */

#ifndef VS_SCREEN_H
#define VS_SCREEN_H

#if defined(_DEBUG)
#define DEBUG_SCENE
#endif // _DEBUG

#include "Graphics/VS_Renderer.h"
#include "Utils/VS_Singleton.h"

class vsDisplayList;
class vsScene;
class vsRenderTarget;
class vsImage;


class vsScreen
{
	vsRenderer *		m_renderer;		// our renderer
	vsScene **			m_scene;		// our draw scenes
    vsRenderer::Settings    m_defaultRenderSettings;

	int					m_sceneCount;	// how many layers we have

	vsDisplayList *		m_fifo;			// our FIFO display list, for rendering
	vsDisplayList *		m_subfifo;			// our FIFO display list, for rendering

	int					m_width;
	int					m_height;
	int					m_depth;
	float				m_aspectRatio;

	vsRenderTarget *	m_currentRenderTarget;

public:

	vsScreen(int width, int height, int depth, bool fullscreen);
	~vsScreen();

	void			UpdateVideoMode(int width, int height, int depth, bool fullscreen);

	int				GetTrueWidth() { return m_width; }
	int				GetTrueHeight() { return m_height; }
	float			GetTrueAspectRatio();

	int				GetWidth();
	int				GetHeight();
	float			GetAspectRatio();



	void			RenderDisplayList( vsDisplayList *list );	// used to pipe our display list to the renderer, for creating compiled display lists.

	void			CreateScenes(int count);
	void			DestroyScenes();

	void			Update( float timeStep );
	void			Draw();
	bool			DrawSceneToTarget( const vsRenderer::Settings &s, int scene, vsRenderTarget *target );
	bool			DrawSceneRangeToTarget( const vsRenderer::Settings &s, int firstScene, int lastScene, vsRenderTarget *target );

	void			SetOpaqueRendering();

	vsImage *       Screenshot();
	vsImage *       ScreenshotDepth();
	vsImage *       ScreenshotAlpha();

	vsScene *		GetScene(int i);

#if defined(DEBUG_SCENE)
	vsScene *		GetDebugScene();
#endif // DEBUG_SCENE
};

#endif // VS_SCREEN_H
