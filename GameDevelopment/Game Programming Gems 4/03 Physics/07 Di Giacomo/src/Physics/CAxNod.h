// **********************************************************************
// * FILE  : CAxNod.h
// *---------------------------------------------------------------------
// * PURPOSE : handles axial nodes (axial deformations)
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



#ifndef _CAxNod_h_
#define _CAxNod_h_

#include "CMass.h"
#include "CEnvNod.h"
#include "CNet.h"

class CAxNod : public CMass
{
public:
	friend class CEnvNod;
	friend class CNet;

  CAxNod(const int&, CNet*);
	~CAxNod() 
	{ 	
		if (m_Contact)
		{
			delete[] m_Contact;
			m_Contact = NULL;
		}
	};


	void		SetKd(const float& f)			{  m_Kd=f;};
	void		SetKs(const float& f)			{  m_Ks=f;};
	void		SetPosition(const tVect& p)		{  m_Position=p;};
	void		SetVelocity(const tVect& v)		{  m_Velocity=v;};
	void		SetForce(const tVect& f)		{  m_Force=f;};
	void		SetRestLength(const float& l)	{  m_RestLength=l;};
	void		SetCurrentLength(const float& l){  m_CurrentLength=l;};
	void		SetSon(CAxNod* s)	
	{  
		m_Son=s; 
		m_Son->m_Father = this; 
		m_Son->m_Index = m_Index + 1;
	};


	inline	const	float&			GetKd()								{  return m_Kd;};
	inline	const	float&			GetKs()								{  return m_Ks;};
	inline	const	tVect&			GetPosition()					{  return m_Position;};
	inline	const	tVect&			GetVelocity()					{  return m_Velocity;};
	inline	const	tVect&			GetForce()						{  return m_Force;};
  inline  CNet*							GetWire()							{  return m_Wire;};
  inline  CAxNod*						GetSon()							{  return m_Son;};
	inline	CEnvNod*					GetBros(const int& i)	{  return m_Bros[i];};
	inline	const	float&			GetRestLength()				{  return m_RestLength;};
	inline	const	float&			GetCurrentLength()		{  return m_CurrentLength;};


	void		ComputeForce();
	void		Integrate();
	void		CheckWorldCollisions(CMyObj*);
	void		Reset();

protected:
	tVect									m_Position;
	tVect									m_Velocity;
	tVect									m_Force;

	float									m_Ks;			
	float									m_Kd;
	float									m_CurrentLength;
	float									m_RestLength;
	
	CNet*									m_Wire;
	
	CAxNod*									m_Son;
	CAxNod*									m_Father;

	tVect									m_EnvNormal;
	std::vector<CEnvNod*>	m_Bros;

	tVect*									m_Contact;
	int										m_ContactCnt;
};

#endif // _CAxNod_h_