#include "stdafx.h"
#include "dsutil.h"
#include "OggPlayer.h"
#include "DXBase.h"

// these #pragmas tell the linker to include the libraries in the final link
// this allows the main application to not be concerned with DirectX libraries
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dxerr9.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "vorbisfile_static_d.lib")
#pragma comment(lib, "ogg_static_d.lib")



DXBase *DXBase::m_pDXBase = NULL;


DXBase::DXBase(HINSTANCE hInstance,LPCSTR pClass,LPCSTR pCaption,WNDPROC pProc)
{
	if(!m_pDXBase)
	{
		WNDCLASS wc;

		// clear the error register

		lastErr=S_OK;

		// clear the active flag

		active=FALSE;

		// Set up and register window class

		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = (WNDPROC) pProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = sizeof(DWORD);
		wc.hInstance = hInstance;
		wc.hIcon = NULL;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = pClass;
		if (!RegisterClass(&wc)) 
		{
			lastErr=-2;
			return;
		}

		// Get dimensions of display

		int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
		hwnd = CreateWindow(pClass, // class
							pCaption, // caption
							WS_OVERLAPPEDWINDOW, // style 
							0, // left
							0, // top
							ScreenWidth, // width
							ScreenHeight, // height
							NULL, // parent window
							NULL, // menu 
							hInstance, // instance
							NULL); // parms
		if (!hwnd) 
		{
			lastErr=-3;
			return;
		}
		m_pDXBase = this;

		m_pSoundManager = NULL;
	}
}

DXBase::~DXBase()
{
    // call cleanup in case program did not
    // the cleanup routine will only release objects once, despite repeated calls 

    Cleanup();
}

void DXBase::Cleanup()
{
    // clear active flag
    active=FALSE;

	// walk the music map and release anything there.


	// release the sound interfaces
	SAFE_DELETE(m_pSoundManager);

    // release 3D interfaces

    SAFE_RELEASE(lpDevice);
    SAFE_RELEASE(lpD3D);
}

BOOL DXBase::TestFormat(D3DFORMAT fmt)
{
    if (D3D_OK==lpD3D->CheckDeviceType(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,fmt,fmt,FALSE))
        return TRUE;
    return FALSE;
}

BOOL DXBase::TestDepth(D3DFORMAT fmt)
{
    if (D3D_OK!=lpD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,format,
                                         D3DUSAGE_DEPTHSTENCIL,D3DRTYPE_SURFACE,fmt))
        return FALSE;
    if (D3D_OK!=lpD3D->CheckDepthStencilMatch(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,
                                              format,format,fmt))
        return FALSE;
    return TRUE;
}

#define DXTestFmt(x) if (TestFormat(x)) {format=x; break;}
#define DXTestDepth(x) if (TestDepth(x)) zFormat=x

BOOL DXBase::Init(WORD wWidth,WORD wHeight,WORD wDepth,WORD wBackBuffers,BOOL bWindowed)
{
    // save screen parameters

    width=wWidth;
    height=wHeight;

    SetWindowPos(hwnd,HWND_TOP,0,0,width,height,SWP_SHOWWINDOW);

    lpD3D=Direct3DCreate9(D3D_SDK_VERSION);
    if (!lpD3D) 
	{
        lastErr=-4;
        return FALSE;
    }

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp,sizeof(d3dpp));
    if (bWindowed) 
	{
        d3dpp.Windowed=TRUE;
        D3DDISPLAYMODE d3ddm;
        if( FAILED( hRes=lpD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm ) ) ) 
		{
            lastErr=-5;
            return FALSE;
        }
        format=d3dpp.BackBufferFormat=d3ddm.Format; 
    } 
	else 
	{
        d3dpp.Windowed=FALSE;
        switch (wDepth) {
            case 32:
                DXTestFmt(D3DFMT_A8R8G8B8);
                DXTestFmt(D3DFMT_X8R8G8B8);
            case 16:
                DXTestFmt(D3DFMT_R5G6B5);
                DXTestFmt(D3DFMT_X1R5G5B5);
            default:
                format=D3DFMT_UNKNOWN;
        }
        if (format==D3DFMT_UNKNOWN) 
		{
            lastErr=-6;
            return FALSE;
        }
        d3dpp.BackBufferFormat=format;
        d3dpp.FullScreen_RefreshRateInHz=D3DPRESENT_RATE_DEFAULT;
        d3dpp.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;
    }
    d3dpp.BackBufferWidth=width;
    d3dpp.BackBufferHeight=height;
    d3dpp.BackBufferCount=wBackBuffers;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; 
    zFormat=D3DFMT_UNKNOWN;
    DXTestDepth(D3DFMT_D32);
    else DXTestDepth(D3DFMT_D32);
    else DXTestDepth(D3DFMT_D24S8);
    else DXTestDepth(D3DFMT_D24X4S4);
    else DXTestDepth(D3DFMT_D24X8);
    else DXTestDepth(D3DFMT_D16);
    else DXTestDepth(D3DFMT_D15S1);
    else DXTestDepth(D3DFMT_D16_LOCKABLE);
    if (zFormat!=D3DFMT_UNKNOWN) 
	{
        d3dpp.EnableAutoDepthStencil=TRUE;
        d3dpp.AutoDepthStencilFormat=zFormat;
    }
    if( FAILED( hRes=lpD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
                                          D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                          &d3dpp,&lpDevice ) ) ) 
	{

        lastErr=-7;
        return FALSE;
    }    

	// set up the standard DirectX renderstate for all our drawing.
	lpDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	lpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	lpDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	lpDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	lpDevice->SetRenderState( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1 );

	lpDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	lpDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	lpDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	lpDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	lpDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	lpDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );


	// now init the sound stuff
	m_pSoundManager = new CSoundManager;
	m_pSoundManager->Initialize( hwnd, DSSCL_PRIORITY );
	m_pSoundManager->SetPrimaryBufferFormat( 2, 22050, 16 );

    // return success
    return TRUE;
}

