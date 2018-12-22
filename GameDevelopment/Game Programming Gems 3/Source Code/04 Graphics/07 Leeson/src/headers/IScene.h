/* Copyright (C) William Leeson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) William Leeson, 2001"
 */
/*
 *   IScene - An renderng interface to allow state sorting and other optimisations
 */
struct IScene;

#ifndef _ISCENE_
#define _ISCENE_

#include "libarray/libarray.h"

struct IScene
{
  // Render controls
  virtual bool BeginFrame(void)= 0 ;    // start frame (flush buffers etc)
  virtual bool EndFrame(void)= 0;       // end frame (swap buffers etc...)

  // Geometry
  virtual bool AddTriangles(integer n,integer *p_faces,
		    real *p_vertices,integer vstep,
		    real *p_normals,integer nstep,
		    real *p_texuv,integer tstep) = 0;
  virtual bool AddQuads(integer n,integer *p_faces,
		real *p_vertices,integer vstep,
		real *p_normals,integer nstep,
		real *p_texuv,integer tstep) = 0;
                                        // Add a series of triangles
  // Material specification
  virtual bool BeginMaterial(void) = 0; // resets material data
  virtual bool EndMaterial(void) = 0;   // ends current material and reverts to previous one
  virtual bool Phong(real r_a,real r_d,real r_s,real n) = 0;
  virtual bool AddTexture(integer width,integer height,byte *p_texture) = 0;

  // Light settings 
  virtual bool AddLight(real *p_pos,real *p_color) = 0;

  // View settings
  virtual bool Position(real *pos) = 0;
  virtual bool LookAt(real *pos) = 0;
  virtual bool Up(real *up) = 0;
  virtual bool LookDirection(real *lookdir) = 0;
};
#endif
