/*
 *  VS_Renderer.cpp
 *  VectorStorm
 *
 *  Created by Trevor Powell on 17/03/07.
 *  Copyright 2007 Trevor Powell. All rights reserved.
 *
 */

#include "VS_Renderer.h"

vsRenderer::Settings::Settings():
	shaderSuite(NULL),
    aspectRatio(1.f),
	polygonOffsetUnits(0.f),
    useCustomAspectRatio(false),
	writeColor(true),
	writeDepth(true),
	invertCull(false)
{
}

void
vsRenderer::UpdateVideoMode(int width, int height, int depth)
{
	UNUSED(depth);
	m_width = m_viewportWidth = width;
	m_height = m_viewportHeight = height;
}

