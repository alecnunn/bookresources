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
 *                      Function Interface Definition                       *
 ****************************************************************************
 * File: IFunction.h
 * Date: $Date: 2003/02/07 11:19:51 $
 * Author: $Author: leesonw $
 * Version: $Revision: 1.4 $
 * Description: This provides a set of methods to handle various
 *              operations performed on function type classes.
 */
struct IFunction;

#ifndef _IFUNCTION_
#define _IFUNCTION_

#include "libarray/libarray.h"

struct IFunction
{
  virtual integer getDimDomain(void) const = 0;
  virtual integer getDimRange(void) const = 0;
  virtual bool evaluate(const real *p_parameters,real *p_result) = 0;
  virtual void getDomain(real *p_min,real *p_max) const = 0;
  virtual void getRange(real *p_min,real *p_max) const = 0;
  virtual real volDomain(const real *p_min,const real *p_max) = 0;
};
#endif
