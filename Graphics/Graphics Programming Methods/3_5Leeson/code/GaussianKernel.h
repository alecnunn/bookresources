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
 *                           Gaussian Kernel function                       *
 ****************************************************************************
 * File: GaussianKernel.h
 * Date: $Date: 2003/02/07 17:50:43 $
 * Author: $Author: leesonw $
 * Version: $Revision: 1.2 $
 * Description: This is an implimentation of the gaussian kernel function
 *              used for density estimation.
 */

class GaussianKernel;

#ifndef _GAUSSIAN_KERNEL_FUNCTION_
#define _GAUSSIAN_KERNEL_FUNCTION_

#include "IFunction.h"

/** Gaussian kernel used in density estimation problems
 *  This is the uniform kernel use in density estimation problems it is given as
 *  \f$\frac{1}{\sqrt{2\pi}}exp\left(-\frac{1}{2}u^2\right)\f$
 */
class GaussianKernel : public IFunction
{
 public:
  //(Con/De)structor
  GaussianKernel();
  
  // IFunction
  virtual integer getDimDomain(void) const;
  virtual integer getDimRange(void) const;
  virtual bool evaluate(const real *p_parameters,real *p_result);
  virtual void getDomain(real *p_min,real *p_max) const;
  virtual void getRange(real *p_min,real *p_max) const;
  virtual real volDomain(const real *p_min,const real *p_max);
};
#endif
