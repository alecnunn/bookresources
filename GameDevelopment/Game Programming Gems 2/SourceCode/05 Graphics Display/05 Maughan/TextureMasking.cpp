/* Copyright (C) Chris Maughan, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Chris Maughan, 2001"
 */
#define STRICT
#include <windows.h>
#include <commdlg.h>
#include <math.h>
#include <tchar.h>
#include <stdio.h>
#include <D3DX8.h>
#include "D3DApp.h"
#include "D3DUtil.h"
#include "DXUtil.h"
#include "resource.h"
#include "nvresourcemanager.h"
#include "nvstatemanager.h"
#include "nvtextureresource.h"
#include "nvcomplexmesh.h"
#include "nvwalkcamera.h"
#include "nvtext.h"
#include "Tree.h"
#include "TextureMasking.h"

using namespace nv_objects;
using namespace std;

// State manager and resource manager are singletons.
// This just means that there is only one instance of them in the system.
// Access is controlled by the call to NVStateManager::GetSingleton().
// The singleton is valid when it is created.
NVStateManager* g_pNVStateManager;
NVResourceManager* g_pNVResourceManager;

// A debug object
#ifdef _DEBUG
NVDebug g_DebugOutput(3, "texturemasking.log");
#endif

//-----------------------------------------------------------------------------
// Name: CTextureMasking()
// Desc: Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
CTextureMasking::CTextureMasking()
	: m_pCamera(NULL),
	m_pSunCamera(NULL),
	m_bWireframe(false),
	m_pText(NULL),
	m_bShowSunMap(false),
	m_bShowIntensityMap(false),
	m_bLockMethod(false),
	m_bReadBuffer(true),
	m_lSlugAmount(0),
	m_dwSlugTemporary(0)
{
	// Reset LOD's
	for (DWORD i = 0; i < NUM_VEGETATIONTYPES; i++)
	{
		for (DWORD LOD = 0; LOD < MAX_LODS; LOD++)
		{
			m_pObjectMesh[i][LOD] = NULL;
		}
	}
	
	// Window title
	m_strWindowTitle    = _T("TextureMasking: Fast Lens Flare");
	m_bUseDepthBuffer = TRUE;
	
	// A bit bigger than default
	m_dwCreationWidth = DEFAULT_SCREEN_WIDTH;
    m_dwCreationHeight = DEFAULT_SCREEN_HEIGHT;

	// Location of viewer and the vector to the sun in the world
	m_CameraStart = D3DXVECTOR3(0.0f, -WORLD_SIZE + VIEWER_HEIGHT, -5.0f);
	m_SunDirection = D3DXVECTOR3(0.1f, 0.56f, 1.0f);
	m_SunColor = D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f);
	
	m_dwSunColor = (DWORD)m_SunColor;

	DWORD SkyRed = 0xAA;
	DWORD SkyGreen = 0xAA;
	DWORD SkyBlue = 0xFF;

	m_dwSkyColor = (SkyRed << 16) | (SkyGreen << 8) | SkyBlue;
	m_dwSkyColor565 = ((SkyRed >> 3) << 11) | ((SkyGreen >> 2) << 5) | (SkyBlue >> 3);
	m_dwSkyColor5551 = ((SkyRed >> 3) << 10) | ((SkyGreen >> 3) << 5) | (SkyBlue >> 3);

	// Put the sun just in front of the far plane
	m_fSunDistance = (FAR_PLANE - 1.0f);
	m_fSunRadius = SUN_RADIUS;

	// This causes a smaller internal area of the sun to be sampled
	// to save wastage at the edges.
	m_fSunShrink = 0.70f;

}

// A simple quad for the floor
HRESULT CTextureMasking::LoadFloor()
{
	HRESULT hr;

    // create the floor quadrangle
    hr = m_pd3dDevice->CreateVertexBuffer( 4 * sizeof(FloorVertex), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_pFloorVB);
	if (FAILED(hr))
		return hr;
  
    FloorVertex* pFloor;

    hr = m_pFloorVB->Lock(0, 4 * sizeof(FloorVertex),(BYTE**)&pFloor, 0);
	if (FAILED(hr))
	{
		m_strLastError = "Couldn't lock Floor buffer!";
		return hr;
	}
    
    pFloor[0].m_Position = D3DXVECTOR3(-WORLD_SIZE, -WORLD_SIZE, -WORLD_SIZE);
    pFloor[0].m_Texture  = D3DXVECTOR2(0.0f, 0.0f);

    pFloor[1].m_Position = D3DXVECTOR3(WORLD_SIZE, -WORLD_SIZE, -WORLD_SIZE);
    pFloor[1].m_Texture  = D3DXVECTOR2(0.0f, 1.0f);
    
    pFloor[2].m_Position = D3DXVECTOR3(-WORLD_SIZE, -WORLD_SIZE, WORLD_SIZE);
    pFloor[2].m_Texture  = D3DXVECTOR2(1.0f, 0.0f);
    
    pFloor[3].m_Position = D3DXVECTOR3(WORLD_SIZE, -WORLD_SIZE, WORLD_SIZE);
    pFloor[3].m_Texture  = D3DXVECTOR2(1.0f, 1.0f);
    m_pFloorVB->Unlock();

	NVRESOURCEMANAGER.AddResource(new NVTextureFileResource(m_pd3dDevice, "floor.dds", NVTextureFileResource::TextureType_2D), "floor");

	return S_OK;
}

// Draws the trees.  They are tested against the view frustum for speed.  They are also 
// drawn with several different LOD's.  Note that the trees are procedural, and are
// created here.
HRESULT CTextureMasking::LoadTrees()
{
	DWORD i;
	DWORD LOD;
	HRESULT hr;
	// Create a tree mesh and copy in the data
	Tree* pTree = new Tree();
	pTree->setLeafSize(1.0f);
	pTree->setBranchSize(0.08f);
	pTree->setThreshhold(0.25f);
	pTree->setTwist(10.0f);
	pTree->setBranch(0.7f);
	pTree->setBranchBias(0.75f);
	pTree->setBranchNoise(0.8f);
	pTree->setBranchSegments(20);
	
	// Textures for trees
	NVResourceID BarkTextureID = NVRESOURCEMANAGER.AddResource(new NVTextureFileResource(m_pd3dDevice, "bark.dds", NVTextureFileResource::TextureType_2D), "bark");
	NVResourceID LeafTextureID = NVRESOURCEMANAGER.AddResource(new NVTextureFileResource(m_pd3dDevice, "leaf.dds", NVTextureFileResource::TextureType_2D), "leaf");
	pTree->setBarkTexture(BarkTextureID);
	pTree->setLeafTexture(LeafTextureID);
	pTree->setLeafBackTexture(LeafTextureID);

	pTree->setBranchSize(0.08f);
	pTree->setBranchNoise(0.8f);

	// Create several different LOD's.
	DWORD ThisLOD;
	for (ThisLOD = 0; ThisLOD < MAX_LODS; ThisLOD++)
	{
		// Cherry 1
		pTree->setBranchSegments(20 / (ThisLOD + 1));
		m_pObjectMesh[TREE_CHERRY_0][ThisLOD] = new NVComplexMesh();
		pTree->Transfer(m_pObjectMesh[TREE_CHERRY_0][ThisLOD]);
	}


	pTree->setBranchSize(0.05f);
	pTree->setThreshhold(0.25f);
	pTree->setBranchSize(0.08f);
	pTree->setBranchNoise(0.9f);
	pTree->setTwist(30.0f);
	pTree->setBranch(0.6f);
	pTree->setBranchBias(0.75f);
	pTree->setBranchNoise(0.8f);

	for (ThisLOD = 0; ThisLOD < MAX_LODS; ThisLOD++)
	{
		// Pine 0
		pTree->setBranchSegments(20 / (ThisLOD + 1));
		m_pObjectMesh[TREE_PINE_0][ThisLOD] = new NVComplexMesh();
		pTree->Transfer(m_pObjectMesh[TREE_PINE_0][ThisLOD]);
	}

	m_NumLODS[TREE_CHERRY_0] = ThisLOD;
	m_NumLODS[TREE_PINE_0] = ThisLOD;

	SAFE_DELETE(pTree);
	
	// Walk through the tree meshes seting them up for rendering.
	// Scale them and store translation matrices for each tree.
	for (i = 0; i < NUM_VEGETATIONTYPES; i++)
	{
		for (LOD = 0; LOD < m_NumLODS[i]; LOD++)
		{
			// Make sure there are normals
			if (!m_pObjectMesh[i][LOD]->FindVertexData("normal"))
			{
				m_pObjectMesh[i][LOD]->AddVertexData("normal", NVCOMPLEXMESH_FLOAT3);
				m_pObjectMesh[i][LOD]->GenerateNormals();
			}
				
			if (!m_pObjectMesh[i][LOD]->FindVertexData("texture0"))
			{
				m_bTreeHasTexture = false;
			}
			else
			{
				m_bTreeHasTexture = true;
			}
			m_pObjectMesh[i][LOD]->AddVertexData("texture0", NVCOMPLEXMESH_FLOAT2);

			// Get the bounding info and scale the model
			NVComplexMeshBounds Bounds;
			m_pObjectMesh[i][LOD]->ComputeBounds(Bounds);

			D3DXMATRIX matTranslate, matScale;
			D3DXMATRIX matTree;

			D3DXMatrixTranslation(&matTranslate, -Bounds.m_vecCenter.x, -Bounds.m_vecCenter.y, -Bounds.m_vecCenter.z);
			D3DXMatrixScaling(&matScale, 1.0f / Bounds.m_fRadius,  1.0f / Bounds.m_fRadius,  1.0f / Bounds.m_fRadius);
			matTree = matTranslate * matScale;

			m_pObjectMesh[i][LOD]->Transform(matTree);
			
			// We are going to use FVF rendering, so just set it up
			m_pObjectMesh[i][LOD]->SetupFVFStream();

		
			// Prepare the mesh for rendering
			hr = m_pObjectMesh[i][LOD]->PrepareDeviceObjects(m_pd3dDevice);
			if (FAILED(hr))
			{
				m_strLastError = "Loaded, but can't render tree model";
				SAFE_DELETE(m_pObjectMesh[i][LOD]);
				return E_FAIL;
			}
		}
	}

	NVComplexMeshBounds BoundsTypes[NUM_VEGETATIONTYPES];
	for (i = 0; i < NUM_VEGETATIONTYPES; i++)
	{
		m_pObjectMesh[i][0]->ComputeBounds(BoundsTypes[i]);
	}

	// Random selection of trees
	DWORD dwNumTrees = NUM_TREES;
	i = 0;
	while (dwNumTrees > 0)
	{
		float Rnd = RandRangePlusMinusOne();
		if (Rnd < 0.0f)
		{
			m_VegetationTypes[i++] = TREE_CHERRY_0;
		}
		else
		{
			m_VegetationTypes[i++] = TREE_PINE_0;
		}
		dwNumTrees--;
	};

	// Create AABB data for each tree.
	for (i = 0; i < NUM_OBJECTS; i++)
	{
		D3DXVECTOR3 vecTreePosition;
		D3DXVECTOR3 vecTreeScale;
		vecTreePosition.x = RandRangePlusMinusOne() * (GROVE_SIZE);
		vecTreePosition.z = RandRangePlusMinusOne() * (GROVE_SIZE);
		vecTreeScale.x = vecTreeScale.y = vecTreeScale.z = 5.0f + (RandRangePlusMinusOne() * 2.0f);

		D3DXMATRIX matTranslate, matScale, matRotateY, matTree;
		
		D3DXMatrixTranslation(&matTranslate, vecTreePosition.x, -WORLD_SIZE + (-BoundsTypes[m_VegetationTypes[i]].m_vecMinExtents.y * vecTreeScale.y), vecTreePosition.z);
		D3DXMatrixRotationY(&matRotateY, RandRangeZeroOne() * 360.0f);
		D3DXMatrixScaling(&matScale, vecTreeScale.x, vecTreeScale.y, vecTreeScale.z);

		m_matTrees[i] = matRotateY * matScale * matTranslate;

		NVComplexMeshBounds ThisTreeBounds = BoundsTypes[m_VegetationTypes[i]];

		D3DXMATRIX matBounds = matScale * matTranslate;
		ThisTreeBounds.Transform(&matBounds);
		m_TreeBounds[i] = ThisTreeBounds; 

	}
	return S_OK;
}



