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
 *           Stratified VEGAS Integrator Interface Definition               *
 ****************************************************************************
 * File: StratVEGASIntegrator.cc
 * Date: $Date: 2003/02/28 09:22:17 $
 * Author: $Author: leesonw $
 * Version: $Revision: 1.4 $
 * Description: This provides a set of methods to handle various
 *              operations performed on mean sample monte carlo integrator
 *              type classes.
 */
#include "StratVEGASIntegrator.h"

/*
 *  Constructor
 */
StratVEGASIntegrator::StratVEGASIntegrator()
{
}

/*
 * Destructor
 */
StratVEGASIntegrator::~StratVEGASIntegrator()
{
}

/*
 *  integrates the function
 */
bool StratVEGASIntegrator::integrate(const real *p_min,const real *p_max,IFunction *p_func,real *p_result,real *p_var)
{
  integer bins,i,j,k,p,n,pdim,rdim,pass,passes,samples;
  real dd,prob,v,vv,wgt;

  // Setup some stuff
  pdim = p_func->getDimDomain();
  rdim = p_func->getDimRange();

  real rmin[pdim],rmax[pdim],sam[pdim];
  real a[rdim],aa,r[rdim],rr[rdim],f[rdim],ff[rdim],s[rdim],ss[rdim];

  // copy parameter ranges
  //std::cerr << "memcpy with pdim =" << pdim << std::endl;
  for(i = 0;i < pdim;i++)
    {
      rmin[i] = 0.0;
      rmax[i] = 1.0;
    }
  //memcpy(rmin,p_min,sizeof(real)*pdim);
  //memcpy(rmax,p_max,sizeof(real)*pdim);

  // determine the total number of combinations
  //std::cerr << "var2intger" << std::endl;
  this->var2Integer(&bins);

  // how many samples per bin
  //std::cerr << "samples and passes\n";
  samples = m_samples/(bins*m_update);
  passes = m_update;
  if(samples == 0)
    {
      std::cerr << "Error:not enough samples to perform integration." << std::endl;
      return false;
    }
  //std::cerr << "samples = " << samples << " passes = " << passes << std::endl;
  //printf("samples = %ld,passes = %ld\n",samples,passes);
  pass = 0;

  //std::cerr << "Reset partitions.";
  this->resetPartitions(); //p_min,p_max);
  //std::cerr << "....done\n";

  // clear results
  for(i = 0;i < rdim;i++)
    p_var[i] = ss[i] = p_result[i] = 0.0;

  // integration itteration loop (usually 5 passes)
  do{
    //std::cerr << "1\n";
    n = 0;
    for(i = 0;i < rdim;i++)
      f[i] = ff[i] = 0.0;
    // loop over all bins
    do{
      //std::cerr << "2\n";
      // for each group select partition and set parameter range
      for(i = 0;i < rdim;i++)
	r[i]  = rr[i] = 0.0;              // clear partition estimate

      // determine range for this partition
      prob = 1.0;
      for(i = 0;i < m_groups;i++)
	{
	  //std::cerr << "3\n";
	  p = mp_groups[i].choosen;
	  //fprintf(stderr,"%ld  ",p);
	  mp_groups[i].p_partitions[p].samples = samples;
	  
	  // assign range
	  rmin[mp_groups[i].dim] = mp_groups[i].p_partitions[p].min;
	  rmax[mp_groups[i].dim] = mp_groups[i].p_partitions[p].max;
	  dd = (rmax[mp_groups[i].dim] - rmin[mp_groups[i].dim]);
	  prob = prob/dd;
	}
      //std::cerr << "4\n";
      //mp_sampler->setRange(pdim,rmin,rmax);
      mp_sampler->setSamples(samples);
      mp_sampler->reset();

      // stratified sampling
      //fprintf(stderr,"part = { ");
      // loop of samples for this bin
      for(j = 0;j < samples;j++)
	{
	  //std::cerr << "5\n";
	  // generate sample
	  mp_sampler->getSample(sam);
	  // map samples into range using VEGAS partitions and integration domain
	  for(i = 0;i < pdim;i++)
	    sam[i] = p_min[i] + (rmin[i] + sam[i]*(rmax[i] - rmin[i]))*(p_max[i] - p_min[i]);

	  // evaluate sample
	  p_func->evaluate(sam,a);

	  // acculumate results
	  for(k = 0;k < rdim;k++)
	    {
	      a[k] = a[k]/prob;
	      aa = a[k]*a[k];
	      r[k] += a[k];
	      rr[k] += aa;
	    }
	}
      //std::cerr << "6\n";
      v = vv = 0.0;
      for(i = 0;i < rdim;i++)
	{
	  // convert vector result into scalr value for variance estimation creation
	  v += r[i];
	  vv += rr[i];
	  // or could use magnitude
	  // v += r[i]*r[i];
	  // vv += rr[i]*rr[i];
	  
	  f[i] += r[i]/real(samples);   // accumulate partitions
	  ff[i] += rr[i]/real(samples);
	}
      // v = sqrt(v);
      // vv = sqrt(vv);

      // store value in each partition that was sampled from
      for(i = 0;i < m_groups;i++)
	{
	  mp_groups[i].p_partitions[mp_groups[i].choosen].sum += fabs(v)/real(samples);
	  mp_groups[i].p_partitions[mp_groups[i].choosen].sum2 += vv/real(samples);
	}

      //std::cerr << "9\n";
      // step through all combinations of partitions
      this->increment(); 
      n++;
    }while(n < bins); // continue until we have used x samples
    //std::cerr << "10\n";
    // determine answer so far
    for(i = 0;i < rdim;i++)
      {
	s[i] = fabs(f[i]*f[i] - ff[i])/real(n);
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
	//if(s[i] == 0.0)
	//  s[i] = EPSILON;
	//ss[i] += 1.0/s[i];
	//p_result[i] += f[i]/s[i];
	
	//std::cerr << "result:" << p_result[i]
	//	  << " ss:" << ss[i] << " s:" << s[i]
	//	  << "f:" << f[i] << "ff:" << ff[i] << std::endl;
	//fprintf(stderr,"f = %lg, ff = %lg, var = %lg, est = %lg\n",f,ff,sqrt(s),ans/ss);
      }
    //std::cerr << "11\n";
    for(i = 0;i < m_groups;i++)
      {
	// calculate variance for each partition
	//fprintf(stderr,"var = { ");
	//std::cerr << "12\n";
	for(j = 0;j < mp_groups[i].partitions;j++)
	  {
	    v = mp_groups[i].p_partitions[j].sum;
	    vv = mp_groups[i].p_partitions[j].sum2;
	    mp_groups[i].p_partitions[j].sum2 = sqrt(fabs(v*v - vv));
	    //fprintf(stderr,"%lg ", sqrt(fabs(a*a - aa)));
	  }
	//fprintf(stderr,"};\n");

	//this->Dump(mp_groups + i);
	//std::cerr << "13\n";
	if(m_smooth)
	  {
	    this->smooth(mp_groups + i);
	    //this->Dump(mp_groups + i);
	  }

	//std::cerr << "14\n";
	if(m_dampen)
	  {
	    this->dampen(mp_groups + i,1.5);
	    //this->Dump(mp_groups + i);
	  }
	//std::cerr << "15\n";
	this->reBin(mp_groups + i);
      }
    pass++;
    //printf("(%ld)",pass);
    //std::cerr << "16\n";
  }while(pass < passes);
  //printf("\n");
  //fprintf(stderr,"est = %lg\n",ans/ss);
  //std::cerr << "17\n";
  real vol = p_func->volDomain(p_min,p_max);
  for(i = 0;i < rdim;i++)
    {
      p_result[i] = (vol*p_result[i])/ss[i];
      p_var[i] = vol*sqrt(1.0/ss[i]);
    }
  //std::cerr << "18\n";
  return true;
}
