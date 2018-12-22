/* Copyright (C) Jan Svarovsky, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Jan Svarovsky, 2000"
 */
#ifndef PMCREATE_H
#define PMCREATE_H

//
// The interface to the progressive mesh off-line calculation
//

// To use:
//
// Create a PMCMesh and fill in its array of materials with your mesh data.
// Call CreateCollapseList() which will change these data and create internal
// data structures with all necessary information.
// Then you can call CreateRendererMesh() which will return you a progressive
// mesh you can render with.

#include "pmrender.h"

class PMCMesh
{
public:
	 PMCMesh();
	~PMCMesh();

	// Do the calculation
	void CreateCollapseList();

	// Then output a mesh for the renderer (this could be saved to disk)
	PMRMesh *CreateRendererMesh();

	// The mesh is an array of quite independent materials.
	int NumMaterials;
	class PMCMaterial *Materials;
	
private:
	// For internal use:
	
	// There is a list of all the index changes in all collapses
	int TotalIndexChanges;
	int *AllIndexChanges;

	// These are all the collapses, in reverse order (highest resolution mesh collapse first)
	struct PMCEdgeCollapse *Collapses, *CollapseUpTo;

	bool FindACollapse(float &val_so_far);
	void TryEdge(PMCMaterial *mat,
				 int pt_a, int pt_b,
				 float &be_val,
				 PMCMaterial *&be_mat,
				 int &be_a,
				 int &be_b);
	void Collapse(PMCMaterial *mat, PMRIndex pt_to, PMRIndex pt_from, float edge_val);
};

class PMCMaterial
{
public:
	 PMCMaterial();
	~PMCMaterial();

	// A material is an array of vertices and an array of triangles (which is an array of 3x as many indices)
	// Fill these in with your raw mesh data.
	struct PMRVertex *Vertices;
	PMRIndex *Indices;
	int NumVertices, NumIndices;

private:
	friend class PMCMesh;

	// After collapse, NumVertices will be the lowest detail level and OriginalNumVertices the original highest
	int OriginalNumVertices, OriginalNumIndices;

	// Internal to collapse code.
	void SwapToEnd(PMRIndex swapper, PMCEdgeCollapse *collapses, PMCEdgeCollapse *collapses_end, PMRIndex &pt_to, PMRIndex &pt_from);
};

#endif