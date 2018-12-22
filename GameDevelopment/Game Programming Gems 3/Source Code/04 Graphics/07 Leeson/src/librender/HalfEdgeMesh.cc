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
 *                              Half-Edge Mesh                              *
 ****************************************************************************
 * File: HalfEdgeMesh.cc                                                    *
 * Date: $Date$                                                             *
 * Author: $Author$                                                         *
 * Version: $Revision$                                                      *
 * Description: Half-Edge data structure for storing a mesh                 *
\****************************************************************************/
#include "HalfEdgeMesh.h"

/*
 * constructor
 */
HalfEdgeMesh::HalfEdgeMesh()
{
  // Nothing todo...yet!!!!
}

HalfEdgeMesh::~HalfEdgeMesh()
{
  // vectors automatically deleted so nothing todo either
}

/*
 * GetVertices - return the vertex vector
 */
vector<he_vertex> *HalfEdgeMesh::GetVertices(void)
{
  return &m_vertices;
}

/* 
 * GetEdges - return the edge vector
 */
vector<he_edge> *HalfEdgeMesh::GetEdges(void)
{
  return &m_edges;
}

/*
 *  GetFaces - return the face vector
 */
vector<he_face> * HalfEdgeMesh::GetFaces(void)
{
  return &m_faces;
}

/*
 * compileneighbours - generate a list of neighbouring faces and save them in an array
 */
void HalfEdgeMesh::compileneighbours(void)
{
  integer c;
  vector<he_face>::iterator face;
  vector<he_edge>::iterator edge,e;

  // iterate through each face compiling a list of neighbours
  //cerr << "face neighbours\n";
  for(face = m_faces.begin();face != m_faces.end();face++)
    {
      c = 0;
      //cerr << "{";
      // get face on each edge
      edge = face->edge;
      do{
	//cerr << " " << edge->pair->face;
	m_neighbours.push_back(edge->pair->face);
	c++;
	// for each edge itterate over corners
	//            \ /
	//           --o--
	//            / \ 
        //         \ /   \ /
	//        --o-----o--
        //         / \   / \ 
	e = edge->pair;
	do{
	  e = e->next->pair;
	  //cerr << " " << e->face;
	  m_neighbours.push_back(e->face);
	  c++;
	}while(edge->face != face);
	edge = edge->next;
      }while(edge != face->edge);      
      m_neighbourcount.push_back(c);
      //cerr << " } " << c << endl;
    }
}

/*
 * compile -  Compile vertex index for each face
 */
void HalfEdgeMesh::compile(void)
{
  integer c;
  vector<he_face>::iterator face;
  vector<he_edge>::iterator edge;

  c = 0;
  m_indices.reserve(m_faces.size()*3);
  cerr << "compiling array of " << m_faces.size() << " faces";
  for(face = m_faces.begin();face != m_faces.end();face++)
    {
      edge = face->edge;
      // iterate over each half-edge in the face
      do{
	m_indices.push_back(edge->vertex - m_vertices.begin());
	edge = edge->next;
	c++;
      }while(face->edge != edge);
    }
  cerr << " using " << c << ":" << m_faces.size()*3  << " vertices....done." << endl;
}

/*
 * draw the state i.e. the object/thing it represents
 */
void HalfEdgeMesh::draw(real *dir,real lod,IScene *p_scene)
{
  /*
  cerr << "face ptr:"    << m_indices.begin() << " size:" << m_faces.size()
       << " vertex ptr:" << m_vertices.begin()->V
       << " normal ptr:" << m_vertices.begin()->N
       << " tex ptr:"    << m_vertices.begin()->tex << "\n";
  */
  /*
  fprintf(stderr,"vtx: %d face:%d gap:%d size:%d\n",m_vertices.size(),m_faces.size(),
	  ((char *)(m_vertices.begin() + 1)) - ((char *)(m_vertices.begin())),
	  sizeof(he_vertex));

  vector<he_vertex>::iterator vertex;
  for(vertex = m_vertices.begin();vertex != m_vertices.end();vertex++)
    {
      cerr << "[" << vertex - m_vertices.begin() << "] "; APrint4D(vertex->V); cerr << endl;
    }
  */
  p_scene->AddTriangles(m_faces.size(),m_indices.begin(),
			m_vertices.begin()->V,sizeof(he_vertex),
			m_vertices.begin()->N,sizeof(he_vertex),
			m_vertices.begin()->tex,sizeof(he_vertex));
}

