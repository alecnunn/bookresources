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
 *                       Gaussian Kernel Function                           *
 ****************************************************************************
 * File: GaussianKernel.cc
 * Date: $Date: 2003/02/07 16:22:31 $
 * Author: $Author: leesonw $
 * Version: $Revision: 1.1 $
 * Description: This is an implimentation of the gaussian kernel function 
 *              used for density estimation.
 */

#include "GaussianKernel.h"

/*
 *  Default Constructor
 */
GaussianKernel::GaussianKernel()
{ 
  // nothing todo here except initialise the above
}

/*
 *  Gets the domain
 */
void GaussianKernel::getDomain(real *p_min,real *p_max)const
{
  *p_min = -1;
  *p_max = 1;
}

/*
 *  Returns the domain dimension
 */
integer  GaussianKernel::getDimDomain(void) const
{
  return 1; 
}

/*
 * Get the range of the function
 */
void GaussianKernel::getRange(real *p_min,real *p_max) const
{
  p_min[0] = -1;
  p_max[0] =  1;
}

/*
 *  Returns the domain dimension
 */
integer  GaussianKernel::getDimRange(void) const
{
  return 1; 
}

/*
 *	Evaluates a function at a point
 */
bool GaussianKernel::evaluate(const real *p_parameters,real *p_result)
{ 
  *p_result = exp(-0.5*p_parameters[0]*p_parameters[0])/sqrt(2.0*M_PI);

  return true;
}

/*
 * Calculates the volume/area(what ever you choose to call it) of the parameter domain
 */
real GaussianKernel::volDomain(const real *p_min,const real *p_max)
{
  // for sin(x) this is just the length of the parameter space
  // it's only a 1D function after all.
  return p_max[0] - p_min[0];
}
