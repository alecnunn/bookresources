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
//  pofObject.cpp
//    Evan Hart - ATI Research Inc.
//
//   This is the base class for gl-based pof objects.
//
/////////////////////////////////////////////////////////////////////

#include <string.h>
#include <gl/glut.h>

#include "pofObject.h"

#include "pof.h"

#include "glati.h"
#include "wglati.h"
#include "Extension.h"



vertexList::vertexList(){
  m_name=0;
  m_count=0;
  m_verts=NULL;
  m_vComponents=0;
  m_norms=0;
  m_texCoords=NULL;
  m_tComponents=NULL;
  m_tInstances=NULL;
  m_tSets=0;
  m_colors=NULL;
  m_cComponents=NULL;
  m_cInstances=NULL;
  m_cSets=0;
}

vertexList::~vertexList(){
  int ii;

  if (m_verts)
    delete []m_verts;

  for (ii=0;ii<m_tSets;ii++){
    if (m_texCoords[ii])
      delete []m_texCoords[ii];
  }

  if (m_texCoords)
    delete []m_texCoords;

  if (m_tComponents)
    delete []m_tComponents;

  if (m_tInstances)
    delete []m_tInstances;

  for (ii=0;ii<m_cSets;ii++){
    if (m_colors[ii])
      delete []m_colors[ii];
  }

  if (m_colors)
    delete []m_colors;

  if (m_cComponents)
    delete []m_cComponents;

  if (m_cInstances)
    delete []m_cInstances;

}