/*
 * GetBoundingBox - copy precalculated bounding box
 */
void HalfEdgeMesh::GetBoundingBox(real *min,real *max) const
{
  ACopy3D(m_min,min);
  ACopy3D(m_max,max);
}

/*
 * Calculate the bounding box and store them for later
 */
void HalfEdgeMesh::determine_volume(real *min,real *max)
{
  // integer i;
  vector<he_vertex>::iterator vertex;
  
  if(m_vertices.size() > 0)
    {
      vertex = m_vertices.begin();
      ACopy3D(vertex->V,min);
      ACopy3D(vertex->V,max);
      // itterate over all vertices 
      do{
	AMaxElements3D(vertex->V,max,max);
	AMinElements3D(vertex->V,min,min);
	vertex++;
      }while(vertex != m_vertices.end());
    }
  else
    {
      min[0] = min[1] = min[2] = -1;
      max[0] = max[1] = max[2] = 1;
    }
}

/*
 * MatchPoint - determine if this point already exists
 */
vector<he_vertex>::iterator HalfEdgeMesh::matchpoint(vector<he_vertex>::iterator pt)
{
  vector<he_vertex>::iterator vertex,match;

  match = NULL;
  for(vertex = m_vertices.begin();vertex != pt;vertex++)
    {
      if(ACompare3D(pt->V,vertex->V) == true)
	match = vertex;
    }
  for(vertex = pt + 1;vertex != m_vertices.end();vertex++)
    {
      if(ACompare3D(pt->V,vertex->V) == true)
	match = vertex;
    }
  if(match != NULL)
    fprintf(stderr,"found %p = %p\n",pt,match);
  return match;
}

/*
 * Create a full edge using two half-edges
 */
vector<he_edge>::iterator HalfEdgeMesh::createedge(vector<he_vertex>::iterator v1,
						   vector<he_vertex>::iterator v2)
{
  vector<he_edge>::iterator e1,e2;

  e1 = createhalfedge(v1,NULL,NULL,NULL,false);
  v1->edge = e1;

  e2 = createhalfedge(v2,NULL,NULL,e1,false);
  v2->edge = e2;
  
  e1->pair = e1;

  return e2;
}

/*
 * finds the edge containing the 2 vertices
 */
vector<he_edge>::iterator HalfEdgeMesh::findedge(vector<he_vertex>::iterator v1,
						 vector<he_vertex>::iterator v2)
{
  vector<he_edge>::iterator edge;

  //fprintf(stderr,"searching for an edge of %d:<%p,%p>\n",m_edges.size(),v1,v2);
  for(edge = m_edges.begin();edge != m_edges.end();edge++)
    {
      //printedge(edge); cerr << endl;
      //fprintf(stderr,"edge->next:%p edge->prev:%p",edge->next,edge->prev);
      //fprintf(stderr,"edge->next->vertex:%p",edge->next->vertex);
      if((edge->vertex == v1) && (edge->next != NULL) && (edge->next->vertex == v2))
	return edge;
    }

  return NULL;
}

/*
 * deleteface - remove face from half-edge mesh
 */
void HalfEdgeMesh::deleteface(vector<he_face>::iterator face)
{
  vector<he_edge>::iterator edge,next;

  fprintf(stderr,"deleting face %p\n",face);
  // delete all half-edges in the face
  edge = face->edge; 
  do{
    next = edge->next;
    if(edge->pair != NULL) // if there is a neighbouring half-edge
      edge->pair->pair = NULL;
    edge = edge->next;
    m_edges.erase(edge);
    edge = next;
  }while(face->edge != edge);
  m_faces.erase(face); // remove face
}

/*
 * deletes an edge
 */
