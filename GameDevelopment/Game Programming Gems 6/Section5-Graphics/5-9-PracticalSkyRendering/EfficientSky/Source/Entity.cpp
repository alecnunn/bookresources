// Entity.cpp: A Scene Entity Class.
// Date: 08/02/05
// Created by: Aurelio Reis

#include "dxstdafx.h"

#include "../Include/Entity.h"
#include "../Include/SceneManager.h"



// Constructor.
CEntity::CEntity()
{
	Reset();
}

// Pure Virtual Destructor.
CEntity::~CEntity()
{
	// Remove from the scene manager.
	CSceneManager::m_pInstance->RemoveEntity( this );

	// Reset to default states.
	Reset();
}

// Reset to default values.
void CEntity::Reset()
{
	m_ulSceneViewMask = 0;
	D3DXMatrixIdentity( &m_matTransform );
	m_pModel = NULL;
	m_bVisible = true;
	m_bViewHack = false;
}

// Release the Entity.
void CEntity::Release()
{
	Reset();
}