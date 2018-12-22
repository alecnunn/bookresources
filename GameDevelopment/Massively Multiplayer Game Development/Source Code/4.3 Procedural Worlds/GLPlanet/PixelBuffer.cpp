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
#include "PixelBuffer.h"

/*
	COMPUTING CRATER DIAMETERS FROM EARTH-IMPACTING ASTEROIDS

	Astrogeologist Gene Shoemaker proposes the following formula, based on
	studies of cratering caused by nuclear tests. Units are MKS unless
	otherwise noted; impact energy is sometimes expressed in nuclear bomb
	terms (kilotons TNT equivalent) due to the origin of the model.

	D = Sg Sp Kn W^(1/3.4)
	Crater diameter, meters. On Earth, if D > 3 km, the crater is
	assumed to collapse by a factor of 1.3 due to gravity.

	Sg = (ge/gt)^(1/6)
	Gravity correction factor cited for craters on the Moon. May hold
	true for other bodies. ge = 9.8 m/s^2 is Earth gravity, gt is
	gravity of the target body.

	Sp = (pa/pt)^(1/3.4)
	Density correction factor for target material relative to the Jangle
	U nuclear crater site. pa = 1.8e3 kg/m^3 (1.8 gm/cm^3) for alluvium,
	pt = density at the impact site. For reference, average rock on the
	continental shields has a density of 2.6e3 kg/m^3 (2.6 gm/cm^3).

	Kn = 74 m / (kiloton TNT equivalent)^(1/3.4)
	Empirically determined scaling factor from bomb yield to crater
	diameter at Jangle U.

	W = Ke / (4.185e12 joules/KT)
	Kinetic energy of asteroid, kilotons TNT equivalent.

	Ke = 1/2 m v^2
	Kinetic energy of asteroid, joules.

	v = impact velocity of asteroid, m/s.
	2e4 m/s (20 km/s) is common for an asteroid in an Earth-crossing
	orbit.

	m = 4/3 pi r^3 rho
	Mass of asteroid, kg.

	r = radius of asteroid, m

	rho = density of asteroid, kg/m^3
	3.3e3 kg/m^3 (3 gm/cm^3) is reasonable for a common S-type asteroid.

	For an example, let's work the body which created the 1.1 km diameter
	Barringer Meteor Crater in Arizona (in reality the model was run
	backwards from the known crater size to estimate the meteor size, but
	this is just to show how the math works):

	r = 40 m	    Meteor radius
	rho = 7.8e3 kg/m^3  Density of nickel-iron meteor
	v = 2e4 m/s	    Impact velocity characteristic of asteroids in Earth-crossing orbits
	pt = 2.3e3 kg/m^3   Density of Arizona at impact site

	Sg = 1		    No correction for impact on Earth
	Sp = (1.8/2.3)^(1/3.4) = .93
	m = 4/3 pi 40^3 7.8e3 = 2.61e8 kg
	Ke = 1/2 * 2.61e8 kg * (2e4 m/s)^2
	   = 5.22e16 joules
	W = 5.22e16 / 4.185e12 = 12,470 KT
	D = 1 * .93 * 74 * 12470^(1/3.4) = 1100 meters

	More generally, one can use (after Gehrels, 1985):

	Asteroid	    Number of Impact probability  Impact energy as multiple
	diameter (km)   Objects    (impacts/year)	    of Hiroshima bomb
	-------------   --------- ------------------  -------------------------
	 10				10		  10e-8				  1e9 (1 billion)
	  1			    1e3		  10e-6				  1e6 (1 million)
	  0.1		    1e5		  10e-4				  1e3 (1 thousand)

	The Hiroshima explosion is assumed to be 13 kilotons.

	Finally, a back of the envelope rule is that an object moving at a speed
	of 3 km/s has kinetic energy equal to the explosive energy of an equal
	mass of TNT; thus a 10 ton asteroid moving at 30 km/sec would have an
	impact energy of (10 ton) (30 km/sec / 3 km/sec)^2 = 1 KT.

	Impact craters are divided into two groups based on morphology:
	simple craters and complex craters. Simple craters are relatively
	small with depth-to-diameter ratios of about 1:5 to 1:7 and a
	smooth bowl shape. In larger craters, however, gravity causes the
	initially steep crater walls to collapse downward and inward, forming
	a complex structure with a central peak or peak ring and a shallower
	depth compared to diameter (1:10 to 1:20). The diameter at which
	craters become complex depends on the surface gravity of the planet:
	The greater the gravity, the smaller the diameter that will produce
	a complex structure. On Earth, this transition diameter is 2 to 4
	kilometers (1.2 to 2.5 miles) depending on target rock properties;
	on the Moon, at one-sixth Earth's gravity, the transition diameter
	is 15 to 20 kilometers (9 to 12 miles).

	(For objects with diameter >100 meters, as in the Popigai and Chesapeake
	Bay impactors, the Earth’s atmosphere does not have almost any effect on
	their mass or velocity.)


	Notes: A power function going from 0 to 1 should do the trick
	inside the bowl, with depth, wall height, and radius all being
	determined first. From the rim outward, it could be a graduated
	blend between rim height and actual ground height from 1x to 1.1x
	the radius. The power should get lower as the radius gets larger,
	making the walls less high and steep (proportionally). Gravity
	will determine how quickly the power drops, as well as how soon
	a central peak will form. Perhaps the outer peaks start at 50% up
	the amount the center goes down, but gravity may pull the walls
	lower for larger craters.

*/

