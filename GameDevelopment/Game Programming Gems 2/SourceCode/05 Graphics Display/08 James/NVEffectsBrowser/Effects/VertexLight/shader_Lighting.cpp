/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/

#include "eb_effect.h"
#include "nvinc.h"
#include "nvdevice.h"
#include "nvmesh.h"
#include "shader_Lighting.h"
#include "lighting.h"

using namespace nv_objects;
using namespace std;

DECLARE_EFFECT_MAIN()

extern "C"
{

__declspec(dllexport) unsigned int GetNumEffects() { return 4; }

__declspec(dllexport) EBEffect* CreateEffect(unsigned int iEffectNum)
{
	switch (iEffectNum)
	{
		case 0:
			return new CShaderDirectionalLight();
		case 1:
			return new CShaderPointLight();
		case 2:
			return new CShaderManyPointLight();
		case 3:
			return new CShaderTwoSideLight();
		default:
			return NULL;
	}

	return NULL;
}


}

// Old-style D3D vertices
class Vertex
{
public:
	Vertex(const D3DXVECTOR3& Pos, const D3DXVECTOR3& Norm, const D3DXVECTOR2& Tex)
		: Position(Pos), Normal(Norm), Texture(Tex)
	{}

	Vertex()
	{}

	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
	D3DXVECTOR2 Texture;
};

class TLVertex
{
public:
	D3DXVECTOR4 Position;
	DWORD Diffuse;
	DWORD Specular;
	D3DXVECTOR2 Texture;
};

#define D3DFVF_VERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
#define D3DFVF_TLVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)

CShaderLighting::CShaderLighting(LightType Type)
	: m_LightType(Type),
	m_pSphereBuffer(NULL),
	m_fAngle(0.0f),
	m_pShapeMesh(NULL),
	m_pLightMesh(NULL),
	m_pNVDevice(NULL),
	m_dwCurrentLightDraw(-1),
	m_bWireframe(false),
	m_bPause(false),
	m_pUI(NULL)
{
}

CShaderLighting::~CShaderLighting()
{
	
	Free();	
}

void CShaderLighting::UpdateProperties()
{
	EBEffect::UpdateProperties();
	AddProperty(new EBProperty("Wireframe", OBJECT_MEMBER(m_bWireframe), EBTYPE_BOOL_PROP));
	AddProperty(new EBProperty("Pause", OBJECT_MEMBER(m_bPause), EBTYPE_BOOL_PROP));
}