void HalfEdgeMesh::deleteedge(vector<he_edge>::iterator edge)
{
  bool del = false;
  vector<he_face>::iterator face;
  vector<he_edge>::iterator e; //,n;

  fprintf(stderr,"delete edge %p\n",edge);
 
  // delete any faces which use this edge
  for(face = m_faces.begin();face != m_faces.end();face++)
    {
      e = face->edge;
      do{
	if(e == edge)
	  deleteface(edge->face);
      }while((e != face->edge) && (e != NULL) && (del == false));
    }
  
  // remove edge
  m_edges.erase(edge);
  fprintf(stderr,"done\n");
}

/*
 *  print list of edges
 */
void HalfEdgeMesh::printedgelist(void)
{
  // integer i;
  vector<he_edge>::iterator edge;
  
  fprintf(stderr,"(id)\tvertex\tpair\tface\n");
  for(edge = m_edges.begin();edge != m_edges.end();edge++)
    {
      fprintf(stderr,"\t"); printedge(edge);
    }
}

/*
 * print data about edge
 */
void HalfEdgeMesh::printedge(vector<he_edge>::iterator edge)
{
  if(edge != NULL)
    {
      /*
      fprintf(stderr,"%d: vertex:%d pair:%d face:%d oface:%d next:%d prev:%d ",
	      edge - m_edges.begin(),
	      edge->vertex - m_vertices.begin(),
	      edge->pair - m_edges.begin(),
	      edge->face - m_faces.begin(),
	      ((edge->pair != NULL) ? edge->pair->face - m_faces.begin() : -1),
	      edge->next - m_edges.begin(),edge->prev - m_edges.begin());
      */
      fprintf(stderr,"%d: %d %d %d %d %d %d ",
	      edge - m_edges.begin(),
	      edge->vertex - m_vertices.begin(),
	      ((edge->pair != NULL) ? edge->pair - m_edges.begin() : -1),
	      ((edge->face != NULL) ? edge->face - m_faces.begin() : -1),
	      ((edge->pair != NULL) ? edge->pair->face - m_faces.begin() : -1),
	      edge->next - m_edges.begin(),
	      edge->prev - m_edges.begin());
      APrint3D(edge->vertex->V);
      fprintf(stderr,"\n");
    }
}

/*
 * print a vertex
 */
void HalfEdgeMesh::printvertex(vector<he_vertex>::iterator vertex)
{
  if(vertex != NULL)
    {
      fprintf(stderr,"id:%d edge:%d ",vertex - m_vertices.begin(),vertex->edge - m_edges.begin());
      APrint3D(vertex->V);
      fprintf(stderr,"\n");
    }
}

/*
 * print a list of vertices
 */
void HalfEdgeMesh::printvertexlist()
{
  vector<he_vertex>::iterator vertex;

  for(vertex = m_vertices.begin();vertex != m_vertices.end();vertex++)
    printvertex(vertex);
}

/*
 * print connected vertices
 */
void HalfEdgeMesh::printconnected(vector<he_vertex>::iterator vertex)
{
  vector<he_edge>::iterator edge,next;

  edge = vertex->edge;
  do{
    next = edge->next;
    fprintf(stderr,"%d ",next->vertex - m_vertices.begin());
    edge = edge->pair;
    if(edge != NULL)
      edge = edge->next;
  }while((edge != vertex->edge) && (edge != NULL));
}

/*
 * 
 */
void HalfEdgeMesh::printfacelist(void)
{
  vector<he_face>::iterator face;

  for(face = m_faces.begin();face != m_faces.end();face++)
    printface(face);
}

/*
 * 
 */
void HalfEdgeMesh::printface(vector<he_face>::iterator face)
{
  vector<he_edge>::iterator edge;

  fprintf(stderr,"face:%d\n\t(id)\tvertex\tpair\tface\tnext\tprev\n",face - m_faces.begin());
  if((edge = face->edge) != NULL)
    {
      do{
	cerr << "\t"; printedge(edge); cerr << endl;
	edge = edge->next;
      }while(edge != face->edge);
    }
}

/*
 * splits an edge with the given vertex to produce 2 edges
 */
