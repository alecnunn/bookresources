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
 *                          Miser Interface Definition                      *
 ****************************************************************************
 * File: MiserIntegerator.h
 * Date: $Date: 2003/02/28 09:21:37 $
 * Author: $Author: leesonw $
 * Version: $Revision: 1.3 $
 * Description: This provides a set of methods to handle various
 *              operations performed on Miser type classes.
 */
#include "MiserIntegrator.h"

/*
 *  Constructor
 */
MiserIntegrator::MiserIntegrator() : m_dither(false),
				     m_samples(60),m_divs(0),m_maxdepth(0),
				     mp_sampler(NULL),
				     mp_integrator(NULL),
				     m_dithersize(0.0),m_varfrac(0.1),
				     mp_weights(NULL),m_totalweight(0.0),m_minfrac(0.1),m_minsubdivfrac(0.5),
				     m_minsamples(0),m_minsubdivsamples(0),
				     mp_generator(NULL)
{
  for(int i = 0;i< 1024;i++)
    m_divisions[i] = 0;
}

/*
 *  Destructor
 */
MiserIntegrator::~MiserIntegrator()
{
  // nothing todo yet!!
}

/*
 *	Set the sampler
 */
void MiserIntegrator::setSampler(ISampler *p_sampler)
{
  mp_sampler = p_sampler;
}

/*
 *	Set the generator
 */
void MiserIntegrator::setGenerator(IGenerator *p_generator)
{
  mp_generator = p_generator;
}

/*
 * setMinSubdivide
 */
void MiserIntegrator::setMinSubDivide(real frac)
{
  m_minsubdivfrac = frac;
}

/*
 *	Set the min samples
 */
void MiserIntegrator::setMinSamples(integer n)//real frac)
{
  m_minsamples = n;
}

/*
 *	Set the max depth
 */
void MiserIntegrator::setMaxDepth(integer depth)
{
  m_maxdepth = depth;
}

/*
 *	Set the min samples
 */
void MiserIntegrator::setSamples(integer samples)
{
  m_minsubdivsamples = integer(real(samples)*m_minsubdivfrac);
  m_samples = samples;
}

/*
 *	Set the Dither
 */
void MiserIntegrator::setDither(real dither)
{
  m_dither = true;
  m_dithersize = dither;
}

/*
 *	Set the samples fraction
 */
void MiserIntegrator::setVarianceSamples(real frac)
{
  m_varfrac = frac;
}

/*
 *	Set the dimension weights
 */
void MiserIntegrator::setWeights(integer dim,real *p_weights)
{
  integer i;
  real total;

  mp_weights = p_weights;
  total = mp_weights[0];
  for(i = 1;i < dim;i++)
    {
      total = total + mp_weights[i];
    }
  //fprintf(stderr,"weights = {");
  for(i = 0;i < dim;i++)
    {
      mp_weights[i] = mp_weights[i]/total;
      //fprintf(stderr," %f ",mp_weights[i]);
    }
  //fprintf(stderr,"};\n");
}

/*
 *	Set the integrator
 */
void MiserIntegrator::setIntegrator(IIntegrator *p_integrator)
{
  mp_integrator = p_integrator;
}

/*
 *	Integrate
 */
bool MiserIntegrator::integrate(const real *p_min,const real *p_max,IFunction *p_func,real *p_result,real *p_var)
{
  integer rdim = p_func->getDimRange(),
    pdim = p_func->getDimDomain();;
  
  // clear variance and result
  //for(i = 0;i < rdim;i++)
  //  p_result[i] = p_var[i] = 0.0;

  return this->miser(pdim,p_min,p_max,p_func,m_samples,0,rdim,p_result,p_var);
}

/*
 *	Integrate
 */