//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CTextureMasking::InitDeviceObjects()
{
	HRESULT hr;
	vector<DWORD> Declaration;

	// Create resource and state manager.  These just make changing render state and managing textures
	// easier.
	g_pNVStateManager = new NVStateManager(m_pd3dDevice);
	g_pNVResourceManager = new NVResourceManager(m_pd3dDevice);

	// Create the cameras
	float fLimit = GROVE_SIZE * 2.0f;

	// Projection doesn't change
	D3DXMATRIX matProjection;
    float fAspect = (float)m_d3dsdBackBuffer.Width / (float)m_d3dsdBackBuffer.Height;
	D3DXMatrixPerspectiveFovLH( &matProjection, D3DXToRadian(90.0f), fAspect, NEAR_PLANE, FAR_PLANE);

	// Create a camera limited to the world dimensions.
#define LIMIT_CAMERA
#ifdef LIMIT_CAMERA
	m_pCamera = new NVWalkCamera(matProjection, m_CameraStart, 0.0f, 0.0f, D3DXVECTOR3(-fLimit, -WORLD_SIZE + VIEWER_HEIGHT, -fLimit), D3DXVECTOR3(fLimit, -WORLD_SIZE + (VIEWER_HEIGHT * 1.5f), fLimit));
#else
	m_pCamera = new NVWalkCamera();
#endif
	m_pSunCamera = new NVWalkCamera();

	// Create a texture map with a font in it for text rendering.
	// We don't use GDI to render to the buffer because that hurts performance
	m_pText = new NVText(m_pd3dDevice);
	m_pText->SetupTextImage("Arial", 10, D3DFONT_BOLD);
	m_pText->InitDeviceObjects();

	// Add resources.
	// Add Sun, lens flare and sun/intensity maps.
	// Resource manager simply manages creation/lifetime of textures
	NVRESOURCEMANAGER.AddResource(new NVTextureFileResource(m_pd3dDevice, "flare0.dds", NVTextureFileResource::TextureType_2D), "Sun");
	NVRESOURCEMANAGER.AddResource(new NVTextureFileResource(m_pd3dDevice, "flare1.dds", NVTextureFileResource::TextureType_2D), "LensFlare1");
	NVRESOURCEMANAGER.AddResource(new NVTextureFileResource(m_pd3dDevice, "flare2.dds", NVTextureFileResource::TextureType_2D), "LensFlare2");
	NVRESOURCEMANAGER.AddResource(new NVTextureFileResource(m_pd3dDevice, "flare3.dds", NVTextureFileResource::TextureType_2D), "LensFlare3");
	NVRESOURCEMANAGER.AddResource(new NVTextureFileResource(m_pd3dDevice, "flare4.dds", NVTextureFileResource::TextureType_2D), "LensFlare4");
	NVRESOURCEMANAGER.AddResource(new NVTextureFileResource(m_pd3dDevice, "flare5.dds", NVTextureFileResource::TextureType_2D), "LensFlare5");

	// Static resources - the sunmap and the intensity map
	NVRESOURCEMANAGER.AddResource(new NVTextureStaticResource(m_pd3dDevice, NVTextureStaticResource::TextureType_2D, SUNMAP_WIDTH, SUNMAP_HEIGHT, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT), "SunMap");
	NVRESOURCEMANAGER.AddResource(new NVTextureStaticResource(m_pd3dDevice, NVTextureStaticResource::TextureType_2D, INTENSITYMAP_WIDTH, INTENSITYMAP_HEIGHT, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT), "IntensityMap");

	// Create a big square for rendering the sunmap
    if( FAILED( m_pd3dDevice->CreateVertexBuffer( ((NUM_SUNMAP_SAMPLES) + 8) *sizeof(MappingVertexTL), D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_TEX1, 
                                                  D3DPOOL_MANAGED, &m_pMappingVB ) ) )
        return E_FAIL;
	
	// Create a quad for the sun map sprite
    MappingVertexTL* v;
	float fStartX = 225.0f;
	float fStartY = 0.0f;
	float fSize = 100.0f;
	D3DXVECTOR4 TopLeft(fStartX, fStartY, 0.0f, 1.0f);
	D3DXVECTOR4 BottomRight(fStartX + fSize, fStartY + fSize, 0.0f, 1.0f);

    hr = m_pMappingVB->Lock( 0, 0, (BYTE**)&v, 0 );
	if (FAILED(hr))
		return E_FAIL;
	
	DWORD dwVertexIndex = 0;
	m_dwSunMapVBOffset = dwVertexIndex;
    v[dwVertexIndex].m_Position = D3DXVECTOR4(TopLeft.x, BottomRight.y, 0, 1);
	v[dwVertexIndex].m_Texture = D3DXVECTOR2(0, 1);
	dwVertexIndex++;

    v[dwVertexIndex].m_Position = TopLeft;
	v[dwVertexIndex].m_Texture = D3DXVECTOR2(0, 0);
	dwVertexIndex++;
    
	v[dwVertexIndex].m_Position = BottomRight;
	v[dwVertexIndex].m_Texture = D3DXVECTOR2(1, 1);
	dwVertexIndex++;

	v[dwVertexIndex].m_Position = D3DXVECTOR4(BottomRight.x, TopLeft.y, 0, 1);
	v[dwVertexIndex].m_Texture = D3DXVECTOR2(1, 0);
	dwVertexIndex++;

	// Create a quad for the intensity map sprite
	fStartX = fStartX + (fSize);
	fSize = fSize / 2.0f;
	TopLeft = D3DXVECTOR4(fStartX, fStartY, 0.0f, 1.0f);
	BottomRight = D3DXVECTOR4(fStartX + fSize, fStartY + fSize, 0.0f, 1.0f);

	m_dwIntensityMapVBOffset = dwVertexIndex;
	v[dwVertexIndex].m_Position = D3DXVECTOR4(TopLeft.x, BottomRight.y, 0, 1);
	v[dwVertexIndex].m_Texture = D3DXVECTOR2(0, 0);
	dwVertexIndex++;

    v[dwVertexIndex].m_Position = TopLeft;
	v[dwVertexIndex].m_Texture = D3DXVECTOR2(0, 0);
	dwVertexIndex++;
    
	v[dwVertexIndex].m_Position = BottomRight;
	v[dwVertexIndex].m_Texture = D3DXVECTOR2(0, 0);
	dwVertexIndex++;

	v[dwVertexIndex].m_Position = D3DXVECTOR4(BottomRight.x, TopLeft.y, 0, 1);
	v[dwVertexIndex].m_Texture = D3DXVECTOR2(0, 0);
	dwVertexIndex++;

	// Create the 256 polygons which will sum into our intensity map
	m_dwIntensityRenderVBOffset = dwVertexIndex;
	for (float y = 0.0f; y < 1.0f; y += (1.0f / (float)SUNMAP_HEIGHT))
	{
		for (float x = 0.0f; x < 1.0f; x += (1.0f / (float)SUNMAP_WIDTH))
		{
			v[dwVertexIndex].m_Position = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
			v[dwVertexIndex].m_Texture = D3DXVECTOR2(x, y);
			dwVertexIndex++;
		}
	}

	m_pMappingVB->Unlock();

	// Create a bunch of flares
	m_FlareInfo.push_back(FlareInfo(0.60f, GetRandFlareColor(), 1.3f, "LensFlare3"));
	m_FlareInfo.push_back(FlareInfo(0.50f, GetRandFlareColor(), 1.0f, "LensFlare2"));
	m_FlareInfo.push_back(FlareInfo(0.4f, GetRandFlareColor(), 0.5f, "LensFlare2"));
	m_FlareInfo.push_back(FlareInfo(0.25f, GetRandFlareColor(), 0.0f, "LensFlare1"));
	m_FlareInfo.push_back(FlareInfo(0.4f, GetRandFlareColor(), -0.3f, "LensFlare1"));
	m_FlareInfo.push_back(FlareInfo(0.5f, GetRandFlareColor(), -0.6f, "LensFlare3"));
	m_FlareInfo.push_back(FlareInfo(2.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), -1.0f, "LensFlare5"));
	
	// Load the tree objects
	hr = LoadTrees();
	if (FAILED(hr))
	{
		return hr;
	}

	// Load the floor object
	hr = LoadFloor();
	if (FAILED(hr))
	{
		return hr;
	}
		
    return S_OK;
}

