/* Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001"
 */
///////////////////////////////////////////////////////////////////////////////
//
//   BumpObject.cpp - This file contains the members of the BumpObject.
//                    The object draws bumpmapped POF obects.
//
/////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <glut.h>
#include <stdio.h>
#include <math.h>
#include "glati.h"
#include "wglati.h"

#include "BumpObject.h"

// Prototype needed
extern PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTexture;

extern void matrixIdentity(GLfloat matrix[16]);
extern void matrixInvert(GLfloat in[16], GLfloat out[16]);
extern void vecMatMult(GLfloat vecIn[3], GLfloat m[16], GLfloat vecOut[3]);
extern void vecMat3x3Mult(GLfloat vecIn[3], GLfloat m[16], GLfloat vecOut[3]);

//////////////////////////
// Constructor handled in base class
///////////////////////////
BumpObject::BumpObject() : pofObject()
{
}

//////////////////////////
// Destructor -- nothing needed
///////////////////////////
BumpObject::~BumpObject()
{
}

//////////////////////////
// Addtional loading to make sure that all the memory we need has been
// allocated.
///////////////////////////
void 
BumpObject::loadObject(char *fn)
{
  // Load up the POF file using baseclass.
  pofObject::loadObject(fn);

  // Allocate color so we can use it for passing the light in tangent space
  for (int i = 0; i < m_numObjects; i++) {
    vertexList *v = m_faces[i]->getVertexList();
    if (v->m_cSets == 0) {
      v->m_cSets = v->m_tSets;
      v->m_colors = new float* [v->m_cSets];
      v->m_cComponents = new int[v->m_cSets];
      v->m_cInstances = new unsigned int[v->m_cSets];
      for (int i = 0; i < v->m_cSets; i++){
        v->m_cComponents[i] = 3;
        v->m_cInstances[i] = v->m_tInstances[i];
        v->m_colors[i] = new float[v->m_count*3];
      }    
    }
  }
}

//////////////////////////
// This method computes the lighting vector at each vertex in tangent space
// and packs this into the color. This will be dotted with the pre-filtered
// bump texture.
//   lightpos -- Postion of the the light in OBJECT space.
//   forCubemap -- Flag to say we are using a cubemap for normalization.
//////////////////////////
void 
BumpObject::computeLighting(GLfloat lightpos[3], GLuint forCubemap)
{
  // Loop through all the objects in the file.
  for (int i = 0; i < m_numObjects; i++) {
    vertexList *v = m_faces[i]->getVertexList();
    float *vtx = v->getVertices();

    // Make sure the vertex has all the required data
    float *nrm, *binorm, *tangent, *color;
    if ((nrm = v->getNormals()) && 
        (tangent = v->getTexCoords(POF_TCI_TAN0)) &&
        (binorm =  v->getTexCoords(POF_TCI_BINORM0)) &&
        (color = v->getColors(POF_CI_DIFFUSE)) ) { 
      int szVert = v->m_vComponents;
      int szTan = v->getNumTexCoordComponents(POF_TCI_TAN0);
      int szBN = v->getNumTexCoordComponents(POF_TCI_BINORM0);
      int szClr = v->getNumColorComponents(POF_CI_DIFFUSE);
      int szNorm = 3;

      // Loop over all the vertices
      for (int j = 0; j < v->getNumVertices(); j++) {

        // Compute light vector in OBJECT space.
        GLfloat lv[3];
        lv[0] = (lightpos[0] - vtx[j*szVert + 0]);
        lv[1] = (lightpos[1] - vtx[j*szVert + 1]);
        lv[2] = (lightpos[2] - vtx[j*szVert + 2]);

        // Normalize light vector so the conversion is normalized.
        GLfloat lsz = sqrt((lv[0]*lv[0]) + (lv[1]*lv[1]) +
                           (lv[2]*lv[2]));
        if ((lsz > -0.0000001) && (lsz < 0.0000001)) { // Don't divide by zero
          lv[0] = 0.0f;
          lv[1] = 0.0f;
          lv[2] = 1.0f;
        } else {
          lv[0] = lv[0]/lsz;
          lv[1] = lv[1]/lsz;
          lv[2] = lv[2]/lsz;
        }

        // Convert the light vector into tangent space
        //
        // T - Tangent to surface
        // N - Normal to surface
        // B - Bi-Normal (T cross N)
        // color = lightvec  | T(x) B(x) N(x) |
        //                   | T(y) B(y) N(y) |
        //                   | T(z) B(z) N(z) |
        GLfloat m[16]; 
        matrixIdentity(m);
        m[0] = tangent[j*szTan + 0];
        m[1] = binorm[j*szBN + 0];
        m[2] = nrm[j*szNorm + 0];   
        m[4] = tangent[j*szTan + 1];
        m[5] = binorm[j*szBN + 1];  
        m[6] = nrm[j*szNorm + 1];  
        m[8] = tangent[j*szTan + 2];
        m[9] = binorm[j*szBN + 2];
        m[10]=nrm[j*szNorm + 2];
        GLfloat v[3];
        vecMat3x3Mult(lv,m,v);
        color[j*szClr + 0] = v[0];
        color[j*szClr + 1] = v[1];
        color[j*szClr + 2] = v[2];
        if (!forCubemap) {
          color[j*szClr + 0] = (color[j*szClr + 0] * 0.5) + 0.5;
          color[j*szClr + 1] = (color[j*szClr + 1] * 0.5) + 0.5;
          color[j*szClr + 2] = (color[j*szClr + 2] * 0.5) + 0.5;
        }
      } // end for j
    } // end if we have normals, tangents, binormals, and colors
  } // end for i
}