void vertexList::loadVertices(POF* pof, unsigned int name){
  POFVertexChunk vChunk;
  POFNormalChunk nChunk;
  POFTexCoordChunk tChunk;
  POFColorChunk cChunk;
  unsigned int format, type;
  const void *data;
  float *tempDat;
  int ii,gg;

  m_name=name;
  
  //
  //load the vertex data
  //
  if (!(data=pofGetVertexList(pof,name,0,&vChunk)))
    return;

  format=vChunk.format & 0xffff;
  type=(vChunk.format & 0xffff0000) >> 16;

  if (type!=POF_FLOAT)
    return; //only handle floats

  switch (format){
    case POF_VERTEX_2:
      m_vComponents=2;
      break;
    case POF_VERTEX_3:
      m_vComponents=3;
      break;
    case POF_VERTEX_4:
      m_vComponents=4;
      break;
    default:
      return; //bad vertex data
  };

  m_count=vChunk.count;
  tempDat=(float *)data;
  m_verts=new float[m_count*m_vComponents];

  memcpy(m_verts,tempDat,sizeof(float)*m_count*m_vComponents);

  //
  //load the normal list
  //
  if (data=pofGetNormalList(pof,name,0,&nChunk)){

    format=nChunk.format & 0xffff;
    type=(nChunk.format & 0xffff0000) >> 16;

    if (type!=POF_FLOAT)
      goto END_NORMALS; //can't handle non-float normals

    if (format!=POF_NORMAL_3)
      goto END_NORMALS; //can only handle 3 component normals

    if (nChunk.count!=m_count)
      goto END_NORMALS; // # normals != # num vertices
    
    tempDat=(float *)data;
    m_norms=new float[m_count*3];
    memcpy(m_norms,tempDat,sizeof(float)*m_count*3);
END_NORMALS:;
  }

  //
  //load the texture coord list(s)
  //
  if (m_tSets=pofGetNumInstances(pof,POF_TYPE_TEX_COORD,name)){
    m_texCoords=new float* [m_tSets];
    m_tComponents=new int[m_tSets];
    m_tInstances=new unsigned int[m_tSets];

    for (ii=0; ii<m_tSets; ii++){
      m_texCoords[ii]=NULL;
      m_tComponents[ii]=0;
      m_tInstances[ii]=pofGetInstances(pof,POF_TYPE_TEX_COORD,name)[ii];

      data=pofGetTexCoordList(pof,name,m_tInstances[ii],&tChunk);
      
      if (!data)
        break;

      if (m_count!=tChunk.count)
        break;
      
      format=tChunk.format & 0xffff;
      type=(tChunk.format & 0xffff0000) >> 16;

      if (type!=POF_FLOAT)
        break; //can only handle float tex coords

      tempDat=(float *)data;

      switch (format){
        case POF_TEXCOORD_S:
          m_tComponents[ii]=1;
          m_texCoords[ii]=new float[m_count];
          memcpy(m_texCoords[ii],tempDat,sizeof(float)*m_count);
          break;
        case POF_TEXCOORD_ST:
          m_tComponents[ii]=2;
          m_texCoords[ii]=new float[m_count*2];
          memcpy(m_texCoords[ii],tempDat,sizeof(float)*m_count*2);
          break;
        case POF_TEXCOORD_STR:
          m_tComponents[ii]=3;
          m_texCoords[ii]=new float[m_count*3];
          memcpy(m_texCoords[ii],tempDat,sizeof(float)*m_count*3);
          break;
        case POF_TEXCOORD_STRQ:
          m_tComponents[ii]=4;
          m_texCoords[ii]=new float[m_count*4];
          memcpy(m_texCoords[ii],tempDat,sizeof(float)*m_count*4);
          break;
        case POF_TEXCOORD_STQ:
          m_tComponents[ii]=4;
          m_texCoords[ii]=new float[m_count*4];
          for (gg=0; gg<m_count; gg++){
            m_texCoords[ii][gg*4+0]=tempDat[gg*3+0];
            m_texCoords[ii][gg*4+1]=tempDat[gg*3+1];
            m_texCoords[ii][gg*4+2]=0.0f;
            m_texCoords[ii][gg*4+3]=tempDat[gg*3+2];
          }
          break;
      }

    }
  }

  //
  //load the color list(s)
  //
  if (m_cSets=pofGetNumInstances(pof,POF_TYPE_COLOR,name)){
    m_colors=new float* [m_cSets];
    m_cComponents=new int[m_cSets];
    m_cInstances=new unsigned int[m_cSets];

    for (ii=0; ii<m_cSets; ii++){
      m_colors[ii]=NULL;
      m_cComponents[ii]=0;
      m_cInstances[ii]=pofGetInstances(pof,POF_TYPE_COLOR,name)[ii];

      data=pofGetColorList(pof,name,m_cInstances[ii],&cChunk);
      
      if (!data)
        break;

      if (m_count!=cChunk.count)
        break;
      
      format=cChunk.format & 0xffff;
      type=(cChunk.format & 0xffff0000) >> 16;

      if (type!=POF_FLOAT)
        break; //can only handle float colors

      tempDat=(float *)data;

      switch (format){
        case POF_COLOR_RGB:
          m_cComponents[ii]=3;
          m_colors[ii]=new float[m_count*3];
          memcpy(m_colors[ii],tempDat,sizeof(float)*m_count*3);
          break;
        case POF_COLOR_RGBA:
          m_cComponents[ii]=4;
          m_colors[ii]=new float[m_count*4];
          memcpy(m_colors[ii],tempDat,sizeof(float)*m_count*4);
          break;
      }

    }
  }
  
}

int vertexList::getNumVertices(){
  return m_count;
}

float *vertexList::getTexCoords(unsigned int which){
  int ii;

  for (ii=0;ii<m_tSets;ii++){
    if (m_tInstances[ii]==which)
      break;
  }

  if (ii==m_tSets)
    return NULL;

  return m_texCoords[ii];
}

int vertexList::getNumTexCoordComponents(unsigned int which){
  int ii;

  for (ii=0;ii<m_tSets;ii++){
    if (m_tInstances[ii]==which)
      break;
  }

  if (ii==m_tSets)
    return 0;

  return m_tComponents[ii];
}

float *vertexList::getNormals(){
  return m_norms;
}

float *vertexList::getVertices(){
  return m_verts;
}

float *vertexList::getColors(unsigned int which){
  int ii;

  for (ii=0;ii<m_cSets;ii++){
    if (m_cInstances[ii]==which)
      break;
  }

  if (ii==m_cSets)
    return NULL;

  return m_colors[ii];
}

