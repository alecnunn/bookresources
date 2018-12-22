#ifndef _ATMOSPHERE_H
#define _ATMOSPHERE_H

#include <stdio.h>
#include <d3dutil.h>
 
// Various Atmosphere parameters.
typedef enum {
	eAtmBetaRayMultiplier,
	eAtmBetaMieMultiplier,
	eAtmInscatteringMultiplier,
	eAtmExtinctionMultiplier,
	eAtmHGg,
} AtmosphereParams;


class Atmosphere {
	
public:
	Atmosphere(); 
	~Atmosphere() {};

	void SetParam(AtmosphereParams param, float fValue);
	float GetParam(AtmosphereParams param);
	
	D3DXVECTOR3 GetBetaRayleigh() {return m_vBetaRay;};
	D3DXVECTOR3 GetBetaDashRayleigh() {return m_vBetaDashRay;};

	D3DXVECTOR3 GetBetaMie() {return m_vBetaMie;};
	D3DXVECTOR3 GetBetaDashMie() {return m_vBetaDashMie;};
	
	void Interpolate(Atmosphere *one, Atmosphere *two, float f);

	void Dump2(FILE *f);
	void Read2(FILE *f);
	void Finalize() {};

	Atmosphere operator + ( const Atmosphere&) const;
    Atmosphere operator * ( float ) const;
    friend Atmosphere operator * ( float , const Atmosphere& );




private:
	float m_fHGg;		// g value in Henyey Greenstein approximation function.

	// Ideally, the following multipliers should all be 1.0, but provided here for study.
	// The final color of an object in atmosphere is sum of inscattering term and extinction term. 
	float m_fInscatteringMultiplier;	// Multiply inscattering term with this factor.
	float m_fExtinctionMultiplier;		// Multiply extinction term with this factor.
	float m_fBetaRayMultiplier;			// Multiply  Rayleigh scattering coefficient with this factor
	float m_fBetaMieMultiplier;			// Multiply  Mie scattering coefficient with this factor

	D3DXVECTOR3 m_vBetaRay;				// Rayleigh scattering coeff
	D3DXVECTOR3 m_vBetaDashRay;			// Rayleigh Angular scattering coeff without phase term.
	D3DXVECTOR3 m_vBetaMie;				// Mie scattering coeff
	D3DXVECTOR3 m_vBetaDashMie;			// Mie Angular scattering coeff without phase term.

	void CalculateScatteringConstants();


};

Atmosphere operator * ( float , const Atmosphere& );

#endif