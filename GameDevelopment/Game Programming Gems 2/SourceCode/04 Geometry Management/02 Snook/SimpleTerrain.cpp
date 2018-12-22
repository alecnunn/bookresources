/* Copyright (C) Greg Snook, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg Snook, 2001"
 */
#ifndef SIMPLETERRAIN_H
#define SIMPLETERRAIN_H
/****************************************************************************************\

	SimpleTerrain.cpp

	SimpleTerrain component interface for a basic terrain manager.

	Part of the Gems II sample code library.
	Created 3/27/2001 by Greg Snook
	gregsnook@home.com

    ------------------------------------------------------------------------------------- 
	Notes/Revisions:
	I make no aplogies for spelling errors :)

\****************************************************************************************/
#define STRICT
#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include <commdlg.h>
#include <d3dx8.h>
#include "D3DApp.h"
#include "D3DFont.h"
#include "D3DUtil.h"
#include "DXUtil.h"
#include "resource.h"
#include "TileDefinitions.h" // <- the index buffers are defined in here


/*	SimpleTerrain
------------------------------------------------------------------------------------------
	
	SimpleTerrain is a sample class that demonstrates the terrain method. 
	It's been written for clarity, not speed. 
	
------------------------------------------------------------------------------------------
*/
#define TILE_VERTS 9	// our tiles are 9x9 vertices
#define TILE_COUNT 16	// our terrain in 16x16 tiles

// the overall terrain is a set of VERTEX_COUNT x VERTEX_COUNT vertices
#define VERTEX_COUNT ((TILE_COUNT*(TILE_VERTS-1))+1) 

class SimpleTerrain : public CD3DApplication
{
public:

	// Data Types & Constants...

	// our sample contains 4 possible detail levels per tile
	enum LEVEL
	{
		LEVEL_0 = 0,
		LEVEL_1,
		LEVEL_2,
		LEVEL_3,
		TOTAL_LEVELS
	};

	// data about each index buffer we create
	struct INDEX_BUFFER
	{
		IDirect3DIndexBuffer8*	pIndexBuffer;
		int						IndexCount;
		int						TriangleCount;
	};

	// the index buffers needed for each of our detail levels
	struct DETAIL_LEVEL
	{
		INDEX_BUFFER 	TileBodies[16];
		INDEX_BUFFER	TileConnectors[TOTAL_SIDES][TOTAL_LEVELS];
	};

	// a tile of our terrain
	struct TILE
	{
		LEVEL					DetailLevel;// the current detail level to use
		IDirect3DVertexBuffer8* VBuffer;	// a pile of vertices for this tile
		D3DXVECTOR3				Center;		// the center point of the tile
	};

	// our vertex format for this sample
	struct TERRAIN_VERTEX
	{
		D3DXVECTOR3  vert;
		D3DXVECTOR3  norm;
		float		tu,tv;
	};

	// the FVF code for this sample
	enum VERTEX_FVF
	{
		FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1
	};

	// Creators...

	SimpleTerrain();
	~SimpleTerrain(){};

	// Operators...

	// Mutators...
	void GenerateDetailLevels();
	void GenerateTiles();
	void DrawTerrain();

    HRESULT OneTimeSceneInit();
    HRESULT InitDeviceObjects();
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
    HRESULT DeleteDeviceObjects();
    HRESULT Render();
    HRESULT FrameMove();
    HRESULT FinalCleanup();

    LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	// Accessors...

private:

	// Data...
	TILE				m_TerrainTile[TILE_COUNT][TILE_COUNT];	// our sample terrain of 32x32 tiles
	DETAIL_LEVEL		m_DetailLevel[TOTAL_LEVELS]; // data for each detail level

    LPDIRECT3DTEXTURE8	m_pMeshTexture;			// simple texture for our terrain
	LPDIRECT3DSURFACE8	m_pHeightData;			// greyscale height information
    D3DMATERIAL8		m_MeshMaterial;			// a basic material for the terrain

    CD3DFont*           m_pFont;              // Font for outputting frame stats
    CD3DArcBall         m_ArcBall;            // Mouse rotation utility
    D3DXVECTOR3         m_vObjectCenter;      // Center of bounding sphere of object
    FLOAT               m_fObjectRadius;      // Radius of bounding sphere of object
    D3DXMATRIX          m_matWorld;			  // the world transform matrix

	D3DXVECTOR3			m_CameraPos;		  // position of the camera
	int					m_FacesDrawn;		  // count of faces actually rendered each frame

	bool				m_bWireframe;		// wireframe mode toggle
	float				m_CameraOffset;		// camera's zoom factor

	// Private Functions...

	// Nonexistant Functions...

