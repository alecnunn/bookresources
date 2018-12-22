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
 * File: SubdivisionMesh.cc                                                 *
 * Date: $Date$                                                             *
 * Author: $Author$                                                         *
 * Version: $Revision$                                                      *
 * Description: Interpolating subdivision scheme that uses a linear         *
 *              interpolation and is the base class for other schemes.      *
 \****************************************************************************/
#include "SubdivisionMesh.h"

/*
 * constructor
 */
SubdivisionMesh::SubdivisionMesh()
{
  // Nothing todo...yet!!!!
}

SubdivisionMesh::~SubdivisionMesh()
{
  // nothing todo here either!!!!!
}

/*
 * Subdivide - from previous Half Edge Mesh to fill out this mesh
 */
bool SubdivisionMesh::subdivide(HalfEdgeMesh *p_mesh)
{
  // create enough spaces for faces, edges and vertices
  mp_childvertices = p_mesh->GetVertices();
  mp_childedges  = p_mesh->GetEdges();
  mp_childfaces  = p_mesh->GetFaces();

  // Allocate/reserve sufficient memory
  /*
  cerr << "reserving vertices:" << mp_childvertices->size() + mp_childfaces->size()*3
       << " edges:" << mp_childedges->size()*2 + mp_childfaces->size()*2*3
       << " faces:" << mp_childfaces->size()*4 << "\n";
  */

  // need to reserve space for the exact amount of memory needed or more because
  // pointer to these structures are use if a reallocation should occur these
  // pointers may become invalid causing all kind of trouble
  m_vertices.reserve(mp_childvertices->size() + mp_childfaces->size()*3 + 1000);
  m_edges.reserve(mp_childedges->size()*2 + mp_childfaces->size()*2*3 + 1000);
  m_faces.reserve(mp_childfaces->size()*4 + 1000);

  this->even(mp_childfaces,mp_childedges,mp_childvertices);
  this->recreate(mp_childfaces,mp_childedges,mp_childvertices);
  this->odd(mp_childfaces,mp_childedges,mp_childvertices);
  this->faces(mp_childfaces,mp_childedges,mp_childvertices);
  this->normals();
  cerr << "subdivide faces:" << m_faces.size() 
       << " edges:"          << m_edges.size() 
       << " vertices:"       << m_vertices.size() << endl;

  return true;
}

/*
 * even vertex masks - points just stay them same
 */
void SubdivisionMesh::even(vector<he_face> *p_faces,
			   vector<he_edge> *p_edges,
			   vector<he_vertex> *p_vertices)
{
  vector<he_vertex>::iterator vertex;

  // rather that copy the vertices here it would be more benifical to to link back to the
  // vertices in the parent structure this saving time and space but this is easier
  for(vertex = p_vertices->begin();vertex != p_vertices->end();vertex++)
    {
      m_vertices.push_back((*vertex));
    }
}

/*
 * odd vertex masks - just halfway between 2
 *     1/2 o---x---o 1/2
 */
void SubdivisionMesh::odd(vector<he_face> *p_faces,
			  vector<he_edge> *p_edges,
			  vector<he_vertex> *p_vertices)
{
  vector<he_edge>::iterator edge,edgeend;
  vector<he_vertex>::iterator vn;
  he_vertex v;

  //cerr << "splitting " << m_edges.size() << "edges\n";
  edgeend = p_edges->end();
  for(edge = p_edges->begin();edge != edgeend;edge++)
    {
      //cerr << "edge:" << edge << " pair:" << edge->pair << endl;
      //printedge(edge);
      // Only create new vertex for half the half-edges 
      // i.e. ones who have their pair less the than 
      // themself as either they are higher or their
      // pair but they are never equal
      if((edge < edge->pair) || (edge->pair == NULL))
	{
	  // create new vertex
	  //cerr << "v1:"; APrint4D(edge->vertex->V); cerr << " v2:"; APrint4D(edge->next->vertex->V); cerr << endl;
	  AAdd3D(edge->vertex->V,edge->next->vertex->V,v.V);
	  ASMultiply3D(0.5,v.V,v.V);
	  v.V[3] = 1.0;
	  // create texture coord
	  AAdd2D(edge->vertex->tex,edge->next->vertex->tex,v.tex);
	  ASMultiply2D(0.5,v.tex,v.tex);
	  m_vertices.push_back(v);
	  vn = m_vertices.end() - 1;
	  //cerr << "splitting edge " << edge - m_edges.begin() << "....";
	  splitedge(&m_edges[edge - p_edges->begin()],vn);
	  //cerr << "done\n";
	}
    }
}

/*
 * faces - construct new faces
 */
void SubdivisionMesh::faces(vector<he_face> *p_faces,
			    vector<he_edge> *p_edges,
			    vector<he_vertex> *p_vertices)
{
  vector<he_face>::iterator face,newface,faceend;
  vector<he_edge>::iterator e1,e2; //,e3,e4,e5,e6,e7,e8,e9,e10,e11,e12;
  // vector<he_vertex>::iterator v2,v4,v6;

  faceend = m_faces.end();
  for(face = m_faces.begin();face != faceend;face++)
    {
      //cerr << "split one\n";
      //printface(face);
      e1 = face->edge;
      e2 = e1->next;
      e2 = e2->next;
      newface = splitface(face,e1,e2);
      //cerr << "face:" << face - m_faces.begin() << "{ (" << edgecount(face) << "," << edgecount(newface) << ") ";
      //printface(face);
      //printface(newface);

      //cerr << "split two\n";
      e1 = face->edge;
      e2 = e1->prev;
      e2 = e2->prev;
      newface = splitface(face,e2,e1);
      //cerr << "(" << edgecount(face) << "," << edgecount(newface) << ") ";
      //printface(face);
      //printface(newface);

      //cerr << "split three\n";
      e1 = face->edge;
      e2 = e1->next;
      e2 = e2->next;
      newface = splitface(face,e1,e2);
      //cerr << "(" << edgecount(face) << "," << edgecount(newface) << ") }\n";
      //printface(face);
      //printface(newface);      
    }
  //printfacelist();
}


