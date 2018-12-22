/***************************************************************
* TechniqueApplication.h                                       *
*                                                              *
* This file contains the header information for the class      *
* derived from CHostApplication.  This new class will          *
* serve as a base for all the techniques.  For this file to    *
* compile correctly, the path for the DX include files must be *
* added to your path.                                          *
*                                                              *
* Changes:                                                     *
*	*  None                                                    *
***************************************************************/
#include "Application.h"

#include "DirectXVideoTexture.h"

class CTechniqueApplication : public CHostApplication  
{
public:
	void DrawSpriteEx(float X, float Y, float Z, float Angle, float XScale, float YScale);
	HRESULT CreateSpriteBuffer();
	HRESULT SetupDevice();
	CTechniqueApplication();
	virtual ~CTechniqueApplication();

	virtual BOOL PostInitialize();
	virtual BOOL PreTerminate();
	virtual BOOL PostReset();
	virtual void Render();

	D3DXMATRIX m_PercentProjection;

	LPDIRECT3DVERTEXBUFFER8 m_pSpriteVertexBuffer;

	CDirectXVideoTexture m_VideoTexture;

	D3DXMATRIX m_SpriteTranslation;
	D3DXMATRIX m_SpriteRotation;
	D3DXMATRIX m_SpriteScaling;
};
