/* Copyright (C) Jan Svarovsky, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Jan Svarovsky, 2000"
 */
#ifndef PMRENDER_H
#define PMRENDER_H

/*
The progressive mesh renderer.

To use:
Create a PMRMesh using the PMCMesh class in PMCreate.h

For each instance on-screen, create a PMRInstance.

Then whenever you want to change level of detail, call
instance->SetLOD(val) where val has been calculated by
your level-of-detail calculating function.

Alternatively you can step up and down the collapse list
by calling instance->CollapseOne() and instance->SplitOne()

To render, just step through each of the materials in the
instance:

If the system call is

void draw_triangle_list(Vertex *vertices, int num_vertices,
						   Index *indices, int num_indices)

call

for (int i = 0; i < instance->Mesh->NumMaterials; i++)
{
	set_material_state();

	draw_triangle_list(instance->Mesh->Materials[i].Vertices,
					   instance->Materials[i].NumVertices,
					   instance->Materials[i].Indices,
					   instance->Materials[i].NumIndices);
}
*/

// draw_triangle_list(instance->Mesh

// An index into the vertex list:
typedef unsigned short PMRIndex;

// A progressive mesh. You have one of these for each type of mesh in the game,
// but you need a separate PMRInstance for each instance of each PMRMesh on-screen.

// These PMRMesh'es are created by the PMCMesh class.
class PMRMesh
{
public:
	 PMRMesh();
	~PMRMesh();

	int NumMaterials, NumCollapses;
	int *AllIndexChanges; // All the index changes

	// The mesh is an array of materials, and an edge collapse list. The mesh
	// is saved in its lowest level of detail
	struct PMRMaterial     *Materials;
	struct PMREdgeCollapse *Collapses;
};

// A material owns vertices and triangles. This structure could also contain
// extra information like a pointer to a texture, a bumpmap, etc etc
struct PMRMaterial
{
	 PMRMaterial();
	~PMRMaterial();
	struct PMRVertex *Vertices;
	PMRIndex *Indices;
	int MaxNumVertices, MaxNumIndices;
	int MinNumVertices, MinNumIndices;
};

// This vertex structure can also contain any extra data you want such as
// texture coordinates, shininess, vertex normal, etc, etc.
struct PMRVertex
{
	vector3 Position;
};

// A single edge collapse / vertex split operation.
struct PMREdgeCollapse
{
	float Value;

	// This would be a pointer to a material, but it has to index
	// into PMRMesh's materials and PMRInstance's materials
	int Material;

	int NumIndicesToLose, NumIndicesToChange, NumVerticesToLose;
	int *IndexChanges;
	int CollapseTo;
};

// Every instance of the mesh in the game needs one of these structures. They
// encode the level of detail being used to render at the moment.
class PMRInstance
{
public:
	 PMRInstance(PMRMesh *mesh);
	~PMRInstance();

	class PMRMaterialInstance *Materials;
	struct PMREdgeCollapse     *CollapseUpTo;

	// The value you pass into this could be based on distance from camera, frame rate, etc
	void SetLOD(float value);

	// Or you can choose to collapse/split one step at a time. These return false if there was
	// no more collapsing/splitting to do.
	bool CollapseOne();
	bool    SplitOne();

	// A pointer to the original mesh. This is where you'll find the vertex data (which
	// is invariant between PMRInstance's)
	PMRMesh *Mesh;
};

// A material, these only contain triangles here because the vertices are in the
// original PMRMesh class.
class PMRMaterialInstance
{
public:
	 PMRMaterialInstance();
	~PMRMaterialInstance();

	PMRIndex *Indices;
	int NumIndices, NumVertices; // number of triangles/vertices to use at this moment.
};

#endif