/*
 * re-create - recreates mesh conectivity with out copying vertices
 */
void SubdivisionMesh::recreate(vector<he_face> *p_faces,
			       vector<he_edge> *p_edges,
			       vector<he_vertex> *p_vertices)
{
  vector<he_vertex>::iterator vertex;
  vector<he_edge>::iterator edge;
  vector<he_face>::iterator face;

  //cerr << "recreate edges\n";
  // recreate edges
  for(edge = p_edges->begin();edge != p_edges->end();edge++)
    {
      createhalfedge(&m_vertices[edge->vertex - p_vertices->begin()]);
    }
  
  //cerr << "link edges\n";
  // link edges together
  for(edge = p_edges->begin();edge != p_edges->end();edge++)
    {
      m_edges[edge - p_edges->begin()].next = &m_edges[edge->next - p_edges->begin()];
      m_edges[edge - p_edges->begin()].prev = &m_edges[edge->prev - p_edges->begin()];
      if(edge->pair != NULL)
	m_edges[edge - p_edges->begin()].pair = &m_edges[edge->pair - p_edges->begin()];
      else
	m_edges[edge - p_edges->begin()].pair = NULL;
      m_edges[edge - p_edges->begin()].vertex = &m_vertices[edge->vertex - p_vertices->begin()];
    }

  //cerr << "recreate faces\n";
  // recreate faces
  for(face = p_faces->begin();face != p_faces->end();face++)
    {
      createface(&m_edges[face->edge - p_edges->begin()]);
    }

  //cerr << "link vertices\n";
  // link vertices to edges
  for(vertex = p_vertices->begin();vertex != p_vertices->end();vertex++)
    {
      m_vertices[vertex - p_vertices->begin()].edge = &m_edges[vertex->edge - p_edges->begin()];
    }
  //cerr << "done\n";
}

/*
 * compileweights - create an array featuring
 */
bool SubdivisionMesh::compileweights(void)
{
  integer c;
  vector<he_vertex>::iterator vertex;
  vector<he_edge>::iterator edge;

  // odd vertices
  cerr <<  "compiling weight array ";
  for(vertex = mp_childvertices->begin();vertex != mp_childvertices->end();vertex++)
    {
      m_counts.push_back(1);

      m_weights.push_back(1.0);
      m_sources.push_back(vertex);
      
      //cerr << "\t1 1.0 " << vertex - mp_childvertices->begin() << endl;
      m_destinations.push_back(&m_vertices[vertex - mp_childvertices->begin()]);
    }

  // even vertices
  c = mp_childvertices->size();
 
  //cerr << "even vertices: vt:" << c << " ed: " << mp_childedges->size() << "\n";
  for(edge = mp_childedges->begin();edge != mp_childedges->end();edge++)
    {
      if((edge < edge->pair) || (edge->pair == NULL))
	{
	  m_counts.push_back(2);

	  m_weights.push_back(0.5);
	  m_sources.push_back(edge->vertex);

	  m_weights.push_back(0.5);
	  m_sources.push_back(edge->next->vertex);

	  m_destinations.push_back(c + m_vertices.begin());
	  /*
	  cerr << "0.5 "  << edge->vertex       - mp_childvertices->begin()
	       << " 0.5 " << edge->next->vertex - mp_childvertices->begin()
	       << " "     << c << endl;
	  */
	  c++;
	}
    }
  cerr << "....done\n";

  return true;
}

/*
 * update - update the vertices according to control mesh
 */
bool SubdivisionMesh::update(void)
{
  vector<integer>::iterator count,countend;
  vector<vector<he_vertex>::iterator>::iterator vs,vsend,vd;
  vector<real>::iterator weight; //,weightend;
  real V[4]; //,tex[2];

  countend = m_counts.end();
  vs = m_sources.begin();
  vd = m_destinations.begin();
  weight = m_weights.begin();
  for(count = m_counts.begin();count != countend;count++)
    {
      //cerr << "update vtx:" << (*vd) - m_vertices.end() << " count:" << (*count) << endl;
      vsend = vs + (*count);
      ASMultiply3D((*weight),(*vs)->V,(*vd)->V);
      weight++;
      vs++;
      //ASMultiply2D((*weight),(*vs)->tex,tex);
      while(vs != vsend)
	{
	  //cerr << " " << (*weight);
	  ASMultiply3D((*weight),(*vs)->V,V);
	  AAdd3D(V,(*vd)->V,(*vd)->V);
	  weight++;
	  vs++;
	  //ASMultiply2D((*weight),(*vs)->tex,tex);
	  //AAdd2D(tex,(*vd)->tex,(*vd)->tex);
	};
      (*vd)->V[3] = 1.0;
      //cerr << " vtx:"; APrint4D((*vd)->V); cerr << endl;
      vd++;
    }

  // recalculate the normals
  normals();
  //cerr << "-----------------" << endl;

  return true;
}