// Draws the floor quad
HRESULT CTextureMasking::DrawFloor()
{    
    // all color comes from texture, no alpha at all
    NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
    NVSTATEMANAGER.SaveVertexShader(D3DFVF_XYZ | D3DFVF_TEX1);
    NVSTATEMANAGER.SaveTexture(0, NVTextureResource::GetTexture("floor"));
	NVSTATEMANAGER.SaveRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	D3DXMATRIX Identity;
	D3DXMatrixIdentity(&Identity);
	NVSTATEMANAGER.SetTransform(D3DTS_WORLD, &Identity);
	
	NVSTATEMANAGER.SetStreamSource(0, m_pFloorVB, sizeof(FloorVertex));	
    NVSTATEMANAGER.DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

    NVSTATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLOROP);
    NVSTATEMANAGER.RestoreVertexShader();
    NVSTATEMANAGER.RestoreTexture(0);
	NVSTATEMANAGER.RestoreRenderState(D3DRS_CULLMODE);

	return S_OK;
}

// Draws the trees, checking each against the current camera.
// We use static LOD to enable more trees,
// we sort the whole world by material for speed.
// If bSunMap then the trees are rendered in white.
HRESULT CTextureMasking::DrawTrees(bool bSunMap)
{
	DWORD i;
	long LOD;
	D3DXVECTOR3 vecCameraPos = m_pCamera->GetCameraPosition();
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity(&matIdentity);

	tSectionList::iterator itrSections;

	if (!m_pObjectMesh[0])
		return S_OK;

	// Setup material depending on how we are rendering the trees.
	for (i = 0; i < NUM_VEGETATIONTYPES; i++)
	{
		for (LOD = 0; LOD < m_NumLODS[i]; LOD++)
		{
			tSectionList& Sections = m_pObjectMesh[i][LOD]->GetPass(0)->GetSections();
			itrSections = Sections.begin();

			// Walk all the model sections in this pass
			while (itrSections != Sections.end())
			{
				// Set the constants and vertex shaders in this section
				NVMaterialResource* pMaterial = static_cast<NVMaterialResource*>(NVRESOURCEMANAGER.FindResource((*itrSections).GetMaterialID()));
				assert(pMaterial);

				// Draw the trees in black if we are sun-mapping
				if (bSunMap)
				{
					pMaterial->AddRenderState(D3DRS_TEXTUREFACTOR, 0);
					pMaterial->AddTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
					pMaterial->AddTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
				}
				// Draw the trees with their normal texture
				else
				{
					pMaterial->AddTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
					pMaterial->AddTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
				}
				itrSections++;
			}
		}
	}

	// Collect the visible objects in the scene in advance
	vector<DWORD> VisibleObjects;
	vector<DWORD>::iterator itrVisibleObjects;

	DWORD dwTreeCount = 0;
	for (i = 0; i < NUM_OBJECTS; i++)
	{
		if (m_pCamera->CheckAABBVisible(m_TreeBounds[i].m_vecMinExtents, m_TreeBounds[i].m_vecMaxExtents) > 0)
		{
			dwTreeCount++;
			VisibleObjects.push_back(i);
		}
	}

	// Walk the visible objects, drawing by material
	// This is THE way to get fast performance out of the graphics card.
	// This way we only set the state for the bark once and draw all the branches, 
	// and set the state for the leaves once, and draw all the leaves.
	const tResourceIDSet& MaterialSet = NVRESOURCEMANAGER.GetMaterialResources();
	tResourceIDSet::const_iterator itrMaterials = MaterialSet.begin();
	while (itrMaterials != MaterialSet.end())
	{
		// Get the material
		NVMaterialResource* pMaterial = static_cast<NVMaterialResource*>(NVRESOURCEMANAGER.FindResource(*itrMaterials));

		// Set the material on the device.
		pMaterial->SaveMaterial();

		// Walk the visible object list
		itrVisibleObjects = VisibleObjects.begin();
		while (itrVisibleObjects != VisibleObjects.end())
		{
			bool bDrawn = false;

			i = *itrVisibleObjects;

			// Calculate the LOD for this visible object
			D3DXVECTOR3 vecTreeDistance = m_TreeBounds[i].m_vecCenter - vecCameraPos;
			vecTreeDistance.y = 0.0f;
			float fDistance = D3DXVec3Dot(&vecTreeDistance, &vecTreeDistance);
						
			LOD = 0;
			if (bSunMap)
			{
				LOD = MAX_LODS-1;
			}
			else
			{
				if (fDistance > LOD_DISTANCE_0)
					LOD++;
				if (fDistance > LOD_DISTANCE_1)
					LOD++;
				if (fDistance > LOD_DISTANCE_2)
					LOD++;
			
				if (LOD >= MAX_LODS)
					LOD = MAX_LODS - 1;
			}

			// Walk up the lods to find the nearest available LOD
			while (m_pObjectMesh[m_VegetationTypes[i]][LOD] == NULL)
				LOD--;

			NVASSERT(LOD >= 0, "Couldn't find tree LOD!");

			// Set this tree's world transform
			NVSTATEMANAGER.SetTransform(D3DTS_WORLD, &m_matTrees[i]);

			// Walk the model sections, finding the materials to render.
			// Optimization opportunity!  The mesh could have a map of materials so it could do the check quickly,
			// rather than walking the list of sections
			tSectionList& Sections = m_pObjectMesh[m_VegetationTypes[i]][LOD]->GetPass(0)->GetSections();
			itrSections = Sections.begin();
			while (itrSections != Sections.end())
			{
				// If this material matches, draw the object
				if ((*itrSections).GetMaterialID() == (*itrMaterials))
				{
					// Draw it
					m_pObjectMesh[m_VegetationTypes[i]][LOD]->GetPass(0)->RenderSection(m_pd3dDevice, itrSections);
				}
				itrSections++;
			}

			itrVisibleObjects++;
		}

		// Restore this material
		pMaterial->RestoreMaterial();
		
		itrMaterials++;
	}

	return S_OK;
}

// Draw the sunmap for diagnostic purposes
HRESULT CTextureMasking::DrawSunMap()
{
	NVSTATEMANAGER.SaveTexture(0, NVTextureResource::GetTexture("SunMap"));
	NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	NVSTATEMANAGER.SaveVertexShader(D3DFVF_XYZRHW | D3DFVF_TEX1);
	NVSTATEMANAGER.SaveRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	NVSTATEMANAGER.SaveRenderState(D3DRS_LIGHTING, FALSE);

	// Draw sun
	NVSTATEMANAGER.SetStreamSource(0, m_pMappingVB, sizeof(MappingVertexTL));
	NVSTATEMANAGER.DrawPrimitive(D3DPT_TRIANGLESTRIP, m_dwSunMapVBOffset, 2);
	
	NVSTATEMANAGER.RestoreTexture(0);
	NVSTATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLOROP);
	NVSTATEMANAGER.RestoreTextureStageState(0, D3DTSS_ALPHAOP);
	NVSTATEMANAGER.RestoreVertexShader();
	NVSTATEMANAGER.RestoreRenderState(D3DRS_CULLMODE);
	NVSTATEMANAGER.RestoreRenderState(D3DRS_LIGHTING);

	return S_OK;
}