	SimpleTerrain( const SimpleTerrain& Src);
	SimpleTerrain& operator=( const SimpleTerrain& Src);

};


//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    SimpleTerrain d3dApp;

    if( FAILED( d3dApp.Create( hInst ) ) )
        return 0;

    return d3dApp.Run();
}

//-----------------------------------------------------------------------------
// Name: SimpleTerrain()
// Desc: Constructor
//-----------------------------------------------------------------------------
SimpleTerrain::SimpleTerrain()
{
    // Override base class members
    m_strWindowTitle     = _T("GEMS II: Simple Terrain");
    m_bUseDepthBuffer    = TRUE;
    m_bShowCursorWhenFullscreen = TRUE;

    // Initialize member variables
    m_pFont              = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
	m_pMeshTexture = 0;
	m_pHeightData = 0;

	D3DUtil_InitMaterial(m_MeshMaterial, 0.5f, 0.6f, 0.5f, 1.0f);

	ZeroMemory(m_TerrainTile, sizeof(TILE)*TILE_COUNT*TILE_COUNT);
	ZeroMemory(m_DetailLevel, sizeof(DETAIL_LEVEL)*TOTAL_LEVELS);

	m_FacesDrawn = 0;

	m_bWireframe = false;
	m_CameraOffset = -2.0f;

}




//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT SimpleTerrain::OneTimeSceneInit()
{
    // Set cursor to indicate that user can move the object with the mouse
#ifdef _WIN64
    SetClassLongPtr( m_hWnd, GCLP_HCURSOR, (LONG_PTR)LoadCursor( NULL, IDC_SIZEALL ) );
#else
    SetClassLong( m_hWnd, GCL_HCURSOR, (LONG)LoadCursor( NULL, IDC_SIZEALL ) );
#endif


    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT SimpleTerrain::FrameMove()
{
     // Setup world matrix
    D3DXMATRIX matWorld;
    D3DXMATRIX matRotationInverse;
    D3DXMatrixTranslation( &matWorld, -m_vObjectCenter.x,
                                      -m_vObjectCenter.y,
                                      -m_vObjectCenter.z );
    D3DXMatrixInverse( &matRotationInverse, NULL, m_ArcBall.GetRotationMatrix() );
    D3DXMatrixMultiply( &matWorld, &matWorld, &matRotationInverse );
    D3DXMatrixMultiply( &matWorld, &matWorld, m_ArcBall.GetTranslationMatrix() );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

 	m_CameraPos = D3DXVECTOR3( 0, 0, m_CameraOffset*m_fObjectRadius );
	D3DXMATRIX matView;
    D3DXMatrixLookAtLH( &matView, &m_CameraPos,
                                  &D3DXVECTOR3( 0, 0, 0 ),
                                  &D3DXVECTOR3( 0, 1, 0 ) );
    m_pd3dDevice->SetTransform( D3DTS_VIEW,  &matView );
    
	// transform the camera point into model space 
    D3DXMATRIX matWorldInv;
    D3DXMatrixInverse( &matWorldInv, NULL, &matWorld);

	D3DXVECTOR4 NewCamera;
	D3DXVec3Transform(&NewCamera, &m_CameraPos, &matWorldInv);

	// store this value as our camera position in model space
	m_CameraPos.x = NewCamera.x;
	m_CameraPos.y = NewCamera.y;
	m_CameraPos.z = NewCamera.z;

	// calc the minnimim distance for each detail level
	float detail_step = m_fObjectRadius/4.0f;
	float min_level3 = detail_step;
	float min_level2 = detail_step * 2.0f;
	float min_level1 = detail_step * 3.0f;

	// choose detail levels for each tile
	for (int i=0;i<TILE_COUNT;++i)
	{
		for (int j=0;j<TILE_COUNT;++j)
		{
			D3DXVECTOR3 VectorToCamera(m_TerrainTile[i][j].Center - m_CameraPos);
			float DistanceToCamera = D3DXVec3Length(&VectorToCamera) - m_fObjectRadius;

			if (DistanceToCamera < min_level3)
			{
				m_TerrainTile[i][j].DetailLevel = LEVEL_3;
			}
			else if (DistanceToCamera < min_level2)
			{
				m_TerrainTile[i][j].DetailLevel = LEVEL_2;
			}
			else if (DistanceToCamera < min_level1)
			{
				m_TerrainTile[i][j].DetailLevel = LEVEL_1;
			}
			else 
			{
				m_TerrainTile[i][j].DetailLevel = LEVEL_0;
			}
		}
	}

	// allow the user to drive the camera closer or further away
	// using the Up/Down arrow keys
	if (GetAsyncKeyState(VK_DOWN) & 0x8000) 
	{
		m_CameraOffset -= 0.01f;
	}
	else if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		m_CameraOffset += 0.01f;

		if (m_CameraOffset >= 0.0f)
		{
			m_CameraOffset = -0.01f;
		}
	}

	return S_OK;
}



