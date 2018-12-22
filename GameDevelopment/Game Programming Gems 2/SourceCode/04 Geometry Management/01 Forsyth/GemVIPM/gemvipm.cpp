/* Copyright (C) Tom Forsyth, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Tom Forsyth, 2001"
 */



#include "TomsD3DLib.h"


#define STRICT
#define D3D_OVERLOADS
#include <windows.h>
#include <tchar.h>
#include <math.h>
#include <stdio.h>
#include <D3DX8.h>
#include <DInput.h>
#include "D3DApp.h"
#include "D3DFont.h"
#include "D3DUtil.h"
#include "DXUtil.h"
#include "resource.h"
#include "commctrl.h"
#include <list>


#include "object.h"



//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
struct CUSTOMVERTEX
{
    D3DXVECTOR3 v;
    DWORD       diffuse;
    DWORD       specular;
    FLOAT       tu, tv;
};

#define CUSTOMVERTEX_FVF (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_SPECULAR|D3DFVF_TEX1)


struct CUSTOM3VERTEX
{
    D3DXVECTOR3 v;
    DWORD       diffuse;
    DWORD       specular;
    FLOAT       tu, tv, tt;
};

#define CUSTOM3VERTEX_FVF (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_SPECULAR|D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE3(0))




struct TVERTEX
{
    D3DXVECTOR3 v;
	float		rhw;
};

#define TVERTEX_FVF (D3DFVF_XYZRHW)




// Fudge - exposes m_pd3dDevice to the outside world.
LPDIRECT3DDEVICE8 g_pd3dDevice = NULL;





// DInput stuff.

IDirectInput8*			g_pDI			= NULL;
IDirectInputDevice8*	g_pMouse		= NULL;
HINSTANCE				g_hInst			= NULL;
BOOL					g_bActive		= TRUE;
BOOL					g_bExclusive	= FALSE;
BOOL					g_bKeyDownCtrl	= FALSE;
BOOL					g_bKeyDownShift	= FALSE;
BOOL					m_bOrbitStrafe	= TRUE;


DIMOUSESTATE2			g_dims;      // DirectInput mouse state structure



//-----------------------------------------------------------------------------
// Function: SetAcquire
//
// Description: 
//      Acquire or unacquire the mouse, depending on if the app is active
//       Input device must be acquired before the GetDeviceState is called
//
//-----------------------------------------------------------------------------
HRESULT SetAcquire( HWND hWnd )
{
	// nothing to do if g_pMouse is NULL
	if (NULL == g_pMouse)
		return S_FALSE;

	if (g_bActive) 
	{
		// acquire the input device 
		g_pMouse->Acquire();
	} 
	else 
	{
		// unacquire the input device 
		g_pMouse->Unacquire();
	}

	return S_OK;
}




//-----------------------------------------------------------------------------
// Function: UpdateInputState
//
// Description: 
//      Get the input device's state and display it.
//
//-----------------------------------------------------------------------------
char g_szMouseText[256];    // current  mouse state text
HRESULT UpdateInputState( HWND hWnd )
{
	if (NULL != g_pMouse) 
	{
		HRESULT hr;

		hr = DIERR_INPUTLOST;

		// if input is lost then acquire and keep trying 
		while ( DIERR_INPUTLOST == hr ) 
		{
			// get the input's device state, and put the state in g_dims
			hr = g_pMouse->GetDeviceState( sizeof(DIMOUSESTATE2), &g_dims );

			if ( hr == DIERR_INPUTLOST )
			{
				// DirectInput is telling us that the input stream has
				// been interrupted.  We aren't tracking any state
				// between polls, so we don't have any special reset
				// that needs to be done.  We just re-acquire and
				// try again.
				hr = g_pMouse->Acquire();
				if ( FAILED(hr) )
				{
					return hr;
				}
			}
		}

		if ( FAILED(hr) )
		{
			return hr;
		}

	}
	else
	{
		// No mouse!
	}

	return S_OK;
}




//-----------------------------------------------------------------------------
// Function: InitDirectInput
//
// Description: 
//      Initialize the DirectInput variables.
//
//-----------------------------------------------------------------------------
HRESULT InitDirectInput( HWND hWnd, bool bExclusive = FALSE )
{
	HRESULT hr;

	// Register with the DirectInput subsystem and get a pointer
	// to a IDirectInput interface we can use.
	hr = DirectInput8Create( g_hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&g_pDI, NULL );
	if ( FAILED(hr) )
	{
		ASSERT ( FALSE );
		return hr;
	}

	// Obtain an interface to the system mouse device.
	hr = g_pDI->CreateDevice( GUID_SysMouse, &g_pMouse, NULL );
	if ( FAILED(hr) )
	{
		ASSERT ( FALSE );
		return hr;
	}

	// Set the data format to "mouse format" - a predefined data format 
	//
	// A data format specifies which controls on a device we
	// are interested in, and how they should be reported.
	//
	// This tells DirectInput that we will be passing a
	// DIMOUSESTATE2 structure to IDirectInputDevice::GetDeviceState.
	hr = g_pMouse->SetDataFormat( &c_dfDIMouse2 );
	if ( FAILED(hr) )
	{
		ASSERT ( FALSE );
		return hr;
	}

	// Set the cooperativity level to let DirectInput know how
	// this device should interact with the system and with other
	// DirectInput applications.
	DWORD dwFlags = DISCL_FOREGROUND;


// KLUDGE! Win2k really hates FOREGROUND+EXCLUSIVE devices - don't know why.
#if 0
	if ( bExclusive )
	{
		dwFlags |= DISCL_EXCLUSIVE;
	}
	else
	{
		dwFlags |= DISCL_NONEXCLUSIVE;
	}
#else
	dwFlags |= DISCL_NONEXCLUSIVE;
#endif


	hr = g_pMouse->SetCooperativeLevel( hWnd, dwFlags );
	if ( FAILED(hr) )
	{
		ASSERT ( FALSE );
		return hr;
	}

	// And acquire it the first time (the windows message has already missed us).
	SetAcquire( hWnd );

	return S_OK;
}





//-----------------------------------------------------------------------------
// Function: FreeDirectInput
//
// Description: 
//      Initialize the DirectInput variables.
//
//-----------------------------------------------------------------------------
HRESULT FreeDirectInput()
{
	// Unacquire and release any DirectInputDevice objects.
	if (NULL != g_pMouse) 
	{
		// Unacquire the device one last time just in case 
		// the app tried to exit while the device is still acquired.
		g_pMouse->Unacquire();

		g_pMouse->Release();
		g_pMouse = NULL;
	}

	// Release any DirectInput objects.
	if (NULL != g_pDI) 
	{
		g_pDI->Release();
		g_pDI = NULL;
	}

	return S_OK;
}



// Enter or leave exclusive mode.
HRESULT SetExclusiveMode ( bool bExclusive, HWND hWnd )
{
	if ( ( bExclusive && g_bExclusive ) || ( !bExclusive && !g_bExclusive ) )
	{
		return ( DI_OK );
	}


	// Need to rip it all down and recreate - what a hassle.
	FreeDirectInput();

	HRESULT hr = InitDirectInput ( hWnd, bExclusive );
	if ( FAILED ( hr ) )
	{
		return ( hr );
	}

	g_bExclusive = bExclusive;

	return ( hr );
}








//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class (CD3DApplication) provides the 
//       generic functionality needed in all Direct3D samples. CMyD3DApplication 
//       adds functionality specific to this sample program.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
    CD3DFont*          m_pFont;

	BOOL				m_bIgnoreBackFaced;
	BOOL				m_bShowSlidingWindowInfo;
	float				m_fSlidingWindowErrorTolerance;
	int					m_iFindBestErrorCountdown;
	VIPMTypeEnum		m_vteCurrentDisplayStyle;

	BOOL				m_bCreateCollapseMode;

	int					m_iTargetNumCollapses;
	float				m_fTargetErrorFactor;
	BOOL				m_bTargetErrorAutoGen;
	BOOL				m_bWireframe;
	int					m_iCreateThisManyCollapses;


	MeshEdge			*m_pedgeBestError;
	MeshPt				*m_pptBestError;


	ObjectInstance		m_ObjectInstRoot;
	Object				*m_pObject;
	D3DXMATRIX			m_matView;
	D3DXMATRIX			m_matProjClose;
	D3DXMATRIX			m_matProjCloseZbias;
	D3DXMATRIX			m_matProjFar;

    HRESULT ConfirmDevice( D3DCAPS8*, DWORD, D3DFORMAT );
    LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

