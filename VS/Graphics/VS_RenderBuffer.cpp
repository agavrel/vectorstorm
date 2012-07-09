/*
 *  VS_RenderBuffer.cpp
 *  Lord
 *
 *  Created by Trevor Powell on 5/01/09.
 *  Copyright 2009 Trevor Powell. All rights reserved.
 *
 */

#include "VS_RenderBuffer.h"

#include "VS_RendererState.h"

#include "VS_OpenGL.h"

static int s_glBufferType[vsRenderBuffer::TYPE_MAX] =
{
	0,
	GL_STATIC_DRAW,
	GL_DYNAMIC_DRAW,
#if TARGET_OS_IPHONE
	GL_DYNAMIC_DRAW
#else
	GL_STREAM_DRAW
#endif
};

vsRenderBuffer::vsRenderBuffer(vsRenderBuffer::Type type):
    m_array(NULL),
    m_arrayBytes(0),
    m_glArrayBytes(0),
    m_activeBytes(0),
    m_type(type),
    m_contentType(ContentType_Custom),
    m_bufferID(-1),
    m_vbo(false),
    m_indexType(false)
{
	vsAssert( sizeof( uint16 ) == 2, "I've gotten the size wrong??" );

	// TESTING:  Seems like iPhone runs SLOWER with VBOs than with arrays, so just use our vsRenderBuffer in "array" mode.
#if !TARGET_OS_IPHONE
	if ( glGenBuffers && m_type != Type_NoVBO )
	{
		glGenBuffers(1, (GLuint*)&m_bufferID);
		m_vbo = true;
	}
#endif
}

vsRenderBuffer::~vsRenderBuffer()
{
	if ( m_vbo )
	{
		glDeleteBuffers( 1, (GLuint*)&m_bufferID );
	}

	{
		vsDeleteArray( m_array );
	}
}

void
vsRenderBuffer::ResizeArray( int size )
{
    SetArraySize_Internal( size );
}

void
vsRenderBuffer::SetArraySize_Internal( int size )
{
	if ( m_array && size > m_arrayBytes )
	{
		vsDeleteArray( m_array );
	}
	if ( m_array == NULL )
	{
		m_array = new char[size];
		m_arrayBytes = size;
	}
    SetActiveSize(size);
}

void
vsRenderBuffer::SetArray_Internal( char *data, int size, bool elementArray )
{
	vsAssert( size, "Error:  Tried to set a zero-length GPU buffer!" );

	int bindPoint = (elementArray) ? GL_ELEMENT_ARRAY_BUFFER : GL_ARRAY_BUFFER;

	m_indexType = elementArray;

	if ( m_vbo )
	{
		glBindBuffer(bindPoint, m_bufferID);

		if ( size > m_glArrayBytes )
		{
			glBufferData(bindPoint, size, data, s_glBufferType[m_type]);
			m_glArrayBytes = size;
		}
		else
		{
			glBufferData(bindPoint, size, NULL, s_glBufferType[m_type]);
#if TARGET_OS_IPHONE
			char *ptr = (char *)glMapBufferOES(bindPoint, GL_WRITE_ONLY_OES);
#else
			char *ptr = (char *)glMapBufferARB(bindPoint, GL_WRITE_ONLY_ARB);
#endif

			if ( ptr )
			{
				memcpy(ptr, data, size);
				/*for ( int i = 0; i < size; i++ )
					ptr[i] = data[i];*/
#if TARGET_OS_IPHONE
				glUnmapBufferOES(bindPoint);
#else
				glUnmapBufferARB(bindPoint);
#endif
			}
		}

		glBindBuffer(bindPoint, 0);
	}

	if ( data != m_array )
	{
		SetArraySize_Internal( size );
		memcpy(m_array,data,size);
	}
}

void
vsRenderBuffer::SetArray( const vsRenderBuffer::PC *array, int size )
{
	m_contentType = ContentType_PC;

	SetArray_Internal((char *)array, size*sizeof(vsRenderBuffer::PC), false);
}

void
vsRenderBuffer::SetArray( const vsRenderBuffer::PT *array, int size )
{
	m_contentType = ContentType_PT;

	SetArray_Internal((char *)array, size*sizeof(vsRenderBuffer::PT), false);
}

