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
 *                        Metropolis-Hastings sampler class                       *
 **********************************************************************************
 * File: MetropolisHastings.cc
 * Date: $Date: 2003/02/18 11:25:36 $
 * Author: $Author: leesonw $
 * Version: $Revision: 1.1 $
 * Description: Uses the Metropolis-Hasting Markov Chain sampling method.
 */
#include "MetropolisHastings.h"

/*
 *   Constructor
 */
MetropolisHastings::MetropolisHastings() : m_accept(0),
					   mp_generator(NULL),mp_function(NULL),
					   mp_sampler(NULL),mp_mutator(NULL),
					   mp_max(NULL),mp_min(NULL),mp_buf(NULL),
					   mp_tbuf(NULL),
					   m_dim(0),m_rejections(100),m_samples(0),m_count(0),
					   m_pd(0.0),m_X(0.0)
{
  // nothing todo here
}

/*
 * Destructor
 */
MetropolisHastings::~MetropolisHastings()
{
  if(mp_buf != NULL)
    delete[] mp_buf;
  
  if(mp_tbuf != NULL)
    delete[] mp_tbuf;
}

/*
 *Acceptance function
 */
real MetropolisHastings::accept(real X,real pdX,real Y,real pdY)
{
  real v;

  v = fabs(Y)/fabs(X);
  if(v <= 1.0)
    return v;

  return 1.0;
}

/*
 *      Reset the distribuitor
 */
void MetropolisHastings::reset(void)
{
  integer i;
  real s[m_dim];

  // find a non-zero sample to start.
  m_accept = 0;
  mp_sampler->reset();
  mp_generator->reset();
  mp_mutator->reset();
  do{    
    m_pd = mp_sampler->getSample(mp_buf);

    // map sample into domain of function
    //std::cerr << "[";
    for(i = 0;i < m_dim;i++)
      {
	mp_tbuf[i] = mp_min[i] + mp_buf[i]*(mp_max[i] - mp_min[i]);
	//std::cerr << "(" << m_pd << ")" << mp_tbuf[i] << ":" << mp_buf[i] << " ";
      }
    //std::cerr << "]" << std::endl;

    mp_function->evaluate(mp_tbuf,&m_X);
    //std::cerr << "X:" << m_X << std::endl;
  }while(m_X == 0.0);

  // reject the first few samples to get started.
  m_count = m_rejections;
  for(int i = 0;i < m_rejections;i++)
    this->getSample(s);

  m_count = m_samples;
}

/*
 *Set Function we wish to sample
 */
void MetropolisHastings::setFunction(IFunction *p_func)
{
  integer dim;

  dim = p_func->getDimDomain();
  if(m_dim != dim) // reallocate buffer if need be
    {
      // allocate space to store temp sample
      if(mp_buf != NULL)
	delete[] mp_buf;
      mp_buf = new real[dim];    
      if(mp_tbuf != NULL)
	delete[] mp_tbuf;
      
      mp_tbuf = new real[dim];
    }

  m_dim = dim;
  mp_function = p_func;
}

/*
 *Get function were sampling
 */
IFunction *MetropolisHastings::getFunction(void) const
{
  return mp_function;
}

/*
 *Set the initial sampler
 */
void MetropolisHastings::setInitialSampler(ISampler *p_sampler)
{
  mp_sampler = p_sampler;
}

/*
 *Get the initial sampler
 */
ISampler *MetropolisHastings::getInitialSampler(void) const
{
  return mp_sampler;
}

/*
 *Set the generator
 */
void MetropolisHastings::setGenerator(IGenerator *p_generator)
{
  mp_generator = p_generator;
}

/*
 *Get the generator
 */
IGenerator *MetropolisHastings::getGenerator(void) const
{
  return mp_generator;
}

/*
 *Set the mutation sampler
 */
void MetropolisHastings::setMutationSampler(ISampler *p_mutator)
{
  mp_mutator = p_mutator;
}

/*
 *Get the mutation sampler
 */
