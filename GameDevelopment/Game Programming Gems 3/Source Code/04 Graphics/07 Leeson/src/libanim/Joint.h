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
 *                                 Joint                                    *
 ****************************************************************************
 * File: Joint.h                                                            *
 * Date: $Date$                                                             *
 * Author: $Author: leesonw $                                               *
 * Version: $Revision$                                                      *
 * Description: Joint for a transformation hierarchy.                       *
\****************************************************************************/

class joint;

#ifndef _JOINT_
#define _JOINT_

#include <stdio.h>
#include <stdlib.h>
#include<iostream>
#include<vector>
#include<algorithm>
#include<functional>
#include<IScene.h>
#include "libarray/libarray.h"

// keyframe for part of an animation stores orientation,
// displacement and the time it orrcurs at
struct keyframe
{
  real orient[4];
  real time;
  real disp[4];
};

// records vertex source, destination and weighting
struct j_vertex
{
  real *source;
  real *dest;
  real weight;
};

class Joint
{
 protected:
  real m_origin[4];                    // global displacement
  real m_displ[4];                     // displacement from origin
  vector<keyframe> m_keyframes;        // keyframes for animation
  vector<j_vertex> m_vertices;         // vertex and weighting list
  Joint *mp_parent;                    // parent joint
  vector<Joint> m_children;            // child joints
 public:
  Joint();                             // constructor
  virtual ~Joint();                    // destructor

  virtual void SetOrigin(real *origin);         // Sets the origin or initial position in gobal coords of the joint
  virtual void SetDisplacement(real *displ);    // Sets the displacement from parent
  virtual vector<keyframe> *GetKeyFrames(void); // get the keyframes vector
  virtual vector<j_vertex> *GetVertices(void);  // get the vertices vector
  virtual vector<Joint> *GetChildren(void);     // get the child joint vector
  virtual void update(integer n,real time,real *M);       // update vertices
};
#endif