void
vsRenderBuffer::SetArray( const vsRenderBuffer::PN *array, int size )
{
	m_contentType = ContentType_PN;

	SetArray_Internal((char *)array, size*sizeof(vsRenderBuffer::PN), false);
}

void
vsRenderBuffer::SetArray( const vsRenderBuffer::PCT *array, int size )
{
	m_contentType = ContentType_PCT;

	SetArray_Internal((char *)array, size*sizeof(vsRenderBuffer::PCT), false);
}

void
vsRenderBuffer::SetArray( const vsRenderBuffer::PNT *array, int size )
{
	m_contentType = ContentType_PNT;

	SetArray_Internal((char *)array, size*sizeof(vsRenderBuffer::PNT), false);
}

void
vsRenderBuffer::SetArray( const vsRenderBuffer::PCN *array, int size )
{
	m_contentType = ContentType_PCN;

	SetArray_Internal((char *)array, size*sizeof(vsRenderBuffer::PCN), false);
}

void
vsRenderBuffer::SetArray( const vsRenderBuffer::PCNT *array, int size )
{
	m_contentType = ContentType_PCNT;

	SetArray_Internal((char *)array, size*sizeof(vsRenderBuffer::PCNT), false);
}

void
vsRenderBuffer::SetArray( const vsVector3D *array, int size )
{
	SetArray_Internal((char *)array, size*sizeof(vsVector3D), false);
}

void
vsRenderBuffer::SetArray( const vsVector2D *array, int size )
{
	SetArray_Internal((char *)array, size*sizeof(vsVector2D), false);
}

void
vsRenderBuffer::SetArray( const vsColor *array, int size )
{
	SetArray_Internal((char *)array, size*sizeof(vsColor), false);
}

void
vsRenderBuffer::SetArray( const uint16 *array, int size )
{
	SetArray_Internal((char *)array, size*sizeof(uint16), true);
}

void
vsRenderBuffer::SetVector3DArraySize( int size )
{
	SetArraySize_Internal(size*sizeof(vsVector3D));
}

void
vsRenderBuffer::SetVector2DArraySize( int size )
{
	SetArraySize_Internal(size*sizeof(vsVector2D));
}

void
vsRenderBuffer::SetColorArraySize( int size )
{
	SetArraySize_Internal(size*sizeof(vsColor));
}

void
vsRenderBuffer::SetIntArraySize( int size )
{
	SetArraySize_Internal(size*sizeof(uint16));
}

void
vsRenderBuffer::BakeArray()
{
	SetArray_Internal( m_array, m_activeBytes, false );
}

void
vsRenderBuffer::BakeIndexArray()
{
	SetArray_Internal( m_array, m_activeBytes, true );
}


void
vsRenderBuffer::BindVertexBuffer( vsRendererState *state )
{
	state->SetBool( vsRendererState::ClientBool_VertexArray, true );

	if ( m_vbo )
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
		glVertexPointer( 3, GL_FLOAT, 0, 0 );
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	else
	{
		glVertexPointer( 3, GL_FLOAT, 0, m_array );
	}
}

void
vsRenderBuffer::UnbindVertexBuffer( vsRendererState *state )
{
	state->SetBool( vsRendererState::ClientBool_VertexArray, false );
}

void
vsRenderBuffer::BindNormalBuffer( vsRendererState *state )
{
	state->SetBool( vsRendererState::ClientBool_NormalArray, true );

	if ( m_vbo )
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
		glNormalPointer( GL_FLOAT, 0, 0 );
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	else
	{
		glNormalPointer( GL_FLOAT, 0, m_array );
	}
}

void
vsRenderBuffer::UnbindNormalBuffer( vsRendererState *state )
{
	state->SetBool( vsRendererState::ClientBool_NormalArray, false );
}

void
vsRenderBuffer::BindTexelBuffer( vsRendererState *state )
{
	state->SetBool( vsRendererState::ClientBool_TextureCoordinateArray, true );

	if ( m_vbo )
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
		glTexCoordPointer( 2, GL_FLOAT, 0, 0 );
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	else
	{
		glTexCoordPointer( 2, GL_FLOAT, 0, m_array );
	}
}

