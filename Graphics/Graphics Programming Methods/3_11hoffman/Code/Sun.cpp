#include "Sun.h"

const float pi = 3.14159265358f;


Sun::Sun(float fTheta, float fPhi, float fIntensity)
{
	m_fTheta = fTheta;
	m_fPhi = fPhi;
	m_fIntensity = fIntensity;
}

D3DXVECTOR4 Sun::GetDirection()
{
	// Note: in our world, zenith direction is y axis,
	// z is into the screen. Thanks to MS.

	float y = cos(m_fTheta);
	float x = sin(m_fTheta) * cos(m_fPhi);
	float z = sin(m_fTheta) * sin(m_fPhi);

	/* this one when using normal map as the up direction is in z coordinate.
	float z = cos(m_fTheta);
	float x = sin(m_fTheta) * cos(m_fPhi);
	float y = sin(m_fTheta) * sin(m_fPhi); */
	float w = 1.0f;
	return D3DXVECTOR4(x,y,z,w);
}


// This function taken from source available on 99 sig paper web page.
// http://www.cs.utah.edu/vissim/papers/sunsky/code/RiSunConstants.C

D3DXVECTOR4 Sun::ComputeAttenuation(float fTheta, int nTurbidity/* = 2*/)
// fTheta is in radians.
// nTurbidity >= 2
{
    float fBeta = 0.04608365822050f * nTurbidity - 0.04586025928522f;
    float fTauR, fTauA, fTau[3];
    float m = 1.0/(cos(fTheta) + 0.15f*pow(93.885f-fTheta/pi*180.0f,-1.253f));  // Relative Optical Mass

    int i;
    float fLambda[3]; 
	fLambda[0] = 0.65f;	// red (in um.)
	fLambda[1] = 0.57f;	// green (in um.)
	fLambda[2] = 0.475f;	// blue (in um.)


    for(i = 0; i < 3; i++)
	{
		// Rayleigh Scattering
		// Results agree with the graph (pg 115, MI) */
		// lambda in um.
		fTauR = exp( -m * 0.008735f * pow(fLambda[i], float(-4.08f)));

		// Aerosal (water + dust) attenuation
		// beta - amount of aerosols present 
		// alpha - ratio of small to large particle sizes. (0:4,usually 1.3)
		// Results agree with the graph (pg 121, MI) 
		const float fAlpha = 1.3f;
		fTauA = exp(-m * fBeta * pow(fLambda[i], -fAlpha));  // lambda should be in um


		fTau[i] = fTauR * fTauA; 

    }

	D3DXVECTOR4 vAttenuation = D3DXVECTOR4(fTau[0], fTau[1], fTau[2], 1);
	return vAttenuation;
}

float Sun::GetIntensity()
{
	return m_fIntensity;
}

D3DXVECTOR4 Sun::GetColor()
{
	// Note: Sun color changes with the sun position.
	return ComputeAttenuation(m_fTheta);

}

D3DXVECTOR4 Sun::GetColorAndIntensity()
{
	D3DXVECTOR4 result = GetColor();
	result[3] = GetIntensity();
	return result;
}

D3DXVECTOR4 Sun::GetColorWithIntensity()
{
	D3DXVECTOR4 result = GetColor() * GetIntensity();
	return result;
}

void Sun::Interpolate(Sun *one, Sun *two, float f)
{
	(*this) = *one;
	m_fIntensity = one->m_fIntensity * (1-f) + two->m_fIntensity * f;

	// Do better job of interpolating direction.
	D3DXVECTOR4 dir = one->GetDirection() * (1-f) + two->GetDirection() * f;
	D3DXVECTOR3 dirn, dir3(dir[0],dir[1],dir[2]);
	D3DXVec3Normalize(&dirn, &dir3);
	float horiz = sqrt(dirn[0]*dirn[0] + dirn[2]*dirn[2]);
	m_fTheta = pi/2-atan2(dirn[1], horiz);
	m_fPhi = atan2(dirn[2],dirn[0]);
	
}

void Sun::Dump2(FILE *f)
{
	fprintf(f, "%f ", m_fIntensity);
	fprintf(f, "%f %f ", m_fTheta,m_fPhi);
}

void Sun::Read2(FILE *f)
{
	fscanf(f, "%f ", &m_fIntensity);
	fscanf(f, "%f %f ", &m_fTheta, &m_fPhi);
}


Sun Sun::operator + ( const Sun& s2) const
{
	Sun res;
	res.m_fIntensity = m_fIntensity + s2.m_fIntensity;
	res.m_fTheta = m_fTheta + s2.m_fTheta;
	res.m_fPhi = m_fPhi + s2.m_fPhi;
	return res;
}

Sun Sun::operator * ( float f) const
{
	Sun res;
	res.m_fIntensity = m_fIntensity * f;
	res.m_fTheta = m_fTheta * f;
	res.m_fPhi = m_fPhi * f;
	return res;
}

Sun operator * ( float f, const Sun& s )
{
	Sun res;
	res.m_fIntensity = s.m_fIntensity * f;
	res.m_fTheta = s.m_fTheta * f;
	res.m_fPhi = s.m_fPhi * f;
	return res;
}

