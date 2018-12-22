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
 * File: RandomSampler.h
 * Date: $Date: 2003/02/06 16:16:36 $
 * Author: $Author: leesonw $
 * Version: $Revision: 1.5 $
 * Description: This provides a set of functions to handle various
 *              operations performed on psuedo random generator type
 *              classes.
 */
class RandomSampler;

#ifndef _RANDOM_SAMPLER_
#define _RANDOM_SAMPLER_

#include "ISampler.h"
#include "IGenerator.h"
#include "libarray/libarray.h"

/** Random sampler used to produce statistically random n-dimensional vectors.
 *  Produced random n-dimensional vectors using a sequence generator usually random to produce
 *  random numbers for each dimension.
 */
class RandomSampler : public ISampler
{
 private:
  integer m_n,                                              ///< Maximum number of samples.
    m_counter,                                              ///< Counts the number of samples so far.
    m_dim;                                                  ///< Dimension of samples to be produced.
  IGenerator *mp_g;                                         ///< Sequence generator used to produce values for each dim.
 public:
  // (Con/De)structor
  RandomSampler();                                         ///< Initialises the variables to their default values.

  // Property Methods
  virtual integer getSamples(void) const;                  ///< Get the number of samples to be produced.
  virtual void setGenerator(IGenerator *g);                ///< Set the generator used to produce the samples.
  virtual IGenerator *getGenerator(void);                  ///< Get the generator used to produce the samples.
  virtual integer getDim(void);                            ///< Get the dimension of the samples to be produced.

  // ISampler
  virtual void setDim(integer dim);                        ///< Set the dimension of the samples to be produced.
  virtual real getSample(real *s);                         ///< Draw a sample from the sequence.
  virtual void reset(void);                                ///< Reset the sampler to the first in the sequence.
  virtual void setSamples(integer s);                      ///< Set the number of samples to be produced.
};
#endif
