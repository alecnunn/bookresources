// **********************************************************************
// * FILE  : CHermInterp.h
// *---------------------------------------------------------------------
// * PURPOSE : cubic hermitte interpolation
// *
// *---------------------------------------------------------------------
// * AUTHOR   : Thomas Di Giacomo . MIRALab (giacomo@miralab.unige.ch)
// **********************************************************************
// * COPYRIGHT :
/*
 * This copyright notice must be included in all copies or derivative works.
 * (c) Copyright 2003, MIRALab, C.U.I. University of Geneva 
 * ALL RIGHTS RESERVED
 * Permission to use, copy, modify, and distribute this software for
 * any purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright notice
 * and this permission notice appear in supporting documentation, and that
 * the name of MIRALab not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL MIRALAB
 * BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
 * KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
 * LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
 * THIRD PARTIES, WHETHER OR NOT MIRALAB  HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
 * POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#ifndef __CHERMINTERP_H__
#define __CHERMINTERP_H__

#include "tDef.h"

class CHermInterp
{
public:

	CHermInterp(tVect, tVect, tVect, tVect);
	CHermInterp();

  ~CHermInterp(){};


	tVect operator() (const float& a) const 
	{
		tVect v1, v2, v3, v4, res;
		ScaleVector(&m_X0, (2*a-3)*a*a+1, &v1);
		ScaleVector(&m_X_0, ((a-2)*a+1)*a, &v2);
		ScaleVector(&m_X1, (-2*a+3)*a*a, &v3);
		ScaleVector(&m_X_1, (a-1)*a*a, &v4);
		SumVector(&v1, &v2, &res);
		SumVector(&res, &v3, &res);
		SumVector(&res, &v4, &res);
		return res;
	}


	void	SetX0(tVect x0)		{  m_X0=x0;};
	void	SetX1(tVect x1)		{  m_X1=x1;};
	void	SetX_0(tVect x_0)	{  m_X_0=x_0;};
	void	SetX_1(tVect x_1)	{  m_X_1=x_1;};

	
	inline	const	tVect& GetX0()				{  return m_X0;};
	inline	const	tVect& GetX1()				{  return m_X1;};
	inline	const	tVect& GetX_0()				{  return m_X_0;};
	inline	const	tVect& GetX_1()				{  return m_X_1;};
	
private:
	tVect	m_X0;
	tVect	m_X1;
	tVect	m_X_0;
	tVect	m_X_1;
};

#endif // __CHERMINTERP_H__
