#include "Atmosphere.h"


Atmosphere::Atmosphere()
{	
	m_fInscatteringMultiplier = 1.0;
	m_fExtinctionMultiplier = 1.0;
	m_fHGg = 0.8f;

	m_fBetaRayMultiplier = 1.0;
	m_fBetaMieMultiplier = 1.0;

	CalculateScatteringConstants();
}

	
void Atmosphere::CalculateScatteringConstants()
{
	// Reference: my thesis - appendices

	const float pi = 3.14159265358f;
	const float n = 1.0003f; // refractive index
	const float N = 2.545e25;
	const float pn = 0.035f;

	float fLambda[3],fLambda2[3], fLambda4[3];
	fLambda[0]  = 1/650e-9f; // red // note 650e-9 m = 650nm.
	fLambda[1]  = 1/570e-9f;	// green
	fLambda[2]  = 1/475e-9f; // blue
	for (int i=0; i < 3; i++)
	{
		fLambda2[i] = fLambda[i]*fLambda[i];
		fLambda4[i] = fLambda2[i]*fLambda2[i];
	}
	D3DXVECTOR3 vLambda2(fLambda2[0], fLambda2[1], fLambda2[2]); 
	D3DXVECTOR3 vLambda4(fLambda4[0], fLambda4[1], fLambda4[2]); 

	// Rayleigh scattering constants.
	float fTemp = pi*pi*(n*n-1)*(n*n-1)*(6+3*pn)/(6-7*pn)/N;
	float fBeta = 8*fTemp*pi/3;
	m_vBetaRay = fBeta * vLambda4;

	float fBetaDash = fTemp/2;
	m_vBetaDashRay = fBetaDash * vLambda4;

	// Mie scattering constants.
	const float T = 2.0f;
	float c = (6.544*T - 6.51)*1e-17; // from page 57 of my thesis.
	float fTemp2 = 0.434*c*(2*pi)*(2*pi)*0.5f;
	m_vBetaDashMie = fTemp2 * vLambda2;

	float K[3] = {0.685f, 0.679f, 0.670f}; // from pg 64 of my thesis.
	float fTemp3 = 0.434f*c*pi*(2*pi)*(2*pi);
	D3DXVECTOR3 vBetaMieTemp(K[0]*fLambda2[0],K[1]*fLambda2[1], K[2]*fLambda2[2]);
	m_vBetaMie = fTemp3*vBetaMieTemp;

}


void Atmosphere::SetParam(AtmosphereParams eParam, float fValue)
{
	switch (eParam) {
	case eAtmBetaRayMultiplier:
		m_fBetaRayMultiplier = fValue;
		break;
	case eAtmBetaMieMultiplier:
		m_fBetaMieMultiplier = fValue;
		break;
	case eAtmInscatteringMultiplier:
		m_fInscatteringMultiplier = fValue;
		break;
	case eAtmExtinctionMultiplier:
		m_fExtinctionMultiplier = fValue;
		break;
	case eAtmHGg:
		m_fHGg = fValue;
		break;
	}
	return;

}


float Atmosphere::GetParam(AtmosphereParams eParam)
{
	switch (eParam) {
	case eAtmBetaRayMultiplier:
		return m_fBetaRayMultiplier;
	case eAtmBetaMieMultiplier:
		return m_fBetaMieMultiplier;;
	case eAtmInscatteringMultiplier:
		return m_fInscatteringMultiplier;
	case eAtmExtinctionMultiplier:
		return m_fExtinctionMultiplier;
	case eAtmHGg:
		return m_fHGg;
		
	}
	
	return 0;

}

void Atmosphere::Interpolate(Atmosphere *one, Atmosphere *two, float f)
{

	m_fBetaRayMultiplier = one->m_fBetaRayMultiplier * (1-f) + two->m_fBetaRayMultiplier * f;
	m_fBetaMieMultiplier = one->m_fBetaMieMultiplier * (1-f) + two->m_fBetaMieMultiplier * f;
	m_fInscatteringMultiplier = one->m_fInscatteringMultiplier * (1-f) + two->m_fInscatteringMultiplier * f;
	m_fExtinctionMultiplier = two->m_fExtinctionMultiplier; //Do not interpolate, its either 0 or 1.
	m_fHGg = one->m_fHGg * (1-f) + two->m_fHGg *f;
	
}

void Atmosphere::Dump2(FILE *f)
{

	fprintf(f, "%f ", m_fBetaRayMultiplier);
	fprintf(f, "%f ", m_fBetaMieMultiplier);
	fprintf(f, "%f ", m_fInscatteringMultiplier);
	fprintf(f, "%f ", m_fExtinctionMultiplier);
	fprintf(f, "%f ", m_fHGg);

}

void Atmosphere::Read2(FILE *f)
{
	fscanf(f, "%f ", &m_fBetaRayMultiplier);
	fscanf(f, "%f ", &m_fBetaMieMultiplier);
	fscanf(f, "%f ", &m_fInscatteringMultiplier);
	fscanf(f, "%f ", &m_fExtinctionMultiplier);
	fscanf(f, "%f ", &m_fHGg);
}



Atmosphere Atmosphere::operator + ( const Atmosphere&atm2) const
{
	Atmosphere res;
	res.m_fBetaRayMultiplier = m_fBetaRayMultiplier + atm2.m_fBetaRayMultiplier;
	res.m_fBetaMieMultiplier = m_fBetaMieMultiplier + atm2.m_fBetaMieMultiplier;
	res.m_fInscatteringMultiplier = m_fInscatteringMultiplier + atm2.m_fInscatteringMultiplier;
	res.m_fExtinctionMultiplier = m_fExtinctionMultiplier + atm2.m_fExtinctionMultiplier;
	res.m_fHGg = m_fHGg + atm2.m_fHGg;
	return res;
}

Atmosphere Atmosphere::operator * ( float f) const
{
	Atmosphere res;
	res.m_fBetaRayMultiplier = m_fBetaRayMultiplier * f;
	res.m_fBetaMieMultiplier = m_fBetaMieMultiplier * f;
	res.m_fInscatteringMultiplier = m_fInscatteringMultiplier * f;
	res.m_fExtinctionMultiplier = m_fExtinctionMultiplier * f;
	res.m_fHGg = m_fHGg * f;
	return res;
}

Atmosphere operator * ( float f, const Atmosphere&a )
{
	Atmosphere res;
	res.m_fBetaRayMultiplier = a.m_fBetaRayMultiplier * f;
	res.m_fBetaMieMultiplier = a.m_fBetaMieMultiplier * f;
	res.m_fInscatteringMultiplier = a.m_fInscatteringMultiplier * f;
	res.m_fExtinctionMultiplier = a.m_fExtinctionMultiplier * f;
	res.m_fHGg = a.m_fHGg * f;
	return res;
}
