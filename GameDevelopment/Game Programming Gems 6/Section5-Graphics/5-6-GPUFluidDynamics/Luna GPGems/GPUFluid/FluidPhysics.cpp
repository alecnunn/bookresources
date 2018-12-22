//=============================================================================
// FluidPhysics.cpp by Frank Luna (C) 2005 All Rights Reserved.
//=============================================================================

#include "FluidPhysics.h"

struct FluidVertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR2 uv;
};

FluidPhysics::FluidPhysics(int m, float dx, float dt, float speed, float damping)
{
	mNumVertRows = m;
	mNumVertCols = m;
	mSpatialStep = dx;
	mTimeStep    = dt;

	// Compute finite difference scheme constants.
	float d      = damping*dt+2.0f;
	float e      = (speed*speed)*(dt*dt)/(dx*dx);
	mK1          = (damping*dt-2.0f)/ d;
	mK2          = (4.0f-8.0f*e) / d;
	mK3          = (2.0f*e) / d;
	mTexelSize   = 1.0f / (float)m;

	D3DVIEWPORT9 vp = {0, 0, m, m, 0.0f, 1.0f};
	mPrevStepMap = new DrawableTex2D(m, m, 1, D3DFMT_A16B16G16R16F, false, D3DFMT_UNKNOWN, vp);
	mCurrStepMap = new DrawableTex2D(m, m, 1, D3DFMT_A16B16G16R16F, false, D3DFMT_UNKNOWN, vp);
	mNextStepMap = new DrawableTex2D(m, m, 1, D3DFMT_A16B16G16R16F, false, D3DFMT_UNKNOWN, vp);
	mTangentMap  = new DrawableTex2D(m, m, 1, D3DFMT_G16R16F, false, D3DFMT_UNKNOWN, vp);
	mVertexDataMap = new DrawableTex2D(m, m, 1, D3DFMT_A32B32G32R32F, false, D3DFMT_UNKNOWN, vp);

	buildQuadVB();
	buildDisturbanceIB();
	buildPhysicsFX();
}

FluidPhysics::~FluidPhysics()
{
	delete mPrevStepMap;
	delete mCurrStepMap;
	delete mNextStepMap;
	delete mTangentMap;
	delete mVertexDataMap;

	ReleaseCOM(mPhysicsFX);
	ReleaseCOM(mQuadVB);
	ReleaseCOM(mDisturbanceVB);
	ReleaseCOM(mDisturbanceIB);
}

void FluidPhysics::onLostDevice()
{
	mPrevStepMap->onLostDevice();
	mCurrStepMap->onLostDevice();
	mNextStepMap->onLostDevice();
	mTangentMap->onLostDevice();
	mVertexDataMap->onLostDevice();
	HR(mPhysicsFX->OnLostDevice());

	ReleaseCOM(mDisturbanceVB);
}

void FluidPhysics::onResetDevice()
{
	mPrevStepMap->onResetDevice();
	mCurrStepMap->onResetDevice();
	mNextStepMap->onResetDevice();
	mTangentMap->onResetDevice();
	mVertexDataMap->onResetDevice();
	HR(mPhysicsFX->OnResetDevice());

	mPrevStepMap->clear(0x00000000);
	mCurrStepMap->clear(0x00000000);
	mNextStepMap->clear(0x00000000);
	mTangentMap->clear(0x00000000);
	mVertexDataMap->clear(0x00000000);

	HR(gd3dDevice->CreateVertexBuffer(9*sizeof(D3DXVECTOR3), D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY, 
		D3DFVF_XYZ, D3DPOOL_DEFAULT, &mDisturbanceVB, 0));
}

IDirect3DTexture9* FluidPhysics::getHeightMap()
{
	return mVertexDataMap->d3dTex();
}

