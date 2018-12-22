//=============================================================================
// Fluid.cpp by Frank Luna (C) 2005 All Rights Reserved.
//=============================================================================

#include "Fluid.h"
#include "Vertex.h"


Fluid::Fluid(int m, int n, float dx, float dt, float speed, float damping)
{
	mRows = m;
	mCols = n;
	mDX   = dx;

	mPhysics = new FluidPhysics(m, dx, dt, speed, damping);

	buildGridMesh();
	buildRenderingFX();

	HR(D3DXCreateTextureFromFile(gd3dDevice, "wave1.dds", &mNormMap0));
	HR(D3DXCreateTextureFromFile(gd3dDevice, "wave0.dds", &mNormMap1));

	// Texture offsets for scrolling normal maps.  Coords x, y control
	// the first normal map, and coords z, w control the second normal map.
	mTexOffsets = D3DXVECTOR4(0.0f, 0.0f, 0.2f, 0.3f);
}

Fluid::~Fluid()
{
	delete mPhysics;
	ReleaseCOM(mGridMesh);
	ReleaseCOM(mRenderingFX);
	ReleaseCOM(mNormMap0);
	ReleaseCOM(mNormMap1);
}

int Fluid::numRows()const
{
	return mRows;
}

int Fluid::numCols()const
{
	return mCols;
}

int Fluid::numVertices()const
{
	return mGridMesh->GetNumVertices();
}

int Fluid::numTriangles()const
{
	return mGridMesh->GetNumFaces();
}

float Fluid::width()const
{
	return mCols*mDX;
}

float Fluid::depth()const
{
	return mRows*mDX;
}

void Fluid::onLostDevice()
{
	mPhysics->onLostDevice();
	HR(mRenderingFX->OnLostDevice());
}

void Fluid::onResetDevice()
{
	mPhysics->onResetDevice();
	HR(mRenderingFX->OnResetDevice());
}

void Fluid::disturb(float x, float y, float radius, float height)
{
	mPhysics->disturb(x, y, radius, height);
}

void Fluid::update(float dt)
{
	mPhysics->update(dt);

	// Scroll normal maps.
	mTexOffsets += (D3DXVECTOR4(0.02f, 0.06f, -0.01f, 0.03f)*dt);

	if(mTexOffsets.x >= 1.0f) mTexOffsets.x -= 1.0f;
	if(mTexOffsets.y >= 1.0f) mTexOffsets.y -= 1.0f;
	if(mTexOffsets.z >= 1.0f) mTexOffsets.z -= 1.0f;
	if(mTexOffsets.w >= 1.0f) mTexOffsets.w -= 1.0f;

	if(mTexOffsets.x <= -1.0f) mTexOffsets.x += 1.0f;
	if(mTexOffsets.y <= -1.0f) mTexOffsets.y += 1.0f;
	if(mTexOffsets.z <= -1.0f) mTexOffsets.z += 1.0f;
	if(mTexOffsets.w <= -1.0f) mTexOffsets.w += 1.0f;
}

void Fluid::draw(const D3DXMATRIX& W, const D3DXMATRIX& V, const D3DXMATRIX& P, const D3DXVECTOR3& eyePosW,
				 IDirect3DTexture9* refractMap, IDirect3DTexture9* reflectMap)
{
	D3DXMATRIX fluidWorldInv;
	D3DXMatrixInverse(&fluidWorldInv, 0, &W);

	HR(mRenderingFX->SetTechnique(mfxhFluidRenderingTech));
	HR(mRenderingFX->SetMatrix(mfxhWorld, &W));
	HR(mRenderingFX->SetMatrix(mfxhWorldInv, &fluidWorldInv));
	HR(mRenderingFX->SetMatrix(mfxhWVP, &(W*V*P)));
	HR(mRenderingFX->SetValue(mfxhEyePosW, &eyePosW, sizeof(D3DXVECTOR3)));
	HR(mRenderingFX->SetTexture(mfxhDispMap, mPhysics->getHeightMap()));
	HR(mRenderingFX->SetTexture(mfxhRefractMap, refractMap));
	HR(mRenderingFX->SetTexture(mfxhReflectMap, reflectMap));
	HR(mRenderingFX->SetTexture(mfxhNormMap0, mNormMap0));
	HR(mRenderingFX->SetTexture(mfxhNormMap1, mNormMap1));
	HR(mRenderingFX->SetVector(mfxhTexOffsets, &mTexOffsets));

	UINT numPasses = 0;
	HR(mRenderingFX->Begin(&numPasses, 0));
	HR(mRenderingFX->BeginPass(0));	
	HR(mGridMesh->DrawSubset(0));
	HR(mRenderingFX->EndPass());
	HR(mRenderingFX->End());
}