void
vsRenderBuffer::UnbindTexelBuffer( vsRendererState *state )
{
	state->SetBool( vsRendererState::ClientBool_TextureCoordinateArray, false );
}

void
vsRenderBuffer::BindColorBuffer( vsRendererState *state )
{
	state->SetBool( vsRendererState::ClientBool_ColorArray, true );

	if ( m_vbo )
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
		glColorPointer( 4, GL_FLOAT, 0, 0 );
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	else
	{
		glColorPointer( 4, GL_FLOAT, 0, m_array );
	}
}

void
vsRenderBuffer::UnbindColorBuffer( vsRendererState *state )
{
	state->SetBool( vsRendererState::ClientBool_ColorArray, false );
}

void
vsRenderBuffer::Bind( vsRendererState *state )
{
	switch( m_contentType )
	{
		case ContentType_PC:
		{
			PC dummyArray[2];
			int stride = ((size_t)&dummyArray[1] - (size_t)&dummyArray[0]);
			size_t cStart = ((size_t)&dummyArray[0].color.r - (size_t)&dummyArray[0].position.x);
			GLvoid* cStartPtr = (GLvoid*)cStart;

			state->SetBool( vsRendererState::ClientBool_VertexArray, true );
			state->SetBool( vsRendererState::ClientBool_ColorArray, true );

			if ( m_vbo )
			{
				glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);

				glVertexPointer( 3, GL_FLOAT, stride, 0 );
				glColorPointer( 4, GL_UNSIGNED_BYTE, stride, cStartPtr );
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}
			else
			{
				glVertexPointer( 3, GL_FLOAT, stride, m_array );
				glColorPointer( 4, GL_UNSIGNED_BYTE, stride, &((PC*)m_array)[0].color );
			}
			break;
		}
		case ContentType_PT:
		{
			PT dummyArray[2];
			int stride = ((size_t)&dummyArray[1] - (size_t)&dummyArray[0]);
			size_t tStart = ((size_t)&dummyArray[0].texel.x - (size_t)&dummyArray[0].position.x);
			GLvoid* tStartPtr = (GLvoid*)tStart;

			state->SetBool( vsRendererState::ClientBool_VertexArray, true );
			state->SetBool( vsRendererState::ClientBool_TextureCoordinateArray, true );

			if ( m_vbo )
			{
				glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);

				glVertexPointer( 3, GL_FLOAT, stride, 0 );
				glTexCoordPointer( 2, GL_FLOAT, stride, tStartPtr );
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}
			else
			{
				glVertexPointer( 3, GL_FLOAT, stride, m_array );
				glTexCoordPointer( 2, GL_FLOAT, stride, &((PT*)m_array)[0].texel );
			}
			break;
		}
		case ContentType_PN:
		{
			PN dummyArray[2];
			int stride = ((size_t)&dummyArray[1] - (size_t)&dummyArray[0]);
			size_t nStart = ((size_t)&dummyArray[0].normal.x - (size_t)&dummyArray[0].position.x);
			GLvoid* nStartPtr = (GLvoid*)nStart;

			state->SetBool( vsRendererState::ClientBool_VertexArray, true );
			state->SetBool( vsRendererState::ClientBool_NormalArray, true );

			if ( m_vbo )
			{
				glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);

				glVertexPointer( 3, GL_FLOAT, stride, 0 );
				glNormalPointer( GL_FLOAT, stride, nStartPtr );

				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}
			else
			{
				glVertexPointer( 3, GL_FLOAT, stride, m_array );
				glNormalPointer( GL_FLOAT, stride, &((PN*)m_array)[0].normal );
			}
			break;
		}
		case ContentType_PNT:
		{
			PNT dummyArray[2];
			int stride = ((size_t)&dummyArray[1] - (size_t)&dummyArray[0]);
			size_t nStart = ((size_t)&dummyArray[0].normal.x - (size_t)&dummyArray[0].position.x);
			size_t tStart = ((size_t)&dummyArray[0].texel.x - (size_t)&dummyArray[0].position.x);
			GLvoid* nStartPtr = (GLvoid*)nStart;
			GLvoid* tStartPtr = (GLvoid*)tStart;

			state->SetBool( vsRendererState::ClientBool_VertexArray, true );
			state->SetBool( vsRendererState::ClientBool_NormalArray, true );
			state->SetBool( vsRendererState::ClientBool_TextureCoordinateArray, true );

			if ( m_vbo )
			{
				glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);

				glVertexPointer( 3, GL_FLOAT, stride, 0 );
				glTexCoordPointer( 2, GL_FLOAT, stride, tStartPtr );
				glNormalPointer( GL_FLOAT, stride, nStartPtr );

				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}
			else
			{
				glVertexPointer( 3, GL_FLOAT, stride, m_array );
				glTexCoordPointer( 2, GL_FLOAT, stride, &((PNT*)m_array)[0].texel );
				glNormalPointer( GL_FLOAT, stride, &((PNT*)m_array)[0].normal );
			}
			break;
		}
		case ContentType_PCNT:
		{
			PCNT dummyArray[2];
			int stride = ((size_t)&dummyArray[1] - (size_t)&dummyArray[0]);
			size_t nStart = ((size_t)&dummyArray[0].normal.x - (size_t)&dummyArray[0].position.x);
			size_t tStart = ((size_t)&dummyArray[0].texel.x - (size_t)&dummyArray[0].position.x);
			size_t cStart = ((size_t)&dummyArray[0].color.r - (size_t)&dummyArray[0].position.x);
			GLvoid* cStartPtr = (GLvoid*)cStart;
			GLvoid* nStartPtr = (GLvoid*)nStart;
			GLvoid* tStartPtr = (GLvoid*)tStart;

			state->SetBool( vsRendererState::ClientBool_VertexArray, true );
			state->SetBool( vsRendererState::ClientBool_NormalArray, true );
			state->SetBool( vsRendererState::ClientBool_ColorArray, true );
			state->SetBool( vsRendererState::ClientBool_TextureCoordinateArray, true );

			if ( m_vbo )
			{
				glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);

				glVertexPointer( 3, GL_FLOAT, stride, 0 );
				glTexCoordPointer( 2, GL_FLOAT, stride, tStartPtr );
				glNormalPointer( GL_FLOAT, stride, nStartPtr );
				glColorPointer( 4, GL_UNSIGNED_BYTE, stride, cStartPtr );

				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}
			else
			{
				glVertexPointer( 3, GL_FLOAT, stride, m_array );
				glTexCoordPointer( 2, GL_FLOAT, stride, &((PCNT*)m_array)[0].texel );
				glNormalPointer( GL_FLOAT, stride, &((PCNT*)m_array)[0].normal );
				glColorPointer( 4, GL_UNSIGNED_BYTE, stride, &((PCNT*)m_array)[0].color );
			}
			break;
		}
		case ContentType_PCN:
		{
			PCN dummyArray[2];
			int stride = ((size_t)&dummyArray[1] - (size_t)&dummyArray[0]);
			size_t nStart = ((size_t)&dummyArray[0].normal.x - (size_t)&dummyArray[0].position.x);
			size_t cStart = ((size_t)&dummyArray[0].color.r - (size_t)&dummyArray[0].position.x);
			GLvoid* cStartPtr = (GLvoid*)cStart;
			GLvoid* nStartPtr = (GLvoid*)nStart;

			state->SetBool( vsRendererState::ClientBool_VertexArray, true );
			state->SetBool( vsRendererState::ClientBool_NormalArray, true );
			state->SetBool( vsRendererState::ClientBool_ColorArray, true );

			if ( m_vbo )
			{
				glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);

				glVertexPointer( 3, GL_FLOAT, stride, 0 );
				glNormalPointer( GL_FLOAT, stride, nStartPtr );
				glColorPointer( 4, GL_UNSIGNED_BYTE, stride, cStartPtr );

				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}
			else
			{
				glVertexPointer( 3, GL_FLOAT, stride, m_array );
				glNormalPointer( GL_FLOAT, stride, &((PCN*)m_array)[0].normal );
				glColorPointer( 4, GL_UNSIGNED_BYTE, stride, &((PCN*)m_array)[0].color );
			}
			break;
		}
		case ContentType_PCT:
		{
			PCT dummyArray[2];
			int stride = ((size_t)&dummyArray[1] - (size_t)&dummyArray[0]);
			size_t cStart = ((size_t)&dummyArray[0].color.r - (size_t)&dummyArray[0].position.x);
			size_t tStart = ((size_t)&dummyArray[0].texel.x - (size_t)&dummyArray[0].position.x);
			GLvoid* cStartPtr = (GLvoid*)cStart;
			GLvoid* tStartPtr = (GLvoid*)tStart;

			state->SetBool( vsRendererState::ClientBool_VertexArray, true );
			state->SetBool( vsRendererState::ClientBool_ColorArray, true );
			state->SetBool( vsRendererState::ClientBool_TextureCoordinateArray, true );

			if ( m_vbo )
			{
				glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);

				glVertexPointer( 3, GL_FLOAT, stride, 0 );
				glTexCoordPointer( 2, GL_FLOAT, stride, tStartPtr );
				glColorPointer( 4, GL_UNSIGNED_BYTE, stride, cStartPtr );

				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}
			else
			{
				glVertexPointer( 3, GL_FLOAT, stride, m_array );
				glTexCoordPointer( 2, GL_FLOAT, stride, &((PCT*)m_array)[0].texel );
				glColorPointer( 4, GL_UNSIGNED_BYTE, stride, &((PCT*)m_array)[0].color );
			}
			break;
		}
		default:
		{
			vsAssert(0, "Unknown content type!");
		}
	}
}

