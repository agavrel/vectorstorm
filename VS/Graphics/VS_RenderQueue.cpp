/*
 *  VS_RenderQueue.cpp
 *  MMORPG2
 *
 *  Created by Trevor Powell on 5/09/10.
 *  Copyright 2010 Trevor Powell. All rights reserved.
 *
 */

#include "VS_RenderQueue.h"

#include "VS_Camera.h"
#include "VS_Fragment.h"
#include "VS_Scene.h"
#include "VS_System.h"

#include "VS_MaterialInternal.h"


vsRenderQueueStage::Batch::Batch():
	material("White"),
	elementList(NULL),
	next(NULL)
{
}

vsRenderQueueStage::Batch::~Batch()
{
}

vsRenderQueueStage::vsRenderQueueStage():
	m_batch(NULL),
	m_batchCount(0),
	m_batchPool(NULL),
	m_batchElementPool(NULL)
{
}

vsRenderQueueStage::~vsRenderQueueStage()
{
	while( m_batchPool )
	{
		Batch *b = m_batchPool;
		m_batchPool = m_batchPool->next;
		vsDelete(b);
	}
	while( m_batchElementPool )
	{
		BatchElement *e = m_batchElementPool;
		m_batchElementPool = m_batchElementPool->next;
		vsDelete(e);
	}
}

vsRenderQueueStage::Batch *
vsRenderQueueStage::FindBatch( vsMaterial *material )
{
	for (Batch *batch = m_batch; batch; batch = batch->next)
	{
		if ( batch->material == material )
		{
			return batch;
		}
	}

	if ( !m_batchPool )
	{
		m_batchPool = new Batch;
	}
	Batch *batch = m_batchPool;
	m_batchPool = batch->next;
	batch->next = NULL;
	batch->material = *material;

	// insert this batch into our batch list, SORTED.
	bool inserted = false;

	if ( m_batch == NULL )
	{
		m_batch = batch;
	}
	else
	{
		if ( m_batch->material.GetResource()->m_layer > material->GetResource()->m_layer )
		{
			// we should sort before the first thing in the batch list.
			batch->next = m_batch;
			m_batch = batch;
			inserted = true;
		}
		else
		{
			Batch *lb;
			for(lb = m_batch; lb->next; lb = lb->next)
			{
				if ( lb->next->material.GetResource()->m_layer > material->GetResource()->m_layer )
				{
					batch->next = lb->next;
					lb->next = batch;
					inserted = true;
					break;
				}
			}

			if ( !inserted )
			{
				lb->next = batch;
			}
		}
	}
	m_batchCount++;

	return batch;
}


void
vsRenderQueueStage::AddBatch( vsMaterial *material, const vsMatrix4x4 &matrix, vsDisplayList *batchList )
{
	Batch *batch = FindBatch(material);

	if ( !m_batchElementPool )
	{
		m_batchElementPool = new BatchElement;
		m_batchElementPool->next = NULL;
	}
	BatchElement *element = m_batchElementPool;
	m_batchElementPool = element->next;
	element->next = NULL;

	element->matrix = matrix;
	element->list = batchList;

	element->next = batch->elementList;
	batch->elementList = element;
}

vsDisplayList *
vsRenderQueueStage::MakeTemporaryBatchList( vsMaterial *material, const vsMatrix4x4 &matrix, int size )
{
	Batch *batch = FindBatch(material);

	if ( m_batchElementPool == NULL )
	{
		m_batchElementPool = new BatchElement;
		m_batchElementPool->next = NULL;
	}
	BatchElement *element = m_batchElementPool;
	m_batchElementPool = m_batchElementPool->next;
	element->next = NULL;

	element->matrix = matrix;
	element->list = new vsDisplayList(size);

	element->next = batch->elementList;
	batch->elementList = element;
	m_temporaryLists.AddItem(element->list);

	return element->list;
}

void
vsRenderQueueStage::StartRender()
{
	m_batchCount = 0;
	vsAssert( m_batch == NULL, "Batches not cleared?" );
	//	m_batch = NULL;


}

void
vsRenderQueueStage::Draw( vsDisplayList *list )
{
	for (Batch *b = m_batch; b; b = b->next)
	{
		list->SetMaterial( &b->material );

		for (BatchElement *e = b->elementList; e; e = e->next)
		{
			bool hasTransform = true;//e->matrix != vsMatrix4x4::Identity;

			if ( hasTransform )
			{
				list->SetMatrix4x4( e->matrix );
			}

			list->Append( *e->list );

			if ( hasTransform )
			{
				list->PopTransform();
			}
		}
	}
}

