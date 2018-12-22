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
 *              Stratified VEGAS Integrator Interface Definition            *
 ****************************************************************************
 * File: StratVEGASIntegrator.h
 * Date: $Date: 2003/02/06 16:02:39 $
 * Author: $Author: leesonw $
 * Version: $Revision: 1.2 $
 * Description: This provides a set of methods to handle various
 *              operations performed on mean sample monte carlo integrator
 *              type classes.
 */
class StratVEGASIntegrator;

#ifndef _STRATIFIED_VEGAS_INTEGRATOR_
#define _STRATIFIED_VEGAS_INTEGRATOR_
#include "ISampler.h"
#include "IIntegrator.h"
#include "VEGASIntegrator.h"

/** VEGAS style integrator based on stratified integration
 *  This integrator is a multipass stratified integration method. Which uses an adaptive variance estimate
 *  to control the areas where samples are directed.
 */
class StratVEGASIntegrator : public VEGASIntegrator
{
 public:
  //	(Con/De)structor
  StratVEGASIntegrator();                          ///< Constructor initialises variables
  virtual ~StratVEGASIntegrator();                 ///< Frees allocated memory
  
  // IIntegrator
  virtual bool integrate(const real *min,const real *max,IFunction *p_func,real *p_result,real *p_var);
                                                   ///< Integrate function using min and max as domain of integration.
};
#endif
