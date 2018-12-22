/* Copyright (C) Chris Maughan, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Chris Maughan, 2001"
 */
#ifndef __TEXTUREMASKING_H
#define __TEXTUREMASKING_H

// constants
static const DWORD DEFAULT_SCREEN_WIDTH = 600;
static const DWORD DEFAULT_SCREEN_HEIGHT = 500;
static const DWORD NUM_TREES = 150;
static const DWORD NUM_OBJECTS = NUM_TREES;
static const float WORLD_SIZE = 150.0f;
static const float GROVE_SIZE = 14.0f;
static const float VIEWER_HEIGHT = 1.5f;
static const float NEAR_PLANE = 0.05f;
static const float FAR_PLANE = 100.0f;
static const float SUN_RADIUS = 30.0f;
static const DWORD SUNMAP_WIDTH = 16;
static const DWORD SUNMAP_HEIGHT = 16;
static const DWORD NUM_SUNMAP_SAMPLES = SUNMAP_WIDTH * SUNMAP_HEIGHT;
static const DWORD INTENSITYMAP_WIDTH = 16;
static const DWORD INTENSITYMAP_HEIGHT = 16;
static const D3DXCOLOR FLARE_MINCOLOR = D3DXCOLOR(0.65f, 0.39f, 0.19f, 1.0f);
static const D3DXCOLOR FLARE_MAXCOLOR = D3DXCOLOR(1.0f, 0.8f, 0.8f, 1.0f);
static const float LOD_DISTANCE_0 = 20.0f;
static const float LOD_DISTANCE_1 = 40.0f;
static const float LOD_DISTANCE_2 = 60.0f;
static const float LOD_DISTANCE_3 = 80.0f;

// Custom vertex types
class FlareVertex
{
public:
	FlareVertex(const D3DXVECTOR3& Position, DWORD dwColor, const D3DXVECTOR2& Texture)
		: m_Position(Position),
		m_Texture(Texture),
		m_dwColor(dwColor)
	{};

	D3DXVECTOR3 m_Position;
	DWORD m_dwColor;
	D3DXVECTOR2 m_Texture;
};

class FloorVertex
{
public:
	FloorVertex(const D3DXVECTOR3& Position, const D3DXVECTOR2& Texture)
		: m_Position(Position),
		m_Texture(Texture)
	{};

	D3DXVECTOR3 m_Position;
	D3DXVECTOR2 m_Texture;
};

class MappingVertexTL
{
public:
	MappingVertexTL(const D3DXVECTOR4& Position, const D3DXVECTOR2& Texture)
		: m_Position(Position),
		m_Texture(Texture)
	{};

	D3DXVECTOR4 m_Position;
	D3DXVECTOR2 m_Texture;
};


class FlareInfo
{
public:
	FlareInfo(float fRadius, D3DXCOLOR Color, float fPosition, std::string strFlareName)
		: m_fRadius(fRadius),
		m_FlareColor(Color),
		m_fPosition(fPosition),
		m_strFlareName(strFlareName)
	{
	};

	std::string m_strFlareName;
	float m_fRadius;
	D3DXCOLOR m_FlareColor;
	float m_fPosition;
};
typedef std::vector<FlareInfo> tFlareInfoList;

//-----------------------------------------------------------------------------
// Name: class CTextureMasking
// Desc: Application class. The base class (CD3DApplication) provides the 
//       generic functionality needed in all Direct3D samples. CTextureMasking 
//       adds functionality specific to this sample program.
//-----------------------------------------------------------------------------
class CTextureMasking : public CD3DApplication
{
public:
    LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
    CTextureMasking();

private:
	void SetMenuItems();

    HRESULT OneTimeSceneInit();
    HRESULT InitDeviceObjects();
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
    HRESULT DeleteDeviceObjects();
    HRESULT Render();
    HRESULT FrameMove();
    HRESULT FinalCleanup();
	HRESULT ConfirmDevice( D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format );

	HRESULT LoadFloor();
	HRESULT LoadTrees();
		
	HRESULT DrawFloor();
	
	HRESULT PrepareSunAndFlare();

	// Texture masking method
	HRESULT DrawLensFlareVisibility();