void
vsRenderQueueStage::EndRender()
{
	m_batchCount = 0;
	Batch *last = NULL;
	for (Batch *b = m_batch; b; b = b->next)
	{
		last = b;
		BatchElement *lastElement = b->elementList;
		while ( lastElement->next )
		{
			lastElement = lastElement->next;
		}
		lastElement->next = m_batchElementPool;
		m_batchElementPool = b->elementList;
		b->elementList = NULL;
	}
	if ( last )
	{
		last->next = m_batchPool;
		m_batchPool = m_batch;
	}
	m_batch = NULL;

	m_temporaryLists.Clear();
}

vsRenderQueue::vsRenderQueue( int stageCount, int genericListSize):
	m_parent(NULL),
	m_genericList(new vsDisplayList(genericListSize)),
	m_stage(new vsRenderQueueStage[stageCount]),
	m_stageCount(stageCount),
	m_transformStack(),
	m_transformStackLevel(0)
{
}

vsRenderQueue::~vsRenderQueue()
{
	vsDeleteArray( m_stage );
	vsDelete( m_genericList );
}

void
vsRenderQueue::StartRender(vsScene *parent)
{
	m_parent = parent;
	InitialiseTransformStack();

	for ( int i = 0; i < m_stageCount; i++ )
	{
		m_stage[i].StartRender();
	}
	m_genericList->Clear();

}

void
vsRenderQueue::Draw( vsDisplayList *list )
{
	for ( int i = 0; i < m_stageCount; i++ )
	{
		m_stage[i].Draw(list);
	}
	list->Append(*m_genericList);

	DeinitialiseTransformStack();
	vsAssert( m_transformStackLevel == 0, "Unbalanced push/pop of transforms?");
}

void
vsRenderQueue::EndRender()
{
	for ( int i = 0; i < m_stageCount; i++ )
	{
		m_stage[i].EndRender();
	}
	m_genericList->Clear();
}

void
vsRenderQueue::InitialiseTransformStack()
{
	if ( m_parent->Is3D() )
	{
		vsTransform3D startingTransform;
		switch( vsSystem::Instance()->GetOrientation() )
		{
			case Orientation_Normal:
				break;
			case Orientation_Six:
				startingTransform.SetRotation ( vsQuaternion( vsVector3D::ZAxis, DEGREES(180.f) ) );
				break;
			case Orientation_Three:
				startingTransform.SetRotation ( vsQuaternion( vsVector3D::ZAxis, DEGREES(270.f) ) );
				break;
			case Orientation_Nine:
				startingTransform.SetRotation ( vsQuaternion( vsVector3D::ZAxis, DEGREES(90.f) ) );
				break;
		}
		//
//		startingTransform.SetTranslation( vsVector3D(0.f, 0.0f, 0.f) );
		vsMatrix4x4 startingMatrix = startingTransform.GetMatrix();

		vsMatrix4x4 requestedMatrix = vsMatrix4x4::Identity;

		requestedMatrix.w -= m_parent->GetCamera3D()->GetPosition();

		vsMatrix4x4 myIdentity;
		myIdentity.x *= -1.f;

		vsMatrix4x4 cameraMatrix = m_parent->GetCamera3D()->GetTransform().GetMatrix();

		vsVector3D forward = cameraMatrix.z;
		vsVector3D up = cameraMatrix.y;
		vsVector3D side = forward.Cross(up);

		cameraMatrix.x = side;
		cameraMatrix.y = up;
		cameraMatrix.z = -forward;
		cameraMatrix.w.Set(0.f,0.f,0.f,1.f);
		cameraMatrix.Invert();

		cameraMatrix = startingMatrix * myIdentity * cameraMatrix;

		m_transformStack[0] = cameraMatrix * requestedMatrix;
		m_transformStackLevel = 1;
	}
	else
	{
		vsTransform3D startingTransform;
		switch( vsSystem::Instance()->GetOrientation() )
		{
			case Orientation_Normal:
				break;
			case Orientation_Six:
				startingTransform.SetRotation ( vsQuaternion( vsVector3D::ZAxis, DEGREES(180.f) ) );
				break;
			case Orientation_Three:
				startingTransform.SetRotation ( vsQuaternion( vsVector3D::ZAxis, DEGREES(90.f) ) );
				break;
			case Orientation_Nine:
				startingTransform.SetRotation ( vsQuaternion( vsVector3D::ZAxis, DEGREES(270.f) ) );
				break;
		}
		//
		//		startingTransform.SetTranslation( vsVector3D(0.f, 0.0f, 0.f) );
		vsMatrix4x4 startingMatrix = startingTransform.GetMatrix();
		vsTransform2D cameraTransform = m_parent->GetCamera()->GetCameraTransform();
		// cameraMatrix will have a scale on its members from the camera. (Since
		// that's where it stores the FOV).
		// We remove that, since that eventually becomes part of the PROJECTION
		// transform, not the MODELVIEW transform, which is all we care about here..
		cameraTransform.SetScale(vsVector2D(1.f,1.f));
		vsMatrix4x4 cameraMatrix = cameraTransform.GetMatrix();

		m_transformStack[0] = cameraMatrix * startingMatrix;
		m_transformStackLevel = 1;
	}
}