void HalfEdgeMesh::splitedge(vector<he_edge>::iterator edge,
			     vector<he_vertex>::iterator vertex)
{
  vector<he_edge>::iterator he1,he2,he3,he4;

  he1 = edge;
  he2 = he1->pair;

  // create new half-edge 1
  he3 = createhalfedge(vertex,he1,he1->next,he2,he1->crease);
  he3->face = he1->face;
  vertex->edge = he3;
  he1->next->prev = he3;
  he1->next = he3;

  // create new half-edge 2
  if(he2 != NULL)
    {
      he4 = createhalfedge(vertex,he2,he2->next,he1,he2->crease);
      he4->face = he2->face;
      he2->next->prev = he4;

      // update old edges
      he2->pair = he3;
      he2->next = he4;
      he1->pair = he4;
    }
}

/*
 *  splits face along vertices v1 and v2
 */
vector<he_face>::iterator HalfEdgeMesh::splitface(vector<he_face>::iterator face,
						  vector<he_edge>::iterator e1,
						  vector<he_edge>::iterator e2)
{
  vector<he_vertex>::iterator v1,v2;
  vector<he_edge>::iterator ne1,ne2;
  he_face newface;

  // create new face
  face->edge = e1;
  newface.edge = e2;
  //cerr << "spliting from " << ((e1 != NULL) ? e1 - m_edges.begin() : -1) << "[" << e1 << "]"
  //     << " to "           << ((e2 != NULL) ? e2 - m_edges.begin() : -1) << "[" << e2 << "]" << endl;

  // determine vertices to span with new edges
  v1 = e1->next->vertex;
  v2 = e2->next->vertex;
  //cerr << "\tv1:" << v1 - m_vertices.begin() << "\tv2:" << v2 - m_vertices.begin() << endl;

  // create new edges to split face with
  ne1 = createhalfedge(v2,e2,e1->next,NULL,false);
  ne2 = createhalfedge(v1,e1,e2->next,ne1,false);
  ne1->pair = ne2;

  // link in new edges
  e1->next->prev = ne1;
  e2->next->prev = ne2;

  // link in old edges
  e1->next = ne2;
  e2->next = ne1;

  // assign edges to correct face
  ne2->face = face;
  m_faces.push_back(newface);

  ne1->face = m_faces.end() - 1;
  ne1->next->face = m_faces.end() - 1;
  e2->face = m_faces.end() - 1;

  return m_faces.end() - 1;
}

/*
 * generate normals for each point
 */
void HalfEdgeMesh::normals(void)
{
  integer k;
  real N[3];
  vector<he_face>::iterator face;
  vector<he_edge>::iterator edge;
  vector<he_vertex>::iterator vertex;

  for(face = m_faces.begin();face != m_faces.end();face++)
    facenormal(face,face->N);

  // calculate the normal for each point by averageing the face normals
  // that use this vertex
  //fprintf(stderr,"cycle through %ld points\n",m_vertices.size());
  for(vertex = m_vertices.begin();vertex < m_vertices.end();vertex++)
    {
      // determine valence
      k = 0;
      if((edge = vertex->edge) != NULL)
	{
	  N[0] = N[1] = N[2] = 0.0;
	  //fprintf(stderr,"(%ld) {",i);
	  do{
	    edge = edge->pair;
	    if(edge != NULL)
	      {
		if((face = edge->face) != NULL)
		  {
		    //facenormal(mp_edges[e].face,tmp);
		    //fprintf(stderr,"(%ld,%ld) ",edge,edge->next);
		    //APrint3D(tmp);
		    AAdd3D(face->N,N,N);
		  }
		edge = edge->next;
		k++;
	      }
	  }while((edge != vertex->edge) && (edge != NULL));
	  // if the edge is NULL we must wind the other way around the vertex
	  if(edge == NULL)
	    {
	      edge = vertex->edge;
	      do{
		if((face = edge->face) != NULL)
		  {
		    //facenormal(mp_edges[e].face,tmp);
		    //fprintf(stderr,"(%ld,%ld) ",edge,edge->next);
		    //APrint3D(tmp);
		    AAdd3D(face->N,N,N);
		  }
		edge = edge->prev->pair;
		k++;
	      }while((edge != vertex->edge) && (edge != NULL));
	    }
	  //fprintf(stderr,"\n");
	  //ASDivide3D(k,N,N);
	  ANormalise3D(N,vertex->N);
	  //cerr << "<" << vertex - m_vertices.begin() << "> ";
	  //APrint3D(vertex->N);
	  //cerr << endl;
	}
      else
	{
	  vertex->N[0] = 1.0;
	  vertex->N[1] = 0.0;
	  vertex->N[2] = 0.0;
	}
      //fprintf(stderr,"valence:%ld N:",k);
      //APrint3D(N);
      //fprintf(stderr,"\n");
    }  
}

