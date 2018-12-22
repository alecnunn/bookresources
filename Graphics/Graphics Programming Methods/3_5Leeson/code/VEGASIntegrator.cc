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
 * File: VEGASIntegrator.cc
 * Date: $Date: 2003/02/28 09:23:14 $
 * Author: $Author: leesonw $
 * Version: $Revision: 1.4 $
 * Description: This provides a set of methods to handle various
 *              operations performed on mean sample monte carlo integrator
 *              type classes.
 */
#include "VEGASIntegrator.h"

/*
 *  Constructor
 */
VEGASIntegrator::VEGASIntegrator() : m_groups(0),m_samples(0),m_update(0),m_combinations(0),
				     m_ypos(0.1),m_xpos(0.05),m_alpha(1.5),
				     mp_sampler(NULL),
                                     mp_groups(NULL), 
				     m_dampen(true),
				     m_smooth(true)
{
  // Nothing todo except initialise the above
}

/*
 * Destructor
 */
VEGASIntegrator::~VEGASIntegrator()
{
  integer i;

  // free PDF strucutres
  for(i = 0;i < m_groups;i++)
    {
      delete[] mp_groups[i].p_partitions;
      delete[] mp_groups[i].p_submin;
      delete[] mp_groups[i].p_submax;
    }
  delete[] mp_groups;
}

/*
 *  set the sampler
 */
bool VEGASIntegrator::setSampler(ISampler *p_sampler)
{
  mp_sampler = p_sampler;

  return true;
}

/*
 *  set the samples
 */
void VEGASIntegrator::setSamples(integer samples)
{
  m_samples = samples;
}

/*
 * Enable/Disable smoothing
 */
void VEGASIntegrator::smooth(void)
{
  m_smooth = !m_smooth;
}

/*
 * Enable/Disable damping
 */
void VEGASIntegrator::dampen(real alpha)
{
  m_alpha = alpha;
  if(alpha <= 0.0)
    m_dampen = false;
  else
    m_dampen = true;
}

/*
 * SetGroups
 * takes an array which describes the dimensions to partition
 * {dim1,partitions},...,{dimn,partitions} x n
 */
void VEGASIntegrator::setGroups(integer n,integer *p_groups)
{
  integer i;

  // if already allocated free memory
  if(mp_groups != NULL)
    {
      for(i = 0;i < m_groups;i++)
	{
	  delete[] mp_groups[i].p_partitions;
	  delete[] mp_groups[i].p_submin;
	  delete[] mp_groups[i].p_submax;
	}
      delete[] mp_groups;
    }
  m_groups = n;
  mp_groups = new group[m_groups];
  for(i = 0;i < m_groups;i++)
    {
      mp_groups[i].dim = p_groups[i*2 + 0];

      // allocate partitions
      mp_groups[i].partitions = p_groups[i*2 + 1];
      mp_groups[i].p_partitions = new partition[p_groups[i*2 + 1]];

      // allocate subpartitions (used to record extents for partition while rebinning)
      mp_groups[i].p_submin = new real[p_groups[i*2 + 1]];
      mp_groups[i].p_submax = new real[p_groups[i*2 + 1]];
    }

  // determine the number of combos for stratified sampling
  m_combinations = mp_groups[0].partitions;
  for(i = 1;i < m_groups;i++)
      m_combinations *= mp_groups[i].partitions;
}

/*
 *  Set the number of samples to update after
 */
void VEGASIntegrator::setUpdate(integer update)
{
  m_update = update;
}

/*
 * Smooths the partition using
 * d_0 = (d_0 + d_1)/2
 * d_i = (d_{i - 1} + d_i + d_{i + 1})/3
 * d_{n - 1} =  (d_{n - 2} + d_{n - 1})/2
 */
