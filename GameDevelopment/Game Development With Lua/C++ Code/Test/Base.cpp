// Base.cpp: implementation of the CBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <WinConsole.h>
#include "Base.h"

// global pointer to the one and only CBase object
// defined in WinMain.cpp
extern CBase *g_pBase;

// static function just to keep the DX timekeeping and other message handling 
// functions hidden in the DX9 library. 
static void	RenderStub(float fTime)
{
	if(g_pBase)
		g_pBase->Render(fTime);
}


// testing stuff

Sprite *g_pSprite = NULL;
Font   *g_pd3dxFont  = NULL;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBase::CBase()
{
	m_lpDX = NULL;
}

bool CBase::Init(HINSTANCE hInstance, const char *szClass, const char *szCaption, WNDPROC WindowProc)
{
    // create application handler and link to our WindowProc

    m_lpDX=new DXContext(hInstance,szClass,szCaption,WindowProc);
    if (!m_lpDX)
        return false;;

    // check for error

    if (m_lpDX->lastErr!=S_OK) {
        delete m_lpDX;
        return false;
    }

    // initialize full screen graphics to 640 x 480 x 32 bit, with 2 back buffers

    if (!m_lpDX->Init(640,480,16,2,TRUE)) {
        delete m_lpDX;
        return false;
    }

	m_pGUIManager = new CGUIManager;
	m_pGUIManager->Init(&m_lua);

	CWinConsole::StartConsole(hInstance, &m_lua);

#if 0
	// testing stuff here...
	g_pSprite = new Sprite();
	g_pSprite->SetTexture("test.bmp");
	g_pSprite->SetPosition(0, 0, 0);
	g_pSprite->SetDimensions(640, 480);


	g_pd3dxFont = new Font;
	g_pd3dxFont->Create(48, "Arial");
#endif
	return true;
}

CBase::~CBase()
{
	CWinConsole::StopConsole();

	SAFE_DELETE(g_pSprite);
	SAFE_DELETE(g_pd3dxFont);

    if(m_lpDX)
	{
        m_lpDX->Cleanup();
		delete m_lpDX;
	}
}

void CBase::MainLoop()
{
	if(m_lpDX)
	{
		m_lpDX->Run(RenderStub);
	}

}
	
void CBase::Render(float fTime)
{
	// update the user interface
	CGUIManager::GetInstance()->Update(fTime);


	// clear the screen and z buffer
    m_lpDX->lpDevice->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,0x00000000,1.0f,0);
    if (D3D_OK==m_lpDX->lpDevice->BeginScene()) 
	{
        // Place your rendering code here


		// just clear the zbuffer. This makes the GUI objects always on top
		m_lpDX->lpDevice->Clear(0,NULL,D3DCLEAR_ZBUFFER,0x00000000,1.0f,0);
		// render the GUI
		if(m_pGUIManager)
		{
			m_pGUIManager->Render();
		}

#if 0
		if(g_pSprite)
			g_pSprite->Render();

		g_pd3dxFont->DrawText( "This is a line 1.", D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), 100, 100 );
		g_pd3dxFont->DrawText( "This is a line 2.", D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 200, 200 );
		g_pd3dxFont->DrawText( "This is a line 3.", D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f), 300, 300 );
#endif

        m_lpDX->lpDevice->EndScene();
    }
    m_lpDX->lpDevice->Present(NULL,NULL,NULL,NULL);
}


void CBase::HandleSize(WPARAM wParam)
{
    // clear or set activity flag to reflect focus

    if (m_lpDX) {
        if( SIZE_MAXHIDE==wParam || SIZE_MINIMIZED==wParam )
            m_lpDX->active = FALSE;
        else
            m_lpDX->active = TRUE;
    }
}

