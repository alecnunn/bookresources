/* Copyright (C) Chris Maughan, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Chris Maughan, 2001"
 */
#ifndef Branches_INCLUDED
#define Branches_INCLUDED

#include <vector>
#include "nvcomplexmesh.h"
#include "nvresourcemanager.h"

struct Branches 
{
	Branches(nv_objects::NVResourceID BranchTextureID, int facets);
	~Branches();

	int add(const D3DXVECTOR3& n, const D3DXVECTOR2& tc, const D3DXVECTOR3& v);
	void addIndex(int);

	int getPolygonCount() const;
	int getBranchCount() const;

	void Transfer(nv_objects::NVComplexMesh* pMesh);

	std::vector<D3DXVECTOR3> vertices;
	std::vector<D3DXVECTOR3> normals;
	std::vector<D3DXVECTOR2> texcoords;

	std::vector<WORD> indecies;

	D3DMATERIAL8 material;
	nv_objects::NVResourceID m_BranchTextureID;

	int facets;
	D3DXVECTOR2* geom;
};

#endif



