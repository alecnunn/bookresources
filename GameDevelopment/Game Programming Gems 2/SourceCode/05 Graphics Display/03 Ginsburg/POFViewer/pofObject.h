/* Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001"
 */
//
//  pofObject.h
//    Evan Hart - ATI Research Inc.
//
//   This is the base class for gl-based pof objects.
//
/////////////////////////////////////////////////////////////////////

#ifndef POFOBJECT_H
#define POFOBJECT_H

#include "pof.h"

class vertexList{
  friend class pofObject;

  public:
    //name of this vertex list
    unsigned int m_name;

    //number of vertices
    int m_count;

    //vertex info
    float *m_verts;
    int m_vComponents;

    //normal info
    float *m_norms;

    //tex coord info
    float **m_texCoords;
    int *m_tComponents;
    unsigned int *m_tInstances;
    int m_tSets;

    //color info
    float **m_colors;
    int *m_cComponents;
    unsigned int *m_cInstances;
    int m_cSets;

  public:
    vertexList();
    ~vertexList();

    void loadVertices(POF* pof, unsigned int name);
    int getNumVertices();
    float *getTexCoords(unsigned int which);
    int getNumTexCoordComponents(unsigned int which);
    float *getNormals();
    float *getVertices();
    float *getColors(unsigned int which);
    int getNumColorComponents(unsigned int which);
};

class faceSet{
  friend class pofObject;

  public:
    //name of the object
    unsigned int m_name;

    //index list to draw the object
    unsigned int *m_indices;
    int m_count;

    vertexList *m_vtx;
    unsigned int m_coordSet; 

  public:
    faceSet();
    ~faceSet();

    void loadFaceSet(POF* pof, unsigned int name);
    unsigned int* getIndices();
    int getNumFaces();
    unsigned int getCoordSet();
    vertexList *getVertexList();
    void setVertexList(vertexList *v);
};



class pofObject{
  protected:
    faceSet** m_faces;
    vertexList** m_verts;

    int m_numObjects;
    int m_numLists;

    bool m_hasBBox;
    float m_bBox[6];

    bool m_hasNumPolys;
    int m_numPolys;

  public:
    pofObject();
    virtual ~pofObject();

    virtual void loadObject(char *fn);
    virtual void renderObject();
    virtual void renderVectors();
    void getBBox(float *bbox);
    int numPolygons();
};

#endif