HRESULT CShaderLighting::GenerateSphere(D3DXVECTOR3& vCenter, FLOAT fRadius, WORD wNumRings, WORD wNumSections, FLOAT sx, FLOAT sy, FLOAT sz)
{
    FLOAT x, y, z, v, rsintheta; // Temporary variables
    WORD  i, j, n, m;            // counters
    D3DXVECTOR3 vPoint;
	HRESULT hr;

	SAFE_RELEASE(m_pSphereBuffer);

    //Generate space for the required triangles and vertices.
    WORD       wNumTriangles = (wNumRings + 1) * wNumSections * 2;
    DWORD      dwNumIndices   = wNumTriangles*3;
    DWORD      dwNumVertices  = (wNumRings + 1) * wNumSections + 2;

    Vertex* pvVertices     = new Vertex[dwNumVertices];
    WORD*      pwIndices      = new WORD[3*wNumTriangles];

    // Generate vertices at the top and bottom points.
    D3DXVECTOR3 vTopPoint  = vCenter + D3DXVECTOR3( 0.0f, +sy*fRadius, 0.0f);
    D3DXVECTOR3 vBotPoint  = vCenter + D3DXVECTOR3( 0.0f, -sy*fRadius, 0.0f);
    D3DXVECTOR3 vNormal = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

    pvVertices[0]               = Vertex( D3DXVECTOR3(vTopPoint.x, vTopPoint.y, vTopPoint.z),  D3DXVECTOR3(vNormal.x, vNormal.y, vNormal.z), D3DXVECTOR2(0.0f, 0.0f) );
    pvVertices[dwNumVertices-1] = Vertex( D3DXVECTOR3(vBotPoint.x, vTopPoint.y, vTopPoint.z), D3DXVECTOR3(-vNormal.x, -vNormal.y, -vNormal.z), D3DXVECTOR2(0.0f, 0.0f) );

    // Generate vertex points for rings
    FLOAT dtheta = (float)(D3DX_PI / (wNumRings + 2));     //Angle between each ring
    FLOAT dphi   = (float)(2*D3DX_PI / wNumSections); //Angle between each section
    FLOAT theta  = dtheta;
    n = 1; //vertex being generated, begins at 1 to skip top point

    for( i = 0; i < (wNumRings+1); i++ )
    {
        y = fRadius * (float)cos(theta); // y is the same for each ring
        v = theta / D3DX_PI;     // v is the same for each ring
        rsintheta = fRadius * (float)sin(theta);
        FLOAT phi = 0.0f;

        for( j = 0; j < wNumSections; j++ )
        {
            x = rsintheta * (float)sin(phi);
            z = rsintheta * (float)cos(phi);
        
            FLOAT u = (FLOAT)(1.0 - phi / (2*D3DX_PI) );
            
            vPoint        = vCenter + D3DXVECTOR3( sx*x, sy*y, sz*z );
            vNormal       = D3DXVECTOR3( x/fRadius, y/fRadius, z/fRadius );
			D3DXVec3Normalize(&vNormal, &vNormal);
			pvVertices[n] = Vertex( D3DXVECTOR3(vPoint.x, vPoint.y, vPoint.z), D3DXVECTOR3(vNormal.x, vNormal.y, vNormal.z), D3DXVECTOR2(u, v) );

            phi += dphi;
            ++n;
        }
        theta += dtheta;
    }

    // Generate triangles for top and bottom caps.
    for( i = 0; i < wNumSections; i++ )
    {
        pwIndices[3*i+0] = 0;
        pwIndices[3*i+1] = i + 1;
        pwIndices[3*i+2] = 1 + ((i + 1) % wNumSections);

        pwIndices[3*(wNumTriangles - wNumSections + i)+0] = (WORD)( dwNumVertices - 1 );
        pwIndices[3*(wNumTriangles - wNumSections + i)+1] = (WORD)( dwNumVertices - 2 - i );
        pwIndices[3*(wNumTriangles - wNumSections + i)+2] = (WORD)( dwNumVertices - 2 - 
                ((1 + i) % wNumSections) );
    }

    // Generate triangles for the rings
    m = 1;            // first vertex in current ring,begins at 1 to skip top point
    n = wNumSections; // triangle being generated, skip the top cap 
        
    for( i = 0; i < wNumRings; i++ )
    {
        for( j = 0; j < wNumSections; j++ )
        {
            pwIndices[3*n+0] = m + j;
            pwIndices[3*n+1] = m + wNumSections + j;
            pwIndices[3*n+2] = m + wNumSections + ((j + 1) % wNumSections);
            
            pwIndices[3*(n+1)+0] = pwIndices[3*n+0];
            pwIndices[3*(n+1)+1] = pwIndices[3*n+2];
            pwIndices[3*(n+1)+2] = m + ((j + 1) % wNumSections);
            
            n += 2;
        }
        m += wNumSections;
    }

	// Put the sphere in a VB.
	hr = m_pD3DDev->CreateVertexBuffer(dwNumIndices * (sizeof(D3DXVECTOR3) * 3), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_pSphereBuffer);
    if (FAILED(hr))
	{
		delete [] pvVertices;
		delete [] pwIndices;

        return hr;
	}

	SphereVertex* pSphereVertexBase;
	SphereVertex* pSphereVertex;
	DWORD dwVertices;

	hr = m_pSphereBuffer->Lock(0, dwNumIndices * (sizeof(D3DXVECTOR3) * 3), (BYTE **)&pSphereVertexBase, 0);
	if (FAILED(hr))
        return hr;

	pSphereVertex = pSphereVertexBase;
	for (dwVertices = 0; dwVertices < dwNumIndices; dwVertices++)
	{
		pSphereVertex->Position = pvVertices[pwIndices[dwVertices]].Position;
		pSphereVertex->Normal = pvVertices[pwIndices[dwVertices]].Normal;
		pSphereVertex++;
	}

	pSphereVertex = pSphereVertexBase;
	for (dwVertices = 0; dwVertices < dwNumIndices; dwVertices += 3)
	{
		D3DXVECTOR3 vecV0,vecV1;
		D3DXVECTOR3 vecFaceNormal;
		vecV0 = pvVertices[pwIndices[dwVertices]].Position - pvVertices[pwIndices[dwVertices + 1]].Position;
		vecV1 = pvVertices[pwIndices[dwVertices]].Position - pvVertices[pwIndices[dwVertices + 2]].Position;

		D3DXVec3Cross(&vecFaceNormal, &vecV0, &vecV1);
		D3DXVec3Normalize(&vecFaceNormal, &vecFaceNormal);
		
		// Copy the face normal into all 3 buffers
		pSphereVertex->FaceNormal = vecFaceNormal;
		pSphereVertex++;

		pSphereVertex->FaceNormal = vecFaceNormal;
		pSphereVertex++;
		
		pSphereVertex->FaceNormal = vecFaceNormal;
		pSphereVertex++;
	}

	m_dwSphereFaces = (dwNumIndices / 3);

	m_pSphereBuffer->Unlock();

	delete [] pvVertices;
	delete [] pwIndices;
    return S_OK;
}


