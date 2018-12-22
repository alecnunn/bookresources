/* Copyright (C) Dan Ginsburg, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Ginsburg, 2000"
 */
////////////////////////////////////////////////////////////////////////////////////////
//	
//	Octree.cpp
//
//	Author: Dan Ginsburg (ginsburg@alum.wpi.edu)
//
//	Description:
//
//		Supplement to Game Programming Gems article: "Octree Construction"
//
//		This code provides a class for taking a list of triangles and
//		partitioning them into an Octree.  
//
#include <stdio.h>
#include <stdlib.h>
#include "Octree.h"

///
//	Macros
//
#define MAX_VAL	((float)(unsigned int)(1 << 31))
#define ABS(x)	((x)<0?(-x):(x))

////////////////////////////////////////////////////////////////////////////////////////
//
//	Constructor/Destructor
//
//

///
//	Octree()
//
Octree::Octree()
{
}

///
//	~Octree()
//
Octree::~Octree()
{
}

////////////////////////////////////////////////////////////////////////////////////////
//
//	Protected Methods
//
//

///
//  GetOppositeIdx()
//
//      Given a cube side index, return an index to the side opposite it
//
int Octree::GetOppositeIdx(int Idx)
{
    if(Idx % 2 == 0)
        return(Idx + 1);
    else
        return(Idx - 1);
}

///
//	FindNeighbor()
//
//      Given the side of a cube face, search through the Octree
//      to find the best fit neighbor
//
void Octree::FindNeighbor(OCT_NODE **OctTable, OCT_NODE *Node, BoxSide *Side,
						  int Idx, int *Found, float *FoundSize)
{
	if(Node && Node->ChildIdx[0] != NULL_NODE)
	{
		int i;

        for(i= 0; i < NUM_CHILDREN; i++)
        {
            int       TestIdx;
            BoxSide   TestSide;
            OCT_NODE *Child= OctTable[Node->ChildIdx[i]];

            ///
            //  Find the side of the test box facing the opposite
            //  direction of the current side
            //
            TestIdx= GetOppositeIdx(Idx);
            TestSide.SetFromBBox(Child->BBox, TestIdx);

            ///
            //  If it's the same size or greater, test to see if 
            //  planes are neighbors
            //
            if(Side->GetSize() <= TestSide.GetSize())
            {
                if(Side->Neighbors(TestSide))
                    if(TestSide.GetSize() < *FoundSize)
                    {
                        *Found= Node->ChildIdx[i];
                        *FoundSize= TestSide.GetSize();
                    }

                ///
                //  Keep searching if we haven't found a perfect fit
                //
                if(*FoundSize != Side->GetSize())                                 
                    FindNeighbor(OctTable, Child, Side, Idx, Found, FoundSize);
            }
        }
        
	}
}

///
//	FindBox()
//
//		Given a list of triangles, finds a cube centered at the origin which 
//		fully encloses all points
//
void Octree::FindBox(TRI *Tris, int TriCount, BBOX *BBox)
{
	float   Extent;
	int	    i,
		    j;

	for(i= 0; i < TriCount; i++)
	{
		TRI *Tri= &Tris[i];

		for(j= 0; j < 3; j++)
		{
			if(ABS(Tri->Pt[j].x) > Extent)
				Extent= ABS(Tri->Pt[j].x);

			if(ABS(Tri->Pt[j].y) > Extent)
				Extent= ABS(Tri->Pt[j].y);

			if(ABS(Tri->Pt[j].z) > Extent)
				Extent= ABS(Tri->Pt[j].z);
		}
	}

	BBox->Max.x= Extent;
	BBox->Max.y= Extent;
	BBox->Max.z= Extent;
	
	BBox->Min.x= -Extent;
	BBox->Min.y= -Extent;
	BBox->Min.z= -Extent;
}

