/* Copyright (C) William Leeson, 2003. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) William Leeson, 2003"
 */
/****************************************************************************\
 *                     Integrator Interface Definition                      *
 ****************************************************************************
 * File: IIntegrator.h
 * Date: $Date: 2003/01/27 12:54:18 $
 * Author: $Author: leesonw $
 * Version: $Revision: 1.1 $
 * Description: This provides a set of methods to handle various
 *              operations performed on integrator type classes.
 */
struct  IIntegrator;

#ifndef _IINTEGRATOR_
#define _IINTEGRATOR_
#include "IFunction.h"

struct IIntegrator
{
  virtual void setSamples(integer samples) = 0;                   // # of smaples to use
  virtual bool integrate(const real *p_min,const real *p_max,     // range of integration
			 IFunction *p_func,                       // function to integrate
			 real *p_result,                          // estimate (this is numerical integration afterall)
			 real *p_variance) = 0;                   // variance in estimate
};
#endif
