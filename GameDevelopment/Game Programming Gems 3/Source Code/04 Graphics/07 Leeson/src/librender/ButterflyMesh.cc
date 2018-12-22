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
 *                         Butterfly Subdivision Mesh                       *
 ****************************************************************************
 * File: ButterflyMesh.cc                                                   *
 * Date: $Date$                                                             *
 * Author: $Author: leesonw $                                               *
 * Version: $Revision$                                                      *
 * Description: Butterfly interpolating subdivision scheme.                 *
 \****************************************************************************/
#include "ButterflyMesh.h"

/*
 * constructor
 */
ButterflyMesh::ButterflyMesh()
{

  // Nothing todo...yet!!!!
}

ButterflyMesh::~ButterflyMesh()
{
  // nothing todo here either!!!!!
}

/*
 * odd vertex masks
 *-1/16 v5    v3 1/8  v5 -1/16 
 *       o-----o-----o
 *        \   / \   /
 *         \ /   \ /
 *   1/2 v1 o--x--o v2 1/2
 *         / \   / \
 *        /   \ /   \
 *       o-----o-----o
 *-1/16 v7    v4 1/8  v8 -1/16
 *
 */
void ButterflyMesh::odd(vector<he_face> *p_faces,
			vector<he_edge> *p_edges,
			vector<he_vertex> *p_vertices)
{
  real tmp[4],pt[4],tuv[2];
  vector<he_edge>::iterator edge,next,pair;
  vector<he_vertex>::iterator v1,v2,v3,v4,v5,v6,v7,v8,vn;
  he_vertex v;

  for(edge = p_edges->begin();edge != p_edges->end();edge++)
    {
      v1 = edge->vertex;
      v7 = edge->next->pair->next->next->vertex;

      next = edge->next;
      pair = edge->pair;

      v2 = pair->vertex;
      v8 = pair->next->pair->next->next->vertex;

      if(edge < pair)
	{
	  // get v3
	  next = next->next;
	  v3 = next->vertex;
	  
	  v5 = next->pair->next->next->vertex;

	  // get v4
	  next = pair->next;
	  next = next->next;
	  v4 = next->vertex;

	  v6 = next->pair->next->next->vertex;

	  // point
	  AAdd3D(v1->V,v2->V,tmp);
	  ASMultiply3D(1.0/2.0,tmp,pt);
	  AAdd3D(v3->V,v4->V,tmp);
	  ASMultiply3D(1.0/8.0,tmp,tmp);
	  AAdd3D(tmp,pt,v.V);
	  AAdd3D(v5->V,v6->V,tmp);
	  AAdd3D(v7->V,tmp,tmp);
	  AAdd3D(v8->V,tmp,tmp);
	  ASMultiply3D(-1.0/16.0,tmp,pt);
	  AAdd3D(pt,v.V,v.V);
	  v.V[3] = 1.0;

	  //texture
	  AAdd2D(v1->tex,v2->tex,tmp);
	  ASMultiply2D(1.0/2.0,tmp,tuv);
	  AAdd2D(v3->tex,v4->tex,tmp);
	  ASMultiply2D(1.0/8.0,tmp,tmp);
	  AAdd2D(tmp,tuv,v.tex);
	  AAdd2D(v5->tex,v6->tex,tmp);
	  AAdd2D(v7->tex,tmp,tmp);
	  AAdd2D(v8->tex,tmp,tmp);
	  ASMultiply2D(-1.0/16.0,tmp,tuv);
	  AAdd2D(tuv,v.tex,v.tex);
	  
	  m_vertices.push_back(v);
	  vn = m_vertices.end() - 1;
	  
	  splitedge(&m_edges[edge - p_edges->begin()],vn);
	}
    }
}

/*
 * compileweights - create an array featuring
 */
bool ButterflyMesh::compileweights(void)
{
  integer c; //,n;
  // real B;
  vector<he_vertex>::iterator vertex,v1,v2,v3,v4,v5,v6,v7,v8;
  vector<he_edge>::iterator edge,next,pair;

  // odd vertices
  cerr <<  "compiling weight array ";
  //cerr << "even - { ";
  for(vertex = mp_childvertices->begin();vertex != mp_childvertices->end();vertex++)
    {
      m_counts.push_back(1);

      m_weights.push_back(1.0);
      m_sources.push_back(vertex);
      
      // cerr << "\t1 1.0 " << vertex - mp_childvertices->begin() << endl;
      m_destinations.push_back(&m_vertices[vertex - mp_childvertices->begin()]);

      //cerr << " " << vertex - mp_childvertices->begin();
    }
  //cerr << "}\n";

  // even vertices
  //cerr << "odd - { ";
  c = mp_childvertices->size();
  //cerr << "even vertices: vt:" << c << " ed: " << mp_childedges->size() << "\n";
  for(edge = mp_childedges->begin();edge != mp_childedges->end();edge++)
    {
      v1 = edge->vertex;
      v7 = edge->next->pair->next->next->vertex;

      next = edge->next;
      pair = edge->pair;

      v2 = pair->vertex;
      v8 = pair->next->pair->next->next->vertex;

      if(edge < pair)
	{
	  m_counts.push_back(8);
	  
	  // get v3
	  next = next->next;
	  v3 = next->vertex;
	  
	  v5 = next->pair->next->next->vertex;

	  // get v4
	  next = pair->next;
	  next = next->next;
	  v4 = next->vertex;

	  v6 = next->pair->next->next->vertex;

	  m_sources.push_back(v1);
	  m_weights.push_back(1.0/2.0);
	  m_sources.push_back(v2);
	  m_weights.push_back(1.0/2.0);
	  m_sources.push_back(v3);
	  m_weights.push_back(1.0/8.0);
	  m_sources.push_back(v4);
	  m_weights.push_back(1.0/8.0);
	  m_sources.push_back(v5);
	  m_weights.push_back(-1.0/16.0);
	  m_sources.push_back(v6);
	  m_weights.push_back(-1.0/16.0);
	  m_sources.push_back(v7);
	  m_weights.push_back(-1.0/16.0);
	  m_sources.push_back(v8);
	  m_weights.push_back(-1.0/16.0);

	  m_destinations.push_back(c + m_vertices.begin());
	  //cerr << " " << c;
	  /*
	  cerr << " " << c;
	  cerr << "-( " << v1 - mp_childvertices->begin()
	       << " " << v2 - mp_childvertices->begin()
	       << " " << v3 - mp_childvertices->begin()
	       << " " << v4 - mp_childvertices->begin() << ")\n";
	  */
	  c++;
	}
    }
  //cerr << "}\n";
  cerr << "....done\n";

  return true;
}
