/* Copyright (C) Jan Svarovsky, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Jan Svarovsky, 2000"
 */
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <memory.h>
#include "mtxlib.h"
#include "pmrender.h"

//////////////////////////////////////////////////////////////////////////////
//
// PMRMesh class. Created by PMCMesh'es

PMRMesh::PMRMesh()
{
	Materials = NULL;
	Collapses = NULL;
	AllIndexChanges = NULL;
}

PMRMesh::~PMRMesh()
{
	delete[] Materials;
	delete[] Collapses;
	delete[] AllIndexChanges;
}

//////////////////////////////////////////////////////////////////////////////
//
// a material. The renderer meshes are arrays of these.

PMRMaterial::PMRMaterial()
{
	Vertices = NULL;
	Indices  = NULL;
}

PMRMaterial::~PMRMaterial()
{
	delete[] Vertices;
	delete[] Indices ;
}

//////////////////////////////////////////////////////////////////////////////
//
// an instance of a mesh. Each version of each mesh that is rendered at a 
// separate level of detail needs one of these.

// Each instance is tied to the original mesh that it represents.
PMRInstance::PMRInstance(PMRMesh *mesh)
{
	Mesh = mesh;

	// And setup my instance-specific data.
	// We start with the lowest level of detail.
	Materials = new PMRMaterialInstance[Mesh->NumMaterials];

	for (int i = 0; i < Mesh->NumMaterials; i++)
	{
		Materials[i].Indices = new PMRIndex[Mesh->Materials[i].MaxNumIndices];
		assert(Materials[i].Indices);
		memcpy(Materials[i].Indices, Mesh->Materials[i].Indices, Mesh->Materials[i].MaxNumIndices * sizeof(Materials[i].Indices[0]));

		// lowest level of detail right now.
		Materials[i].NumVertices = Mesh->Materials[i].MinNumVertices;
		Materials[i].NumIndices  = Mesh->Materials[i].MinNumIndices ;
	}

	// So far, we're at the last collapse
	CollapseUpTo = Mesh->Collapses;
}

PMRInstance::~PMRInstance()
{
	delete[] Materials;
}

void PMRInstance::SetLOD(float value)
{
	// Keep collapsing/expanding until correct LOD.
	while (1)
	{
		// Bounds check.
		
		// If we're at the lowest detail
		if (CollapseUpTo == Mesh->Collapses)
		{
			// but we're being asked to lose a more significant edge, forget it
			if (value > CollapseUpTo->Value) return;
			else
			if (value < CollapseUpTo[0].Value)
			{
				// we want some detail back
				SplitOne();
			}
			else
			{
				// Nothing to do.
				break;
			}
		}

		// If we're at the highest detail
		if (CollapseUpTo == Mesh->Collapses + Mesh->NumCollapses)
		{
			// and being asked to gain a less significant edge, forget it
			if (value < CollapseUpTo[-1].Value) return;
			else
			if (value > CollapseUpTo[-1].Value)
			{
				// we're prepared to lose a certain value of edge.
				CollapseOne();
			}
			else
			{
				// Nothing to do.
				break;
			}
		}
		
		// Not at either end, can check either direction safely.
		if (value < CollapseUpTo[0].Value)
		{
			// we want some detail back
			SplitOne();
		}
		else
		if (value > CollapseUpTo[-1].Value)
		{
			// we're prepared to lose a certain value of edge.
			CollapseOne();
		}
		else
		{
			// Nothing to do.
			break;
		}
	}
}

bool PMRInstance::CollapseOne()
{
	if (CollapseUpTo <= Mesh->Collapses) return false;

	CollapseUpTo--;

	PMRMaterialInstance *mat = Materials + CollapseUpTo->Material;

	mat->NumIndices -= CollapseUpTo->NumIndicesToLose;
	
	for (int *i = CollapseUpTo->IndexChanges; i < CollapseUpTo->IndexChanges + CollapseUpTo->NumIndicesToChange; i++)
	{
		assert(mat->Indices[*i] == mat->NumVertices - 1);
		mat->Indices[*i] = CollapseUpTo->CollapseTo;
	}

	mat->NumVertices -= CollapseUpTo->NumVerticesToLose;

	return true;
}

bool PMRInstance::SplitOne()
{
	if (CollapseUpTo >= Mesh->Collapses + Mesh->NumCollapses) return false;

	PMRMaterialInstance *mat = Materials + CollapseUpTo->Material;

	mat->NumIndices  += CollapseUpTo->NumIndicesToLose;
	mat->NumVertices += CollapseUpTo->NumVerticesToLose;

	for (int *i = CollapseUpTo->IndexChanges; i < CollapseUpTo->IndexChanges + CollapseUpTo->NumIndicesToChange; i++)
	{
		assert(mat->Indices[*i] == CollapseUpTo->CollapseTo);
		mat->Indices[*i] = mat->NumVertices - 1;
	}

	CollapseUpTo++;

	return true;
}

//////////////////////////////////////////////////////////////////////////////
//
// each mesh instance only contains index data, the vertex data is invariant.

PMRMaterialInstance::PMRMaterialInstance()
{
	Indices = NULL;
}

PMRMaterialInstance::~PMRMaterialInstance()
{
	delete[] Indices;
}