void CPixelBuffer::MakeRandom(unsigned int nSeed)
{
	// Instead of calling Random() to fill a byte every pixel,
	// call it every 4 pixels to fill a dword
	srand(nSeed);
	int nSize = (m_nWidth * m_nHeight * m_nChannels) >> 1;
	unsigned short *pDest = (unsigned short *)m_pBuffer;
	for(int i=0; i<nSize; i++)
		*pDest++ = rand();
}

void CPixelBuffer::MakeNoise(unsigned long nSeed)
{
	const unsigned char nMin = 224, nMax = 255;
	const float fHalf = (nMax + 1 - nMin) / 2;

	CFractal fractal(2, nSeed, 0.5f, 2.0f);
	int nHalfWidth = m_nWidth >> 1, nHalfHeight = m_nHeight >> 1;
	float f[2];
	unsigned char *pBuffer = (unsigned char *)GetBuffer();
	for(int y=0; y<m_nHeight; y++)
	{
		for(int x=0; x<m_nWidth; x++)
		{
			if(x < nHalfWidth+1 && y < nHalfHeight+1)
			{
				f[0] = 32.0f * (float)x / (float)m_nWidth;
				f[1] = 32.0f * (float)y / (float)m_nHeight;
				float fValue = fractal.fBm(f, 4.0f);
				unsigned char nValue = (unsigned char)Max(nMin, Min(nMax, ((fValue+1.0f) * fHalf + nMin)));
				*pBuffer++ = nValue;
			}
			else
			{
				int xTemp = x, yTemp = y;
				if(xTemp > nHalfWidth)
					xTemp = m_nWidth - xTemp;
				if(yTemp > nHalfHeight)
					yTemp = m_nHeight - yTemp;
				*pBuffer++ = *(unsigned char *)operator()(xTemp, yTemp);
			}
		}
	}
}

void CPixelBuffer::MakeGlow(float fExpose, float fSizeDisc)
{
	int i;
	int n = 0;
	unsigned char nIntensity;
	for(int y=0; y<m_nHeight; y++)
	{
		float fDy = (float)y/(m_nHeight-1) - 0.5f;
		for(int x=0; x<m_nWidth; x++)
		{
			float fDx = (float)x/(m_nWidth-1) - 0.5f;
			float fDist = sqrtf(fDx*fDx + fDy*fDy);
			float fIntensity = expf(-Max(fDist-fSizeDisc,0)*fExpose);
			switch(m_nDataType)
			{
				case GL_UNSIGNED_BYTE:
					nIntensity = (unsigned char)(fIntensity*255 + 0.5f);
					for(i=0; i<m_nChannels; i++)
						((unsigned char *)m_pBuffer)[n++] = nIntensity;
					break;
				case GL_FLOAT:
					for(i=0; i<m_nChannels; i++)
						((float *)m_pBuffer)[n++] = fIntensity;
					break;
			}
		}
	}
}

