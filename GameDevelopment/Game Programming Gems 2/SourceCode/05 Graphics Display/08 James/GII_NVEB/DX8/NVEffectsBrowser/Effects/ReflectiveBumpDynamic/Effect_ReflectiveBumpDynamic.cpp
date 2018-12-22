/*********************************************************************NVMH2****
Path:  C:\DEV\devrel\NV_SDK_4\DX8\NVEffectsBrowser\Effects\ReflectiveBumpDynamic
File:  Effect_ReflectiveBumpDynamic.cpp

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/



#include "eb_effect.h"

#include "CA_Water.h"

#include "nvdevice.h"
#include "NV_Error.h"

#include "SimpleObject8.h"
#include "Dot3_util.h"
#include "Effect_ReflectiveBumpDynamic.h"

#include "Constants.h"
#include "PixelConstants.h"

#include <vector>

using namespace nv_objects;
using namespace std;


/////////////////////////////////////////////////

#define INIT_BUMPSCALE		0.12f

/////////////////////////////////////////////////

#ifndef ASSERT_IF_FAILED
	#define ASSERT_IF_FAILED( hres )	\
	{									\
		if( FAILED(hres) )				\
		   assert( false );				\
	}
#endif


/////////////////////////////////////////////////

DECLARE_EFFECT_MAIN()
DECLARE_EFFECT_COUNT(1)
DECLARE_EFFECT_CREATE_BEG()
DECLARE_EFFECT_CREATE(0, NVEffect_DynamicReflection())
DECLARE_EFFECT_CREATE_END()


///////////////////////////////////////////////////////////////////////////


HRESULT NVEffect_DynamicReflection::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
{
	if (!(pCaps->TextureCaps & D3DPTEXTURECAPS_CUBEMAP))
	{
		m_strLastError = "Device does not support cubemaps!";
		return E_FAIL;
	}

	if (!(pCaps->TextureCaps & D3DPTEXTURECAPS_PROJECTED))
	{
		m_strLastError = "Device does not support 3 element texture coordinates!";
		return E_FAIL;
	}

	if (!(pCaps->MaxTextureBlendStages >= 4))
	{
		m_strLastError = "Not enough texture blend stages!";
		return E_FAIL;
	}

	if(D3DSHADER_VERSION_MAJOR(pCaps->PixelShaderVersion) < 1)
	{
		m_strLastError = "Device does not support pixel shaders!";
		return E_FAIL;
	}

    if(!(pCaps->DevCaps & D3DDEVCAPS_RTPATCHES) )
    {
        m_strLastError = "Device does not support RTPATCHES!";
        return E_FAIL;
    }

	return S_OK;
}

#define STR_INCREASEBUMPSCALE "Bump scale increase (+)"
#define STR_DECREASEBUMPSCALE "Bump scale decrease (-)"
#define STR_RESETPATCH "Reset patch offset (HOME)"
#define STR_MOREOPTION "Hit F1 for more options!"


void NVEffect_DynamicReflection::UpdateProperties()
{
	EBEffect::UpdateProperties();

	AddProperty(new EBTriggerProperty(STR_INCREASEBUMPSCALE));
	AddProperty(new EBTriggerProperty(STR_DECREASEBUMPSCALE));
	AddProperty(new EBTriggerProperty(STR_RESETPATCH));
	AddProperty(new EBTriggerProperty(STR_MOREOPTION));

    EBEnumProperty* pEnumProp = new EBEnumProperty("Display Options", OBJECT_MEMBER(m_eDisplayOption), EBTYPE_DWORD_PROP);

	if (m_bSupportsQWVU)
		pEnumProp->AddEnumerant(new EBEnumValue(pEnumProp, "8 Bit Signed BumpMap",
		(DWORD)DISPLAY_BLINN8BITSIGNED, EBTYPE_DWORD_PROP));


	AddProperty(pEnumProp);
    
	if (m_bSupportsPatches)
	{
		AddProperty(new EBProperty("Show Bezier-Patch", OBJECT_MEMBER(m_bUsePatch), EBTYPE_BOOL_PROP));
	}

	AddProperty(new EBProperty("Wireframe", OBJECT_MEMBER(m_bWireframe), EBTYPE_BOOL_PROP));
	AddProperty(new EBProperty("Pause geometry animation", OBJECT_MEMBER(m_bPause), EBTYPE_BOOL_PROP));

	//////////////////////////////////////////////////////////////

    string aboutText = "file://";
    aboutText += GetFilePath("NVEffectsExplained.htm");
    aboutText += "#DynamicTrueReflectiveBumpMapping";
	SetAboutInfo( NULL, _T("Dynamic Normal Map"), _T(aboutText.c_str()));
	SetAboutInfo( NULL, _T("Developer Relations"), _T("http://www.nvidia.com/developer"));
	SetAboutInfo( NULL, _T("NVIDIA Corporation"), _T("http://www.nvidia.com"));
	SetAboutInfo( _T("Date"), _T("April 2001"));


	//////////////////////////////////////////////////////////////////////////
	// Add vertex & pixel shader display to the tabbed text boxes:
	// No need to GetFilePath() with these.  The Add..() function does it for you

	// Vertex shaders
	AddShaderCodeFile( SHADERTYPE_VERTEX, "Texture Space",		"blinn_reflect.nvv" );
	AddShaderCodeFile( SHADERTYPE_VERTEX, "Simple Transform",	"dot3_transform.nvv");


	// Pixel shaders
	AddShaderCodeFile( SHADERTYPE_PIXEL, "Per-pixel Bump Reflections",	"blinn_reflect.nvp");
	AddShaderCodeFile( SHADERTYPE_PIXEL, "Neighbor Sampling Setup",		"TexCoord_4_Offset.nvv");
	AddShaderCodeFile( SHADERTYPE_PIXEL, "Force: Step 1",			"NeighborForceCalc.nvp");
	AddShaderCodeFile( SHADERTYPE_PIXEL, "Force: Step 2",			"NeighborForceCalc2.nvp");
	AddShaderCodeFile( SHADERTYPE_PIXEL, "Apply Force",				"ApplyForceShader.nvp");
	AddShaderCodeFile( SHADERTYPE_PIXEL, "Apply Velocity",			"ApplyVelocityShader.nvp");
	AddShaderCodeFile( SHADERTYPE_PIXEL, "Smooth",				"EqualWeightCombine_PostMult.nvp");
	AddShaderCodeFile( SHADERTYPE_PIXEL, "Gray to NormalMap",	"NormalMapCreate2_Scale.nvp");

}


NVEffect_DynamicReflection::NVEffect_DynamicReflection()
:	m_eDisplayOption(DISPLAY_BLINN8BITSIGNED),
	m_pPatchBuffer(NULL),
	m_pVertexBuffer(NULL),
    m_pIndexBuffer(NULL),
	m_fAngle(0.0f),
	m_pNVDevice(NULL),
	m_pBumpMapQWVU(NULL),
    m_bUsePatch(true),
	m_dwBlinnPixelShader(0),
	m_dwBlinnVertexShader(0),
	m_dwTransformShader(0),
	m_dwBlinnPatchVertexShader(0),
	m_dwTransformPatchShader(0),
	m_pCubeTexture(NULL),
	m_bSupportsPatches(true),
	m_pUI(NULL),
	m_bWireframe(false),
	m_bPause(true),
	m_fBumpScale(INIT_BUMPSCALE),
	m_pCA_Water(NULL)
{
	m_strEffectName = "Dynamic Normal Map";	// A string holding the name of the effect
	m_strEffectLocation = "Effects\\Bump Mapping";
	m_strEffectPixelShader = GetFilePath("blinn_reflect.nvp");
	m_strEffectVertexShader = GetFilePath("blinn_reflect.nvv");
	m_strEffectVersion  = "1.0";
}

NVEffect_DynamicReflection::~NVEffect_DynamicReflection()
{
	Free();	
}



#define TEX_SCALE 3.0f

HRESULT NVEffect_DynamicReflection::GenerateSphere(D3DXVECTOR3& vCenter, FLOAT fRadius, WORD wNumRings,
											WORD wNumSections, FLOAT scale_x, FLOAT scale_y, FLOAT scale_z)
{

	SimpleObject8	* pObj = new SimpleObject8();
	assert( pObj != NULL );

	pObj->InitSphere( fRadius, wNumRings, wNumSections );


	////////////////////////////////////////////////
	// Make vertex buffer & index buffer from it

	HRESULT hr;
	D3DXVECTOR3 vPoint;
	DWORD i;

	SAFE_RELEASE(m_pVertexBuffer);
    SAFE_RELEASE(m_pIndexBuffer);

	assert( pObj->m_PrimType == D3DPT_TRIANGLELIST );


	DWORD	wNumTriangles	= pObj->m_wNumInd / 3;
    DWORD   dwNumIndices	= pObj->m_wNumInd;
    DWORD   dwNumVertices	= pObj->m_wNumVerts;

	m_dwNumVertices = dwNumVertices;
	m_dwNumIndices = wNumTriangles * 3;
	m_dwNumFaces = wNumTriangles;	


	hr = m_pD3DDev->CreateVertexBuffer(dwNumVertices * sizeof(Dot3Vertex), 0, 0, D3DPOOL_DEFAULT,
										&m_pVertexBuffer);
	ASSERT_IF_FAILED(hr);
	if (FAILED(hr))
		return hr;

	hr = m_pD3DDev->CreateIndexBuffer(3 * wNumTriangles * sizeof(WORD), 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT,
										&m_pIndexBuffer);
	ASSERT_IF_FAILED(hr);
	if (FAILED(hr))
		return hr;


	Dot3Vertex* pVertices;
	WORD* pIndices;
	
	m_pVertexBuffer->Lock(0, 0, (BYTE**)&pVertices, 0);
	m_pIndexBuffer->Lock(0, 0, (BYTE**)&pIndices, 0);

	
	float x,y,z, nx, ny, nz, u, v;

	// copy positions into VB
    for( i = 0; i < dwNumVertices; i++ )
    {
		x = scale_x * pObj->m_pVertices[i].pos.x	+ vCenter.x;
		y = scale_y * pObj->m_pVertices[i].pos.y	+ vCenter.y;
		z = scale_z * pObj->m_pVertices[i].pos.z	+ vCenter.z;

		nx = pObj->m_pVertices[i].nrm.x;
		ny = pObj->m_pVertices[i].nrm.y;
		nz = pObj->m_pVertices[i].nrm.z;

		u = pObj->m_pVertices[i].t0.x * TEX_SCALE;
		v = pObj->m_pVertices[i].t0.y * TEX_SCALE;

		pVertices[i] = Dot3Vertex(	D3DXVECTOR3( x, y, z ),
									D3DXVECTOR3( nx, ny, nz ),
									D3DXVECTOR2( u, v ) );
	}



    // Generate triangle index buffer from simple object's indices

    for( i = 0; i < dwNumIndices; i++ )
    {
        pIndices[i] = pObj->m_pIndices[i];
	}


	// All done - unlock the buffers:

	hr = m_pVertexBuffer->Unlock();
	ASSERT_IF_FAILED(hr);

	hr = m_pIndexBuffer->Unlock();
	ASSERT_IF_FAILED(hr);


	// Free the SimpleObject

	delete( pObj );
	pObj = NULL;


	// Compute the tangent space
	// Degenerate vertices (same position, different coords) are
	//  found and averaged in the CreateBasis

	hr = CreateBasisVectors( m_pVertexBuffer, m_pIndexBuffer);
	ASSERT_IF_FAILED(hr);


    return S_OK;
}



HRESULT NVEffect_DynamicReflection::GenerateQuad(D3DXVECTOR3& vCenter, FLOAT fRadius)
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);

	m_dwNumVertices = 4;
	m_dwNumIndices = 6;
	m_dwNumFaces = m_dwNumIndices / 3;	

	m_pD3DDev->CreateVertexBuffer(m_dwNumVertices * sizeof(Dot3Vertex), 0, 0, D3DPOOL_MANAGED, &m_pVertexBuffer);
	m_pD3DDev->CreateIndexBuffer(m_dwNumIndices * sizeof(WORD), 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIndexBuffer);

	Dot3Vertex* pVertices;
	WORD* pIndices;
	
	m_pVertexBuffer->Lock(0, 0, (BYTE**)&pVertices, 0);
	m_pIndexBuffer->Lock(0, 0, (BYTE**)&pIndices, 0);

	pVertices[0] = Dot3Vertex(D3DXVECTOR3(vCenter.x - fRadius, vCenter.y + fRadius, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR2(0.0f, 0.0f));
	pVertices[1] = Dot3Vertex(D3DXVECTOR3(vCenter.x + fRadius, vCenter.y + fRadius, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR2(1.0f * TEX_SCALE, 0.0f));
	pVertices[2] = Dot3Vertex(D3DXVECTOR3(vCenter.x - fRadius, vCenter.y - fRadius, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR2(0.0f, 1.0f * TEX_SCALE));
	pVertices[3] = Dot3Vertex(D3DXVECTOR3(vCenter.x + fRadius, vCenter.y - fRadius, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR2(1.0f * TEX_SCALE, 1.0f * TEX_SCALE));

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 2;
	pIndices[4] = 1;
	pIndices[5] = 3;

	m_pVertexBuffer->Unlock();
	m_pIndexBuffer->Unlock();

    return S_OK;
}

HRESULT NVEffect_DynamicReflection::GenerateSplinePatch()
{
	HRESULT hr;
	SAFE_RELEASE(m_pPatchBuffer);

    // first alloc the buffers
    int const   kNumCtrlPts = 4 * 4;

    hr = m_pD3DDev->CreateVertexBuffer(kNumCtrlPts * sizeof(Dot3Vertex), 
                                  D3DUSAGE_RTPATCHES|D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY, 
                                  0, D3DPOOL_DEFAULT, &m_pPatchBuffer);

    // then do not do anything with it: AnimateSplinePatch will generate *all* data!
    return hr;
}

HRESULT NVEffect_DynamicReflection::AnimateSplinePatch(EBTimer* pTimer)
{
 	Dot3Vertex*         pVertices; 
	static const float fZOffset = 0.0f;
	m_pPatchBuffer->Lock(0, 0, (BYTE**)&pVertices, D3DLOCK_DISCARD);

    // re-initialize positions
    int i, j;
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)
		{
            pVertices[4*i + j].Position = D3DXVECTOR3(j - 1.5f, 1.5f - i, fZOffset);
			pVertices[4*i + j].Position *= 0.25f;
		}
	}


	float anim_disp_scale = 0.65f;

	if (!m_bPause)
	{
		// modify central control points
		m_ctrl1 = (fZOffset + anim_disp_scale * sinf(1.0f + 0.5f  * pTimer->GetDuration())); 
		m_ctrl2 = (fZOffset + anim_disp_scale * sinf(2.0f + 0.35f * pTimer->GetDuration())); 
		m_ctrl3 = (fZOffset + anim_disp_scale * sinf(3.0f + 0.6f  * pTimer->GetDuration())); 
		m_ctrl4 = (fZOffset + anim_disp_scale * sinf(4.0f + 0.1f  * pTimer->GetDuration())); 
	}
	
    pVertices[4*1 + 1].Position.z = m_ctrl1;
    pVertices[4*2 + 1].Position.z = m_ctrl2;
    pVertices[4*1 + 2].Position.z = m_ctrl3;
    pVertices[4*2 + 2].Position.z = m_ctrl4;

	// compute d position / dv 
    D3DXVECTOR3     mesh[16];

	for(i=0; i < 3; i++)
		for(j=0; j < 4; j++)
			mesh[4*i + j] = pVertices[4*(i+1) + j].Position - pVertices[4*i + j].Position;

	// If the positions are reversed, the basis will be inverted and "high"
	//  bumps will appear as depressions instead
	// ie, if you were to do:
	//			mesh[4*i + j] = pVertices[4*i + j].Position - pVertices[4*(i+1) + j].Position;
	// the bumps would be reversed in that axis.

    ElevateToCubicMesh(mesh, 4, 3);

	for(i=0; i < 4; i++)
		for(j=0; j < 4; j++)
			pVertices[4*i + j].T = mesh[4*i + j];

	///////////////////////////////////////////////
    // d position / du 
    for(i=0; i < 4; i++)
        for(j=0; j < 3; j++)
	        mesh[4*i + j] = pVertices[4*i + j+1].Position - pVertices[4*i + j].Position ; 

	// If the positions are reversed, the basis will be inverted and "high"
	//  bumps will appear as depressions instead
	// ie, if you were to do:
	//	        mesh[4*i + j] = pVertices[4*i + j].Position - pVertices[4*i + j+1].Position; 
	// the bumps would be reversed in that axis.
		
    ElevateToCubicMesh(mesh, 3, 4);

    for(i=0; i < 4; i++)
        for(j=0; j < 4; j++)
	        pVertices[4*i + j].S = mesh[4*i + j];


    return S_OK;
}

void NVEffect_DynamicReflection::ElevateToCubicMesh(D3DXVECTOR3    *pControlMesh, 
                                             unsigned int    uOrder, 
                                             unsigned int    vOrder) const
{
    // Can transform constant, linear, quadratic, or cubic in either u/v
    // to fully cubic in both u and v.
    assert(uOrder >= 1 && uOrder <= 4);
    assert(vOrder >= 1 && vOrder <= 4);

    // mesh is assumed to be a 4x4 grid of D3DXVECTOR3, the upper left
    // uOrder by vOrder corner is assumed to be populated with values and 
    // the elevation result is returned in the full 4x4 mesh.
    D3DXMATRIX const  allElevators[3] = {D3DXMATRIX(1.0f, 1.0f,    1.0f,    1.0f, 
                                                    0.0f, 0.0f,    0.0f,    0.0f, 
                                                    0.0f, 0.0f,    0.0f,    0.0f, 
                                                    0.0f, 0.0f,    0.0f,    0.0f), 
                                         D3DXMATRIX(1.0f, 0.75f,   0.25f,   0.0f, 
                                                    0.0f, 0.25f,   0.75f,   1.0f, 
                                                    0.0f, 0.0f,    0.0f,    0.0f, 
                                                    0.0f, 0.0f,    0.0f,    0.0f), 
                                         D3DXMATRIX(1.0f, 1.f/3.f, 0.0f,    0.0f, 
                                                    0.0f, 2.f/3.f, 2.f/3.f, 0.0f, 
                                                    0.0f, 0.0f,    1.f/3.f, 1.0f, 
                                                    0.0f, 0.0f,    0.0f,    0.0f)  };

    D3DXMATRIX  const   *pElevator;
    float       const   *pConstVec3Base;
    float               *pVec3Base;

    int     const        kFloatsInVec = 3;
    int                  i, j;

    // elevate in x-direction first (since we are only applying linear equations, 
    // order don't matter)
    if (uOrder < 4)
    {
        // determine elevation level (and transpose since D3DX uses row-vecs...)
        pElevator = &allElevators[uOrder-1];

        D3DXVECTOR3  row    [kFloatsInVec];
        D3DXVECTOR4  elevRow[kFloatsInVec];

        for (i = 0; i < vOrder; ++i)
        {
            // copy the row of points to be elevated
            pConstVec3Base = (float const *)(&(pControlMesh[4*i]));

            for (j = 0; j < kFloatsInVec; ++j)
                row[j] = D3DXVECTOR3( *(pConstVec3Base                  + j), 
                                      *(pConstVec3Base +   kFloatsInVec + j), 
                                      *(pConstVec3Base + 2*kFloatsInVec + j));

            // elevate
            for (j = 0; j < kFloatsInVec; ++j)
                D3DXVec3Transform(&(elevRow[j]), &(row[j]), pElevator);

            // and write result back into pControlMesh
            pVec3Base = (float *)(&(pControlMesh[4*i]));
            
            for (j = 0; j < kFloatsInVec; ++j)
            {
                *(pVec3Base + j + 0*kFloatsInVec) = elevRow[j].x;
                *(pVec3Base + j + 1*kFloatsInVec) = elevRow[j].y;
                *(pVec3Base + j + 2*kFloatsInVec) = elevRow[j].z;
                *(pVec3Base + j + 3*kFloatsInVec) = elevRow[j].w;
            }
        }
    }
    // we are now guaranteed that the first vOrder rows are fully populated
    // now elevate the v-direction
    if (vOrder < 4)
    {
        // determine elevation level (and transpose since D3DX uses row-vecs...)
        pElevator = &allElevators[vOrder-1];

        D3DXVECTOR3 column[kFloatsInVec];
        D3DXVECTOR4 elevColumn[kFloatsInVec];

        for (i = 0; i < 4; ++i)
        {
            // copy the column of points to be elevated
            pConstVec3Base = (float const *)(&(pControlMesh[i]));

            for (j = 0; j < kFloatsInVec; ++j)
                column[j] = D3DXVECTOR3( *(pConstVec3Base + kFloatsInVec*4*0 + j), 
                                         *(pConstVec3Base + kFloatsInVec*4*1 + j),
                                         *(pConstVec3Base + kFloatsInVec*4*2 + j) );
            // elevate
            for (j = 0; j < kFloatsInVec; ++j)
                D3DXVec3Transform(&(elevColumn[j]), &(column[j]), pElevator);

            // and write result back into pControlMesh
            pVec3Base = (float *)(&(pControlMesh[i]));
            
            for (j = 0; j < kFloatsInVec; ++j)
            {
                *(pVec3Base + kFloatsInVec*4*0 + j) = elevColumn[j].x;
                *(pVec3Base + kFloatsInVec*4*1 + j) = elevColumn[j].y;
                *(pVec3Base + kFloatsInVec*4*2 + j) = elevColumn[j].z;
                *(pVec3Base + kFloatsInVec*4*3 + j) = elevColumn[j].w;
            }
        }
    }
}

void NVEffect_DynamicReflection::ElevateToCubicMesh(D3DXVECTOR4    *pControlMesh, 
                                             unsigned int    uOrder, 
                                             unsigned int    vOrder) const
{
    // Can transform constant, linear, quadratic, or cubic in either u/v
    // to fully cubic in both u and v.
    assert(uOrder >= 1 && uOrder <= 4);
    assert(vOrder >= 1 && vOrder <= 4);

    // mesh is assumed to be a 4x4 grid of D3DXVECTOR3, the upper left
    // uOrder by vOrder corner is assumed to be populated with values and 
    // the elevation result is returned in the full 4x4 mesh.
    D3DXMATRIX const  allElevators[3] = {D3DXMATRIX(1.0f, 1.0f,    1.0f,    1.0f, 
                                                    0.0f, 0.0f,    0.0f,    0.0f, 
                                                    0.0f, 0.0f,    0.0f,    0.0f, 
                                                    0.0f, 0.0f,    0.0f,    0.0f), 
                                         D3DXMATRIX(1.0f, 0.75f,   0.25f,   0.0f, 
                                                    0.0f, 0.25f,   0.75f,   1.0f, 
                                                    0.0f, 0.0f,    0.0f,    0.0f, 
                                                    0.0f, 0.0f,    0.0f,    0.0f), 
                                         D3DXMATRIX(1.0f, 1.f/3.f, 0.0f,    0.0f, 
                                                    0.0f, 2.f/3.f, 2.f/3.f, 0.0f, 
                                                    0.0f, 0.0f,    1.f/3.f, 1.0f, 
                                                    0.0f, 0.0f,    0.0f,    0.0f)  };

    D3DXMATRIX  const   *pElevator;
    float       const   *pConstVec3Base;
    float               *pVec3Base;

    int     const        kFloatsInVec = 4;
    int                  i, j;

    // elevate in x-direction first (since we are only applying linear equations, 
    // order don't matter)
    if (uOrder < 4)
    {
        // determine elevation level (and transpose since D3DX uses row-vecs...)
        pElevator = &allElevators[uOrder-1];

        D3DXVECTOR3  row    [kFloatsInVec];
        D3DXVECTOR4  elevRow[kFloatsInVec];

        for (i = 0; i < vOrder; ++i)
        {
            // copy the row of points to be elevated
            pConstVec3Base = (float const *)(&(pControlMesh[4*i]));

            for (j = 0; j < kFloatsInVec; ++j)
                row[j] = D3DXVECTOR3( *(pConstVec3Base                  + j), 
                                      *(pConstVec3Base +   kFloatsInVec + j), 
                                      *(pConstVec3Base + 2*kFloatsInVec + j));

            // elevate
            for (j = 0; j < kFloatsInVec; ++j)
                D3DXVec3Transform(&(elevRow[j]), &(row[j]), pElevator);

            // and write result back into pControlMesh
            pVec3Base = (float *)(&(pControlMesh[4*i]));
            
            for (j = 0; j < kFloatsInVec; ++j)
            {
                *(pVec3Base + j + 0*kFloatsInVec) = elevRow[j].x;
                *(pVec3Base + j + 1*kFloatsInVec) = elevRow[j].y;
                *(pVec3Base + j + 2*kFloatsInVec) = elevRow[j].z;
                *(pVec3Base + j + 3*kFloatsInVec) = elevRow[j].w;
            }
        }
    }
    // we are now guaranteed that the first vOrder rows are fully populated
    // now elevate the v-direction
    if (vOrder < 4)
    {
        // determine elevation level (and transpose since D3DX uses row-vecs...)
        pElevator = &allElevators[vOrder-1];

        D3DXVECTOR3 column[kFloatsInVec];
        D3DXVECTOR4 elevColumn[kFloatsInVec];

        for (i = 0; i < 4; ++i)
        {
            // copy the column of points to be elevated
            pConstVec3Base = (float const *)(&(pControlMesh[i]));

            for (j = 0; j < kFloatsInVec; ++j)
                column[j] = D3DXVECTOR3( *(pConstVec3Base + kFloatsInVec*4*0 + j), 
                                         *(pConstVec3Base + kFloatsInVec*4*1 + j),
                                         *(pConstVec3Base + kFloatsInVec*4*2 + j) );
            // elevate
            for (j = 0; j < kFloatsInVec; ++j)
                D3DXVec3Transform(&(elevColumn[j]), &(column[j]), pElevator);

            // and write result back into pControlMesh
            pVec3Base = (float *)(&(pControlMesh[i]));
            
            for (j = 0; j < kFloatsInVec; ++j)
            {
                *(pVec3Base + kFloatsInVec*4*0 + j) = elevColumn[j].x;
                *(pVec3Base + kFloatsInVec*4*1 + j) = elevColumn[j].y;
                *(pVec3Base + kFloatsInVec*4*2 + j) = elevColumn[j].z;
                *(pVec3Base + kFloatsInVec*4*3 + j) = elevColumn[j].w;
            }
        }
    }
}


HRESULT NVEffect_DynamicReflection::Initialize(IDirect3DDevice8* pDev)
{
	HRESULT hr;
	vector<DWORD> Declaration, PatchDeclaration;


	m_pD3DDev = pDev;
	pDev->AddRef();


	m_bDrawing = false;
	m_bDrawModeOn = true;

	//initialize mouse UI
	RECT rect;
	rect.left = rect.top = 0;
	D3DVIEWPORT8 viewport;
	m_pD3DDev->GetViewport(&viewport);
	rect.bottom = viewport.Height;
	rect.right  = viewport.Width;

	m_pUI = new MouseUI((const RECT)rect);
	assert( m_pUI != NULL );

	// Must Reset BEFORE setting translatin sens factor

    m_pUI->Reset();
	m_pUI->Translate( 0.0f, 0.0f, -0.25f );
	m_pUI->OnLButtonDown( 50, 50 );
	m_pUI->OnMouseMove( 67, 57 );
	m_pUI->OnLButtonUp( 67, 57 );


	m_pUI->SetTranslationalSensitivityFactor( 0.1f );


	//////////////////////////////////////////////////////
	//  allocate class for creating dynamic normal maps

	assert( m_pCA_Water == NULL );

	m_pCA_Water = new CA_Water();
	assert( m_pCA_Water != NULL );

	m_pCA_Water->Initialize( pDev );

	m_pCA_Water->SetSimMode( SM_NORMAL_GRAY );


	//////////////////////////////////////////////////////

	m_bShowProceduralMaps = false;


	m_pNVDevice = new NVEffectDevice(pDev, this);



	D3DDEVICE_CREATION_PARAMETERS CreationParams;
	pDev->GetCreationParameters(&CreationParams);

	if (CreationParams.BehaviorFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING)
	{
		m_bSupportsPatches = false;
		m_bUsePatch = false;
	}
	else
	{
		m_bSupportsPatches = true;
	}

    hr = GenerateSplinePatch();
	if (FAILED(hr))
	{
		m_strLastError = "Failed to generate spline patch";
		return E_FAIL;
	}

    hr = GenerateSphere(D3DXVECTOR3(0.0f, 0.0f, 0.0f), 0.35f, 30, 40, 1.0f, 1.0f, 1.0f);
	if (FAILED(hr))
	{
		m_strLastError = "Failed to generate sphere";
		return hr;
	}

     //GenerateQuad(D3DXVECTOR3(0.0f, 0.0f, 0.0f), 1.0f);

    PatchDeclaration.clear();
    PatchDeclaration.push_back(D3DVSD_STREAM(0));
    PatchDeclaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3)); // Position
    PatchDeclaration.push_back(D3DVSD_REG(4, D3DVSDT_FLOAT3)); // Normal  not used!
    PatchDeclaration.push_back(D3DVSD_REG(2, D3DVSDT_FLOAT3)); // Diffuse not used!
	PatchDeclaration.push_back(D3DVSD_REG(7, D3DVSDT_FLOAT2)); // Texture not used!
	PatchDeclaration.push_back(D3DVSD_REG(5, D3DVSDT_FLOAT3)); // S
	PatchDeclaration.push_back(D3DVSD_REG(6, D3DVSDT_FLOAT3)); // T
	PatchDeclaration.push_back(D3DVSD_REG(8, D3DVSDT_FLOAT3)); // SxT
    PatchDeclaration.push_back(D3DVSD_STREAM_TESS());          // is computed
    PatchDeclaration.push_back(D3DVSD_TESSUV( 3 ));     
    PatchDeclaration.push_back(D3DVSD_END());
    
	Declaration.clear();
    Declaration.push_back(D3DVSD_STREAM(0));
	Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3)); // Position
	Declaration.push_back(D3DVSD_REG(1, D3DVSDT_FLOAT3)); // Normal
	Declaration.push_back(D3DVSD_REG(2, D3DVSDT_FLOAT3)); // Diffuse
	Declaration.push_back(D3DVSD_REG(3, D3DVSDT_FLOAT2)); // Texture
	Declaration.push_back(D3DVSD_REG(5, D3DVSDT_FLOAT3)); // S
	Declaration.push_back(D3DVSD_REG(6, D3DVSDT_FLOAT3)); // T
	Declaration.push_back(D3DVSD_REG(4, D3DVSDT_FLOAT3)); // SxT
    Declaration.push_back(D3DVSD_END());


	// Create the shaders, 2 versions - one for the sphere, one for the patch

	if (m_bSupportsPatches)
	{
		
		hr = LoadAndCreateShader(GetFilePath("blinn_reflect.vso"), &PatchDeclaration[0], 0, SHADERTYPE_VERTEX, &m_dwBlinnPatchVertexShader);
		if (FAILED(hr))
			return hr;

		hr = LoadAndCreateShader(GetFilePath("dot3_transform.vso"), &PatchDeclaration[0], 0, SHADERTYPE_VERTEX, &m_dwTransformPatchShader);
		if (FAILED(hr))
			return hr;

	}

	hr = LoadAndCreateShader(GetFilePath("blinn_reflect.pso").c_str(), NULL, 0, SHADERTYPE_PIXEL, &m_dwBlinnPixelShader);
		if (FAILED(hr))
			return hr;
		
	hr = LoadAndCreateShader(GetFilePath("blinn_reflect.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwBlinnVertexShader);
	if (FAILED(hr))
		return hr;


	hr = LoadAndCreateShader(GetFilePath("dot3_transform.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwTransformShader);
	if (FAILED(hr))
		return hr;
	

    m_bSupportsHILO = false;

	hr = D3DXCreateCubeTextureFromFileExA( pDev,
		GetFilePath("nvlobby_cube_mipmap.dds").c_str(), 
		D3DX_DEFAULT,
		0,
		0,
		D3DFMT_UNKNOWN,
		D3DPOOL_MANAGED,
		D3DX_FILTER_LINEAR,
		D3DX_FILTER_LINEAR,
		0,
		NULL,
		NULL,
		&m_pCubeTexture);
	if (FAILED(hr))
	{
		m_strLastError = "Could not load texture (sky_cube_mipmap.dds)";
		return hr;
	}

	// Setup constants
	m_pD3DDev->SetVertexShaderConstant(CV_ZERO,   D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_ONE,    D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), 1);


	// Camera stuff
	D3DXMATRIX matProj;
	D3DXMATRIX matView;

	D3DXVECTOR3 vEyePt    = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
	D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f,  0.0f );
	D3DXVECTOR3 vUp       = D3DXVECTOR3( 0.0f, 1.0f,  0.0f );
	m_pD3DDev->SetVertexShaderConstant(CV_EYE_WORLD, D3DXVECTOR4(vEyePt.x, vEyePt.y, vEyePt.z, 0.0f), 1);

	// View
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUp);

	// Projection
	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(60.0f), 1.0f, 0.01f, 70.0f);

	m_pNVDevice->SetViewTransform(&matView);
	m_pNVDevice->SetProjectionTransform(&matProj);

	m_pD3DDev->SetRenderState(D3DRS_SPECULARENABLE, FALSE);


	// Reset control points for mesh
	m_ctrl1 = m_ctrl2 = m_ctrl3 = m_ctrl4 = 0.0f;

	// Point at the vertex data
    CreateBasisVectors(m_pVertexBuffer, m_pIndexBuffer);
	
	// Point at the index data
	hr = m_pD3DDev->SetIndices(m_pIndexBuffer, 0);
	if (FAILED(hr))
	{
		m_strLastError = "Could not set Index source";
		return hr;
	}

	return S_OK;

}

HRESULT NVEffect_DynamicReflection::Free()
{
	SAFE_DELETE( m_pCA_Water );

    SAFE_RELEASE(m_pPatchBuffer);
    SAFE_RELEASE(m_pVertexBuffer);
    SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pBumpMapQWVU);
	SAFE_RELEASE(m_pCubeTexture);
	SAFE_DELETE(m_pUI);

	SAFE_DELETE(m_pNVDevice);

	if (m_pD3DDev)
	{
		if (m_dwBlinnPixelShader)
			m_pD3DDev->DeletePixelShader(m_dwBlinnPixelShader);
		if (m_dwBlinnVertexShader)
			m_pD3DDev->DeleteVertexShader(m_dwBlinnVertexShader);
		if (m_dwTransformShader)
			m_pD3DDev->DeleteVertexShader(m_dwTransformShader);
		if (m_dwBlinnPatchVertexShader)
			m_pD3DDev->DeleteVertexShader(m_dwBlinnPatchVertexShader);
		if (m_dwTransformPatchShader)
			m_pD3DDev->DeleteVertexShader(m_dwTransformPatchShader);

		m_dwBlinnPixelShader = 0;
		m_dwBlinnVertexShader = 0;
		m_dwTransformShader = 0;
		m_dwBlinnPatchVertexShader = 0;
		m_dwTransformPatchShader = 0;

		SAFE_RELEASE(m_pD3DDev);
	}
	
	return S_OK;
}

HRESULT NVEffect_DynamicReflection::Start()
{
	m_fAngle = 0.0f;
	return S_OK;
}

  
HRESULT NVEffect_DynamicReflection::SetTransform()
{
	D3DXMATRIX matTemp;
	D3DXMATRIX matWorldViewProj;
	D3DXMATRIX matWorldView;
	D3DXMATRIX matWorldViewI;
	D3DXMATRIX matWorldI;

	D3DXMATRIX matWorld = m_pNVDevice->GetWorldTransform();
	D3DXMATRIX matProj = m_pNVDevice->GetProjectionTransform();
	D3DXMATRIX matView = m_pNVDevice->GetViewTransform();

	D3DXMatrixMultiply(&matTemp, &matWorld, &matView);
	D3DXMatrixMultiply(&matWorldViewProj, &matTemp, &matProj);
	D3DXMatrixMultiply(&matWorldView, &matWorld, &matView);
	D3DXMatrixInverse(&matWorldI, NULL, &matWorld);
	
	// Create a 3x3 invertse of the worldview for the normal transformation (we transpose it as we load it into
	// the constant store)
	D3DXMatrixInverse(&matWorldViewI, NULL, &matWorldView);
		
	// Projection to clip space
	D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matWorldViewProj(0, 0), 4);
	D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);

	D3DXMatrixTranspose(&matWorld, &matWorld);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLD_0, &matWorld(0, 0), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLD_1, &matWorld(1, 0), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLD_2, &matWorld(2, 0), 1);
	D3DXMatrixTranspose(&matWorld, &matWorld);

	// If you are rotating the cubemap, you need to apply that transform here too
	D3DXMATRIX matBasisTransform = matWorld;

	D3DXMatrixTranspose(&matBasisTransform, &matBasisTransform);
	m_pD3DDev->SetVertexShaderConstant(CV_BASISTRANSFORM_0, &matBasisTransform(0, 0), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_BASISTRANSFORM_1, &matBasisTransform(1, 0), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_BASISTRANSFORM_2, &matBasisTransform(2, 0), 1);
	D3DXMatrixTranspose(&matBasisTransform, &matBasisTransform);

	return S_OK;

}

HRESULT NVEffect_DynamicReflection::Tick(EBTimer* pTimer)
{
	HRESULT hr = S_OK;


	if( m_bShowProceduralMaps )
	{
		m_pCA_Water->m_bDrawOutput = true;
	}
	else
	{
		m_pCA_Water->m_bDrawOutput = false;

		// Clear to grey
		hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
								D3DCOLOR_XRGB( 0xAA, 0xAA, 0xAA ), 1.0, 0);
		ASSERT_IF_FAILED(hr);
	}

	//////////////////////////////////////////
	// Update dynamic normal map

	assert( m_pCA_Water != NULL );

	// Tick also draws the maps in background if m_bDrawOutput is on
	hr = m_pCA_Water->Tick();
	
	ASSERT_IF_FAILED(hr);

	////////////////////////////////////////////////////
	// Restore render state because it may have been polluted
	//		by water animation

	RestoreRenderState();

	if( m_bShowProceduralMaps )
	{
		// Clear z buffer if background map was drawn
		hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
								D3DCOLOR_XRGB( 0xAA, 0xAA, 0xAA ), 1.0, 0);
		ASSERT_IF_FAILED(hr);
	}


	D3DXMATRIX matWorld;
	D3DXMATRIX matView = m_pNVDevice->GetViewTransform();
	D3DXMATRIX matTemp;



	m_pD3DDev->SetRenderState(D3DRS_FILLMODE, m_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID);

	// Set the bump scale
	m_pD3DDev->SetVertexShaderConstant(CV_BUMPSCALE, D3DXVECTOR4(0.0f, 0.0f, 0.0f, m_fBumpScale), 1);

    AnimateSplinePatch(pTimer);

	D3DXMatrixMultiply( &matWorld, &m_pUI->GetRotationMatrix(), &m_pUI->GetTranslationMatrix() );
			
	// Load transform data.
	m_pNVDevice->SetWorldTransform(&matWorld);
	SetTransform();

	DrawReflection();

	return hr;
}



HRESULT NVEffect_DynamicReflection::DrawReflection()
{
	D3DXMATRIX matWorld;
	D3DXMATRIX matTemp;

	m_pD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	// General setup
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	m_pD3DDev->SetRenderState( D3DRS_WRAP0, D3DWRAP_U | D3DWRAP_V);

	// Point at the vertex data
    m_pD3DDev->SetStreamSource(0, m_bUsePatch ? m_pPatchBuffer : m_pVertexBuffer, sizeof(Dot3Vertex));

	// Backface cull the sphere
	m_pD3DDev->SetRenderState(D3DRS_CULLMODE, m_bUsePatch ? D3DCULL_NONE : D3DCULL_CCW);

	// Point at the index data
	m_pD3DDev->SetIndices(m_pIndexBuffer, 0);

	m_pD3DDev->SetPixelShader(0);

	if ((m_eDisplayOption == DISPLAY_BLINN8BITSIGNED) && (!m_bSupportsQWVU))
	{
		m_eDisplayOption = DISPLAY_BLINNHILOSIGNED;
	}
	
	if ((m_eDisplayOption == DISPLAY_BLINNHILOSIGNED) && (!m_bSupportsHILO))
	{
		m_eDisplayOption = DISPLAY_BLINN8BITSIGNED;
	}

	// Normal map in stage 0
	// DotProduct 3 the normal map with the diffuse color set in the vertex
	// shader (this will be the local light vector)
	if (m_eDisplayOption == DISPLAY_BLINN8BITSIGNED)
	{
		assert( m_pCA_Water != NULL );

		IDirect3DTexture8 * pTex = m_pCA_Water->GetOutputTexture();

		assert( pTex != NULL );

		m_pD3DDev->SetTexture( 0, m_pCA_Water->GetOutputTexture() );

	}


	m_pD3DDev->SetTexture(1, NULL);
	m_pD3DDev->SetTexture(2, NULL);
	m_pD3DDev->SetTexture(3, m_pCubeTexture);
	

	// Setup the vertex shader
    m_pD3DDev->SetVertexShader(m_bUsePatch ? m_dwBlinnPatchVertexShader : m_dwBlinnVertexShader);	
	m_pD3DDev->SetPixelShader(m_dwBlinnPixelShader);


	float numSegs[4];
	numSegs[0] = 8.0f;
	numSegs[1] = 8.0f;
	numSegs[2] = 8.0f;
	numSegs[3] = 8.0f;

	D3DRECTPATCH_INFO info;
	info.StartVertexOffsetWidth  = 0;
	info.StartVertexOffsetHeight = 0;
	info.Width  = 4;
	info.Height = 4;
	info.Stride = 4; // verticies to next row of verticies
	info.Basis  = D3DBASIS_BEZIER;
	info.Order  = D3DORDER_CUBIC;

    // Draw 
    if (m_bUsePatch)
    {
        // draw a single patch for now
        // always tesselate from scratch...

		// Calculate SxT in the vertex shader
		m_pD3DDev->SetVertexShaderConstant( CV_CALC_SXT, D3DXVECTOR4( 1.0f, 0.0f, 0.0f, 0.0f), 1 );

	    m_pD3DDev->DrawRectPatch(1, numSegs, &info);
    }
    else
	{
		// Draw the sphere
		// Don't calculate SxT in the vertex shader
		m_pD3DDev->SetVertexShaderConstant( CV_CALC_SXT, D3DXVECTOR4( 0.0f, 1.0f, 0.0f, 0.0f), 1 );

        m_pD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_dwNumVertices, 0, m_dwNumFaces);
	}



	return S_OK;
}



void NVEffect_DynamicReflection::MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y)
{
	if(button == MOUSE_LEFTBUTTON)
	{
		if(bDown)
		{
			m_pUI->OnLButtonDown(x, y);
		}
		else
		{
			m_pUI->OnLButtonUp(x, y);
		}
	}

	if( button == MOUSE_LEFTBUTTON && m_bDrawModeOn == true )
	{
		if(bDown)
		{
			m_bDrawing = true;
		}
		else
		{
			// Draw the last point clicked as the mouse button 
			//   comes up.
			
			TryDrawDroplet( x,y );

			m_bDrawing = false;
		}
	}

	return;
}


void NVEffect_DynamicReflection::TryDrawDroplet( float x, float y )
{
	// Adds droplet to the queue for later rendering

	float fx,fy;
	RECT winr;
	float scale;

	if( m_pUI->IsInWindow( x,y ))
	{
		winr = m_pUI->GetRECT();

		fx = 1.0f - ((float)x - (float)winr.left)/((float)winr.right - winr.left);
		fy =        ((float)y - (float)winr.top)/((float)winr.bottom - winr.top);

		scale = 14.0f;

		m_pCA_Water->AddDroplet( fx, fy, scale );
	}
}


void NVEffect_DynamicReflection::RestoreRenderState()
{
	// call to reset render modes to those appropriate for
	//  rendering the reflective surface

	int i;
	for( i=0; i < 4; i++ )
	{
        m_pD3DDev->SetTextureStageState(i, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	    m_pD3DDev->SetTextureStageState(i, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	    m_pD3DDev->SetTextureStageState(i, D3DTSS_MIPFILTER, D3DTEXF_POINT  );  // nearest level
	}

	m_pD3DDev->SetRenderState( D3DRS_ZENABLE,			D3DZB_TRUE );
	m_pD3DDev->SetRenderState( D3DRS_ZWRITEENABLE,		true );

}


void NVEffect_DynamicReflection::MouseMove(HWND hWnd, int x, int y)
{

	if( m_bDrawing && m_bDrawModeOn )
	{
		if( m_pCA_Water != NULL )
		{
			TryDrawDroplet( x,y );
		}
	}
	else if( m_pUI != NULL )
	{
		m_pUI->OnMouseMove(x, y);
	}


	return;
}

void NVEffect_DynamicReflection::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
{

	assert( m_pCA_Water != NULL );
	bool result;
	result = m_pCA_Water->Keyboard( dwKey, nFlags, bDown );

	if( result == true )
	{
		return;			// no more keyboard processing!
	}

	if( !bDown )
	{
		switch( dwKey )
		{
		case VK_NUMPAD8:
			m_pUI->Reset();
			m_pUI->Translate( 0.0f, 0.0f, -0.25f );
			m_pUI->OnLButtonDown( 50, 50 );
			m_pUI->OnMouseMove( 67, 57 );
			m_pUI->OnLButtonUp( 67, 57 );

			m_pUI->SetTranslationalSensitivityFactor( 0.1f );

            m_bWireframe = false;
			m_pCA_Water->m_bWireframe = false;
			m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;

			m_bShowProceduralMaps = false;

			break;

		case 'D':
			m_bDrawModeOn = !m_bDrawModeOn;
			FDebug("Set draw mode: %s\n", m_bDrawModeOn?"ON":"OFF");
			break;
		case '5':
			m_bShowProceduralMaps = !m_bShowProceduralMaps;
			break;
		case 'P':
			m_bPause = !m_bPause;
			m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
			break;
		}
	}


	eEBKeyAction Action = TranslateEffectKey(dwKey, nFlags, bDown);
	

	if( result == true && Action != EB_WIREFRAME )
	{
		return;			// no more keyboard processing!
	}


	EBString msg;

    switch ( Action )
    {
		case EB_HELP:
		{
			msg  = "H  or  F1 for Help\n";
			msg += "\n";
			msg += "*** This demo is framerate limited.  ***\n";
			msg += "    Hit 'S' to run as fast as possible\n";
			msg += "\n";
			msg += "D       : Toggle draw/move mode\n";
			msg += "L       : Toggle logo in water\n";
			msg += "\n";
			msg += "NUMPAD7   : Reset to initial conditions\n";
			msg += "NUMPAD8   : Reset view\n";
			msg += "\n";
			msg += "5       : Display dynamic maps\n";	
			msg += "   1    :   Display normal map\n";	
			msg += "   2    :   Display velocity map\n";	
			msg += "   3    :   Display initial,pos,v,n maps\n";	
			msg += "\n";	
			msg += "W       : Wireframe\n";
			msg += "+        : Increase bump height\n";
			msg += "-         : Decrease bump height\n";
			msg += "\n";
			msg += "SPACE     : Start/stop procedural animation       \n";
			msg += "RETURN    : Single step of animation\n";
			msg += "S          : Toggle animation rate limiting\n";
			msg += "  L arrow : Run animation faster\n";
			msg += "  R arrow : Run animation slower\n";
			msg += "\n";	
			msg += "Up arrow   : Increase droplet frequency\n";	
			msg += "Dwn arrow  : Decrease droplet frequency\n";	
			msg += "\n";
			msg += "M       : Toggle normal map creation method\n";
			msg += "Y       : Decrease normal map red/green\n";
			msg += "U       : Increase normal map red/green\n";
			msg += "\n";
			msg += "C       : Decrease height smoothing\n";	
			msg += "V       : Increase height smoothing\n";	
			msg += "<, >    : Dec/Inc velocity apply factor\n";
			msg += "[, ]    : Dec/Inc force apply factor\n";
			msg += "\n";
			msg += "\n";
			
			
			MessageBoxEx( NULL, msg.c_str(), "Dynamic Reflection Map Help",
						MB_ICONINFORMATION | MB_TASKMODAL, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ) );

		}
		break;

		case EB_WIREFRAME:
        {
            m_bWireframe = !m_bWireframe;
			m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
            
        }
		break;

		case EB_RESET:
        {
			// Reset mesh control points
			m_ctrl1 = m_ctrl2 = m_ctrl3 = m_ctrl4 = 0.0f;

			m_fBumpScale = INIT_BUMPSCALE;
			m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;

			if( m_pCA_Water != NULL )
			{
				m_pCA_Water->m_bReset = true;
			}
        }
		break;

		case EB_PAUSE:
        {
            m_bPause = !m_bPause;
			m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
        }
        break;

		case EB_ADD:
		{
			m_fBumpScale += 0.01f;
			if (m_fBumpScale > 2.0f)
				m_fBumpScale = 2.0f;
		}
		break;

		case EB_SUBTRACT:
		{
			m_fBumpScale -= 0.01f;
			if (m_fBumpScale < 0.0f)
				m_fBumpScale = 0.0f;
		}		
		break;

        default :
            break;
    }
}

void NVEffect_DynamicReflection::PropertyUpdateCallback(const EBProperty* pProperty, bool bWritten)
{
	if (!bWritten)
		return;

	EBString	name;

	if (pProperty->IsKindOf(EBTYPE_TRIGGER_PROP))
	{

		name = pProperty->GetPropertyName();
		
		if(name == EBString(STR_INCREASEBUMPSCALE))
		{
			Keyboard(VK_ADD, 0, true);
		}
		else if (name == EBString(STR_DECREASEBUMPSCALE))
		{
			Keyboard(VK_SUBTRACT, 0, true);
		}
		else if (name == EBString(STR_RESETPATCH))
		{
			Keyboard(VK_HOME, 0, true);
		}
		else if (name == EBString(STR_MOREOPTION))
		{
			Keyboard(VK_F1, 0, true );
		}
	}
}



