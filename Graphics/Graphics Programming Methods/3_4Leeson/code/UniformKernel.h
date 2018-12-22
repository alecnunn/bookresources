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
 *                            Uniform Kernel function                       *
 ****************************************************************************
 * File: UniformKernel.h
 * Date: $Date: 2003/02/07 17:50:43 $
 * Author: $Author: leesonw $
 * Version: $Revision: 1.2 $
 * Description: This is an implimentation of the uniform kernel function
 *              used for density estimation.
 */

class UniformKernel;

#ifndef _UNIFORM_KERNEL_FUNCTION_
#define _UNIFORM_KERNEL_FUNCTION_

#include "IFunction.h"

/** Uniform kernel used in density estimation problems
 *  This is the uniform kernel use in density estimation problems it is given as
 *  \f$\frac{1}{2}I(|u| \le 1)\f$
 */
class UniformKernel : public IFunction
{
 public:
  //(Con/De)structor
  UniformKernel();
  
  // IFunction
  virtual integer getDimDomain(void) const;
  virtual integer getDimRange(void) const;
  virtual bool evaluate(const real *p_parameters,real *p_result);
  virtual void getDomain(real *p_min,real *p_max) const;
  virtual void getRange(real *p_min,real *p_max) const;
  virtual real volDomain(const real *p_min,const real *p_max);
};
#endif