ISampler *MetropolisHastings::getMutationSampler(void) const
{
  return mp_mutator;
}


/*
 *Set the initial number of rejections
 */
void MetropolisHastings::setInitialRejections(integer n)
{
  m_rejections = n;
}

/*
 *Get the intial number of rejections
 */
integer MetropolisHastings::getInitialRejections(void)
{
  return m_rejections;
}

/*
 *Set the number of samples
 */
void MetropolisHastings::setSamples(integer n)
{
  m_samples = n;
}

/*
 *Get the number of samples
 */
integer MetropolisHastings::getSamples(void)
{
  return m_samples;
}

/*
 *    Set a range for each distribuitor dimension
 */
void MetropolisHastings::setRange(const real *p_min,const real *p_max)
{
  mp_min = (real *)p_min;
  mp_max = (real *)p_max;
}

/*
 *    Get a range for each distribuitor dimension
 */
void MetropolisHastings::getRange(real **pp_min,real **pp_max) const
{
  *pp_max = mp_max;
  *pp_min = mp_min;
}

/*
 *	produces samples from the sample space
 */
real MetropolisHastings::getSample(real *s)
{
  integer i;
  real Y,v,a,pdab,pdba;

  if(m_count > 0)
    {
      m_count--;
      if(mutate(mp_buf,mp_tbuf,&pdab,&pdba) == true) // was a sample inside the bounds produced?
	{
	  // map sample into range
	  for(i = 0;i < m_dim;i++)
	    s[i] = mp_min[i] + mp_tbuf[i]*(mp_max[i] - mp_min[i]);

	  // Evaulate sample and test for acceptance
	  mp_function->evaluate(s,&Y);
	  mp_generator->generate(&v);
	  a = this->accept(m_X,1.0,Y,1.0);
	  if(a > v) // do we accept this sample?
	    {
	      m_accept++;
	      m_X = Y;
	      memcpy(mp_buf,mp_tbuf,sizeof(real)*m_dim); // save new scaled sample for next itteration
	      memcpy(s,mp_tbuf,sizeof(real)*m_dim);      // give new sample to user
	    }
	  else // use old sample as it's rejected by acceptance func.
	    {
	      //std::cerr << "reject." << std::endl;
	      memcpy(s,mp_buf,sizeof(real)*m_dim);
	    }
	}
      else // use old sample as sample outside bounds so it's rejected.
	{
	  //std::cerr << "out of bounds." << std::endl;
	  memcpy(s,mp_buf,sizeof(real)*m_dim);
	}

      return 1.0;
    }

  return 0.0;
}

/*
 * Mutate the sample
 */
bool MetropolisHastings::mutate(const real *p_src,real *p_dest,real *p_pdab,real *p_pdba)
{
  integer i;
  real t,pd;
  bool status = true;
  
  for(i = 0;(i < m_dim) && (status == true);i++)
    {
      pd = 1.0;
      pd = pd*mp_generator->generate(&t);
      //std::cerr << "t:" << t << "->" << (t - 0.5) << "->";
      t = p_src[i] + 2.0*mp_dd[i]*(t - 0.5);
      //std::cerr << t << " p_src[i]:" << p_src[i] << std::endl;
      if((t >= 0.0) && (t <= 1.0))
	{
	  //memcpy(p_dest,p_src,sizeof(real)*m_dim);
	  p_dest[i] = t;
	}
      else
	{
	  status = false;
	}
    }
  *p_pdab =  1.0;
  *p_pdba =  1.0;

  return status;
}

/*
 *  Set the amount a samples is allowed to be mutated
 */
void MetropolisHastings::setChange(real *p_dd)
{
  mp_dd = p_dd;
}

/*
 * Get the amount a sample is allowed to be changed
 */
void MetropolisHastings::getChange(real **pp_dd)
{
  *pp_dd = mp_dd;
}

/*
 * setDim this is a dummy function as we can't really do this for this type of sampler
 */
void MetropolisHastings::setDim(integer n)
{
}
