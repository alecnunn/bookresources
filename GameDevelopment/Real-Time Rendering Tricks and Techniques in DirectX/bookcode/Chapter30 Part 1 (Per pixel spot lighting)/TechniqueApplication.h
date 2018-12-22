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
	virtual void PreRender();
	HRESULT SetupEnvironment();
	HRESULT CreateShaders();
	HRESULT ExtractBuffers();
	void SetupDevice();
	CTechniqueApplication();
	virtual ~CTechniqueApplication();

	virtual BOOL PostInitialize();
	virtual BOOL PreTerminate();
	virtual BOOL PreReset();
	virtual BOOL PostReset();
	virtual void Render();

	LPDIRECT3DVERTEXBUFFER8 m_pBoxVertexBuffer;
	LPDIRECT3DINDEXBUFFER8  m_pBoxIndexBuffer;

	LPDIRECT3DTEXTURE8 m_pSpotLightTexture;

	DWORD m_ProjectionShader;
	DWORD m_AttenuationShader;
};