void FluidPhysics::disturb(float x, float y, float radius, float height)
{
	// We disturb the point (x, y) [center] and the points around it.
	// 0*-1*-2*
	//  |  |  |
	// 3*-4*-5*
	//  |  |  |
	// 6*-7*-8*

	D3DXVECTOR3* v = 0;
	HR(mDisturbanceVB->Lock(0, 0, (void**)&v, D3DLOCK_DISCARD));
	v[0] = D3DXVECTOR3(x-radius, y+radius, 0.1f*height);
	v[1] = D3DXVECTOR3(x, y+radius, 0.4f*height);
	v[2] = D3DXVECTOR3(x+radius, y+radius, 0.1f*height);
	v[3] = D3DXVECTOR3(x-radius, y, 0.4f*height);
	v[4] = D3DXVECTOR3(x, y, height);
	v[5] = D3DXVECTOR3(x+radius, y, 0.4f*height);
	v[6] = D3DXVECTOR3(x-radius, y-radius, 0.1f*height);
	v[7] = D3DXVECTOR3(x, y-radius, 0.4f*height);
	v[8] = D3DXVECTOR3(x+radius, y-radius, 0.1f*height);
	HR(mDisturbanceVB->Unlock());
	
	HR(mPhysicsFX->SetTechnique(mfxhDisturbTech));
	mPrevStepMap->beginScene();
	UINT numPasses = 0;
	HR(mPhysicsFX->Begin(&numPasses, 0));
	for(UINT i = 0; i < numPasses; ++i)
	{
		HR(mPhysicsFX->BeginPass(i));
		HR(gd3dDevice->SetStreamSource(0, mDisturbanceVB, 0, sizeof(D3DXVECTOR3)));
		HR(gd3dDevice->SetFVF(D3DFVF_XYZ));
		HR(gd3dDevice->SetIndices(mDisturbanceIB));
		HR(gd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 9, 0, 8));
		HR(mPhysicsFX->EndPass());
	}
	HR(mPhysicsFX->End());
	mPrevStepMap->endScene();
	 
	HR(mPhysicsFX->SetTechnique(mfxhDisturbTech));
	mCurrStepMap->beginScene();
	HR(mPhysicsFX->Begin(&numPasses, 0));
	for(UINT i = 0; i < numPasses; ++i)
	{
		HR(mPhysicsFX->BeginPass(i));
		HR(gd3dDevice->SetStreamSource(0, mDisturbanceVB, 0, sizeof(D3DXVECTOR3)));
		HR(gd3dDevice->SetFVF(D3DFVF_XYZ));
		HR(gd3dDevice->SetIndices(mDisturbanceIB));
		HR(gd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 9, 0, 8));
		HR(mPhysicsFX->EndPass());
	}
	HR(mPhysicsFX->End());
	mCurrStepMap->endScene();
}

void FluidPhysics::update(float dt)
{
	static float timeAccum = 0.0f;
	timeAccum += dt;
	if( timeAccum >= mTimeStep )
	{
		HR(mPhysicsFX->SetTechnique(mfxhPhysicsTech));
		HR(mPhysicsFX->SetFloat(mfxhStepSizeL, mSpatialStep));
		HR(mPhysicsFX->SetFloat(mfxhTexelSize, mTexelSize));
		HR(mPhysicsFX->SetFloat(mfxhK1, mK1));
		HR(mPhysicsFX->SetFloat(mfxhK2, mK2));
		HR(mPhysicsFX->SetFloat(mfxhK3, mK3));
		HR(mPhysicsFX->SetTexture(mfxhPrevStepMap, mPrevStepMap->d3dTex()));
		HR(mPhysicsFX->SetTexture(mfxhCurrStepMap, mCurrStepMap->d3dTex()));
		UINT numPasses = 0;
		mNextStepMap->beginScene();
		HR(mPhysicsFX->Begin(&numPasses, 0));
		for(UINT i = 0; i < numPasses; ++i)
		{
			HR(mPhysicsFX->BeginPass(i));
			HR(gd3dDevice->SetStreamSource(0, mQuadVB, 0, sizeof(D3DXVECTOR3)));
			HR(gd3dDevice->SetFVF(D3DFVF_XYZ));
			HR(gd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2));
			HR(mPhysicsFX->EndPass());
		}
		HR(mPhysicsFX->End());
		mNextStepMap->endScene();

		// Cycle the render targets for next iteration.
		DrawableTex2D* temp = mPrevStepMap;
		mPrevStepMap = mCurrStepMap;
		mCurrStepMap = mNextStepMap;
		mNextStepMap = temp;
	
		timeAccum = 0.0f;

		// Now generate tangent info.
		HR(mPhysicsFX->SetTechnique(mfxhCalcTangentsTech));
		HR(mPhysicsFX->SetTexture(mfxhCurrStepMap, mCurrStepMap->d3dTex()));
		mTangentMap->beginScene();
		HR(mPhysicsFX->Begin(&numPasses, 0));
		for(UINT i = 0; i < numPasses; ++i)
		{
			HR(mPhysicsFX->BeginPass(i));
			HR(gd3dDevice->SetStreamSource(0, mQuadVB, 0, sizeof(D3DXVECTOR3)));
			HR(gd3dDevice->SetFVF(D3DFVF_XYZ));
			HR(gd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2));
			HR(mPhysicsFX->EndPass());
		}
		HR(mPhysicsFX->End());
		mTangentMap->endScene();

		copy16To32();
	}
}

