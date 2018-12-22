//=============================================================================
// GPUFluidDemo.cpp by Frank Luna (C) 2005 All Rights Reserved.
//
// Demonstrates how to interactively render and disturb fluids on the GPU
// by applying a finite difference scheme on the GPU and using 
// displacement mapping.
//
// Controls: Use mouse to orbit and zoom; use the 'W' and 'S' keys to 
//           alter the height of the camera.
//=============================================================================

#include "d3dApp.h"
#include "DirectInput.h"
#include <crtdbg.h>
#include "GfxStats.h"
#include <list>
#include "Fluid.h"
#include "Vertex.h"

class GPUFluidDemo : public D3DApp
{
public:
	GPUFluidDemo(HINSTANCE hInstance, std::string winCaption, D3DDEVTYPE devType, DWORD requestedVP);
	~GPUFluidDemo();

	bool checkDeviceCaps();
	void onLostDevice();
	void onResetDevice();
	void updateScene(float dt);
	void drawScene();

private:
	// Helper methods
	void buildViewMtx();
	void buildProjMtx();
	void drawTerrain();
	void buildEffects();
	void drawSky();
	void drawTerrainReflected();
	void drawSkyReflected();

private:
	GfxStats* mGfxStats;

	Fluid* mFluid;
	D3DXPLANE mWaterPlaneW;

	DrawableTex2D* mRefractMap;
	DrawableTex2D* mReflectMap;

	ID3DXMesh* mTerrain;
	ID3DXEffect* mTerrainFX;
	IDirect3DTexture9* mTerrainTex;
	D3DXHANDLE mfxhTerrainTech;
	D3DXHANDLE mfxhWorld;
	D3DXHANDLE mfxhWorldInvTrans;
	D3DXHANDLE mfxhEyePosW;
	D3DXHANDLE mfxhTerrainWVP;
	D3DXHANDLE mfxhTexMap;

	ID3DXMesh* mSkyDome;
	IDirect3DCubeTexture9* mSkyEnvMap;
	ID3DXEffect* mSkyFX;
	D3DXHANDLE mfxhSkyTech;
	D3DXHANDLE mfxhSkyReflectTech;
	D3DXHANDLE mfxhSkyEnvMap;
	D3DXHANDLE mfxhSkyWVP;

	float mCameraRotationY;
	float mCameraRadius;
	float mCameraHeight;
	D3DXVECTOR3 mCameraPos;

	D3DXMATRIX mTerrainWorld;
	D3DXMATRIX mTerrainWorldInvTrans;
	D3DXMATRIX mFluidWorld;
	D3DXMATRIX mSkyWorld;

	D3DXMATRIX mView;
	D3DXMATRIX mProj;
};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
	#if defined(DEBUG) | defined(_DEBUG)
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif

	GPUFluidDemo app(hInstance, "Fluid GPU Demo", D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING);
	gd3dApp = &app;

	if( !app.checkDeviceCaps() )
	{
		MessageBox(0, "app.checkDeviceCaps() Failed", 0, 0);
		return 0;
	}

	DirectInput di(DISCL_NONEXCLUSIVE|DISCL_FOREGROUND, DISCL_NONEXCLUSIVE|DISCL_FOREGROUND);
	gDInput = &di;

	gd3dApp->enableFullScreenMode(true);
    return gd3dApp->run();
}

