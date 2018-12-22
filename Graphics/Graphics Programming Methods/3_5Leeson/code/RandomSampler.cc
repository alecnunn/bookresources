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
 *                    Random Sampler Interface Definition                   *
 ****************************************************************************
 * File: RandomSampler.cc
 * Date: $Date: 2003/01/31 16:06:52 $
 * Author: $Author: leesonw $
 * Version: $Revision: 1.4 $
 * Description: This provides a set of functions to handle various
 *              operations performed on psuedo random generator type
 *              classes.
 */
#include "RandomSampler.h"

/*
 *   Constructor
 */
RandomSampler::RandomSampler() : m_n(0),m_counter(0),m_dim(0),
				 mp_g(NULL)
{
  // nothing todo except the above
}

/*
 *  Number of samples
 */
void RandomSampler::setSamples(integer n)
{
  m_n = n;  
}

/*
 *  Number of samples
 */
integer RandomSampler::getSamples(void) const
{
  return m_n;
}

/*
 *		Set the generator 
 */
void RandomSampler::setGenerator(IGenerator *p_g)
{
  mp_g = p_g;
}

/*
 *		Get the generator 
 */
IGenerator *RandomSampler::getGenerator(void)
{
  return mp_g;
}

/*
 *  get a multivariate random sample
 *  this just uses the same psuedo random number generator for each dimension
 */
real RandomSampler::getSample(real *p_s)
{ 
  integer i;
  real pd,v;
 
  // any samples left?
  if(m_counter < m_n)
    {
      m_counter++;
      pd = mp_g->generate(&v);
      p_s[0] = v;
      // itterate over each dimension
      for(i = 1;i < m_dim;i++)
	{
	  // generate number and accumulate probability
	  pd = pd*(mp_g->generate(&v));
	  p_s[i] = v;
	}
      return pd;
    }
  return 0.0;
}

/*
 *  set the dimensionality of the samples produced
 */
void RandomSampler::setDim(integer dim)
{
  m_dim = dim;
}

/*
 *  Range of sample in each dimension
 */
integer RandomSampler::getDim(void)
{
  return m_dim;
}

/*
 *		Resets the sampler
 */
void RandomSampler::reset(void)
{ 
  m_counter = 0; 
  mp_g->reset();
}
