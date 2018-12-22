/* Copyright (C) William Leeson, 2003. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) William Leeson, 2003"
 */
/**************************************************************************************
 *              Bayes-Durham Shuffled L'Ecuyer random number generator.               *
 **************************************************************************************
 * File:ShuffledLEcuyer.h
 * Date:$Date: 2003/01/31 14:32:36 $
 * Author:$Author: leesonw $
 * Version:$Revision: 1.1 $
 * Description: A psuedo random number sequence generator which takes any
 *              generator and applies a Bays-Durham shuffle as found in 
 *              "Numerical Recipies in C".
 */
class ShuffledLEcuyer;

#ifndef _SHUFFLED_LECUYER_RANDOM_GENERATOR_
#define _SHUFFLED_LECUYER_RANDOM_GENERATOR_

#include "IGenerator.h"
#include "libarray/libarray.h"
#include <math.h>
#include <iostream>

/** Bays-Durham shuffled L'Ecuyer psuedo-random number generator.
 *  Takes an L'Ecuyer generator and applies a Bays-Durham shuffle to it.
 */
class ShuffledLEcuyer : public IGenerator
{      
 private:
  integer m_im1,
    m_im2,
    m_imm1,
    m_im,
    m_ia1,
    m_ia2,
    m_iq1,
    m_iq2,
    m_ir1,                                    
    m_ir2,
    m_ntab,                                    ///< Table size.
    m_ndiv;                                    ///< Used to ge correct offset into table.
  real m_am;
  const real m_eps,                            ///< Smallest real number
    m_rnmx;                                    ///< Smallest real number less than 1.0.
  integer *mp_iv,                              ///< Table used for shuffling.
    m_iy,
    m_seed,
    m_idnum1,
    m_idnum2;
 public:
  //(Con/De)structor
  ShuffledLEcuyer();                           ///< Constructor to initialise variables.

  // IGenerate
  virtual real generate(real *s);              ///< Generate next number in sequence
  virtual void reset();                        ///< Reset sequence to begining.
	  
  // properties and initialisation
  virtual void setSeed(const integer n);  ///< Set the seed for the sequence.
  virtual integer getSeed(void) const;    ///< Get the seed for the sequence.
};
#endif


