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

#include "PlanetInfo.h"
#include "Impostor.h"
#include "ROAMSphere.h"
#include "Star.h"
#define ATMOSPHERE_LEVEL		20
#define ATMOSPHERE_HALF_LEVEL	(ATMOSPHERE_LEVEL/2)

class CPlanet : public CImpostor, public CROAMSphere
{
protected:
	static CVector m_vAtmosphereNormal[ATMOSPHERE_LEVEL+1][ATMOSPHERE_LEVEL+1];
	static float m_fAtmosphereTexCoord[ATMOSPHERE_LEVEL+1];

	void DrawAtmosphere();
	void SetupAtmosphereVertex(int x, int y)
	{
#ifdef ATMOSPHERE_2D
		glTexCoord2f(m_fAtmosphereTexCoord[x], m_fAtmosphereTexCoord[y]);
#else
		glTexCoord1f(sqrtf(Square((x-ATMOSPHERE_HALF_LEVEL)/(float)ATMOSPHERE_HALF_LEVEL) + Square((y-ATMOSPHERE_HALF_LEVEL)/(float)ATMOSPHERE_HALF_LEVEL)));
#endif
		glNormal3fv(m_vAtmosphereNormal[x][y]);
	}

	CHeightMap m_mapHeight;
	CSurfaceMap m_mapSurface;
	CHeightMap m_mapSkyDome;
	CROAMSphere m_rsSkyDome;

	CVector m_vCameraPosition;			// The position of the camera (relative to the planet)
	CQuaternion m_qCameraOrientation;	// The orientation of the camera (relative to the planet)
	float m_fHeight;					// The distance between the camera and the ground directly beneath it
	float m_fAltitude;					// The distance between the camera and the planet (using average radius)
	float m_fHorizon;					// The distance from the camera to the planet's horizon (squared)
	bool m_bInAtmosphere;
	float m_fRelativeDensity;

	// Members loaded from config file
	char m_szName[_MAX_NAME+1];
	float m_fOrbitalDistance;		// The planet's average orbital distance from its star
	float m_fAngularVelocity;		// The speed at which the planet rotates (radians/sec)
	float m_fAtmosphereDensity;		// Atmosphere density (in ratio to Earth's atmosphere)
	float m_fAtmosphereThickness;	// Atmosphere thickness (in kilometers)
	CVector4 m_vAtmosphereColor;	// Primary color of the atmosphere
	CVector4 m_vHazeColor;			// Primary haze color for the atmosphere
	CVector4 m_vCloudColor;			// Primary cloud color for the atmosphere
	int m_nColors;
	CColor *m_pColors;
	int m_nMoons;
	CPlanet *m_pMoons;

public:
	CPlanet()
	{
		m_pColors = NULL;
		m_pMoons = NULL;
	}
	~CPlanet()
	{
		Cleanup();
	}
	static void BuildEarthTexture(int nSeed, int nSize);
	static void InitStaticMembers(int nSeed, int nSize=128);
	void Init(CInfo &info);
	void Cleanup();

	const char *GetName()	{ return m_szName; }
	int GetMoonCount()		{ return m_nMoons; }
	CPlanet *GetMoon(int i)	{ return &m_pMoons[i]; }

	float GetHeight()		{ return m_fHeight; }
	float GetAltitude()		{ return m_fAltitude; }
	float GetHorizon()		{ return m_fHorizon; }
	float GetOrbitalDistance()	{ return m_fOrbitalDistance; }

	void SetViewpoint(C3DObject *pCamera, float fSeconds)
	{
		Rotate(CVector(0, 1, 0), m_fAngularVelocity*fSeconds);
		m_vCameraPosition = UnitInverse().RotateVector(pCamera->GetPosition() - GetPosition());
		m_qCameraOrientation = *pCamera;
		m_qCameraOrientation.Rotate(UnitInverse());

		float fOriginalHeight = m_vCameraPosition.Magnitude();
		m_fHeight = fOriginalHeight - m_mapHeight.GetHeight(m_vCameraPosition, fOriginalHeight);
		m_fAltitude = Max(4.0f, fOriginalHeight - m_mapHeight.GetRadius());
		m_fHorizon = m_fAltitude*m_fAltitude + 2.0f*m_fAltitude*m_mapHeight.GetRadius();	// Distance to horizon squared
		m_bInAtmosphere = m_fAltitude <= m_fAtmosphereThickness;
		m_fRelativeDensity = 0;
		if(m_fAtmosphereDensity > DELTA)
		{
			if(m_bInAtmosphere)
			{
				m_fRelativeDensity = m_fAtmosphereDensity * (m_fAtmosphereThickness - m_fAltitude) / m_fAtmosphereThickness;
				m_fRelativeDensity *= m_fRelativeDensity;
			}
		}
	}
	bool CollisionCheck()	{ return m_fHeight < DELTA; }
	bool CollisionCheck(C3DObject *pCamera)
	{
		CVector vCameraPosition = UnitInverse().RotateVector(pCamera->GetPosition() - GetPosition());
		float fOriginalHeight = vCameraPosition.Magnitude();
		m_fHeight = fOriginalHeight - m_mapHeight.GetHeight(m_vCameraPosition, fOriginalHeight);
		return m_fHeight < DELTA;
	}

	void Update(float fMaxError)
	{
		CVector vHeading = m_qCameraOrientation.GetViewAxis();
		CROAMSphere::Update(m_vCameraPosition, vHeading, m_fHorizon, fMaxError);
		if(m_bInAtmosphere)
			m_rsSkyDome.Update(m_vCameraPosition, vHeading, m_fHorizon+m_fAtmosphereThickness, 0.5f);
	}
	void Draw(C3DObject *pCamera, CStar *pStar, bool bTexture=true, bool bImpostor=false);
};
