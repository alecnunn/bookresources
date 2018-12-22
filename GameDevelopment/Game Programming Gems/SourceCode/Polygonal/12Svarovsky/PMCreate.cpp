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
#include "pmcreate.h"

// Maximum buffer sizes. For greater flexibility, the buffers should reallocate dynamically.
#define MAX_COLLAPSES 2000
#define MAX_INDEX_CHANGES 10000
#define MAX_VERTICES_PER_MATERIAL 10000

//////////////////////////////////////////////////////////////////////////////
//
// An edge collapse. No-one but this module needs to see this.

struct PMCEdgeCollapse
{
	float Value;
	PMCMaterial *Material;
	int NumTrianglesToLose, NumIndicesToChange, NumVerticesToLose;
	int *IndexChanges;
	int CollapseTo;
};

//////////////////////////////////////////////////////////////////////////////
//
// Helper functions

// swap two variables
template <class T> static void swap(T &a, T &b)
{
	T t;

	t = a;
	a = b;
	b = t;
}

// swap whole triangles
static void swap_triangle(PMRIndex *t1, PMRIndex *t2)
{
	swap(t1[0], t2[0]);
	swap(t1[1], t2[1]);
	swap(t1[2], t2[2]);
}

// add the cost of a triangle being modified (ie one of its vertices being changed) into the accumulator "sofar".
// This is the meat of the edge evaluation function.
static void combine_modified(float &sofar, PMCMaterial *mat, PMRIndex *tri, int which, int what_to)
{
	// These are the corners of the triangle at the moment.
	vector3 pts[3] =
	{
		mat->Vertices[tri[0]].Position,
		mat->Vertices[tri[1]].Position,
		mat->Vertices[tri[2]].Position
	};

	// This is a point in the plane of the triangle.
	vector3 in_plane = pts[0];

	// Calculate the area and face normal for the triangle at the moment.
	vector3 oldcross = CrossProduct(pts[2] - pts[0], pts[1] - pts[0]);
	float oldarea = oldcross.length();
	vector3 oldnorm = oldcross;
	oldnorm = oldnorm.normalize();

	// Change the triangle.
	pts[which] = mat->Vertices[what_to].Position;

	// Re-calculate the area and face normal.
	vector3 newcross = CrossProduct(pts[2] - pts[0], pts[1] - pts[0]);
	float newarea = newcross.length();
	vector3 newnorm = newcross;
	newnorm = newnorm.normalize();

	// A measure of the difference in the face normals.
	float cosangdiff = DotProduct(newnorm, oldnorm);

	// Calculate some statistics about the triangle change.
	float volume_change = fabsf(DotProduct(mat->Vertices[what_to].Position - in_plane, oldcross));

	// The angle change weighted by the area of the triangle.
	float weighted_angle_change = (1.f - cosangdiff) * (oldarea + newarea);

	// These numbers are not in the same "units", one being length^3 and the other length^2
	// And let's put some arbitrary weighting on these things.
	        volume_change = powf(        volume_change, .333333f);
	weighted_angle_change = powf(weighted_angle_change, .5f     ) * 5.f;

	// Work out a cost for the changing of this triangle
	float newval = weighted_angle_change + volume_change;

	// And factor it in. Here I choose to accumulate the maximum cost.
	if (newval > sofar) sofar = newval;
}

//////////////////////////////////////////////////////////////////////////////
//
// PMCMaterial methods

PMCMaterial::PMCMaterial()
{
	Vertices = NULL;
	Indices = NULL;
};

PMCMaterial::~PMCMaterial()
{
	delete[] Vertices;
	delete[] Indices ;
}

// swap "swapper" to the end of the material, updating all references to the vertices being swapped
void PMCMaterial::SwapToEnd(PMRIndex swapper, PMCEdgeCollapse *collapses, PMCEdgeCollapse *collapses_end, PMRIndex &pt_to, PMRIndex &pt_from)
{
	// NOTE: Here you may want to call back into your animation system (for example), so that it knows that
	// the vertex list is being reordered.

	// callback(this, swapper, NumVertices - 1); - tell someone that the vertices are being swapped.

	// pointer to the vertex
	PMRVertex *v = &Vertices[swapper];

	// actually swap the vertex data
	swap(*v, Vertices[NumVertices - 1]);

	// Then we have to find all references to the swapped vertices, and swap them too.

	// There's references in the triangles still uncollapsed, and the collapsed ones. So run through all
	// the triangles
	int tri_index;
	for (tri_index = 0; tri_index < OriginalNumIndices; tri_index++)
	{
		// Swap any references around
		if (Indices[tri_index] == swapper) 
		{
			Indices[tri_index] = NumVertices - 1;
		}
		else
		if (Indices[tri_index] == NumVertices - 1)
		{
			Indices[tri_index] = swapper;
		}
	}

	// There's references in all the edge collapses for this material.
	for (PMCEdgeCollapse *c = collapses; c < collapses_end; c++)
	{
		// must be a collapse in this material.
		if (c->Material != this) continue;

		if (c->CollapseTo == swapper)
		{
			c->CollapseTo = NumVertices - 1;
		}
		else
		if (c->CollapseTo == NumVertices - 1)
		{
			c->CollapseTo = swapper;
		}
	}

	// and the points on the collapsing edge themselves may have been moved
	if (pt_to == NumVertices - 1)
	{
		pt_to = swapper;
	}
	else
	if (pt_to == swapper)
	{
		pt_to = NumVertices - 1;
	}

	if (pt_from == NumVertices - 1)
	{
		pt_from = swapper;
	}
	else
	if (pt_from == swapper)
	{
		pt_from = NumVertices - 1;
	}
}

