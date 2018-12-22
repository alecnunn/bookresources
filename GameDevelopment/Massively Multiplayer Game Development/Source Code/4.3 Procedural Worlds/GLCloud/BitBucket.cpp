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
#include "GameApp.h"
#include "BitBucket.h"

void CCloudBlock::NoiseFill(int nSeed)
{
	CFractal fr(3, nSeed, 0.5f, 2.0f);
	srand(nSeed);
	int nIndex = 0;
	for(unsigned long z=0; z < nz; z++)
	{
		for(unsigned long y=0; y < ny; y++)
		{
			for(unsigned long x=0; x < nx; x++)
			{
				float f[3] = {(float)(x<<2)/nx, (float)(y<<2)/nx, (float)z/nz};
				float fNoise = Max(0, fr.fBm(f, 8.0f));
				m_pGrid[nIndex++].m_cDensity = (unsigned char)(fNoise * 64.999f);
			}
		}
	}
}

void CCloudBlock::Light(CVector &vLight2)
{
	CVector vLight = UnitInverse().RotateVector(vLight2);
	int nStart[3] = {0, 0, 0};
	int nInc[3] = {1, 1, 1};
	int nMax[3] = {nx, ny, nz};
	int nMap[3] = {0, 1, 2};

	for(int nIndex=0; nIndex<3; nIndex++)
	{
		if(vLight[nIndex] < 0)
		{
			nStart[nIndex] = nMax[nIndex] - 1;
			nInc[nIndex] = -1;
		}
	}

	if(Abs(vLight.x) >= Abs(vLight.y) && Abs(vLight.x) >= Abs(vLight.z))
	{
		nMap[0] = 0;
		if(Abs(vLight.y) >= Abs(vLight.z))
		{
			nMap[1] = 1;
			nMap[2] = 2;
		}
		else
		{
			nMap[1] = 2;
			nMap[2] = 1;
		}
	}
	else if(Abs(vLight.y) >= Abs(vLight.x) && Abs(vLight.y) >= Abs(vLight.z))
	{
		nMap[0] = 1;
		if(Abs(vLight.x) >= Abs(vLight.z))
		{
			nMap[1] = 0;
			nMap[2] = 2;
		}
		else
		{
			nMap[1] = 2;
			nMap[2] = 0;
		}
	}
	else if(Abs(vLight.z) >= Abs(vLight.x) && Abs(vLight.z) >= Abs(vLight.y))
	{
		nMap[0] = 2;
		if(Abs(vLight.x) >= Abs(vLight.y))
		{
			nMap[1] = 0;
			nMap[2] = 1;
		}
		else
		{
			nMap[1] = 1;
			nMap[2] = 0;
		}
	}

	CVector vDir = -vLight / Abs(vLight[nMap[0]]);

	// Cheesy sort
	int nLoop[3];
	nLoop[nMap[0]] = nStart[nMap[0]];
	for(int i=0; i<nMax[nMap[0]]; i++)
	{
		nLoop[nMap[1]] = nStart[nMap[1]];
		for(int j=0; j<nMax[nMap[1]]; j++)
		{
			nLoop[nMap[2]] = nStart[nMap[2]];
			for(int k=0; k<nMax[nMap[2]]; k++)
			{
				unsigned char c = 255;
				int nPrev = nLoop[nMap[0]] - nInc[nMap[0]];
				if(nPrev >= 0 && nPrev < nMax[nMap[0]])
				{
					int nCell[4][3];
					nCell[0][nMap[0]] = nPrev;
					nCell[1][nMap[0]] = nPrev;
					nCell[2][nMap[0]] = nPrev;
					nCell[3][nMap[0]] = nPrev;
					nCell[0][nMap[1]] = nLoop[nMap[1]];
					nCell[1][nMap[1]] = nLoop[nMap[1]]-nInc[nMap[1]];
					nCell[2][nMap[1]] = nLoop[nMap[1]]-nInc[nMap[1]];
					nCell[3][nMap[1]] = nLoop[nMap[1]];
					nCell[0][nMap[2]] = nLoop[nMap[2]];
					nCell[1][nMap[2]] = nLoop[nMap[2]];
					nCell[2][nMap[2]] = nLoop[nMap[2]]-nInc[nMap[2]];
					nCell[3][nMap[2]] = nLoop[nMap[2]]-nInc[nMap[2]];

					float fRatio[4];
					fRatio[0] = (1-Abs(vDir[nMap[1]])) * (1-Abs(vDir[nMap[2]]));
					fRatio[1] = Abs(vDir[nMap[1]]) * (1-Abs(vDir[nMap[2]]));
					fRatio[2] = Abs(vDir[nMap[1]]) * Abs(vDir[nMap[2]]);
					fRatio[3] = (1-Abs(vDir[nMap[1]])) * Abs(vDir[nMap[2]]);

					float f = 0;
					for(int n=0; n<4; n++)
					{
						fRatio[n] = Min(1.0f, Max(0.0f, fRatio[n]));
						int a = 255;
						CCloudCell *pCell = GetCloudCell(nCell[n][0], nCell[n][1], nCell[n][2]);
						if(pCell)
							a = pCell->m_cBrightness;
						f += a * fRatio[n];
					}

					c = (unsigned char)Min(255, Max(0, f));
				}
				CCloudCell *pCell = GetCloudCell(nLoop[0], nLoop[1], nLoop[2]);
				c = MAX(c, 64);
				pCell->m_cBrightness = (unsigned char)(c * (255-(pCell->m_cDensity*0.5f))/255.0f);

				nLoop[nMap[2]] += nInc[nMap[2]];
			}
			nLoop[nMap[1]] += nInc[nMap[1]];
		}
		nLoop[nMap[0]] += nInc[nMap[0]];
	}
}

