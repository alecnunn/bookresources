/***************************************************************
* TransformApplication.h                                       *
*                                                              *
* This file contains the header information for the class      *
* derived from CHostApplication.  This new class will          *
* demonstrate the basics of transformations.  For this file to *
* compile correctly, the path for the DX include files must be *
* added to your path.                                          *
*                                                              *
* Changes:                                                     *
*	*  None                                                    *
***************************************************************/
#include "Application.h"

class CTransformApplication : public CHostApplication  
{
public:
	void InitializeViewports();

	CTransformApplication();
	virtual ~CTransformApplication();

	BOOL FillVertexBuffer();
	BOOL CreateVertexBuffer();
	void DestroyVertexBuffer();

	virtual BOOL PostInitialize();
	virtual BOOL PreTerminate();
	virtual BOOL PreReset();
	virtual BOOL PostReset();
	virtual void Render();
	virtual BOOL HandleMessage(MSG *pMessage);

	LPDIRECT3DVERTEXBUFFER8 m_pVertexBuffer;

	//These matrices are reusable transformation matrices
	D3DXMATRIX m_WorldMatrix;
	D3DXMATRIX m_ViewMatrix;
	D3DXMATRIX m_ProjectionMatrix;

	//Create a set of viewports to render the different views
	//We have viewports for rotate, scale-rotate-translate,
	//translate-rotate, and rotate-translate-rotate-scale.
	D3DVIEWPORT8  m_RViewport;
	D3DVIEWPORT8  m_SRTViewport;
	D3DVIEWPORT8  m_TRViewport;
	D3DVIEWPORT8  m_RTRSViewport;
};