void
vsRenderBuffer::Unbind( vsRendererState *state )
{
	switch( m_contentType )
	{
        case ContentType_PC:
			state->SetBool( vsRendererState::ClientBool_VertexArray, false );
			state->SetBool( vsRendererState::ClientBool_ColorArray, false );
            break;
        case ContentType_PT:
			state->SetBool( vsRendererState::ClientBool_VertexArray, false );
			state->SetBool( vsRendererState::ClientBool_TextureCoordinateArray, false );
            break;
        case ContentType_PN:
			state->SetBool( vsRendererState::ClientBool_VertexArray, false );
			state->SetBool( vsRendererState::ClientBool_NormalArray, false );
            break;
        case ContentType_PNT:
			state->SetBool( vsRendererState::ClientBool_VertexArray, false );
			state->SetBool( vsRendererState::ClientBool_NormalArray, false );
			state->SetBool( vsRendererState::ClientBool_TextureCoordinateArray, false );
            break;
        case ContentType_PCN:
			state->SetBool( vsRendererState::ClientBool_VertexArray, false );
			state->SetBool( vsRendererState::ClientBool_NormalArray, false );
			state->SetBool( vsRendererState::ClientBool_ColorArray, false );
            break;
        case ContentType_PCT:
			state->SetBool( vsRendererState::ClientBool_VertexArray, false );
			state->SetBool( vsRendererState::ClientBool_ColorArray, false );
			state->SetBool( vsRendererState::ClientBool_TextureCoordinateArray, false );
            break;
        case ContentType_PCNT:
			state->SetBool( vsRendererState::ClientBool_VertexArray, false );
			state->SetBool( vsRendererState::ClientBool_NormalArray, false );
			state->SetBool( vsRendererState::ClientBool_ColorArray, false );
			state->SetBool( vsRendererState::ClientBool_TextureCoordinateArray, false );
            break;
        default:
            vsAssert(0, "Unknown content type!");
	}
}