void VEGASIntegrator::smooth(group *p_group)
{
  integer n;
  real total,d,t;

  n =  p_group->partitions;
  d = p_group->p_partitions[0].sum2;
  p_group->p_partitions[0].sum2 = (d + p_group->p_partitions[1].sum2)/2.0;
  total = p_group->p_partitions[0].sum2;
  for(integer i = 1;i < n - 1;i++)
    {
      t = (d + p_group->p_partitions[i].sum2 + p_group->p_partitions[i + 1].sum2)/3.0;
      d = p_group->p_partitions[i].sum2;
      p_group->p_partitions[i].sum2 = t;
      total += t;
    }
  p_group->p_partitions[n - 1].sum2 = (d + p_group->p_partitions[n - 1].sum2)/2.0;
  total += p_group->p_partitions[n - 1].sum2;

  // re-normalise
  if(total > 0.0)
    {
      for(integer i = 0;i < n;i++)
	p_group->p_partitions[i].sum2 = p_group->p_partitions[i].sum2/total;
    }
}

/*
 * Dampen using d_i = ((d_i - 1)/log(d_i))^a
 */
void VEGASIntegrator::dampen(group *p_group,real a)
{
  real d,total;

  // get total
  total = p_group->p_partitions[0].sum2;
  for(integer i = 1;i < p_group->partitions;i++)
    total += p_group->p_partitions[i].sum2;

  // normalise
  if(total > 0.0)
    {
      for(integer i = 0;i < p_group->partitions;i++)
	p_group->p_partitions[i].sum2 = p_group->p_partitions[i].sum2/total;
    }
  
  total = 0;
  for(integer i = 0;i < p_group->partitions;i++)
    {
      d = p_group->p_partitions[i].sum2;
      p_group->p_partitions[i].sum2 = pow((d - 1)/log(d),a);
      total += p_group->p_partitions[i].sum2;
    }

  // re-normalise
  if(total > 0.0)
    {
      for(integer i = 0;i < p_group->partitions;i++)
	p_group->p_partitions[i].sum2 = p_group->p_partitions[i].sum2/total;
    }
}

/*
 * Rebin
 */
void VEGASIntegrator::reBin(group *p_group)
{
  integer i,j,n;
  real dd,total,min,max,Sd,dj=0.0,d=0.0;

  n = p_group->partitions;

  // work out total
  total = p_group->p_partitions[0].sum2;
  for(i = 1;i < n;i++)
    total += p_group->p_partitions[i].sum2;
  
  if(total > 0.0)
    {
      // divide up domain between partitions
      dd = total/real(n);
      min = p_group->p_partitions[0].min;
      max = p_group->p_partitions[n - 1].max;
      i = j = 0;
      Sd = 0.0;
      do{
	if(i != 0)
	  p_group->p_submin[i] = p_group->p_submax[i - 1];
	else
	  p_group->p_submin[i] = min;
	while((Sd < dd) && (j < n)) // keep summing up till we add up to dd or we run out of partitions
	  {
	    Sd += p_group->p_partitions[j].sum2;
	    min = p_group->p_partitions[j].min;
	    d = p_group->p_partitions[j].sum2;
	    dj = p_group->p_partitions[j].max - p_group->p_partitions[j].min;
	    j++;
	  };
	Sd = Sd - dd;
	p_group->p_submax[i] = min + (1.0 - Sd/d)*dj;
	i++;
      }while(i < n - 1);
      p_group->p_submin[n - 1] = p_group->p_submax[i - 1];
      p_group->p_submax[n - 1] = max;
      
      // assign partitions and zero values
      //fprintf(stderr,"parts = { ");
      for(i = 0;i < n;i++)
	{
	  //fprintf(stderr,"[%lg,%lg] ",p_group->p_submin[i],p_group->p_submax[i]);
	  p_group->p_partitions[i].min = p_group->p_submin[i];
	  p_group->p_partitions[i].max = p_group->p_submax[i];
	  p_group->p_partitions[i].sum = 0.0;
	  p_group->p_partitions[i].sum2 = 0.0;
	  p_group->p_partitions[i].samples = 0;
	}
      //fprintf(stderr,"\n");
      p_group->choosen = 0;
    }
  else
    {
      // just clear
      for(i = 0;i < n;i++)
	{
	  p_group->p_partitions[i].sum = 0.0;
	  p_group->p_partitions[i].sum2 = 0.0;
	  p_group->p_partitions[i].samples = 0;
	}
    }
}

