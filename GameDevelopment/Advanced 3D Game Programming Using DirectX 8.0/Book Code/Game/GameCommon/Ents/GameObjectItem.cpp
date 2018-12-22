/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: GameObjectItem.cpp
 *    Desc: An item in the game
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#include "..\stdafx.h"
#include "GameObjectItem.h"

cGameObjectItem::cGameObjectItem() :
	cGameObject()
{

}

cGameObjectItem::~cGameObjectItem()
{

}

bool cGameObjectItem::IsObject( eObjectType objType )
{
	return ( objType = EOT_ITEM ) ? true : false; 
}

void cGameObjectItem::AddToScene()
{

}


void cGameObjectItem::RemoveFromScene()
{

}


void cGameObjectItem::Tick( float tickLength )
{

}


void cGameObjectItem::Draw()
{

}

void cGameObjectItem::RespondToCollision( cGameObject* pOther )
{
}