int
vsRenderBuffer::GetPositionCount()
{
	switch( m_contentType )
	{
		case ContentType_PC:
			return m_arrayBytes / sizeof(PC);
			break;
		case ContentType_PT:
			return m_arrayBytes / sizeof(PT);
			break;
		case ContentType_PN:
			return m_arrayBytes / sizeof(PN);
			break;
		case ContentType_PNT:
			return m_arrayBytes / sizeof(PNT);
			break;
		case ContentType_PCN:
			return m_arrayBytes / sizeof(PCN);
			break;
		case ContentType_PCT:
			return m_arrayBytes / sizeof(PCT);
			break;
		case ContentType_PCNT:
			return m_arrayBytes / sizeof(PCNT);
			break;
        default:
            vsAssert(0, "Unknown content type!");
	}
	return 0;
}


vsVector3D
vsRenderBuffer::GetPosition(int i)
{
	vsAssert( i < GetPositionCount(), "Illegal buffer position request!" );

	switch( m_contentType )
	{
		case ContentType_PC:
		{
			PC* pc = (PC*)m_array;
			return pc[i].position;
		}
		case ContentType_PT:
		{
			PT* pt = (PT*)m_array;
			return pt[i].position;
		}
		case ContentType_PN:
		{
			PN* pn = (PN*)m_array;
			return pn[i].position;
		}
		case ContentType_PCN:
		{
			PCN* pnc = (PCN*)m_array;
			return pnc[i].position;
		}
		case ContentType_PNT:
		{
			PNT* pnt = (PNT*)m_array;
			return pnt[i].position;
		}
		case ContentType_PCNT:
		{
			PCNT* pnct = (PCNT*)m_array;
			return pnct[i].position;
		}
		default:
		{
			vsAssert(0, "Unhandled vsRenderBuffer content type!");
		}
	}
	return vsVector3D::Zero;

}


