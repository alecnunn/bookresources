
#include "TomsD3DLib.h"


#ifndef STRICT
#define STRICT
#endif
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


#include "Filter.h"
#include "Image.h"







// The vertices of the landscape.
// Might not actually correspond to real verts - they are places to hold filter info.
class FilterVert
{
public:

	// This vert's seed value.
	ui32 u32Seed;
	// Position of the vertex in texels.
	Position2D p2dPosition;

	FilterRoot root;

	float fHeight;

	// A seed to generate seeds used for the creator.
	static ui32 u32DefaultSeed;

	FilterVert ( const Position2D &p2dNewPosition, ui32 u32NewSeed = 0 )
	{
		p2dPosition = p2dNewPosition;
		if ( u32NewSeed == 0 )
		{
			// The app just wants a random one.
			u32DefaultSeed++;
			u32NewSeed = GetRandom ( 0xffff, u32DefaultSeed, 9238476 );
			u32DefaultSeed++;
			u32NewSeed = ( u32NewSeed << 16 ) | GetRandom ( 0xffff, u32DefaultSeed, 2398476 );
		}
		u32Seed = u32NewSeed;

		fHeight = 0.0f;
	}

	~FilterVert ( void )
	{
		// Bin all the filters.
		while ( root.Next() != NULL )
		{
			delete root.Next();
		}
	}
};

/*static*/ ui32 FilterVert::u32DefaultSeed = 12345;




// The distance between two landscape verts (in texels).
// The alpha-splat used by the landscape must be twice this size, or things won't work terribly well.
const int ciLandscapeSpacing = 64;

// The maximum texture size INCLUSIVE.
// So a size of 8 means the landscape textures are 256x256
const int ciMaxTextureSizeLog2 = 8;



// How many vertices across the landscape is.
const int ciMaxSizeLandscape = 256;

FilterVert *aapfvertLand[ciMaxSizeLandscape][ciMaxSizeLandscape];

// How many vertices across a chunk has.
const int ciChunkSize = ( 1 << ciMaxTextureSizeLog2 ) / ciLandscapeSpacing;
// How many chunks across to make up a map.
const int ciNumChunksWidth = ciMaxSizeLandscape / ciChunkSize;






// A scaler to vary the size of mipmaps. Found by tweakage.
float g_fMipmapScaler = 9.0f;

struct Chunk
{
	// Where the chunk starts on the landscape in filter verts
	// i.e. 3,5 means it starts at aapfvertLand[3][5].
	Position2D p2dTlc;

	// The current mipmap level this chunk is at.
	// 0 = 1x1, 1 = 2x2, etc
	// -1 = invisible - distance-culled.
	int iMipmapLevel;

	// Set if this texture needs to be updated.
	bool bNeedsUpdate;

	// The texture for displaying this chunk.
	LPDIRECT3DTEXTURE8 pd3dtex;

	// The list of active (i.e. drawn, having textures) chunks.
	// If a chunk is active, it has a non-NULL pd3dtex.
	Chunk *pchunkActiveNext;

	DWORD dwLastFrameDrawn;

	Chunk ( Position2D p2dNewTlc = Position2D ( 0, 0 ) )
	{
		p2dTlc = p2dNewTlc;
		pd3dtex = NULL;
		pchunkActiveNext = NULL;
	}


	// Returns the desired mipmap level for the given camera position.
	// The position is in texels.
	int FindMipmapLevel ( Position2D p2dCamera )
	{
		Position2D p2dOffset = p2dTlc + Position2D ( ciChunkSize / 2, ciChunkSize / 2 );
		p2dOffset = p2dOffset * ciLandscapeSpacing - p2dCamera;
		// And wrap the coords.
		const int ciLandscapeTexels = ciMaxSizeLandscape * ciLandscapeSpacing;
		p2dOffset.X &= ciLandscapeTexels - 1;
		p2dOffset.Y &= ciLandscapeTexels - 1;
		if ( p2dOffset.X > ciLandscapeTexels / 2 )
		{
			p2dOffset.X -= ciLandscapeTexels;
		}
		if ( p2dOffset.Y > ciLandscapeTexels / 2 )
		{
			p2dOffset.Y -= ciLandscapeTexels;
		}

		float fDist = (float)p2dOffset.X * (float)p2dOffset.X + (float)p2dOffset.Y * (float)p2dOffset.Y;
		fDist = sqrtf ( fDist );
		fDist /= ciLandscapeSpacing;
		// And subtract the size of the tile, because we are interested in the mipmap level of the nearest edge.
		fDist -= ciChunkSize / 2;
		if ( fDist <= 0.0f )
		{
			// Camera is inside the chunk.
			return ciMaxTextureSizeLog2;
		}
		else
		{
			fDist = g_fMipmapScaler - log ( fDist );
			int iMipmap = (int)fDist;
			if ( iMipmap < 0 )
			{
				// Teeny texture.
				return 0;
			}
			else if ( iMipmap > ciMaxTextureSizeLog2 )
			{
				// Huge texture.
				return ciMaxTextureSizeLog2;
			}
			else
			{
				return iMipmap;
			}
		}
	}


} chunkLand[ciNumChunksWidth][ciNumChunksWidth];

