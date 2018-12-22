/***************************************************************
* BasicTextApplication.h                                       *
*                                                              *
* This file contains the header information for the class      *
* derived from CHostApplication.  This new class will          *
* demonstrate the basics of drawing text.  For this file to    *
* compile correctly, the path for the DX include files must be *
* added to your path.                                          *
*                                                              *
* Changes:                                                     *
*	*  None                                                    *
***************************************************************/
#include "Application.h"
#include "DirectXFont.h"

class CBasicTextApplication : public CHostApplication  
{
public:
	void DestroyTextObjects();
	void CreateTextObjects();
	CBasicTextApplication();
	virtual ~CBasicTextApplication();

	virtual BOOL PostInitialize();
	virtual BOOL PreTerminate();
	virtual BOOL PreReset();
	virtual BOOL PostReset();
	virtual void Render();

	//The font
	CDirectXFont m_Font;

	//A vertex buffer for a static text buffer
	LPDIRECT3DVERTEXBUFFER8 m_pStaticTextBuffer;
};
