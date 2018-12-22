/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
#ifndef _MODELINSTANCE_H_
#define _MODELINSTANCE_H_

#include <d3dx8.h>
#include <NVMesh.h>

class ModelInstance
{
	public :

		nv_objects::NVMesh* m_pNVMesh;

		D3DXMATRIX	mModelMatrix;

		LPDIRECT3DBASETEXTURE8 m_pOldTexture;

		ModelInstance( nv_objects::NVMesh* pMesh = 0, LPDIRECT3DBASETEXTURE8 pOldTexture = 0 ) : m_pNVMesh( pMesh ), m_pOldTexture( pOldTexture )
		{

		}

		ModelInstance& operator=( const ModelInstance& rhs ) 
		{
			m_pNVMesh = rhs.m_pNVMesh;
			mModelMatrix = rhs.mModelMatrix;
			m_pOldTexture = rhs.m_pOldTexture;
			return (*this);
		}

		ModelInstance( const ModelInstance& rhs )
		{
			(*this) = rhs;
		}

};


#endif  _MODELINSTANCE_H_