void Fluid::buildGridMesh()
{
	std::vector<D3DXVECTOR3> verts;
	std::vector<DWORD> indices;
	GenTriGrid(mRows, mCols, mDX, mDX, D3DXVECTOR3(0.0f, 0.0f, 0.0f), verts, indices);

	int numVerts = mRows*mCols;
	int numTris  = (mRows-1)*(mCols-1)*2;

	D3DVERTEXELEMENT9 elems[MAX_FVF_DECL_SIZE];
	UINT numElems = 0;
	HR(VertexPT::Decl->GetDeclaration(elems, &numElems));
	HR(D3DXCreateMesh(numTris, numVerts, 
		D3DXMESH_MANAGED|D3DXMESH_32BIT, elems, gd3dDevice, &mGridMesh));

	VertexPT* v = 0;
	HR(mGridMesh->LockVertexBuffer(0,(void**)&v));

	// grid width/depth
	float w = (mRows-1) * mDX; 
	float d = (mCols-1) * mDX;
	for(int i = 0; i < mRows; ++i)
	{
		for(int j = 0; j < mCols; ++j)
		{
			DWORD index = i * mCols + j;
			v[index].pos    = verts[index];
			v[index].tex0.x = (v[index].pos.x + (0.5f*w)) / w;
			v[index].tex0.y = (v[index].pos.z - (0.5f*d)) / -d;
		}
	}

	HR(mGridMesh->UnlockVertexBuffer());


	DWORD* k = 0;
	HR(mGridMesh->LockIndexBuffer(0, (void**)&k));
	DWORD* attBuffer = 0;
	HR(mGridMesh->LockAttributeBuffer(0, &attBuffer));

	for(int i = 0; i < numTris; ++i)
	{
		k[i*3+0] = (DWORD)indices[i*3+0];
		k[i*3+1] = (DWORD)indices[i*3+1];
		k[i*3+2] = (DWORD)indices[i*3+2];

		attBuffer[i] = 0; // Always subset 0
	}

	HR(mGridMesh->UnlockIndexBuffer());
	HR(mGridMesh->UnlockAttributeBuffer());


	DWORD* adj = new DWORD[mGridMesh->GetNumFaces()*3];
	HR(mGridMesh->GenerateAdjacency(EPSILON, adj));
	HR(mGridMesh->OptimizeInplace(D3DXMESHOPT_VERTEXCACHE|D3DXMESHOPT_ATTRSORT,
		adj, 0, 0, 0));
	delete[] adj;
}

void Fluid::buildRenderingFX()
{
	HRESULT hr = 0;
	ID3DXBuffer* errors = 0;
	hr = D3DXCreateEffectFromFileEx(gd3dDevice, "fluidRendering.fx", 0, 0, 0, 
		D3DXSHADER_DEBUG, 0, &mRenderingFX, &errors);
	if( errors )
	{
		MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);
		HR(hr);
	}

	mfxhFluidRenderingTech = mRenderingFX->GetTechniqueByName("GPUFluidRenderingTech");
	mfxhWorld    = mRenderingFX->GetParameterByName(0, "gWorld");
	mfxhWorldInv = mRenderingFX->GetParameterByName(0, "gWorldInv");
	mfxhEyePosW  = mRenderingFX->GetParameterByName(0, "gEyePosW");
	mfxhWVP      = mRenderingFX->GetParameterByName(0, "gWVP");
	mfxhDispMap  = mRenderingFX->GetParameterByName(0, "gDispMap");
	mfxhRefractMap = mRenderingFX->GetParameterByName(0, "gRefractMap");
	mfxhReflectMap = mRenderingFX->GetParameterByName(0, "gReflectMap");
	mfxhNormMap0   = mRenderingFX->GetParameterByName(0, "gNormMap0");
	mfxhNormMap1   = mRenderingFX->GetParameterByName(0, "gNormMap1");
	mfxhTexOffsets = mRenderingFX->GetParameterByName(0, "gTexOffsets");
}