Chunk *pchunkActiveFirst;

// An array of lists of textures.
// This is where unused textures live.
ArbitraryList<LPDIRECT3DTEXTURE8> alpd3dtexFree[ciMaxTextureSizeLog2+1];

// A list of images, each of the correct size. These are used as temporaries when compositing.
Image *pimagePrepared[ciMaxTextureSizeLog2+1];



// Where on the landscape the camera is (in vertices).
float fCameraX;
float fCameraY;



enum LandTypes
{
	eltSand = 0,		// First item must be 0.
	eltGrass,
	eltRock,
	eltSnow,

	eltLastItem,		// Must always be last.
};


Image *paimageLand[eltLastItem];
Image *pimageSplat = NULL;



struct FilterAndVertex
{
	DWORD dwLayerNumber;		// For slightly faster sorting.
	Filter		*pfilter;
	FilterVert	*pfvert;
};



// Composites the given image from the list of vertices.
// The first vertex in the list will always "win" a layer fight and get applied first.
// iMaxNumLayers is just for debugging.
// p2dTlc
void CompositeImage ( Image *pres, const ArbitraryList<FilterVert*> &lpfverts, Position2D p2dTlc, int iMaxNumLayers = 0x7fffffff )
{
	ArbitraryList<FilterAndVertex> lfavWorking1;
	ArbitraryList<FilterAndVertex> lfavWorking2;
	ArbitraryList<FilterAndVertex> *plfavDst = &lfavWorking1;
	ArbitraryList<FilterAndVertex> *plfavSrc = &lfavWorking2;
	ArbitraryList<FilterAndVertex> *plfavTmp;
	lfavWorking1.SizeTo ( 0 );
	lfavWorking2.SizeTo ( 0 );

	for ( int iVert = 0; iVert < lpfverts.Size(); iVert++ )
	{
		const FilterVert &vert = *(lpfverts[iVert]);
		// How many filters?
		int iNumFilters = 0;
		for ( Filter *pfilter = vert.root.Next(); pfilter != NULL; pfilter = pfilter->Next() )
		{
			iNumFilters++;
		}
		if ( iNumFilters == 0 )
		{
			// A vert with no filters. Odd, but not completely insane.
			continue;
		}

		// Add this vert's filters to the current list, sorted correctly.
		plfavDst->SizeTo ( plfavSrc->Size() + iNumFilters );

		int iCurSrc = 0;
		int iCurDst = 0;
		int iNumSrcLeft = plfavSrc->Size();
		Filter *pfilter = vert.root.Next();
		while ( true )
		{
			if ( pfilter == NULL )
			{
				// Run out of new filters to add.
				if ( iNumSrcLeft > 0 )
				{
					// And there's more source filters to add.
					plfavDst->CopyFrom ( iCurDst, *plfavSrc, iCurSrc, iNumSrcLeft );
				}
				break;
			}
			if ( ( iNumSrcLeft == 0 ) || ( pfilter->dwLayerNumber >= (plfavSrc->Item(iCurSrc))->dwLayerNumber ) )
			{
				// Add this new record.
				FilterAndVertex &fav = *(plfavDst->Item(iCurDst));
				fav.dwLayerNumber = pfilter->dwLayerNumber;
				fav.pfilter = pfilter;
				fav.pfvert = (FilterVert* /*ditch the annoying const*/) &vert;
				iCurDst++;
				pfilter = pfilter->Next();
			}
			else
			{
				// Copy the existing record.
				*(plfavDst->Item(iCurDst)) = *(plfavSrc->Item(iCurSrc));
				iCurDst++;
				iCurSrc++;
				iNumSrcLeft--;
			}
		}

		// Now swap the previous and current lists over.
		plfavTmp = plfavDst;
		plfavDst = plfavSrc;
		plfavSrc = plfavTmp;
	}

	// Now the filters are sorted, render them.
	// Apply them in reverse order, since they are listed "topmost" first.
#ifdef _DEBUG
	DWORD dwLayerNumber = 0;
#endif
	for ( int i = plfavSrc->Size() - 1; i >= 0; i-- )
	{
#ifdef _DEBUG
		iMaxNumLayers--;
		if ( iMaxNumLayers < 0 )
		{
			break;
		}
#endif
		const FilterAndVertex &fav = *(plfavSrc->Item(i));
#ifdef _DEBUG
		ASSERT ( dwLayerNumber <= fav.dwLayerNumber );
		dwLayerNumber = fav.dwLayerNumber;
#endif
		Position2D p2dRelative = fav.pfvert->p2dPosition - p2dTlc;
		p2dRelative.X &= ( 2 << ciMaxTextureSizeLog2 ) - 1;
		p2dRelative.Y &= ( 2 << ciMaxTextureSizeLog2 ) - 1;
		fav.pfilter->Render ( pres, p2dRelative >> pres->iSizeLog2, fav.pfvert->u32Seed );
	}
}



