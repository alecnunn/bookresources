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
 *                             OpenGL Scene                                 *
 ****************************************************************************
 * File: GLScene.h                                                          *
 * Date: $Date$                                                             *
 * Author: $Author$                                                         *
 * Version: $Revision$                                                      *
 * Description: An OpenGL based renderng class that allows state sorting    *
 *              and other optimisations.                                    *
\****************************************************************************/
class GLScene;

#ifndef _GLSCENE_
#define _GLSCENE_

#include<stdio.h>
#include<stdlib.h>
#include<GL/glut.h>
#include<vector>
#include<algorithm>
#include"libarray/libarray.h"
#include"IScene.h"

void check_opengl_error(char *cmd);

class GLScene : public IScene
{
  struct textureimage
  {
    bool resident;                     // Is texture resident?
    integer refs;                      // number of reference to texture
    integer frame;                     // last frame texture was used on
    GLuint id;                         // Texture id
    integer width,height;              // width and height of texturex
    byte *p_texture;                   // Texture array
  };

  struct texture
  {
    GLint wrap_s,wrap_t;               // Wrap texture in a direction
    GLenum min_filter,mag_filter;      // min and max texture filters
    GLint mode;                        // texture mode (modulate,decal,...etc)
    bool coord_gen;                    // Enable/Disable automatic texture coord generation
    GLenum gen_s,gen_t;                // types of coord generation
    vector<textureimage>::iterator image;
                                       // reference to stored texture image
  };
  struct fragment
  {
    GLenum type;                       // type of geometry represented by fragment
    integer n;                         // number of elements in fragment
    integer *p_faces;                  // list of faces
    real *p_vertices;                  // vertex array
    integer vstep;                     // stepping for vertices
    real *p_normals;                   // normal array
    integer nstep;                     // normal step
    real *p_texuv;                     // texture coordinates
    integer tstep;                     // texture coord step
    // vector<real *> texcoords;       // texture coords for multi-texturing
  };
  struct material
  {
    integer frame;                      // frame material was last used in
    GLfloat r_s[4],r_d[4],r_a[4],n;     // specular & diffuse coeff specular power
    vector<texture> textures;           // textures associated with material
    vector<fragment> fragments;         // geometery fragments associated with material
  };
  struct object
  {
    vector<material> materials;        // fragments making up object
    real M[4*4];                       // transformation
  };
  struct light
  {
    GLfloat pos[4];                    // position
    GLfloat c[4];                      // color
    bool resident;                     // Is light resident? Has it been/Is it setup?
  };
  GLenum m_ff;                         // Stores front face type for FlipFrontFace
 private:
  integer m_frame,m_frame_threshold;   // current frame were on and the frame threshold
  GLfloat m_ca,m_la,m_qa;              // attenuation properties
  // vector<object *> m_objects;       // objects in scene
  vector<material> m_materials;        // materials used in this rendering    
  vector<textureimage> m_images;       // texture set needed for all materials
  vector<light> m_lights;              // set of lights used in this rendering
  real m_up[3];                        // up direction
  real m_lookdir[3];                   // look direction
  real m_lookat[3];                    // look at position
  real m_pos[3];                       // viewing position
  GLint m_vx,m_vy,m_vdx,m_vdy;         // viewport position and dimensions
  GLdouble m_aspect,m_near,m_far,m_fovy;// perspective view
 public:
  // (Con/De)structor
  GLScene();
  virtual ~GLScene();

  // Render controls
  virtual bool BeginFrame(void);     // start frame (flush buffers etc)
  virtual bool EndFrame(void);       // end frame (swap buffers etc...)

  // Geometry
  virtual bool AddTriangles(integer n,integer *p_faces,
			    real *p_vertices,integer vstep,
			    real *p_normals,integer nstep,
			    real *p_texuv,integer tstep);
  virtual bool AddQuads(integer n,integer *p_faces,
			real *p_vertices,integer vstep,
			real *p_normals,integer nstep,
			real *p_texuv,integer tstep);
  virtual bool AddLines(integer n,integer *p_indices,real *p_vertices,integer vstep);
  virtual bool AddPoints(integer n,integer *p_indices,real *p_vertices,integer vstep);

  // object specification
  virtual bool BeginObject(void);
  virtual bool EndObject(void);
  virtual bool Rotate(real angle,real *axis);
  virtual bool Translate(real x,real y,real z);
  virtual bool Scale(real x,real y,real z);

  // Material specification
  virtual bool BeginMaterial(void);  // resets material data
  virtual bool EndMaterial(void);    // ends current material and reverts to previous one
  virtual bool Phong(real r_a,real r_d,real r_s,real n);
  virtual bool AddTexture(integer width,integer height,byte *p_texture);
  virtual bool AddEnvironmentMap(integer width,integer height,byte *p_tex);
  
  // Light settings 
  virtual bool AddLight(real *p_pos,real *p_color);

  // Viewing settings
  virtual bool Position(real *pos);
  virtual bool LookAt(real *pos);
  virtual bool Up(real *up);
  //bool Right(real *right);
  virtual bool LookDirection(real *lookdir);
  virtual bool FieldOfView(real fov);

  // SetUp Screen Controls
  virtual bool Viewport(integer minx,integer miny,integer maxx,integer maxy);

  // Some Rendering properties
  virtual bool WireFrame(void);
  virtual bool SolidFill(void);
  virtual bool SmoothShade(void);
  virtual bool FlatShade(void);
  virtual bool FlipBackFace(void);
};
#endif