///
//	BuildRootNode()
//  
//      Build the first node in the tree which encloses all the triangles
//
int Octree::BuildRootNode(TRI *Tris, int TriCount)
{
	OCT_NODE Root;
	int		 i;

	///
	//	Start by constructing a root node that contains
	//	all triangles
	//
	FindBox(Tris, TriCount, &Root.BBox);

	for(i= 0; i < NUM_CHILDREN; i++)
		Root.ChildIdx[i]= NULL_NODE;

	for(i= 0; i < NUM_NEIGHBORS; i++)
		Root.NeighborIdx[i]= NULL_NODE;

	Root.TriIdxStart= TriIdxList.GetCount();
	Root.TriIdxCount= TriCount;

	///
	//	Fill the start of the TriIdxList with pointers to all
	//	the triangles
	//	
	for(i= 0; i < TriCount; i++)
	{
		unsigned int Index= (unsigned int)i;

		TriIdxList.Append(&Index, sizeof(unsigned int));
	}
	
	///
	//	Add the root node to the table
	//
	OctNodeList.Append(&Root, sizeof(OCT_NODE));

	return(TRUE);
}

///
//	GetBox()
//
//		Given a parent box, finds a partitioned child box based on the 
//		the index i (i refers to the octant of the new box 0-7)
//
//
void Octree::GetBox(BBOX ParentBox, BBOX *NewBox, int i)
{
	vector3 BoxSize;
	vector3 Center;
	vector3 NewCenter;

	Center.x= (ParentBox.Max.x + ParentBox.Min.x) / 2;
	Center.y= (ParentBox.Max.y + ParentBox.Min.y) / 2;
	Center.z= (ParentBox.Max.z + ParentBox.Min.z) / 2;

	BoxSize.x= (ParentBox.Max.x - ParentBox.Min.x);
	BoxSize.y= (ParentBox.Max.y - ParentBox.Min.y);
	BoxSize.z= (ParentBox.Max.z - ParentBox.Min.z);

	if(i % 2 == 0)
		NewCenter.x= (Center.x - (BoxSize.x / 4));
	else
		NewCenter.x= (Center.x + (BoxSize.x / 4));

	if(i < 4)
		NewCenter.y= (Center.y - (BoxSize.y / 4));
	else
		NewCenter.y= (Center.y + (BoxSize.y / 4));

	if(i== 2 || i== 3 || i== 6 || i== 7)
		NewCenter.z= (Center.z - (BoxSize.z / 4));
	else
		NewCenter.z= (Center.z + (BoxSize.z / 4));

	NewBox->Min.x= (NewCenter.x - BoxSize.x / 4);
	NewBox->Min.y= (NewCenter.y - BoxSize.y / 4);
	NewBox->Min.z= (NewCenter.z - BoxSize.z / 4);

	NewBox->Max.x= (NewCenter.x + BoxSize.x / 4);
	NewBox->Max.y= (NewCenter.y + BoxSize.y / 4);
	NewBox->Max.z= (NewCenter.z + BoxSize.z / 4);
}