void
vsRenderBuffer::TriStripBuffer()
{
	if ( m_vbo )
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferID);
		//glDrawElements(GL_TRIANGLE_STRIP, m_activeBytes/sizeof(int), GL_UNSIGNED_INT, 0);
		glDrawElements(GL_TRIANGLE_STRIP, m_activeBytes/sizeof(uint16), GL_UNSIGNED_SHORT, 0 );
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else
	{
		glDrawElements(GL_TRIANGLE_STRIP, m_activeBytes/sizeof(uint16), GL_UNSIGNED_SHORT, m_array );
	}
}

void
vsRenderBuffer::TriListBuffer()
{
	if ( m_vbo )
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferID);
		glDrawElements(GL_TRIANGLES, m_activeBytes/sizeof(uint16), GL_UNSIGNED_SHORT, 0);
		//glDrawRangeElements(GL_TRIANGLES, 0, m_activeBytes/sizeof(uint16), m_activeBytes/sizeof(uint16), GL_UNSIGNED_SHORT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else
	{
		glDrawElements(GL_TRIANGLES, m_activeBytes/sizeof(uint16), GL_UNSIGNED_SHORT, m_array );
	}
}

void
vsRenderBuffer::TriFanBuffer()
{
	if ( m_vbo )
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferID);
		glDrawElements(GL_TRIANGLE_FAN, m_activeBytes/sizeof(uint16), GL_UNSIGNED_SHORT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else
	{
		glDrawElements(GL_TRIANGLE_FAN, m_activeBytes/sizeof(uint16), GL_UNSIGNED_SHORT, m_array );
	}
}

void
vsRenderBuffer::LineStripBuffer()
{
	if ( m_vbo )
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferID);
		glDrawElements(GL_LINE_STRIP, m_activeBytes/sizeof(uint16), GL_UNSIGNED_SHORT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else
	{
		glDrawElements(GL_LINE_STRIP, m_activeBytes/sizeof(uint16), GL_UNSIGNED_SHORT, m_array );
	}
}

void
vsRenderBuffer::LineListBuffer()
{
	if ( m_vbo )
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferID);
		glDrawElements(GL_LINES, m_activeBytes/sizeof(uint16), GL_UNSIGNED_SHORT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else
	{
		glDrawElements(GL_LINES, m_activeBytes/sizeof(uint16), GL_UNSIGNED_SHORT, m_array );
	}
}


void
vsRenderBuffer::UnmapAll()
{
	for ( vsRenderBuffer *b = vsRenderBuffer::GetFirstInstance(); b; b = b->GetNextInstance() )
	{
		if ( b->m_vbo )
		{
			glDeleteBuffers( 1, (GLuint*)&b->m_bufferID );
		}
		b->m_vbo = false;
		b->m_glArrayBytes = 0;
	}
}

void
vsRenderBuffer::MapAll()
{
#if !TARGET_OS_IPHONE
	for ( vsRenderBuffer *b = vsRenderBuffer::GetFirstInstance(); b; b = b->GetNextInstance() )
	{
		if ( glGenBuffers && b->m_type != vsRenderBuffer::Type_NoVBO )
		{
			glGenBuffers(1, (GLuint*)&b->m_bufferID);
			b->m_vbo = true;
		}
		if ( b->m_indexType )
		{
			b->BakeIndexArray();
		}
		else
		{
			b->BakeArray();
		}
	}
#endif
}

