/***************************************************************
* VertexApplication.h                                          *
*                                                              *
* This file contains the header information for the class      *
* derived from CHostApplication.  This new class will          *
* demonstrate the basics of vertex buffers.  For this file to  *
* compile correctly, the path for the DX include files must be *
* added to your path.                                          *
*                                                              *
* Changes:                                                     *
*	*  None                                                    *
***************************************************************/
#include "Application.h"

class CVertexApplication : public CHostApplication  
{
public:
	BOOL FillVertexBuffer();
	BOOL CreateVertexBuffer();
	void DestroyVertexBuffer();
	CVertexApplication();
	virtual ~CVertexApplication();

	//This function will be called after the D3D object is 
	//created.  Here we'll actually create the vertex buffer.
	virtual BOOL PostInitialize();

	//This function will clean up the vertex buffer prior to 
	//the destruction of the device
	virtual BOOL PreTerminate();

	//These functions will ensure that lost vertex buffers are
	//recreated once the device is reset
	virtual BOOL PreReset();
	virtual BOOL PostReset();

	//This function will actually render the data.
	virtual void Render();

	//This is the pointer to our vertex buffer
	LPDIRECT3DVERTEXBUFFER8 m_pVertexBuffer;
};
