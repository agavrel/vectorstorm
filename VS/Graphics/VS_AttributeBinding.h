/*
 *  VS_AttributeBinding.h
 *  VectorStorm
 *
 *  Created by Trevor Powell on 26/06/2019
 *  Copyright 2019 Trevor Powell.  All rights reserved.
 *
 */

#ifndef VS_ATTRIBUTEBINDING_H
#define VS_ATTRIBUTEBINDING_H

#include "VS/Utils/VS_Array.h"
#include "VS_RendererState.h"
class vsRenderBuffer;
class vsVector2D;
class vsVector3D;
class vsColor;

class vsAttributeBinding
{
	struct Binding
	{
		vsRenderBuffer *buffer;

		float *p;
		int typeCount;
		int floatCount;

		Binding(): buffer(NULL), p(NULL), typeCount(0), floatCount(0) {}
	};
	// TODO:  This vsAttributeState may just go away, shortly.
	vsAttributeState m_attributeState;
	vsRenderBuffer *m_vertexAttributes; // attributes [0-3]

	vsArray<Binding> m_attribute; // attributes [4+]

	bool m_dirty;
	uint32_t m_vao;

	void SetupAndBindVAO();
	void DoBindAttribute(int i);

public:
	vsAttributeBinding(int attributeCount = 4);
	~vsAttributeBinding();

	void SetVertexAttributes( vsRenderBuffer *buffer );
	void SetAttribute( int attribute, vsRenderBuffer *buffer );

	void SetAttribute( int attribute, vsVector3D *p, int count );
	void SetAttribute( int attribute, vsVector2D *p, int count );
	void SetAttribute( int attribute, vsColor *p, int count );

	void ClearAttribute( int attribute );

	// "Bind()' is only to be called from the main thread.
	void Bind();
};

#endif // VS_ATTRIBUTEBINDING_H

