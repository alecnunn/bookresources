/*
s_p_oneil@hotmail.com
Copyright (c) 2000, Sean O'Neil
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
* Neither the name of this project nor the names of its contributors
  may be used to endorse or promote products derived from this software
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __StarSystem_h__
#define __StarSystem_h__

#include "Star.h"
#include "Planet.h"

class CStarSystem : public CStar
{
protected:
	int m_nPlanets;
	CPlanet *m_pPlanets;
	CPtrList m_llPlanets;

public:
	CStarSystem()
	{
		m_pPlanets = NULL;
	}
	CStarSystem(CInfo &info)
	{
		m_pPlanets = NULL;
		Init(info);
	}
	~CStarSystem()
	{
		Cleanup();
	}

	void Init(CInfo &info);
	void Cleanup();
	void Update(C3DObject *pCamera, float fSeconds, int nPolygonMode, float fMaxError, bool bUpdate, bool bTexture);
	void Draw(C3DObject *pCamera, float fSeconds, int nPolygonMode, float fMaxError, bool bUpdate, bool bTexture);

	int GetPlanetCount()		{ return m_nPlanets; }
	CPlanet *GetPlanet(int i)	{ return &m_pPlanets[i]; }
	CPtrListNode *GetHeadPlanet()	{ return m_llPlanets.GetHead(); }
	CPtrListNode *GetTailPlanet()	{ return m_llPlanets.GetTail(); }
};

#endif // __StarSystem_h__