HRESULT CShaderLighting::Initialize(IDirect3DDevice8* pDev)
{
	HRESULT hr;
	vector<DWORD> Declaration;

	m_pD3DDev = pDev;
	pDev->AddRef();

	m_pNVDevice = new NVLightingDevice(pDev, this);

	//initialize mouse UI
	RECT rect;
	rect.left = rect.top = 0;
	D3DVIEWPORT8 viewport;
	m_pD3DDev->GetViewport(&viewport);
	rect.bottom = viewport.Height;
	rect.right  = viewport.Width;
	m_pUI = new MouseUI((const RECT)rect);

	DWORD dwVBFlags = D3DUSAGE_WRITEONLY;

	switch(m_LightType)
	{
		case LIGHTTYPE_MANYPOINT:
			m_dwNumLights = 17;
			break;
		case LIGHTTYPE_POINT:
			m_dwNumLights = 3;
			break;
		case LIGHTTYPE_DIRECTIONAL:
			m_dwNumLights = 2;
			break;
		case LIGHTTYPE_TWOSIDE:
			m_pUI->OnLButtonDown( 40, 10 );
			m_pUI->OnMouseMove( 30, 0 );
			m_pUI->OnLButtonUp( 3, 30 );
			m_dwNumLights = 1;
			break;
	}

	m_LightColorDiffuse.resize(m_dwNumLights);
	m_LightColorSpecular.resize(m_dwNumLights);
	m_LightColorAmbient.resize(m_dwNumLights);
	
	// Setup the light colors
	srand(1);
	for (DWORD dwCurrentLight = 0; dwCurrentLight < m_dwNumLights; dwCurrentLight++)
	{
		switch (dwCurrentLight)
		{
			case 0:
				m_LightColorDiffuse[dwCurrentLight] = D3DXCOLOR(0.8f, 0.0f, 0.0f, 1.0f);
				m_LightColorSpecular[dwCurrentLight] = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
				m_LightColorAmbient[dwCurrentLight] = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
				break;
			case 1:
				m_LightColorDiffuse[dwCurrentLight] = D3DXCOLOR(0.0f, 0.8f, 0.0f, 1.0f);
				m_LightColorSpecular[dwCurrentLight] = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
				m_LightColorAmbient[dwCurrentLight] = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
				break;
			case 2:
				m_LightColorDiffuse[dwCurrentLight] = D3DXCOLOR(0.0f, 0.0f, 0.8f, 1.0f);
				m_LightColorSpecular[dwCurrentLight] = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
				m_LightColorAmbient[dwCurrentLight] = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
				break;
			default:
				float fLuminance = (float)(dwCurrentLight / (float)(m_dwNumLights + 1));
				D3DXCOLOR Luminance(fLuminance, fLuminance, fLuminance, 1.0f);
				D3DXCOLOR Color((float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, 1.0f);
				D3DXColorModulate(&m_LightColorDiffuse[dwCurrentLight], &Luminance, &Color);
				break;
		}
	}

	// We want a white light in the twosided case
	if (m_LightType == LIGHTTYPE_TWOSIDE)
	{
		m_LightColorDiffuse[0] = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// Create a shape to light
	if (m_LightType == LIGHTTYPE_TWOSIDE)
	{
		hr = GenerateSphere(D3DXVECTOR3(0.0f, 0.0f, 0.0f),	// Center
					0.8f,				// Radius
					10,					// Rings
					20,					// Sections
					1.0f, 1.0f, 1.0f	// Scaling
					);	
		if (FAILED(hr))
		{
			m_strLastError = "Failed to generate sphere";
			return hr;
		}
	}
	else
	{
		m_pShapeMesh = new NVMesh();
		hr = m_pShapeMesh->Create(m_pNVDevice, GetFilePath("teapot.x"));
		if (FAILED(hr))
		{
			m_strLastError = "Failed to create teapot!";
			return hr;
		}

		m_pShapeMesh->SetFVF(m_pNVDevice, D3DFVF_NORMAL | D3DFVF_XYZ | D3DFVF_TEX1);
	}
	
	m_pLightMesh = new NVMesh();
	// Create a shape to display the position of the light
	switch(m_LightType)
	{
		case LIGHTTYPE_MANYPOINT:
		case LIGHTTYPE_POINT:
			hr = m_pLightMesh->Create(m_pNVDevice, GetFilePath("sphere.x"));
			if (FAILED(hr))
			{
				m_strLastError = "Could not load sphere.x";
				return hr;
			}
			break;
		case LIGHTTYPE_DIRECTIONAL:
		case LIGHTTYPE_TWOSIDE:
			hr = m_pLightMesh->Create(m_pNVDevice, GetFilePath("arrow.x"));
			if (FAILED(hr))
			{
				m_strLastError = "Could not load arrow.x";
				return hr;
			}
			break;
	}
	m_pLightMesh->SetFVF(m_pNVDevice, D3DFVF_NORMAL | D3DFVF_XYZ);

	Declaration.clear();
	Declaration.push_back(D3DVSD_STREAM(0));
	Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(1, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_END());
	hr = LoadAndCreateShader(GetFilePath("fixedcolor.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwFixedColorShader);
	if (FAILED(hr))
		return hr;


	Declaration.clear();
	Declaration.push_back(D3DVSD_STREAM(0));

	switch(m_LightType)
	{
		case LIGHTTYPE_MANYPOINT:
			Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3));
			Declaration.push_back(D3DVSD_REG(1, D3DVSDT_FLOAT3));
			Declaration.push_back(D3DVSD_REG(2, D3DVSDT_FLOAT2));
			Declaration.push_back(D3DVSD_END());
			hr = LoadAndCreateShader(GetFilePath("ManyPoint.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwCurrentShader);
			if (FAILED(hr))
				return hr;
			
			break;
		case LIGHTTYPE_POINT:
			Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3));
			Declaration.push_back(D3DVSD_REG(1, D3DVSDT_FLOAT3));
			Declaration.push_back(D3DVSD_REG(2, D3DVSDT_FLOAT2));
			Declaration.push_back(D3DVSD_END());
			hr = LoadAndCreateShader(GetFilePath("PointLight.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwCurrentShader);
			if (FAILED(hr))
				return hr;

			break;
		case LIGHTTYPE_DIRECTIONAL:
			Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3));
			Declaration.push_back(D3DVSD_REG(1, D3DVSDT_FLOAT3));
			Declaration.push_back(D3DVSD_REG(2, D3DVSDT_FLOAT2));
			Declaration.push_back(D3DVSD_END());
			hr = LoadAndCreateShader(GetFilePath("DirectionalLight.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwCurrentShader);
			if (FAILED(hr))
				return hr;

			break;
		case LIGHTTYPE_TWOSIDE:
			Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3));
			Declaration.push_back(D3DVSD_REG(1, D3DVSDT_FLOAT3));
			Declaration.push_back(D3DVSD_REG(2, D3DVSDT_FLOAT3));
			Declaration.push_back(D3DVSD_END());
			hr = LoadAndCreateShader(GetFilePath("TwoSide.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwCurrentShader);
			if (FAILED(hr))
				return hr;

			break;
	}

	if (m_pShapeMesh)
		m_pShapeMesh->SetVertexShader(m_dwCurrentShader);

	if (m_pLightMesh)
		m_pLightMesh->SetVertexShader(m_dwFixedColorShader);

	// Setup constants
	m_pD3DDev->SetVertexShaderConstant(CV_ZERO, D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_ONE, D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), 1);

	// Half vector is used in lighting calculations
	D3DXVECTOR4 HalfVector(0.0f, 0.0f, -1.0f, 0.0f);
	m_pD3DDev->SetVertexShaderConstant(CV_EYE_VECTOR, &HalfVector.x, 1);

	// material power 
	D3DXVECTOR4 MaterialPower(0.0f, 0.0f, 0.0f, 5.0f);
	m_pD3DDev->SetVertexShaderConstant(CV_MATPOWER, &MaterialPower.x, 1);

	// Attenuation for lights
	D3DXVECTOR4 Attenuation(1.0f, 0.0f, 0.0f, 0.0f);
	m_pD3DDev->SetVertexShaderConstant(CV_LIGHT1_ATTENUATION, &Attenuation.x, 1);
	m_pD3DDev->SetVertexShaderConstant(CV_LIGHT2_ATTENUATION, &Attenuation.x, 1);
	m_pD3DDev->SetVertexShaderConstant(CV_LIGHT3_ATTENUATION, &Attenuation.x, 1);

	// Camera stuff
	D3DXMATRIX matProj;
	D3DXMATRIX matView;

	D3DXVECTOR3 vEyePt = D3DXVECTOR3( 0.0f, 0.0f, -3.5f );
	D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUp = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

	// View
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUp);

	// Projection
	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(60.0f), 1.0f, 0.5f, 20.0f);

	m_pNVDevice->SetViewTransform(&matView);
	m_pNVDevice->SetProjectionTransform(&matProj);

	if (m_pShapeMesh)
		m_pShapeMesh->RestoreDeviceObjects(m_pNVDevice);

	if (m_pLightMesh)
		m_pLightMesh->RestoreDeviceObjects(m_pNVDevice);

	return S_OK;

}

HRESULT CShaderLighting::Free()
{
	SAFE_DELETE(m_pShapeMesh);
	SAFE_DELETE(m_pLightMesh);
	SAFE_RELEASE(m_pSphereBuffer);
	SAFE_DELETE(m_pNVDevice);
	SAFE_DELETE(m_pUI);
	if (m_pD3DDev)
	{
		m_pD3DDev->DeleteVertexShader(m_dwCurrentShader);
		SAFE_RELEASE(m_pD3DDev);
	}
	
	return S_OK;
}

HRESULT CShaderLighting::Start()
{
	m_fAngle = 0.0f;
	return S_OK;
}

  
HRESULT CShaderLighting::SetTransform()
{
	D3DXMATRIX matTemp;
	D3DXMATRIX matWorldViewProj;
	D3DXMATRIX matWorldView;
	D3DXMATRIX matWorldViewIT;

	D3DXMATRIX matWorld = m_pNVDevice->GetWorldTransform();
	D3DXMATRIX matProj = m_pNVDevice->GetProjectionTransform();
	D3DXMATRIX matView = m_pNVDevice->GetViewTransform();

	D3DXMatrixMultiply(&matTemp, &matWorld, &matView);
	D3DXMatrixMultiply(&matWorldViewProj, &matTemp, &matProj);
	D3DXMatrixMultiply(&matWorldView, &matWorld, &matView);
	
	// Create a 3x3 invertse of the worldview for the normal transformation (we transpose it as we load it into
	// the constant store)
	D3DXMatrixInverse(&matWorldViewIT, NULL, &matWorldView);
		
	// Projection to clip space
	D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matWorldViewProj(0, 0), 4);
	D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);

	// Transform to eye space
	D3DXMatrixTranspose(&matWorldView, &matWorldView);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEW_0, &matWorldView(0, 0), 4);
	D3DXMatrixTranspose(&matWorldView, &matWorldView);

	// Transform for normals
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWIT_0, &matWorldViewIT(0, 0), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWIT_1, &matWorldViewIT(1, 0), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWIT_2, &matWorldViewIT(2, 0), 1);

	return S_OK;

}