// Draw the intensity map for diagnostic purposes
HRESULT CTextureMasking::DrawIntensityMap()
{
	NVSTATEMANAGER.SaveTexture(0, NVTextureResource::GetTexture("IntensityMap"));
	NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT );
	NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT );
	NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE );

	NVSTATEMANAGER.SaveVertexShader(D3DFVF_XYZRHW | D3DFVF_TEX1);
	NVSTATEMANAGER.SaveRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	NVSTATEMANAGER.SaveRenderState(D3DRS_LIGHTING, FALSE);


	// Draw sun
	NVSTATEMANAGER.SetStreamSource(0, m_pMappingVB, sizeof(MappingVertexTL));
	NVSTATEMANAGER.DrawPrimitive(D3DPT_TRIANGLESTRIP, m_dwIntensityMapVBOffset, 2);
	
	NVSTATEMANAGER.RestoreTexture(0);
	NVSTATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLOROP);
	NVSTATEMANAGER.RestoreTextureStageState(0, D3DTSS_MINFILTER);
	NVSTATEMANAGER.RestoreTextureStageState(0, D3DTSS_MAGFILTER);
	NVSTATEMANAGER.RestoreTextureStageState(0, D3DTSS_MIPFILTER);
	NVSTATEMANAGER.RestoreVertexShader();
	NVSTATEMANAGER.RestoreRenderState(D3DRS_CULLMODE);
	NVSTATEMANAGER.RestoreRenderState(D3DRS_LIGHTING);


	return S_OK;
}

// Draw the sun itself.  If it is rendered into the sunmap then it is drawn in white.
HRESULT CTextureMasking::DrawSun(bool bSunMap)
{
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity(&matIdentity);

	NVSTATEMANAGER.SaveTexture(0, NVTextureResource::GetTexture("Sun"));
	NVSTATEMANAGER.SaveRenderState(D3DRS_TEXTUREFACTOR, 0xFFFFFFFF);
	if (bSunMap)
	{
		NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
		NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		NVSTATEMANAGER.SaveRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	}
	else
	{
		NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		NVSTATEMANAGER.SaveRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	}
	
	NVSTATEMANAGER.SaveVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	NVSTATEMANAGER.SaveRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	NVSTATEMANAGER.SaveRenderState(D3DRS_LIGHTING, FALSE);
	NVSTATEMANAGER.SaveRenderState(D3DRS_COLORVERTEX, TRUE);

	NVSTATEMANAGER.SaveRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	NVSTATEMANAGER.SaveRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	NVSTATEMANAGER.SaveRenderState(D3DRS_ALPHAREF, 0x5);
	NVSTATEMANAGER.SaveRenderState(D3DRS_ZENABLE, bSunMap ? FALSE : TRUE);

    NVSTATEMANAGER.SaveRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	NVSTATEMANAGER.SaveRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// Draw sun
	NVSTATEMANAGER.SetTransform(D3DTS_WORLD, &matIdentity);
	NVSTATEMANAGER.SetStreamSource(0, m_pFlareVB, sizeof(FlareVertex));
	
	if (bSunMap)
	{
		NVSTATEMANAGER.DrawPrimitive(D3DPT_TRIANGLESTRIP, 4, 2);
	}
	else
	{
		NVSTATEMANAGER.DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	}
	
	NVSTATEMANAGER.RestoreTexture(0);
	NVSTATEMANAGER.RestoreRenderState(D3DRS_TEXTUREFACTOR);
	NVSTATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLOROP);
	NVSTATEMANAGER.RestoreTextureStageState(0, D3DTSS_ALPHAOP);
	NVSTATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLORARG1);
	NVSTATEMANAGER.RestoreTextureStageState(0, D3DTSS_ALPHAARG1);
	NVSTATEMANAGER.RestoreVertexShader();
	NVSTATEMANAGER.RestoreRenderState(D3DRS_CULLMODE);
	NVSTATEMANAGER.RestoreRenderState(D3DRS_LIGHTING);
	NVSTATEMANAGER.RestoreRenderState(D3DRS_COLORVERTEX);
	NVSTATEMANAGER.RestoreRenderState(D3DRS_ALPHABLENDENABLE);
	NVSTATEMANAGER.RestoreRenderState(D3DRS_ALPHATESTENABLE);
	NVSTATEMANAGER.RestoreRenderState(D3DRS_ALPHAFUNC);
	NVSTATEMANAGER.RestoreRenderState(D3DRS_ALPHAREF);
	NVSTATEMANAGER.RestoreRenderState(D3DRS_ZENABLE);

	NVSTATEMANAGER.RestoreRenderState(D3DRS_SRCBLEND);
	NVSTATEMANAGER.RestoreRenderState(D3DRS_DESTBLEND);

	return S_OK;
}

// Draw the lens flares, modulated by the intensity map.
HRESULT CTextureMasking::DrawFlares()
{
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity(&matIdentity);

	NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	NVSTATEMANAGER.SaveTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
	if (!m_bLockMethod)
	{
		NVSTATEMANAGER.SaveTexture(1, NVTextureResource::GetTexture("IntensityMap"));
		NVSTATEMANAGER.SaveTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	}
	else
	{
		NVSTATEMANAGER.SaveTexture(1, NULL);
		NVSTATEMANAGER.SaveTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	}
	NVSTATEMANAGER.SaveTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
	NVSTATEMANAGER.SaveTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	NVSTATEMANAGER.SaveTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	NVSTATEMANAGER.SaveTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
	NVSTATEMANAGER.SaveTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_POINT );
	NVSTATEMANAGER.SaveTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_POINT );
	NVSTATEMANAGER.SaveTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_NONE );
	NVSTATEMANAGER.SaveRenderState(D3DRS_TEXTUREFACTOR, m_dwLockedIntensity);
	
	NVSTATEMANAGER.SaveVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	NVSTATEMANAGER.SaveRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	NVSTATEMANAGER.SaveRenderState(D3DRS_LIGHTING, FALSE);
	NVSTATEMANAGER.SaveRenderState(D3DRS_COLORVERTEX, TRUE);
    NVSTATEMANAGER.SaveRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	NVSTATEMANAGER.SaveRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	NVSTATEMANAGER.SaveRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	NVSTATEMANAGER.SaveRenderState(D3DRS_ALPHAREF, 0x10);

    NVSTATEMANAGER.SaveRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	NVSTATEMANAGER.SaveRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	NVSTATEMANAGER.SaveRenderState(D3DRS_ZENABLE, FALSE);
	
	NVSTATEMANAGER.SetTransform(D3DTS_WORLD, &matIdentity);
	NVSTATEMANAGER.SetStreamSource(0, m_pFlareVB, sizeof(FlareVertex));
	
	// Draw flares without z enabled
	tFlareInfoList::iterator itrFlares = m_FlareInfo.begin();
	while (itrFlares != m_FlareInfo.end())
	{

		NVSTATEMANAGER.SetTexture(0, NVTextureResource::GetTexture((*itrFlares).m_strFlareName.c_str()));
		NVSTATEMANAGER.DrawPrimitive(D3DPT_TRIANGLESTRIP, 8 + ((itrFlares - m_FlareInfo.begin()) * 4), 2);
		itrFlares++;
	}	

	NVSTATEMANAGER.RestoreTexture(0);
	NVSTATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLOROP);
	NVSTATEMANAGER.RestoreTextureStageState(0, D3DTSS_ALPHAOP);

	NVSTATEMANAGER.RestoreTexture(1);
	NVSTATEMANAGER.RestoreTextureStageState(1, D3DTSS_COLOROP);
	NVSTATEMANAGER.RestoreTextureStageState(1, D3DTSS_COLORARG1);
	NVSTATEMANAGER.RestoreTextureStageState(1, D3DTSS_COLORARG2);
	NVSTATEMANAGER.RestoreTextureStageState(1, D3DTSS_ALPHAOP);
	NVSTATEMANAGER.RestoreTextureStageState(1, D3DTSS_ALPHAARG1);
	NVSTATEMANAGER.RestoreTextureStageState(1, D3DTSS_ALPHAARG2);
	NVSTATEMANAGER.RestoreTextureStageState(1, D3DTSS_MINFILTER);
	NVSTATEMANAGER.RestoreTextureStageState(1, D3DTSS_MAGFILTER);
	NVSTATEMANAGER.RestoreTextureStageState(1, D3DTSS_MIPFILTER);
	NVSTATEMANAGER.RestoreRenderState(D3DRS_TEXTUREFACTOR);

	NVSTATEMANAGER.RestoreVertexShader();
	NVSTATEMANAGER.RestoreRenderState(D3DRS_CULLMODE);
	NVSTATEMANAGER.RestoreRenderState(D3DRS_LIGHTING);
	NVSTATEMANAGER.RestoreRenderState(D3DRS_COLORVERTEX);
	NVSTATEMANAGER.RestoreRenderState(D3DRS_ALPHABLENDENABLE);
	NVSTATEMANAGER.RestoreRenderState(D3DRS_ALPHATESTENABLE);
	NVSTATEMANAGER.RestoreRenderState(D3DRS_ALPHAFUNC);
	NVSTATEMANAGER.RestoreRenderState(D3DRS_ALPHAREF);
	
	NVSTATEMANAGER.RestoreRenderState(D3DRS_SRCBLEND);
	NVSTATEMANAGER.RestoreRenderState(D3DRS_DESTBLEND);
	NVSTATEMANAGER.RestoreRenderState(D3DRS_ZENABLE);

	return S_OK;
}