void
vsRenderQueue::DeinitialiseTransformStack()
{
	if ( m_parent->Is3D() )
	{
		m_transformStackLevel--;
	}
	else
	{
		m_transformStackLevel--;
	}
}

void
vsRenderQueue::AddBatch( vsMaterial *material, const vsMatrix4x4 &matrix, vsDisplayList *batch )
{
	int stageId = PickStageForMaterial( material );

	m_stage[stageId].AddBatch( material, matrix, batch );
}

void
vsRenderQueue::AddFragmentBatch( vsFragment *fragment )
{
	AddBatch( fragment->GetMaterial(), GetMatrix(), fragment->GetDisplayList() );
}

vsDisplayList *
vsRenderQueue::MakeTemporaryBatchList( vsMaterial *material, const vsMatrix4x4 &matrix, int size )
{
	int stageId = PickStageForMaterial( material );

	return m_stage[stageId].MakeTemporaryBatchList( material, matrix, size );
}

vsDisplayList *
vsRenderQueue::MakeTemporaryBatchList( vsMaterial *material, int size )
{
	int stageId = PickStageForMaterial( material );

	return m_stage[stageId].MakeTemporaryBatchList( material, m_transformStack[0], size );
}

vsRenderQueueStage *
vsRenderQueue::GetStage( int i )
{
	vsAssert( i >= 0 && i < m_stageCount, "Requested nonexistant render stage!" );

	if ( i >= 0 && i < m_stageCount )
	{
		return &m_stage[i];
	}

	return NULL;
}


vsMatrix4x4
vsRenderQueue::PushMatrix( const vsMatrix4x4 &matrix )
{
	vsAssert( m_transformStackLevel < MAX_STACK_DEPTH, "Transform stack overflow!" )
	vsAssert( m_transformStackLevel > 0, "Uninitialised transform stack??" )

	if ( m_transformStackLevel < MAX_SCENE_STACK )
	{
		m_transformStack[m_transformStackLevel] = m_transformStack[m_transformStackLevel-1] * matrix;
		m_transformStackLevel++;
	}

	return m_transformStack[ m_transformStackLevel-1 ];
}

vsMatrix4x4
vsRenderQueue::PushTransform2D( const vsTransform2D &transform )
{
	vsAssert( m_transformStackLevel < MAX_STACK_DEPTH, "Transform stack overflow!" )
	vsAssert( m_transformStackLevel > 0, "Uninitialised transform stack??" )

	if ( m_transformStackLevel < MAX_SCENE_STACK )
	{
		vsMatrix4x4 matrix = transform.GetMatrix();
		// matrix.SetTranslation( -1.f * matrix.w );
		m_transformStack[m_transformStackLevel] = m_transformStack[m_transformStackLevel-1] * matrix;
		m_transformStackLevel++;
	}

	return m_transformStack[ m_transformStackLevel-1 ];
}

vsMatrix4x4
vsRenderQueue::PushTranslation( const vsVector3D &t )
{
	vsMatrix4x4 mat;
	mat.SetTranslation(t);

	return PushMatrix( mat );
}

void
vsRenderQueue::PopMatrix()
{
	vsAssert( m_transformStackLevel > 0, "Transform stack underflow!" );
	m_transformStackLevel--;
}

const vsMatrix4x4&
vsRenderQueue::GetMatrix()
{
	vsAssert( m_transformStackLevel > 0, "Nothing in the transform stack!" );

	return m_transformStack[m_transformStackLevel-1];
}

const vsMatrix4x4&
vsRenderQueue::GetTopMatrix()
{
	vsAssert( m_transformStackLevel > 0, "Nothing in the transform stack!" );

	return m_transformStack[0];
}

int
vsRenderQueue::PickStageForMaterial( vsMaterial *material )
{
	if ( material->GetResource()->m_postGlow )
	{
		return 2;
	}
	else if ( material->GetResource()->m_glow )
	{
		return 1;
	}
	return 0;
}