//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT SimpleTerrain::Render()
{
    D3DXMATRIX matWorld;
    D3DXMATRIX matTemp;
    FLOAT fTrisPerSec;
    TCHAR strInfo[120];

    // Clear the scene
    m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         0x000000ff, 1.0f, 0x00000000 );

    // Draw scene
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
		// setup the proper material and texture
		m_pd3dDevice->SetMaterial( &m_MeshMaterial);
		m_pd3dDevice->SetTexture( 0, m_pMeshTexture);

		if (m_bWireframe)
		{
			m_pd3dDevice->SetRenderState(D3DRS_FILLMODE , D3DFILL_WIREFRAME );
		}
		else
		{
			m_pd3dDevice->SetRenderState(D3DRS_FILLMODE , D3DFILL_SOLID );
		}

		// we turn off backface culling for this example - so you can flip the terrain over
		m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		// draw our terrain
		DrawTerrain();

		// output our stats

        // Calculate and show triangles per sec, a reasonable throughput number
        fTrisPerSec = m_fFPS * m_FacesDrawn;
		
        // Output statistics
        wsprintf( strInfo, _T("%ld tris per sec, %ld triangles"),
                  (DWORD)fTrisPerSec,
                  m_FacesDrawn);

        m_pFont->DrawText( 2, 0, D3DCOLOR_ARGB(255,255,255,0), m_strFrameStats );
        m_pFont->DrawText( 2, 20, D3DCOLOR_ARGB(255,255,255,0), m_strDeviceStats );
        m_pFont->DrawText( 2, 40, D3DCOLOR_ARGB(255,255,255,0), strInfo);


        m_pd3dDevice->EndScene();
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT SimpleTerrain::InitDeviceObjects()
{
	// load our sample texture
    D3DXCreateTextureFromFile( m_pd3dDevice, _T("Grass.bmp"), &m_pMeshTexture);

	if (SUCCEEDED(m_pd3dDevice->CreateImageSurface(256,256, D3DFMT_R8G8B8, &m_pHeightData)))
	{
		D3DXLoadSurfaceFromFile( m_pHeightData, 0, 0, _T("HeightMap.bmp"), 0, D3DX_FILTER_NONE, 0, 0);
	}

	m_vObjectCenter.x=0.0f;
	m_vObjectCenter.y=0.0f;
	m_vObjectCenter.z=0.0f;

	m_fObjectRadius = VERTEX_COUNT/2.0f;

    // Initialize the font 
    m_pFont->InitDeviceObjects( m_pd3dDevice );

	// create the tile vertices
	GenerateTiles();
	GenerateDetailLevels();

    return D3D_OK;
}




