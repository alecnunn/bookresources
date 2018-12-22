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
 *                              Subdivision Mesh                            *
 ****************************************************************************
 * File: SubdivisionMesh.h                                                  *
 * Date: $Date$                                                             *
 * Author: $Author$                                                         *
 * Version: $Revision$                                                      *
 * Description: Interpolating subdivision scheme that uses a linear         *
 *              interpolation and is the base class for other schemes.      *
 \****************************************************************************/
class SubdivisionMesh;

#ifndef _SUBDIVISION_MESH_
#define _SUBDIVISION_MESH_

#include "HalfEdgeMesh.h"
#include<algorithm>
#include<vector>

class SubdivisionMesh : public HalfEdgeMesh
{
 protected:
  vector<he_vertex> *mp_childvertices;
  vector<he_edge> *mp_childedges;
  vector<he_face> *mp_childfaces;

  vector<real> m_weights;
  vector<vector<he_vertex>::iterator> m_sources;
  vector<integer> m_counts;
  vector<vector<he_vertex>::iterator> m_destinations;
 public:
  // (Con/De)structor
  SubdivisionMesh();
  ~SubdivisionMesh();
  bool subdivide(HalfEdgeMesh *p_mesh);                        // subdivide given mesh to create this mesh

 private:
  virtual void odd(vector<he_face> *p_faces,
		   vector<he_edge> *p_edges,
		   vector<he_vertex> *p_vertices);             // create odd vertices using odd mask
  virtual void even(vector<he_face> *p_faces,
		    vector<he_edge> *p_edges,
		    vector<he_vertex> *p_vertices);            // create even vertices using even mask

  virtual void faces(vector<he_face> *p_faces,
		     vector<he_edge> *p_edges,
		     vector<he_vertex> *p_vertices);           // create sub-faces
  virtual void recreate(vector<he_face> *p_faces,
			vector<he_edge> *p_edges,
			vector<he_vertex> *p_vertices);        // recreate mesh conectivity
 public:
  virtual bool compileweights(void);                           // create fast access weight array
  bool update(void);                                           // update vertices using compiled weights
};
#endif