protected:
    HRESULT OneTimeSceneInit();
    HRESULT InitDeviceObjects();
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
    HRESULT DeleteDeviceObjects();
    HRESULT Render();
    HRESULT FrameMove();
    HRESULT FinalCleanup();
	void SetMenuItems();

public:
    CMyD3DApplication();
};




//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    CMyD3DApplication d3dApp;

	// DInput needs the instance.
    g_hInst = hInst;


    if( FAILED( d3dApp.Create( hInst ) ) )
        return 0;

    return d3dApp.Run();
}




//-----------------------------------------------------------------------------
// Name: CMyD3DApplication()
// Desc: Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication()
{
    m_strWindowTitle  = _T("VIPM tester");
    m_bUseDepthBuffer = TRUE;

    m_pFont                = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );

	m_bIgnoreBackFaced = TRUE;
	m_bShowSlidingWindowInfo = TRUE;
	m_bCreateCollapseMode = TRUE;
	m_fSlidingWindowErrorTolerance = 0.1f;	// 10%
	m_iFindBestErrorCountdown = 0;
	m_vteCurrentDisplayStyle = VIPMType_Vanilla;

	m_iTargetNumCollapses = 0;
	m_fTargetErrorFactor = 1.0f;
	m_bTargetErrorAutoGen = TRUE;
	m_bWireframe = FALSE;
	m_iCreateThisManyCollapses = 0;

	m_pedgeBestError = NULL;
	m_pptBestError = NULL;



	g_iMaxNumTrisDrawn = -1;	// No limit.
	g_bOptimiseVertexOrder = FALSE;
	g_bShowVIPMInfo = FALSE;
#if ALLOW_PROGRESS_BARS
	g_hWndApp = NULL;		// Will be set later.
#endif


}




