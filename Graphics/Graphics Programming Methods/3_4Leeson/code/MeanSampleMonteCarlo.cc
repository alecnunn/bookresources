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
 * File: MeanSampleMonteCarlo.cc
 * Date: $Date: 2003/02/28 09:21:15 $
 * Author: $Author: leesonw $
 * Version: $Revision: 1.4 $
 * Description: This provides a set of methods to handle various
 *              operations performed on mean sample monte carlo integrator
 *              type classes.
 */
#include "MeanSampleMonteCarlo.h"

/*
 *  Constructor
 */
MeanSampleMonteCarlo::MeanSampleMonteCarlo() : mp_sampler(NULL)
{
}

/*
 *  set the sampler
 */
void MeanSampleMonteCarlo::setSampler(ISampler *p_sampler)
{
  mp_sampler = p_sampler;
}

/*
 *  set the samples
 */
void MeanSampleMonteCarlo::setSamples(integer samples)
{
  mp_sampler->setSamples(samples);
}

/*
 *  get the sampler
 */
ISampler *MeanSampleMonteCarlo::getSampler(void)const
{
  return mp_sampler;
}

/*
 *  integrates the function
 */
bool MeanSampleMonteCarlo::integrate(const real *p_min,const real *p_max,IFunction *p_func,real *p_result,real *p_var)
{
  integer i,n,pdim,rdim;
  real pd,vol;

  // get dimension of parameter domain and setup related variables
  pdim = p_func->getDimDomain();
  real sample[pdim];

  // setup up sampler
  mp_sampler->setDim(pdim);
  mp_sampler->reset();
 
  // get dimension of result and setup related variables
  rdim = p_func->getDimRange();
  n = 0;
  // zero result and variance
  for(i = 0;i < rdim;i++)
    p_result[i] = p_var[i] = 0;

  // perform integration
  real tmp[rdim];

  while((pd = mp_sampler->getSample(sample)) > 0.0)
    {
      // map sample in [0 1]^n to parameter domain
      for(i = 0;i < pdim;i++)
	sample[i] = p_min[i] + sample[i]*(p_max[i] - p_min[i]);

      // evaluate!!!
      p_func->evaluate(sample,tmp);

      // accumulate results
      for(i = 0;i < rdim;i++)
	{
	  tmp[i] = tmp[i]/pd;
	  p_result[i] += tmp[i];
	  p_var[i] += (tmp[i]*tmp[i]);
	}
      n++;
    }

  // calculate the final estimates
  vol = p_func->volDomain(p_min,p_max);
  for(i = 0;i < rdim;i++)
    {
      // estimate answer
      p_result[i] = (p_result[i])/real(n);
      // estimate variance
      p_var[i] = vol*sqrt(fabs((p_var[i])/real(n) - p_result[i]*p_result[i])/real(n));

      p_result[i] = vol*p_result[i];
    }

  return true;
}
