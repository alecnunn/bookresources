// **********************************************************************
// * FILE  : CEnvNod.h
// *---------------------------------------------------------------------
// * PURPOSE : handles enveloppe nodes (radial deformations)
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


#ifndef _CEnvNod_h_
#define _CEnvNod_h_

#include "../Math/CHermInterp.h"
#include "../Math/CAlpha.h"

#include "CMass.h"
#include "CMyObj.h"
#include "CAxNod.h"

class CEnvNod : public CMass {
public:
	friend class CAxNod;
  
	CEnvNod(CAxNod*, const int&, const int&);

	~CEnvNod() 
	{ 	
		if (m_Contact)
		{
			delete[] m_Contact;
			m_Contact = NULL;
		}
 	};


	void		SetKdEnv(const float& f)			{  m_KdEnv=f;};
	void		SetKsEnv(const float& f)			{  m_KsEnv=f;};
	void		SetAxForce(const float& f)		{  m_AxForce=f;};
	void		SetAxPosition(const float& f)	{  m_AxPosition=f;};
	void		SetBros(CEnvNod *bl)					{  m_BrosLeft=bl;};
	void		SetConstrained(const bool& b)	{  m_Constrained=b;};

	inline const	tVect&			GetVectDir()							{  return m_VectDir;};
	inline const	tVect&			GetNurbsPts(const int& i)	{  return m_NurbsPts[i];};
	inline const	float&			GetAxPosition()						{  return m_AxPosition;};
	inline const	float&			GetKdEnv()								{  return m_KdEnv;};
	inline const	float&			GetKsEnv()								{  return m_KsEnv;};
	inline const	int&				GetEnvIndex()							{  return m_EnvIndex;};
	inline				CAxNod*			GetBrosGlob()							{  return m_BrosGlob;};

	void		ComputeForce();
	void		CheckWorldCollisions(CMyObj*);
	void		CheckObjectCollisions(CMyObj*);
	void		Integrate();
	void		Reset();
	tVect		Compute3DRelativePosition();
	void		ComputeNurbsCrossSection(const tVect&);

protected:
	bool			m_Constrained;
	int				m_EnvIndex;
	tVect			m_VectDir;
	float			m_AxPosition;
	float			m_AxVelocity;
	float			m_AxForce;

	float			m_KsEnv;
	float			m_KdEnv;
	float			m_CurrentEnvLength;
	float			m_RestEnvLength;
	
	tVect*		m_Contact;
	int				m_ContactCnt;

	CAxNod*		m_BrosGlob;
	CEnvNod*	m_BrosLeft;

	std::vector<tVect>	m_NurbsPts;
};


#endif // _CEnvNod_h_