//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT SimpleTerrain::RestoreDeviceObjects()
{
    m_pFont->RestoreDeviceObjects();

    // Setup render state
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,     TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,      TRUE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );

    // Setup the light
    D3DLIGHT8 light;
    light.Type         = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r    = light.Diffuse.g  = light.Diffuse.b  = 1.0f;
    light.Specular.r   = light.Specular.g = light.Specular.b = 0.0f;
    light.Ambient.r    = light.Ambient.g  = light.Ambient.b  = 0.3f;
    light.Position     = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &D3DXVECTOR3( 0.3f, -1.0f, 1.0f ) );
    light.Attenuation0 = light.Attenuation1 = light.Attenuation2 = 0.0f;
    light.Range        = sqrtf(FLT_MAX);
    m_pd3dDevice->SetLight(0, &light );
    m_pd3dDevice->LightEnable(0, TRUE );

    m_ArcBall.SetWindow( m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height, 0.85f );
    m_ArcBall.SetRadius( m_fObjectRadius );

    FLOAT fAspect = m_d3dsdBackBuffer.Width / (FLOAT)m_d3dsdBackBuffer.Height;

    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspect, m_fObjectRadius/64.0f,
                                m_fObjectRadius*200.0f);
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT SimpleTerrain::InvalidateDeviceObjects()
{
    m_pFont->InvalidateDeviceObjects();


    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.
//-----------------------------------------------------------------------------
HRESULT SimpleTerrain::DeleteDeviceObjects()
{
    m_pFont->DeleteDeviceObjects();

    SAFE_RELEASE( m_pMeshTexture);
	SAFE_RELEASE(m_pHeightData);

	// release the detail levels
	int i,j,k,l;
	for (i=0;i<TOTAL_LEVELS;++i)
	{
		for (j=0;j<16;++j)
		{
			SAFE_RELEASE(m_DetailLevel[i].TileBodies[j].pIndexBuffer);
		}
		for (k=0;k<TOTAL_SIDES;++k)
		{
			for (l=0;l<TOTAL_LEVELS;++l)
			{
				SAFE_RELEASE(m_DetailLevel[i].TileConnectors[k][l].pIndexBuffer);
			}
		}
	}

	// release the tiles
	for (i=0;i<TILE_COUNT;++i)
	{
		for (j=0;j<TILE_COUNT;++j)
		{
			SAFE_RELEASE(m_TerrainTile[i][j].VBuffer);
		}
	}

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT SimpleTerrain::FinalCleanup()
{
    SAFE_DELETE( m_pFont );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Message proc function to handle key and menu input
//-----------------------------------------------------------------------------
LRESULT SimpleTerrain::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam,
                                    LPARAM lParam )
{
    // Pass mouse messages to the ArcBall so it can build internal matrices
    m_ArcBall.HandleMouseMessages( hWnd, uMsg, wParam, lParam );

    // Trap the context menu
    if( WM_CONTEXTMENU==uMsg )
        return 0;

	if( uMsg == WM_COMMAND )
    {
        // Toggle mesh drawing mode
        if( LOWORD(wParam) == IDM_SHOWWIREFRAME )
        {
            m_bWireframe = !m_bWireframe;

            if( m_bWireframe )
                CheckMenuItem( GetMenu(hWnd), IDM_SHOWWIREFRAME, MF_CHECKED );
            else
                CheckMenuItem( GetMenu(hWnd), IDM_SHOWWIREFRAME, MF_UNCHECKED );
        }
    }

    return CD3DApplication::MsgProc( hWnd, uMsg, wParam, lParam );
}

void SimpleTerrain::DrawTerrain()
{
	m_FacesDrawn = 0;

	if (SUCCEEDED(m_pd3dDevice->SetVertexShader(FVF)))
	{
		// render each tile
		for (int i=0;i<TILE_COUNT;++i)
		{
			for (int j=0;j<TILE_COUNT;++j)
			{
				// activate the vertex buffer for this tile
				if (SUCCEEDED(m_pd3dDevice->SetStreamSource(0,m_TerrainTile[i][j].VBuffer, sizeof(TERRAIN_VERTEX))))
				{
					int body_tile = 0;
					LEVEL MyLevel = m_TerrainTile[i][j].DetailLevel;

					// examine the tile above this tile
					if (i && m_TerrainTile[i-1][j].DetailLevel < MyLevel)
					{
						LEVEL ThisLevel = m_TerrainTile[i-1][j].DetailLevel;
						body_tile |= 1<<TOP;

						// draw the connecting piece needed
						if (SUCCEEDED(m_pd3dDevice->SetIndices(m_DetailLevel[MyLevel].TileConnectors[TOP][ThisLevel].pIndexBuffer, 0)))
						{
							m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 81, 0, m_DetailLevel[MyLevel].TileConnectors[TOP][ThisLevel].TriangleCount);
							m_FacesDrawn += m_DetailLevel[MyLevel].TileConnectors[TOP][ThisLevel].TriangleCount;
						}
					}

					// examine the tile below this tile
					if (i<(TILE_COUNT-1) && m_TerrainTile[i+1][j].DetailLevel < MyLevel)
					{
						LEVEL ThisLevel = m_TerrainTile[i+1][j].DetailLevel;
						body_tile |= 1<<BOTTOM;

						// draw the connecting piece needed
						if (SUCCEEDED(m_pd3dDevice->SetIndices(m_DetailLevel[MyLevel].TileConnectors[BOTTOM][ThisLevel].pIndexBuffer, 0)))
						{
							m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 81, 0, m_DetailLevel[MyLevel].TileConnectors[BOTTOM][ThisLevel].TriangleCount);
							m_FacesDrawn += m_DetailLevel[MyLevel].TileConnectors[BOTTOM][ThisLevel].TriangleCount;
						}
					}

					// examine the tile to the left this tile
					if (j && m_TerrainTile[i][j-1].DetailLevel < MyLevel)
					{
						LEVEL ThisLevel = m_TerrainTile[i][j-1].DetailLevel;
						body_tile |= 1<<LEFT;

						// draw the connecting piece needed
						if (SUCCEEDED(m_pd3dDevice->SetIndices(m_DetailLevel[MyLevel].TileConnectors[LEFT][ThisLevel].pIndexBuffer, 0)))
						{
							m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 81, 0, m_DetailLevel[MyLevel].TileConnectors[LEFT][ThisLevel].TriangleCount);
							m_FacesDrawn += m_DetailLevel[MyLevel].TileConnectors[LEFT][ThisLevel].TriangleCount;
						}
					}

					// examine the tile to the right this tile
					if (j<(TILE_COUNT-1) && m_TerrainTile[i][j+1].DetailLevel < MyLevel)
					{
						LEVEL ThisLevel = m_TerrainTile[i][j+1].DetailLevel;
						body_tile |= 1<<RIGHT;

						// draw the connecting piece needed
						if (SUCCEEDED(m_pd3dDevice->SetIndices(m_DetailLevel[MyLevel].TileConnectors[RIGHT][ThisLevel].pIndexBuffer, 0)))
						{
							m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 81, 0, m_DetailLevel[MyLevel].TileConnectors[RIGHT][ThisLevel].TriangleCount);
							m_FacesDrawn += m_DetailLevel[MyLevel].TileConnectors[RIGHT][ThisLevel].TriangleCount;
						}
					}

					// finally, draw the body tile needed
					if (m_DetailLevel[MyLevel].TileBodies[body_tile].pIndexBuffer)
					{
						if (SUCCEEDED(m_pd3dDevice->SetIndices(m_DetailLevel[MyLevel].TileBodies[body_tile].pIndexBuffer, 0)))
						{
							m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 81, 0, m_DetailLevel[MyLevel].TileBodies[body_tile].TriangleCount);
							m_FacesDrawn += m_DetailLevel[MyLevel].TileBodies[body_tile].TriangleCount;
						}
					}
				}
			}
		}
	}
}




