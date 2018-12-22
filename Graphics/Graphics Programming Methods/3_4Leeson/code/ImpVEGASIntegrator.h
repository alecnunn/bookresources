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
 *              Importance VEGAS Integrator Interface Definition            *
 ****************************************************************************
 * File: ImpVEGASIntegrator.h
 * Date: $Date: 2003/02/06 16:09:34 $
 * Author: $Author: leesonw $
 * Version: $Revision: 1.2 $
 * Description: This provides a set of methods to handle various
 *              operations performed on mean sample monte carlo integrator
 *              type classes.
 */
class ImpVEGASIntegrator;

#ifndef _IMPORTANCE_VEGAS_INTEGRATOR_
#define _IMPORTANCE_VEGAS_INTEGRATOR_
#include "ISampler.h"
#include "IIntegrator.h"
#include "VEGASIntegrator.h"
  
/** Importance based VEGAS style integrator.
 *  This integrator uses an importance sampling scheme. It adaptively build up this from an initial uniform sampling scheme.
 */
class ImpVEGASIntegrator : public VEGASIntegrator
{
 public:
  //	(Con/De)structor
  ImpVEGASIntegrator();                                     ///< Constructor to initialise variables.
  virtual ~ImpVEGASIntegrator();                            ///< Destructor frees allocated data.
  
  // IIntegrator
  virtual bool integrate(const real *min,const real *max,IFunction *p_func,real *p_result,real *p_var);
                                                            ///< Integrate function using min and max as domain of integ.
};
#endif



