/* Copyright (C) William Leeson, 2003. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) William Leeson, 2003"
 */
/**********************************************************************************
 *                      Metropolis-Hastings sampler class                         *
 **********************************************************************************
 * File: MetropolisHastings.h
 * Date: $Date: 2003/02/18 11:25:19 $
 * Author: $Author: leesonw $
 * Version: $Revision: 1.1 $
 * Description: Uses the Metropolis-Hasting Markov Chain sampling method.
 */
class MetropolisHastings;

#ifndef _METROPOLIS_HASTINGS_SAMPLER_
#define _METROPOLIS_HASTINGS_SAMPLER_

#include "ISampler.h"
#include "IGenerator.h"
#include "IFunction.h"
#include <iostream>

/** Metropolis-Hastings sampler
 * This class used the Metropolis-Hastings sampling method to create a series of correlated samples
 * that are drawn from a function \f$f(x)\f$ using the detailed balance condition. This works by
 * mutating an intial sample which is then evaluated in \f$f(x)\f$ and if it satisfies the following
 * \f$ \frac{f(x')A(x' \rightarrow x)}{f(x)A(x \rightarror x')} \ge \zeta_i\f$ where \f$A(x)\f$ is 
 * the acceptance probability which is usually the same and therfore cancels out leaving just the
 * ratio of \f$f(x)\f$ and \f$f(x')\f$. Which is tested against \f$zeta_i\f$ a random value and if it
 * is lower the values is rejected otherwise it is used (this is a form of rejection sampling).
 */
class MetropolisHastings :public ISampler
{
 private:
  integer m_accept;
  IGenerator *mp_generator;                       ///< Generator used to test against accpetance func.
  IFunction *mp_function;                         ///< Function from which to sample.
  ISampler *mp_sampler;                           ///< Sampler used to produce initial sample.
  ISampler *mp_mutator;                           ///< Sampler used to mutate exisint sample.
  real *mp_max,                                   ///< Domain from which to sample.
    *mp_min,                                      ///< Domain from which to sample.
    *mp_buf,                                      ///< buffer for storing last sample.
    *mp_tbuf,                                     ///< temp buffer for storing new mutation.
    *mp_dd;                                       ///< The amount a sample is allowed to change.
  integer m_dim,                                  ///< Dimension of samples generated
    m_rejections,                                 ///< Number of rejections before samples are returned
    m_samples,                                    ///< Total number of samples to generate.
    m_count;                                      ///< Number of samples produced so far.
  real m_pd,                                      ///< Probability os last sample.
    m_X;                                          ///< Value returned by eval of last sample.
 protected:
  real accept(real X,real pdX,real Y,real pdY);   ///< Acceptance probability funtcion.
  bool mutate(const real *p_src,real *p_dest,real *p_pdab,real *p_pdba);
                                                  ///< Used to mutate an existing sample.
 public:
  // (Con/De)structor
  MetropolisHastings();                           ///< Initialises variables and allocs temp storgage.
  virtual ~MetropolisHastings();                  ///< Frees temp. storage.

  //Property methods
  virtual void setFunction(IFunction *p_func);    ///< Set the function we wish to sample
  virtual IFunction *getFunction(void) const;     ///< Get the fucntion were sampling.

  virtual void setInitialSampler(ISampler *p_sampler); ///< Set the sampler to generate initial sample.
  virtual ISampler *getInitialSampler(void) const;     ///< Get the sampler to generate initial sample.

  virtual void setGenerator(IGenerator *p_generator);  ///< Set the generator to test against the acceptance function.
  virtual IGenerator *getGenerator(void) const;        ///< Get the generator to test against the acceptance function.

  virtual void setMutationSampler(ISampler *p_sampler); ///< Set the sampler use to generate samples for the mutation function.
  virtual ISampler *getMutationSampler(void) const;     ///< Get the sampler use to generate samples for the mutation function.

  virtual void setInitialRejections(integer n); ///< Set number of samples to reject.
  virtual integer getInitialRejections(void);   ///< Get number of samples to reject.

  virtual integer getSamples(void);             ///< Get the number of samples to generate.

  virtual void setRange(const real *p_min,const real *p_max);
                                                ///< Set the domain from which to sample.
  virtual void getRange(real **pp_min,real **pp_max) const;
                                                ///< Get the domain from which to sample.

  virtual void setChange(real *p_dd);           ///< Set the amount a sample is allowed to be mutated.
  virtual void getChange(real **pp_dd);         ///< Get the amount a sample is allowed to be mutated.

  // ISampler
  virtual void setSamples(integer n);           ///< Set the number of samples to be taken.
  virtual real getSample(real *s);              ///< Produce a sample.
  virtual void reset(void);                     ///< Reset the sampler to the begining.
  virtual void setDim(integer n);               ///< Set the dimension of the samples created.
};
#endif