GPUFluidDemo::GPUFluidDemo(HINSTANCE hInstance, std::string winCaption, D3DDEVTYPE devType, DWORD requestedVP)
: D3DApp(hInstance, winCaption, devType, requestedVP)
{
	InitAllVertexDeclarations();

	mGfxStats = new GfxStats();

	mFluid    = new Fluid(256, 256, 0.2f, 0.03f, 4.0f, 0.7f);

	mWaterPlaneW = D3DXPLANE(0.0f, -1.0f, 0.0f, 0.0f);

	mCameraRadius    = 40.0f;
	mCameraRotationY = 0.5f * D3DX_PI;
	mCameraHeight    = 20.0f;
 
	D3DXMatrixScaling(&mTerrainWorld, 7.0f, 10.0f, 7.0f);
	D3DXMatrixInverse(&mTerrainWorldInvTrans, 0, &mTerrainWorld);
	D3DXMatrixTranspose(&mTerrainWorldInvTrans, &mTerrainWorldInvTrans);
	D3DXMatrixTranslation(&mFluidWorld, 0.0f, -0.2f, 0.0f); // Set water down a bit to avoid texture perturb artifacts.
	D3DXMatrixIdentity(&mSkyWorld);

	HR(D3DXLoadMeshFromX("Terrain.x", D3DXMESH_MANAGED, gd3dDevice, 0, 0, 0, 0, &mTerrain));
	DWORD* adj = new DWORD[mTerrain->GetNumFaces()*3];
	HR(mTerrain->GenerateAdjacency(EPSILON, adj));
	HR(mTerrain->OptimizeInplace(D3DXMESHOPT_VERTEXCACHE|D3DXMESHOPT_ATTRSORT,
		adj, 0, 0, 0));
	delete[] adj;
	HR(D3DXCreateTextureFromFile(gd3dDevice, "stone.jpg", &mTerrainTex));


	HR(D3DXCreateSphere(gd3dDevice, 1000.0f, 30, 30, &mSkyDome, 0));
	adj = new DWORD[mSkyDome->GetNumFaces()*3];
	HR(mSkyDome->GenerateAdjacency(EPSILON, adj));
	HR(mSkyDome->OptimizeInplace(D3DXMESHOPT_VERTEXCACHE|D3DXMESHOPT_ATTRSORT,
		adj, 0, 0, 0));
	delete[] adj;

	HR(D3DXCreateCubeTextureFromFile(gd3dDevice, "grassenvmap.dds", &mSkyEnvMap));


	// Counts do not include the drawing into the reflection/refraction maps.
	mGfxStats->addVertices(mFluid->numVertices());
	mGfxStats->addTriangles(mFluid->numTriangles());
	mGfxStats->addVertices(mTerrain->GetNumVertices());
	mGfxStats->addTriangles(mTerrain->GetNumFaces());
	mGfxStats->addVertices(mSkyDome->GetNumVertices());
	mGfxStats->addTriangles(mSkyDome->GetNumFaces());

	D3DVIEWPORT9 vp = {0, 0, 512, 512, 0.0f, 1.0f};
	mRefractMap = new DrawableTex2D(512, 512, 0, D3DFMT_A8R8G8B8, true, D3DFMT_D24X8, vp);
	mReflectMap = new DrawableTex2D(512, 512, 0, D3DFMT_X8R8G8B8, true, D3DFMT_D24X8, vp);
	buildEffects();

	onResetDevice();
}

GPUFluidDemo::~GPUFluidDemo()
{
	delete mGfxStats;
	delete mFluid;
	delete mRefractMap;
	delete mReflectMap;

	ReleaseCOM(mTerrain);
	ReleaseCOM(mTerrainFX);
	ReleaseCOM(mTerrainTex);
	ReleaseCOM(mSkyEnvMap);
	ReleaseCOM(mSkyFX);
	ReleaseCOM(mSkyDome);
	DestroyAllVertexDeclarations();
}

