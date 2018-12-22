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
 *         Mean Sample Monte Carlo Integrator Interface Definition          *
 ****************************************************************************
 * File: MeanSampleMonteCarlo.h
 * Date: $Date: 2003/02/06 16:08:30 $
 * Author: $Author: leesonw $
 * Version: $Revision: 1.3 $
 * Description: This provides a set of methods to handle various
 *              operations performed on mean sample monte carlo integrator
 *              type classes.
 */
class MeanSampleMonteCarlo;

#ifndef _MEAN_SAMPLE_MONTE_CARLO_INTEGRATOR_
#define _MEAN_SAMPLE_MONTE_CARLO_INTEGRATOR_
#include "ISampler.h"
#include "IIntegrator.h"
#include <iostream>

/** Means sample style Monte Carlo integration scheme.
 *  This is a Mean sample Monte Carlo integration scheme which can take an arbitrary sampler
 *  such as a uniform point set, random point set or a quasi-random one.
 */
class MeanSampleMonteCarlo : public IIntegrator
{
 private:
  ISampler *mp_sampler;                       ///< Sampler used to create samples during integration.
 public:
  // (Con/De)structor
  MeanSampleMonteCarlo();                     ///< Constructor initialises variables.
  
  // Properties
  void setSampler(ISampler *p_sampler);       ///< Set the sampler to be used.
  ISampler *getSampler(void) const;           ///< Get the sampler being used.
  
  //	Action
  virtual void setSamples(integer samples);   ///< Set the number of samples to use.
  virtual bool integrate(const real *min,const real *max,IFunction *p_func,real *p_result,real *p_var);
                                              ///< Integrate function over domain given by min and max.
};
#endif
