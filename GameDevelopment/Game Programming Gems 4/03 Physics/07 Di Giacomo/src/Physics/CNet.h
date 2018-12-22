// **********************************************************************
// * FILE  : CNet.h
// *---------------------------------------------------------------------
// * PURPOSE : handles global pipes
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


#ifndef _CNet_h_
#define _CNet_h_

#include "CAxNod.h"

class CNet 
{
public:
	friend class CAxNod;

	CNet(const int&, const int&, const int&);
	CNet(){};

  ~CNet() 	{ 	};

	void	SetDrawMode(int i)						{  m_DrawMode[i]=!m_DrawMode[i];}
	void	SetAxWorldColliding(bool b)		{  m_AxWorldColliding = b;}
	void	SetEnvWorldColliding(bool b)	{  m_EnvWorldColliding = b;}


	inline	CAxNod*						GetRoot()								{  return m_Root;};
	inline	const	bool&				GetDrawMode(int i)			{  return m_DrawMode[i];};
	inline	bool*							GetDrawMode()						{  return m_DrawMode;};
	inline	const	float&			GetDrawSize()						{  return m_DrawSize;};
	inline	const	int&				GetNbEnv()							{  return m_NEnv;};
	inline	const	int&				GetNbMass()							{  return m_NMass;};
	inline	const	int&				GetNURBSAcc()						{  return m_NURBSAcc;};
	inline	const	float&			GetAxKr()								{  return m_AxKr;};
	inline	const	float&			GetAxDepthEpsilon()			{  return m_AxDepthEpsilon;};
	inline	const	float&			GetAxSpringTolerance()	{  return m_AxSpringTolerance;};
	inline	const	float&			GetAxRestLg()						{  return m_AxRestLg;};
	inline	const	float&			GetEnvKr()							{  return m_EnvKr;};
	inline	const	float&			GetEnvDepthEpsilon()		{  return m_EnvDepthEpsilon;};
	inline	const	float&			GetEnvSpringTolerance()	{  return m_EnvSpringTolerance;};
	inline	const	float&			GetEnvRestLg()					{  return m_EnvRestLg;};
	inline	const	tVect&			GetGravity()						{  return m_Gravity;};
	inline	const	bool&				GetEnvWorldColliding()	{  return m_EnvWorldColliding;};
	inline	const	bool&				GetAxWorldColliding()		{  return m_AxWorldColliding;};


	void		ComputeForce();
	void		Integrate();
	void		CheckWorldCollisions(CMyObj*);
	void		Reset();

protected:
	CAxNod*							m_Root;
	float							m_DrawSize;
	
	int								m_NMass;
	int								m_NEnv;
	int								m_NURBSAcc;

	std::vector<CAxNod*> m_Mass;

	bool							m_DrawMode[5];
	// 0 Mass
	// 1 Enveloppe
	// 2 Force
	// 3 Velocity
	// 4 Rendered

	float		m_AxKr;
	float		m_AxDepthEpsilon;
	float		m_AxSpringTolerance;
	float		m_AxRestLg;
	bool		m_AxWorldColliding;

	float		m_EnvRestLg;
	float		m_EnvSpringTolerance;
	float		m_EnvKr;
	float		m_EnvDepthEpsilon;
	bool		m_EnvWorldColliding;

	tVect		m_Gravity;
};

#endif // _CNet_h_
