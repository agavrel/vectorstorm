//
//  VS_RendererState.cpp
//  MMORPG2
//
//  Created by Trevor Powell on 23/04/11.
//  Copyright 2011 VectorStorm. All rights reserved.
//

#include "VS_RendererState.h"
#include "VS_OpenGL.h"

class glEnableSetter : public StateSetter<bool>
{
	int m_type;
public:
	glEnableSetter( int type, const bool &initialValue ):
		StateSetter<bool>(initialValue),
		m_type(type)
	{
	}

	virtual void DoFlush()
	{
		if ( m_value )
		{
			glEnable(m_type);
		}
		else
		{
			glDisable(m_type);
		}
	}
};

class glClientStateSetter : public StateSetter<bool>
{
	int m_type;
public:
	glClientStateSetter( int type, const bool &initialValue ):
		StateSetter<bool>(initialValue),
		m_type(type)
	{
	}

	virtual void DoFlush()
	{
		if ( m_value )
		{
			// glEnableClientState(m_type);
			glEnableVertexAttribArray(m_type);
		}
		else
		{
			// glDisableClientState(m_type);
			glDisableVertexAttribArray(m_type);
		}
	}
};


class glDepthMaskSetter : public StateSetter<bool>
{
public:
	glDepthMaskSetter( const bool &initialValue ):
	StateSetter<bool>( initialValue )
	{
	}

	virtual void DoFlush()
	{
		glDepthMask( m_value ) ;
	}
};

class glAlphaThreshSetter : public StateSetter<float>
{
public:
	glAlphaThreshSetter( const float &initialValue ):
		StateSetter<float>( initialValue )
	{
	}

	virtual void DoFlush()
	{
		glAlphaFunc( GL_GREATER, m_value ) ;
	}
};

class glPolygonOffsetUnitsSetter : public StateSetter2<float,float>
{
public:
	glPolygonOffsetUnitsSetter( const float &initialValueA, const float &initialValueB ):
		StateSetter2<float,float>( initialValueA, initialValueB )
	{
	}

	virtual void DoFlush()
	{
		glPolygonOffset( m_valueB, m_valueA ) ;
	}
};

class glCullFaceSetter : public StateSetter<int>
{
public:
	glCullFaceSetter( const int &initialValue ):
	StateSetter<int>( initialValue )
	{
	}

	virtual void DoFlush()
	{
		glCullFace( m_value ) ;
	}
};

vsRendererState::vsRendererState()
{
	// m_boolState[Bool_AlphaTest] =		new glEnableSetter( GL_ALPHA_TEST, false );
	m_boolState[Bool_Blend] =			new glEnableSetter( GL_BLEND, false );
	m_boolState[Bool_ColorMaterial] =	new glEnableSetter( GL_COLOR_MATERIAL, false );
	m_boolState[Bool_CullFace] =		new glEnableSetter( GL_CULL_FACE, false );
	m_boolState[Bool_DepthTest] =		new glEnableSetter( GL_DEPTH_TEST, false );
	m_boolState[Bool_StencilTest] =		new glEnableSetter( GL_STENCIL_TEST, false );
	m_boolState[Bool_ScissorTest] =		new glEnableSetter( GL_SCISSOR_TEST, false );
	// m_boolState[Bool_Fog] =				new glEnableSetter( GL_FOG, false );
	// m_boolState[Bool_Lighting] =		new glEnableSetter( GL_LIGHTING, false );
	m_boolState[Bool_Multisample] =		new glEnableSetter( GL_MULTISAMPLE, false );
	m_boolState[Bool_PolygonOffsetFill] = new glEnableSetter( GL_POLYGON_OFFSET_FILL, false );
	//m_boolState[Bool_Smooth] =			new glEnableSetter( GL_SMOOTH, false );
	//m_boolState[Bool_Texture2D] =		new glEnableSetter( GL_TEXTURE_2D, false );

	m_boolState[Bool_DepthMask] =		new glDepthMaskSetter( false );

	m_boolState[ClientBool_VertexArray] =				new glClientStateSetter( 0, false );
	m_boolState[ClientBool_TextureCoordinateArray] =	new glClientStateSetter( 1, false );
	m_boolState[ClientBool_NormalArray] =				new glClientStateSetter( 2, false );
	m_boolState[ClientBool_ColorArray] =				new glClientStateSetter( 3, false );

	// m_floatState[Float_AlphaThreshhold] = new glAlphaThreshSetter( 0.f );
	m_float2State[Float2_PolygonOffsetConstantAndFactor] = new glPolygonOffsetUnitsSetter( 0.f, 0.f );

	m_intState[Int_CullFace] = new glCullFaceSetter(0);
}

vsRendererState::~vsRendererState()
{
	for ( int i = 0; i < BOOL_COUNT; i++ )
	{
		delete m_boolState[i];
	}
    for ( int i = 0; i < INT_COUNT; i++ )
    {
        delete m_intState[i];
    }
	/*for ( int i = 0; i < FLOAT_COUNT; i++ )
	{
		delete m_floatState[i];
	}*/
}

void
vsRendererState::SetBool( vsRendererState::Bool key, bool value )
{
	m_boolState[key]->Set(value);

	//Flush();
}

/*void
vsRendererState::SetFloat( vsRendererState::Float key, float value )
{
	m_floatState[key]->Set(value);

	//Flush();
}*/

void
vsRendererState::SetFloat2( vsRendererState::Float2 key, float valueA, float valueB )
{
	m_float2State[key]->Set(valueA, valueB);
}

void
vsRendererState::SetInt( vsRendererState::Int key, int value )
{
	m_intState[key]->Set(value);

	//Flush();
}

void
vsRendererState::Flush()
{
	for ( int i = 0; i < BOOL_COUNT; i++ )
	{
		CheckGLError("Flush");
		m_boolState[i]->Flush();
		CheckGLError("Flush");
	}
    for ( int i = 0; i < INT_COUNT; i++ )
    {
		CheckGLError("Flush");
        m_intState[i]->Flush();
		CheckGLError("Flush");
    }
	/*for ( int i = 0; i < FLOAT_COUNT; i++ )
	{
		CheckGLError("Flush");
		m_floatState[i]->Flush();
		CheckGLError("Flush");
	}*/
	for ( int i = 0; i < FLOAT2_COUNT; i++ )
	{
		CheckGLError("Flush");
		m_float2State[i]->Flush();
		CheckGLError("Flush");
	}
}


void
vsRendererState::Force()
{
	for ( int i = 0; i < BOOL_COUNT; i++ )
	{
		m_boolState[i]->Force();
	}
    for ( int i = 0; i < INT_COUNT; i++ )
    {
        m_intState[i]->Force();
    }
	/*for ( int i = 0; i < FLOAT_COUNT; i++ )
	{
		m_floatState[i]->Force();
	}*/
}