/*
 * calculate the normal for a face
 */
void HalfEdgeMesh::facenormal(vector<he_face>::iterator face,real *N)
{
  vector<he_edge>::iterator e1,e2,e3;
  real V1[3],V2[3];

  e1 = face->edge;
  e2 = e1->next;
  e3 = e2->next;
  ASubtract3D(e2->vertex->V,e1->vertex->V,V1);
  ASubtract3D(e3->vertex->V,e1->vertex->V,V2);
  ANormalise3D(V1,V1);
  ANormalise3D(V2,V2);
  ACrossProduct3D(V1,V2,N);
  ANormalise3D(N,N);
}

/*
 *  fill in an already existing face element
 */
vector<he_face>::iterator HalfEdgeMesh::createface(vector<he_edge>::iterator e1,
						   vector<he_edge>::iterator e2,
						   vector<he_edge>::iterator e3)
{
  he_face f;

  // link up each edge to its next and previous edge
  e1->next  = e2;
  e2->next  = e3;
  e3->next  = e1;
  
  e1->prev  = e3;
  e2->prev  = e1;
  e3->prev  = e2;

  // link face to first edge
  f.edge = e1;
  m_faces.push_back(f);

  // link each edge to the new face
  e1->face = m_faces.end() - 1;
  e2->face = m_faces.end() - 1;
  e3->face = m_faces.end() - 1;
  
  return m_faces.end() - 1;
}

/*
 *  fill in an already existing face element
 */
vector<he_face>::iterator HalfEdgeMesh::createface(vector<he_edge>::iterator e)
{
  he_face f;
  vector<he_edge>::iterator edge;

  // link face to first edge
  f.edge = e;
  m_faces.push_back(f);
  
  // link each edge to the new face
  edge = e;
  do{
     edge->face = m_faces.end() - 1;
     edge = edge->next;
  }while(e != edge);


  return m_faces.end() - 1;
}

/*
 * create a half-edge
 */
vector<he_edge>::iterator HalfEdgeMesh::createhalfedge(vector<he_vertex>::iterator vertex,
						       vector<he_edge>::iterator prev,
						       vector<he_edge>::iterator next,
						       vector<he_edge>::iterator pair,
						       bool crease)
{
  he_edge e;

  e.vertex = vertex;
  e.prev = prev;
  e.next = next;
  e.pair = pair;
  e.crease = crease;
  m_edges.push_back(e);

  vertex->edge = m_edges.end() - 1;
  //cerr << " "<< vertex->edge - m_edges.begin();

  return m_edges.end() - 1;
}

/*
 * create a half-edge
 */
vector<he_edge>::iterator HalfEdgeMesh::createhalfedge(vector<he_vertex>::iterator vertex)
{
  he_edge e;

  //fprintf(stderr,"vertex:%p\n",vertex);
  e.vertex = vertex;
  e.prev = NULL;
  e.next = NULL;
  e.pair = NULL;
  e.crease = false;
  m_edges.push_back(e);

  vertex->edge = m_edges.end() - 1;
  //cerr << " " << vertex->edge - m_edges.begin();

  return m_edges.end() - 1;
}

/*
 * Load - Load in a OFF or NOFF mesh
 */