void SimpleTerrain::GenerateTiles()
{
	//
	// Generate tiles based on the values in the height map
	//

	TERRAIN_VERTEX TerrainVerts[VERTEX_COUNT][VERTEX_COUNT];
	int i,j;
	float x= -VERTEX_COUNT/2.0f;
	float y= -VERTEX_COUNT/2.0f;

	// lock the height map
	D3DLOCKED_RECT LockedRect;
	if (m_pHeightData && SUCCEEDED(m_pHeightData->LockRect(&LockedRect, 0, D3DLOCK_NO_DIRTY_UPDATE|D3DLOCK_NOSYSLOCK|D3DLOCK_READONLY )))
	{
		BYTE* pNextRow = (BYTE*)LockedRect.pBits;

		for (i=0;i<VERTEX_COUNT;++i)
		{
			BYTE* pPixel = pNextRow;
			pNextRow = pPixel + LockedRect.Pitch;

			for (j=0;j<VERTEX_COUNT;++j)
			{
				BYTE height_value = *pPixel;
				float height = (float)height_value/ 16.0f;
				pPixel += 3; // assuming a 24 bit heightmap

				// set the vertex position
				TerrainVerts[i][j].vert.x = x;
				TerrainVerts[i][j].vert.y = y;
				TerrainVerts[i][j].vert.z = height;

				// set the texture coordinates
				TerrainVerts[i][j].tu = (i / 32.0f);
				TerrainVerts[i][j].tv = (j / 32.0f);

				// calc the normal from the last pixel and row
				D3DXVECTOR3 RowVector(-1.0f, 0.0f, 0.0f);
				D3DXVECTOR3 CollumnVector(0.0f, -1.0f, 0.0f);
				D3DXVECTOR3 TempNorm;
				if (i)
				{
					RowVector = TerrainVerts[i-1][j].vert - TerrainVerts[i][j].vert;
				}
				if (j)
				{
					CollumnVector = TerrainVerts[i][j-1].vert - TerrainVerts[i][j].vert;
				}
				D3DXVec3Cross(&TempNorm, &RowVector, &CollumnVector);
				D3DXVec3Normalize(&TerrainVerts[i][j].norm, &TempNorm);
				x += 1.0f;
			}
			y += 1.0f;
			x= -VERTEX_COUNT/2.0f;
		}

		// unlock the height map
		m_pHeightData->UnlockRect();



		// now create the vertex buffers from the global vertex data
		int center_vertex = TILE_VERTS>>1;
		for (i=0;i<TILE_COUNT;++i)
		{
			int verty = i*(TILE_VERTS-1);

			for (j=0;j<TILE_COUNT;++j)
			{
				int vertx = j*(TILE_VERTS-1);

				// create a vertex buffer for this tile
				m_TerrainTile[i][j].DetailLevel = LEVEL_0;
				m_TerrainTile[i][j].VBuffer = 0;

				if (SUCCEEDED(m_pd3dDevice->CreateVertexBuffer(sizeof(TERRAIN_VERTEX)*TILE_VERTS*TILE_VERTS, D3DUSAGE_WRITEONLY, FVF, D3DPOOL_MANAGED, &m_TerrainTile[i][j].VBuffer)))
				{
					TERRAIN_VERTEX *pData;

					if (SUCCEEDED(m_TerrainTile[i][j].VBuffer->Lock(0,0, (BYTE**)&pData, D3DLOCK_NOSYSLOCK)))
					{
						for (int y=0;y<TILE_VERTS;++y)
						{
							for (int x=0;x<TILE_VERTS;++x)
							{
								// if this is the center of the tile, store it for distance checking
								if (y==center_vertex && x==center_vertex)
								{
									m_TerrainTile[i][j].Center = TerrainVerts[vertx+x][verty+y].vert;
								}

								// copy the vertex to our buffer
								memcpy(pData, &TerrainVerts[vertx+x][verty+y], sizeof(TERRAIN_VERTEX));
								++pData;
							}
						}
						m_TerrainTile[i][j].VBuffer->Unlock();
					}
				}
			}
		}
	}
}


