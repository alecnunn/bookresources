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
 *                       Sampler Interface Definition                       *
 ****************************************************************************
 * File: ISampler.h
 * Date: $Date: 2003/02/06 16:16:22 $
 * Author: $Author: leesonw $
 * Version: $Revision: 1.4 $
 * Description: This provides a set of functions to handle various
 *              operations performed on samplers.
 */
struct ISampler;

#ifndef _ISAMPLER_
#define _ISAMPLER_

#include "libarray/libarray.h"

/** An interface for getting multi-dimensional samples.
 *  This interface is used by sampling algorithm to get multidimensional samples.
 */
struct ISampler
{
  virtual void reset(void) = 0;                ///< Reset the sequence of samples to the begining.
  virtual real getSample(real *s) = 0;         ///< Get a sample from the current position in th e sequence.
  virtual void setSamples(integer n) = 0;      ///< Set the number of samples to be taken.
  virtual void setDim(integer n) = 0;          ///< Set the dimension of the samples to be produced.
};
#endif
