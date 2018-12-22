/* Copyright (C) William Leeson, 2003. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) William Leeson, 2003"
 */
/********************************************************************************
 *              Bayes-Durham Shuffled L'Ecuyer random number generator.         *
 ********************************************************************************
 * File:ShuffledLEcuyer.cc
 * Date:$Date: 2003/01/31 14:32:36 $
 * Author:$Author: leesonw $
 * Version:$Revision: 1.1 $
 * Description: A L'Ecuyer psuedo random number sequence generator with a
 *              Bays-Durham shuffle as found in "Numerical Recipies in C".
 */
#include "ShuffledLEcuyer.h"

/*
 *		Default constructor
 */
ShuffledLEcuyer::ShuffledLEcuyer() : m_im1(2147483563),
				     m_im2(2147483399),
                                     m_imm1(m_im1 - 1),
				     m_ia1(40014),
				     m_ia2(40692),
				     m_iq1(53668),
				     m_iq2(52774),
				     m_ir1(12211),
				     m_ir2(3791),
				     m_ntab(32),
				     m_ndiv(1 + m_imm1/m_ntab),
				     m_am(1.0/real(m_im1)),
				     m_eps(1.2e-7),
				     m_rnmx(1.0 - m_eps),
				     mp_iv(NULL),
				     m_iy(0),
				     m_seed(100),
				     m_idnum1(m_seed),
				     m_idnum2(m_seed)
{
  // setup with some reasionable default values
  mp_iv = new integer[m_ntab];
}

/*
 *     generate next in sequence
 */
real ShuffledLEcuyer::generate(real *s)
{
  integer j,k;
  real temp;

  k = m_idnum1*m_iq1;
  m_idnum1 = m_ia1*(m_idnum1 - k*m_iq1) - k*m_ir1;
  if(m_idnum1 < 0)
    m_idnum1 += m_im1;

  k = m_idnum2/m_iq2;
  m_idnum2 = m_ia2*(m_idnum2 - k*m_iq2) - k*m_ir2;
  if(m_idnum2 < 0)
    m_idnum2 += m_im2;
  j = m_iy/m_ndiv;

  m_iy = mp_iv[j] - m_idnum2;
  mp_iv[j] = m_idnum1;
  if(m_iy < 1)
    m_iy += m_imm1;
  if((temp = m_am*m_iy) > m_rnmx)
    *s = m_rnmx;
  else
    *s = temp;

  return 1.0;
}

/*
 *	Set Seed
 */
void ShuffledLEcuyer::setSeed(integer n)
{
  m_seed = n;
}
  
/*
 *	Get Seed
 */
integer ShuffledLEcuyer::getSeed(void) const
{
  return m_seed;
}

/*
 *   reset the generator
 */
void ShuffledLEcuyer::reset(void)
{
  integer j,k;
  
  m_idnum1 = m_idnum2 = -m_seed;
  
  for(j = m_ntab + 7;j >= 0;j--)
    {
      k = m_idnum1/m_iq1;
      m_idnum1 = m_ia1*(m_idnum1 - k*m_iq1) - k*m_ir1;
      if(m_idnum1 < 0)
	m_idnum1 += m_im1;
      if(j < m_ntab)
	mp_iv[j] = m_idnum1;
    }
  m_iy = mp_iv[0];
}