int vertexList::getNumColorComponents(unsigned int which){
  int ii;

  for (ii=0;ii<m_cSets;ii++){
    if (m_cInstances[ii]==which)
      break;
  }

  if (ii==m_cSets)
    return 0;

  return m_cComponents[ii];
}


faceSet::faceSet(){
  m_name=0;
  m_indices=NULL;
  m_count=0;
  m_vtx=NULL;
  m_coordSet=0;
}

faceSet::~faceSet(){
  if (m_indices)
    delete []m_indices;
}

void faceSet::loadFaceSet(POF* pof, unsigned int name){
  POFFaceChunk fChunk;
  const void *data;
  unsigned int *tempDat;

  if (data=pofGetFaceList(pof,name,&fChunk)){
    m_count=fChunk.count;
    m_coordSet=fChunk.coordSet;

    m_indices=new unsigned int[m_count*3];
    tempDat=(unsigned int *)data;

    memcpy(m_indices,tempDat,sizeof(unsigned int)*m_count*3);
  }
}

unsigned int* faceSet::getIndices(){
  return m_indices;
}

int faceSet::getNumFaces(){
  return m_count;
}

unsigned int faceSet::getCoordSet(){
  return m_coordSet;
}

vertexList *faceSet::getVertexList(){
  return m_vtx;
}

void faceSet::setVertexList(vertexList *v){
  m_vtx=v;
}



pofObject::pofObject(){
  m_faces=NULL;
  m_verts=NULL;
  m_numObjects=0;
  m_numLists=0;
  m_hasBBox=false;
  m_hasNumPolys=false;
}

pofObject::~pofObject(){
  
  while (m_numObjects)
    delete m_faces[--m_numObjects];

  while (m_numLists)
    delete m_verts[--m_numLists];

  delete []m_faces;
  delete []m_verts;
}

void pofObject::loadObject(char *fn){
  POF *pof;
  int ii,gg;
  faceSet **tempf;
  vertexList **tempv;

  pof=pofOpen(fn,POF_MODE_READ);

  for (ii=0;ii<pofGetNumObjects(pof,POF_TYPE_FACE);ii++){
    if (!(m_numObjects%5)){
      tempf=new faceSet*[m_numObjects+5];
      memcpy(tempf,m_faces,sizeof(faceSet*)*m_numObjects);
      delete []m_faces;
      m_faces=tempf;
    }
    m_faces[m_numObjects]=new faceSet();
    m_faces[m_numObjects]->loadFaceSet(pof,pofGetObjectNames(pof,POF_TYPE_FACE)[ii]);

    for (gg=0;gg<m_numLists;gg++){
      if (m_faces[m_numObjects]->m_coordSet==m_verts[gg]->m_name)
        break;
    }

    if (gg==m_numLists){
      if (!(m_numLists%5)){
        tempv=new vertexList* [m_numLists+5];
        memcpy(tempv,m_verts,sizeof(vertexList*)*m_numLists);
        delete []m_verts;
        m_verts=tempv;
      }
      m_verts[m_numLists]=new vertexList();
      m_verts[m_numLists]->loadVertices(pof,m_faces[m_numObjects]->m_coordSet);
      m_faces[m_numObjects]->m_vtx=m_verts[m_numLists];
      m_numLists++;
    }
    else
      m_faces[m_numObjects]->m_vtx=m_verts[gg];

    m_numObjects++;
  }
  pofClose(pof);
}

