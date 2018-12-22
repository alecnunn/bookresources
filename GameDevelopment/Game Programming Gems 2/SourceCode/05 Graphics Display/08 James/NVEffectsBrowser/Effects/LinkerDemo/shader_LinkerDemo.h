/*********************************************************************NVMH2****
Path:  E:\devrel\NV_SDK_4\DX8\NVEffectsBrowser\Effects\Rainbow
File:  shader_Rainbow.h

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/


#ifndef __SHADER_LINKERDEMO_H
#define __SHADER_LINKERDEMO_H
#include "MouseUI.h"
#include <string>
#include "linkercommon.h"
#include "nvlink.h"
#include "nvinc.h"
#include "nvresourcemanager.h"
#include "nvstatemanager.h"
#include "nvcomplexmesh.h"

#define NUM_PASSES 2

typedef enum eTagFragmentNames
{
	// Setup stuff
	F_SET_AMBIENT = 0,
	F_SET_DEFAULT_TEXTURE,
	F_SET_POWER,
	F_XFORM_WORLDVIEWPROJECTION,
	F_CLAMP_DIFFUSE_EXPONENTIATE_SPECULAR,
	F_CALCULATE_CUBEREFLECTION_VECTOR,
	F_CALCULATE_LINEAR_RANGE_FOG,
	F_POINTLIGHT_ACCUMULATE_DIFFUSE,
	F_POINTLIGHT_ACCUMULATE_DIFFUSE_LAST,
	F_POINTLIGHT_ACCUMULATE_SPECULAR_FIRST,
	F_POINTLIGHT_ACCUMULATE_SPECULAR,
	F_POINTLIGHT_ACCUMULATE_SPECULAR_LAST,	
	F_POINTLIGHT_ACCUMULATE_SPECULAR_FIRST_AND_LAST,	
	F_DIRLIGHT_ACCUMULATE_DIFFUSE,
	F_DIRLIGHT_ACCUMULATE_DIFFUSE_LAST,
	F_DIRLIGHT_ACCUMULATE_SPECULAR_FIRST,
	F_DIRLIGHT_ACCUMULATE_SPECULAR,
	F_DIRLIGHT_ACCUMULATE_SPECULAR_LAST,	
	F_DIRLIGHT_ACCUMULATE_SPECULAR_FIRST_AND_LAST,	
	// Eyespace fragments
	F_CREATE_EYESPACE_VERTEX,
	F_CREATE_EYESPACE_EYEVECTOR_LOCAL_VIEWER,
	F_CREATE_EYESPACE_NORMAL,
	F_CALCULATE_EYESPACE_REFLECTION,
	F_CALCULATE_EYESPACE_DIRECTIONALLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_NONLOCALVIEWER,
	F_CALCULATE_EYESPACE_DIRECTIONALLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_LOCALVIEWER,
	F_CALCULATE_EYESPACE_DIRECTIONALLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_RDOTL,
	F_CALCULATE_EYESPACE_DIRECTIONALLIGHT_DIFFUSE_INTENSITY,
	F_CALCULATE_EYESPACE_POINTLIGHT_VECTOR,
	F_CALCULATE_EYESPACE_POINTLIGHT_DIFFUSE_INTENSITY,
	F_CALCULATE_EYESPACE_POINTLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_LOCALVIEWER,
	F_CALCULATE_EYESPACE_POINTLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_NONLOCALVIEWER,
	F_CALCULATE_EYESPACE_POINTLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_RDOTL,
	F_CALCULATE_BASIS_VECTORS,
	// Objectspace fragments
	F_CREATE_OBJECTSPACE_EYEVECTOR_LOCAL_VIEWER,
	F_CALCULATE_OBJECTSPACE_REFLECTION,
	F_CALCULATE_OBJECTSPACE_DIRECTIONALLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_NONLOCALVIEWER,
	F_CALCULATE_OBJECTSPACE_DIRECTIONALLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_LOCALVIEWER,
	F_CALCULATE_OBJECTSPACE_DIRECTIONALLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_RDOTL,
	F_CALCULATE_OBJECTSPACE_DIRECTIONALLIGHT_DIFFUSE_INTENSITY,
	F_CALCULATE_OBJECTSPACE_POINTLIGHT_VECTOR,
	F_CALCULATE_OBJECTSPACE_POINTLIGHT_DIFFUSE_INTENSITY,
	F_CALCULATE_OBJECTSPACE_POINTLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_LOCALVIEWER,
	F_CALCULATE_OBJECTSPACE_POINTLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_NONLOCALVIEWER,
	F_CALCULATE_OBJECTSPACE_POINTLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_RDOTL,
	// Output stuff
	F_WRITE_TEXTURE,
	F_WRITE_DEFAULT_DIFFUSE,
	F_WRITE_DEFAULT_TEXTURE,
	F_WRITE_DEFAULT_SPECULAR,
	F_NUM_FRAGMENTS
} eFragmentName;

typedef enum eTagConstantNames
{
	C_AMBIENT_LIGHT = 0,
	C_CONSTANTS,
	C_OBJECTSPACE_EYEVECTOR,
	C_OBJECTSPACE_EYEPOSITION,
	C_EYESPACE_EYEVECTOR,
	C_WORLDVIEWPROJECTION_0,
	C_WORLDVIEWPROJECTION_1,
	C_WORLDVIEWPROJECTION_2,
	C_WORLDVIEWPROJECTION_3,
	C_WORLDVIEWIT_0,
	C_WORLDVIEWIT_1,
	C_WORLDVIEWIT_2,
	C_WORLDVIEW_0,
	C_WORLDVIEW_1,
	C_WORLDVIEW_2,
	C_WORLDVIEW_3,
	C_WORLDVIEWCUBEMAP_0,
	C_WORLDVIEWCUBEMAP_1,
	C_WORLDVIEWCUBEMAP_2,
	C_CUBEMATRIX_0,
	C_CUBEMATRIX_1,
	C_CUBEMATRIX_2,
	C_DIRLIGHT_DIRECTION,
	C_DIRLIGHT_DIFFUSE,
	C_DIRLIGHT_SPECULAR,
	C_POINTLIGHT_POSITION,
	C_POINTLIGHT_DIFFUSE,
	C_POINTLIGHT_SPECULAR,
	C_MAT_POWER,
	C_DIRLIGHT_HALFVECTOR,
	C_FOGCONST,
	C_NUM_CONSTANTS,
} eConstantName;

typedef enum eTagVertexNames
{
	V_POSITION,
	V_NORMAL,
	V_DIFFUSE,
	V_SPECULAR,
	V_SBASIS0,
	V_TBASIS0,
	V_SXTBASIS0,
	V_TEXTURE0,	// This one is fixed = FIXED_TEXTURE0_LOCATION
	V_TEXTURE1,
	V_TEXTURE2,
	V_TEXTURE3,
	V_TEXTURE4,
	V_TEXTURE5,
	V_TEXTURE6,
	V_TEXTURE7,
	V_NUM_VERTICES
} eVertexName;

typedef enum tagLightType
{
	LIGHT_DIRECTIONAL,
	LIGHT_POINT,
	LIGHT_SPOT
} LightType;

typedef enum tagLightSpace
{
	LIGHT_OBJECT,
	LIGHT_WORLD,
	LIGHT_EYE
} LightSpace;

class WorldBoxVertex
{
public:
	WorldBoxVertex(const D3DXVECTOR3& vecPosition)
		: m_vecPosition(vecPosition),
			m_vecTexture(vecPosition) // Not an error! Copies position into texture coords to draw cubemap as box
	{};

	D3DXVECTOR3 m_vecPosition;
	D3DXVECTOR3 m_vecTexture;
};

class CLight
{
public:
	CLight(LightType Type = LIGHT_DIRECTIONAL) 
		: m_LightDirection(0.0f, 0.0f, 0.0f),
		m_LightPosition(0.0f, 0.0f, 0.0f),
		m_LightAttenuation(1.0f, 0.0f, 0.0f),
		m_LightTranslation(0.0f, 0.0f, 0.0f),
		m_LightType(Type)
	{};

	virtual ~CLight() {}

	
	void SetWorldMatrix(const D3DXMATRIX& World) { m_World = World;}
	void SetViewMatrix(const D3DXMATRIX& View) { m_View = View;}
	const D3DXMATRIX& GetWorldMatrix() const { return m_World; }
	const D3DXMATRIX& GetViewMatrix() const { return m_View; }

	// Light setup
	void SetLightType(const LightType Type) { m_LightType = Type; }
	void SetLightPosition(const D3DXVECTOR3& LightPosition) { m_LightPosition = LightPosition; }
	void SetLightDirection(const D3DXVECTOR3& LightDirection) { m_LightDirection = LightDirection; }
	
	void SetLightDiffuse(const D3DXVECTOR3& LightDiffuse) { m_LightDiffuse = LightDiffuse; }
	void SetLightSpecular(const D3DXVECTOR3& LightSpecular) { m_LightSpecular = LightSpecular; }
	void SetLightAmbient(const D3DXVECTOR3& LightAmbient) { m_LightAmbient = LightAmbient; }
	void SetLightAttenuation(const D3DXVECTOR3& LightAttenuation) { m_LightAttenuation = LightAttenuation; }
	void SetLightFalloff(float fFalloff) { m_fLightFalloff = fFalloff; }
	void SetLightTheta(float fLightTheta) { m_fLightTheta = fLightTheta; }
	void SetLightPhi(float fLightPhi) { m_fLightPhi = fLightPhi; }
	void SetLightRange(float fLightRange) { m_fLightRange = fLightRange; }
	void SetLightTranslation(const D3DXVECTOR3& LightTranslation) { m_LightTranslation = LightTranslation; }
	
	const LightType& GetLightType() const { return m_LightType; }
	const D3DXVECTOR3 GetLightPosition() const { return m_LightPosition + m_LightTranslation; }
	const D3DXVECTOR3& GetLightDirection() const { return m_LightDirection; }
	const D3DXVECTOR3& GetLightDiffuse() const { return m_LightDiffuse; }
	const D3DXVECTOR3& GetLightSpecular() const { return m_LightSpecular; }
	const D3DXVECTOR3& GetLightAmbient() const { return m_LightAmbient; }
	const D3DXVECTOR3& GetLightTranslation() const { return m_LightTranslation; }

	void SetLightRotateAxis(const D3DXVECTOR3& RotateAxis) { m_LightRotateAxis = RotateAxis; }
	const D3DXVECTOR3& GetLightRotateAxis() const { return m_LightRotateAxis; }

	const D3DXVECTOR4 GetLightSpacePosition(LightSpace Space) { UpdateLightVectors(Space); return m_LightSpacePosition; }
	const D3DXVECTOR4 GetLightSpaceDirection(LightSpace Space) { UpdateLightVectors(Space); return m_LightSpaceDirection; }
	void UpdateLightVectors(LightSpace Space)
	{
		D3DXVECTOR3 vecDirectionToSpace;
		D3DXVECTOR4 vecPositionToSpace;
		D3DXMATRIX* pMatTransform;

		D3DXVECTOR3 LightPos = m_LightPosition + m_LightTranslation;

		switch(Space)
		{
			case LIGHT_EYE:
				pMatTransform = &m_View;
				break;
			case LIGHT_OBJECT:
				D3DXMatrixInverse(&m_WorldI, NULL, &m_World);
				pMatTransform = &m_WorldI;
				break;
			case LIGHT_WORLD:
				m_LightSpacePosition.x = LightPos.x;
				m_LightSpacePosition.y = LightPos.y;
				m_LightSpacePosition.z = LightPos.z;
				m_LightSpacePosition.w = 1.0f;

				m_LightSpaceDirection.x = LightPos.x;
				m_LightSpaceDirection.y = LightPos.y;
				m_LightSpaceDirection.z = LightPos.z;
				m_LightSpaceDirection.w = 1.0f;
				return;
				break;
			default:
				assert(0);
				return;
				break;
		}

		// Get transformed direction
		D3DXMATRIX MatIT = *pMatTransform;
		D3DXMatrixInverse(&MatIT, NULL, &MatIT);
		D3DXMatrixTranspose(&MatIT, &MatIT);
		D3DXVec3Normalize(&vecDirectionToSpace, &m_LightDirection);
		D3DXVec3TransformNormal(&vecDirectionToSpace, &vecDirectionToSpace, &MatIT);
		D3DXVec3Normalize(&vecDirectionToSpace, &vecDirectionToSpace);

		// Get transformed position
		D3DXVec3TransformCoord((D3DXVECTOR3*)&vecPositionToSpace, &LightPos, pMatTransform);
		vecPositionToSpace.w = 1.0f;
		
		m_LightSpacePosition = vecPositionToSpace;
		
		m_LightSpaceDirection.x = -vecDirectionToSpace.x;
		m_LightSpaceDirection.y = -vecDirectionToSpace.y;
		m_LightSpaceDirection.z = -vecDirectionToSpace.z;
		m_LightSpaceDirection.w = 1.0f;
		
	}

private:

	// Light properties
	LightType m_LightType;
	
	D3DXVECTOR3 m_LightDirection;
	D3DXVECTOR3 m_LightPosition;
	D3DXVECTOR3 m_LightDiffuse;
	D3DXVECTOR3 m_LightSpecular;
	D3DXVECTOR3 m_LightAmbient;
	D3DXVECTOR3 m_LightAttenuation;
	float m_fLightTheta;
	float m_fLightPhi;
	float m_fLightFalloff;
	float m_fLightRange;
		
    
	D3DXMATRIX m_View;
	D3DXMATRIX m_World;
	
	// Internally used
	D3DXMATRIX m_WorldI;
	D3DXVECTOR4 m_LightSpacePosition;
	D3DXVECTOR4 m_LightSpaceDirection;
	D3DXVECTOR3 m_LightRotateAxis;
	D3DXVECTOR3 m_LightTranslation;

};

static const DWORD NUM_CUBE_VERTICES = (4*6);
static const DWORD NUM_CUBE_INDICES = (6*6);
static const DWORD NUM_CUBE_FACES = 6;

static const DWORD INVALID_PARAM = 0xFFFFFFFF;

static D3DXVECTOR3 DefaultAmbient(0.2f, 0.2f, 0.2f);
typedef enum eDisplayOption
{
	DISPLAY_MODEL = 0,
	DISPLAY_SPHERE = 1,
	DISPLAYOPTION_FORCE_DWORD = 0xFFFFFFFF
} eDisplayOption;

typedef std::vector<CLight> tLightList;

class CLinkerConstantSlot
{
public:
	CLinkerConstantSlot(NVLINK_CONSTANT_ID ConstantID, LONG Offset)
		: m_ConstantID(ConstantID),
		m_Offset(Offset)
	{}

	NVLINK_CONSTANT_ID GetID() const { return m_ConstantID; }
	void SetID(NVLINK_CONSTANT_ID ConstantID) { m_ConstantID = ConstantID; }

	LONG GetOffset() const { return m_Offset; }
	void SetOffset(LONG Offset) { m_Offset = Offset; }

	bool operator < (const CLinkerConstantSlot& rhs) const
	{
		if (m_ConstantID < rhs.m_ConstantID)
			return true;
		else if (m_ConstantID > rhs.m_ConstantID)
			return false;
		else if (m_Offset < rhs.m_Offset)
		 return true;

		return false;
	}

private:
	NVLINK_CONSTANT_ID m_ConstantID;
	LONG m_Offset;
};
typedef std::map<CLinkerConstantSlot, DWORD> tConstantInfoToSlot;

class CShaderLinkerDemo : public EBEffect
{
public:	
	CShaderLinkerDemo();
	~CShaderLinkerDemo();

	virtual void PropertyUpdateCallback(const EBProperty* pProperty, bool bWritten );
	virtual void    UpdateProperties();
	virtual HRESULT Initialize(IDirect3DDevice8* pDev);
	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick(EBTimer* pTimer);
	virtual HRESULT ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format);
	virtual void MouseMove(HWND hWnd, int x, int y);
	virtual void MouseButton(HWND hWnd, eButtonID Button, bool bDown, int x, int y);
	virtual void Keyboard(DWORD dwKey, UINT nFlags, bool bDown);

	
private: 
	void ConditionalSetConstant(DWORD dwPass, eConstantName Name, LONG lOffset, const void* Value);

	void UpdateMeshVertexSlots();

	HRESULT CreateCubeEnvironment();

	HRESULT UpdateShader();
	HRESULT LoadFragmentFile(const std::string& strFragmentFile);
	HRESULT LoadMesh(const std::string& strFileName);
	HRESULT LoadLightObjects();
	void FreeLightObjects();
	HRESULT DrawLightObjects();
	HRESULT DrawLight(CLight& Light);
	HRESULT DrawCubeEnvironment();
	HRESULT UpdateLights();

	D3DXVECTOR3 m_Eye;
	D3DXVECTOR3 m_LookAt;
	D3DXVECTOR3 m_Up;

	D3DXVECTOR4 m_vecFogData;

	//our matrices
	D3DXMATRIX m_matWorld;
	D3DXMATRIX m_matView;
	D3DXMATRIX m_matProjection;
	D3DXMATRIX m_matCubeRotation;
	
	HINSTANCE m_hLinker;
	NVLink* m_pLinker;
	
	float m_fAngle;
	float m_fBumpScale;

	eDisplayOption m_eDisplayOption;

	DWORD m_dwPass0Shader;
	DWORD m_dwPass1Shader;
	DWORD m_dwLightArrowShader;
	DWORD m_dwLightSphereShader;
	DWORD m_dwBumpPixelShader;
	std::string m_strFileName;
	DialogSettings m_Settings;

	NVLINK_FRAGMENT_ID m_FragmentID[F_NUM_FRAGMENTS];
	NVLINK_CONSTANT_ID m_ConstantID[C_NUM_CONSTANTS];
	NVLINK_VERTEX_ID m_VertexID[V_NUM_VERTICES];

	tConstantInfoToSlot m_ConstantSlots[NUM_PASSES];

	std::vector<DWORD> m_MeshDeclaration;

	bool m_bWireframe;
	bool m_bPause;;
	bool m_bSetShaderToShow;
	bool m_bSupportsNormalMap;
	bool m_bSupportsCubeMap;
	bool m_bHasTextureCoords;
	MouseUI* m_pUI;
	DWORD m_dwParent;

	nv_objects::NVComplexMesh* m_pMesh;
	nv_objects::NVComplexMesh* m_pSphereMesh;
	nv_objects::NVComplexMesh* m_pArrowMesh;

	nv_objects::NVResourceID m_GeneratedNormalMapID;

	tLightList m_DirectionalLights;
	tLightList m_PointLights;

	LPDIRECT3DVERTEXBUFFER8 m_pWorldBoxVertices;
	LPDIRECT3DINDEXBUFFER8 m_pWorldBoxIndices;

	D3DXVECTOR3 m_AmbientLight;

	HMODULE m_hDialog;
	LinkerDialogFunc m_DialogFunc;

	DWORD m_dwNumTriangles;
	DWORD m_dwNumVertices;

};

__inline D3DXVECTOR3 ColorModulate(const D3DXVECTOR3& Color1, const D3DXVECTOR3& Color2)
{
	return D3DXVECTOR3(Color1.x * Color2.x, Color1.y * Color2.y, Color1.z * Color2.z);
}


#endif