HRESULT CShaderLighting::Tick(EBTimer* pTimer)
{
	HRESULT hr = S_OK;
	DWORD i;
	D3DXMATRIX matWorld;
	D3DXMATRIX matView = m_pNVDevice->GetViewTransform();
	D3DXMATRIX matTemp;

	// Clear to grey
	hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0xAA, 0xAA, 0xAA ), 1.0, 0);

	m_pD3DDev->SetRenderState(D3DRS_FILLMODE, m_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID);

	// Increase rotation
	if (!m_bPause)
	{
		m_fAngle = pTimer->GetDuration() * 100.0f;
	}

	matWorld = m_pUI->GetRotationMatrix() * m_pUI->GetTranslationMatrix();
	if (m_LightType != LIGHTTYPE_TWOSIDE)
	{
		const NVBounds* pBounds = m_pShapeMesh->GetBounds();

		// Move the loaded object to the middle and scale
		D3DXMatrixScaling(&matTemp, 1.0f / pBounds->m_fRadius, 1.0f / pBounds->m_fRadius, 1.0f / pBounds->m_fRadius);
		D3DXMatrixMultiply(&matWorld, &matWorld, &matTemp);

		D3DXMatrixTranslation(&matTemp, -pBounds->m_vecCenter.x, -pBounds->m_vecCenter.y, -pBounds->m_vecCenter.z);
		D3DXMatrixMultiply(&matWorld, &matTemp, &matWorld);
	}
    
		
	// Load transform data.
	m_pNVDevice->SetWorldTransform(&matWorld);
	SetTransform();

	// Update the light positions
	vector<D3DXVECTOR3>vLight;
	vLight.resize(m_dwNumLights);

	float fAngleIncr = 360.0f / m_dwNumLights;
	for (i = 0; i < m_dwNumLights; i++)
	{
		float x, y, z;
		float fLightDistance = 0.3f + 1.0f;
		D3DXVECTOR3 vecLightAngles;
		fLightDistance = 1.0f;
		//fLightDistance = 0.5f;
		vecLightAngles.x = D3DXToRadian(-m_fAngle + fAngleIncr * i);
		vecLightAngles.y = D3DXToRadian(m_fAngle + fAngleIncr * i);
		vecLightAngles.z = D3DXToRadian(m_fAngle + 45.0f + fAngleIncr * i);

		x = fLightDistance * cos(vecLightAngles.x);
		y = fLightDistance * sin(vecLightAngles.y);
		z = fLightDistance * sin(vecLightAngles.z);

		// For directional, create a light vector that points towards the origin
		switch(m_LightType)
		{
			case LIGHTTYPE_TWOSIDE:
				z = 1.1f;
				x = -x;
				y = -y;
				break;
			case LIGHTTYPE_POINT:
			case LIGHTTYPE_MANYPOINT:
				break;
			case LIGHTTYPE_DIRECTIONAL:
				x = -x;
				y = -y;
				z = -z;
				break;
		}

		vLight[i] = D3DXVECTOR3(x, y, z);

		D3DXVECTOR4 vLightEye;
		if (m_LightType == LIGHTTYPE_POINT)
		{
			vLight[i] += D3DXVECTOR3(matWorld._41, matWorld._42, matWorld._43);
			D3DXVec3Transform(&vLightEye, &vLight[i], &matView);
		}
		else if (m_LightType == LIGHTTYPE_MANYPOINT)
		{
			D3DXMATRIX matWorldInverse;

			vLight[i] += D3DXVECTOR3(matWorld._41, matWorld._42, matWorld._43);
			// Transform light back to model space where the lighting will be done
			D3DXMatrixInverse(&matWorldInverse, NULL, &matWorld);
			D3DXVec3Transform(&vLightEye, &vLight[i], &matWorldInverse);
		}
		else
		{
			D3DXVECTOR3 vLightToEye;

			// Transform direction vector into eye space
			D3DXVec3Normalize(&vLightToEye, &vLight[i]);
			D3DXVec3TransformNormal(&vLightToEye, &vLightToEye, &matView);
			D3DXVec3Normalize(&vLightToEye, &vLightToEye);

			// Shader math requires that the vector is to the light
			vLightEye.x = -vLightToEye.x;
			vLightEye.y = -vLightToEye.y;
			vLightEye.z = -vLightToEye.z;
			vLightEye.w = 1.0f;
		}
			
		m_pD3DDev->SetVertexShaderConstant(CV_LIGHT1_POSITION + i, &vLightEye.x, 1);			
	}


	// Draw the object
	switch(m_LightType)
	{
		case LIGHTTYPE_TWOSIDE:
			m_pD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			break;
		default:
			m_pD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
			break;
	}

	if (m_LightType != LIGHTTYPE_TWOSIDE)
	{
		m_pShapeMesh->Render(m_pNVDevice);
	}
	else
	{
		m_pD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		// Point at the vertex data
		hr = m_pD3DDev->SetStreamSource(0, m_pSphereBuffer, (sizeof(D3DXVECTOR3) * 3));
		if (FAILED(hr))
			return hr;

		D3DXCOLOR LightFront;
		D3DXCOLOR LightBack;
		
		D3DXCOLOR matFront(1.0f, 0.0f, 0.0f, 1.0f);
		D3DXCOLOR matBack(1.0f, 1.0f, 1.0f, 1.0f);

		// Lighting colors - Light 1
		D3DXColorModulate(&LightFront, &m_LightColorDiffuse[0], &matFront);
		D3DXColorModulate(&LightBack, &m_LightColorDiffuse[0], &matBack);

		m_pD3DDev->SetVertexShaderConstant(CV_FRONTCOLOR, &LightFront.r, 1);
		m_pD3DDev->SetVertexShaderConstant(CV_BACKCOLOR, &LightBack.r, 1);

		m_pD3DDev->SetVertexShader(m_dwCurrentShader);

		m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_dwSphereFaces / 2);
	}
	
	// Draw the lights
	m_pD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	hr = m_pD3DDev->SetVertexShader(m_dwFixedColorShader);
	if (FAILED(hr))
		return hr;

	// Don't want textures
	m_pD3DDev->SetTexture(0, NULL);

	D3DXMATRIX TeapotWorld = matWorld;

	// Walk the list of lights, calculate the correct info needed in the vertex shader to apply them, 
	// and draw them
	for (i = 0; i < m_dwNumLights; i++)
	{
		const NVBounds* pBounds = m_pLightMesh->GetBounds();
		D3DXMatrixIdentity(&matWorld);

		// Translate to the middle
		D3DXMatrixTranslation(&matTemp, -pBounds->m_vecCenter.x, -pBounds->m_vecCenter.y, -pBounds->m_vecCenter.z);
		D3DXMatrixMultiply(&matWorld, &matWorld, &matTemp);

		float fLightScale = 0.1f; 
		D3DXMatrixScaling(&matTemp, fLightScale / pBounds->m_fRadius, fLightScale / pBounds->m_fRadius, fLightScale / pBounds->m_fRadius);
		D3DXMatrixMultiply(&matWorld, &matWorld, &matTemp);

		if ((m_LightType != LIGHTTYPE_POINT) && (m_LightType != LIGHTTYPE_MANYPOINT))
		{
			// Orientation of the light object (the arrow)
			D3DXVECTOR3 vLightObject = D3DXVECTOR3(0.0f, -1.0f, 0.0f);

			// Create the rotation axis
			D3DXVECTOR3 vOrthoNormal;
			D3DXVec3Cross(&vOrthoNormal, &vLight[i], &vLightObject);
			D3DXVec3Normalize(&vOrthoNormal, &vOrthoNormal);

			D3DXVECTOR3 vLightDirection;
			D3DXVec3Normalize(&vLightDirection, &vLight[i]);

			// Calculate the angle between the two vectors.
			float fAngle = acos(D3DXVec3Dot(&vLightDirection, &vLightObject));

			// Rotate the object about our rotation axis to map one vector onto another
			D3DXMatrixRotationAxis(&matTemp, &vOrthoNormal, -fAngle);
			D3DXMatrixMultiply(&matWorld, &matWorld, &matTemp);

			// vLight is a direction vector, pointing at the origin, so we negate it to find the position
			D3DXMatrixTranslation(&matTemp, -vLight[i].x, -vLight[i].y, -vLight[i].z);

			matTemp._41 += TeapotWorld._41;
			matTemp._42 += TeapotWorld._42;
			matTemp._43 += TeapotWorld._43;

			D3DXMatrixMultiply(&matWorld, &matWorld, &matTemp);
		}
		else
		{
			// Put the light at it's location
			D3DXMatrixTranslation(&matTemp,vLight[i].x, vLight[i].y, vLight[i].z);
			D3DXMatrixMultiply(&matWorld, &matWorld, &matTemp);
		}


		m_pNVDevice->SetWorldTransform(&matWorld);
		SetTransform();

		// Set the fixed color for the light
		m_dwCurrentLightDraw = i;
		
		m_pLightMesh->Render(m_pNVDevice);		
	}
	m_dwCurrentLightDraw = -1;

	return hr;
}

