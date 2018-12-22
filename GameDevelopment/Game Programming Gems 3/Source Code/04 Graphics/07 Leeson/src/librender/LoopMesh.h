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
 *                         Loop Subdivision Mesh                            *
 ****************************************************************************
 * File: LoopMesh.h                                                         *
 * Date: $Date$                                                             *
 * Author: $Author$                                                         *
 * Version: $Revision$                                                      *
 * Description: Loop approximating subdivision scheme.                      *
 \****************************************************************************/
class LoopMesh;

#ifndef _LOOP_SUBDIVISION_MESH_
#define _LOOP_SUBDIVISION_MESH_

#include "SubdivisionMesh.h"
#include<algorithm>
#include<vector>

class LoopMesh : public SubdivisionMesh
{
  /*
 private:
  vector<he_vertex> *mp_childvertices;
  vector<he_edge> *mp_childedges;
  vector<he_face> *mp_childfaces;

  vector<real> m_weights;
  vector<vector<he_vertex>::iterator> m_sources;
  vector<integer> m_counts;
  vector<vector<he_vertex>::iterator> m_destinations;
  */
 public:
  // (Con/De)structor
  LoopMesh();
  ~LoopMesh();
  // bool subdivide(HalfEdgeMesh *p_mesh);                // subdivide given mesh to create this mesh
 private:
  void odd(vector<he_face> *p_faces,
	   vector<he_edge> *p_edges,
	   vector<he_vertex> *p_vertices);             // create odd vertices using odd mask
  void even(vector<he_face> *p_faces,
	    vector<he_edge> *p_edges,
	    vector<he_vertex> *p_vertices);            // create even vertices using even mask
 public:
  bool compileweights(void);                           // create fast access weight array
};
#endif




