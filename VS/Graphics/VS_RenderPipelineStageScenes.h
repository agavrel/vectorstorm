/*
 *  VS_RenderPipelineStageScenes.h
 *  VectorStorm
 *
 *  Created by Trevor Powell on 17/02/2014
 *  Copyright 2014 Trevor Powell.  All rights reserved.
 *
 */

#ifndef VS_RENDERPIPELINESTAGESCENES_H
#define VS_RENDERPIPELINESTAGESCENES_H

#include "VS_RenderPipelineStage.h"
#include "VS_Renderer.h"

class vsDisplayList;
class vsScene;
class vsRenderTarget;

class vsRenderPipelineStageScenes: public vsRenderPipelineStage
{
	vsScene **m_scene;
	int m_sceneCount;
	vsRenderTarget *m_target;
	vsRenderer::Settings m_settings;
public:
	vsRenderPipelineStageScenes( vsScene *scene, vsRenderTarget *target, const vsRenderer::Settings& settings );
	vsRenderPipelineStageScenes( vsScene **scenes, int sceneCount, vsRenderTarget *target, const vsRenderer::Settings& settings );
	virtual ~vsRenderPipelineStageScenes();

	virtual void Draw( vsDisplayList *list );
};

#endif // VS_RENDERPIPELINESTAGESCENES_H