bool MiserIntegrator::miser(integer pdim,const real *p_min,const real *p_max,
			    IFunction *p_func,
			    integer samples,integer depth,
			    integer rdim,real *p_result,real *p_var)
{
  integer npre,jb;
  real pd;
  bool status = true;

  npre = integer(m_varfrac*real(samples));
  //std::cerr << "samples:" << samples << " min subdiv:" << m_minsubdivsamples
  //    << " npre:" << npre << " min samples:" << m_minsamples 
  //    <<" depth:" << depth << " max depth:" << m_maxdepth << std::endl;
  if((samples <= m_minsubdivsamples) || (npre <= m_minsamples) || (depth > m_maxdepth))
    {
      integer i;
      //real result[rdim],var[rdim];
      //std::cerr << "Integrating over [";
      //for(i = 0;i < pdim;i++)
      //std::cerr << p_min[i] << " ";
      //std::cerr << "]-[";
      //for(i = 0;i < pdim;i++)
      //std::cerr << p_max[i] << " ";
      //std::cerr << "]" << std::endl;

      mp_integrator->setSamples(samples);
      mp_integrator->integrate(p_min,p_max,p_func,p_result,p_var);

      // add result and variance to total so far
      //for(i = 0;i< rdim;i++)
      //	{
      //	  p_result[i] += result[i];
      //	  p_var[i] += var[i];
      //	}
      //std::cerr << "result @ depth " << depth << " is " << p_result[0] << " using " << samples << " samples." << std::endl;
    }
  else
    {
      integer i,j,nptl,nptr;
      real val[rdim],sumb,siglb,sigrb,sum,sigl,sigr,t;
      real ansr[rdim],ansl[rdim],varr[rdim],varl[rdim],ratio;
      real mid[pdim],mintmp[pdim],maxtmp[pdim],pt[pdim],
	fminl[pdim],fmaxl[pdim],fminr[pdim],fmaxr[pdim];
      
      jb = -1;
      sumb = HUGE_VAL;
      siglb = sigrb = 1.0;
      samples = samples - npre;
      
      // Partition domain
      // Allocate space for partition information
      for(i = 0;i < pdim;i++)
	{
	  // Setup left and right bounds for each dimension
	  if(m_dither == true)
	    {
	      mp_generator->generate(&t);
	      t = -m_dithersize/2.0 + t*m_dithersize; // map to dither extents
	      mid[i] = (0.5 + t)*(p_max[i] - p_min[i]) + p_min[i];
	    }
	  else
	    {
	      mid[i] = 0.5*(p_max[i] - p_min[i]) + p_min[i];
	    }
	  fminr[i] = fminl[i] = HUGE_VAL;
	  fmaxr[i] = fmaxl[i] = -HUGE_VAL;
	}
      
      // Sample function and determine left and right bounds
      mp_sampler->setSamples(npre);
      mp_sampler->reset();
      while((pd = mp_sampler->getSample(pt)) > 0.0)
	{
	  // map sample into sample space
	  for(j = 0;j < pdim;j++)
	    pt[i] = p_min[i] + pt[i]*(p_max[i] - p_min[i]);
	  p_func->evaluate(pt,val);
	  for(j = 0;j < pdim;j++)
	    {
	      // which partition does the result lie in
	      if(pt[j] < mid[j]) // In left partition
		{
		  // Find min and max
		  if(fminl[j] > val[j])
		    fminl[j] = val[j];
		  if(fmaxl[j] < val[j])
		    fmaxl[j] = val[j];
		}
	      else // In right partition
		{
		  // Find min and max
		  if(fminr[j] > val[j])
		    fminr[j] = val[j];
		  if(fmaxr[j] < val[j])
		    fmaxr[j] = val[j];
		}
	    }
	}
      
      // Estimate variance
      for(i = 0;i < pdim;i++)
	{
	  // Is it valid to use this dimension ie did we sample it sufficiently
	  if((fmaxl[i] > fminl[i]) && (fmaxr[i] > fminr[i]))
	    {
	      if(mp_weights[i] > 0.0) // Only consider if sufficiently weighted
		{
		  sigl = pow(fmaxl[i] - fminl[i],2.0/3.0);
		  sigr = pow(fmaxr[i] - fminr[i],2.0/3.0);
		  sum = pow(fabs(sigl + sigr),3.0);//*(1.0 - mp_weights[i]);
		  //na = (samples - npre)*(pow(sigl,1.0/3.0)/(pow(sigl,1.0/3.0) + pow(sigr,1.0/3.0)));
		  //sum = 0.25*(sigl/pow(na,2.0) + sigl/pow((samples - na),2.0));
		  if(sum <= sumb)
		    {
		      sumb = sum;
		      jb = i;
		      siglb = sigl;
		      sigrb = sigr;
		    }
		}
	    }
	}
      
      // If we did not determine a split randomly create one
      if(jb == -1)
	{
	  // Choose random value for jb if no suitable dimension found
	  mp_generator->generate(&t);
	  // use weights a a CDF to pick sample
	  do{
	    jb++;
	    t = t - mp_weights[jb];
	  }while(t > 0.0);
	}
      
      m_divisions[jb]++;
      // Calculate samples to be used in left and right sub regions
      ratio = siglb/(siglb + sigrb);
      nptl = integer(real(samples)*ratio);
      nptr = samples - nptl;
      if(nptr == 0)
	{
	  nptr++;
	  nptl--;
	}
      if(nptl == 0)
	{
	  nptl++;
	  nptr--;
	}

      // copy parameter space for use in next call to miser so as not to alter parameterspace
      memcpy(mintmp,p_min,sizeof(real)*pdim);
      memcpy(maxtmp,p_max,sizeof(real)*pdim);
      mintmp[jb] = p_min[jb];
      maxtmp[jb] = mid[jb];
      status = this->miser(pdim,mintmp,maxtmp,p_func,nptl,depth + 1,rdim,ansl,varl);
      
      mintmp[jb] = mid[jb];
      maxtmp[jb] = p_max[jb];
      status = this->miser(pdim,mintmp,maxtmp,p_func,nptr,depth + 1,rdim,ansr,varr);

      for(i = 0;i < rdim;i++)
	{
	  p_result[i] =  ansl[i] + ansr[i];
	  p_var[i] = varl[i] + varr[i];
	}
    }
  return status;
}

/*
 * print out some integration statisitcs
 */
void MiserIntegrator::printStatistics(void)
{
  fprintf(stderr,"Miser Statistics\n");
  for(int i = 0;i < 100;i++)
    {
      fprintf(stderr,"%4d ",int(m_divisions[i]));
    }
  fprintf(stderr,"\n");
}
