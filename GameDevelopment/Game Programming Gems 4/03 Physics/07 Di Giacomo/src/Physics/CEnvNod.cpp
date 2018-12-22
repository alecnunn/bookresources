// **********************************************************************
// * FILE  : CEnvNod.cpp
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

#include "CEnvNod.h"

tCollisionPlane collisionPlane[6];

// ********************
// * CEnvNod class
// ********************

CEnvNod::CEnvNod(CAxNod* bg, const int& i, const int& n) 
{
	std::vector<tVect> lNurbsPts(n);
	for(int j = 0; j < n; j++)
	{
		NullVector(&lNurbsPts[j]);		
	}
	m_NurbsPts = lNurbsPts;
	lNurbsPts.clear();
	
	m_BrosGlob = bg;

	m_BrosLeft = NULL;
	m_Index = bg->m_Index;
	m_EnvIndex = i;
	NullVector(&m_VectDir);
	switch (m_EnvIndex)
	{
		case 0:
			m_VectDir.x = -1.f;
		break;
		case 1:
			m_VectDir.z = -1.f;
		break;
		case 2:
			m_VectDir.x = 1.f;
		break;
		case 3:
			m_VectDir.z = 1.f;
		break;
	}
	NormalVector(&m_VectDir);

	m_Constrained = false;

	m_AxVelocity = 0.f;
	m_KsEnv = 0.1f;
	m_KdEnv = 0.875f;
	m_RestEnvLength =
	m_AxPosition = 
	m_CurrentEnvLength = m_BrosGlob->GetWire()->GetEnvRestLg();

	m_AxForce =
	m_ContactCnt = 0;
	m_Contact = new tVect[3];
}



void CEnvNod::ComputeNurbsCrossSection(const tVect& t)
{
	tVect thisDer, brosDer;
	tVect brosPos = m_BrosLeft->Compute3DRelativePosition();
	ScaleVector(&t, -1.5f, &brosDer);
	ScaleVector(&brosPos, 1.5f, &thisDer);
	CHermInterp interp = CHermInterp(t, thisDer, brosPos, brosDer);
	CAlpha alpha = CAlpha(0.f, 1.f);
 	m_NurbsPts[0] = interp(alpha(0.f));
	for(int e = 1; e <= m_BrosGlob->GetWire()->GetNURBSAcc(); e++)
	{
		m_NurbsPts[e] = interp(alpha((float) e / m_BrosGlob->GetWire()->GetNURBSAcc()));
	}
	if (m_BrosGlob->m_Son)
	{
		tVect s = m_BrosGlob->m_Son->m_Bros[m_EnvIndex]->Compute3DRelativePosition();
		m_BrosGlob->m_Son->m_Bros[m_EnvIndex]->ComputeNurbsCrossSection(s);
	}
}

void CEnvNod::Integrate()
{
	if (!m_Constrained)
	{
		float accA = m_AxForce * m_InvMass;		
		m_AxVelocity += accA;
		m_AxPosition += m_AxVelocity;
		m_AxVelocity *= m_KdEnv;
		m_AxForce = 0.f;
	}
}


void CEnvNod::CheckWorldCollisions(CMyObj* obj)
{
	m_ContactCnt = 0;

	for (int i = 0; i <6; i++)
	{
		m_BrosGlob->GetWire()->SetEnvWorldColliding(false);
		
		// Detect collision
		tVect t = Compute3DRelativePosition();
		SumVector(&t, &m_BrosGlob->m_Position, &t);
		float axbyczd = DotVector(&t, &collisionPlane[i].normal) + collisionPlane[i].d;
		if(axbyczd < -m_BrosGlob->GetWire()->GetEnvDepthEpsilon())
		{
			float relativeVelocity = DotVector(&collisionPlane[i].normal, &m_BrosGlob->m_Velocity);
			if(relativeVelocity < 0.f)
			{
 				m_BrosGlob->GetWire()->SetEnvWorldColliding(true);
				CopyVector(&collisionPlane[i].normal, &m_Contact[m_ContactCnt]);
				m_ContactCnt++;
			}
		}
	}
	// Response to collision
	if (m_BrosGlob->GetWire()->GetEnvWorldColliding())
	{
		tVect		Vn,Vt;
		for (i = 0; i < m_ContactCnt; i++)
		{
			float VdotN = DotVector(&m_Contact[i], &m_BrosGlob->m_Velocity);
			ScaleVector(&m_Contact[i], VdotN, &Vn);
			SubVector(&m_BrosGlob->m_Velocity, &Vn, &Vt);
			ScaleVector(&Vn, m_BrosGlob->GetWire()->GetEnvKr(), &Vn);
			SubVector(&Vt, &Vn, &m_BrosGlob->m_Velocity);
			m_AxForce = DotVector(&m_BrosGlob->m_Velocity, &m_VectDir);
		}
	}
	CheckObjectCollisions(obj);
}

void CEnvNod::CheckObjectCollisions(CMyObj* obj)
{	
	tVect t = Compute3DRelativePosition();
	SumVector(&t, &m_BrosGlob->m_Position, &t);
	SubVector(&obj->GetPosition(), &t, &t);
	
	if (LengthVector(&t) <= (obj->GetSize()+0.0001f))
	{
		m_AxPosition += 0.01f;
		m_Constrained = true;
	}
	else
		m_Constrained = false;
}

tVect CEnvNod::Compute3DRelativePosition()
{
	tVect v;
	ScaleVector(&m_VectDir, m_AxPosition, &v);
	return v;
}

void CEnvNod::ComputeForce()
{
	if (!m_Constrained)
	{
		tVect t = Compute3DRelativePosition();
		m_CurrentEnvLength = LengthVector(&t);

		if (m_CurrentEnvLength > m_BrosGlob->GetWire()->GetEnvSpringTolerance())
		{
			ScaleVector(&t, 1/m_CurrentEnvLength, &t);
			float r = DotVector(&t, &m_VectDir);
			float intensity = m_KsEnv * (m_CurrentEnvLength - m_RestEnvLength);
			m_AxForce -= r*intensity;
		}
	}
}

void CEnvNod::Reset()
{
	m_CurrentEnvLength =
	m_RestEnvLength =
	m_AxPosition = m_BrosGlob->GetWire()->GetEnvRestLg();
	m_AxForce = 0.f;
	m_AxVelocity = 0.f;
	
	NullVector(&m_VectDir);
	switch (m_EnvIndex)
	{
		case 0:
			m_VectDir.x = -1.f;
		break;
		case 1:
			m_VectDir.z = -1.f;
		break;
		case 2:
			m_VectDir.x = 1.f;
		break;
		case 3:
			m_VectDir.z = 1.f;
		break;
	}
	NormalVector(&m_VectDir);
	for (int i=0; i<=m_BrosGlob->GetWire()->GetNURBSAcc(); i++)
	{
		NullVector(&m_NurbsPts[i]);
	}
}
