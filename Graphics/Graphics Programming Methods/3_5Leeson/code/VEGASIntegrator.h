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
 *                   VEGAS Integrator Interface Definition                  *
 ****************************************************************************
 * File: VEGASIntegrator.h
 * Date: $Date: 2003/02/28 09:29:13 $
 * Author: $Author: leesonw $
 * Version: $Revision: 1.4 $
 * Description: This provides a set of methods to handle various operations 
 *              performed on mean sample monte carlo integrator type classes.
 */
class VEGASIntegrator;

#ifndef _VEGAS_INTEGRATOR_
#define _VEGAS_INTEGRATOR_
#include "ISampler.h"
#include "IIntegrator.h"
#include <iostream>

/** Base class which provides the rebinning damping and smoothing necessary for a VEGAS style integrator
 *  This class impliments the bin and rebinning functionality found in a VEGAS style integrator. Also
 *  smoothing and damping methods are provided. Along with all the propertys that can be attributed to
 *  such an integration method.
 */
class VEGASIntegrator : public IIntegrator
{
 protected:
  /// partition holds the extents and data for a segment used in binning
  struct partition
  {
    integer dim,             /// Dimension that this partitions
      samples;               /// Number of samples that have fallen in the bin
    real min,max,            /// Extents of the partition
      sum,sum2,              /// Sum of f(x)'s and sum of f^2(x)'s
      subpartitions;         /// Sub-partitions in partition
  };

  /// Used to record the relivant dimensions and their partitions
  struct group
  {
    integer dim,                  ///< Dimension of group
      partitions,                 ///< Number of partitions
      choosen;                    ///< Partition choosen so we don't have to calculate it from sample.
    real *p_submin,*p_submax,     ///< Sub-partitions used to record extents of new partitions while rebinning
      est,var;                    ///< Estimate of integratl of f(x) and its variance
    partition *p_partitions;      ///< Partition used in group
  };
  
  integer m_groups,               ///< Total number of groups
    m_samples,                    ///< Number of samples aloted
    m_update,                     ///< After how many samples in the result updated
    m_combinations;               ///< If using stratified sampling this holds the minumum number of samples required.
                                  ///< It is calculated by counting the number of combinations the partitions can be in.
  real m_ypos,m_xpos,
    m_alpha;
  ISampler *mp_sampler;           ///< Sampler to be used
  group *mp_groups;               ///< Groups
  bool m_dampen,m_smooth;         ///< Is smoothing and damping enabled.
 protected:
  void smooth(group *p_group);          ///< Perform smoothing on the group
  void dampen(group *p_group,real a);   ///< Dampen bins in group
  void reBin(group *p_group);           ///< Rebin bins in group
  void dump(group *p_group);            ///< ASCII dump of data in group 
  
  bool var2Integer(integer *p_number);                        ///< Convert n digit partition combination to an integer.
  bool increment(void);                                       ///< Increment partition combination to next one.
  void resetPartitions(void); //const real *p_min,const real *p_max);  ///< Reset partitons and bins to uniform divisions.
 public:
  //	(Con/De)structor
  VEGASIntegrator();                                          ///< Initialise variables.
  virtual ~VEGASIntegrator();                                 ///< Delete allocated data.
  
  //	Properties
  virtual bool setSampler(ISampler *p_sampler);               ///< Set the sampler used to generate samples
  virtual void setGroups(integer n,integer *p_groups);        ///< Set the dimensions and their groupings to be binned
  virtual void smooth(void);                                  ///< Toggle smoothing
  virtual void dampen(real alpha);                            ///< Set damping parameter (0 implies no damping usually 1.5) 
  virtual void setUpdate(integer samples);                    ///< Set after how many samples estimate is to be recalculated
  
  // IIntegrator
  virtual bool integrate(const real *min,const real *max,IFunction *p_func,real *p_result,real *p_var) = 0;
                                                      ///< Integrate the function using min and max as domain of integration
  virtual void setSamples(integer samples);           ///< Set total number of samples to be used in integration.
};
#endif
