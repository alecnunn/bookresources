#ifndef _SKY_H
#define _SKY_H


#include <d3d8.h>
#include <d3dutil.h>

class Shader;

class SkyVertex {
public: 
	D3DXVECTOR3 p;
	float u, v;
};

class Sky {
public:
	Sky(D3DXVECTOR3& vCenter, float fRadiusHoriz, float fRadiusUp);
	~Sky();


    HRESULT Render(); 

private:
	// A hemisphere with radius of fRadius, centered at Center.
	// Note: Center is usually the camera eye point to begin with.
	// todo: Can be updated to move with eye
	float m_fRadiusHoriz;
	float m_fRadiusUp;
	int m_nThetaSides;
	int m_nPhiSides;			// Phi is angle from zenith, theta is azimuth angle.
	D3DXVECTOR3 m_vCenter;

	LPDIRECT3DVERTEXBUFFER8 m_pVB; 

	Shader *m_pShader1;

private:
	HRESULT CreateVertexBuffer();
	HRESULT FillupVertexBuffer();


};

#endif