	// Locking method
	HRESULT CalculateLensFlareVisibility();

	HRESULT DrawFlares();
	HRESULT DrawSun(bool bSunMap);
	HRESULT DrawTrees(bool bSunMap);
	HRESULT DrawSunMap();
	HRESULT DrawIntensityMap();

	
private:
	HWND m_hWnd;

	bool m_bWireframe;
	
	LPDIRECT3DVERTEXBUFFER8 m_pFloorVB;
	LPDIRECT3DVERTEXBUFFER8 m_pFlareVB;
	
	nv_objects::NVWalkCamera* m_pCamera;
	nv_objects::NVWalkCamera* m_pSunCamera;

	nv_objects::NVText* m_pText;

	typedef enum
	{
		TREE_CHERRY_0 = 0,
		TREE_PINE_0,
		NUM_VEGETATIONTYPES
	} VegetationType;

	typedef enum
	{
		LOD_0 = 0,
		LOD_1 = 1,
		LOD_2 = 2,
		LOD_3 = 3,
		MAX_LODS
	} LODNumber;

	// Objects drawn in the landscape
	nv_objects::NVComplexMesh* m_pObjectMesh[NUM_VEGETATIONTYPES][MAX_LODS];
	DWORD m_NumLODS[NUM_VEGETATIONTYPES];
	bool m_bTreeHasTexture;
	DWORD m_dwNumTriangles;
	DWORD m_dwNumVertices;

	VegetationType m_VegetationTypes[NUM_OBJECTS];
	D3DXMATRIX m_matTrees[NUM_OBJECTS];
	D3DXCOLOR m_ShrubColors[NUM_OBJECTS];
	nv_objects::NVComplexMeshBounds m_TreeBounds[NUM_OBJECTS];

	// Sun projection info for the locking method
	D3DXVECTOR3 m_SunScreenTopLeft;
	D3DXVECTOR3 m_SunScreenBottomRight;
	D3DXVECTOR3 m_SunDirection;
	D3DXCOLOR m_SunColor;
	
	// Sky colors used when locking and reading the framebuffer
	DWORD m_dwSunColor;
	DWORD m_dwSkyColor;
	DWORD m_dwSkyColor565;
	DWORD m_dwSkyColor5551;

	// Sun location info for billboards, etc.
	float m_fSunDistance;
	float m_fSunRadius;
	D3DXVECTOR3 m_FarPlaneLight;
	D3DXVECTOR3 m_FarPlaneCenter;

	// Flare info
	bool m_bDrawFlare;
	tFlareInfoList m_FlareInfo;

	// Toggles
	bool m_bShowSunMap;
	bool m_bShowIntensityMap;
	bool m_bLockMethod;
	bool m_bReadBuffer;

	// Data for sunmap and intensity maps
	LPDIRECT3DVERTEXBUFFER8 m_pMappingVB;
	DWORD m_dwSunMapVBOffset;
	DWORD m_dwIntensityMapVBOffset;
	DWORD m_dwIntensityRenderVBOffset;
	DWORD m_dwLockedIntensity;

	LONG m_lSlugAmount;
	DWORD m_dwSlugTemporary;

	// Sun shrink factor for better coverage of sun texels.
	float m_fSunShrink;

	// Start position
	D3DXVECTOR3 m_CameraStart;

	std::string m_strLastError;

};

// Random functions.
static float RandRangePlusMinusOne()
{
	float fRand = ((float)rand() / (float)RAND_MAX);
	fRand *= 2.0f;
	fRand -= 1.0f;
	return fRand;
}

static float RandRangeZeroOne()
{
	float fRand = ((float)rand() / (float)RAND_MAX);
	return fRand;
}

static D3DXCOLOR GetRandFlareColor()
{
	D3DXCOLOR Range = FLARE_MAXCOLOR - FLARE_MINCOLOR;
	Range.r *= RandRangeZeroOne();
	Range.g *= RandRangeZeroOne();
	Range.b *= RandRangeZeroOne();
	return (Range + FLARE_MINCOLOR);
}


#define GET_X_MESSAGE(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_MESSAGE(lp)                        ((int)(short)HIWORD(lp))

#endif __TEXTUREMASKING_H