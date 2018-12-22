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

#include "Master.h"
#include "Planet.h"


CVector CPlanet::m_vAtmosphereNormal[ATMOSPHERE_LEVEL+1][ATMOSPHERE_LEVEL+1];
float CPlanet::m_fAtmosphereTexCoord[ATMOSPHERE_LEVEL+1];

void CPlanet::InitStaticMembers(int nSeed, int nSize)
{
	for(int y=0; y<=ATMOSPHERE_LEVEL; y++)
	{
		m_fAtmosphereTexCoord[y] = y/(float)ATMOSPHERE_LEVEL;
		for(int x=0; x<=ATMOSPHERE_LEVEL; x++)
		{
			float fDx = Max(-1.0, Min(1.0, 1.25f * (float)(x-ATMOSPHERE_HALF_LEVEL)/(float)ATMOSPHERE_HALF_LEVEL));
			float fDy = Max(-1.0, Min(1.0, 1.25f * (float)(y-ATMOSPHERE_HALF_LEVEL)/(float)ATMOSPHERE_HALF_LEVEL));
			m_vAtmosphereNormal[x][y] = CVector(fDx, fDy, 1.2f-sqrtf(fDx*fDx + fDy*fDy));
			m_vAtmosphereNormal[x][y].Normalize();
		}
	}
}

void CPlanet::Init(CInfo &info)
{
	Cleanup();

	// Load some values into local variables for initializing the maps
	int nSeed = info.GetIntValue("Seed");
	int nResolution = info.GetIntValue("Resolution");
	int nCraterTopLevel = info.GetIntValue("CraterTopLevel");
	int nCraterBottomLevel = info.GetIntValue("CraterBottomLevel");
	int nCratersPerLevel = info.GetIntValue("CratersPerLevel");
	float fRadius = info.GetFloatValue("Radius");
	float fMaxHeight = info.GetFloatValue("MaxHeight");
	float fWaterLevel = info.GetFloatValue("WaterLevel");
	float fOctaves = info.GetFloatValue("Octaves");
	float fRoughness = info.GetFloatValue("Roughness");

	// Load some values into permanent members
	strcpy(m_szName, info.GetTextValue("Name"));
	m_fMass = info.GetFloatValue("Mass");
	m_fOrbitalDistance = info.GetFloatValue("OrbitalDistance") * 0.1f;
	m_fAngularVelocity = info.GetFloatValue("AngularVelocity");
	m_fAtmosphereDensity = info.GetFloatValue("AtmosphereDensity");
	m_fAtmosphereThickness = info.GetFloatValue("AtmosphereThickness");
	m_vAtmosphereColor = info.GetVector4Value("AtmosphereColor");
	m_vHazeColor = info.GetVector4Value("HazeColor");
	m_vCloudColor = info.GetVector4Value("CloudColor");
	m_nMoons = info.GetIntValue("Moons");
	m_nColors = info.GetIntValue("Colors");

	// Load the color map
	if(m_nColors)
	{
		m_pColors = new CColor[m_nColors];
		for(int i=0; i<m_nColors; i++)
		{
			char szName[20];
			sprintf(szName, "Color%d", i+1);
			m_pColors[i] = info.GetColorValue(szName);
		}
	}

	// Initialize the planetary maps and ROAM spheres
	m_mapHeight.InitCraters(nSeed, nCraterTopLevel, nCraterBottomLevel, nCratersPerLevel);
	m_mapHeight.InitFractal(nSeed, fOctaves, fRoughness);
	m_mapHeight.Init(nResolution, fRadius, fMaxHeight, fWaterLevel);
	m_mapSurface.Init(nResolution, &m_mapHeight, m_pColors, m_nColors);
	CROAMSphere::Init(&m_mapHeight, &m_mapSurface);
	m_mapSkyDome.Init(0, fRadius+m_fAtmosphereThickness);
	m_rsSkyDome.Init(&m_mapSkyDome, &m_mapSurface);

	// Initialize impostor values
	m_fBoundingRadius = (fRadius+m_fAtmosphereThickness) * 1.1f;
	CDoubleVector vPosition = m_pParent->GetPosition();
	vPosition.x += m_fOrbitalDistance;
	SetPosition(vPosition);

	// Last but not least, set up any moons that might revolve around this planet
	if(m_nMoons)
	{
		m_pMoons = new CPlanet[m_nMoons];
		char szKey[_MAX_PATH];
		for(int i=0; i<m_nMoons; i++)
		{
			sprintf(szKey, "%s.%d", m_szName, i+1);
			m_pMoons[i].SetParent(this);
			m_pMoons[i].Init(CInfo(info, szKey));
		}
	}

	/*
	for(int i=0; i<10000; i++)
	{
		CVector vPos(rand()-HALF_RAND, rand()-HALF_RAND, rand()-HALF_RAND);
		vPos *= fRadius / vPos.Magnitude();
		m_mapHeight.AddModification(CModification::CreateNewVolcano(vPos, fRadius/200, fRadius/200));
	}
	*/
}

