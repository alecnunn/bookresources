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
 *                       Epanechnikov Kernel Function                       *
 ****************************************************************************
 * File: EpanechnikovKernel.cc
 * Date: $Date: 2003/02/07 17:50:43 $
 * Author: $Author: leesonw $
 * Version: $Revision: 1.2 $
 * Description: This is an implimentation of the Epanechnikov kernel function 
 *              used for density estimation.
 */

#include "EpanechnikovKernel.h"

/*
 *  Default Constructor
 */
EpanechnikovKernel::EpanechnikovKernel()
{ 
  // nothing todo here except initialise the above
}

/*
 *  Gets the domain
 */
void EpanechnikovKernel::getDomain(real *p_min,real *p_max)const
{
  *p_min = -1;
  *p_max = 1;
}

/*
 *  Returns the domain dimension
 */
integer  EpanechnikovKernel::getDimDomain(void) const
{
  return 1; 
}

/*
 * Get the range of the function
 */
void EpanechnikovKernel::getRange(real *p_min,real *p_max) const
{
  p_min[0] = -1;
  p_max[0] =  1;
}

/*
 *  Returns the domain dimension
 */
integer  EpanechnikovKernel::getDimRange(void) const
{
  return 1; 
}

/*
 *	Evaluates a function at a point
 */
bool EpanechnikovKernel::evaluate(const real *p_parameters,real *p_result)
{ 
  if(fabs(p_parameters[0]) <= 1.0)
    *p_result = 3.0*(1 -p_parameters[0]*p_parameters[0])/4.0; //*sqrt(5.0));
  else
    *p_result = 0;

  return true;
}

/*
 * Calculates the volume/area(what ever you choose to call it) of the parameter domain
 */
real EpanechnikovKernel::volDomain(const real *p_min,const real *p_max)
{
  // for sin(x) this is just the length of the parameter space
  // it's only a 1D function after all.
  return p_max[0] - p_min[0];
}