bool GPUFluidDemo::checkDeviceCaps()
{
	D3DCAPS9 caps;
	HR(gd3dDevice->GetDeviceCaps(&caps));

	// Check for vertex shader version 3.0 support.
	if( caps.VertexShaderVersion < D3DVS_VERSION(3, 0) )
		return false;

	// Check for pixel shader version 3.0 support.
	if( caps.PixelShaderVersion < D3DPS_VERSION(3, 0) )
		return false;

	// Check for D3DFMT_A32B32G32R32F render target and vertex texture fetch support.
	if(FAILED(md3dObject->CheckDeviceFormat(D3DADAPTER_DEFAULT, mDevType, md3dPP.BackBufferFormat, 
		D3DUSAGE_RENDERTARGET | D3DUSAGE_QUERY_VERTEXTEXTURE, 
		D3DRTYPE_TEXTURE, D3DFMT_A32B32G32R32F)))
		return false;

	// Check for D3DFMT_A16B16G16R16F render target and blending support.
	if(FAILED(md3dObject->CheckDeviceFormat(D3DADAPTER_DEFAULT, mDevType, md3dPP.BackBufferFormat, 
		D3DUSAGE_RENDERTARGET | D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
		D3DRTYPE_TEXTURE, D3DFMT_A16B16G16R16F)))
		return false;

	// Check for D3DFMT_A8R8G8B8 render target with D3DFMT_D24X8 depth format.
	if(FAILED(md3dObject->CheckDeviceFormat(D3DADAPTER_DEFAULT, mDevType, md3dPP.BackBufferFormat, 
		D3DUSAGE_RENDERTARGET, D3DRTYPE_TEXTURE, D3DFMT_A8R8G8B8)))
		return false;
	if(FAILED(md3dObject->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, mDevType, md3dPP.BackBufferFormat,
		D3DFMT_A8R8G8B8, D3DFMT_D24X8)))
		return false;

	return true;
}

void GPUFluidDemo::onLostDevice()
{
	mGfxStats->onLostDevice();
	mFluid->onLostDevice();
	HR(mTerrainFX->OnLostDevice());
	HR(mSkyFX->OnLostDevice());
	mRefractMap->onLostDevice();
	mReflectMap->onLostDevice();
}

void GPUFluidDemo::onResetDevice()
{
	mGfxStats->onResetDevice();
	mFluid->onResetDevice();
	HR(mTerrainFX->OnResetDevice());
	HR(mSkyFX->OnResetDevice());
	mRefractMap->onResetDevice();
	mReflectMap->onResetDevice();
	mRefractMap->clear(0xff000000);
	mReflectMap->clear(0xff000000);
 
	buildProjMtx();
}

void GPUFluidDemo::updateScene(float dt)
{
	mGfxStats->update(dt);

	//==================================================================
	// Handle Input 

	gDInput->poll();

	if( gDInput->keyDown(DIK_W) )	 
		mCameraHeight   += 25.0f * dt;
	if( gDInput->keyDown(DIK_S) )	 
		mCameraHeight   -= 25.0f * dt;

	mCameraRotationY += gDInput->mouseDX() / 100.0f;
	mCameraRadius    += gDInput->mouseDY() / 25.0f;

	// If we rotate over 360 degrees, just roll back to 0
	if( fabsf(mCameraRotationY) >= 2.0f * D3DX_PI ) 
		mCameraRotationY = 0.0f;

	// Don't let radius get too small or large.
	if( mCameraRadius < 5.0f )
		mCameraRadius = 5.0f;
	if( mCameraRadius > 35.0f )
		mCameraRadius = 35.0f;

	// Don't let height get too small or large.
	if( mCameraHeight < 8.0f )
		mCameraHeight = 8.0f;
	if( mCameraHeight > 40.0f )
		mCameraHeight = 40.0f;

	buildViewMtx();

	static float delay = 0.0f;
	if( gDInput->keyDown(DIK_SPACE) && delay <= 0.0f)
	{
		delay = 1.0f;

		mFluid->disturb(0.0f, 0.0f, 0.02f, 2.5f);
		mFluid->disturb(0.6f, 0.0f, 0.02f, 2.5f);
		mFluid->disturb(-0.3f, 0.6f, 0.02f, 2.5f);
		mFluid->disturb(-0.3f, 0.0f, 0.02f, 2.5f);
		mFluid->disturb(0.0f, -0.6f, 0.02f, 2.5f);
	
	}
	delay -= dt;

	
	//==================================================================
	// Update the simulation and rebuild reflect/refract maps. 

	mFluid->update(dt);


	mRefractMap->clear(0xff000000);
	mRefractMap->beginScene();
	drawTerrain();
	mRefractMap->endScene();


	HR(gd3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, 1));
	HR(gd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW));
	mReflectMap->clear(0xffff0000);
	mReflectMap->beginScene();
	drawTerrainReflected();
	drawSkyReflected();
	mReflectMap->endScene();
	HR(gd3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, 0));
	HR(gd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW));
}