//////////////////////////
// Render the bumpmaped object. This member sets up the Vertex, Normal, Texture
// and Color arrays. The color array is special in the case of non-cubemapping
// it contains the the encoded (0.5 = 0.0) light vector in tangent space.
//   forCubemap -- Flag to say we are using a cubemap for normalization.
//   pass -- Which rendering pass we are using.
///////////////////////////
void 
BumpObject::renderObject(GLuint forCubemap, GLuint pass)
{
  glEnableClientState(GL_VERTEX_ARRAY);

  // Loop through all the objects in the file.
  for (int i = 0; i < m_numObjects; i++) {
    // Send the Vertex array
    vertexList *v = m_faces[i]->getVertexList();
    glVertexPointer(v->m_vComponents, GL_FLOAT, 0, v->m_verts);

    // Only set texture and color coordinates on the first pass.
    if (pass == 0) {
      // Send the Texture Coordinate array.
      float *tex;
      if (tex = v->getTexCoords(POF_TCI_BASE)){
        if (!forCubemap) {
          glClientActiveTexture(GL_TEXTURE0_ARB);
          glEnableClientState(GL_TEXTURE_COORD_ARRAY);
          glTexCoordPointer(v->getNumTexCoordComponents(POF_TCI_BASE), GL_FLOAT,
                            0, tex);
        }
        glClientActiveTexture(GL_TEXTURE1_ARB);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(v->getNumTexCoordComponents(POF_TCI_BASE), GL_FLOAT,
                          0, tex);
        glClientActiveTexture(GL_TEXTURE2_ARB);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(v->getNumTexCoordComponents(POF_TCI_BASE), GL_FLOAT,
                          0, tex);
      }

      // Send the color array (really the light vector in tangent space)
      float *binorm, *tangent, *color;
      if ((v->m_norms) && 
          (tangent = v->getTexCoords(POF_TCI_TAN0)) &&
          (binorm = v->getTexCoords(POF_TCI_BINORM0)) &&
          (color = v->getColors(POF_CI_DIFFUSE)) ) { 
        glClientActiveTexture(GL_TEXTURE0_ARB);
        if (forCubemap) {
          glEnableClientState(GL_TEXTURE_COORD_ARRAY);
          glTexCoordPointer(v->m_cComponents[0], GL_FLOAT, 0, color);
        } else {
          glEnableClientState(GL_COLOR_ARRAY);
          glColorPointer(v->m_cComponents[0], GL_FLOAT, 0, color);
        }
      }
    }

    // Draw the elements.
    glDrawElements(GL_TRIANGLES, m_faces[i]->m_count*3,
                   GL_UNSIGNED_INT, m_faces[i]->m_indices);

    // Disable Array states
    glClientActiveTexture(GL_TEXTURE0_ARB);
       glDisableClientState(GL_TEXTURE_COORD_ARRAY);
       glDisableClientState(GL_COLOR_ARRAY);
    glClientActiveTexture(GL_TEXTURE1_ARB);
       glDisableClientState(GL_TEXTURE_COORD_ARRAY);
       glDisableClientState(GL_COLOR_ARRAY);
    glClientActiveTexture(GL_TEXTURE2_ARB);
       glDisableClientState(GL_TEXTURE_COORD_ARRAY);
       glDisableClientState(GL_COLOR_ARRAY);
  } // end for i

  // Disable Array state
  glDisableClientState(GL_VERTEX_ARRAY);
} // render object