int giNextSplatLayer = 0;



void AddSplat ( Pixel pixColour, Position2D p2dTexelPosition )
{
	giNextSplatLayer++;

	// Which vertex?
	p2dTexelPosition.X &= ( ( ciLandscapeSpacing * ciMaxSizeLandscape ) - 1 );
	p2dTexelPosition.Y &= ( ( ciLandscapeSpacing * ciMaxSizeLandscape ) - 1 );
	int iX = ( p2dTexelPosition.X + ( ciLandscapeSpacing >> 1 ) ) / ciLandscapeSpacing;
	int iY = ( p2dTexelPosition.Y + ( ciLandscapeSpacing >> 1 ) ) / ciLandscapeSpacing;
	iX &= ciMaxSizeLandscape - 1;
	iY &= ciMaxSizeLandscape - 1;
	FilterVert *pfvert = aapfvertLand[iX][iY];
	iX /= ciChunkSize;
	iY /= ciChunkSize;
	iX &= ciNumChunksWidth - 1;
	iY &= ciNumChunksWidth - 1;
	chunkLand[(iX-0) & (ciNumChunksWidth-1)][(iY-0) & (ciNumChunksWidth-1)].bNeedsUpdate = true;
	chunkLand[(iX-1) & (ciNumChunksWidth-1)][(iY-0) & (ciNumChunksWidth-1)].bNeedsUpdate = true;
	chunkLand[(iX-0) & (ciNumChunksWidth-1)][(iY-1) & (ciNumChunksWidth-1)].bNeedsUpdate = true;
	chunkLand[(iX-1) & (ciNumChunksWidth-1)][(iY-1) & (ciNumChunksWidth-1)].bNeedsUpdate = true;


	p2dTexelPosition.X += ciLandscapeSpacing/2;
	p2dTexelPosition.Y += ciLandscapeSpacing/2;
	p2dTexelPosition.X &= ciLandscapeSpacing - 1;
	p2dTexelPosition.Y &= ciLandscapeSpacing - 1;
	p2dTexelPosition.X -= ciLandscapeSpacing/2;
	p2dTexelPosition.Y -= ciLandscapeSpacing/2;
	new FilterAlphaColourLayer ( &(pfvert->root), giNextSplatLayer, pimageSplat, pixColour, p2dTexelPosition );
}




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
BOOL					g_bKeyDownL		= FALSE;
BOOL					g_bKeyDownR		= FALSE;
BOOL					g_bKeyDownU		= FALSE;
BOOL					g_bKeyDownD		= FALSE;

BOOL					g_bHelpTextOn	= TRUE;


DIMOUSESTATE2			g_dims;      // DirectInput mouse state structure


bool g_bInitDone = false;

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

	BOOL				m_bWireframe;

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
    m_strWindowTitle  = _T("Impostor tester");
    m_bUseDepthBuffer = TRUE;

    m_pFont                = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );

	m_bWireframe = FALSE;

}




