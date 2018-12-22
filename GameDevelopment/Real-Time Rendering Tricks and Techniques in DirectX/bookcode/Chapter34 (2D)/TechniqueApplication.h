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

class CTechniqueApplication : public CHostApplication  
{
public:
	void DrawSpriteEx(float X, float Y, float Z, float Angle, float XScale, float YScale);
	void DrawSprite();
	void ScaleSprite(float X, float Y);
	void RotateSprite(float Angle);
	void TranslateSprite(float X, float Y, float Z);
	HRESULT CreateSpriteBuffer();
	HRESULT SetupDevice();
	CTechniqueApplication();
	virtual ~CTechniqueApplication();

	virtual BOOL PostInitialize();
	virtual BOOL PreTerminate();
	virtual BOOL PostReset();
	virtual void Render();

	D3DXMATRIX m_PixelProjection;
	D3DXMATRIX m_ProportionalProjection;

	LPDIRECT3DVERTEXBUFFER8 m_pSpriteVertexBuffer;
	LPDIRECT3DTEXTURE8      m_pSpriteTexture;
	LPDIRECT3DTEXTURE8      m_pBackTexture;

	D3DXMATRIX m_SpriteTranslation;
	D3DXMATRIX m_SpriteRotation;
	D3DXMATRIX m_SpriteScaling;
};