//////////////////////////////////////////////////////////////////////////////
//
// PMCMesh methods

PMCMesh::PMCMesh()
{
	Collapses = NULL;
	Materials = NULL;
	CollapseUpTo = 0;
	NumMaterials = 0;
	AllIndexChanges = NULL;
	TotalIndexChanges = 0;
}

PMCMesh::~PMCMesh()
{
	delete[] Collapses;
	delete[] Materials;
	delete[] AllIndexChanges;
}

// collapse pt_from onto pt_to, adding the collapse to the collapse list
void PMCMesh::Collapse(PMCMaterial *mat, PMRIndex pt_to, PMRIndex pt_from, float edge_val)
{
	// Swap the collapsing vertex to the end of the vertex list.
	mat->SwapToEnd(pt_from, Collapses, CollapseUpTo, pt_to, pt_from);

	// Now we do the collapse.
	mat->NumVertices--;

	// Run through uncollapsed triangles to find ones that
	// refer to this vertex, and count up the ones that disappear.
	CollapseUpTo->NumTrianglesToLose = 0;
	CollapseUpTo->IndexChanges = NULL;
	CollapseUpTo->NumIndicesToChange = 0;

	// step through the triangle list
	int tri_index;
	for (tri_index = 0; tri_index < mat->NumIndices;)
	{
		// step through each point in the triangle
		for (int pt = 0; pt < 3; pt++)
		{
			// Is this the collapsing vertex?
			if (mat->Indices[tri_index + pt] == mat->NumVertices)
			{
				// Yes, but will this triangle be degenerate? Must look for
				// the other end of the collapse edge
				int pt2;

				// look for the other point
				for (pt2 = 0; pt2 < 3; pt2++)
				{
					if (mat->Indices[tri_index + pt2] == pt_to)
					{
						// This triangle contains the collapsing edge.
						
						// It goes on the end of the list, unless it's already there.
						CollapseUpTo->NumTrianglesToLose++;
						mat->NumIndices -= 3;

						if (tri_index == mat->NumIndices)
						{
							// This triangle is the end of the list. We're done here,
							// and we've got no more triangles to look at (because it's
							// the end of the list)
							goto done_triangle_list;
						}

						// Swap the triangles.
						swap_triangle(mat->Indices + tri_index, mat->Indices + mat->NumIndices);

						// And any references to them (in other collapses)
						for (PMCEdgeCollapse *c = Collapses; c < CollapseUpTo; c++)
						{
							// must be a collapse in this material.
							if (c->Material != mat) continue;

							// Look through the index changes.
							int *ic;

							for (ic = c->IndexChanges;
								 ic < c->IndexChanges + c->NumIndicesToChange;
								 ic++)
							{
								if (*ic >= tri_index && *ic < tri_index + 3)
								{
									// This index change refers to the triangle that's
									// been moved!
									*ic += mat->NumIndices - tri_index;
								}
								else
								if (*ic >= mat->NumIndices && *ic < mat->NumIndices + 3)
								{
									// This index change refers to the other triangle
									*ic -= mat->NumIndices - tri_index;
								}
							}
						}

						// So, this triangle became redundant. Try the next triangle,
						// but don't increment tri_index.
						goto try_same_triangle_location;
					}
				}

				// This triangle isn't degenerate if we've got here. But it does
				// have a reference to the collapsing vertex. This is fine - it can
				// now just refer to the non-collapsing vertex.

				// We want to record the index change. First, do we have a buffer of them?
				if (!CollapseUpTo->IndexChanges)
				{
					// We haven't recorded an index change yet.
					CollapseUpTo->IndexChanges = AllIndexChanges + TotalIndexChanges;
				}

				// Add another index change on.
				assert(TotalIndexChanges < MAX_INDEX_CHANGES && "You must increase MAX_INDEX_CHANGES");

				AllIndexChanges[TotalIndexChanges++] = tri_index + pt;
				CollapseUpTo->NumIndicesToChange++;

				// And actually do the change.
				mat->Indices[tri_index + pt] = pt_to;

				// Definately no more indices to change in this triangle.
				goto try_another_triangle;
			}
		}

try_another_triangle:
		// step on to the next triangle
		tri_index += 3;

try_same_triangle_location:;
		// You can jump to here if the next triangle is still in the same
		// place because triangles have been moved around.
	}

done_triangle_list:

	// So, the triangle list has been edited to account for the collapse.

	// Were any other vertices made redundant by the loss of these triangles?
	CollapseUpTo->NumVerticesToLose = 1; // That's the collapser itself, of course.

	// Flag all vertices to see if they're referred to by triangles any more.
	int referred[MAX_VERTICES_PER_MATERIAL];
	memset(referred, 0, sizeof(referred));
	assert(mat->NumVertices < MAX_VERTICES_PER_MATERIAL && "You must increase MAX_VERTICES_PER_MATERIAL");

	int i;
	for (i = 0; i < mat->NumIndices; i++)
	{
		assert(mat->Indices[i] < mat->NumVertices);
		referred[mat->Indices[i]] = 1;
	}

	// If not, let's lose them.
	for (i = 0; i < mat->NumVertices;)
	{
		if (!referred[i])
		{
			// We can swap this vertex with the one at the end of the list.
			mat->SwapToEnd(i, Collapses, CollapseUpTo + 1, pt_to, pt_from);

			// And better swap its "referred" status.
			swap(referred[i], referred[mat->NumVertices - 1]);

			// One more vertex lost.
			CollapseUpTo->NumVerticesToLose++;
			mat->NumVertices--;

			// And don't increment i
			continue;
		}

		i++;
	}

	// Fill in the remaining collapse data.
	CollapseUpTo->CollapseTo = pt_to;
	CollapseUpTo->Value = edge_val;
	CollapseUpTo->Material = mat;
	CollapseUpTo++;
}

