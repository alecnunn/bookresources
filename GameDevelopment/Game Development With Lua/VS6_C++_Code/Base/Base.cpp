// Base.cpp: implementation of the CBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Settings.h"
#include <WinConsole.h>
#include "Base.h"

// global pointer to the one and only CBase object
// defined in WinMain.cpp
extern CBase *g_pBase;

extern "C" {
typedef struct 
{
	const char *name;
	int (*func)(lua_State *);
}luaDef;
}


extern luaDef MyGlue[];

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
	m_hConsole = NULL;
}

void LuaErrorHandler(const char *pError)
{
	HWND hConsole = g_pBase->GetConsole();
	if(hConsole && IsWindow(hConsole))
	{
		CWinConsole::Write(pError);
		BringWindowToTop(hConsole);
	}

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

    if (!m_lpDX->Init(800,600,16,2,TRUE)) {
        delete m_lpDX;
        return false;
    }

	// init the glue functions required for the GUI
	for(int i=0; MyGlue[i].name; i++)
	{
		m_lua.AddFunction(MyGlue[i].name, MyGlue[i].func);
	}

	m_lua.SetErrorHandler(LuaErrorHandler);


	m_pSettings = new Settings;
	m_pSettings->Init("Base");

	m_hConsole = CWinConsole::StartConsole(hInstance, &m_lua);
	if(m_hConsole)
	{
		int x = m_pSettings->GetInteger("DebugWinX");
		int y = m_pSettings->GetInteger("DebugWinY");
		int w = m_pSettings->GetInteger("DebugWinW");
		int h = m_pSettings->GetInteger("DebugWinH");

		if(w == 0 || h == 0)
		{
			RECT r;
			GetWindowRect(m_hConsole, &r);
			w = r.right - r.left;
			h = r.bottom - r.top;
		}

		SetWindowPos(m_hConsole, HWND_BOTTOM, x, y, w, h, 0);
	}

	m_pGUIManager = new CGUIManager;
	m_pGUIManager->Init(&m_lua);




//	m_lpDX->PlayMusic(1, "PianoBlues.ogg");




	return true;
}

CBase::~CBase()
{

	// if the debug window is open, save its position for next run
	if(m_hConsole && IsWindow(m_hConsole))
	{
		RECT r;
		GetWindowRect(m_hConsole, &r);

		m_pSettings->SetInteger("DebugWinX", r.left);
		m_pSettings->SetInteger("DebugWinY", r.top);
		m_pSettings->SetInteger("DebugWinW", r.right - r.left);
		m_pSettings->SetInteger("DebugWinH", r.bottom - r.top);

	}

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
	
void CBase::KeyHit(int k)
{
	if(m_pGUIManager)
	{
		m_pGUIManager->KeyHit(k);
	}

}


void CBase::Render(float fTime)
{
	// update the user interface
	CGUIManager::GetInstance()->Update(fTime);


	// clear the screen and z buffer
    m_lpDX->Get3DDevice()->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,0x00000000,1.0f,0);
    if (D3D_OK==m_lpDX->Get3DDevice()->BeginScene()) 
	{
        // Place your rendering code here


		// just clear the zbuffer. This makes the GUI objects always on top
		m_lpDX->Get3DDevice()->Clear(0,NULL,D3DCLEAR_ZBUFFER,0x00000000,1.0f,0);
		// render the GUI
		if(m_pGUIManager)
		{
			m_pGUIManager->Render();
		}
			
        m_lpDX->Get3DDevice()->EndScene();
    }
    m_lpDX->Get3DDevice()->Present(NULL,NULL,NULL,NULL);
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


///////////////////////

extern "C" int _QuitProgram(lua_State *L)
{
	PostQuitMessage(0);
	return 0;
}

extern "C" int _PlaySound(lua_State *L)
{
	int argNum = 1;
	cLua *lua = g_pBase->GetLua();

	int  soundID		= lua->GetNumberArgument(argNum++);
	const char *soundName		= lua->GetStringArgument(argNum++);
	bool bLoop			= lua->GetNumberArgument(argNum++, 0) != 0;

	if(soundName && (soundID != 0))
	{
		DXContext::GetInstance()->PlaySound(soundID, soundName);
	}

	return 0;
}

extern "C" int _StopSound(lua_State *L)
{
	int argNum = 1;
	cLua *lua = g_pBase->GetLua();
	int  soundID		= lua->GetNumberArgument(argNum++);

	if(soundID != 0)
	{
		DXContext::GetInstance()->StopSound(soundID);
	}

	return 0;
}

extern "C" int _PlayMusic(lua_State *L)
{
	int argNum = 1;
	cLua *lua = g_pBase->GetLua();

	int  musicID			= lua->GetNumberArgument(argNum++);
	const char *musicName	= lua->GetStringArgument(argNum++);
	bool bLoop				= lua->GetNumberArgument(argNum++, 0) != 0;

	if(musicName && (musicID != 0))
	{
		DXContext::GetInstance()->PlayMusic(musicID, musicName);
	}

	return 0;
}

extern "C" int _StopMusic(lua_State *L)
{
	int argNum = 1;
	cLua *lua = g_pBase->GetLua();

	int  musicID = lua->GetNumberArgument(argNum++);

	if(musicID != 0)
	{
		DXContext::GetInstance()->StopMusic(musicID);
	}

	return 0;
}

luaDef MyGlue[] = 
{
  {"QuitProgram",			_QuitProgram},
  {"PlaySound",				_PlaySound},
  {"StopSound",				_StopSound},
  {"PlayMusic",				_PlayMusic},
  {"StopMusic",				_StopMusic},
  {NULL,					NULL},
};