/*
 *  Clear adatpive structure
 */
void VEGASIntegrator::resetPartitions(void) //const real *p_min,const real *p_max)
{
  // Setup initial partitions
  for(integer j = 0;j < m_groups;j++)
    {
      mp_groups[j].choosen = 0;
      for(integer i = 0;i < mp_groups[j].partitions;i++)
	{
	  mp_groups[j].p_partitions[i].samples = 0;
	  mp_groups[j].p_partitions[i].min = real(i)/real(mp_groups[j].partitions); //p_min[mp_groups[j].dim] + ((p_max[mp_groups[j].dim] - p_min[mp_groups[j].dim])*real(i))/real(mp_groups[j].partitions);
	  mp_groups[j].p_partitions[i].max = real(i + 1)/real(mp_groups[j].partitions); //p_min[mp_groups[j].dim] + ((p_max[mp_groups[j].dim] - p_min[mp_groups[j].dim])*real(i + 1))/real(mp_groups[j].partitions);
	  mp_groups[j].p_partitions[i].sum = 0.0;
	  mp_groups[j].p_partitions[i].sum2 = 0.0;
	}
    } 
}

/*
 * PostScript dump partitions
 */
void VEGASIntegrator::dump(group *p_group)
{
  real maxv,minx,maxx,dx;

  maxv = 0.0;
  minx = HUGE_VAL;
  maxx = -HUGE_VAL;

  // determine extents
  for(integer i = 0;i < p_group->partitions;i++)
    {
      //fprintf(stderr,"(%ld,%ld)maxv = %lg, sum = %lg\n",j,i,maxv,p_group->p_partitions[i].sum);
      if(maxv < p_group->p_partitions[i].sum2)
	maxv = p_group->p_partitions[i].sum2;
      if(maxx < p_group->p_partitions[i].max)
	maxx = p_group->p_partitions[i].max;
      if(minx > p_group->p_partitions[i].min)
	minx = p_group->p_partitions[i].min;
    }
  dx = maxx - minx;

  // PS dump data
  for(integer i = 0;i < p_group->partitions;i++)
    {
      /*
      printf("%%%% partition %ld of %ld\n",i,p_group->partitions);
      // draw box for partition
      printf("newpath\n");
      printf("%lg inch %lg inch moveto\n"
	     ,(p_group->p_partitions[i].min - minx)/dx + m_xpos
	     ,double(0.0)  + m_ypos);
      printf("%lg inch %lg inch lineto\n"
	     ,(p_group->p_partitions[i].min - minx)/dx + m_xpos
	     ,p_group->p_partitions[i].sum2/maxv + m_ypos);
      printf("%lg inch %lg inch lineto\n"
	     ,(p_group->p_partitions[i].max - minx)/dx + m_xpos
	     ,p_group->p_partitions[i].sum2/maxv + m_ypos);
      printf("%lg inch %lg inch lineto\n"
	     ,(p_group->p_partitions[i].max - minx)/dx + m_xpos
	     ,double(0.0) + m_ypos);
      printf("closepath\nstroke\n");
      */
      printf("0.0 %lg %lg %lg\t",p_group->p_partitions[i].sum2,p_group->p_partitions[i].min,p_group->p_partitions[i].max);
    }
  printf("\n");
}

/*
 *Converts a segment to an integer  
 */
bool VEGASIntegrator::var2Integer(integer *p_number)
{
  integer i;

  (*p_number) = mp_groups[0].partitions;
  for(i = 1;i < m_groups;i++)
    {
      (*p_number) *= mp_groups[i].partitions;
    }
  return true;
}

/*
 * Increments segment counter!!!!
 */
bool VEGASIntegrator::increment(void)
{
  int i;

  for(i = 0;i < m_groups;i++)
    {
      if(mp_groups[i].choosen < (mp_groups[i].partitions - 1))
	{
	  mp_groups[i].choosen++;
	  return false;
	}
      else
	{
	  mp_groups[i].choosen = 0;
	}
    }
  return true;
}
