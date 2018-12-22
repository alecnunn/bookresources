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
 *                        Miser Interface Definition                        *
 ****************************************************************************
 * File: MiserIntegrator.h
 * Date: $Date: 2003/02/06 16:04:29 $
 * Author: $Author: leesonw $
 * Version: $Revision: 1.2 $
 * Description: This provides a set of methods to handle various
 *              operations performed on integrator type classes.
 */
class MiserIntegrator;

#ifndef _MISER_INTEGRATOR_
#define _MISER_INTEGRATOR_
#include "IIntegrator.h"
#include "ISampler.h"
#include "IGenerator.h"
#include <iostream>

/** Miser integration algorithm from Numerical Recipies in C with some modifications
 * This is a recursive stratified integration algorithm based on the Miser algorithm
 * presented in Numerical Recipies in C.
 */
class MiserIntegrator : public IIntegrator
{
 private:
  bool m_dither;                                             ///< Is dithering of partition allowed.
  integer m_samples,                                         ///< Number of samples.
    m_divs,                                                  ///< Number of divisions.
    m_divisions[1024],                                       ///<
    m_maxdepth;                                              ///< Maximum depth allowed in recursion.
  ISampler *mp_sampler;                                      ///< Sampler used to generate samples
  IIntegrator *mp_integrator;                                ///< Integrator to use to estimate the integral of a partition.
  real m_dithersize,                                         ///< Amount that a division is allowed to shift left or right.
    m_varfrac,                                               ///< Fraction of samples used to estimate variance usually [0 1]
    *mp_weights,                                             ///< Weights use to indicate which dimensions are important.
    m_totalweight,                                           ///< Sum of the weights
    m_minfrac,                                               ///< 
    m_minsubdivfrac;                                         ///< Number of samples allowed before subdivision  has to stop.
  integer m_minsamples,                                      ///< Minimum samples before integration must be performed.
    m_minsubdivsamples;                                      ///<
  IGenerator *mp_generator;	                             ///< Generator used to...
 private:
  bool miser(integer pdim,const real *p_min,const real *p_max,IFunction *p_func,integer samples,integer depth,integer rdim,real *p_result,real *p_var);                                   ///< This is the actual Miser recursive scheme
 public:
  MiserIntegrator();                                         ///< Initialises variable to sensible values.
  virtual ~MiserIntegrator();                                ///< Frees any allocated data.

  // Property methods
  virtual void setSampler(ISampler *p_sampler);              ///< Set the sampler used to estimate the variance.
  virtual void setGenerator(IGenerator *p_generator);        ///< Set the generator used to dither partition.
  virtual void setIntegrator(IIntegrator *p_integrator);     ///< Set the integrator used to estimate the int of a part.
  virtual void setMinSamples(integer n); //real frac);       ///< Set the minimum number of allowed
  virtual void setMinSubDivide(real frac);                   ///< Sets the min number of samples that can be subdivided.
  virtual void setVarianceSamples(real frac);                ///< Sets the fraction of samples used to estimate the variance.
  virtual void setDither(real dither);                       ///< Set the amount of dither.
  virtual void setMaxDepth(integer depth);                   ///< Sets the max amount of recursion.
  virtual void setWeights(integer dim,real *p_weights);      ///< Sets the weights used to prioritise dimensions 
  
  virtual void printStatistics(void);                        ///< Print out statistics of integration.

  // IIntegrate
  virtual void setSamples(integer n);                        ///< Set the samples used in the integration
  virtual bool integrate(const real *p_min,const real *p_max,IFunction *p_func,real *p_result,real *p_var);
                                                             ///< Integrate function using min and max as domain.
};
#endif