///
//	BuildTree()
//
//      After building the root node, this function recursively subdivides
//      the tree into octants.  Each octants gets a new bounding box and
//      all the polygons in the parent are tested to see which lie within
//      the new box.  Stops once each node contains no more than TrisPerNode
//
void Octree::BuildTree(OCT_NODE *Node, int TrisPerNode,
					   TRI *Tris,	   int TriCount)
{
	///
	//	Node has more than threshold number of triangles,
	//	create children
	//
	if(Node->TriIdxCount > TrisPerNode)
	{
		int i;

		for(i= 0; i < NUM_CHILDREN; i++)
		{
			int       j;
			OCT_NODE  NewNode;
			
			///
			//	Set this node's child pointer
			//
			Node->ChildIdx[i]= OctNodeList.GetCount();

			///
			//	Partition the new box for the child
			//
			GetBox(Node->BBox, &NewNode.BBox, i);
			
			///
			//	Initialize fields
			//
			for(j= 0; j < NUM_CHILDREN; j++)
				NewNode.ChildIdx[i]= NULL_NODE;

			for(j= 0; j < NUM_NEIGHBORS; j++)
				NewNode.NeighborIdx[i]= NULL_NODE;

			///
			//	See which of the parent's triangles lie within the
			//	new node
			//
			NewNode.TriIdxCount= 0;
			NewNode.TriIdxStart= TriIdxList.GetCount();
			TriIdxList.PositionAt(Node->TriIdxStart);
			for(j= 0; j < Node->TriIdxCount; j++)
			{
				unsigned int *TriIdx= (unsigned int*)TriIdxList.GetCurrent();
				TRI			 *Tri= &Tris[*TriIdx];
				
				if(TriBoxIntersect(NewNode.BBox, *Tri)== INSIDE)
				{
					TriIdxList.Append(TriIdx, sizeof(unsigned int));
					NewNode.TriIdxCount++;
				}
				
				TriIdxList.Advance();
			}

			///
			//	Add the new node and recurse on its children
			//
			OctNodeList.Append(&NewNode, sizeof(OCT_NODE));
			BuildTree((OCT_NODE*)OctNodeList.GetLast(), TrisPerNode, Tris, TriCount);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
//	Public Methods
//
//

///
//	Create()
//
//      Input:
//  
//          Tris        -  Array of triangles to construct Octree with
//          TriCount    -  Number of Triangles in the Tris array
//          TrisPerNode -  Max number of triangles in each octree node
//                         
//
//      Generates the all octree data
//
int Octree::Create(TRI *Tris, int TriCount, int TrisPerNode)
{
	OCT_NODE **OctTable;
	int		   OctCount;
	int		   i;

	if(!BuildRootNode(Tris, TriCount))
		return(FALSE);

	BuildTree((OCT_NODE*)OctNodeList.GetLast(), TrisPerNode,
			  Tris, TriCount);

	///
	//	Get the table and find neighbors
	//
	OctTable= (OCT_NODE**)OctNodeList.BuildTable(&OctCount);
	for(i = 0; i < OctCount; i++)
	{
		int       j;
		OCT_NODE *Node= OctTable[i];

		for(j = 0; j < NUM_NEIGHBORS; j++)
		{
			int		Found= NULL_NODE;
            float   FoundSize= MAX_VAL;

			BoxSide Side;

			Side.SetFromBBox(Node->BBox, j);

			FindNeighbor(OctTable, OctTable[0], &Side, j, 
                         &Found, &FoundSize);

            Node->NeighborIdx[j]= Found;
		}
	}

	free(OctTable);

	return(TRUE);
}

///
//  Save()
//
//      Input: 
//          
//          Fp - binary file to write to
//
//      Example of how Octree data could be saved to a file
//
int Octree::Save(FILE *Fp)
{
    OCT_NODE       **OctTable;
    int              OctNodeCount;
    unsigned int   **TriIdxTable;
    int              TriIdxCount;
    int              i;
    
    OctTable=       (OCT_NODE**)OctNodeList.BuildTable(&OctNodeCount);
    TriIdxTable=    (unsigned int**)TriIdxList.BuildTable(&TriIdxCount);

    ///
    //  Write out table counts
    //
    fwrite(&OctNodeCount, sizeof(int), 1, Fp);
    fwrite(&TriIdxCount,  sizeof(int), 1, Fp);

    ///
    //  Write out Octree
    //
    for(i= 0; i < OctNodeCount; i++)
        fwrite(OctTable[i], sizeof(OCT_NODE), 1, Fp);

    ///
    //  Write out Triangles indices
    //
    for(i= 0; i < TriIdxCount; i++)
        fwrite(TriIdxTable[i], sizeof(unsigned int), 1, Fp);

    free(OctTable);
    free(TriIdxTable);
    return(TRUE);
}


///
//  GetTables()
//
//      Input -
//
//          OctTable    - Gets a pointer to a linear block of 
//                        oct node data
//          OctCount    - Number of node's in OctTable
//          TriIdxTable - Gets a pointer to a linear block
//                        of all triangle indices
//          TriIdxCount - Number of indices in TriIdxTable
//
void Octree::GetTables(OCT_NODE     **OctTable,    int *OctCount,
                       unsigned int **TriIdxTable, int *TriIdxCount)
{        
    *OctTable=      (OCT_NODE*)OctNodeList.BuildLinearTable(OctCount);
    *TriIdxTable=   (unsigned int*)TriIdxList.BuildLinearTable(TriIdxCount);
}