// Evaluate the cost of an edge, and if it's better than the best edge so far, record it
void PMCMesh::TryEdge(PMCMaterial *mat,
					 int pt_a, int pt_b,
					 float &be_val,
					 PMCMaterial *&be_mat,
					 int &be_to,
					 int &be_from)
{
	// There's actually two edge modifications being considered here:
	// - collapse of pt_a onto pt_b (a_loss)
	// - collapse of pt_b onto pt_a (b_loss)

	// Start with both costing very little.
	float a_loss = -1000000000000000.f;
	float b_loss = -1000000000000000.f;

	// Look for all triangles affected by the collapse
	for (PMRIndex *tri = mat->Indices; tri < mat->Indices + mat->NumIndices; tri += 3)
	{
		if (tri[0] == pt_a)
		{
			if (tri[1] != pt_b && tri[2] != pt_b) combine_modified(a_loss, mat, tri, 0, pt_b);
			else
			{
				// tri collapses.
				continue;
			}
		}
		else
		if (tri[1] == pt_a)
		{
			if (tri[2] != pt_b && tri[0] != pt_b) combine_modified(a_loss, mat, tri, 1, pt_b);
			else
			{
				// tri collapses.
				continue;
			}
		}
		else
		if (tri[2] == pt_a)
		{
			if (tri[0] != pt_b && tri[1] != pt_b) combine_modified(a_loss, mat, tri, 2, pt_b);
			else
			{
				// tri collapses.
				continue;
			}
		}
		else
		{
			// no points are a's
			if (tri[0] == pt_b) combine_modified(b_loss, mat, tri, 0, pt_a);
			else
			if (tri[1] == pt_b) combine_modified(b_loss, mat, tri, 1, pt_a);
			else
			if (tri[2] == pt_b) combine_modified(b_loss, mat, tri, 2, pt_a);
		}
	}

	// NOTE: What you should do here is check for edges of the object, where
	// the continuity of the mesh is broken. This example code will currently
	// actually collapse these preferentially! Bad idea.

	// We want to lose the point that costs the least.

	// Make sure it's point B that is the least cost by swapping if necessary.
	if (b_loss > a_loss)
	{
		swap(pt_a, pt_b);
		b_loss = a_loss;
	}

	// And therefore, the cost of this edge is the cost of losing point B.
	float val = b_loss;

	// Is it a better candidate than what we've had so far?
	if (val < be_val)
	{
		be_to = pt_a;
		be_from = pt_b;
		be_val = val;
		be_mat = mat;
	}
}