// This function calculates the lens flare visibility using the lock call
// The option is available to just do the lock without the reads
// to compare lock overhead versus lock + read overhead.
HRESULT CTextureMasking::CalculateLensFlareVisibility()
{
	LPDIRECT3DSURFACE8 pBackBuffer;
	DWORD* p32Bits;
	WORD* p16Bits;
	HRESULT hr;
	DWORD x1,y1,x2,y2, x, y;
	DWORD dwIntensity = 0;
	x1 = m_SunScreenTopLeft.x;
	y1 = m_SunScreenTopLeft.y;
	x2 = m_SunScreenBottomRight.x;
	y2 = m_SunScreenBottomRight.y;

	// Get the render target
	D3DSURFACE_DESC Desc;
	hr = m_pd3dDevice->GetRenderTarget(&pBackBuffer);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// Lock the buffer
	D3DLOCKED_RECT LockData;
	hr = pBackBuffer->LockRect(&LockData, NULL, 0);
	if (FAILED(hr))
	{
		SAFE_RELEASE(pBackBuffer);
		return E_FAIL;
	}

	// read back and count any pixels brighter than the sky as
	// sun pixels.
	if (m_bReadBuffer)
	{
		pBackBuffer->GetDesc(&Desc);

		switch(Desc.Format)
		{
			case D3DFMT_A8R8G8B8:
			case D3DFMT_X8R8G8B8:
			{
				for (y = y1; y < y2; y++)
				{
					p32Bits = (DWORD*)((BYTE*)LockData.pBits + (y * LockData.Pitch));
					for (x = x1; x < x2; x++)
					{
						if ((p32Bits[x] & 0xFFFFFF) > m_dwSkyColor)
							dwIntensity++;
					}
				}
			}
			break;

			case D3DFMT_R5G6B5:
			{
				for (y = y1; y < y2; y++)
				{
					p16Bits = (WORD*)((BYTE*)LockData.pBits + (y * LockData.Pitch));
					for (x = x1; x < x2; x++)
					{
						if ((p16Bits[x]) > m_dwSkyColor565)
							dwIntensity++;
					}
				}
			}
			break;

			case D3DFMT_X1R5G5B5:
			case D3DFMT_A1R5G5B5:
			{
				for (y = y1; y < y2; y++)
				{
					p16Bits = (WORD*)((BYTE*)LockData.pBits + (y * LockData.Pitch));
					for (x = x1; x < x2; x++)
					{
						if ((p16Bits[x] & 0x7FFF) > m_dwSkyColor5551)
							dwIntensity++;
					}
				}
			}
			break;
			
			default:
				NVASSERT(0,"Don't understand backbuffer format!");
				break;
		}
		// As in the masking method, we want to count sun pixels, so we increase the intensity by the same
		// amount.  The effect still isn't quite the same because we are doing fundamenatally different things,
		// with different areas of coverage.
		float fSunArea = ((x2 - x1) * (y2 - y1)) * (m_fSunShrink);

		// Divide by the area.
		float fIntensity = (float)dwIntensity / fSunArea;
		
		// Fill a DWORD with the ratio of sun pixels visible
		m_dwLockedIntensity = (DWORD)(fIntensity * 255.0f);
		m_dwLockedIntensity |= ((m_dwLockedIntensity & 0xFF) << 8);
		m_dwLockedIntensity |= ((m_dwLockedIntensity & 0xFF) << 16);
		m_dwLockedIntensity |= ((m_dwLockedIntensity & 0xFF) << 24);
	}
	else
	{
		m_dwLockedIntensity = 0xFFFFFFFF;
	}

	pBackBuffer->UnlockRect();
	SAFE_RELEASE(pBackBuffer);

	return S_OK;
}

// The main part of the demo - generate a texture map containing an intensity value based on the
// visibility of the sun.
HRESULT CTextureMasking::DrawLensFlareVisibility()
{
	HRESULT hr;
	LPDIRECT3DSURFACE8 pBackBuffer, pZBuffer, pSunMap, pIntensityMap;

	// Keep copies of the back and z buffer
	m_pd3dDevice->GetRenderTarget(&pBackBuffer);
	m_pd3dDevice->GetDepthStencilSurface(&pZBuffer);

	// Get pointers to the top-level surfaces of the sun map and the intensity map
	hr = ((LPDIRECT3DTEXTURE8)NVTextureResource::GetTexture("SunMap"))->GetSurfaceLevel(0, &pSunMap);
	if (FAILED(hr))
		return hr;

	hr = ((LPDIRECT3DTEXTURE8)NVTextureResource::GetTexture("IntensityMap"))->GetSurfaceLevel(0, &pIntensityMap);
	if (FAILED(hr))
		return hr;

	// Swap in the sun camera, which looks directly at the sun with a narrow projection angle
	NVWalkCamera* pCurrentCamera = m_pCamera;
	m_pCamera = m_pSunCamera;

	// We render to the sun map first of all
	hr = m_pd3dDevice->SetRenderTarget(pSunMap, NULL);
	if (FAILED(hr))
		return hr;

	// Clear to black
	m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0L);

	// Setup projection/view matrices based on sun camera
	NVSTATEMANAGER.SetTransform(D3DTS_VIEW, &m_pSunCamera->GetCameraMatrix());
	NVSTATEMANAGER.SetTransform(D3DTS_PROJECTION, &m_pSunCamera->GetProjectionMatrix());
	
	// Draw into the sun map with z off - we only want occlusion info, not z data
	NVSTATEMANAGER.SetRenderState(D3DRS_ZENABLE, FALSE);
	DrawSun(true);
	DrawTrees(true);
	NVSTATEMANAGER.SetRenderState(D3DRS_ZENABLE, TRUE);

	// Now render to the intensity map
	hr = m_pd3dDevice->SetRenderTarget(pIntensityMap, NULL);
	if (FAILED(hr))
		return hr;

	// Clear intensity map to black
	m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0x0, 1.0f, 0L);

	NVSTATEMANAGER.SaveTexture(0, NVTextureResource::GetTexture("SunMap"));
	NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
	NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT );
	NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT );
	NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE );
	NVSTATEMANAGER.SaveRenderState(D3DRS_TEXTUREFACTOR, 0x01010101);
	NVSTATEMANAGER.SaveVertexShader(D3DFVF_XYZRHW | D3DFVF_TEX1);
	NVSTATEMANAGER.SaveRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	NVSTATEMANAGER.SaveRenderState(D3DRS_LIGHTING, FALSE);
    NVSTATEMANAGER.SaveRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	NVSTATEMANAGER.SaveRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	NVSTATEMANAGER.SaveRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	NVSTATEMANAGER.SaveRenderState(D3DRS_ZENABLE, FALSE);

	// Draw the intensity samples to the intensity map
	NVSTATEMANAGER.SetStreamSource(0, m_pMappingVB, sizeof(MappingVertexTL));
	NVSTATEMANAGER.DrawPrimitive(D3DPT_POINTLIST, m_dwIntensityRenderVBOffset, NUM_SUNMAP_SAMPLES);
	
	NVSTATEMANAGER.RestoreTexture(0);
	NVSTATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLOROP);
	NVSTATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLORARG1);
	NVSTATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLORARG2);
	NVSTATEMANAGER.RestoreTextureStageState(0, D3DTSS_MINFILTER);
	NVSTATEMANAGER.RestoreTextureStageState(0, D3DTSS_MAGFILTER);
	NVSTATEMANAGER.RestoreTextureStageState(0, D3DTSS_MIPFILTER);
	NVSTATEMANAGER.RestoreRenderState(D3DRS_TEXTUREFACTOR);
	NVSTATEMANAGER.RestoreVertexShader();
	NVSTATEMANAGER.RestoreRenderState(D3DRS_CULLMODE);
	NVSTATEMANAGER.RestoreRenderState(D3DRS_LIGHTING);
    NVSTATEMANAGER.RestoreRenderState(D3DRS_SRCBLEND);
	NVSTATEMANAGER.RestoreRenderState(D3DRS_DESTBLEND);
	NVSTATEMANAGER.RestoreRenderState(D3DRS_ALPHABLENDENABLE);
	NVSTATEMANAGER.RestoreRenderState(D3DRS_ZENABLE);

	// Restore main render target and free references to buffers
	hr = m_pd3dDevice->SetRenderTarget(pBackBuffer, pZBuffer);

	SAFE_RELEASE(pBackBuffer);
	SAFE_RELEASE(pZBuffer);
	SAFE_RELEASE(pSunMap);
	SAFE_RELEASE(pIntensityMap);

	// Restore the camera to the main view camera
	m_pCamera = pCurrentCamera;
	NVSTATEMANAGER.SetTransform(D3DTS_VIEW, &m_pCamera->GetCameraMatrix());
	NVSTATEMANAGER.SetTransform(D3DTS_PROJECTION, &m_pCamera->GetProjectionMatrix());

	return S_OK;
}

