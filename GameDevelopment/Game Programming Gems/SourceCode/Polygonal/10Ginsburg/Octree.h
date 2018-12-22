/* Copyright (C) Dan Ginsburg, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Ginsburg, 2000"
 */
#ifndef _OCTREE_H_
#define _OCTREE_H_

///
//	Dependent Includes
//
#include <stdio.h>
#include "mtxlib.h"
#include "List.h"
#include "TriBox.h"
#include "BoxSide.h"

///
//	Macros
//
#define NUM_CHILDREN	8
#define NUM_NEIGHBORS	6
#define NULL_NODE		-1

///
//	Types
//

typedef struct
{
	BBOX	BBox;
	int		ChildIdx[NUM_CHILDREN];
	int		NeighborIdx[NUM_NEIGHBORS];

	int		TriIdxStart,
			TriIdxCount;
	
} OCT_NODE;

///
//	Classes
//
class Octree
{
	protected:
		List	OctNodeList;
		List	TriIdxList;

        int  GetOppositeIdx(int Idx);
    	
        void FindNeighbor(OCT_NODE **OctTable, OCT_NODE *Node, BoxSide *Side,
						  int Idx, int *Found, float *FoundSize);

		void FindBox(TRI *Tris, int TriCount, BBOX *BBox);

		int  BuildRootNode(TRI *Tris, int TriCount);

		void BuildTree(OCT_NODE *Node, int TrisPerNode,
					   TRI      *Tris, int TriCount);

		void GetBox(BBOX ParentBox, BBOX *NewBox, int i);


	public:

		///
		//	Constructor/Destructor
		//
		Octree();
		~Octree();

        ///
        //  Create()
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
        int Create(TRI *Tris, int TriCount, int TrisPerNode);

        ///
        //  Save()
        //  
        //      Input: 
        //          
        //          Fp - binary file to write to
        //
        //      Example of how Octree data could be saved to a file
        //
        int Save(FILE *Fp);

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
        void GetTables(OCT_NODE     **OctTable,    int *OctCount,
                       unsigned int **TriIdxTable, int *TriIdxCount);

};

#endif