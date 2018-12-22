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
 *                            HyperSphere function                          *
 ****************************************************************************
 * File: HyperSphere.h
 * Date: $Date: 2003/02/07 14:35:04 $
 * Author: $Author: leesonw $
 * Version: $Revision: 1.2 $
 * Description: This provides a set of functions to handle various
 *              operations performed on linear interpolator classes.
 */

class HypereSphere;

#ifndef _HYPERSPHERE_FUNCTION_
#define _HYPERSPHERE_FUNCTION_

#include "IFunction.h"

class HyperSphere : public IFunction
{
 private:
  integer m_dim;                  ///< Dimension of the hypersphere
 public:
  //(Con/De)structor
  HyperSphere();
  
  // Properties
  void setDim(integer n);            ///< Set the dimension of the hypersphere
  integer getDim(void);              ///< Get the dimension of the hypersphere

  // IFunction
  virtual integer getDimDomain(void) const;
  virtual integer getDimRange(void) const;
  virtual bool evaluate(const real *p_parameters,real *p_result);
  virtual void getDomain(real *p_min,real *p_max) const;
  virtual void getRange(real *p_min,real *p_max) const;
  virtual real volDomain(const real *p_min,const real *p_max);
};
#endif