// Prepares the sun flare data.
// First check is to see if we need to do this at all.
// Then we calculate the position of the sun on the far clip plane, and the center of the far clip plane.
// We create an axis through these points to draw the flare with.
// We also create two billboard data sets - the billboard for the sun when looking straight at it, 
// and the billboard for the sun when looking at it from the point of view of the viewer.
HRESULT CTextureMasking::PrepareSunAndFlare()
{
	D3DXVECTOR3 vecX, vecY;
	D3DXVECTOR3 vecSxViewer, vecSyViewer;
	D3DXVECTOR3 vecSxSun, vecSySun;
	D3DXMATRIX matViewerLocation;
	HRESULT hr;

    // Figure out of light (or flare) might be visible.
	float fDot = D3DXVec3Dot(&m_SunDirection, &m_pCamera->GetCameraLookDirection());
	if (fDot <= 0.00001f)
	{
		m_bDrawFlare = false;
		return S_OK;
	}
	m_bDrawFlare = true;

    D3DXVECTOR3 Axis, vecDx, vecDy;

    // Calculate the point directly in front of us, on the far clip plane
    m_FarPlaneCenter = m_pCamera->GetCameraPosition() + (m_pCamera->GetCameraLookDirection() * m_fSunDistance);

    // Calculate position of light on the far clip plane
    m_FarPlaneLight = m_pCamera->GetCameraPosition() + (m_SunDirection * (m_fSunDistance / fDot));

    // Compute axis which goes from light through the center of the screen
	// This is used to align the flares along the sun axis.
    Axis = m_FarPlaneCenter - m_FarPlaneLight;
    D3DXVec3Normalize(&vecDx, &Axis);
    D3DXVec3Cross(&vecDy, &vecDx, &m_pCamera->GetCameraLookDirection());

	// Get the viewer's location in world space
	D3DXMatrixInverse(&matViewerLocation, NULL, &m_pCamera->GetCameraMatrix());

	// If doing the texture masking method we need to calculate a sun camera
	if (!m_bLockMethod)
	{
		// Calculate the sun camera which looks directly at the sun.
		// first get required azimuth and altitude angles
		float fAltitudeAngle, fAzimuthAngle;
		D3DXVECTOR3 ZAxis = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
		D3DXVECTOR3 XAxis = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

		D3DXVECTOR3 NormalizedDirection;
		D3DXVec3Normalize(&NormalizedDirection, &m_SunDirection);

		D3DXVECTOR3 SunZ = D3DXVECTOR3(0.0f, NormalizedDirection.y, NormalizedDirection.z);
		D3DXVECTOR3 SunX = D3DXVECTOR3(NormalizedDirection.x, 0.0f, NormalizedDirection.z);
		D3DXVec3Normalize(&SunZ, &SunZ);
		D3DXVec3Normalize(&SunX, &SunX);
		fAzimuthAngle = -acos(D3DXVec3Dot(&SunX, &ZAxis));
		fAltitudeAngle = acos(D3DXVec3Dot(&SunZ, &ZAxis));

		// Work out projection angle
		D3DXMATRIX ProjectSun;
		D3DXVECTOR3 CenterSunDirection = m_SunDirection;
		D3DXVECTOR3 OuterSunDirection = D3DXVECTOR3(m_FarPlaneLight.x + m_fSunRadius, m_FarPlaneLight.y + m_fSunRadius, m_FarPlaneLight.z) - m_pCamera->GetCameraPosition();
		D3DXVec3Normalize(&CenterSunDirection, &CenterSunDirection);
		D3DXVec3Normalize(&OuterSunDirection, &OuterSunDirection);
		float fProjectionAngle = 2.0f * acos(D3DXVec3Dot(&CenterSunDirection, &OuterSunDirection));
		
		// Make the sun a bit fatter than the sun map to try to fill it better.
		fProjectionAngle *= m_fSunShrink;
		D3DXMatrixPerspectiveFovLH( &ProjectSun, fProjectionAngle, 1.0f, NEAR_PLANE, FAR_PLANE + (m_fSunRadius * 2.0f));

		// Create a camera looking at the sun
		*m_pSunCamera = NVWalkCamera(ProjectSun, m_pCamera->GetCameraPosition(), fAzimuthAngle, fAltitudeAngle);

		// Figure out screen X and Y axes in world coordinates for the sun billboard
		// from the point of view of the viewer looking at the sun
		D3DXMatrixInverse(&matViewerLocation, NULL, &m_pSunCamera->GetCameraMatrix());
		vecX = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	    D3DXVec3TransformNormal(&vecX, &vecX, &matViewerLocation);
		D3DXVec3Normalize(&vecX, &vecX);
	    D3DXVec3Cross(&vecY, &vecX, &m_pSunCamera->GetCameraLookDirection());
		vecSxSun = vecX * m_fSunRadius;
		vecSySun = vecY * m_fSunRadius;
	}

    // Figure out screen X and Y axes in world coordinates for the sun billboard
	// from the point of view of the viewer looking through the camera
	vecX = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
    D3DXVec3TransformNormal(&vecX, &vecX, &matViewerLocation);
    D3DXVec3Normalize(&vecX, &vecX);
    D3DXVec3Cross(&vecY, &vecX, &m_pCamera->GetCameraLookDirection());
	vecSxViewer = vecX * m_fSunRadius;
    vecSyViewer = vecY * m_fSunRadius;

	// If doing the lock method we need to know where to look in the framebuffer for the sun pixels
	if (m_bLockMethod)
	{
		// Calculate the coordinates in screen space of the flare for the locking method
		m_SunScreenTopLeft = D3DXVECTOR3(m_FarPlaneLight.x - vecSxViewer.x - vecSyViewer.x,
									m_FarPlaneLight.y - vecSxViewer.y - vecSyViewer.y,
									m_FarPlaneLight.z - vecSxViewer.z - vecSyViewer.z);
		m_SunScreenBottomRight = D3DXVECTOR3(m_FarPlaneLight.x + vecSxViewer.x + vecSyViewer.x,
									m_FarPlaneLight.y + vecSxViewer.y + vecSyViewer.y,
									m_FarPlaneLight.z + vecSxViewer.z + vecSyViewer.z);

		D3DXMATRIX matTransform = m_pCamera->GetCameraMatrix() * m_pCamera->GetProjectionMatrix();
		
		D3DXVec3TransformCoord(&m_SunScreenTopLeft, &m_SunScreenTopLeft, &matTransform);
		D3DXVec3TransformCoord(&m_SunScreenBottomRight, &m_SunScreenBottomRight, &matTransform);

		// Scale up to the viewport - assumed to be the size of the window.
		// NOTE: We can't query for the viewport on the PURE device, so we do this instead.
		// We know that our viewport matches the window, so it isn't a problem
		m_SunScreenTopLeft *= 0.5f;
		m_SunScreenBottomRight *= 0.5f;

		m_SunScreenTopLeft.x += 0.5f;
		m_SunScreenBottomRight.x += 0.5f;
		m_SunScreenTopLeft.y += 0.5f;
		m_SunScreenBottomRight.y += 0.5f;

		m_SunScreenTopLeft.x *= m_d3dsdBackBuffer.Width;
		m_SunScreenBottomRight.x *= m_d3dsdBackBuffer.Width;

		m_SunScreenTopLeft.y *= m_d3dsdBackBuffer.Height;
		m_SunScreenBottomRight.y *= m_d3dsdBackBuffer.Height;

		// Clamp to the screen
		if (m_SunScreenTopLeft.x < 0.0f)
			m_SunScreenTopLeft.x = 0.0f;
		if (m_SunScreenTopLeft.x > m_d3dsdBackBuffer.Width)
			m_SunScreenTopLeft.x = m_d3dsdBackBuffer.Width;

		if (m_SunScreenTopLeft.y < 0.0f)
			m_SunScreenTopLeft.y = 0.0f;
		if (m_SunScreenTopLeft.y > m_d3dsdBackBuffer.Height)
			m_SunScreenTopLeft.y = m_d3dsdBackBuffer.Height;
		
		if (m_SunScreenBottomRight.x < 0.0f)
			m_SunScreenBottomRight.x = 0.0f;
		if (m_SunScreenBottomRight.x > m_d3dsdBackBuffer.Width)
			m_SunScreenBottomRight.x = m_d3dsdBackBuffer.Width;
		
		if (m_SunScreenBottomRight.y < 0.0f)
			m_SunScreenBottomRight.y = 0.0f;
		if (m_SunScreenBottomRight.y > m_d3dsdBackBuffer.Height)
			m_SunScreenBottomRight.y = m_d3dsdBackBuffer.Height;

		m_SunScreenBottomRight.y = m_d3dsdBackBuffer.Height - m_SunScreenBottomRight.y;
		m_SunScreenTopLeft.y = m_d3dsdBackBuffer.Height - m_SunScreenTopLeft.y;

	}

	// Create the sun and flare vertices
	FlareVertex* pFlare;
	

	// Create vertices, in a dynamic VB for speed.
	// In a real app we would be using this VB for other things.
	hr = m_pFlareVB->Lock(0, ((2 + m_FlareInfo.size()) * 4 * sizeof(FlareVertex)), (BYTE**)&pFlare, D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK);
	if (SUCCEEDED(hr))
	{
		DWORD dwVertexIndex = 0;

		// Vertices to draw the sun from the point of view of the viewer's camera
		pFlare[dwVertexIndex].m_Position.x = m_FarPlaneLight.x - vecSxViewer.x - vecSyViewer.x;
		pFlare[dwVertexIndex].m_Position.y = m_FarPlaneLight.y - vecSxViewer.y - vecSyViewer.y;
		pFlare[dwVertexIndex].m_Position.z = m_FarPlaneLight.z - vecSxViewer.z - vecSyViewer.z;
		pFlare[dwVertexIndex].m_Texture = D3DXVECTOR2(0.0f, 1.0f);
		pFlare[dwVertexIndex].m_dwColor = m_SunColor;
		dwVertexIndex++;

		pFlare[dwVertexIndex].m_Position.x = m_FarPlaneLight.x - vecSxViewer.x + vecSyViewer.x;
		pFlare[dwVertexIndex].m_Position.y = m_FarPlaneLight.y - vecSxViewer.y + vecSyViewer.y;
		pFlare[dwVertexIndex].m_Position.z = m_FarPlaneLight.z - vecSxViewer.z + vecSyViewer.z;
		pFlare[dwVertexIndex].m_Texture = D3DXVECTOR2(0.0f, 0.0f);
		pFlare[dwVertexIndex].m_dwColor = m_SunColor;
		dwVertexIndex++;

		pFlare[dwVertexIndex].m_Position.x = m_FarPlaneLight.x + vecSxViewer.x - vecSyViewer.x;
		pFlare[dwVertexIndex].m_Position.y = m_FarPlaneLight.y + vecSxViewer.y - vecSyViewer.y;
		pFlare[dwVertexIndex].m_Position.z = m_FarPlaneLight.z + vecSxViewer.z - vecSyViewer.z;
		pFlare[dwVertexIndex].m_Texture = D3DXVECTOR2(1.0f, 1.0f);
		pFlare[dwVertexIndex].m_dwColor = m_SunColor;
		dwVertexIndex++;

		pFlare[dwVertexIndex].m_Position.x = m_FarPlaneLight.x + vecSxViewer.x + vecSyViewer.x;
		pFlare[dwVertexIndex].m_Position.y = m_FarPlaneLight.y + vecSxViewer.y + vecSyViewer.y;
		pFlare[dwVertexIndex].m_Position.z = m_FarPlaneLight.z + vecSxViewer.z + vecSyViewer.z;
		pFlare[dwVertexIndex].m_Texture = D3DXVECTOR2(1.0f, 0.0f);
		pFlare[dwVertexIndex].m_dwColor = m_SunColor;
		dwVertexIndex++;

		// Vertices to draw the sun from the point of view of the sun camera
		if (!m_bLockMethod)
		{
			pFlare[dwVertexIndex].m_Position.x = m_FarPlaneLight.x - vecSxSun.x - vecSySun.x;
			pFlare[dwVertexIndex].m_Position.y = m_FarPlaneLight.y - vecSxSun.y - vecSySun.y;
			pFlare[dwVertexIndex].m_Position.z = m_FarPlaneLight.z - vecSxSun.z - vecSySun.z;
			pFlare[dwVertexIndex].m_Texture = D3DXVECTOR2(0.0f, 1.0f);
			pFlare[dwVertexIndex].m_dwColor = m_SunColor;
			dwVertexIndex++;

			pFlare[dwVertexIndex].m_Position.x = m_FarPlaneLight.x - vecSxSun.x + vecSySun.x;
			pFlare[dwVertexIndex].m_Position.y = m_FarPlaneLight.y - vecSxSun.y + vecSySun.y;
			pFlare[dwVertexIndex].m_Position.z = m_FarPlaneLight.z - vecSxSun.z + vecSySun.z;
			pFlare[dwVertexIndex].m_Texture = D3DXVECTOR2(0.0f, 0.0f);
			pFlare[dwVertexIndex].m_dwColor = m_SunColor;
			dwVertexIndex++;

			pFlare[dwVertexIndex].m_Position.x = m_FarPlaneLight.x + vecSxSun.x - vecSySun.x;
			pFlare[dwVertexIndex].m_Position.y = m_FarPlaneLight.y + vecSxSun.y - vecSySun.y;
			pFlare[dwVertexIndex].m_Position.z = m_FarPlaneLight.z + vecSxSun.z - vecSySun.z;
			pFlare[dwVertexIndex].m_Texture = D3DXVECTOR2(1.0f, 1.0f);
			pFlare[dwVertexIndex].m_dwColor = m_SunColor;
			dwVertexIndex++;

			pFlare[dwVertexIndex].m_Position.x = m_FarPlaneLight.x + vecSxSun.x + vecSySun.x;
			pFlare[dwVertexIndex].m_Position.y = m_FarPlaneLight.y + vecSxSun.y + vecSySun.y;
			pFlare[dwVertexIndex].m_Position.z = m_FarPlaneLight.z + vecSxSun.z + vecSySun.z;
			pFlare[dwVertexIndex].m_Texture = D3DXVECTOR2(1.0f, 0.0f);
			pFlare[dwVertexIndex].m_dwColor = m_SunColor;
			dwVertexIndex++;
		}
		else
		{
			dwVertexIndex += 4;
		}

		// Setup the flares
		tFlareInfoList::iterator itrFlares = m_FlareInfo.begin();
		while (itrFlares != m_FlareInfo.end())
		{
			D3DXCOLOR FlareColor = (*itrFlares).m_FlareColor;
			FlareColor.a *= fDot;
			D3DXVECTOR3 Flare;

			Flare = Axis * (*itrFlares).m_fPosition + m_FarPlaneCenter;

			vecSxViewer = vecDx * (*itrFlares).m_fRadius * m_fSunRadius * fDot;
			vecSyViewer = vecDy * (*itrFlares).m_fRadius * m_fSunRadius * fDot;

			pFlare[dwVertexIndex].m_Position.x = Flare.x - vecSxViewer.x - vecSyViewer.x;
			pFlare[dwVertexIndex].m_Position.y = Flare.y - vecSxViewer.y - vecSyViewer.y;
			pFlare[dwVertexIndex].m_Position.z = Flare.z - vecSxViewer.z - vecSyViewer.z;
			pFlare[dwVertexIndex].m_Texture = D3DXVECTOR2(0.0f, 1.0f);
			pFlare[dwVertexIndex].m_dwColor = FlareColor;
			dwVertexIndex++;

			pFlare[dwVertexIndex].m_Position.x = Flare.x - vecSxViewer.x + vecSyViewer.x;
			pFlare[dwVertexIndex].m_Position.y = Flare.y - vecSxViewer.y + vecSyViewer.y;
			pFlare[dwVertexIndex].m_Position.z = Flare.z - vecSxViewer.z + vecSyViewer.z;
			pFlare[dwVertexIndex].m_Texture = D3DXVECTOR2(0.0f, 0.0f);
			pFlare[dwVertexIndex].m_dwColor = FlareColor;
			dwVertexIndex++;

			pFlare[dwVertexIndex].m_Position.x = Flare.x + vecSxViewer.x - vecSyViewer.x;
			pFlare[dwVertexIndex].m_Position.y = Flare.y + vecSxViewer.y - vecSyViewer.y;
			pFlare[dwVertexIndex].m_Position.z = Flare.z + vecSxViewer.z - vecSyViewer.z;
			pFlare[dwVertexIndex].m_Texture = D3DXVECTOR2(1.0f, 1.0f);
			pFlare[dwVertexIndex].m_dwColor = FlareColor;
			dwVertexIndex++;

			pFlare[dwVertexIndex].m_Position.x = Flare.x + vecSxViewer.x + vecSyViewer.x;
			pFlare[dwVertexIndex].m_Position.y = Flare.y + vecSxViewer.y + vecSyViewer.y;
			pFlare[dwVertexIndex].m_Position.z = Flare.z + vecSxViewer.z + vecSyViewer.z;
			pFlare[dwVertexIndex].m_Texture = D3DXVECTOR2(1.0f, 0.0f);
			pFlare[dwVertexIndex].m_dwColor = FlareColor;
			dwVertexIndex++;

			itrFlares++;
		}
		m_pFlareVB->Unlock();
	}

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    CTextureMasking d3dApp;

    if( FAILED( d3dApp.Create( hInst ) ) )
        return 0;

    return d3dApp.Run();
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CTextureMasking::Render()
{
	HRESULT hr = S_OK;
	DWORD i;
	bool bFailedLock = false;
	DWORD LOD;

	// Allow the camera to move
	m_pCamera->CheckKeyboard(m_hWnd);

	D3DXVECTOR3 CameraPosition = m_pCamera->GetCameraPosition();

    // Begin the scene 
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {

		NVSTATEMANAGER.SetRenderState(D3DRS_FILLMODE, m_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID);

		// Setup the camera
		NVSTATEMANAGER.SetTransform(D3DTS_VIEW, &m_pCamera->GetCameraMatrix());
		NVSTATEMANAGER.SetTransform(D3DTS_PROJECTION, &m_pCamera->GetProjectionMatrix());

		// Cleare the back and z buffer
		m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, m_dwSkyColor, 1.0f, 0L);

		// Reset the poly drawing statistics
		for (i = 0; i < NUM_VEGETATIONTYPES; i++)
		{
			for (LOD = 0; LOD < m_NumLODS[i]; LOD++)
			{
				m_pObjectMesh[i][LOD]->ResetStats();
			}
		}

		// Draw the regular scene objects
		DrawFloor();
		DrawTrees(false);
		
		// Do the lens flare
		PrepareSunAndFlare();

		// Draw the sun
		DrawSun(false);

		// If the flare is visible, calculate it's visibility and draw the flares
		if (m_bDrawFlare)
		{
			if (!m_bLockMethod)
			{
				DrawLensFlareVisibility();
			}
			else
			{
				hr = CalculateLensFlareVisibility();
				if (FAILED(hr))
				{
					bFailedLock = true;
				}
			}
			
			DrawFlares();
		}

		if (!m_bLockMethod)
		{
			// Show the intensity & sunmaps if selected
			if (m_bShowSunMap)
			{
				DrawSunMap();
			}
			if (m_bShowIntensityMap)
			{
				DrawIntensityMap();
			}
		}

		// Add up all the polys drawn
		m_dwNumTriangles = 0;
		m_dwNumVertices = 0;
		for (i = 0; i < NUM_VEGETATIONTYPES; i++)
		{
			for (LOD = 0; LOD < m_NumLODS[i]; LOD++)
			{
				m_dwNumTriangles += m_pObjectMesh[i][LOD]->GetTrianglesDrawn();
				m_dwNumVertices += m_pObjectMesh[i][LOD]->GetVerticesDrawn();
			}
		}

		// A random piece of CPU work
		for (LONG Delay = 0; Delay < m_lSlugAmount; Delay++)
		{
			volatile DWORD Slug = m_lSlugAmount + Delay;
			m_dwSlugTemporary = Slug;
		}

		// Display stats
		TCHAR strFrameStats[40];
		DWORD dwStep = m_pText->GetFontHeight() + 8;
		DWORD dwCurrentY = 0;

		m_pText->DrawText(2,  dwCurrentY, D3DCOLOR_ARGB(255,255,255,255), m_strDeviceStats, 0 );
		dwCurrentY += dwStep;

		m_pText->DrawText(2,  dwCurrentY, D3DCOLOR_ARGB(255,255,255,255), m_strFrameStats, 0 );
		dwCurrentY += dwStep;

		_stprintf(strFrameStats, _T("%0.2f MTris/sec"), ((m_fFPS * (float)m_dwNumTriangles) / 1000000.0f));
		m_pText->DrawText(2, dwCurrentY, D3DCOLOR_ARGB(255,255,255,255), strFrameStats, 0);
		dwCurrentY += dwStep;

		_stprintf(strFrameStats, _T("%.02f MIndices/sec"), ((m_fFPS * (float)m_dwNumVertices) / 1000000.0f));
		m_pText->DrawText(2, dwCurrentY, D3DCOLOR_ARGB(255,255,255,255), strFrameStats, 0);
		dwCurrentY += dwStep;

		_stprintf(strFrameStats, _T("CPU Work: %d"), m_lSlugAmount);
		m_pText->DrawText(2, dwCurrentY, D3DCOLOR_ARGB(255,255,255,255), strFrameStats, 0);
		dwCurrentY += dwStep;


        // End the scene.
        m_pd3dDevice->EndScene();

	}

	if (bFailedLock)
	{
		m_bLockMethod = false;
		MessageBox(m_hWnd, "Could not lock backbuffer - returning to masking method", "WARNING", MB_ICONEXCLAMATION | MB_OK);
		SetMenuItems();
	}

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CTextureMasking::RestoreDeviceObjects()
{
	NVSTATEMANAGER.SetDefaultState();

	NVSTATEMANAGER.SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	NVSTATEMANAGER.SetRenderState(D3DRS_LIGHTING, TRUE);
	NVSTATEMANAGER.SetRenderState(D3DRS_COLORVERTEX, FALSE);
	NVSTATEMANAGER.SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	NVSTATEMANAGER.SetRenderState(D3DRS_LOCALVIEWER, FALSE);
	NVSTATEMANAGER.SetRenderState(D3DRS_CLIPPING, FALSE);
	NVSTATEMANAGER.SetRenderState(D3DRS_AMBIENT, 0xFF666666);

	m_pText->RestoreDeviceObjects();

	// Create vertex buffer for the flare
	HRESULT hr = m_pd3dDevice->CreateVertexBuffer( ((2 + m_FlareInfo.size()) * 4 * sizeof(FlareVertex)),
												D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0,
                                                D3DPOOL_DEFAULT, &m_pFlareVB);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

    D3DLIGHT8 light;
    ZeroMemory( &light, sizeof(D3DLIGHT8) );
    light.Type       = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r  = 0.6f;
    light.Diffuse.g  = 0.6f;
    light.Diffuse.b  = 0.6f;
	light.Ambient.r = 0.0f;
	light.Ambient.g = 0.0f;
	light.Ambient.b = 0.0f;
    light.Direction = -m_SunDirection;
    light.Range = 0.0f;
    m_pd3dDevice->SetLight(0, &light);
	m_pd3dDevice->LightEnable(0, true);

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.
//-----------------------------------------------------------------------------
HRESULT CTextureMasking::InvalidateDeviceObjects()
{
	m_pText->InvalidateDeviceObjects();
	
	NVRESOURCEMANAGER.DisposeDeviceObjects();
	
	SAFE_RELEASE(m_pFlareVB);
	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.
//-----------------------------------------------------------------------------
HRESULT CTextureMasking::DeleteDeviceObjects()
{
	SAFE_DELETE(m_pCamera);
	SAFE_DELETE(m_pSunCamera);
	SAFE_RELEASE(m_pFlareVB);

	for (DWORD i = 0; i < NUM_VEGETATIONTYPES; i++)
	{
		for (DWORD LOD = 0; LOD < MAX_LODS; LOD++)
		{
			SAFE_DELETE(m_pObjectMesh[i][LOD]);
		}
	}

	// Final step, release the resource and state managers
	SAFE_DELETE(g_pNVStateManager);
	SAFE_DELETE(g_pNVResourceManager);

	SAFE_DELETE(m_pText);

	SAFE_RELEASE(m_pFloorVB);
	SAFE_RELEASE(m_pMappingVB);
	

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT CTextureMasking::FinalCleanup()
{


    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT CTextureMasking::OneTimeSceneInit()
{
	SetMenuItems();

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CTextureMasking::FrameMove()
{

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: Called during device intialization, this code checks the device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
HRESULT CTextureMasking::ConfirmDevice( D3DCAPS8* pCaps, DWORD dwBehavior,
                                          D3DFORMAT Format )
{
    // If this is a TnL device, make sure it supports directional lights
    if( (dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING ) ||
        (dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING ) )
    {
        if( !(pCaps->VertexProcessingCaps & D3DVTXPCAPS_DIRECTIONALLIGHTS ) )
            return E_FAIL;
    }

	// Need dual texturing
    if( pCaps->MaxSimultaneousTextures < 2 )
        return E_FAIL;

    // Check that we can create a a8r8g8b8 texture that we can render into
    if( FAILED( m_pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, 
        pCaps->DeviceType, Format, D3DUSAGE_RENDERTARGET,
        D3DRTYPE_TEXTURE, D3DFMT_A8R8G8B8) ) )
    {
       return E_FAIL;
    }



    return S_OK;
}

void CTextureMasking::SetMenuItems()
{
	HMENU hMenu = GetMenu(m_hWnd);

	// Checks
	CheckMenuItem(hMenu, IDM_WIREFRAME, MF_BYCOMMAND | m_bWireframe ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hMenu, IDM_SHOWSUNMAP, MF_BYCOMMAND | m_bShowSunMap ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hMenu, IDM_SHOWINTENSITYMAP, MF_BYCOMMAND | m_bShowIntensityMap ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hMenu, IDM_SETLOCKMETHOD, MF_BYCOMMAND | m_bLockMethod ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hMenu, IDM_SETMASKINGMETHOD, MF_BYCOMMAND | !m_bLockMethod ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hMenu, IDM_NOREADS, MF_BYCOMMAND | !m_bReadBuffer ? MF_CHECKED : MF_UNCHECKED);

	// Enables
	EnableMenuItem(hMenu, IDM_SHOWSUNMAP, MF_BYCOMMAND | !m_bLockMethod ? MF_ENABLED : MF_GRAYED);
	EnableMenuItem(hMenu, IDM_SHOWINTENSITYMAP, MF_BYCOMMAND | !m_bLockMethod ? MF_ENABLED : MF_GRAYED);
}

//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Message proc function to handle key and menu input
//-----------------------------------------------------------------------------
LRESULT CTextureMasking::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam,
                                    LPARAM lParam )
{
	switch(uMsg)
	{
		case WM_CREATE:
		{
			m_hWnd = hWnd;
		}
		break;

		case WM_COMMAND:
		{
			HMENU hMenu = GetMenu(hWnd);
			switch(LOWORD(wParam))
			{
				case IDM_INCREASECPUWORK:
					m_lSlugAmount += 100000;
					break;

				case IDM_DECREASECPUWORK:
					m_lSlugAmount -= 100000;
					if (m_lSlugAmount < 0)
						m_lSlugAmount = 0;
					break;

				case IDM_WIREFRAME:
					m_bWireframe = !m_bWireframe;
					break;

				case IDM_SHOWSUNMAP:
					m_bShowSunMap = !m_bShowSunMap;
					break;

				case IDM_SHOWINTENSITYMAP:
					m_bShowIntensityMap = !m_bShowIntensityMap;
					break;

				case IDM_SETLOCKMETHOD:
					m_bLockMethod = true;
					m_bShowSunMap = false;
					m_bShowIntensityMap = false;
					break;

				case IDM_SETMASKINGMETHOD:
					m_bLockMethod = false;
					break;

				case IDM_NOREADS:
					m_bReadBuffer = !m_bReadBuffer;
					break;

				case ID_HELP:
			        ::MessageBoxEx( m_hWnd, " Help : F1 - Help \n\n  W - Wireframe Toggle \n\n PageUp - Increase CPU Work \n\n PageDown - Decrease CPU Work \n\n Left Button & Mouse - Rotate Object\n\n Shift Left Button & Mouse - Pan Camera \n\n Ctrl Left Button & Mouse - Move Camera In & Out\n\n",
				   "Help", MB_ICONINFORMATION | MB_TASKMODAL, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ) );
					break;

				default:
					break;
			}
			SetMenuItems();
		}
		break;
	
		case WM_MOUSEMOVE:
			m_pCamera->OnMouseMove(hWnd, GET_X_MESSAGE(lParam), GET_Y_MESSAGE(lParam));
			break;

		case WM_LBUTTONDOWN:
			m_pCamera->OnLButtonDown(hWnd, GET_X_MESSAGE(lParam), GET_Y_MESSAGE(lParam));
			break;

		case WM_LBUTTONUP:
			m_pCamera->OnLButtonUp(hWnd, GET_X_MESSAGE(lParam), GET_Y_MESSAGE(lParam));
			break;


		case WM_RBUTTONDOWN:
			{
				TrackPopupMenu(GetSubMenu(GetMenu(m_hWnd), 1), TPM_LEFTALIGN | TPM_TOPALIGN, GET_X_MESSAGE(lParam), GET_Y_MESSAGE(lParam), 0, m_hWnd, NULL);
			}
			break;
		default:
			break;
	}

    return CD3DApplication::MsgProc( hWnd, uMsg, wParam, lParam );
}