//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::OneTimeSceneInit()
{

	m_pObject = new Object;
	ObjectInstance *pInst;
	D3DXMATRIX mat;
	D3DXMatrixIdentity ( &mat );
	
	pInst = &m_ObjectInstRoot;

	// First one should always be at the origin - it's used for editing.
	pInst = new ObjectInstance ( m_pObject, pInst );
	pInst->matOrn = mat;
	pInst->iRenderMethod = m_vteCurrentDisplayStyle;


	// Now create loads of objects randomly distributed in a cube or something.
	DWORD dwSeed = 0x12345678;
	float fSize = 100.f;
	for ( int i = 0; i < 1000; i++ )
	{
		pInst = new ObjectInstance ( m_pObject, pInst );
		pInst->matOrn = mat;
		pInst->matOrn._41 = ( ( (float)( ( dwSeed >>  0 ) & 0xff ) * ( 1.0f / 255.0f ) ) - 0.5f ) * fSize;
		pInst->matOrn._42 = ( ( (float)( ( dwSeed >>  8 ) & 0xff ) * ( 1.0f / 255.0f ) ) - 0.5f ) * fSize;
		pInst->matOrn._43 = ( ( (float)( ( dwSeed >> 16 ) & 0xff ) * ( 1.0f / 255.0f ) ) - 0.5f ) * fSize;
		pInst->iRenderMethod = m_vteCurrentDisplayStyle;

		D3DXMATRIX matTemp;

		D3DXMatrixRotationX( &matTemp, (float)( ( dwSeed >> 4 ) & 0xff ) * ( 3.14 / 255.0f ) );
		D3DXMatrixMultiply ( &(pInst->matOrn), &matTemp, &(pInst->matOrn) );
		D3DXMatrixRotationY( &matTemp, (float)( ( dwSeed >> 12 ) & 0xff ) * ( 3.14 / 255.0f ) );
		D3DXMatrixMultiply ( &(pInst->matOrn), &matTemp, &(pInst->matOrn) );
		D3DXMatrixRotationZ( &matTemp, (float)( ( dwSeed >> 20 ) & 0xff ) * ( 3.14 / 255.0f ) );
		D3DXMatrixMultiply ( &(pInst->matOrn), &matTemp, &(pInst->matOrn) );
																			
		// This has no mathematical rigour whatsoever. Just letting you know.
		dwSeed += 0x61902856;
		dwSeed *= 0x34977;
		dwSeed ^= dwSeed >> 23;
	}

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove()
{

    return S_OK;
}







void DrawScreenSpacePoint ( D3DXVECTOR3 &vec )
{
	TVERTEX vert[4];
	vert[0].rhw = 0.5f;		// Random non-wierd value.
	vert[1].rhw = 0.5f;
	vert[2].rhw = 0.5f;
	vert[3].rhw = 0.5f;

	HRESULT hres;
	hres = g_pd3dDevice->SetVertexShader ( TVERTEX_FVF );
	const float fSizeOfDot = 4.0f;

	vert[0].v		= vec;
	vert[0].v.y -= fSizeOfDot;

	vert[1].v		= vec;
	vert[1].v.x -= fSizeOfDot * 0.866f;
	vert[1].v.y += fSizeOfDot * 0.5f;

	vert[2].v		= vec;
	vert[2].v.x += fSizeOfDot * 0.866f;
	vert[2].v.y += fSizeOfDot * 0.5f;

	vert[3].v		= vert[0].v;

	hres = g_pd3dDevice->DrawPrimitiveUP ( D3DPT_LINESTRIP, 3, vert, sizeof(vert[0]) );
}

void DrawScreenSpacePoint ( D3DXVECTOR4 &vec )
{
	if ( vec.w > 0.0f )
	{
		D3DXVECTOR3 vec3 ( vec.x, vec.y, vec.z );
		DrawScreenSpacePoint ( vec3 );
	}
}


void DrawScreenSpaceEdge ( D3DXVECTOR3 &vecBig, D3DXVECTOR3 &vecSmall )
{
	TVERTEX vert[4];
	vert[0].rhw = 0.5f;		// Random non-wierd value.
	vert[1].rhw = 0.5f;
	vert[2].rhw = 0.5f;
	vert[3].rhw = 0.5f;

	HRESULT hres;
	hres = g_pd3dDevice->SetVertexShader ( TVERTEX_FVF );
	const float fSizeOfDot = 4.0f;

	vert[0].v		= vecBig;
	vert[0].v.y -= fSizeOfDot;

	vert[1].v		= vecBig;
	vert[1].v.x -= fSizeOfDot * 0.866f;
	vert[1].v.y += fSizeOfDot * 0.5f;

	vert[2].v		= vecBig;
	vert[2].v.x += fSizeOfDot * 0.866f;
	vert[2].v.y += fSizeOfDot * 0.5f;

	vert[3].v		= vecSmall;

	WORD wIndex[6];
	wIndex[0] = 0;
	wIndex[1] = 3;
	wIndex[2] = 1;
	wIndex[3] = 3;
	wIndex[4] = 2;
	wIndex[5] = 3;

	hres = g_pd3dDevice->DrawIndexedPrimitiveUP ( D3DPT_LINELIST, 0, 4, 3, wIndex, D3DFMT_INDEX16, vert, sizeof(vert[0]) );
}

void DrawScreenSpaceEdge ( D3DXVECTOR4 &vecBig, D3DXVECTOR4 &vecSmall )
{
	if ( ( vecBig.w > 0.0f ) && ( vecSmall.w > 0.0f ) )
	{
		D3DXVECTOR3 vecBig3 ( vecBig.x, vecBig.y, vecBig.z );
		D3DXVECTOR3 vecSmall3 ( vecSmall.x, vecSmall.y, vecSmall.z );
		DrawScreenSpaceEdge ( vecBig3, vecSmall3 );
	}
}







//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{


	ObjectInstance *pFirstObjInst = m_ObjectInstRoot.ListNext();
	ASSERT ( pFirstObjInst != NULL );
	Object *pFirstObj = pFirstObjInst->pObj;


	MeshEdge *pClosestEdge = NULL;
	MeshPt *pClosestPt = NULL;

	float fBestError = 0.0f;
	float fBestOfAnyError = 0.0f;


	if ( m_bCreateCollapseMode )
	{

		// Find out which edge we are closest to.
		// This involves first transforming all the vertices to screenspace.
		// Edges with either end point behind the camera are ignored.

		// Find the complete transform from object space to camera space.
		D3DXMATRIX matViewport;
		D3DVIEWPORT8 vpViewport;

		// The framework annoyingly hides the viewport from me, and indeed it's implicit, not set.
		//HRESULT hres = m_pd3dDevice->GetViewport ( &vpViewport );
		vpViewport.MaxZ = 1.0f;
		vpViewport.MinZ = 1.0f;
		vpViewport.X = 0;
		vpViewport.Y = 0;
		vpViewport.Height = m_d3dsdBackBuffer.Height;
		vpViewport.Width = m_d3dsdBackBuffer.Width;

		MakeMatrixFromViewport ( matViewport, vpViewport );

		D3DXMATRIX matTotal;
		D3DXMatrixMultiply ( &matTotal, &(pFirstObjInst->matOrn), &m_matView );
		D3DXMatrixMultiply ( &matTotal, &(matTotal), &m_matProjClose );
		D3DXMatrixMultiply ( &matTotal, &(matTotal), &matViewport );



		POINT ptMouse;
		if( GetFocus() )
		{
			// Where is the mouse?
			GetCursorPos( &ptMouse );
			ScreenToClient( m_hWnd, &ptMouse );
		}
		else
		{
			ptMouse.x = 0;
			ptMouse.y = 0;
		}


		if ( pFirstObj->bSomethingHappened )
		{
			pFirstObj->bSomethingHappened = FALSE;
		}


		MeshPt *ppt;
		MeshEdge *pedge;
		MeshTri *ptri;

		for ( ppt = pFirstObj->CurPtRoot.ListNext(); ppt != NULL; ppt = ppt->ListNext() )
		{
			ppt->mypt.bFrontFaced = FALSE;

			//D3DXVec3TransformCoord ( &ppt->mypt.v4ScrPos, &(ppt->mypt.vPos), &matTotal );
			D3DXVECTOR4 &pPos = ppt->mypt.v4ScrPos;
			D3DXVec3Transform ( &pPos, &(ppt->mypt.vPos), &matTotal );
			// And do the homogenous thing.
			// I do it this way, instead of using D3DXVec3TransformCoord, because the
			// sign of the W tells me if the vert was behind the camera or not.
			pPos.x /= pPos.w;
			pPos.y /= pPos.w;
			pPos.z /= pPos.w;
		}

		for ( pedge = pFirstObj->CurEdgeRoot.ListNext(); pedge != NULL; pedge = pedge->ListNext() )
		{
			pedge->myedge.bFrontFaced = FALSE;
		}

		for ( ptri = pFirstObj->CurTriRoot.ListNext(); ptri != NULL; ptri = ptri->ListNext() )
		{
			ptri->mytri.bFrontFaced = FALSE;
			D3DXVECTOR2 v2Pos1 ( ptri->pPt1->mypt.v4ScrPos.x, ptri->pPt1->mypt.v4ScrPos.y );
			D3DXVECTOR2 v2Pos2 ( ptri->pPt2->mypt.v4ScrPos.x, ptri->pPt2->mypt.v4ScrPos.y );
			D3DXVECTOR2 v2Pos3 ( ptri->pPt3->mypt.v4ScrPos.x, ptri->pPt3->mypt.v4ScrPos.y );

			// Find front facing tris.
			v2Pos2 -= v2Pos1;
			v2Pos3 -= v2Pos1;
			if ( ( v2Pos2.x * v2Pos3.y - v2Pos3.x * v2Pos2.y ) > 0.0f )
			{
				ptri->mytri.bFrontFaced = TRUE;
				ptri->pPt1->mypt.bFrontFaced = TRUE;
				ptri->pPt2->mypt.bFrontFaced = TRUE;
				ptri->pPt3->mypt.bFrontFaced = TRUE;
				ptri->pEdge12->myedge.bFrontFaced = TRUE;
				ptri->pEdge23->myedge.bFrontFaced = TRUE;
				ptri->pEdge31->myedge.bFrontFaced = TRUE;
			}
		}


		// Find the closest point to the cursor position.
		// Ignore points with no edges on them.
		float fClosestDistSquared = 1.0e10f;
		for ( ppt = pFirstObj->CurPtRoot.ListNext(); ppt != NULL; ppt = ppt->ListNext() )
		{
			if ( ppt->FirstEdge() != NULL )
			{
				// Is this one closer?
				D3DXVECTOR4 &pPos = ppt->mypt.v4ScrPos;
				if ( pPos.w > 0.0f )
				{
					if ( ppt->mypt.bFrontFaced || !m_bIgnoreBackFaced )
					{
						float fTemp;
						fTemp = pPos.x - (float)ptMouse.x;
						float fDistSquared = fTemp * fTemp;
						fTemp = pPos.y - (float)ptMouse.y;
						fDistSquared += fTemp * fTemp;

						if ( fClosestDistSquared > fDistSquared )
						{
							fClosestDistSquared = fDistSquared;
							pClosestPt = ppt;
						}
					}
				}
			}

			// Keep the FirstEdge/NextEdge sequencer happy.
			ppt->EndEdge();
		}


		// Now find the closest edge from the closest point.
		fClosestDistSquared = 1.0e10f;
		if ( pClosestPt != NULL )
		{
			D3DXVECTOR2 v2Pos1 ( pClosestPt->mypt.v4ScrPos.x, pClosestPt->mypt.v4ScrPos.y );
			D3DXVECTOR2 v2Mouse ( (float)ptMouse.x, (float)ptMouse.y );
			D3DXVECTOR2 v2DeltaMouse = v2Mouse - v2Pos1;
			float fDeltaMouseSqu = D3DXVec2Dot ( &v2DeltaMouse, &v2DeltaMouse );
			for ( MeshEdge *pedge = pClosestPt->FirstEdge(); pedge != NULL; pedge = pClosestPt->NextEdge() )
			{
				if ( pedge->myedge.bFrontFaced || !m_bIgnoreBackFaced )
				{
					MeshPt *pOtherPt = pedge->OtherPt ( pClosestPt );

					if ( pOtherPt->mypt.v4ScrPos.w > 0.0f )
					{
						D3DXVECTOR2 v2Pos2 ( pOtherPt->mypt.v4ScrPos.x, pOtherPt->mypt.v4ScrPos.y );
						D3DXVECTOR2 v2DeltaPos2 = v2Pos2 - v2Pos1;

						float fDeltaPos2Squ = D3DXVec2Dot ( &v2DeltaPos2, &v2DeltaPos2 );
						float fPos2DotMouse = D3DXVec2Dot ( &v2DeltaPos2, &v2DeltaMouse );
						if ( fPos2DotMouse > 0.0f )
						{
							float fDistSquared = fDeltaMouseSqu - ( fPos2DotMouse * fPos2DotMouse ) / fDeltaPos2Squ;
							if ( fClosestDistSquared > fDistSquared )
							{
								fClosestDistSquared = fDistSquared;
								pClosestEdge = pedge;
							}
						}
					}
				}
			}
		}


		if ( m_iFindBestErrorCountdown > 0 )
		{
			// Don't find the best one yet.
			// This allows some operations to be done quickly without
			// this (very slow) search slowing stuff down.
			m_pedgeBestError = NULL;
			m_pptBestError = NULL;
			m_iFindBestErrorCountdown--;
		}
		else
		{
			// Find the best collapse you can.
			// (how expensive is this? Ohhhh yes).
			fBestError = 1.0e10f;
			m_pedgeBestError = NULL;
			m_pptBestError = NULL;
			// NL = NewLevel - would force a new level.
			float fBestErrorNL = 1.0e10f;
			MeshEdge *pedgeBestErrorNL = NULL;
			MeshPt *pptBestErrorNL = NULL;

		// Set to 0 (mean) or 1 (geom av)
	#define USE_GEOM_AV 0

	#if USE_GEOM_AV
			float fAverage = 1.0f;
	#else
			float fAverage = 0.0f;
	#endif
			int iAvCount = 0;

			// Flush the cache, just in case.
			pFirstObj->FindCollapseError ( NULL, NULL, FALSE );

			for ( ppt = pFirstObj->CurPtRoot.ListNext(); ppt != NULL; ppt = ppt->ListNext() )
			{
				// Disallow any pts that are on an edge - shouldn't be collapsing them.
				BOOL bAllowed = TRUE;
				for ( pedge = ppt->FirstEdge(); pedge != NULL; pedge = ppt->NextEdge() )
				{
					if ( ( pedge->pTri12 == NULL ) || ( pedge->pTri21 == NULL ) )
					{
						// This edge does not have two tris on it - disallow it.
						bAllowed = FALSE;
					}
				}
				if ( !bAllowed )
				{
					continue;
				}

				BOOL bRequiresNewLevel = FALSE;
				if ( m_bShowSlidingWindowInfo && !pFirstObj->CollapseAllowedForLevel ( ppt, pFirstObj->iCurSlidingWindowLevel ) )
				{
					// This collapse would force a new level.
					bRequiresNewLevel = TRUE;
				}

				for ( pedge = ppt->FirstEdge(); pedge != NULL; pedge = ppt->NextEdge() )
				{
					float fErrorBin = pFirstObj->FindCollapseError ( ppt, pedge, TRUE );

					iAvCount++;

	#if USE_GEOM_AV
					fAverage *= fErrorBin;
	#else
					fAverage += fErrorBin;
	#endif

					if ( bRequiresNewLevel )
					{
						if ( fBestErrorNL > fErrorBin )
						{
							fBestErrorNL = fErrorBin;
							pedgeBestErrorNL = pedge;
							pptBestErrorNL = ppt;
						}
					}
					else
					{
						if ( fBestError > fErrorBin )
						{
							fBestError = fErrorBin;
							m_pedgeBestError = pedge;
							m_pptBestError = ppt;
						}
					}
				}
			}

			if ( m_bShowSlidingWindowInfo )
			{

				// Store the best of either error for later display.
				if ( fBestError > fBestErrorNL )
				{
					fBestOfAnyError = fBestErrorNL;
				}
				else
				{
					fBestOfAnyError = fBestError;
				}



				// Tweak up the NewLevel errors by a factor.
	#if USE_GEOM_AV
				fAverage = powf ( fAverage, (float)iAvCount );
	#else
				fAverage /= (float)iAvCount;
	#endif

				if ( fBestError > ( fBestErrorNL + fAverage * m_fSlidingWindowErrorTolerance ) )
				{
					// Despite the boost, it's still the best,
					// so bite the bullet and do the collapse.
					fBestError = fBestErrorNL;
					m_pedgeBestError = pedgeBestErrorNL;
					m_pptBestError = pptBestErrorNL;
				}
			}
			else
			{
				ASSERT ( pedgeBestErrorNL == NULL );
			}
	#undef USE_GEOM_AV

			// Flush the cache, just in case.
			pFirstObj->FindCollapseError ( NULL, NULL, FALSE );
		}



		

		// Move the viewer here. FrameMove() doesn't get called during pause mode,
		// and we still want to move around then.

		static DWORD dwLastTime = timeGetTime();


		// Get the relative time, in seconds
		DWORD dwThisTime = timeGetTime();
		float fTimeKey = (float)( dwThisTime - dwLastTime ) * 0.001f;
		if ( fTimeKey > 0.1f )
		{
			// A hiccup in framerate - clamp it.
			fTimeKey = 0.1f;
		}
		dwLastTime = dwThisTime;





		// Clear the render target
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x00000000f, 1.0f, 0L );

		// Begin the scene
		if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
		{


			m_pd3dDevice->SetTransform( D3DTS_VIEW,       &m_matView );
			m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProjClose );

			ObjectInstance *pObjInst = pFirstObjInst;
			Object *pObj = pObjInst->pObj;

			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2 );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );


			m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
			m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

			m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
			m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );


			m_pd3dDevice->SetTexture( 0, NULL );



			m_pd3dDevice->SetTransform( D3DTS_WORLD,      &pObjInst->matOrn );


			m_pd3dDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );
			m_pd3dDevice->SetRenderState ( D3DRS_ZENABLE, TRUE );
			m_pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, 0xffff0000 );
			m_pd3dDevice->SetRenderState ( D3DRS_FILLMODE, D3DFILL_SOLID );


			// Draw the current state of the object.
			if ( m_bShowSlidingWindowInfo )
			{
				// Render twice.
				// Current level.
				pObjInst->RenderCurrentObject ( m_pd3dDevice, 0, pObj->iCurSlidingWindowLevel );
				// Next level.
				m_pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, 0xff800000 );
				pObjInst->RenderCurrentObject ( m_pd3dDevice, 0, pObj->iCurSlidingWindowLevel + 1 );
			}
			else
			{
				pObjInst->RenderCurrentObject ( m_pd3dDevice, 0 );
			}


			// Tweak the projection matrix so that things are a bit more visible.
			m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProjCloseZbias );


			// Render all edges
			m_pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, 0xff000000 );
			m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

			pObjInst->RenderCurrentEdges ( m_pd3dDevice, 0, m_bIgnoreBackFaced );
			// Render visible edges again in a different colour.
			m_pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, 0xffffff00 );
			m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
			pObjInst->RenderCurrentEdges ( m_pd3dDevice, 0, m_bIgnoreBackFaced );


			if ( pFirstObj->bSomethingHappened )
			{
				// A collapse or expand happened, so the ps and edges
				// may not be valid any more.
				pFirstObj->bSomethingHappened = FALSE;
				pClosestPt = NULL;
				pClosestEdge = NULL;
				m_pptBestError = NULL;
				m_pedgeBestError = NULL;
			}


	#if 0
	// Shows the vertices on-screen.
	// Basically just shows that my software projection pipe works!

			m_pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, 0xff0000ff );
			m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
			for ( MeshPt *ppt = pFirstObj->CurPtRoot.ListNext(); ppt != NULL; ppt = ppt->ListNext() )
			{
				DrawScreenSpacePoint ( ppt->mypt.v4ScrPos );
			}
	#endif


			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2 );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );

			m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
			m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

			m_pd3dDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );



			if ( pClosestPt != NULL )
			{
				m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

				m_pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, 0xff00ff00 );
				if ( m_bShowSlidingWindowInfo )
				{
					if ( !pFirstObj->CollapseAllowedForLevel ( pClosestPt, pFirstObj->iCurSlidingWindowLevel ) )
					{
						// This collapse would force a new level to be created.
						m_pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, 0xff008000 );
					}
				}

				// Draw the current pt.
				DrawScreenSpacePoint ( pClosestPt->mypt.v4ScrPos );

				// And draw the current edge.
				if ( pClosestEdge != NULL )
				{
					MeshPt *pOtherPt = pClosestEdge->OtherPt ( pClosestPt );
					DrawScreenSpaceEdge ( pClosestPt->mypt.v4ScrPos, pOtherPt->mypt.v4ScrPos );
				}
			}

			if ( m_pptBestError != NULL )
			{
				m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

				m_pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, 0xff0080ff );
				if ( m_bShowSlidingWindowInfo )
				{
					if ( !pFirstObj->CollapseAllowedForLevel ( m_pptBestError, pFirstObj->iCurSlidingWindowLevel ) )
					{
						// This collapse would force a new level to be created.
						m_pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, 0xff0000ff );
					}
				}

				// Draw the best-error pt.
				DrawScreenSpacePoint ( m_pptBestError->mypt.v4ScrPos );

				// And draw the best-error edge.
				if ( m_pedgeBestError != NULL )
				{
					MeshPt *pOtherPt = m_pedgeBestError->OtherPt ( m_pptBestError );
					DrawScreenSpaceEdge ( m_pptBestError->mypt.v4ScrPos, pOtherPt->mypt.v4ScrPos );
				}
			}

			// Output statistics
			m_pFont->DrawText( 2,  0, D3DCOLOR_ARGB(255,255,255,0), m_strFrameStats );
			m_pFont->DrawText( 2, 20, D3DCOLOR_ARGB(255,255,255,0), m_strDeviceStats );

			char strTemp[1000];
			sprintf ( strTemp, "Collapse-creation mode." );
			m_pFont->DrawText( 2, 40, D3DCOLOR_ARGB(255,255,255,0), strTemp );


			if ( pClosestEdge != NULL )
			{
				// Show the error for this collapse.
				ASSERT ( pClosestPt != NULL );
				float fError = pFirstObj->FindCollapseError ( pClosestPt, pClosestEdge );
				sprintf ( strTemp, "User-selected collapse error %f", fError );
				m_pFont->DrawText( 2, 60, D3DCOLOR_ARGB(255,255,255,0), strTemp );
			}
			if ( m_pedgeBestError != NULL )
			{
				// Show the error for this collapse.
				ASSERT ( m_pptBestError != NULL );
				float fError = pFirstObj->FindCollapseError ( m_pptBestError, m_pedgeBestError );
				sprintf ( strTemp, "Auto-selected collapse error %f", fError );
				m_pFont->DrawText( 2, 80, D3DCOLOR_ARGB(255,255,255,0), strTemp );
			}
			if ( m_bShowSlidingWindowInfo )
			{
				if ( m_pedgeBestError != NULL )
				{
					sprintf ( strTemp, "Best collapse error %f", fBestOfAnyError );
					m_pFont->DrawText( 2, 100, D3DCOLOR_ARGB(255,255,255,0), strTemp );
				}

				sprintf ( strTemp, "Sliding window level %i, error tolerance %f%%", pFirstObj->iCurSlidingWindowLevel, m_fSlidingWindowErrorTolerance * 100.0f );
				m_pFont->DrawText( 2, 120, D3DCOLOR_ARGB(255,255,255,0), strTemp );
			}

			// End the scene.
			m_pd3dDevice->EndScene();
		}


	}
	else
	{
		m_pedgeBestError = NULL;
		m_pptBestError = NULL;

		// Clear the render target
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x00000000f, 1.0f, 0L );

		// Begin the scene
		if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
		{


			m_pd3dDevice->SetTransform( D3DTS_VIEW,       &m_matView );
			m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProjFar );

			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

			m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
			m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

			m_pd3dDevice->SetRenderState ( D3DRS_LIGHTING, TRUE );
			m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
			m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );

			if ( m_bWireframe )
			{
				m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
			}
			else
			{
				m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
			}

			if ( m_bIgnoreBackFaced )
			{
				m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
			}
			else
			{
				m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
			}

			m_pd3dDevice->SetTexture( 0, NULL );

			D3DMATERIAL8 matNormal;
			matNormal.Ambient = D3DXCOLOR ( 1.0f, 1.0f, 1.0f, 0.0f );
			matNormal.Diffuse = D3DXCOLOR ( 1.0f, 1.0f, 1.0f, 0.0f );
			matNormal.Specular = D3DXCOLOR ( 0.5f, 0.5f, 0.5f, 0.0f );
			matNormal.Emissive= D3DXCOLOR ( 0.0f, 0.0f, 0.0f, 0.0f );
			matNormal.Power = 20.f;

			D3DMATERIAL8 matHighlight;
			matHighlight.Ambient = D3DXCOLOR ( 1.0f, 1.0f, 0.0f, 0.0f );
			matHighlight.Diffuse = D3DXCOLOR ( 1.0f, 1.0f, 0.0f, 0.0f );
			matHighlight.Specular = D3DXCOLOR ( 0.5f, 0.5f, 0.0f, 0.0f );
			matHighlight.Emissive= D3DXCOLOR ( 0.0f, 0.0f, 0.5f, 0.0f );
			matHighlight.Power = 20.f;

			m_pd3dDevice->SetMaterial ( &matNormal );

			for ( ObjectInstance *pObjInst = m_ObjectInstRoot.ListNext(); pObjInst != NULL; pObjInst = pObjInst->ListNext() )
			{

				if ( pObjInst == pFirstObjInst )
				{
					// Set the highlighted material.
					m_pd3dDevice->SetMaterial ( &matHighlight );
				}

				Object *pObj = pObjInst->pObj;

				m_pd3dDevice->SetTransform( D3DTS_WORLD,      &pObjInst->matOrn );

				if ( m_bTargetErrorAutoGen )
				{
					// Calculate the required error.
					// This should of course do something clever with the error values that
					// are associated with each collapse. But it doesn't, so it doesn't
					// look very good. You can get the proper LoD thing going quickly
					// using a small (e.g. 32 entry) lookup table that has desired
					// error against number of collapses, and linearly interpolating that
					// is fairly quick, but still takes account of the most important
					// changes in collapse error. But I haven't done that here.

					D3DXVECTOR3 vDelta;
					vDelta.x = pObjInst->matOrn._41;
					vDelta.y = pObjInst->matOrn._42;
					vDelta.z = pObjInst->matOrn._43;
					D3DXVec3TransformCoord ( &vDelta, &vDelta, &m_matView );
					// Whether you like it proportional to distance or distance squared is up to you.
					// Although mathematically, a distance squared measure keeps the triangle size
					// constant, the eye has a sort of "auto zoom" for distant objects, so you
					// tend to notice the problems quicker. Maybe a non-integer power?
					//float fDist = D3DXVec3Length ( &vDelta );
					float fDist = D3DXVec3LengthSq ( &vDelta );
					pObjInst->SetNumCollapses ( (int)( m_fTargetErrorFactor * fDist ) );
				}
				else
				{
					pObjInst->SetNumCollapses ( m_iTargetNumCollapses );
				}

				pObjInst->RenderCurrentObject ( m_pd3dDevice, 0, -1, TRUE );


				if ( pObjInst == pFirstObjInst )
				{
					// Set the normal material again.
					m_pd3dDevice->SetMaterial ( &matNormal );
				}

				if ( g_iMaxNumTrisDrawn > 0 )
				{
					// If showing tri order, just draw one object.
					break;
				}

			}


			// Output statistics
			m_pFont->DrawText( 2,  0, D3DCOLOR_ARGB(255,255,255,0), m_strFrameStats );
			m_pFont->DrawText( 2, 20, D3DCOLOR_ARGB(255,255,255,0), m_strDeviceStats );

			char strTemp[1000];
			sprintf ( strTemp, "Optimised VIPM viewing mode." );
			m_pFont->DrawText( 2, 40, D3DCOLOR_ARGB(255,255,255,0), strTemp );

			sprintf ( strTemp, "Current VIPM type: %s", VIPMTypeName(m_vteCurrentDisplayStyle) );
			m_pFont->DrawText( 2, 60, D3DCOLOR_ARGB(255,255,255,0), strTemp );

			sprintf ( strTemp, "Drew %i tris, %i vertices.", g_iNumOfObjectTrisDrawn, g_iNumOfObjectVertsDrawn );
			m_pFont->DrawText( 2, 80, D3DCOLOR_ARGB(255,255,255,0), strTemp );
			g_iNumOfObjectTrisDrawn = 0;
			g_iNumOfObjectVertsDrawn = 0;

			if ( m_bTargetErrorAutoGen )
			{
				// Auto generating collapse LoD.
				sprintf ( strTemp, "Autogenerate LoD factor: %f", m_fTargetErrorFactor );
				m_pFont->DrawText( 2, 100, D3DCOLOR_ARGB(255,255,255,0), strTemp );
			}
			else
			{
				sprintf ( strTemp, "Direct LoD control: %i", m_iTargetNumCollapses );
				m_pFont->DrawText( 2, 100, D3DCOLOR_ARGB(255,255,255,0), strTemp );
			}

			if ( g_bShowVIPMInfo )
			{
				DWORD dwGlobalMem, dwGlobalAGP;
				DWORD dwInstanceMem, dwInstanceAGP;
				DWORD dwVerts, dwTris, dwVertsTotal;

				OptimisedMeshInstance *pomi = pFirstObjInst->GetOpiMeshInst();
				if ( pomi != NULL )
				{
					pomi->InfoGetGlobal ( &dwGlobalMem, &dwGlobalAGP );
					pomi->InfoGetInstance ( &dwInstanceMem, &dwInstanceAGP, &dwVerts, &dwTris, &dwVertsTotal );

					// Don't include AGP in the totals.
					dwGlobalMem -= dwGlobalAGP;
					dwInstanceMem -= dwInstanceAGP;

					sprintf ( strTemp,	"VIPM info: global mem %ikb, global AGP %ikb, "
										"instance mem %ikb, instance AGP %ikb",
										dwGlobalMem / 1024, dwGlobalAGP / 1024,
										dwInstanceMem / 1024, dwInstanceAGP / 1024
										);
					m_pFont->DrawText( 2, 120, D3DCOLOR_ARGB(255,255,255,0), strTemp );

					sprintf ( strTemp,	"%i verts processed, %i tris drawn, "
										"%f verts per tri %s",
										dwVerts, dwTris,
										(float)dwVerts / (float)dwTris,
										g_bOptimiseVertexOrder ? "(optimised)" : "(not optimised)"
										);
					m_pFont->DrawText( 2, 140, D3DCOLOR_ARGB(255,255,255,0), strTemp );

					// When it says "possible", it's not really possible with any sensible cache - just for reference really.
					sprintf ( strTemp,	"%i verts total. (Im)possible best score %f verts per tri",
										dwVertsTotal,
										(float)dwVertsTotal / (float)dwTris
										);
					m_pFont->DrawText( 2, 160, D3DCOLOR_ARGB(255,255,255,0), strTemp );
				}
			}

			if ( g_iMaxNumTrisDrawn > 0 )
			{
				sprintf ( strTemp,	"Limited to %i tris drawn", g_iMaxNumTrisDrawn );
				m_pFont->DrawText( 2, 180, D3DCOLOR_ARGB(255,255,255,0), strTemp );
			}


			// End the scene.
			m_pd3dDevice->EndScene();
		}

	}





	HRESULT hr;

	// Where's the mouse?
	hr = UpdateInputState( m_hWnd );

	DWORD dwButtons = 0;
