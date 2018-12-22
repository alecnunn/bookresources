// **********************************************************************
// * FILE  : CMyObj.h
// *---------------------------------------------------------------------
// * PURPOSE : handles object
// *
// *---------------------------------------------------------------------
// * AUTHOR   : Thomas Di Giacomo . MIRALab (giacomo@miralab.unige.ch)
// **********************************************************************
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


#ifndef _CMyObj_h_
#define _CMyObj_h_

#include "../Math/tDef.h"

class CMyObj
{
public:
	CMyObj();
	CMyObj::CMyObj(const tVect*, const float&, const float&);
	~CMyObj() 
	{ 	
		if (m_Contact)		
				delete[] m_Contact;
	};

	void	SetForce(const tVect& p)		{  m_Force = p;};
	void	SetPosition(const tVect& p)	{  m_Position = p;};
	void	SetVelocity(const tVect& p)	{  m_Velocity = p;};
	void	SetSize(const float& r)			{  m_Size = r;};

	inline	const	tVect&	GetForce()			{  return m_Force;};
	inline	const	tVect&	GetPosition()		{  return m_Position;};
	inline	const tVect&	GetVelocity()		{  return m_Velocity;};
	inline	const	float&	GetSize()				{  return m_Size;};

	void	CheckWorldCollisions();
	void	Integrate();
	void	Reset();

protected:
	tVect				m_Force;
	tVect				m_Velocity;
	tVect				m_Position;
	float				m_Size;
	float				m_Kd;
	float				m_Kr;
	float				m_DepthEpsilon;
	bool				m_WorldColliding;

	tVect*			m_Contact;
	int					m_ContactCnt;
};

#endif // _CMyObj_h_