// Find a collapse to do, and do it. Returns false if there are no more collapses to do.
bool PMCMesh::FindACollapse(float &val_so_far)
{
	// First find the best edge to collapse in any material.
	PMCMaterial *mat;
	PMRIndex *tri;

	float be_val = 10000000000000000000000000000000000.f; // start with a big number
	PMCMaterial *be_mat = NULL;

	// The two ends of the edge.
	int be_index_a;
	int be_index_b;

	for (mat = Materials; mat < Materials + NumMaterials; mat++)
	{
		for (tri = mat->Indices; tri < mat->Indices + mat->NumIndices; tri += 3)
		{
			// Try each of the three edges of this triangle.
			TryEdge(mat, tri[0], tri[1], be_val, be_mat, be_index_a, be_index_b);
			TryEdge(mat, tri[1], tri[2], be_val, be_mat, be_index_a, be_index_b);
			TryEdge(mat, tri[2], tri[0], be_val, be_mat, be_index_a, be_index_b);
		}
	}

	// Was there a best edge? If not, collapsing is over.
	if (!be_mat) return false;

	// The value (cost) of the mesh is the value so far plus the cost of the new edge.
	val_so_far += be_val;

	// Do the collapse
	Collapse(be_mat, be_index_a, be_index_b, val_so_far);

	return true;
}

void PMCMesh::CreateCollapseList()
{
	// Initialise
	for (int i = 0; i < NumMaterials; i++)
	{
		Materials[i].OriginalNumVertices = Materials[i].NumVertices;
		Materials[i].OriginalNumIndices  = Materials[i].NumIndices ;
	}

	// make sure it hasn't been done before
	assert(!Collapses);

	Collapses = new PMCEdgeCollapse[MAX_COLLAPSES];
	assert(Collapses); // note you should really have code to handle failure of malloc
	CollapseUpTo = Collapses;

	PMCEdgeCollapse *CollapseUpTo = Collapses;

	AllIndexChanges = new int[MAX_INDEX_CHANGES];
	assert(AllIndexChanges);
	TotalIndexChanges = 0;

	// Keep collapsing until none left to do.
	float start_val = 0.f;
	while (FindACollapse(start_val))
	{
	}
}

// Converts the mesh into one that can be used by the renderer
PMRMesh *PMCMesh::CreateRendererMesh()
{
	PMRMesh *retval = new PMRMesh;

	retval->AllIndexChanges = new int[TotalIndexChanges];
	assert(retval->AllIndexChanges); // again, you should check for malloc failure
	memcpy(retval->AllIndexChanges, AllIndexChanges, TotalIndexChanges * sizeof(AllIndexChanges[0]));

	retval->Collapses = new PMREdgeCollapse[CollapseUpTo - Collapses];
	assert(retval->Collapses);

	retval->Materials = new PMRMaterial[NumMaterials];
	assert(retval->Materials);

	retval->NumCollapses = CollapseUpTo - Collapses;
	retval->NumMaterials = NumMaterials;

	// Copy the collapses in reverse order, so that the lowest detail collapses are first
	int i;

	for (i = 0; i < retval->NumCollapses; i++)
	{
		PMCEdgeCollapse &src  = Collapses[retval->NumCollapses - i - 1];
		PMREdgeCollapse &dest = retval->Collapses[i];

		dest.CollapseTo = src.CollapseTo;
		dest.IndexChanges = retval->AllIndexChanges + (src.IndexChanges - AllIndexChanges);
		dest.Material = src.Material - Materials;
		dest.NumIndicesToChange = src.NumIndicesToChange;
		dest.NumIndicesToLose = src.NumTrianglesToLose * 3;
		dest.NumVerticesToLose = src.NumVerticesToLose;
		dest.Value = src.Value;
	}

	// And copy the materials as they are.
	for (i = 0; i < NumMaterials; i++)
	{
		PMCMaterial &src  =         Materials[i];
		PMRMaterial &dest = retval->Materials[i];

		dest.MaxNumIndices  = src.OriginalNumIndices ;
		dest.MaxNumVertices = src.OriginalNumVertices;
		dest.MinNumIndices  = src.        NumIndices ;
		dest.MinNumVertices = src.        NumVertices;

		dest.Indices = new PMRIndex[dest.MaxNumIndices];
		assert(dest.Indices);
		dest.Vertices = new PMRVertex[dest.MaxNumVertices];
		assert(dest.Vertices);

		// The indices can be a straight copy.
		memcpy(dest.Indices, src.Indices, dest.MaxNumIndices * sizeof(dest.Indices[0]));

		// The vertices however must be copied by hand, just in case there's extra data in the
		// PMCVertices (if you have a more complex edge choice function
		for (int j = 0; j < dest.MaxNumVertices; j++)
		{
			dest.Vertices[j] = src.Vertices[j];
		}
	}

	return retval;
}