void HalfEdgeMesh::Load(char *filename)
{
  char type[128],line[1024];
  real t;
  integer vertices,faces,edges,i,face_size,v1,v2,v3;
  FILE *fp = fopen(filename,"r");
  assert(fp);
  he_vertex v;
  // he_edge e;
  // he_face f;
  vector<he_edge>::iterator e1,e2,e3,pair;
  vector<he_face>::iterator face;
  // vector<he_vertex>::iterator vertex;
 
  fgets(line,1024,fp);
  sscanf(line,"%s",type);
  fgets(line,1024,fp);
  sscanf(line,"%d %d %d",&vertices,&faces,&edges);
  fprintf(stderr,"vertices:%d faces:%d edges:%d\n",vertices,faces,edges);
  
  // resize vectors to hold data
  m_vertices.reserve(vertices + 10);
  m_faces.reserve(faces + 10);
  m_edges.reserve(faces*4 + 10);

  fprintf(stderr,"vertices\n");
  if(strcmp(type,"OFF") == 0) // without normals
    {
      // read in vertices
      for(i = 0;i < vertices;i++)
	{
	  fgets(line,1024,fp);
	  sscanf(line,"%lf %lf %lf",
		 v.V + 0,
		 v.V + 1,
		 v.V + 2);
	  v.V[3] = 1;
	  APrint4D(v.V); cerr << endl;
	  // assign random texture coord (you may want to remove this)
	  v.tex[0] = 4.0/double((rand() % 100) + 1);
	  v.tex[1] = 4.0/double((rand() % 100) + 1);
	  //cerr << "(" << v.tex[0] << " " << v.tex[1] << ")\n";
	  v.edge = NULL;
	  //printvertex(&v);
	  m_vertices.push_back(v);
	  //cerr << i << endl;
	}
      //cerr << endl;
    }
  else if(strcmp(type,"NOFF") == 0) // with normals
    {
      // read in vertices
      for(i = 0;i < vertices;i++)
	{
	  fgets(line,1024,fp);
	  
	  sscanf(line,"%lf %lf %lf %lf %lf %lf",
		 v.V + 0,
		 v.V + 1,
		 v.V + 2,&t,&t,&t);
	  v.V[3] = 1;
	  v.edge = NULL;
	  m_vertices.push_back(v);
	  //cerr << i << endl;
	}
    }

  // read in faces, creating edges and faces as we go... .  .   .
  fprintf(stderr,"read faces\n");
  for(i  = 0;i < faces;i++)
    {
      fgets(line,1024,fp);
      sscanf(line,"%d %d %d %d",&face_size,&v1,&v2,&v3);
      cerr << v1 << " " << v2 << " " << v3 << endl;
      // build face
      //cerr << i << " { ";
      e1 = createhalfedge(&m_vertices[v1]);
      e2 = createhalfedge(&m_vertices[v2]);
      e3 = createhalfedge(&m_vertices[v3]);
      //cerr << "}\n";
      //fprintf(stderr,">>>>edges:%d<<<<\n(%d) %p %p %p %p\n edge e1:",m_edges.size(),i,face_size,e1,e2,e3);
      //printedge(e1); cerr << endl;
      face = createface(e1,e2,e3);
      //cerr << "e1:"; printedge(e1); cerr << endl;
      //fprintf(stderr,"(%d) %d %p %p %d\n",i,face_size,e1->next,e2->next,e3->next);
      //cerr << "created face " << face << "\n";
      //printface(face); cerr << endl;
      //printedgelist();
      //cerr << "---------------------------------\n";
      // find edge pairs if they exist?
      pair = findedge(&m_vertices[v2],&m_vertices[v1]);
      //cerr << "found edge\n";
      if(pair != NULL)
	{
	  e1->pair = pair;
	  pair->pair = e1;
	}
      //cerr << "find possible pair 1\n";
      pair = findedge(&m_vertices[v3],&m_vertices[v2]);
      if(pair != NULL)
	{
	  e2->pair = pair;
	  pair->pair = e2;
	}
      //cerr << "find possible pair 2\n";
      pair = findedge(&m_vertices[v1],&m_vertices[v3]);
      if(pair != NULL)
	{
	  e3->pair = pair;
	  pair->pair = e3;
	}
      //cerr << "find possible pair 3\n";

      // calculate normal
      facenormal(face,face->N);
      //cerr << "determine face normal\n";
      face->colour[0] = 1.0;
      face->colour[1] = 1.0;
      face->colour[2] = 1.0;
      face->fid = 0;
    }
  //printvertexlist();
  //printedgelist();
  //printfacelist();
  fprintf(stderr,"loaded faces:%d edges:%d vertices:%d\n",m_faces.size(),m_edges.size(),m_vertices.size());
  fclose(fp);
  //printfacelist();
  normals();
}