// This setmaterial call is made by the mesh renderer when it needs to change the material for the shape
// it is about to draw.  We use the opportunity to calculate the light properties for the lights shining 
// on the material
HRESULT NVLightingDevice::SetMaterial(D3DMATERIAL8* pMat)
{
	DWORD dwCurrentLight;

	D3DXCOLOR LightAmbient;
	D3DXCOLOR LightDiffuse;
	D3DXCOLOR LightSpecular;
	
	D3DXCOLOR matAmbient(0.0f, 0.6f, 0.6f, 1.0f);
	D3DXCOLOR matDiffuse(pMat->Diffuse);
	D3DXCOLOR matSpecular(pMat->Specular);
			
	// Lighting colors - Light 1
	for (dwCurrentLight = 0; dwCurrentLight < m_pLightingShader->m_dwNumLights; dwCurrentLight++)
	{
		D3DXColorModulate(&LightAmbient, &m_pLightingShader->m_LightColorAmbient[dwCurrentLight], &matAmbient);
		D3DXColorModulate(&LightDiffuse, &m_pLightingShader->m_LightColorDiffuse[dwCurrentLight], &matDiffuse);
		D3DXColorModulate(&LightSpecular, &m_pLightingShader->m_LightColorSpecular[dwCurrentLight], &matSpecular);

		// We only do ambient/specular for the directional/point light case.
		// The many point lights sample doesn't do ambient and specular
		if (m_pLightingShader->m_LightType != LIGHTTYPE_MANYPOINT)
		{
			m_pD3DDev->SetVertexShaderConstant(CV_LIGHT1_AMBIENT + dwCurrentLight, &LightAmbient.r, 1);
			m_pD3DDev->SetVertexShaderConstant(CV_LIGHT1_SPECULAR + dwCurrentLight, &LightSpecular.r, 1);
		}
		m_pD3DDev->SetVertexShaderConstant(CV_LIGHT1_DIFFUSE + dwCurrentLight, &LightDiffuse.r, 1);

		// Are we currently drawing the lights themselves?
		if (m_pLightingShader->m_dwCurrentLightDraw != -1)
		{
			// By default the fixed color is loaded with the light color, but when we are rendering the 
			// 'handle' of the directional arrows, we want to use the default material
			if ((GetRenderPart() == 0) && 
				((m_pLightingShader->m_LightType == LIGHTTYPE_DIRECTIONAL) || (m_pLightingShader->m_LightType == LIGHTTYPE_TWOSIDE)))
				m_pD3DDev->SetVertexShaderConstant(CV_FIXED_COLOR, &pMat->Diffuse.r, 1);
			else
				m_pD3DDev->SetVertexShaderConstant(CV_FIXED_COLOR, &m_pLightingShader->m_LightColorDiffuse[m_pLightingShader->m_dwCurrentLightDraw].r, 1);
		}

	}

	return S_OK;
}


HRESULT CShaderLighting::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
{
	return S_OK;
}

void CShaderLighting::MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y)
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
	return;
}

void CShaderLighting::MouseMove(HWND hWnd, int x, int y)
{
	m_pUI->OnMouseMove(x, y);
	return;
}

void CShaderLighting::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
{
	eEBKeyAction Action = TranslateEffectKey(dwKey, nFlags, bDown);
	
	static const float fTranslateMagnitude = 0.1f;
    switch ( Action )
    {
		case EB_HELP:
		{
			::MessageBoxEx( NULL, " Help : F1 - Help \n\n Home - Reset To Defaults \n\n W - Wireframe Toggle \n\n Space\\Pause - Toggle Pause/Resume \n\n Left Button & Mouse - Rotate Object\n\n Shift Left Button & Mouse - Pan Camera \n\n Ctrl Left Button & Mouse - Move Camera In & Out\n\n",
				   "Help", MB_ICONINFORMATION | MB_TASKMODAL, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ) );
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
            m_pUI->Reset();
            m_bWireframe = false;
			m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
            
        }
		break;

		case EB_PAUSE:
        {
            m_bPause = !m_bPause;
			m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
        }
        break;


        default :
            break;
    }
}