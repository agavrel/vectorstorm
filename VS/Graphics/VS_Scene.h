/*
 *  VS_Scene.h
 *  VectorStorm
 *
 *  Created by Trevor Powell on 12/03/07.
 *  Copyright 2007 Trevor Powell. All rights reserved.
 *
 */

#ifndef VS_SCENE_H
#define VS_SCENE_H

#include "VS/Math/VS_Vector.h"
#include "VS/Math/VS_Transform.h"
#include "VS/Graphics/VS_DisplayList.h"
#include "VS/Graphics/VS_Screen.h"

class vsEntity;
class vsDisplayList;
class vsCamera2D;
class vsCamera3D;
class vsFog;
class vsLight;
class vsRenderQueue;

extern vsTransform2D	g_drawingCameraTransform;	// this transform is active during Draw() calls, and should tell the camera transform in LOCAL coordinates!

#define MAX_SCENE_LIGHTS	(8)
#define MAX_SCENE_STACK		(20)

class vsScene
{
	vsRenderQueue *	m_queue;
	vsEntity *		m_entityList;	// our sprites
	//	vsDisplayList *	m_displayList;
	vsCamera2D *		m_camera;
	vsCamera2D *		m_defaultCamera;
	
	vsCamera3D *		m_camera3D;
	vsCamera3D *		m_defaultCamera3D;
	bool				m_is3d;
	bool				m_cameraIsReference;
	bool				m_flatShading;
	
	vsLight *			m_light[MAX_SCENE_LIGHTS];
	vsFog *				m_fog;
	
	static vsScene *	s_current;
	
public:
	
	vsScene();
	virtual			~vsScene();
	
	void			Set3D(bool i) { m_is3d = i; }
	bool			Is3D() { return m_is3d; }
	
	float			GetFOV();
	
	vsVector2D		GetCorner(bool bottom, bool right);
	
	vsVector2D		GetTopLeftCorner() { return GetCorner(false,false); }
	vsVector2D		GetTopRightCorner() { return GetCorner(false,true); }
	vsVector2D		GetBottomLeftCorner() { return GetCorner(true,false); }
	vsVector2D		GetBottomRightCorner() { return GetCorner(true,true); }
	
	//	vsDisplayList *	GetDisplayList() { return m_displayList; }
	vsCamera2D *		GetCamera() { return m_camera; }
	vsCamera3D *		GetCamera3D() { return m_camera3D; }
	void			SetCamera2D( vsCamera2D *camera, bool reference=false );	// if the camera is "reference", then we don't own this camera;
	void			SetCamera3D( vsCamera3D *camera, bool reference=false );	// and so do not call Update() on it.  (This is for use if we want to set the same camera on multiple layers)
	
#if defined(DEBUG_SCENE)
	void			SetDebugCamera();
#endif // DEBUG_SCENE
	
	void			Update( float timeStep );
	void			Draw( vsDisplayList *list );
	
	void			RegisterEntityOnTop( vsEntity *sprite );
	void			RegisterEntityOnBottom( vsEntity *sprite );
	
	void			AddLight( vsLight *light );
	void			RemoveLight( vsLight *light );
	
	void			SetFog( vsFog *fog ) { m_fog = fog; }
	
	void			SetFlatShading( bool shading ) { m_flatShading = shading; }
	
	vsEntity *		FindEntityAtPosition( const vsVector2D &pos );	// returns which entity is at the passed position (if any)
	
	static vsScene *	GetCurrent() { return s_current; }
};

#endif // VS_SCENE_H