/*
 * Save - Save mesh back to a file
 */
void HalfEdgeMesh::Save(char *filename)
{
  FILE *fp = fopen(filename,"w");
  vector<integer> indices;
  vector<integer>::iterator index;
  vector<he_face>::iterator face;
  vector<he_edge>::iterator edge;
  vector<he_vertex>::iterator vertex;

  assert(fp != NULL);
  
  fprintf(fp,"OFF\n");
  fprintf(fp,"%d %d %d\n",m_vertices.size(),m_faces.size(),m_edges.size()/2);
  // save vertices
  for(vertex = m_vertices.begin();vertex != m_vertices.end();vertex++)
    {
      fprintf(fp,"%d %f %f %f\n",vertex - m_vertices.begin(),vertex->V[0],vertex->V[1],vertex->V[2]);
    }
  // save faces
  for(face = m_faces.begin();face != m_faces.end();face++)
    {
      indices.clear();
      edge = face->edge;
      //cerr << "{";
      do{
	indices.push_back(edge->vertex - m_vertices.begin());
	//cerr << " " << edge->vertex - m_vertices.begin();
	edge = edge->next;
      }while(face->edge != edge);
      //cerr << "}\n";
      fprintf(fp,"\t%d",indices.size());
      for(index = indices.begin();index != indices.end();index++)
	fprintf(fp," %d",(*index));
      fprintf(fp,"\n");
    }
  fclose(fp);
}

/*
 * Copy - re-create a mesh
 */
bool HalfEdgeMesh::copy(HalfEdgeMesh *p_mesh)
{
  vector<he_vertex> *p_vertices = p_mesh->GetVertices();
  vector<he_edge> *p_edges = p_mesh->GetEdges();
  vector<he_face> *p_faces = p_mesh->GetFaces();

  vector<he_vertex>::iterator vertex;
  vector<he_edge>::iterator edge;
  vector<he_face>::iterator face;

  // copy vertices
  for(vertex = p_vertices->begin();vertex != p_vertices->end();vertex++)
    {
      m_vertices.push_back(*vertex);
    }

  // recreate edges
  for(edge = p_edges->begin();edge != p_edges->end();edge++)
    {
      createhalfedge(&m_vertices[edge->vertex - p_vertices->begin()]);
    }
  
  //link edges together
  for(edge = p_edges->begin();edge != p_edges->end();edge++)
    {
      m_edges[edge - p_edges->begin()].next = &m_edges[edge->next - p_edges->begin()];
      m_edges[edge - p_edges->begin()].prev = &m_edges[edge->prev - p_edges->begin()];
      m_edges[edge - p_edges->begin()].pair = &m_edges[edge->pair - p_edges->begin()];
      m_edges[edge - p_edges->begin()].vertex = &m_vertices[edge->vertex - p_vertices->begin()];
    }

  // recreate faces
  for(face = p_faces->begin();face != p_faces->end();face++)
    {
      createface(&m_edges[face->edge - p_edges->begin()]);
    }

  // link vertices to edges
  for(vertex = p_vertices->begin();vertex != p_vertices->end();vertex++)
    {
      m_vertices[vertex - p_vertices->begin()].edge = &m_edges[vertex->edge - p_edges->begin()];
    }

  return true;
}

/*
 * EdgeCount - count the number of edges in a face
 */
integer HalfEdgeMesh::edgecount(vector<he_face>::iterator face)
{
  integer count = 0;
  vector<he_edge>::iterator edge;

  edge = face->edge;
  do{
    count++;
    edge = edge->next;
  }while(edge != face->edge);

  return count;
}








