/* Copyright (C) William Leeson, 2003. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) William Leeson, 2003"
 */
/****************************************************************************
 *                          Hypershpere Function Class                      *
 ****************************************************************************
 * File: Hypersphere.cc
 * Date: $Date: 2003/02/07 14:34:34 $
 * Author: $Author: leesonw $
 * Version: $Revision: 1.2 $
 * Description: This class uses the sin function. It is mainly used as test
 *              function for the integrators.
 */

#include "HyperSphere.h"

/*
 *  Default Constructor
 */
HyperSphere::HyperSphere() : m_dim(3) // start with a 3D sphere
{ 
  // nothing todo here except initialise the above
}

/*
 * Set dimension of the HyperSphere
 */
void HyperSphere::setDim(integer n)
{
  m_dim = n;
}

/*
 * Get dimension of the HyperSphere
 */
integer HyperSphere::getDim(void)
{
  return m_dim;
}

/*
 *  Gets the domain
 */
void HyperSphere::getDomain(real *p_min,real *p_max)const
{
  integer i;

  for(i = 0;i < m_dim;i++)
    {
      p_min[i] = -1;
      p_max[i] = 1;
    }
}

/*
 *  Returns the domain dimension
 */
integer  HyperSphere::getDimDomain(void) const
{
  return m_dim; 
}

/*
 * Get the range of the function
 */
void HyperSphere::getRange(real *p_min,real *p_max) const
{
  p_min[0] = -1;
  p_max[0] =  1;
}

/*
 *  Returns the domain dimension
 */
integer  HyperSphere::getDimRange(void) const
{
  return 1; 
}

/*
 *	Evaluates a function at a point
 */
bool HyperSphere::evaluate(const real *p_parameters,real *p_result)
{ 
  integer i;
  real val;

  val = p_parameters[0]*p_parameters[0];//*p_parameters[0];
  for(i = 1;i < m_dim;i++)
    val += p_parameters[i]*p_parameters[i];//*p_parameters[i];

  if(val <= 1.0)
    p_result[0] = 1.0;
  else
    p_result[0] = 0.0;

  return true;
}

/*
 * Calculates the volume/area(what ever you choose to call it) of the parameter domain
 */
real HyperSphere::volDomain(const real *p_min,const real *p_max)
{
  real vol;
  integer i;

  // for sin(x) this is just the length of the parameter space
  // it's only a 1D function after all
  vol = p_max[0] - p_min[0];
  for(i = 1;i < m_dim;i++)
    vol *= (p_max[i] - p_min[i]);

  return vol;
}
