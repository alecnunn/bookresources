// **********************************************************************
// * FILE  : CNet.cpp
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

#include "CNet.h"

CNet::CNet(const int& d, const int& e, const int& a) 
{	
	m_AxKr = 0.95f;
	m_AxDepthEpsilon = 0.00001f;
	m_AxSpringTolerance = 0.0000005f;
	m_AxRestLg = 0.02f;

	m_EnvRestLg = 0.15f;
	m_EnvSpringTolerance = 0.0000005f;
	m_EnvKr = 0.9f;
	m_EnvDepthEpsilon = 0.0001f;
	
	tVect grav = {0.f, -0.00004f, 0.f}; 
	CopyVector(&grav, &m_Gravity);
	
	m_DrawSize = 0.035f;
	m_NMass = d;
	m_NEnv = e;
	m_NURBSAcc = a;

	for (int j = 0; j < 5; j++)
		m_DrawMode[j] = 0;

	m_Root=	new CAxNod(0, this);

	std::vector<CAxNod*> lMass(m_NMass);
	lMass[0] = m_Root;
	for(j = 1; j < m_NMass; j++)
	{
		lMass[j] = new CAxNod(j, this);		
		lMass[j-1]->SetSon(lMass[j]);
	}
	m_Mass = lMass;
	lMass.clear();
}

void CNet::CheckWorldCollisions(CMyObj* obj)
{
	m_Root->CheckWorldCollisions(obj);
}


void CNet::ComputeForce() 
{
	m_Root->ComputeForce();
}

void CNet::Integrate() 
{
	m_Root->Integrate();
	NullVector(&m_Root->m_Force);
}

void CNet::Reset() 
{
	m_Root->Reset();
}
