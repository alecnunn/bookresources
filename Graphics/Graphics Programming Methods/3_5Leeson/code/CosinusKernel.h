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
 *                            Cosinus Kernel function                       *
 ****************************************************************************
 * File: CosinusKernel.h
 * Date: $Date: 2003/02/07 17:50:43 $
 * Author: $Author: leesonw $
 * Version: $Revision: 1.2 $
 * Description: This is an implimentation of the cosinus kernel function
 *              used for density estimation.
 */

class CosinusKernel;

#ifndef _COSINUS_KERNEL_FUNCTION_
#define _COSINUS_KERNEL_FUNCTION_

#include "IFunction.h"

/** Cosinus kernel used in density estimation problems
 *  This is the uniform kernel use in density estimation problems it is given as
 *  \f$\frac{\pi}{4}cos\left(\frac{\pi}{2}u\right)I(|u| \le 1)\f$
 */
class CosinusKernel : public IFunction
{
 public:
  //(Con/De)structor
  CosinusKernel();
  
  // IFunction
  virtual integer getDimDomain(void) const;
  virtual integer getDimRange(void) const;
  virtual bool evaluate(const real *p_parameters,real *p_result);
  virtual void getDomain(real *p_min,real *p_max) const;
  virtual void getRange(real *p_min,real *p_max) const;
  virtual real volDomain(const real *p_min,const real *p_max);
};
#endif