void SimpleTerrain::GenerateDetailLevels()
{
	//
	// Generate Tile Bodies and Connectors for our 4 sample detail levels
	//

	//
	// Detail Level 0 (lowest detail level)
	//

	m_DetailLevel[0].TileBodies[0].pIndexBuffer=0;
	if (SUCCEEDED(m_pd3dDevice->CreateIndexBuffer(6*2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16 , D3DPOOL_MANAGED, &m_DetailLevel[0].TileBodies[0].pIndexBuffer)))
	{
		WORD* pIndex;
		if (SUCCEEDED(m_DetailLevel[0].TileBodies[0].pIndexBuffer->Lock(0,6*2, (BYTE**)&pIndex, D3DLOCK_NOSYSLOCK)))
		{
			pIndex[0] = BaseTile0[0];
			pIndex[1] = BaseTile0[1];
			pIndex[2] = BaseTile0[2];
			pIndex[3] = BaseTile0[3];
			pIndex[4] = BaseTile0[4];
			pIndex[5] = BaseTile0[5];

			m_DetailLevel[0].TileBodies[0].pIndexBuffer->Unlock();
			m_DetailLevel[0].TileBodies[0].IndexCount = 6;
			m_DetailLevel[0].TileBodies[0].TriangleCount = 2;
		}
	}

	//
	// Detail Level 1
	//

	// create each of the 16 tile bodies
	for (int body=0;body<16;++body)
	{
		m_DetailLevel[1].TileBodies[body].pIndexBuffer=0;
		m_DetailLevel[1].TileBodies[body].IndexCount = 0;

		int total_indexes=0;
		if (!(body & (1<<0))) total_indexes += 6;
		if (!(body & (1<<1))) total_indexes += 6;
		if (!(body & (1<<2))) total_indexes += 6;
		if (!(body & (1<<3))) total_indexes += 6;

		if (total_indexes)
		{
			if (SUCCEEDED(m_pd3dDevice->CreateIndexBuffer(total_indexes*2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16 , D3DPOOL_MANAGED, &m_DetailLevel[1].TileBodies[body].pIndexBuffer)))
			{
				WORD* pIndex;
				if (SUCCEEDED(m_DetailLevel[1].TileBodies[body].pIndexBuffer->Lock(0,total_indexes*2, (BYTE**)&pIndex, D3DLOCK_NOSYSLOCK)))
				{
					int index=0;

					for (int side=0;side<TOTAL_SIDES;++side)
					{
						if (!(body & (1<<side)))
						{
							for (int data=0;data<6;++data)
							{
								pIndex[index++] = SidesOfLevel1[side][data];
							}
						}
					}
					m_DetailLevel[1].TileBodies[body].pIndexBuffer->Unlock();
					m_DetailLevel[1].TileBodies[body].IndexCount = total_indexes;
					m_DetailLevel[1].TileBodies[body].TriangleCount = total_indexes/3;
				}
			}
		}
	}

	// create the tile connectors
	for (int side=0;side<TOTAL_SIDES;++side)
	{
		m_DetailLevel[1].TileConnectors[side][0].pIndexBuffer = 0;
		if (SUCCEEDED(m_pd3dDevice->CreateIndexBuffer(3*2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16 , D3DPOOL_MANAGED, &m_DetailLevel[1].TileConnectors[side][0].pIndexBuffer)))
		{
			WORD* pIndex;
			if (SUCCEEDED(m_DetailLevel[1].TileConnectors[side][0].pIndexBuffer->Lock(0,3*2, (BYTE**)&pIndex, D3DLOCK_NOSYSLOCK)))
			{
				int index=0;

				for (int count=0;count<3;++count)
				{
					pIndex[count] = Connect1to0[side][count];
				}
				m_DetailLevel[1].TileConnectors[side][0].pIndexBuffer->Unlock();
				m_DetailLevel[1].TileConnectors[side][0].IndexCount = 3;
				m_DetailLevel[1].TileConnectors[side][0].TriangleCount = 1;
			}
		}
	}

	//
	// Detail Level 2
	//

	// create each of the 16 tile bodies
	for (body=0;body<16;++body)
	{
		m_DetailLevel[2].TileBodies[body].pIndexBuffer=0;
		m_DetailLevel[2].TileBodies[body].IndexCount = 0;

		int total_indexes=24;
		if (!(body & (1<<0))) total_indexes += 18;
		if (!(body & (1<<1))) total_indexes += 18;
		if (!(body & (1<<2))) total_indexes += 18;
		if (!(body & (1<<3))) total_indexes += 18;

		if (total_indexes)
		{
			if (SUCCEEDED(m_pd3dDevice->CreateIndexBuffer(total_indexes*2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16 , D3DPOOL_MANAGED, &m_DetailLevel[2].TileBodies[body].pIndexBuffer)))
			{
				WORD* pIndex;
				if (SUCCEEDED(m_DetailLevel[2].TileBodies[body].pIndexBuffer->Lock(0,total_indexes*2, (BYTE**)&pIndex, D3DLOCK_NOSYSLOCK)))
				{
					int index=0;

					// start by copying the center portion of the tile
					for (int center_vert=0;center_vert<24;++center_vert)
					{
						pIndex[index++] = Level2_Center[center_vert];
					}

					for (int side=0;side<TOTAL_SIDES;++side)
					{
						if (!(body & (1<<side)))
						{
							for (int data=0;data<18;++data)
							{
								pIndex[index++] = SidesOfLevel2[side][data];
							}
						}
					}
					m_DetailLevel[2].TileBodies[body].pIndexBuffer->Unlock();
					m_DetailLevel[2].TileBodies[body].IndexCount = total_indexes;
					m_DetailLevel[2].TileBodies[body].TriangleCount = total_indexes/3;
				}
			}
		}
	}

	// create the tile connectors
	for (side=0;side<TOTAL_SIDES;++side)
	{
		// connections to detail level 0
		m_DetailLevel[2].TileConnectors[side][0].pIndexBuffer = 0;
		if (SUCCEEDED(m_pd3dDevice->CreateIndexBuffer(9*2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16 , D3DPOOL_MANAGED, &m_DetailLevel[2].TileConnectors[side][0].pIndexBuffer)))
		{
			WORD* pIndex;
			if (SUCCEEDED(m_DetailLevel[2].TileConnectors[side][0].pIndexBuffer->Lock(0,9*2, (BYTE**)&pIndex, D3DLOCK_NOSYSLOCK)))
			{
				int index=0;

				for (int count=0;count<9;++count)
				{
					pIndex[count] = Connect2to0[side][count];
				}
				m_DetailLevel[2].TileConnectors[side][0].pIndexBuffer->Unlock();
				m_DetailLevel[2].TileConnectors[side][0].IndexCount = 9;
				m_DetailLevel[2].TileConnectors[side][0].TriangleCount = 3;
			}
		}

		// connections to detail level 1
		m_DetailLevel[2].TileConnectors[side][1].pIndexBuffer = 0;
		if (SUCCEEDED(m_pd3dDevice->CreateIndexBuffer(12*2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16 , D3DPOOL_MANAGED, &m_DetailLevel[2].TileConnectors[side][1].pIndexBuffer)))
		{
			WORD* pIndex;
			if (SUCCEEDED(m_DetailLevel[2].TileConnectors[side][1].pIndexBuffer->Lock(0,12*2, (BYTE**)&pIndex, D3DLOCK_NOSYSLOCK)))
			{
				int index=0;

				for (int count=0;count<12;++count)
				{
					pIndex[count] = Connect2to1[side][count];
				}
				m_DetailLevel[2].TileConnectors[side][1].pIndexBuffer->Unlock();
				m_DetailLevel[2].TileConnectors[side][1].IndexCount = 12;
				m_DetailLevel[2].TileConnectors[side][1].TriangleCount = 4;
			}
		}
	}

	//
	// Detail Level 3
	//

	// create each of the 16 tile bodies
	for (body=0;body<16;++body)
	{
		m_DetailLevel[3].TileBodies[body].pIndexBuffer=0;
		m_DetailLevel[3].TileBodies[body].IndexCount = 0;

		int total_indexes=216;
		if (!(body & (1<<0))) total_indexes += 42;
		if (!(body & (1<<1))) total_indexes += 42;
		if (!(body & (1<<2))) total_indexes += 42;
		if (!(body & (1<<3))) total_indexes += 42;

		if (total_indexes)
		{
			if (SUCCEEDED(m_pd3dDevice->CreateIndexBuffer(total_indexes*2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16 , D3DPOOL_MANAGED, &m_DetailLevel[3].TileBodies[body].pIndexBuffer)))
			{
				WORD* pIndex;
				if (SUCCEEDED(m_DetailLevel[3].TileBodies[body].pIndexBuffer->Lock(0,total_indexes*2, (BYTE**)&pIndex, D3DLOCK_NOSYSLOCK)))
				{
					int index=0;

					// start by copying the center portion of the tile
					for (int center_vert=0;center_vert<216;++center_vert)
					{
						pIndex[index++] = Level3_Center[center_vert];
					}

					for (int side=0;side<TOTAL_SIDES;++side)
					{
						if (!(body & (1<<side)))
						{
							for (int data=0;data<42;++data)
							{
								pIndex[index++] = SidesOfLevel3[side][data];
							}
						}
					}
					m_DetailLevel[3].TileBodies[body].pIndexBuffer->Unlock();
					m_DetailLevel[3].TileBodies[body].IndexCount = total_indexes;
					m_DetailLevel[3].TileBodies[body].TriangleCount = total_indexes/3;
				}
			}
		}
	}

	// create the tile connectors
	for (side=0;side<TOTAL_SIDES;++side)
	{
		// connections to detail level 0
		m_DetailLevel[3].TileConnectors[side][0].pIndexBuffer = 0;
		if (SUCCEEDED(m_pd3dDevice->CreateIndexBuffer(21*2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16 , D3DPOOL_MANAGED, &m_DetailLevel[3].TileConnectors[side][0].pIndexBuffer)))
		{
			WORD* pIndex;
			if (SUCCEEDED(m_DetailLevel[3].TileConnectors[side][0].pIndexBuffer->Lock(0,21*2, (BYTE**)&pIndex, D3DLOCK_NOSYSLOCK)))
			{
				int index=0;

				for (int count=0;count<21;++count)
				{
					pIndex[count] = Connect3to0[side][count];
				}
				m_DetailLevel[3].TileConnectors[side][0].pIndexBuffer->Unlock();
				m_DetailLevel[3].TileConnectors[side][0].IndexCount = 21;
				m_DetailLevel[3].TileConnectors[side][0].TriangleCount = 7;
			}
		}

		// connections to detail level 1
		m_DetailLevel[3].TileConnectors[side][1].pIndexBuffer = 0;
		if (SUCCEEDED(m_pd3dDevice->CreateIndexBuffer(24*2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16 , D3DPOOL_MANAGED, &m_DetailLevel[3].TileConnectors[side][1].pIndexBuffer)))
		{
			WORD* pIndex;
			if (SUCCEEDED(m_DetailLevel[3].TileConnectors[side][1].pIndexBuffer->Lock(0,24*2, (BYTE**)&pIndex, D3DLOCK_NOSYSLOCK)))
			{
				int index=0;

				for (int count=0;count<24;++count)
				{
					pIndex[count] = Connect3to1[side][count];
				}
				m_DetailLevel[3].TileConnectors[side][1].pIndexBuffer->Unlock();
				m_DetailLevel[3].TileConnectors[side][1].IndexCount = 24;
				m_DetailLevel[3].TileConnectors[side][1].TriangleCount = 8;
			}
		}

		// connections to detail level 2
		m_DetailLevel[3].TileConnectors[side][2].pIndexBuffer = 0;
		if (SUCCEEDED(m_pd3dDevice->CreateIndexBuffer(30*2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16 , D3DPOOL_MANAGED, &m_DetailLevel[3].TileConnectors[side][2].pIndexBuffer)))
		{
			WORD* pIndex;
			if (SUCCEEDED(m_DetailLevel[3].TileConnectors[side][2].pIndexBuffer->Lock(0,30*2, (BYTE**)&pIndex, D3DLOCK_NOSYSLOCK)))
			{
				int index=0;

				for (int count=0;count<30;++count)
				{
					pIndex[count] = Connect3to2[side][count];
				}
				m_DetailLevel[3].TileConnectors[side][2].pIndexBuffer->Unlock();
				m_DetailLevel[3].TileConnectors[side][2].IndexCount = 30;
				m_DetailLevel[3].TileConnectors[side][2].TriangleCount = 10;
			}
		}
	}
}

//- End of SimpleTerrain -----------------------------------------------------------------
//****************************************************************************************
#endif  // end of file      ( SimpleTerrain.h )

