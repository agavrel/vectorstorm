/*
 *  VS_MaterialManager.cpp
 *  MMORPG2
 *
 *  Created by Trevor Powell on 23/05/09.
 *  Copyright 2009 Trevor Powell. All rights reserved.
 *
 */

#include "VS_MaterialManager.h"

vsMaterialManager::vsMaterialManager():
vsCache<vsMaterialInternal>(512)
{
	if ( vsMaterial::White == NULL )
	{
		vsMaterial::White = new vsMaterial("White");
	}
}

vsMaterialManager::~vsMaterialManager()
{
	vsDelete( vsMaterial::White );
}

vsMaterialInternal *	
vsMaterialManager::LoadMaterial( const vsString &name )
{
	return Get( name );
}