#define BUTTON_LEFT		0x01
#define BUTTON_RIGHT	0x02
#define BUTTON_MID		0x04

	if ( g_dims.rgbButtons[0] & 0x80 )
	{
		dwButtons |= BUTTON_LEFT;
	}
	if ( g_dims.rgbButtons[1] & 0x80 )
	{
		dwButtons |= BUTTON_RIGHT;
	}
	if ( g_dims.rgbButtons[2] & 0x80 )
	{
		dwButtons |= BUTTON_MID;
	}



	// Spot going-down and going-up buttons.
	static DWORD dwPreviousButtons = 0;
	DWORD dwButtonsGoneDown = dwButtons & ~dwPreviousButtons;
	DWORD dwButtonsGoneUp = ~dwButtons & dwPreviousButtons;
	dwPreviousButtons = dwButtons;



	// Do we premult or postmult the transform?
	// Premult for most things, postmult for camera.
	bool bPremult = TRUE;

#define PrePost_MatrixMultiply(res,varg,barg) if ( bPremult ) {D3DXMatrixMultiply ( res, barg, varg );} else D3DXMatrixMultiply ( res, varg, barg )
	

	// If shift is not down, but some mouse buttons are, this may be a movement of some sort.
	if ( ( dwButtons != 0 ) && !g_bKeyDownShift )
	{
		if ( !g_bExclusive )
		{
			// Enter exclusive mode.
			SetExclusiveMode ( TRUE, m_hWnd );
		}

		{
			D3DXMATRIX *pmat;
			if ( !g_bKeyDownCtrl )
			{
				// Move the viewer.
				pmat = &m_matView;
				bPremult = FALSE;
			}
			else
			{
				// Ctrl key down - move the first object.
				pmat = &(pFirstObjInst->matOrn);
				bPremult = TRUE;
			}


			D3DXMATRIX  mat1;

			if ( dwButtons == ( BUTTON_LEFT | BUTTON_RIGHT ) )
			{
				// Strafe.
				// Magic numbers are scales.
				float fStrafeScale = 1.0f;
				if ( m_bOrbitStrafe && !g_bKeyDownCtrl )
				{
					// Try to keep looking roughly at the origin.
					// Rotate perpendicular to the strafe movement by an amount
					// proportional to the distance from the origin.
					// Sort-of works :-)
					D3DXVECTOR3 vVect;
					vVect.x = pmat->_41;
					vVect.y = pmat->_42;
					vVect.z = pmat->_43;
					float fDistToOrigin = D3DXVec3Length ( &vVect );
					if ( fDistToOrigin < 0.000001f )
					{
						// Sod it - too close.
						fStrafeScale = 1.0f;
					}
					else
					{
						fStrafeScale = fDistToOrigin;
					}
				}

				D3DXMatrixTranslation ( &mat1,
					(float)g_dims.lX * 0.003f * fStrafeScale,
					(float)g_dims.lY * 0.003f * -fStrafeScale,
					0.0f );
				PrePost_MatrixMultiply ( pmat, pmat, &mat1 );

				if ( m_bOrbitStrafe && !g_bKeyDownCtrl )
				{
					D3DXMatrixRotationY( &mat1, (float)g_dims.lX * -0.003f );
					PrePost_MatrixMultiply ( pmat, pmat, &mat1 );
					D3DXMatrixRotationX( &mat1, (float)g_dims.lY * -0.003f );
					PrePost_MatrixMultiply ( pmat, pmat, &mat1 );
				}
			}
			else if ( dwButtons == BUTTON_LEFT )
			{
				// Look.

				// Magic number is X scale.
				D3DXMatrixRotationY( &mat1, (float)g_dims.lX * 0.003f );
				PrePost_MatrixMultiply ( pmat, pmat, &mat1 );

				// Magic number is Y scale.
				D3DXMatrixRotationX( &mat1, (float)g_dims.lY * 0.003f );
				PrePost_MatrixMultiply ( pmat, pmat, &mat1 );
			}
			else if ( dwButtons == BUTTON_RIGHT )
			{
				// Rotate on X, zoom on Y.

				// Magic number is X scale (rotate).
				D3DXMatrixRotationZ( &mat1, (float)g_dims.lX * 0.003f );
				PrePost_MatrixMultiply ( pmat, pmat, &mat1 );

				float fHowMuch;
				if ( m_bCreateCollapseMode )
				{
					// Finer control when creating collapses.
					fHowMuch = (float)g_dims.lY * -0.01f;
				}
				else
				{
					// Coarser control when viewing.
					fHowMuch = (float)g_dims.lY * -0.1f;
				}
				// Magic number is Y scale (zoom).
				D3DXMatrixTranslation( &mat1, 0.0f, 0.0f, fHowMuch );
				PrePost_MatrixMultiply ( pmat, pmat, &mat1 );
			}
		}
	}
	else
	{
		// Not moving.

		// Make sure we are out of exclusive mode.
		SetExclusiveMode ( FALSE, m_hWnd );


		if ( g_bKeyDownShift && !g_bKeyDownCtrl )
		{
			// Shift is down - maybe do a collapse.
			if ( ( ( dwButtonsGoneDown == BUTTON_LEFT ) && ( dwButtons == BUTTON_LEFT ) ) ||
				 ( ( dwButtonsGoneDown == BUTTON_RIGHT ) && ( dwButtons == BUTTON_RIGHT ) ) )
			{
				if ( m_bCreateCollapseMode )
				{
					// Stop doing any collapses.
					m_iCreateThisManyCollapses = 0;

					// Collapse an edge.
					MeshEdge *pedge;
					MeshPt *ppt;
					if ( dwButtons == BUTTON_LEFT )
					{
						// Collapse the user-specified edge.
						pedge = pClosestEdge;
						ppt = pClosestPt;
					}
					else
					{
						// Collapse auto edge.
						pedge = m_pedgeBestError;
						ppt = m_pptBestError;
					}

					if ( ( pedge != NULL ) && ( ppt != NULL ) )
					{
						BOOL bSomeCollapsesDone = FALSE;
						while ( pFirstObj->DoCollapse() )
						{
							bSomeCollapsesDone = TRUE;
						}

						if ( bSomeCollapsesDone )
						{
							// Can't do this collapse if we weren't already fully collapsed - 
							// the edge may not exist any more!
						}
						else
						{
							MeshPt *pKeptPt = pedge->OtherPt ( ppt );
							ASSERT ( pKeptPt != NULL );
							pFirstObj->CreateEdgeCollapse ( ppt, pKeptPt );

							// Set these to NULL, otherwise the silly thing tries
							// to render non-existent stuff.
							pedge = NULL;
							ppt = NULL;
						}
					}
				}
			}
		}
	}



	// Do we need to do any collapses?
	if ( m_bCreateCollapseMode  && ( m_iCreateThisManyCollapses > 0 ) )
	{
		// Collapse auto-found edge.
		if ( ( m_pedgeBestError != NULL ) && ( m_pptBestError != NULL ) )
		{
			m_bCreateCollapseMode = TRUE;
			// First make sure all collapses have been done.
			BOOL bSomeCollapsesDone = FALSE;
			while ( m_pObject->DoCollapse() )
			{
				bSomeCollapsesDone = TRUE;
			}

			if ( bSomeCollapsesDone )
			{
				// Can't do this collapse if we weren't already fully collapsed - 
				// the edge may not exist any more!
			}
			else
			{
				MeshPt *pKeptPt = m_pedgeBestError->OtherPt ( m_pptBestError );
				ASSERT ( pKeptPt != NULL );
				m_pObject->CreateEdgeCollapse ( m_pptBestError, pKeptPt );
				m_iCreateThisManyCollapses--;
			}

			m_pedgeBestError = NULL;
			m_pptBestError = NULL;
		}
		else
		{
			// There wasn't an edge to do. Stop.
			m_iCreateThisManyCollapses = 0;
		}
	}
	ASSERT ( m_iCreateThisManyCollapses >= 0 );


    return S_OK;
}