//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::OneTimeSceneInit()
{
	D3DXMATRIX mat;
	D3DXMatrixIdentity ( &mat );

	g_bInitDone = false;

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



	// The number of updates allowed this frame for a given size.
	// This allows some primitive throttling to keep the performance up to a decent level.
	// Otherwise what can happen is you get a vicious feedback loop - a bit of updates
	// means a slower frame time, which means greater movement between frames, which
	// means more updates, which means even slower framerates, etc.
	int iNumUpdatesAllowed[ciMaxTextureSizeLog2+1];
	for ( int i = 0; i <= ciMaxTextureSizeLog2; i++ )
	{
		// Should make it inversely proportional to number of texels, since that is the expensive bit.
		//iNumUpdatesAllowed[i] = ( 1 << ( i * 2 ) );
		// However, to show it working (because otherwise it's quite hard to see)
		// We just limit it to one each.
		iNumUpdatesAllowed[i] = 1;
	}





	static DWORD dwCurFrameNumber = 0;
	dwCurFrameNumber++;


	Position2D p2dCamera = Position2D ( (int)( fCameraX * ciLandscapeSpacing ), (int)( fCameraY * ciLandscapeSpacing ) );
	// Scan the active chunks. If any have mipmap levels different to their current ones,
	// free up the textures and remove them from the active list.
	// If they simply changed levels, they will be re-added in the next sweep.
	Chunk *pchunkCur = pchunkActiveFirst;
	Chunk **ppchunkPrev = &pchunkActiveFirst;
	DWORD dwLastFrameNumber = dwCurFrameNumber - 1;
	while ( pchunkCur != NULL )
	{
		ASSERT ( *ppchunkPrev == pchunkCur );
		int iNewMipmapLevel = pchunkCur->FindMipmapLevel ( p2dCamera );
		if ( dwLastFrameNumber != pchunkCur->dwLastFrameDrawn )
		{
			// Wasn't drawn last frame - bin it.
			iNewMipmapLevel = -2;
		}

		if ( iNewMipmapLevel != pchunkCur->iMipmapLevel )
		{
			// Mipmap level is not what we would like - needs an update.
			iNumUpdatesAllowed[iNewMipmapLevel]--;
			if ( ( iNewMipmapLevel >= 0 ) && ( pchunkCur->iMipmapLevel >= 0 ) && ( iNumUpdatesAllowed[iNewMipmapLevel] < 0 ) )
			{
				// No more updates of this size allowed this frame.
				ppchunkPrev = &(pchunkCur->pchunkActiveNext);
				pchunkCur = pchunkCur->pchunkActiveNext;
			}
			else
			{
				// Go for the update.
				Chunk *pchunkThis = pchunkCur;
				pchunkCur = pchunkCur->pchunkActiveNext;
				// Return this texture to the unused list.
				ASSERT ( pchunkThis->iMipmapLevel >= 0 );
				ASSERT ( pchunkThis->iMipmapLevel <= ciMaxTextureSizeLog2 );
				*(alpd3dtexFree[pchunkThis->iMipmapLevel].AddItem()) = pchunkThis->pd3dtex;
				pchunkThis->pd3dtex = NULL;
				pchunkThis->iMipmapLevel = -1;

				// Unlink this from the active list.
				*ppchunkPrev = pchunkThis->pchunkActiveNext;
				pchunkThis->pchunkActiveNext = NULL;
			}
		}
		else
		{
			ppchunkPrev = &(pchunkCur->pchunkActiveNext);
			pchunkCur = pchunkCur->pchunkActiveNext;
		}
	}


	static bool bShiftPressedLastTime = false;
	bool bUpdateAll = false;
	if ( !!bShiftPressedLastTime != !!g_bKeyDownShift )
	{
		bUpdateAll = true;
	}
	bShiftPressedLastTime = !!g_bKeyDownShift;


	// Now sweep through the chunks around the camera, creating and updating them if necessary.
	// Note we do NOT care about the camera direction. If we did, then turning the view would get unexpectedly chuggy.
	const int ciVisualRange = 6;
	int iXMid = p2dCamera.X / ( ciChunkSize * ciLandscapeSpacing );
	int iYMid = p2dCamera.Y / ( ciChunkSize * ciLandscapeSpacing );
	int iXStart = iXMid - ciVisualRange;
	int iXEnd   = iXMid + ciVisualRange;
	int iYStart = iYMid - ciVisualRange;
	int iYEnd   = iYMid + ciVisualRange;
	for ( int iX = iXStart; iX < iXEnd; iX++ )
	{
		for ( int iY = iYStart; iY < iYEnd; iY++ )
		{
			// Find the chunk using wrapping coords.
			int iXPos = iX & (ciNumChunksWidth-1);
			int iYPos = iY & (ciNumChunksWidth-1);
			Chunk *pchunkCur = &(chunkLand[iXPos][iYPos]);
			if ( pchunkCur->iMipmapLevel > -1 )
			{
				// This should have a texture and be all ready to go them.
				ASSERT ( pchunkCur->pd3dtex != NULL );
				if ( bUpdateAll )
				{
					pchunkCur->bNeedsUpdate = true;
				}
			}
			else
			{
				// Definately shouldn't be in the active list.
				ASSERT ( pchunkCur->pchunkActiveNext == NULL );
				ASSERT ( pchunkCur->pd3dtex == NULL );
				// So add it.
				pchunkCur->pchunkActiveNext = pchunkActiveFirst;
				pchunkActiveFirst = pchunkCur;

				// How big a texture do we need?
				pchunkCur->iMipmapLevel = pchunkCur->FindMipmapLevel ( p2dCamera );

				// Get an unused texture of the right size.
				ASSERT ( pchunkCur->iMipmapLevel >= 0 );
				ASSERT ( pchunkCur->iMipmapLevel <= ciMaxTextureSizeLog2 );
				if ( alpd3dtexFree[pchunkCur->iMipmapLevel].Size() > 0 )
				{
					// Got one lying around already.
					pchunkCur->pd3dtex = alpd3dtexFree[pchunkCur->iMipmapLevel][0];
					alpd3dtexFree[pchunkCur->iMipmapLevel].RemoveItem(0);
				}
				else
				{
					// Oh, we're out of this size. I'll just make another one...
					int iSize = 1 << pchunkCur->iMipmapLevel;
					HRESULT hres = D3DXCreateTexture (
						m_pd3dDevice,
						iSize, iSize,
						D3DX_DEFAULT,
						0,
						D3DFMT_A8R8G8B8,
						D3DPOOL_MANAGED,
						&(pchunkCur->pd3dtex)
						);
					ASSERT ( SUCCEEDED ( hres ) );
				}
				// And it'll need updating.
				pchunkCur->bNeedsUpdate = true;
			}

			pchunkCur->dwLastFrameDrawn = dwCurFrameNumber;

			if ( pchunkCur->bNeedsUpdate )
			{
				// Grab the right sized image.
				ASSERT ( pchunkCur->iMipmapLevel >= 0 );
				ASSERT ( pchunkCur->iMipmapLevel <= ciMaxTextureSizeLog2 );
				Image *pimage = pimagePrepared[pchunkCur->iMipmapLevel];


				if ( g_bKeyDownShift )
				{
					// False colour according to texture size.
					Pixel pix;
					switch ( pimage->iSizeLog2 )
					{
					// Spectrum colours (well, backwards).
					case 0: pix = Pixel ( 0xffffffff ); break;
					case 1: pix = Pixel ( 0xffffff00 ); break;
					case 2: pix = Pixel ( 0xff00ffff ); break;
					case 3: pix = Pixel ( 0xff00ff00 ); break;
					case 4: pix = Pixel ( 0xffff00ff ); break;
					case 5: pix = Pixel ( 0xffff0000 ); break;
					case 6: pix = Pixel ( 0xff0000ff ); break;
					default: pix = Pixel ( 0xff000000 ); break;
					}

					Pixel *ppixDst = pimage->ppixLevels[pimage->iNumLevels];
					for ( int i = 1 << ( pimage->iNumLevels * 2 ); i > 0; i-- )
					{
						*ppixDst++ = pix;
					}
					pimage->GenerateMipmaps();
				}
				else
				{
					// Make a list of the vertices in the chunk.
					ArbitraryList<FilterVert*> lpfvert;
					for ( int i = 0; i <= ciChunkSize; i++ )
					{
						for ( int j = 0; j <= ciChunkSize; j++ )
						{
							int iX = ( i + pchunkCur->p2dTlc.X ) & ( ciMaxSizeLandscape - 1 );
							int iY = ( j + pchunkCur->p2dTlc.Y ) & ( ciMaxSizeLandscape - 1 );
							*(lpfvert.AddItem()) = aapfvertLand[iX][iY];
						}
					}
					CompositeImage ( pimage, lpfvert, pchunkCur->p2dTlc * ciLandscapeSpacing );
				}
				pimage->CopyToD3DTex ( pchunkCur->pd3dtex );
				pchunkCur->bNeedsUpdate = false;
			}
		}
	}





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

		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP  );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP  );

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



		m_pd3dDevice->SetVertexShader ( CUSTOMVERTEX_FVF );
		m_pd3dDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );
		m_pd3dDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_NONE );


		int iXMid = p2dCamera.X / ( ciChunkSize * ciLandscapeSpacing );
		int iYMid = p2dCamera.Y / ( ciChunkSize * ciLandscapeSpacing );
		int iXStart = iXMid - ciVisualRange;
		int iXEnd   = iXMid + ciVisualRange;
		int iYStart = iYMid - ciVisualRange;
		int iYEnd   = iYMid + ciVisualRange;
		for ( int iX = iXStart; iX < iXEnd; iX++ )
		{
			for ( int iY = iYStart; iY < iYEnd; iY++ )
			{
				// Find the chunk using wrapping coords.
				Chunk *pchunkCur = &(chunkLand[iX & (ciNumChunksWidth-1)][iY & (ciNumChunksWidth-1)]);

				ASSERT ( pchunkCur->pd3dtex != NULL );

				m_pd3dDevice->SetTexture( 0, pchunkCur->pd3dtex );

				for ( int iX2 = 0; iX2 < ciChunkSize; iX2++ )
				{
					for ( int iY2 = 0; iY2 < ciChunkSize; iY2++ )
					{
						int iXPos = iX * ciChunkSize + iX2;
						int iYPos = iY * ciChunkSize + iY2;
						float fXPos = iXPos - fCameraX;
						float fYPos = iYPos - fCameraY;

						CUSTOMVERTEX vert[4];

						float fU = (float)iX2 / (float)ciChunkSize;
						float fV = (float)iY2 / (float)ciChunkSize;
						const float cfUVOffset = 1.0f / (float)ciChunkSize;

						vert[0].v.x = fXPos;
						vert[0].v.y = fYPos;
						vert[0].v.z = aapfvertLand[(iXPos+0) & (ciMaxSizeLandscape-1)][(iYPos+0) & (ciMaxSizeLandscape-1)]->fHeight;
						vert[0].tu = fU;
						vert[0].tv = fV;
						vert[0].diffuse = 0xffffffff;
						vert[0].specular = 0;

						vert[1].v.x = fXPos + 1.0f;
						vert[1].v.y = fYPos;
						vert[1].v.z = aapfvertLand[(iXPos+1) & (ciMaxSizeLandscape-1)][(iYPos+0) & (ciMaxSizeLandscape-1)]->fHeight;
						vert[1].tu = fU + cfUVOffset;
						vert[1].tv = fV;
						vert[1].diffuse = 0xffffffff;
						vert[1].specular = 0;

						vert[2].v.x = fXPos + 1.0f;
						vert[2].v.y = fYPos + 1.0f;
						vert[2].v.z = aapfvertLand[(iXPos+1) & (ciMaxSizeLandscape-1)][(iYPos+1) & (ciMaxSizeLandscape-1)]->fHeight;
						vert[2].tu = fU + cfUVOffset;
						vert[2].tv = fV + cfUVOffset;
						vert[2].diffuse = 0xffffffff;
						vert[2].specular = 0;

						vert[3].v.x = fXPos;
						vert[3].v.y = fYPos + 1.0f;
						vert[3].v.z = aapfvertLand[(iXPos+0) & (ciMaxSizeLandscape-1)][(iYPos+1) & (ciMaxSizeLandscape-1)]->fHeight;
						vert[3].tu = fU;
						vert[3].tv = fV + cfUVOffset;
						vert[3].diffuse = 0xffffffff;
						vert[3].specular = 0;

						m_pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLEFAN, 2, vert, sizeof(vert[0]) );
					}
				}
			}
		}




		// Output statistics
		m_pFont->DrawText( 2,  0, D3DCOLOR_ARGB(255,255,255,0), m_strFrameStats );
		m_pFont->DrawText( 2, 20, D3DCOLOR_ARGB(255,255,255,0), m_strDeviceStats );

		char strTemp[1000];

		if ( g_bHelpTextOn )
		{
			m_pFont->DrawText( 2,  80, D3DCOLOR_ARGB(255,255,255,0), "H - toggle this text on and off" );
			m_pFont->DrawText( 2, 100, D3DCOLOR_ARGB(255,255,255,0), "Cursor keys - move viewer around landscape" );
			m_pFont->DrawText( 2, 120, D3DCOLOR_ARGB(255,255,255,0), "Ctrl - put down a splat at the viewer's position" );
			m_pFont->DrawText( 2, 140, D3DCOLOR_ARGB(255,255,255,0), "Shift - show mipmap levels" );
			m_pFont->DrawText( 2, 160, D3DCOLOR_ARGB(255,255,255,0), "Left mouse button - look around" );
			m_pFont->DrawText( 2, 180, D3DCOLOR_ARGB(255,255,255,0), "Right mouse button - zoom & rotate" );
			m_pFont->DrawText( 2, 200, D3DCOLOR_ARGB(255,255,255,0), "Both mouse buttons - orbit" );
		}

		// End the scene.
		m_pd3dDevice->EndScene();
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
	

	if ( dwButtons != 0 )
	{
		if ( !g_bExclusive )
		{
			// Enter exclusive mode.
			SetExclusiveMode ( TRUE, m_hWnd );
		}

		{
			D3DXMATRIX *pmat;
			{
				// Move the viewer.
				pmat = &m_matView;
				bPremult = FALSE;
			}


			D3DXMATRIX  mat1;

			if ( dwButtons == ( BUTTON_LEFT | BUTTON_RIGHT ) )
			{
				// Strafe.
				// Magic numbers are scales.
				float fStrafeScale = 1.0f;
				if ( m_bOrbitStrafe )
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
						// Too close.
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

				if ( m_bOrbitStrafe )
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
				fHowMuch = (float)g_dims.lY * -0.1f;

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
	}


	static DWORD dwLastTime = timeGetTime();
	int iTimeInterval = (DWORD)( timeGetTime() - dwLastTime );
	dwLastTime += iTimeInterval;
	float fScale = 0.01f * (float)iTimeInterval;
	if ( g_bKeyDownL )
	{
		fCameraX -= fScale;
	}
	if ( g_bKeyDownR )
	{
		fCameraX += fScale;
	}
	if ( g_bKeyDownD )
	{
		fCameraY -= fScale;
	}
	if ( g_bKeyDownU )
	{
		fCameraY += fScale;
	}

	if ( g_bKeyDownCtrl )
	{
		// Put down a splat at the current camera pos.
		// Note - a stunningly expensive thing to do, as it requires an update of four of
		// the highest-rez textures (since it is at the camera position).
		// Fortunately, a real game will have splats (e.g. from bullets, explosions, etc)
		// happening all over the place.
		// The other suggestion is to create a standard polygonal decal, and only make it a splat
		// when the texture is no longer visible, and not much else is happening.

		Pixel pix;
		int iType = GetRandom ( 3, giNextSplatLayer, 38463 );
		switch ( iType )
		{
		case 0: pix = Pixel ( 0x00ff0000 ); break;
		case 1: pix = Pixel ( 0x00000000 ); break;
		case 2: pix = Pixel ( 0x00800000 ); break;
		default:
			ASSERT ( FALSE );
		}

		Position2D pos ( (int)( fCameraX * ciLandscapeSpacing ), (int)( fCameraY * ciLandscapeSpacing ) );
		AddSplat ( pix, pos );
	}



    return S_OK;
}





// Sets menu items up correctly.
void CMyD3DApplication::SetMenuItems()
{
	// Set menu states
	CheckMenuItem( GetMenu(m_hWnd), IDM_WIREFRAME,
		   m_bWireframe ? MF_CHECKED : MF_UNCHECKED );
}


//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects()
{

	m_pFont->InitDeviceObjects( m_pd3dDevice );


	SetMenuItems();



	if ( !g_bInitDone )
	{
		// This has to be done here because the image loaders need a D3D device.
		g_bInitDone = true;

		Image::ChangeD3DDevice ( m_pd3dDevice );

		// Load some images.
		paimageLand[eltSand ] = Image::LoadFromFile ( "sand.bmp" );
		paimageLand[eltRock ] = Image::LoadFromFile ( "rock.bmp" );
		paimageLand[eltSnow ] = Image::LoadFromFile ( "snow.bmp" );
		paimageLand[eltGrass] = Image::LoadFromFile ( "grass.bmp" );

		pimageSplat = Image::LoadFromFile ( "splat.bmp", true );


		ui32 u32MySeed = 349634;
		// Create the verts.
		for ( int i = 0; i < ciMaxSizeLandscape; i++ )
		{
			for ( int j = 0; j < ciMaxSizeLandscape; j++ )
			{
				aapfvertLand[i][j] = new FilterVert ( Position2D ( i * ciLandscapeSpacing, j * ciLandscapeSpacing ) );
			}
		}

		// Do a cheap'n'cheesy midpoint subdivision fractal landscape.
		aapfvertLand[0][0]->fHeight = 0.0f;
		const float cfScale = 0.3f;

		const int ciStartGran = ciMaxSizeLandscape >> 2;
		float fScale = cfScale * ciStartGran;
		for ( int iX1 = 0; iX1 < ciMaxSizeLandscape; iX1 += ciStartGran )
		{
			for ( int iY1 = 0; iY1 < ciMaxSizeLandscape; iY1 += ciStartGran )
			{
				aapfvertLand[iX1][iY1]->fHeight = ( fScale / 256.0f ) * (float)( GetRandom ( 256, u32MySeed, 6439 ) - 128 );
				u32MySeed += 23598726;
			}
		}

		for ( int iGran = ciStartGran >> 1; iGran >= 2; iGran >>= 1 )
		{
			int iHalfGran = iGran >> 1;
			fScale *= 0.4f;
			for ( int iX1 = 0; iX1 < ciMaxSizeLandscape; iX1 += iGran )
			{
				for ( int iY1 = 0; iY1 < ciMaxSizeLandscape; iY1 += iGran )
				{
					FilterVert *apfvert[3][3];
					for ( int iX2 = 0; iX2 < 3; iX2++ )
					{
						for ( int iY2 = 0; iY2 < 3; iY2++ )
						{
							apfvert[iX2][iY2] = aapfvertLand
								[ ( iX1 + iX2 * iHalfGran ) & ( ciMaxSizeLandscape - 1 ) ]
								[ ( iY1 + iY2 * iHalfGran ) & ( ciMaxSizeLandscape - 1 ) ];
						}
					}

					apfvert[0][1]->fHeight = 0.5f * ( apfvert[0][0]->fHeight + apfvert[0][2]->fHeight );
					apfvert[2][1]->fHeight = 0.5f * ( apfvert[2][0]->fHeight + apfvert[2][2]->fHeight );
					apfvert[1][0]->fHeight = 0.5f * ( apfvert[0][0]->fHeight + apfvert[2][0]->fHeight );
					apfvert[1][2]->fHeight = 0.5f * ( apfvert[0][2]->fHeight + apfvert[2][2]->fHeight );
					apfvert[1][1]->fHeight = 0.5f * ( apfvert[1][0]->fHeight + apfvert[1][2]->fHeight );

					apfvert[0][1]->fHeight += ( fScale / 256.0f ) * (float)( GetRandom ( 256, u32MySeed, 2943562 ) - 128 );
					apfvert[2][1]->fHeight += ( fScale / 256.0f ) * (float)( GetRandom ( 256, u32MySeed, 4573732 ) - 128 );
					apfvert[1][0]->fHeight += ( fScale / 256.0f ) * (float)( GetRandom ( 256, u32MySeed, 2035629 ) - 128 );
					apfvert[1][2]->fHeight += ( fScale / 256.0f ) * (float)( GetRandom ( 256, u32MySeed, 9135585 ) - 128 );
					apfvert[1][1]->fHeight += ( fScale / 256.0f ) * (float)( GetRandom ( 256, u32MySeed, 3259862 ) - 128 );
					u32MySeed += 65843;
				}
			}
		}


		giNextSplatLayer = 6;


		for ( int i = 0; i < ciMaxSizeLandscape; i++ )
		{
			for ( int j = 0; j < ciMaxSizeLandscape; j++ )
			{
				// OK, what layer is this landscape going to be?
				// Layers from 1 to 4 (0 is reserved).
				int iLayer = ( i & 1 ) + ( ( j & 1 ) << 1 ) + 1;

#if 0
				// Pick an image
				int iLandType = GetRandom ( eltLastItem, u32MySeed, 4297 );
#else
				int iLandType = 0;
				if ( aapfvertLand[i][j]->fHeight < 10.0f * cfScale )
				{
					iLandType = 1;
				}
				if ( aapfvertLand[i][j]->fHeight < 0.0f * cfScale )
				{
					iLandType = 2;
				}
				if ( aapfvertLand[i][j]->fHeight < -10.0f * cfScale )
				{
					iLandType = 3;
				}
#endif

				// Make the filter and add it to the vert.
				Filter *pfilter = NULL;
				if ( iLayer == 1 )
				{
					pfilter = new FilterLandscapeLayerBase ( &(aapfvertLand[i][j]->root), iLayer, paimageLand[iLandType] );
				}
				else if ( iLayer == 4 )
				{
					pfilter = new FilterLandscapeLayerFinal ( &(aapfvertLand[i][j]->root), iLayer, paimageLand[iLandType] );
				}
				else
				{
					pfilter = new FilterLandscapeLayerAlpha ( &(aapfvertLand[i][j]->root), iLayer, paimageLand[iLandType] );
				}


#if 0
				// And add some random splats to the vertex.
				int iNumSplats = GetRandom ( 270, u32MySeed, 3494 );
				iLayer = 5;
				iNumSplats >>= 8;
				for ( int k = 0; k < iNumSplats; k++ )
				{
					iLayer += 1 + GetRandom ( 50, u32MySeed, 385742 );
					// They're all red for now.
					Pixel pix;
					int iType = GetRandom ( 3, u32MySeed, 34896 );
					switch ( iType )
					{
					case 0: pix = Pixel ( 0x00ff0000 ); break;
					case 1: pix = Pixel ( 0x00000000 ); break;
					case 2: pix = Pixel ( 0x00800000 ); break;
					default:
						ASSERT ( FALSE );
					}

					Position2D pos (
						GetRandom ( ciLandscapeSpacing, u32MySeed, 934867 ) - ciLandscapeSpacing / 2,
						GetRandom ( ciLandscapeSpacing, u32MySeed, 349856 ) - ciLandscapeSpacing / 2 );
					pfilter = new FilterAlphaColourLayer ( &(aapfvertLand[i][j]->root), iLayer, pimageSplat, pix, pos );
					u32MySeed += 563;

					if ( giNextSplatLayer < iLayer )
					{
						giNextSplatLayer = iLayer;
					}
				}
#endif

				u32MySeed += 94638;
			}
		}

		// Create the chunks.
		for ( int i = 0; i < ciNumChunksWidth; i++ )
		{
			for ( int j = 0; j < ciNumChunksWidth; j++ )
			{
				chunkLand[i][j].p2dTlc = Position2D ( i * ciChunkSize, j * ciChunkSize );
				chunkLand[i][j].iMipmapLevel = -1;
				chunkLand[i][j].pchunkActiveNext = NULL;
				chunkLand[i][j].pd3dtex = NULL;
				chunkLand[i][j].bNeedsUpdate = false;
			}
		}
		pchunkActiveFirst = NULL;

		fCameraX = 0.0f;
		fCameraY = 0.0f;

		for ( int i = 0; i <= ciMaxTextureSizeLog2; i++ )
		{
			pimagePrepared[i] = new Image ( i, ciMaxTextureSizeLog2 - i );
		}


		// Add some grafitti.
		Image *pimage = Image::LoadFromFile ( "scribble.bmp" );
		Pixel *ppix = pimage->ppixLevels[pimage->iNumLevels];
		for ( int iX = 0; iX < 1 << pimage->iNumLevels; iX++ )
		{
			for ( int iY = 0; iY < 1 << pimage->iNumLevels; iY++ )
			{
				if ( ( ppix->dword & 0xffffff ) != 0x000000 )
				{
					Position2D pos ( iY * 15 + 1800, 500 - iX * 15 );
					AddSplat ( *ppix, pos );
				}
				ppix++;
			}
		}
	}


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
	D3DXVECTOR3 vEyePt    = D3DXVECTOR3( 0.0f, -40.0f, -20.0f );
	D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 10.0f );
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
    SAFE_DELETE( m_pFont );


	for ( int i = 0; i < ciMaxSizeLandscape; i++ )
	{
		for ( int j = 0; j < ciMaxSizeLandscape; j++ )
		{
			SAFE_DELETE ( aapfvertLand[i][j] );
		}
	}



	for ( int i = 0; i < eltLastItem; i++ )
	{
		SAFE_DELETE ( paimageLand[i] );
	}
	SAFE_DELETE ( pimageSplat );


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
		else if ( wParam == VK_LEFT )
		{
			g_bKeyDownL = ( ( lParam & 0x80000000 ) == 0 );
		}
		else if ( wParam == VK_RIGHT )
		{
			g_bKeyDownR = ( ( lParam & 0x80000000 ) == 0 );
		}
		else if ( wParam == VK_UP )
		{
			g_bKeyDownU = ( ( lParam & 0x80000000 ) == 0 );
		}
		else if ( wParam == VK_DOWN )
		{
			g_bKeyDownD = ( ( lParam & 0x80000000 ) == 0 );
		}
		else if ( wParam == 'H' )
		{
			if ( ( lParam & 0x80000000 ) == 0 )
			{
				g_bHelpTextOn = !g_bHelpTextOn;
			}
		}
		
	}
    if( uMsg == WM_COMMAND )
    {
        switch( LOWORD(wParam) )
        {
		case IDM_WIREFRAME:
			m_bWireframe = !m_bWireframe;
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




