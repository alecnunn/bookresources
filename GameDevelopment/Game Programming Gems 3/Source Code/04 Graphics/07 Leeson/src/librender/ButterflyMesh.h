/* Copyright (C) William Leeson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) William Leeson, 2001"
 */
/****************************************************************************\
 *                       Butterfly Subdivision Mesh                         *
 ****************************************************************************
 * File: ButterflyMesh.h                                                    *
 * Date: $Date$                                                             *
 * Author: $Author$                                                         *
 * Version: $Revision$                                                      *
 * Description: Butterfly approximating subdivision scheme.                 *
 \****************************************************************************/
class ButterflyMesh;

#ifndef _BUTTERFLY_SUBDIVISION_MESH_
#define _BUTTERFLY_SUBDIVISION_MESH_

#include "SubdivisionMesh.h"
#include<algorithm>
#include<vector>

class ButterflyMesh : public SubdivisionMesh
{
 public:
  // (Con/De)structor
  ButterflyMesh();
  ~ButterflyMesh();
  // bool subdivide(HalfEdgeMesh *p_mesh);                // subdivide given mesh to create this mesh
 private:
  void odd(vector<he_face> *p_faces,
	   vector<he_edge> *p_edges,
	   vector<he_vertex> *p_vertices);             // create odd vertices using odd mask
 public:
  bool compileweights(void);                           // create fast access weight array
};
#endif




