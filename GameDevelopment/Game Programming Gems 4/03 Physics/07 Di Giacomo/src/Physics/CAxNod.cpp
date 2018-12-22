// **********************************************************************
// * FILE  : CAxNod.cpp
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

#include "CAxNod.h"

// ********************
// * CAxNod class
// ********************

CAxNod::CAxNod(const int& index, CNet* wire) 
{
	m_Wire = wire;
	
	m_Kd = 0.975f;
	m_Contact = new tVect[3];

	m_Ks = 0.3f;
		
	m_Index = index;

	NullVector(&m_Velocity);
	NullVector(&m_Position);
	NullVector(&m_Force);

	SetVector(&m_EnvNormal, 0.f, 1.f, 0.f);

	m_Son = 
	m_Father = NULL;
	m_RestLength = 
	m_CurrentLength = m_Wire->GetAxRestLg();

	// Enveloppe
	std::vector<CEnvNod*> lBros(m_Wire->GetNbEnv());
	for(int i = 0; i < m_Wire->GetNbEnv(); i++)
		lBros[i] = new CEnvNod(this, i, m_Wire->GetNURBSAcc());		
	m_Bros = lBros;
	
	lBros.clear();

	for(i = 0; i < m_Wire->GetNbEnv(); i++)
	{
 		m_Bros[i]->SetBros(m_Bros[(i+1)%m_Wire->GetNbEnv()]);
	}
}

void CAxNod::CheckWorldCollisions(CMyObj* obj)
{
	m_ContactCnt = 0;

	for (int i = 0; i <6; i++)
	{
		m_Wire->SetAxWorldColliding(false);
		// Detect collision
		float axbyczd = DotVector(&m_Position, &collisionPlane[i].normal) + collisionPlane[i].d;
		// Mass "depthEpsilon near" the plane
		if(axbyczd < -m_Wire->GetAxDepthEpsilon())
		{
			float relativeVelocity = DotVector(&collisionPlane[i].normal, &m_Velocity);
			// Mass going the wrong way
			if(relativeVelocity < 0.f)
			{
	 			m_Wire->SetAxWorldColliding(true);
				CopyVector(&collisionPlane[i].normal, &m_Contact[m_ContactCnt]);
				m_ContactCnt++;
			}
		}
		// Resolve collisions
		if (m_Wire->GetAxWorldColliding())
		{	
			tVect		Vn,Vt;
			for (int j = 0; j < m_ContactCnt; j++)
			{
				float VdotN = DotVector(&m_Contact[j], &m_Velocity);
				ScaleVector(&m_Contact[j], VdotN, &Vn);
				SubVector(&m_Velocity, &Vn, &Vt);
				ScaleVector(&Vn, m_Wire->GetAxKr(), &Vn);
				SubVector(&Vt, &Vn, &m_Velocity);
			}
		}
	}

	for (i = 0; i < m_Wire->GetNbEnv(); i++)
	{
		m_Bros[i]->CheckWorldCollisions(obj);
	}
	if (m_Son)  
		m_Son->CheckWorldCollisions(obj);
}

void CAxNod::ComputeForce() 
{
	tVect		force;

	if (m_Index!=0)
		CopyVector(&m_Wire->GetGravity(), &m_Force);
	// Compute force applied by father
	if (m_Father)
	{
		// update father's current length
		tVect diff;
		SubVector(&m_Position, &m_Father->m_Position, &diff);
		m_Father->m_CurrentLength = LengthVector(&diff);
		if (m_Father->m_CurrentLength > m_Wire->GetAxSpringTolerance())
		{
			ScaleVector(&diff, 1/m_Father->m_CurrentLength, &force);
			float intensity = m_Father->m_Ks*(m_Father->m_CurrentLength-m_Father->m_RestLength);				
			ScaleVector(&force, intensity, &force);
			SubVector(&m_Force, &force, &m_Force);
		}
		if (!m_Son)
		{
			SetVector(&m_EnvNormal, m_Position.x - m_Father->m_Position.x, m_Position.y - m_Father->m_Position.y, m_Position.z - m_Father->m_Position.z);
			ComputeOrtho(&m_EnvNormal, &m_Bros[0]->m_VectDir, &m_Bros[1]->m_VectDir, &m_Bros[2]->m_VectDir, &m_Bros[3]->m_VectDir);
		}
	}
	// Compute force applied by axial son
	if (m_Son)
	{ 
		if (m_Index!=0)
		{
			// update current length
			tVect diff;
			SubVector(&m_Son->m_Position, &m_Position, &diff);
			m_CurrentLength = LengthVector(&diff);
			if (m_CurrentLength > m_Wire->GetAxSpringTolerance())
			{
				ScaleVector(&diff, 1/m_CurrentLength, &force);
				float intensity = m_Ks*(m_CurrentLength-m_RestLength);
				ScaleVector(&force, intensity, &force);
				SumVector(&m_Force, &force, &m_Force);
			}
		}
		m_Son->ComputeForce();

		SetVector(&m_EnvNormal, m_Son->m_Position.x - m_Position.x, m_Son->m_Position.y - m_Position.y, m_Son->m_Position.z - m_Position.z); 
		ComputeOrtho(&m_EnvNormal, &m_Bros[0]->m_VectDir, &m_Bros[1]->m_VectDir, &m_Bros[2]->m_VectDir, &m_Bros[3]->m_VectDir);
	}

	for (int i = 0; i < m_Wire->GetNbEnv(); i++)
	{
		m_Bros[i]->ComputeForce();
	}
}

void CAxNod::Integrate() 
{
	tVect acc;
	ScaleVector(&m_Force, m_InvMass, &acc);

	SumVector(&m_Velocity, &acc, &m_Velocity);
	SumVector(&m_Position, &m_Velocity, &m_Position);
	// Damping
	ScaleVector(&m_Velocity, m_Kd, &m_Velocity);

	// Integrate enveloppe bros
	for (int i = 0; i < m_Wire->GetNbEnv(); i++)
	{
		m_Bros[i]->Integrate();
	}
	// Integrate axial sons
	if (m_Son)  
		m_Son->Integrate();
}


void CAxNod::Reset() 
{
	NullVector(&m_Velocity); 
	NullVector(&m_Position);
	NullVector(&m_Force);
	NullVector(&m_EnvNormal);
	
	// Reset enveloppe bros
	for (int i = 0; i < m_Wire->GetNbEnv(); i++)
	{
		m_Bros[i]->Reset();
	}

	// Reset axial sons
	if (m_Son)
		m_Son->Reset();
}

