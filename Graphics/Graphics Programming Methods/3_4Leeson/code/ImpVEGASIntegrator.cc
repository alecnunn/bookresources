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
 *                    VEGAS Integrator Interface Definition                 *
 ****************************************************************************
 * File: ImpVEGASIntegrator.cc
 * Date: $Date: 2003/02/28 09:20:46 $
 * Author: $Author: leesonw $
 * Version: $Revision: 1.4 $
 * Description: This provides a set of methods to handle various
 *              operations performed on mean sample monte carlo integrator
 *              type classes.
 */
#include "ImpVEGASIntegrator.h"

/*
 *  Constructor
 */
ImpVEGASIntegrator::ImpVEGASIntegrator()
{
}

/*
 * Destructor
 */
ImpVEGASIntegrator::~ImpVEGASIntegrator()
{
}

/*
 *  integrates the function
 */
bool ImpVEGASIntegrator::integrate(const real *p_min,const real *p_max,IFunction *p_func,real *p_result,real *p_var)
{
  integer i,j,p,n,pdim,rdim,ntotal,samples,passes;
  real dd,prob;

  // Setup some stuff
  pdim = p_func->getDimDomain();
  rdim = p_func->getDimRange();

  real rmin[pdim],rmax[pdim],sam[pdim];
  real r[rdim],rr[rdim],ss[rdim],a[rdim],s[rdim];

  // set sample range to [0 1]^k
  for(i = 0;i < pdim;i++)
    {
      rmin[i] = 0.0;
      rmax[i] = 1.0;
    }
  //memcpy(rmin,p_min,sizeof(real)*pdim);
  //memcpy(rmax,p_max,sizeof(real)*pdim);

  mp_sampler->setDim(pdim);
  mp_sampler->reset();
  this->resetPartitions();//p_min,p_max);

  // clear variables
  ntotal = 0;
  for(i = 0;i < rdim;i++)
    ss[i] = p_var[i] = p_result[i] = 0.0;
  
  //std::cerr << "m_samples:" << m_samples << " m_update:" << m_update << std::endl;
  samples = m_samples/m_update;
  // loop for each update (estimation loop)
  passes = 0;
  do{
    n = 0;

    // clear results generated for each pass
    for(i = 0;i < rdim;i++)
      r[i] = rr[i] = 0.0;
    do{ // loop for m_samples/m_update samples (integration loop)
      // for each group select partition and set parameter range
      prob = 1.0;
      // pick sample point using importance sampling
      for(i = 0;i < m_groups;i++)
	{
	  // pick a partition
	  p = rand() % mp_groups[i].partitions;
	  
	  //record partition used
	  mp_groups[i].choosen = p;
	  mp_groups[i].p_partitions[p].samples++;

	  // assign sample range
	  rmin[mp_groups[i].dim] = mp_groups[i].p_partitions[p].min;
	  rmax[mp_groups[i].dim] = mp_groups[i].p_partitions[p].max;
	  dd = (rmax[mp_groups[i].dim]  - rmin[mp_groups[i].dim]);
	  prob *= 1.0/(dd*real(mp_groups[i].partitions));
	}

      // generate sample
      mp_sampler->getSample(sam);
      // squeese sample in [0 1] to sample range of partition
      for(i = 0;i < pdim;i++)
	sam[i] = p_min[i] + (rmin[i] + sam[i]*(rmax[i] - rmin[i]))*(p_max[i] - p_min[i]);
      //std::cerr << "Sample @" << s[0] << std::endl;

      // evaluate sample and estimate a scalar quantity to represent the vector
      // how do we combine the weights to create an scalar impression of the vector function? (sum or magnitude)
      real aa,v = 0.0,vv = 0.0;

      p_func->evaluate(sam,a);
      for(i = 0;i < rdim;i++)
	{
	  a[i] = a[i]/prob;
	  aa = a[i]*a[i];
	  r[i] += a[i];
	  rr[i] += aa;
	  
	  // just add them together to scalar rep.
	  v += a[i];
	  vv += aa;

	  // or we could use the magnitude to get scalar rep.
	  // v += a[i]*a[i];
	  // vv += aa*aa;
	}
      // v = sqrt(v);
      // vv = sqrt(vv);

      // store value in each partition that was sampled from
      for(i = 0;i < m_groups;i++)
	{
	  mp_groups[i].p_partitions[mp_groups[i].choosen].sum += v;
	  mp_groups[i].p_partitions[mp_groups[i].choosen].sum2 += vv;
	}
      n++;
      //std::cerr << "n:" << n << " samples:" << samples << std::endl;
    }while(n < samples); // continue until we have used x samples

    // determine answer so far
    //std::cerr << "result so far:";
    real wgt;

    for(i = 0;i < rdim;i++)
      {
	r[i] = r[i]/real(n);      // f estimate for this itteration
	rr[i] = rr[i]/real(n);    // f*f estimate for this itteration
	s[i] = fabs(r[i]*r[i] - rr[i])/real(n); // squared variance estimate
	// kludge to stop division by zero (I don't like this at all) but it works!!!
	if(s[i] > 0.0)       // use estimate
	  wgt = 1.0/s[i];
	else if(ss[i] > 0.0) // use average
	  wgt = ss[i]/real(samples);
	else                 // what to do
	  wgt = 0.0;

	if(wgt > 0.0)
	  {
	    ss[i] += wgt;
	    p_result[i] += r[i]*wgt;
	    //chi[i] += p_result[i]*p_result[i]*wgt;
	  }
	else
	  {
	    p_result[i] += (r[i] - p_result[i])/(passes + 1);
	  }
	//std::cerr << p_result[i] << " ss:" << ss[i] << " s:" << s[i];
      }
    //std::cerr << std::endl;

    for(i = 0;i < m_groups;i++)
      {
	// estimate f and f^2 for each bin
	for(j = 0;j < mp_groups[i].partitions;j++)
	  {
	    if(mp_groups[i].p_partitions[j].samples > 0)
	      {
		mp_groups[i].p_partitions[j].sum /= mp_groups[i].p_partitions[j].samples;
		mp_groups[i].p_partitions[j].sum2 /= mp_groups[i].p_partitions[j].samples;
	      }
	  }

	//this->Dump(mp_groups + i);
	if(m_smooth)
	  {
	    this->smooth(mp_groups + i);
	    //this->Dump(mp_groups + i);

	  }

	if(m_dampen)
	  {
	    this->dampen(mp_groups + i,1.5);
	    //this->Dump(mp_groups + i);
	  }

	this->reBin(mp_groups + i);
      }
    ntotal += n;
    passes++;
  }while(ntotal < samples*m_update);

  real vol = p_func->volDomain(p_min,p_max);
  for(i = 0;i < rdim;i++)
    {
      p_result[i] = (p_result[i]*vol)/ss[i];
      p_var[i] = vol*sqrt(1.0/ss[i]);
    }
  return true;
}