void CCloudBlock::Draw(C3DObject *pCamera, float fHalfSize, bool bImpostor)
{
	CVector vNormal, vUp, vRight;
	if(bImpostor)
	{
		InitImpostorRender(pCamera);

		// For impostor rendering, point all cloud particle billboards toward camera
		vNormal = UnitInverse().RotateVector(pCamera->GetPosition() - (m_vPosition-m_vOffset));
		float fDistance = vNormal.Magnitude();
		vNormal /= fDistance;
		vRight = CVector(vNormal.y, -vNormal.z, vNormal.x) ^ vNormal;
		vRight.Normalize();
		vUp = vNormal ^ vRight;
		vUp.Normalize();
	}
	else
	{
		glPushMatrix();
		CMatrix m = GetModelMatrix(pCamera);
		glMultMatrixf(m);

		// For normal rendering, point all cloud particle billboards based on camera orientation
		vNormal = UnitInverse().RotateVector(-pCamera->GetViewAxis());
		vUp = UnitInverse().RotateVector(pCamera->GetUpAxis());
		vRight = UnitInverse().RotateVector(pCamera->GetRightAxis());
	}

	vUp *= fHalfSize;
	vRight *= fHalfSize;

	int nStart[3] = {0, 0, 0};
	int nInc[3] = {1, 1, 1};
	int nMax[3] = {nx, ny, nz};
	int nMap[3] = {0, 1, 2};

	for(int nIndex=0; nIndex<3; nIndex++)
	{
		if(vNormal[nIndex] < 0)
		{
			nStart[nIndex] = nMax[nIndex] - 1;
			nInc[nIndex] = -1;
		}
	}

	if(Abs(vNormal.x) >= Abs(vNormal.y) && Abs(vNormal.x) >= Abs(vNormal.z))
	{
		nMap[0] = 0;
		if(Abs(vNormal.y) >= Abs(vNormal.z))
		{
			nMap[1] = 1;
			nMap[2] = 2;
		}
		else
		{
			nMap[1] = 2;
			nMap[2] = 1;
		}
	}
	else if(Abs(vNormal.y) >= Abs(vNormal.x) && Abs(vNormal.y) >= Abs(vNormal.z))
	{
		nMap[0] = 1;
		if(Abs(vNormal.x) >= Abs(vNormal.z))
		{
			nMap[1] = 0;
			nMap[2] = 2;
		}
		else
		{
			nMap[1] = 2;
			nMap[2] = 0;
		}
	}
	else if(Abs(vNormal.z) >= Abs(vNormal.x) && Abs(vNormal.z) >= Abs(vNormal.y))
	{
		nMap[0] = 2;
		if(Abs(vNormal.x) >= Abs(vNormal.y))
		{
			nMap[1] = 0;
			nMap[2] = 1;
		}
		else
		{
			nMap[1] = 1;
			nMap[2] = 0;
		}
	}

	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_BLEND);
	CTexture::GetCloudCell().Enable();
	glBegin(GL_QUADS);

	// Cheesy sorted render (don't really sort, just draw from back corner to front corner)
	float fOffset[3] = {nx*0.5f+0.5f, ny*0.5f+0.5f, nz*0.5f+0.5f};
	int n[3];
	n[nMap[0]] = nStart[nMap[0]];
	for(int i=0; i<nMax[nMap[0]]; i++)
	{
		n[nMap[1]] = nStart[nMap[1]];
		for(int j=0; j<nMax[nMap[1]]; j++)
		{
			n[nMap[2]] = nStart[nMap[2]];
			for(int k=0; k<nMax[nMap[2]]; k++)
			{
				CCloudCell *pCell = GetCloudCell(n[0], n[1], n[2]);
				if(pCell->m_cDensity > 4)
				{
					CVector v(n[0]-fOffset[0], n[1]-fOffset[1], n[2]-fOffset[2]);

					unsigned char a = pCell->m_cDensity*2;
					unsigned char c = (unsigned char)(pCell->m_cBrightness * (a/256.0f));
					glColor4ub(c, c, c, a);

					//glColor4ub(pCell->m_cBrightness, pCell->m_cBrightness, pCell->m_cBrightness, (unsigned char)(sqrtf(pCell->m_cDensity / 128.0f) * 256.0f));
					//glColor4ub(pCell->m_cBrightness, pCell->m_cBrightness, pCell->m_cBrightness, pCell->m_cDensity*2);
					glTexCoord2d(0.0, 0.0);
					glVertex3fv(v-vRight+vUp);
					glTexCoord2d(0.0, 1.0);
					glVertex3fv(v-vRight-vUp);
					glTexCoord2d(1.0, 1.0);
					glVertex3fv(v+vRight-vUp);
					glTexCoord2d(1.0, 0.0);
					glVertex3fv(v+vRight+vUp);
				}
				n[nMap[2]] += nInc[nMap[2]];
			}
			n[nMap[1]] += nInc[nMap[1]];
		}
		n[nMap[0]] += nInc[nMap[0]];
	}

	glEnd();
	CTexture::GetCloudCell().Disable();
	glDisable(GL_BLEND);
	glColor4f(1, 1, 1, 1);

	if(bImpostor)
		FinishImpostorRender();
	else
		glPopMatrix();
}