//////////////////////////
// Renders the light vectors and tangent space coordinate systems.
//  sf - scaling factor for how long to make the vectors/coord systems.
//  coordFlag - draw coord system if true.
//  lightFlag - draw light vector if true
//  forCubemap - Were the colors encoded for cubemaping?
//////////////////////////
void 
BumpObject::renderVectors(GLfloat sf, GLuint coordFlag, GLuint lightFlag, 
                          GLuint forCubemap)
{
  float *vtx;
  float *nrm;
  float *tan;
  float *binorm;
  float *color;
  vertexList *v;
  int ii,gg;

  // Loop over the objects in the file
  for (ii = 0; ii < m_numObjects; ii++){
    v = m_faces[ii]->getVertexList();
    vtx = v->getVertices();
    nrm = v->getNormals();
    tan = v->getTexCoords(POF_TCI_TAN0);
    binorm = v->getTexCoords(POF_TCI_BINORM0);
    color = v->getColors(POF_CI_DIFFUSE);
    int szVert = v->m_vComponents;

    // Draw Normals in red
    if (nrm && coordFlag){
      int szNorm = 3;
      glColor3f(1.0f, 0.0f, 0.0f);
      glBegin(GL_LINES);
      for (gg = 0; gg < v->getNumVertices(); gg++){
        glVertex3fv(&vtx[gg*szVert]);
        glVertex3f(vtx[gg*szVert + 0] + nrm[gg*szNorm + 0]*sf, 
                   vtx[gg*szVert + 1] + nrm[gg*szNorm + 1]*sf,
                   vtx[gg*szVert + 2] + nrm[gg*szNorm + 2]*sf);
      }
      glEnd();
    }

    // Draw Tangent in blue
    if (tan && coordFlag){
      int szTan = v->getNumTexCoordComponents(POF_TCI_TAN0);
      glColor3f(0.0f, 0.0f, 1.0f);
      glBegin(GL_LINES);
      for (gg = 0; gg < v->getNumVertices(); gg++){
        glVertex3fv(&vtx[gg*szVert]);
        glVertex3f(vtx[gg*szVert + 0] + tan[gg*szTan + 0]*sf, 
                   vtx[gg*szVert + 1] + tan[gg*szTan + 1]*sf,
                   vtx[gg*szVert + 2] + tan[gg*szTan + 2]*sf);
      }
      glEnd();
    }

    // Draw Binormal in green
    if (binorm && coordFlag){
      int szBN = v->getNumTexCoordComponents(POF_TCI_BINORM0);
      glColor3f(0.0f, 1.0f, 0.0f);
      glBegin(GL_LINES);
      for (gg = 0; gg < v->getNumVertices(); gg++){
        glVertex3fv(&vtx[gg*szVert]);
        glVertex3f(vtx[gg*szVert + 0] + binorm[gg*szBN + 0]*sf, 
                   vtx[gg*szVert + 1] + binorm[gg*szBN + 1]*sf,
                   vtx[gg*szVert + 2] + binorm[gg*szBN + 2]*sf);
      }
      glEnd();
    }

    // Draw light vector in yellow.
    if (color && lightFlag){
      glColor3f(1.0f, 1.0f, 0.0f);
      glBegin(GL_LINES);
      for (gg=0; gg < v->getNumVertices(); gg++){
        if (nrm && binorm && tan) {
          int szTan = v->getNumTexCoordComponents(POF_TCI_TAN0);
          int szBN = v->getNumTexCoordComponents(POF_TCI_BINORM0);
          int szClr = v->getNumColorComponents(POF_CI_DIFFUSE);
          int szNorm = 3;
          glVertex3fv(&vtx[gg*szVert]);

          // Unpack color (which represents light vector in object space)
          GLfloat cv[3];
          if (forCubemap) {
            cv[0] = color[gg*szClr + 0];
            cv[1] = color[gg*szClr + 1];
            cv[2] = color[gg*szClr + 2];
          } else {
            cv[0] = (color[gg*szClr + 0] - 0.5f) * 2.0f;
            cv[1] = (color[gg*szClr + 1] - 0.5f) * 2.0f;
            cv[2] = (color[gg*szClr + 2] - 0.5f) * 2.0f;
          }

          // Convert to screen space coords
          GLfloat m[16]; 
          GLfloat mi[16]; 
          matrixIdentity(m);
          matrixIdentity(mi);
          m[0]=tan[gg*szTan + 0];   
          m[1]=binorm[gg*szBN + 0];
          m[2]=nrm[gg*szNorm + 0];  
          m[4]=tan[gg*szTan + 1];   
          m[5]=binorm[gg*szBN + 1]; 
          m[6]=nrm[gg*szNorm + 1];  
          m[8]=tan[gg*szTan + 2];
          m[9]=binorm[gg*szBN + 2];
          m[10]=nrm[gg*szNorm + 2];
          matrixInvert(m,mi);
          GLfloat v[3];
          vecMat3x3Mult(cv,mi,v);

          // Draw
          glVertex3f(vtx[gg*szVert + 0] + v[0]*sf, 
                     vtx[gg*szVert + 1] + v[1]*sf,
                     vtx[gg*szVert + 2] + v[2]*sf);
        }
      }
      glEnd();
    } // end if color
  } // end for ii
} // render vectors