#ifdef ATMOSPHERE_2D
void CPixelBuffer::MakeAtmosphere(float fExpose)
{
	int i;
	int n = 0;
	unsigned char nIntensity;
	for(int y=0; y<m_nHeight; y++)
	{
		float fDy = /*(y == 0 || y == m_nHeight-1) ? 0 : */(float)y/(m_nHeight-1) - 0.5f;
		for(int x=0; x<m_nWidth; x++)
		{
			float fDx = /*(x == 0 || x == m_nWidth-1) ? 0 : */(float)x/(m_nWidth-1) - 0.5f;
			float fDist = sqrtf(fDx*fDx + fDy*fDy);
			float fIntensity = expf(-Max(Abs(0.4f - fDist),0)*fExpose);
			if(fDist < 0.4f)
				fIntensity = 0.5f + fIntensity*0.5f;
			switch(m_nDataType)
			{
				case GL_UNSIGNED_BYTE:
					nIntensity = (unsigned char)(fIntensity*255 + 0.5f);
					for(i=0; i<m_nChannels; i++)
						((unsigned char *)m_pBuffer)[n++] = nIntensity;
					break;
				case GL_FLOAT:
					for(i=0; i<m_nChannels; i++)
						((float *)m_pBuffer)[n++] = fIntensity;
					break;
			}
		}
	}
}
#else
void CPixelBuffer::MakeAtmosphere(float fExpose)
{
	int i;
	int n = 0;
	unsigned char nIntensity;
	for(int x=0; x<m_nWidth; x++)
	{
		float fDx = /*(x == 0 || x == m_nWidth-1) ? 0 : */(float)x/(m_nWidth-1);
		float fDist = sqrtf(fDx*fDx);
		float fIntensity = expf(-Max(Abs(0.8f - fDist),0)*fExpose);
		if(fDist < 0.8f)
			fIntensity = 0.5f + fIntensity*0.5f;
		switch(m_nDataType)
		{
			case GL_UNSIGNED_BYTE:
				nIntensity = (unsigned char)(fIntensity*255 + 0.5f);
				for(i=0; i<m_nChannels; i++)
					((unsigned char *)m_pBuffer)[n++] = nIntensity;
				break;
			case GL_FLOAT:
				for(i=0; i<m_nChannels; i++)
					((float *)m_pBuffer)[n++] = fIntensity;
				break;
		}
	}
}
#endif

void CPixelBuffer::MakeCloudCell(float fExpose, float fSizeDisc)
{
	int i;
	int n = 0;
	unsigned char nIntensity;
	for(int y=0; y<m_nHeight; y++)
	{
		float fDy = (y+0.5f)/m_nHeight - 0.5f;
		for(int x=0; x<m_nWidth; x++)
		{
			float fDx = (x+0.5f)/m_nWidth - 0.5f;
			float fDist = sqrtf(fDx*fDx + fDy*fDy);
			float fIntensity = 2 - Min(2, powf(2.0f, Max(fDist-fSizeDisc,0)*fExpose));
			switch(m_nDataType)
			{
				case GL_UNSIGNED_BYTE:
					nIntensity = (unsigned char)(fIntensity*255 + 0.5f);
					for(i=0; i<m_nChannels; i++)
						((unsigned char *)m_pBuffer)[n++] = nIntensity;
					break;
				case GL_FLOAT:
					for(i=0; i<m_nChannels; i++)
						((float *)m_pBuffer)[n++] = fIntensity;
					break;
			}
		}
	}
}