// Sets menu items up correctly.
void CMyD3DApplication::SetMenuItems()
{
	// Set menu states
	CheckMenuItem( GetMenu(m_hWnd), IDM_IGNORE_BACKFACING,
		   m_bIgnoreBackFaced ? MF_CHECKED : MF_UNCHECKED );

	CheckMenuItem( GetMenu(m_hWnd), IDM_SLIDING_WINDOW_SHOW,
		   m_bShowSlidingWindowInfo ? MF_CHECKED : MF_UNCHECKED );

	CheckMenuItem( GetMenu(m_hWnd), IDM_CREATE_COLLAPSE_MODE,
		   m_bCreateCollapseMode ? MF_CHECKED : MF_UNCHECKED );


	CheckMenuItem( GetMenu(m_hWnd), IDM_TARGET_AUTO_TOGGLE,
		   m_bTargetErrorAutoGen ? MF_CHECKED : MF_UNCHECKED );

	CheckMenuItem( GetMenu(m_hWnd), IDM_WIREFRAME,
		   m_bWireframe ? MF_CHECKED : MF_UNCHECKED );

	CheckMenuItem( GetMenu(m_hWnd), IDM_SHOW_VIPM_INFO,
		   g_bShowVIPMInfo ? MF_CHECKED : MF_UNCHECKED );

	CheckMenuItem( GetMenu(m_hWnd), IDM_CACHE_DISPLAY_ENABLE,
		   ( g_iMaxNumTrisDrawn > 0 ) ? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( GetMenu(m_hWnd), IDM_CACHE_OPTIMISE,
		   g_bOptimiseVertexOrder ? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( GetMenu(m_hWnd), IDM_CACHE_OPTIMISE_CHEAP,
		   g_bUseFastButBadOptimise ? MF_CHECKED : MF_UNCHECKED );

}


//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects()
{

	m_pFont->InitDeviceObjects( m_pd3dDevice );


	SetMenuItems();


#if ALLOW_PROGRESS_BARS
	g_hWndApp = m_hWnd;
#endif



	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RestoreDeviceObjects()
{

	// Let the outside world know what device we're using.
	g_pd3dDevice = m_pd3dDevice;


	m_pFont->RestoreDeviceObjects();


	// Start up DInput.
	HRESULT hr;
	hr = InitDirectInput( m_hWnd );



	// Set the transform matrices
	D3DXVECTOR3 vEyePt    = D3DXVECTOR3( 0.0f, 0.0f, 5.0f );
	D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	D3DXMATRIX  matWorld;

	D3DXMatrixIdentity( &matWorld );
	D3DXMatrixLookAtLH( &m_matView, &vEyePt, &vLookatPt, &vUpVec );

	// Three different projection matrices for different purposes.
	FLOAT fAspect = m_d3dsdBackBuffer.Width / (FLOAT)m_d3dsdBackBuffer.Height;
	// For close-in work editing collapse orders.
	D3DXMatrixPerspectiveFovLH( &m_matProjClose, D3DX_PI/4, fAspect, 0.1f, 50.0f );
	// For close-in work plus a Zbias for the lines.
	D3DXMatrixPerspectiveFovLH( &m_matProjCloseZbias, D3DX_PI/4, fAspect, 0.1f + 0.001f, 50.0f + 0.001f );
	// For more distant viewing.
	D3DXMatrixPerspectiveFovLH( &m_matProjFar, D3DX_PI/4, fAspect, 1.0f, 500.0f );


	m_pd3dDevice->SetTransform( D3DTS_WORLD,      &matWorld );
	m_pd3dDevice->SetTransform( D3DTS_VIEW,       &m_matView );
	m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProjClose );

	// Set misc render states
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );


	// Make an example object.
	static BOOL bAlreadyDone = FALSE;
	if ( !bAlreadyDone )
	{
		// Only needs doing once, but annoyingly requires a D3D device to do the init.
		bAlreadyDone = TRUE;
		m_pObject->CreateTestObject(m_pd3dDevice);
		m_pObject->MakeCurrentObjectFromPerm();
	}


	// And make a few lights.
	D3DLIGHT8 light;
	light.Type = D3DLIGHT_DIRECTIONAL;
	D3DXCOLOR col = D3DXCOLOR ( 1.0f, 1.0f, 1.0f, 0.0f );
	light.Diffuse  = D3DXCOLOR ( 1.0f, 1.0f, 1.0f, 0.0f );
	light.Ambient  = D3DXCOLOR ( 0.0f, 0.0f, 0.0f, 0.0f );
	light.Specular = D3DXCOLOR ( 0.0f, 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vec ( -1.0f, -1.0f, -1.0f );
	D3DXVec3Normalize ( &vec, &vec );
	light.Direction = vec;
	light.Position = D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f );
	light.Falloff = 1.0f;
	light.Attenuation0 = 0.0f;
	light.Attenuation1 = 1.0f;
	light.Attenuation2 = 0.0f;
	light.Phi = 0.0f;
	light.Theta = 0.0f;

	m_pd3dDevice->SetLight ( 0, &light );
	m_pd3dDevice->LightEnable ( 0, TRUE );

	m_pd3dDevice->SetRenderState ( D3DRS_AMBIENT, 0x00101010 );

	D3DMATERIAL8 mat;
	mat.Ambient = D3DXCOLOR ( 1.0f, 1.0f, 1.0f, 0.0f );
	mat.Diffuse = D3DXCOLOR ( 1.0f, 1.0f, 1.0f, 0.0f );
	mat.Specular = D3DXCOLOR ( 0.5f, 0.5f, 0.5f, 0.0f );
	mat.Emissive= D3DXCOLOR ( 0.0f, 0.0f, 0.0f, 0.0f );
	mat.Power = 20.f;

	m_pd3dDevice->SetMaterial ( &mat );


	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
    m_pFont->InvalidateDeviceObjects();


    FreeDirectInput();

	// Warn things that D3D is leaving now.
	for ( ObjectInstance *pOI = m_ObjectInstRoot.ListNext(); pOI != NULL; pOI = pOI->ListNext() )
	{
		pOI->AboutToChangeDevice();
	}

	// Let the outside world know.
	g_pd3dDevice = NULL;


    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
    m_pFont->DeleteDeviceObjects();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup()
{

	ObjectInstance *pInst = m_ObjectInstRoot.ListNext();
	while ( pInst != NULL )
	{
		delete pInst;
		pInst = m_ObjectInstRoot.ListNext();
	}
	if ( m_pObject != NULL )
	{
		delete m_pObject;
		m_pObject = NULL;
	}

    SAFE_DELETE( m_pFont );
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: Called during device intialization, this code checks the device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::ConfirmDevice( D3DCAPS8* pCaps, DWORD dwBehavior, 
                                          D3DFORMAT Format )
{
	// Whatever - no special properties required.
	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Message proc function to handle key and menu input
//-----------------------------------------------------------------------------
LRESULT CMyD3DApplication::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam,
                                    LPARAM lParam )
{

	if( WM_ACTIVATE == uMsg )
	{
		// Free the mouse.
		SetExclusiveMode ( FALSE, hWnd );

		// sent when window changes active state
		if ( WA_INACTIVE == wParam )
		{
			g_bActive = FALSE;
		}
		else
		{
			g_bActive = TRUE;
		}

		// Set exclusive mode access to the mouse based on active state
		SetAcquire( hWnd );

		return TRUE;
	}
	else if ( ( WM_KEYDOWN == uMsg ) || ( WM_KEYUP == uMsg ) ||
			  ( WM_SYSKEYDOWN == uMsg ) || ( WM_SYSKEYUP == uMsg ) )
	{
		// Top bit (bit31) is current key status.
		if ( wParam == VK_CONTROL )
		{
			g_bKeyDownCtrl = ( ( lParam & 0x80000000 ) == 0 );
		}
		else if ( wParam == VK_SHIFT )
		{
			g_bKeyDownShift = ( ( lParam & 0x80000000 ) == 0 );
		}
	}
    if( uMsg == WM_COMMAND )
    {
        switch( LOWORD(wParam) )
        {
		case IDM_IGNORE_BACKFACING:
			m_bIgnoreBackFaced = !m_bIgnoreBackFaced;
			break;
		case IDM_VIPM_COLLAPSE_NEXT:
			m_iCreateThisManyCollapses = 0;
			if ( m_bCreateCollapseMode )
			{
				m_iFindBestErrorCountdown = 1;
				m_pObject->DoCollapse();
			}
			else
			{
				if ( m_bTargetErrorAutoGen )
				{
					m_fTargetErrorFactor *= 1.1f;
				}
				else
				{
					m_iTargetNumCollapses++;
				}
			}
			break;
		case IDM_VIPM_COLLAPSE_PREV:
			m_iCreateThisManyCollapses = 0;
			if ( m_bCreateCollapseMode )
			{
				m_iFindBestErrorCountdown = 1;
				m_pObject->UndoCollapse();
			}
			else
			{
				if ( m_bTargetErrorAutoGen )
				{
					m_fTargetErrorFactor /= 1.1f;
				}
				else
				{
					m_iTargetNumCollapses--;
					if ( m_iTargetNumCollapses < 0 )
					{
						m_iTargetNumCollapses = 0;
					}
				}
			}
			break;
		case IDM_VIPM_COLLAPSE_NEXT_10:
			m_iCreateThisManyCollapses = 0;
			if ( m_bCreateCollapseMode )
			{
				for ( int i = 10; i > 0; i-- )
				{
					m_pObject->DoCollapse();
				}
				m_iFindBestErrorCountdown = 1;
			}
			else
			{
				if ( m_bTargetErrorAutoGen )
				{
					m_fTargetErrorFactor *= 2.0f;
				}
				else
				{
					m_iTargetNumCollapses += 10;
				}
			}
			break;
		case IDM_VIPM_COLLAPSE_PREV_10:
			m_iCreateThisManyCollapses = 0;
			if ( m_bCreateCollapseMode )
			{
				for ( int i = 10; i > 0; i-- )
				{
					m_pObject->UndoCollapse();
				}
				m_iFindBestErrorCountdown = 1;
			}
			else
			{
				if ( m_bTargetErrorAutoGen )
				{
					m_fTargetErrorFactor /= 2.0f;
				}
				else
				{
					m_iTargetNumCollapses -= 10;
					if ( m_iTargetNumCollapses < 0 )
					{
						m_iTargetNumCollapses = 0;
					}
				}
			}
			break;
		case IDM_VIPM_COLLAPSE_ALL:
			m_iCreateThisManyCollapses = 0;
			if ( m_bCreateCollapseMode )
			{
				while ( m_pObject->DoCollapse() ){}
			}
			else
			{
				if ( m_bTargetErrorAutoGen )
				{
					// No equivalent - reset to standard.
					m_fTargetErrorFactor = 1.0f;
				}
				else
				{
					// No equivalent.
				}
			}
			break;
		case IDM_VIPM_COLLAPSE_NONE:
			m_iCreateThisManyCollapses = 0;
			if ( m_bCreateCollapseMode )
			{
				while ( m_pObject->UndoCollapse() ){}
			}
			else
			{
				if ( m_bTargetErrorAutoGen )
				{
					// No equivalent - reset to standard.
					m_fTargetErrorFactor = 1.0f;
				}
				else
				{
					m_iTargetNumCollapses = 0;
				}
			}
			break;
		case IDM_VIPM_COLLAPSE_DEL:
			m_iCreateThisManyCollapses = 0;
			if ( m_bCreateCollapseMode )
			{
				m_bCreateCollapseMode = TRUE;
				// Do all collapses.
				while ( m_pObject->DoCollapse() ){}
				// Delete the last collapse.
				m_pObject->BinEdgeCollapse();
				m_iFindBestErrorCountdown = 1;
			}
			break;

		case IDM_VIPM_COLLAPSE_DO_BEST:
			if ( m_bCreateCollapseMode )
			{
				if ( m_iCreateThisManyCollapses == 0 )
				{
					// Do a collapse.
					m_iCreateThisManyCollapses = 1;
				}
				else
				{
					// Already doing collapses - stop doing them.
					m_iCreateThisManyCollapses = 0;
				}
			}
			break;
		case IDM_VIPM_COLLAPSE_DO_BEST_ALL:
			if ( m_bCreateCollapseMode )
			{
				if ( m_iCreateThisManyCollapses == 0 )
				{
					// Do all the collapses you can (will automatically stop).
					m_iCreateThisManyCollapses = 1 << 30;
				}
				else
				{
					// Already doing collapses - stop doing them.
					m_iCreateThisManyCollapses = 0;
				}
			}
			break;

		case IDM_SLIDING_WINDOW_SHOW:
			m_bShowSlidingWindowInfo = !m_bShowSlidingWindowInfo;
			break;
		case IDM_CREATE_COLLAPSE_MODE:
			m_bCreateCollapseMode = !m_bCreateCollapseMode;
			// Stop doing any collapses.
			m_iCreateThisManyCollapses = 0;
			break;
		case IDM_SLIDING_WINDOW_INC:
			m_fSlidingWindowErrorTolerance *= 1.1f;
			break;
		case IDM_SLIDING_WINDOW_DEC:
			m_fSlidingWindowErrorTolerance /= 1.1f;
			break;

		case IDM_VIPM_TYPE_NEXT:
			{
				m_vteCurrentDisplayStyle = (VIPMTypeEnum)( ((int)m_vteCurrentDisplayStyle) + 1 );
				if ( m_vteCurrentDisplayStyle == VIPMType_Last )
				{
					m_vteCurrentDisplayStyle = VIPMType_Vanilla;
				}

				for ( ObjectInstance *pOI = m_ObjectInstRoot.ListNext(); pOI != NULL; pOI = pOI->ListNext() )
				{
					pOI->iRenderMethod = m_vteCurrentDisplayStyle;
				}
			}

			break;


		case IDM_TARGET_AUTO_TOGGLE:
			m_bTargetErrorAutoGen = !m_bTargetErrorAutoGen;
			break;

		case IDM_SHOW_VIPM_INFO:
			if ( !m_bCreateCollapseMode )
			{
				// Toggle info on and off.
				g_bShowVIPMInfo = !g_bShowVIPMInfo;
				// And we need to dirty everything.
				for ( ObjectInstance *pOI = m_ObjectInstRoot.ListNext(); pOI != NULL; pOI = pOI->ListNext() )
				{
					pOI->pObj->MarkAsDirty();
				}
			}
			break;

		case IDM_WIREFRAME:
			m_bWireframe = !m_bWireframe;
			break;


		case IDM_CACHE_DISPLAY_ENABLE:
			g_iMaxNumTrisDrawn = -g_iMaxNumTrisDrawn;
			break;

		case IDM_CACHE_OPTIMISE_CHEAP:
			g_bUseFastButBadOptimise = !g_bUseFastButBadOptimise;
			break;

		case IDM_CACHE_DISPLAY_INC:
			if ( g_iMaxNumTrisDrawn > 0 )
			{
				g_iMaxNumTrisDrawn++;
			}
			break;
		case IDM_CACHE_DISPLAY_DEC:
			if ( g_iMaxNumTrisDrawn > 1 )
			{
				g_iMaxNumTrisDrawn--;
			}
			break;
		case IDM_CACHE_DISPLAY_INC_10:
			if ( g_iMaxNumTrisDrawn > 0 )
			{
				g_iMaxNumTrisDrawn += 10;
			}
			break;
		case IDM_CACHE_DISPLAY_DEC_10:
			if ( g_iMaxNumTrisDrawn > 1 )
			{
				g_iMaxNumTrisDrawn -= 10;
				if ( g_iMaxNumTrisDrawn < 1 )
				{
					g_iMaxNumTrisDrawn = 1;
				}
			}
			break;

		case IDM_CACHE_OPTIMISE:
			g_bOptimiseVertexOrder = !g_bOptimiseVertexOrder;
			break;

        }

		SetMenuItems();

    }


	switch ( uMsg )
	{
	case WM_MOUSELEAVE:
	case WM_MOUSEMOVE:
	case WM_MOUSEHOVER:
	case WM_KEYDOWN:
	case WM_KEYUP:
#if 0
	case WM_MOUSELEFTDOWN:
	case WM_MOUSERIGHTDOWN:
	case WM_MOUSELEFTUP:
	case WM_MOUSERIGHTUP:
	case WM_MOUSEWHEEL:
#endif
	case WM_LBUTTONDOWN  :
	case WM_LBUTTONUP    :
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDOWN  :
	case WM_RBUTTONUP    :
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDOWN  :
	case WM_MBUTTONUP    :
	case WM_MBUTTONDBLCLK:
	case WM_NCHITTEST:
	case WM_CONTEXTMENU:
	case WM_SETCURSOR:
		// Don't free the mouse - these happen when moving.
		break;
	default:
		// Free the mouse, just in case.
		SetExclusiveMode ( FALSE, hWnd );
		break;
	}


    return CD3DApplication::MsgProc( hWnd, uMsg, wParam, lParam );
}