void CPlanet::Cleanup()
{
	if(m_pMoons)
	{
		delete[] m_pMoons;
		m_pMoons = NULL;
	}
	if(m_pColors)
	{
		delete[] m_pColors;
		m_pColors = NULL;
	}
}

void CPlanet::DrawAtmosphere()
{
	float fStep = 1.0f/ATMOSPHERE_HALF_LEVEL;
	float fFactor = m_fAtmosphereThickness / (m_mapHeight.GetRadius()*1.25f);
	CVector vPos, v(-1, -1, 0);
	for(int y=0; y<ATMOSPHERE_LEVEL; y++)
	{
		v.x = -1;
		glBegin(GL_TRIANGLE_STRIP);
		for(int x=0; x<=ATMOSPHERE_LEVEL; x++)
		{
			SetupAtmosphereVertex(x, y+1);
			vPos = CVector(v.x, v.y+fStep, fFactor*sqrtf(Square((x-ATMOSPHERE_HALF_LEVEL)/(float)ATMOSPHERE_HALF_LEVEL) + Square((y+1-ATMOSPHERE_HALF_LEVEL)/(float)ATMOSPHERE_HALF_LEVEL)));
			glVertex3fv(vPos);
			SetupAtmosphereVertex(x, y);
			vPos = CVector(v.x, v.y, fFactor*sqrtf(Square((x-ATMOSPHERE_HALF_LEVEL)/(float)ATMOSPHERE_HALF_LEVEL) + Square((y-ATMOSPHERE_HALF_LEVEL)/(float)ATMOSPHERE_HALF_LEVEL)));
			glVertex3fv(vPos);
			v.x += fStep;
		}
		glEnd();
		v.y += fStep;
	}
}