void GPUFluidDemo::drawScene()
{
	HR(gd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffdddddd, 1.0f, 0));
	HR(gd3dDevice->BeginScene());

		drawSky();
		drawTerrain();
		mFluid->draw(mFluidWorld, mView, mProj, mCameraPos, 
			mRefractMap->d3dTex(), mReflectMap->d3dTex());
		mGfxStats->display();

	HR(gd3dDevice->EndScene());
	HR(gd3dDevice->Present(0, 0, 0, 0));
}
 
void GPUFluidDemo::buildViewMtx()
{
	float x = mCameraRadius * cosf(mCameraRotationY);
	float z = mCameraRadius * sinf(mCameraRotationY);
	mCameraPos = D3DXVECTOR3(x, mCameraHeight, z);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&mView, &mCameraPos, &target, &up);
}

void GPUFluidDemo::buildProjMtx()
{
	float w = (float)md3dPP.BackBufferWidth;
	float h = (float)md3dPP.BackBufferHeight;
	D3DXMatrixPerspectiveFovLH(&mProj, D3DX_PI * 0.25f, w/h, 1.0f, 3000.0f);
}

void GPUFluidDemo::drawTerrain()
{
	HR(mTerrainFX->SetTechnique(mfxhTerrainTech));
	
	HR(mTerrainFX->SetMatrix(mfxhWorld, &mTerrainWorld));
	HR(mTerrainFX->SetMatrix(mfxhWorldInvTrans, &mTerrainWorldInvTrans));
	HR(mTerrainFX->SetMatrix(mfxhTerrainWVP, &(mTerrainWorld*mView*mProj)));
	HR(mTerrainFX->SetValue(mfxhEyePosW, &mCameraPos, sizeof(D3DXVECTOR3)));
	HR(mTerrainFX->SetTexture(mfxhTexMap, mTerrainTex));

	UINT numPasses = 0;
	HR(mTerrainFX->Begin(&numPasses, 0));
	HR(mTerrainFX->BeginPass(0));
	HR(mTerrain->DrawSubset(0));
	HR(mTerrainFX->EndPass());
	HR(mTerrainFX->End());
}

void GPUFluidDemo::drawSky()
{
	UINT numPasses = 0;
	HR(mSkyFX->SetTechnique(mfxhSkyTech));
	HR(mSkyFX->SetMatrix(mfxhSkyWVP, &(mSkyWorld*mView*mProj)));
	HR(mSkyFX->SetTexture(mfxhSkyEnvMap, mSkyEnvMap));
	HR(mSkyFX->Begin(&numPasses, 0));
	HR(mSkyFX->BeginPass(0));
	HR(mSkyDome->DrawSubset(0));
	HR(mSkyFX->EndPass());
	HR(mSkyFX->End());
}