int DXBase::Run(void (*ptr)(float))
{
    renderPtr=ptr;

    LONGLONG cur_time; // current time
    BOOL perf_flag=FALSE; // flag determining which timer to use
    LONGLONG last_time=0; // time of previous frame
    float time_elapsed; // time since previous frame
    float time_scale; // scaling factor for time

    // select timer, get current time, and calculate time scale

    if (QueryPerformanceFrequency((LARGE_INTEGER *) &cur_time)) 
	{
        time_scale=1.0f/cur_time;
        QueryPerformanceCounter((LARGE_INTEGER *) &last_time);
        perf_flag=TRUE;
    } 
	else 
	{
        time_scale=0.001f;
        last_time=timeGetTime();
    }

    // set status as active

    active=TRUE;

    // Now we're ready to recieve and process Windows messages.

    BOOL bGotMsg;
    MSG msg;
    PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );

    while( WM_QUIT != msg.message ) 
	{
        if (!active) 
		{
            bGotMsg=GetMessage(&msg, NULL, 0U, 0U);
            if( bGotMsg ) 
			{
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            }
        } 
		else 
		{
            bGotMsg = PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );
            if( bGotMsg ) 
			{
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            } 
			else 
			{
				// no message, go ahead and process a frame

                // use the appropriate method to get time 
                // and calculate elapsed time since last frame

                if (perf_flag) 
                    QueryPerformanceCounter((LARGE_INTEGER *) &cur_time);
                else 
                    cur_time=timeGetTime();

                // calculate elapsed time

                time_elapsed=(cur_time-last_time)*time_scale;

                // save frame time

                last_time=cur_time;

				// update any music that is playing
				std::map<int,OggPlayer *>::iterator it = m_MusicMap.begin();
				while(it != m_MusicMap.end())
				{
					OggPlayer *pOgg = (*it).second;
					if(pOgg)
					{
						if(pOgg->IsPlaying())
						{
							pOgg->Update();
						}
					}
					
					++it;
				}


				// call the app's render function
                if (renderPtr)
                    renderPtr(time_elapsed);
            }
        }
    }

    // return final message

    return msg.wParam;
}

void DXBase::SetRenderFunc(void (__cdecl *ptr)(float))
{
    renderPtr=ptr;
}
 

bool DXBase::PlayMusic(int id, const char *fname)
{
	//check if the id is already taken and close the music
	StopMusic(id);

	OggPlayer *pOgg = new OggPlayer(m_pSoundManager->GetDirectSound());
	pOgg->OpenOgg(fname);
	pOgg->Play(true);
	
	m_MusicMap[id] = pOgg;

	return true;
}

void DXBase::StopMusic(int id)
{
	std::map<int,OggPlayer *>::iterator it = m_MusicMap.find(id);
	if(it != m_MusicMap.end())
	{
		// there is a music already in this slot
		// we need to stop it and delete it.
		OggPlayer *pOgg = (*it).second;
		if(pOgg)
		{
			pOgg->Stop();
			pOgg->Close();
			SAFE_DELETE(pOgg);
			(*it).second = NULL;
		}
	}
}

bool DXBase::PlaySound(int id, const char *fname)
{
	//check if the id is already taken and close the music
	StopSound(id);

	// prepend the sound directory name to the filename
	std::string path = "Sound\\";
	path += fname;

	CSound *pSound;
	m_pSoundManager->Create(&pSound, (char *)path.c_str());
	if(pSound)
	{
		pSound->Play();
		m_SoundMap[id] = pSound;
	}

	return true;
}

void DXBase::StopSound(int id)
{
	std::map<int,CSound *>::iterator it = m_SoundMap.find(id);
	if(it != m_SoundMap.end())
	{
		CSound *pSound = (*it).second;
		if(pSound)
		{
			pSound->Stop();
			SAFE_DELETE(pSound);
			(*it).second = NULL;
		}
	}
}