void CPlanet::Draw(C3DObject *pCamera, CStar *pStar, bool bTexture, bool bImpostor)
{
	// First set up the lighting for this planet
	float fAmbient = 0.05f;
	CVector vLight1 = pStar->GetPosition()-GetPosition();
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT_AND_DIFFUSE, CVector4(1));
	glLightfv(GL_LIGHT0, GL_POSITION, CVector4(vLight1.x, vLight1.y, vLight1.z, 1));

	// If the planet has an atmosphere, set up a second light source for the light scattered by the atmosphere
	if(m_fAtmosphereDensity > DELTA)
	{
		// First calculate the angle between the light source and the camera (relative to the planet)
		CVector vLight2 = pCamera->GetPosition()-GetPosition();
		float fMagnitude = vLight1.Magnitude() * vLight2.Magnitude();
		float fDot = (vLight1 | vLight2)/fMagnitude;
		float fAngle = acosf(fDot);
		if(m_bInAtmosphere)
		{
			fAmbient = (fDot >= -.05) ? 1.0f : Max(0.05f, (0.15f+fDot)*10);
			fAmbient *= m_fRelativeDensity;
		}

		// Then rotate the light vector toward the camera up to 15 degrees to account for scattered light
		CVector vAxis = (vLight1 ^ vLight2) / fMagnitude;
		CQuaternion q(vAxis, Min(fAngle, DEGTORAD(15.0f)));
		vLight2 = q.RotateVector(vLight1);

		// Finally, set up the second light source and dim the first one to compensate for too much brightness
		glEnable(GL_LIGHT1);
		float fTemp = (1-m_fAtmosphereDensity) + 0.5f*m_fAtmosphereDensity*(1-fDot);
		glLightfv(GL_LIGHT0, GL_AMBIENT_AND_DIFFUSE, CVector4(fTemp, fTemp, fTemp, 1.0f));
		glLightfv(GL_LIGHT1, GL_AMBIENT_AND_DIFFUSE, CVector4(m_fAtmosphereDensity, m_fAtmosphereDensity, m_fAtmosphereDensity, 1.0f));
		glLightfv(GL_LIGHT1, GL_POSITION, CVector4(vLight2.x, vLight2.y, vLight2.z, 1.0f));
	}
	
	// Multiply the current view matrix by the model matrix and draw the planet
	CMatrix m;
	float fScale = 1.0f;

	if(!bImpostor)
	{
		fScale = GetScaledModelMatrix(m, pCamera);
		glPushMatrix();
		glMultMatrixf(m);
	}
	
	glEnable(GL_NORMALIZE);		// We have to enable this because we're using a scaled MODELVIEW matrix

	if(m_bInAtmosphere)
	{
		glEnable(GL_FOG);
		glFogi(GL_FOG_MODE, GL_LINEAR);
		glFogfv(GL_FOG_COLOR, m_vHazeColor*Min(1, m_fRelativeDensity*4)*fAmbient);
		glFogf(GL_FOG_START, m_fAltitude*fScale);
		glFogf(GL_FOG_END, sqrtf(m_fHorizon)*fScale);
	}
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, CVector4(fAmbient*0.25f));
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, CVector4(1));

	float fBlendRatio = (m_fAltitude / (m_mapHeight.GetRadius()*0.25f) - 0.5f);
	bool bFaceTextures = false;
	bool bCrossFade = false;
	if(bTexture)
	{
		bFaceTextures =  m_mapSurface.GetResolution() != 0 && (fBlendRatio > 0.0f);
		if(bFaceTextures)
		{
			// We're far enough away that it's better to use the pre-computed face textures
			GLUtil()->EnableTextureCoordArray(GL_TEXTURE0_ARB, 2);
			bCrossFade = (fBlendRatio < 1.0f) && GLUtil()->GetMaxTextureUnits() > 1 && GLUtil()->HasRegisterCombiners();
			if(bCrossFade)
			{
				// Gradually cross-fade between face textures and raw texture
				glActiveTextureARB(GL_TEXTURE1_ARB);
				GLUtil()->EnableTextureCoordArray(GL_TEXTURE1_ARB, 0);
				m_mapSurface.GetRawTextureMap().Enable();
				glActiveTextureARB(GL_TEXTURE0_ARB);

				glEnable(GL_REGISTER_COMBINERS_NV);
				fBlendRatio = sqrtf(fBlendRatio);
				glCombinerParameterfvNV(GL_CONSTANT_COLOR0_NV, CVector4(fBlendRatio, fBlendRatio, fBlendRatio, fBlendRatio));
				glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
				glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
				glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
				glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
				glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_SPARE0_NV, GL_SPARE1_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, 0, 0, 0);
				glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
				glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
				glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
				glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
				/* The code above should be the same as...
				char szBuffer[8192];
				sprintf(szBuffer, 
					"!!RC1.0\n"
					"const0 = (%f, %f, %f, %f);\n"
					"{\n"
					"  rgb {\n"
					"    spare0 = col0 * tex0;\n"
					"    spare1 = col0 * tex1;\n"
					"  }\n"
					"}\n"
					"out.rgb = lerp(const0, spare0, spare1);\n"
					"out.a = unsigned_invert(zero);\n",
					fRatio, fRatio, fRatio, fRatio);
				nvparse(szBuffer);
				*/
			}
		}
		else
		{
			// We're close enough to use the raw texture and detailed noise texture
			GLUtil()->EnableTextureCoordArray(GL_TEXTURE0_ARB, 0);
			m_mapSurface.GetRawTextureMap().Enable();
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			if(GLUtil()->GetMaxTextureUnits() > 1)
			{
				glActiveTextureARB(GL_TEXTURE1_ARB);
				GLUtil()->EnableTextureCoordArray(GL_TEXTURE1_ARB, 2);
				CTexture::GetNoise().Enable();
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
				glMatrixMode(GL_TEXTURE);
				glPushMatrix();
				glLoadIdentity();
				glScalef(128, 128, 128);
				glActiveTextureARB(GL_TEXTURE0_ARB);
			}
		}
	}

	BuildIndexList();
	for(int nFace=0; nFace<6; nFace++)
	{
		if(bFaceTextures)
			m_mapSurface.GetTextureMap(nFace).Enable();
		DrawFace(nFace);
		if(bFaceTextures)
			m_mapSurface.GetTextureMap(nFace).Disable();
	}

	if(bTexture)
	{
		if(bCrossFade)
			glDisable(GL_REGISTER_COMBINERS_NV);

		if(GLUtil()->GetMaxTextureUnits() > 1)
		{
			glActiveTextureARB(GL_TEXTURE1_ARB);
			GLUtil()->DisableTextureCoordArray(GL_TEXTURE1_ARB);
			CTexture::Disable(GL_TEXTURE_1D);	// Needed for now because raw texture is 1D. This should change later.
			CTexture::Disable(GL_TEXTURE_2D);
			if(!bFaceTextures)
			{
				glPopMatrix();
				glMatrixMode(GL_MODELVIEW);
			}
			glActiveTextureARB(GL_TEXTURE0_ARB);
		}
		GLUtil()->DisableTextureCoordArray(GL_TEXTURE0_ARB);
		CTexture::Disable(GL_TEXTURE_1D);	// Needed for now because raw texture is 1D. This should change later.
		CTexture::Disable(GL_TEXTURE_2D);
	}

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, CVector4(fAmbient*0.5f));
	if(m_bInAtmosphere)
	{
		glEnable(GL_BLEND);
		glFrontFace(GL_CW);
		glBlendFunc(GL_ONE, GL_ONE);
		glFogf(GL_FOG_START, 0);
		glFogf(GL_FOG_END, 4*sqrtf(m_fHorizon)*fScale);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, m_vAtmosphereColor*sqrtf(m_fRelativeDensity));

		m_rsSkyDome.Draw();

		glFrontFace(GL_CCW);
		glDisable(GL_BLEND);
		glDisable(GL_FOG);
	}

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, CVector4(0.0f));
	if(m_fAtmosphereDensity > DELTA && m_fAltitude >= m_fAtmosphereThickness*0.5f)
	{
		glPopMatrix();
		CDoubleVector n = pCamera->GetPosition() - GetPosition();
		double dDistance = n.Magnitude();
		n /= dDistance;

		C3DObject obj;
		obj.SetPosition(GetPosition() + n * (dDistance - sqrtf(m_fHorizon)));
		if(bImpostor)
			obj.GetBillboardMatrix(m, pCamera, m_mapHeight.GetRadius() * 1.25f);
		else
			obj.GetScaledBillboardMatrix(m, pCamera, m_mapHeight.GetRadius() * 1.25f, 0.1);
		glPushMatrix();
		glMultMatrixf(m);

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		CTexture::GetAtmosphereShell().Enable();
		float fTemp = 1-m_fRelativeDensity*4;
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, CVector4(m_vAtmosphereColor.x*fTemp, m_vAtmosphereColor.y*fTemp, m_vAtmosphereColor.z*fTemp, m_vAtmosphereColor.w*fTemp));
		DrawAtmosphere();
		CTexture::GetAtmosphereShell().Disable();
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}

	glDisable(GL_NORMALIZE);	// We have to enable this because we're using a scaled MODELVIEW matrix
	if(!bImpostor)
		glPopMatrix();
	glDisable(GL_LIGHT1);
}