void pofObject::getBBox(float *bbox){
  vertexList *v;
  int ii,gg,hh;

  if (!m_hasBBox){
    m_bBox[0]=100000.0f;
    m_bBox[1]=100000.0f;
    m_bBox[2]=100000.0f;
    m_bBox[3]=-100000.0f;
    m_bBox[4]=-100000.0f;
    m_bBox[5]=-100000.0f;
    for (ii=0;ii<m_numLists;ii++){
      v=m_verts[ii];
      for (gg=0;gg<v->m_count;gg++){
        for (hh=0;hh<v->m_vComponents;hh++){
          if (m_bBox[hh]>v->m_verts[gg*v->m_vComponents+hh])
            m_bBox[hh]=v->m_verts[gg*v->m_vComponents+hh];
          if (m_bBox[hh+3]<v->m_verts[gg*v->m_vComponents+hh])
            m_bBox[hh+3]=v->m_verts[gg*v->m_vComponents+hh];
        }
      }
    }
  }
  m_hasBBox=true;
  memcpy(bbox,m_bBox,sizeof(float)*6);
}

int pofObject::numPolygons(){
  int ii;

  if (!m_hasNumPolys){
    m_numPolys=0;
    for (ii=0;ii<m_numObjects;ii++){
      m_numPolys+=m_faces[ii]->getNumFaces();
    }
    m_hasNumPolys=true;
  }

  return m_numPolys;
}

void pofObject::renderObject(){
  int ii;
  vertexList *v;
  float *tex;

  glEnableClientState(GL_VERTEX_ARRAY);

  for (ii=0;ii<m_numObjects;ii++){
    v=m_faces[ii]->getVertexList();
    glVertexPointer(v->m_vComponents,GL_FLOAT,0,v->m_verts);
    if (v->m_norms){
      glEnableClientState(GL_NORMAL_ARRAY);
      glNormalPointer(GL_FLOAT,0,v->m_norms);
    }
    if (v->m_cSets){
      glEnableClientState(GL_COLOR_ARRAY);
      glColorPointer(v->m_cComponents[0],GL_FLOAT,0,v->m_colors[0]);
    }
    if (tex=v->getTexCoords(POF_TCI_BASE)){
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(v->getNumTexCoordComponents(POF_TCI_BASE),GL_FLOAT,
                        0,tex);
    }
    //    glLockArraysEXT(0,v->m_count);
    glDrawElements(GL_TRIANGLES,m_faces[ii]->m_count*3,GL_UNSIGNED_INT,m_faces[ii]->m_indices);
    //    glUnlockArraysEXT();
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  }

  glDisableClientState(GL_VERTEX_ARRAY);
}

void pofObject::renderVectors(){
  float *vtx;
  float *nrm;
  float *tan;
  float *binorm;
  vertexList *v;
  int ii,gg;

  for (ii=0;ii<m_numObjects;ii++){
    v=m_faces[ii]->m_vtx;
    vtx=v->getVertices();
    nrm=v->getNormals();
    tan=v->getTexCoords(POF_TCI_TAN0);
    binorm=v->getTexCoords(POF_TCI_BINORM0);
    if (nrm){
      glColor3f(1.0f,0.0f,0.0f);
      glBegin(GL_LINES);
      for (gg=0;gg<v->getNumVertices();gg++){
        glVertex3fv(&vtx[gg*3]);
        glVertex3f(vtx[gg*3+0]+nrm[gg*3+0], vtx[gg*3+1]+nrm[gg*3+1],
                   vtx[gg*3+2]+nrm[gg*3+2]);
      }
      glEnd();
    }
    if (tan){
      glColor3f(0.0f,0.0f,1.0f);
      glBegin(GL_LINES);
      for (gg=0;gg<v->getNumVertices();gg++){
        glVertex3fv(&vtx[gg*3]);
        glVertex3f(vtx[gg*3+0]+tan[gg*3+0], vtx[gg*3+1]+tan[gg*3+1],
                   vtx[gg*3+2]+tan[gg*3+2]);
      }
      glEnd();
    }
    if (binorm){
      glColor3f(0.0f,1.0f,0.0f);
      glBegin(GL_LINES);
      for (gg=0;gg<v->getNumVertices();gg++){
        glVertex3fv(&vtx[gg*3]);
        glVertex3f(vtx[gg*3+0]+binorm[gg*3+0], vtx[gg*3+1]+binorm[gg*3+1],
                   vtx[gg*3+2]+binorm[gg*3+2]);
      }
      glEnd();
    }
  }
}