void FluidPhysics::buildQuadVB()
{
	HR(gd3dDevice->CreateVertexBuffer(6*sizeof(D3DXVECTOR3), D3DUSAGE_WRITEONLY, 
		D3DFVF_XYZ, D3DPOOL_MANAGED, &mQuadVB, 0));
	
	D3DXVECTOR3* v = 0;
	mQuadVB->Lock(0, 0, (void**)&v, 0);
	v[0] = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);
	v[1] = D3DXVECTOR3(-1.0f,  1.0f, 0.0f);
	v[2] = D3DXVECTOR3( 1.0f,  1.0f, 0.0f);
	v[3] = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);
	v[4] = D3DXVECTOR3( 1.0f,  1.0f, 0.0f);
	v[5] = D3DXVECTOR3( 1.0f, -1.0f, 0.0f);
	mQuadVB->Unlock();
}

void FluidPhysics::buildDisturbanceIB()
{
	HR(gd3dDevice->CreateIndexBuffer(sizeof(WORD)*24,
		D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &mDisturbanceIB, 0));

	// We disturb the ijth texel and its eight neighbors.
	// 0*-1*-2*
	//  |  |  |
	// 3*-4*-5*
	//  |  |  |
	// 6*-7*-8*

	WORD* indices = 0;
	mDisturbanceIB->Lock(0, 0, (void**)&indices, 0);
	indices[0]  = 0;  indices[1]  = 1;  indices[2]  = 3;
	indices[3]  = 1;  indices[4]  = 4;  indices[5]  = 3;
	indices[6]  = 1;  indices[7]  = 2;  indices[8]  = 4;
	indices[9]  = 2;  indices[10] = 5;  indices[11] = 4;
	indices[12] = 3;  indices[13] = 4;  indices[14] = 6;
	indices[15] = 4;  indices[16] = 7;  indices[17] = 6;
	indices[18] = 4;  indices[19] = 5;  indices[20] = 7;
	indices[21] = 5;  indices[22] = 8;  indices[23] = 7;
	mDisturbanceIB->Unlock();
}

void FluidPhysics::buildPhysicsFX()
{
	HRESULT hr = 0;
	ID3DXBuffer* errors = 0;
	hr = D3DXCreateEffectFromFileEx(gd3dDevice, "fluidphysics.fx", 0, 0, 0, 
		D3DXSHADER_DEBUG, 0, &mPhysicsFX, &errors);
	if( errors )
	{
		MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);
		HR(hr);
	}

	mfxhCopyTexTech      = mPhysicsFX->GetTechniqueByName("CopyTexTech");
	mfxhDisturbTech      = mPhysicsFX->GetTechniqueByName("DisturbTech");
	mfxhPhysicsTech      = mPhysicsFX->GetTechniqueByName("PhysicsTech");
	mfxhCalcTangentsTech = mPhysicsFX->GetTechniqueByName("CalcTangentsTech");
	mfxhHeightMap        = mPhysicsFX->GetParameterByName(0, "gHeightMap");
	mfxhTangentMap       = mPhysicsFX->GetParameterByName(0, "gTangentMap");
	mfxhStepSizeL        = mPhysicsFX->GetParameterByName(0, "gStepSizeL");
	mfxhTexelSize        = mPhysicsFX->GetParameterByName(0, "gTexelSize");
	mfxhPrevStepMap      = mPhysicsFX->GetParameterByName(0, "gPrevStepTex");
	mfxhCurrStepMap      = mPhysicsFX->GetParameterByName(0, "gCurrStepTex");
	mfxhK1               = mPhysicsFX->GetParameterByName(0, "gK1");
	mfxhK2               = mPhysicsFX->GetParameterByName(0, "gK2");
	mfxhK3               = mPhysicsFX->GetParameterByName(0, "gK3");
}

void FluidPhysics::copy16To32()
{
	HR(mPhysicsFX->SetTechnique(mfxhCopyTexTech));
	HR(mPhysicsFX->SetTexture(mfxhHeightMap, mCurrStepMap->d3dTex()));
	HR(mPhysicsFX->SetTexture(mfxhTangentMap, mTangentMap->d3dTex()));
	mVertexDataMap->beginScene();
	UINT numPasses = 0;
	HR(mPhysicsFX->Begin(&numPasses, 0));
	for(UINT i = 0; i < numPasses; ++i)
	{
		HR(mPhysicsFX->BeginPass(i));
		HR(gd3dDevice->SetStreamSource(0, mQuadVB, 0, sizeof(D3DXVECTOR3)));
		HR(gd3dDevice->SetFVF(D3DFVF_XYZ));
		HR(gd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2));
		HR(mPhysicsFX->EndPass());
	}
	HR(mPhysicsFX->End());
	mVertexDataMap->endScene();
}