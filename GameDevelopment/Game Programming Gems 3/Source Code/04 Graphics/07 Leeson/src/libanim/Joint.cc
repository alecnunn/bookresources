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
 * File: Joint.cc                                                           *
 * Date: $Date$                                                             *
 * Author: $Author$                                                         *
 * Version: $Revision$                                                      *
 * Description: Joint for a transformation hierarchy.                       *
\****************************************************************************/

#include "Joint.h"

/*
 * constructor
 */
Joint::Joint()
{
  // nothing todo yet
}

/*
 * Destructor
 */
Joint::~Joint()
{
  // nothing todo either
}


/*
 * Get the keyframes vector
 */
vector<keyframe> *Joint::GetKeyFrames(void)
{
  return &m_keyframes;
}

/*
 * Get the children vector
 */
vector<Joint> *Joint::GetChildren(void)
{
  return &m_children;
}

/*
 * update the vertices using the joint hierarchy
 */
void Joint::update(integer n,real time,real *M)
{
  real Mn[16],Mt[16],T[16],R[16],Q[4],D[4],t,V1[4],V2[4];
  vector<Joint>::iterator joint;
  vector<j_vertex>::iterator vertex;
  vector<keyframe>::iterator kframe1,kframe2,kframe;

  // find keyframes between which given time lies using a binary search
  kframe1 = m_keyframes.begin();
  kframe2 = m_keyframes.end();
  while((kframe2 - kframe1) > 1)
    {
      kframe = kframe1 + (kframe2 - kframe1)/2;
      if(kframe->time > time)
	kframe2 = kframe;
      else
	kframe1 = kframe;
    }

  // determine ratio between keyframes where time lies
  t = (time - kframe1->time)/(kframe2->time - kframe1->time);

  // evaluate new orientation and displacement
  slerp(kframe1->orient,kframe2->orient,t,Q);
  //ALerp4D(kframe1->disp,kframe2->disp,t,D);

  MTranslate4x4(m_displ[0],m_displ[1],m_displ[2],T);
  MMMultiply4x4(M,T,Mt);
  Quat2M4x4(Q,R);
  MMMultiply4x4(Mt,R,Mn);
  
  //MTranslate4x4(m_origin[0],m_origin[1],m_origin[2],T);
  //MMMultiply4x4(Mn,T,Mt);
  //cerr << "#############\n";
  // Apply transformation and weight to attached vertices
  for(vertex = m_vertices.begin();vertex != m_vertices.end();vertex++)
    {
      ASubtract3D(vertex->source,m_origin,V1);
      //cerr << "src:"; APrint4D(vertex->source); cerr << endl;
      //cerr << "\torigin:"; APrint4D(m_origin); cerr << endl;
      V1[3] = vertex->source[3];
      //cerr << "\tV1    :"; APrint4D(V1); cerr << endl;
      MVMultiply4x4(Mn,V1,V2);
      //cerr << "\tV2 rot:"; APrint4D(V2); cerr << endl;
      // AAdd3D(V2,m_origin,V2);
      ASMultiply4D(vertex->weight,V2,V2);
      //cerr << "\tV2 w  :"; APrint4D(V2); cerr << endl;
      AAdd4D(V2,vertex->dest,vertex->dest);
      // cerr << "<" << n << "> "; APrint4D(vertex->dest); cerr << endl;
    }

  // propagate results to children
  for(joint = m_children.begin();joint != m_children.end();joint++)
    joint->update(n + 1,time,Mn);
}

/*
 * SetOrigin - sets the original global origin of the joint
 */
void Joint::SetOrigin(real *origin)
{
  ACopy4D(origin,m_origin);
}

/*
 * SetDisplacement - sets the original global origin of the joint
 */
void Joint::SetDisplacement(real *displ)
{
  ACopy4D(displ,m_displ);
}

/*
 * GetVertices - get list of vertex (source,dest,weight)
 */
/*
 * Get the destinations vector
 */
vector<j_vertex> *Joint::GetVertices(void)
{
  return &m_vertices;
}



