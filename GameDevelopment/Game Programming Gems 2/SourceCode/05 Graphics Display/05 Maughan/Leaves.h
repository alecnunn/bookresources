/* Copyright (C) Chris Maughan, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Chris Maughan, 2001"
 */
#ifndef Leaves_INCLUDED
#define Leaves_INCLUDED

#include "Random.h"
#include "nvcomplexmesh.h"
#include "nvresourcemanager.h"

struct Leaves 
{
	Leaves(nv_objects::NVResourceID LeafFrontTextureID, nv_objects::NVResourceID LeafBackTextureID, int facets, float radius);

	void addLeaf(const D3DXMATRIX&);

	void Transfer(nv_objects::NVComplexMesh* pMesh);

	int getPolygonCount() const;
	int getLeafCount() const;

	nv_objects::NVResourceID m_LeafFrontTextureID;
	nv_objects::NVResourceID m_LeafBackTextureID;

	nv_objects::NVResourceID m_LeafFrontMaterialID;
	nv_objects::NVResourceID m_LeafBackMaterialID;

	D3DMATERIAL8 front_material;
	D3DMATERIAL8 back_material;

	float center_height;
	float radius;
	int facets;
	int count;

	std::vector<D3DXVECTOR3> vertices;
	std::vector<D3DXVECTOR3> normals;
	std::vector<D3DXVECTOR3> colors;
	std::vector<D3DXVECTOR2> texcoords;

	Random random;
	inline float rand() {
	return random * 0.5 + 0.5;
	}
};


#endif