void GPUFluidDemo::drawTerrainReflected()
{
	D3DXPLANE waterPlaneH;

	D3DXMATRIX WVPInvTrans;
	D3DXMatrixInverse(&WVPInvTrans, 0, &(mTerrainWorld*mView*mProj));
	D3DXMatrixTranspose(&WVPInvTrans, &WVPInvTrans);
	D3DXPlaneTransform(&waterPlaneH, &mWaterPlaneW, &WVPInvTrans);
	HR(gd3dDevice->SetClipPlane(0, (float*)waterPlaneH));
	
	// Add reflection transform.
	D3DXMATRIX R;
	D3DXMatrixReflect(&R, &mWaterPlaneW);
	
	HR(mTerrainFX->SetTechnique(mfxhTerrainTech));
	
	HR(mTerrainFX->SetMatrix(mfxhWorld, &mTerrainWorld));
	HR(mTerrainFX->SetMatrix(mfxhWorldInvTrans, &mTerrainWorldInvTrans));
	HR(mTerrainFX->SetMatrix(mfxhTerrainWVP, &(mTerrainWorld*R*mView*mProj)));
	HR(mTerrainFX->SetValue(mfxhEyePosW, &mCameraPos, sizeof(D3DXVECTOR3)));
	HR(mTerrainFX->SetTexture(mfxhTexMap, mTerrainTex));

	UINT numPasses = 0;
	HR(mTerrainFX->Begin(&numPasses, 0));
	HR(mTerrainFX->BeginPass(0));
	HR(mTerrain->DrawSubset(0));
	HR(mTerrainFX->EndPass());
	HR(mTerrainFX->End());
}

void GPUFluidDemo::drawSkyReflected()
{
	D3DXPLANE waterPlaneH;

	D3DXMATRIX WVPInvTrans;
	D3DXMatrixInverse(&WVPInvTrans, 0, &(mTerrainWorld*mView*mProj));
	D3DXMatrixTranspose(&WVPInvTrans, &WVPInvTrans);
	D3DXPlaneTransform(&waterPlaneH, &mWaterPlaneW, &WVPInvTrans);
	HR(gd3dDevice->SetClipPlane(0, (float*)waterPlaneH));

	// Add reflection transform.
	D3DXMATRIX R;
	D3DXMatrixReflect(&R, &mWaterPlaneW);
	
	HR(mSkyFX->SetTechnique(mfxhSkyReflectTech));

	UINT numPasses = 0;
	HR(mSkyFX->SetTechnique(mfxhSkyTech));
	HR(mSkyFX->SetMatrix(mfxhSkyWVP, &(mSkyWorld*R*mView*mProj)));
	HR(mSkyFX->SetTexture(mfxhSkyEnvMap, mSkyEnvMap));
	HR(mSkyFX->Begin(&numPasses, 0));
	HR(mSkyFX->BeginPass(0));
	HR(mSkyDome->DrawSubset(0));
	HR(mSkyFX->EndPass());
	HR(mSkyFX->End());
}

void GPUFluidDemo::buildEffects()
{
	HRESULT hr = 0;
	ID3DXBuffer* errors = 0;

	// Terrain
	hr = D3DXCreateEffectFromFileEx(gd3dDevice, "terrain.fx", 0, 0, 0, 
		0, 0, &mTerrainFX, &errors);
	if( errors )
	{
		MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);
		HR(hr);
	}

	mfxhTerrainTech        = mTerrainFX->GetTechniqueByName("TerrainTech");
	mfxhWorld              = mTerrainFX->GetParameterByName(0, "gWorld");
	mfxhWorldInvTrans      = mTerrainFX->GetParameterByName(0, "gWorldInvTrans");
	mfxhEyePosW            = mTerrainFX->GetParameterByName(0, "gEyePosW");
	mfxhTerrainWVP         = mTerrainFX->GetParameterByName(0, "gWVP");
	mfxhTexMap             = mTerrainFX->GetParameterByName(0, "gTexMap");

	// Sky
	hr = D3DXCreateEffectFromFileEx(gd3dDevice, "sky.fx", 0, 0, 0, 
		0, 0, &mSkyFX, &errors);
	if( errors )
	{
		MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);
		HR(hr);
	}
	mfxhSkyTech   = mSkyFX->GetTechniqueByName("SkyTech");
	mfxhSkyReflectTech = mSkyFX->GetTechniqueByName("SkyReflectTech");
	mfxhSkyWVP    = mSkyFX->GetParameterByName(0, "gWVP");
	mfxhSkyEnvMap = mSkyFX->GetParameterByName(0, "gSkyMap");
}
