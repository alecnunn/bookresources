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
 * File: LoopMesh.cc                                                        *
 * Date: $Date$                                                             *
 * Author: $Author$                                                         *
 * Version: $Revision$                                                      *
 * Description: Loop approximating subdivision scheme.                      *
\****************************************************************************/
#include "LoopMesh.h"

/*
 * constructor
 */
LoopMesh::LoopMesh()
{
  // Nothing todo...yet!!!!
}

LoopMesh::~LoopMesh()
{
  // nothing todo here either!!!!!
}

/*
 * odd vertex masks
 *            v3 1/8  
 *             o
 *            / \
 *           /   \
 *   v1 3/8 o--x--o  v2 3/8
 *           \   /
 *            \ / 
 *             o 
 *           v4 1/8
 *
 */
void LoopMesh::odd(vector<he_face> *p_faces,
		   vector<he_edge> *p_edges,
		   vector<he_vertex> *p_vertices)
{
  real tmp[4],pt[4],tuv[2];
  vector<he_edge>::iterator edge,next,pair;
  vector<he_vertex>::iterator v1,v2,v3,v4,vn;
  he_vertex v;

  for(edge = p_edges->begin();edge != p_edges->end();edge++)
    {
      v1 = edge->vertex;
      next = edge->next;
      pair = edge->pair;

      v2 = next->vertex;
      
      if(edge < pair)
	{
	  next = next->next;
	  v3 = next->vertex;
	  
	  next = pair->next;
	  next = next->next;
	  v4 = next->vertex;

	  // point
	  AAdd3D(v1->V,v2->V,tmp);
	  ASMultiply3D(3.0/8.0,tmp,pt);
	  AAdd3D(v3->V,v4->V,tmp);
	  ASMultiply3D(1.0/8.0,tmp,tmp);
	  AAdd3D(tmp,pt,v.V);
	  v.V[3] = 1.0;

	  //texture
	  AAdd2D(v1->tex,v2->tex,tuv);
	  ASMultiply2D(3.0/8.0,tuv,pt);
	  AAdd2D(v3->tex,v4->tex,tuv);
	  ASMultiply2D(1.0/8.0,tuv,tuv);
	  AAdd2D(tuv,pt,v.tex);

	  // create new vertex
	  m_vertices.push_back(v);
	  vn = m_vertices.end() - 1;

	  splitedge(&m_edges[edge - p_edges->begin()],vn);
	}
    }
}

/*
 * even vertex masks
 *            v3 B        v2 B 
 *                 o     o
 *                  \   /
 *                   \ /
 *           v4 B o---o---o v1 B
 *                   / \v0 1-kB
 *                  /   \
 *                 o     o
 *              v5 B    v6 B
 *   B = 1/n(5/8 - (3/8 + 1/4cos(2*PI/n))^2)
 *
 */
void LoopMesh::even(vector<he_face> *p_faces,
		    vector<he_edge> *p_edges,
		    vector<he_vertex> *p_vertices)
{
  integer n;
  real V[4],pt[4],tex[2],B;
  vector<he_edge>::iterator edge;
  vector<he_vertex>::iterator vertex,vertexend;
  he_vertex v;

  vertexend = p_vertices->end();
  for(vertex = p_vertices->begin();vertex != vertexend;vertex++)
    {
      edge = vertex->edge;
      n = 0;
      V[0] = V[1] = V[2] = V[3] = tex[0] = tex[1] = 0.0;
      do{
	edge = edge->pair;
	AAdd3D(edge->vertex->V,V,V);
	AAdd2D(edge->vertex->tex,tex,tex);
	edge = edge->next;
	n++;
      }while(edge != vertex->edge);
      B = (5.0/8.0 - pow(3.0/8.0 + 0.25*cos((2*M_PI)/n),2))/n;
      ASMultiply3D(B,V,V);
      ASMultiply3D(1.0 - n*B,edge->vertex->V,pt);
      AAdd3D(V,pt,v.V);

      ASMultiply2D(B,tex,tex);
      ASMultiply2D(1 - n*B,tex,pt);
      AAdd2D(tex,pt,v.tex);

      m_vertices.push_back(v);
    }
}

/*
 * compileweights - create an array featuring vertices and there weights
 */
bool LoopMesh::compileweights(void)
{
  integer c,n;
  real B;
  vector<he_vertex>::iterator vertex,v1,v2,v3,v4;
  vector<he_edge>::iterator edge,next,pair;

  // odd vertices
  cerr <<  "compiling weight array ";
  //cerr << "even - {\n";
  for(vertex = mp_childvertices->begin();vertex != mp_childvertices->end();vertex++)
    {
      //cerr << " " << vertex - mp_childvertices->begin();
      //cerr << " - (";
      edge = vertex->edge;
      n = 0;
      do{
	edge = edge->pair;
	m_sources.push_back(edge->vertex);
	//cerr << " <" << edge - mp_childedges->begin() << "," << edge->vertex - mp_childvertices->begin() << ">";
	edge = edge->next;
	n++;
      }while(edge != vertex->edge);
      //cerr << " )\n";
      B = (5.0/8.0 - pow(3.0/8.0 + 0.25*cos((2*M_PI)/n),2))/n;
      edge = vertex->edge;
      do{
	m_weights.push_back(B);
	edge = edge->pair->next;
      }while(edge != vertex->edge);

      m_sources.push_back(edge->vertex);
      m_weights.push_back(1.0 - n*B);

      m_counts.push_back(n + 1);
      
      m_destinations.push_back(&m_vertices[vertex - mp_childvertices->begin()]);
    }
  //cerr << "}\n";

  // even vertices
  //cerr << "odd - {\n";
  c = mp_childvertices->size();
  //cerr << "even vertices: vt:" << c << " ed: " << mp_childedges->size() << "\n";
  for(edge = mp_childedges->begin();edge != mp_childedges->end();edge++)
    {
      v1 = edge->vertex;

      next = edge->next;
      pair = edge->pair;

      v2 = next->vertex;

      if(edge < pair)
	{
	  m_counts.push_back(4);

	  m_sources.push_back(v1);
	  m_weights.push_back(3.0/8.0);

	  m_sources.push_back(v2);
	  m_weights.push_back(3.0/8.0);

	  next = next->next;
	  v3 = next->vertex;
	  m_sources.push_back(v3);
	  m_weights.push_back(1.0/8.0);
	  
	  next = pair->next;
	  next = next->next;
	  v4 = next->vertex;
	  m_sources.push_back(v4);
	  m_weights.push_back(1.0/8.0);

	  m_destinations.push_back(c + m_vertices.begin());
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
