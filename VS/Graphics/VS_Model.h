/*
 *  VS_Model.h
 *  VectorStorm
 *
 *  Created by Trevor Powell on 9/12/08.
 *  Copyright 2009 Trevor Powell. All rights reserved.
 *
 */

#ifndef VS_MODEL_H
#define VS_MODEL_H

#include "VS/Graphics/VS_Color.h"
#include "VS/Graphics/VS_DisplayList.h"
#include "VS/Graphics/VS_Entity.h"
#include "VS/Graphics/VS_Fragment.h"
#include "VS/Graphics/VS_Material.h"
#include "VS/Math/VS_Box.h"
#include "VS/Math/VS_Transform.h"
#include "VS/Utils/VS_LinkedList.h"
#include "VS/Utils/VS_LinkedListStore.h"

class vsModel;

struct vsModelInstance
{
private:
	vsModel *model;
	int index;       // our ID within the instance array.
	int matrixIndex; // our ID within the matrix array.
	friend class vsModel;
};

class vsModel : public vsEntity
{
	typedef vsEntity Parent;

	struct InstanceData
	{
		vsArray<vsMatrix4x4> matrix;
		vsArray<int> matrixInstanceId;
		vsArray<vsModelInstance*> instance;
	};

	vsMaterial *	m_material;

	vsBox3D				m_boundingBox;
	float				m_boundingRadius;

protected:

	vsDisplayList	*m_displayList;				// old-style rendering

	vsLinkedListStore<vsFragment>	m_fragment;	// new-style rendering

	// m_instanceMat and m_instance may not be in the same order.
	struct InstanceData *m_instanceData;

	vsTransform3D m_transform;

	void LoadFrom( vsRecord *record );

public:

	static vsModel *	Load( const vsString &filename );

	vsModel( vsDisplayList *displayList = NULL );
	virtual			~vsModel();

	vsModelInstance * MakeInstance();		// create an instance of me.
	void RemoveInstance( vsModelInstance *model );
	void			UpdateInstance( vsModelInstance *, const vsMatrix4x4& matrix, bool show = true ); // must be called to change the matrix on this instance

	void			SetMaterial( const vsString &name ) { vsDelete( m_material ); m_material = new vsMaterial(name); }
	void			SetMaterial( vsMaterial *material ) { vsDelete( m_material ); m_material = material; }
	vsMaterial *	GetMaterial() { return m_material; }

	virtual void		SetPosition( const vsVector3D &pos ) { m_transform.SetTranslation( pos ); }
	const vsVector3D &	GetPosition() { return m_transform.GetTranslation(); }

	virtual void			SetOrientation( const vsQuaternion &quat ) { m_transform.SetRotation( quat ); }
	const vsQuaternion &	GetOrientation() { return m_transform.GetRotation(); }

	const vsMatrix4x4 &		GetMatrix() { return m_transform.GetMatrix(); }

	const vsVector3D &		GetScale() { return m_transform.GetScale(); }
	void					SetScale( const vsVector3D &s ) { m_transform.SetScale(s); }
	void					SetScale( float s ) { m_transform.SetScale(s); }

	const vsBox3D &			GetBoundingBox() { return m_boundingBox; }
	void					SetBoundingBox(const vsBox3D &box) { m_boundingBox = box; }
	void					BuildBoundingBox();

	float					GetBoundingRadius() { return m_boundingRadius; }

	void				SetTransform( const vsTransform3D &t ) { m_transform = t; }
	const vsTransform3D&	GetTransform() { return m_transform; }

	void			SetDisplayList( vsDisplayList *list );
	void			AddFragment( vsFragment *fragment );
	void			ClearFragments();

	virtual void	Draw( vsRenderQueue *list );
};

#endif // VS_MODEL_H

