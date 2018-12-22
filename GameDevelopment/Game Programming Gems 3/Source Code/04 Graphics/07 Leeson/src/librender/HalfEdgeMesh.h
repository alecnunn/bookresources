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
 *                           Half-Edge Mesh                                 *
 ****************************************************************************
 * File: HalfEdgeMesh.h                                                     *
 * Date: $Date$                                                             *
 * Author: $Author$                                                         *
 * Version: $Revision$                                                      *
 * Description: Half-Edge data structure for storing a mesh                 *
\****************************************************************************/

struct he_vertex;
struct he_edge;
struct he_edge;
class HalfEdgeMesh;

#ifndef _HALF_EDGE_MESH_
#define _HALF_EDGE_MESH_

#include <stdio.h>
#include <stdlib.h>
#include<iostream>
#include<vector>
#include<IScene.h>
#include "libarray/libarray.h"

struct he_vertex
{
  real V[4];                              // vertex
  real N[3];                              // normal
  real tex[2];                            // texture coords
  vector<he_edge>::iterator edge;         // half edge containing vertex
};

struct he_face
{
  vector<he_edge>::iterator edge;         // edges in face
  real colour[3];                         // colour
  real N[3];                              // normal for face
  integer fid;                            // tag for fame id used to detect if poly has been drawn
  bool visibility;                        // face visibilty
};

struct he_edge
{
  bool crease;                            // is this edge a crease
  vector<he_vertex>::iterator vertex;     // vertex
  vector<he_edge>::iterator pair;         // next edge in graph i.e. at head
  vector<he_face>::iterator face;         // next edge clockwise
  vector<he_edge>::iterator next,prev;    // left and right faces
};

class HalfEdgeMesh
{
 public:

 protected:
  enum { visible = 0, hidden, partial };
  real m_min[3],m_max[3];
  vector<he_face> m_faces;    // edges making up faces
  vector<he_edge> m_edges;    // edge array
  vector<he_vertex> m_vertices;// list of vertices with their corresponding edge

  vector<integer> m_indices;                      // index array for faces
  vector<vector<he_face>::iterator> m_neighbours; // neighbouring face list
  vector<integer> m_neighbourcount;               // neighbouring face count
 public:
  HalfEdgeMesh();   // constructor
  virtual ~HalfEdgeMesh();  // destructor

  // Properties
  virtual vector<he_vertex> *GetVertices(void);  // Get vertex vector
  virtual vector<he_edge> *GetEdges(void);       // Get edge vector
  virtual vector<he_face> *GetFaces(void);       // Get face vector

  // drawing stuff
  virtual void draw(real *dir,real lod,IScene *p_scene); // draw the object using assigned scene
  virtual void compile(void);                            // compile vertex index list for vertex array
  virtual void compileneighbours(void);                  // compile list of neighbours for culling

  // Bounding Box Stuff
  void GetBoundingBox(real *min,real *max) const;        // Get bounding box for mesh 
  void determine_volume(real *min,real *max);            // Calculate bounding box for mesh

  // I/O should really be HEOFF format (put conversion in another method)
  virtual void Load(char *filename);
  virtual void Save(char *filename);

  // half edge opperations
  virtual vector<he_edge>::iterator createhalfedge(vector<he_vertex>::iterator v,
						   vector<he_edge>::iterator prev,
						   vector<he_edge>::iterator next,
						   vector<he_edge>::iterator pair,
						   bool crease);                    // create a half-edge
  virtual vector<he_edge>::iterator createhalfedge(vector<he_vertex>::iterator v);

  // edge opperations
  virtual vector<he_edge>::iterator createedge(vector<he_vertex>::iterator v1,
					       vector<he_vertex>::iterator v2);     // create a half-edge
  virtual void splitedge(vector<he_edge>::iterator edge,
			 vector<he_vertex>::iterator vertex);                       // split an edge in half
  //virtual void rotateedge(vector<he_edge>::iterator e);
  virtual vector<he_edge>::iterator findedge(vector<he_vertex>::iterator v1,
					     vector<he_vertex>::iterator v2);       // find a half-edge from v1 to v2
  virtual void deleteedge(vector<he_edge>::iterator edge);                          // remove an edge from mesh

  // face opperations
  virtual vector<he_face>::iterator createface(vector<he_edge>::iterator e);        // create a face
  virtual vector<he_face>::iterator createface(vector<he_edge>::iterator e1,        // create a face
					       vector<he_edge>::iterator e2,
					       vector<he_edge>::iterator e3);
  virtual vector<he_face>::iterator splitface(vector<he_face>::iterator face,
					      vector<he_edge>::iterator e1,
					      vector<he_edge>::iterator e2);        // split face using 2 edges
  virtual void facenormal(vector<he_face>::iterator face,
			  real *N);                                                 // determine normal for face
  virtual void deleteface(vector<he_face>::iterator face);
  virtual integer edgecount(vector<he_face>::iterator face);                        // count edges in face
  virtual vector<he_vertex>::iterator matchpoint(vector<he_vertex>::iterator pt);   // find a similar point in vertex list
  virtual void normals(void);                                                       // calc vertex normals from face normals

  virtual bool copy(HalfEdgeMesh *p_mesh);                                          // copy a mesh to another

  // display data/stats
  virtual void printedgelist(void);                                                 // print list of edged
  virtual void printfacelist(void);                                                 // print list of faces
  virtual void printvertexlist();                                                   // print list of vertices
  virtual void printedge(vector<he_edge>::iterator edge);                           // print and edge
  virtual void printvertex(vector<he_vertex>::iterator vertex);                     // print a vertex
  virtual void printconnected(vector<he_vertex>::iterator vertex);                  // print edges connected to a vertex
  virtual void printface(vector<he_face>::iterator face);                           // print face information
};
#endif



