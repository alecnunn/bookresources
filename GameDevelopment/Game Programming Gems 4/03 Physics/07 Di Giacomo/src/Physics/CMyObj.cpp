// **********************************************************************
// * FILE  : CMyObj.cpp
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

#include "CMyObj.h"
#include "CEnvNod.h"

static const tVect initPosition = {-1.f, 1.f, 0.f};

// ********************
// * CMyObj class
// ********************

CMyObj::CMyObj() 
{
	NullVector(&m_Velocity);
	m_Kd = 0.975f;
	m_Contact = new tVect[3];

	NullVector(&m_Force);
	CopyVector(&initPosition, &m_Position);
	m_Size = 0.2f;

	m_WorldColliding = false;
	m_Kr = 0.99f;
	m_DepthEpsilon = 0.0001f;
	m_WorldColliding = false;
}

CMyObj::CMyObj(const tVect* pos, const float& kr, const float& depthEpsilon) 
{
	NullVector(&m_Velocity);
	m_Kd = 0.975f;
	m_Kr = kr;
	m_DepthEpsilon = depthEpsilon;
	m_Contact = (tVect *)malloc(sizeof(tVect) * 3);

	NullVector(&m_Force);
	CopyVector(pos, &m_Position);
	m_Size = 0.2f;

	m_WorldColliding = false;
}

void CMyObj::CheckWorldCollisions()
{
	m_ContactCnt = 0;
	
	for (int i = 0; i <6; i++)
	{
		m_WorldColliding = false;
		
		// Detect collision
		float axbyczd = DotVector(&collisionPlane[i].normal, &m_Position) + collisionPlane[i].d;
		// Mass "depthEpsilon" near the plane
		if(axbyczd < -m_DepthEpsilon)
		{
			float relativeVelocity = DotVector(&collisionPlane[i].normal, &m_Velocity);
			// Mass going the wrong way
			if(relativeVelocity < 0.f)
			{
 				m_WorldColliding = true;
				CopyVector(&collisionPlane[i].normal, &m_Contact[m_ContactCnt]);
				m_ContactCnt++;
			}
		}

		// Response to collision
		if (m_WorldColliding)
		{
			tVect Vn,Vt;
			for (int i = 0; i < m_ContactCnt; i++)
			{
				float VdotN = DotVector(&m_Contact[i], &m_Velocity);
				ScaleVector(&m_Contact[i], VdotN, &Vn);
				SubVector(&m_Velocity, &Vn, &Vt);
				ScaleVector(&Vn, m_Kr, &Vn);
				SubVector(&Vt, &Vn, &m_Velocity);
			}
		}
	}
}

void CMyObj::Integrate()
{
	SumVector(&m_Velocity, &m_Force, &m_Velocity);
	SumVector(&m_Position, &m_Velocity, &m_Position);
	ScaleVector(&m_Velocity, m_Kd, &m_Velocity);
	NullVector(&m_Force);
}

void CMyObj::Reset()
{
	NullVector(&m_Force);
	CopyVector(&initPosition, &m_Position);
	NullVector(&m_Velocity);
}