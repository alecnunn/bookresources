#ifndef _SUN_H
#define _SUN_H

#include <stdio.h>

#include <d3dutil.h>

class Sun {
	
public:
	Sun(float fTheta = 0, float fPhi = 0,
		float fIntensity = 1.0); 
								// Theta is angle from zenith direction (Y axis)
								// Theta = 0 (Pos y), Theta = 180 (Neg Y)
								// Phi is angle in CW from x axis in XZ coordinate system.
								// eg. E(pos X) = 0, N(pos Z) = 90, W = 180, S = 270.
								

	~Sun() {};

				
	D3DXVECTOR4 GetDirection();
	float GetIntensity();
	D3DXVECTOR4 GetColor();
	D3DXVECTOR4 GetColorAndIntensity();
	D3DXVECTOR4 GetColorWithIntensity();

	void SetSunIntensity(float fIntensity) {m_fIntensity = fIntensity;};
	void SetSunThetaPhi(float fTheta, float fPhi) {m_fTheta = fTheta; m_fPhi = fPhi;} ;
	float GetSunTheta() {return m_fTheta;};
	float GetSunPhi() {return m_fPhi;};

	void Interpolate(Sun *one, Sun *two, float f);

	void Dump2(FILE *f);
	void Read2(FILE *f);
	void Finalize() {};

	Sun operator + ( const Sun&) const;
    Sun operator * ( float ) const;
    friend Sun operator * ( float , const Sun& );


private:
	float m_fTheta, m_fPhi;
	float m_fIntensity;


	D3DXVECTOR4 ComputeAttenuation(float fTheta, int nTurbidity = 2);

};


#endif
