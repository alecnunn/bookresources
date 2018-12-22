//=============================================================================
// FluidPhysics.h by Frank Luna (C) 2005 All Rights Reserved.
//=============================================================================

#ifndef FLUID_PHYSICS_H
#define FLUID_PHYSICS_H

#include <d3dx9.h>
#include "DrawableTex2D.h"

class FluidPhysics
{
public:
	FluidPhysics(int m, float dx, float dt, float speed, float damping);
	~FluidPhysics();

	void onLostDevice();
	void onResetDevice();

	IDirect3DTexture9* getHeightMap();

	void disturb(float x, float y, float radius, float height); // Specified on projection window [-1, 1].
	void update(float dt);

private:
	// This class is not designed to be copied.
	FluidPhysics(const FluidPhysics& rhs);
	FluidPhysics& operator=(const FluidPhysics& rhs);

	void buildQuadVB();
	void buildDisturbanceIB();
	void buildGridVB();
	void buildGridIB();
	void buildPhysicsFX();

	void copy16To32();

private:
	int   mNumVertRows;
	int   mNumVertCols;
	float mSpatialStep;
	float mTimeStep;
	float mK1;
	float mK2;
	float mK3;
	float mTexelSize;

	DrawableTex2D* mPrevStepMap; // D3DFMT_A16B16G16R16F
	DrawableTex2D* mCurrStepMap; // D3DFMT_A16B16G16R16F
	DrawableTex2D* mNextStepMap; // D3DFMT_A16B16G16R16F
	DrawableTex2D* mTangentMap;  // D3DFMT_G16R16F

	DrawableTex2D* mVertexDataMap; // D3DFMT_A32B32G32R32F

	IDirect3DVertexBuffer9* mQuadVB;
	IDirect3DVertexBuffer9* mDisturbanceVB;
	IDirect3DIndexBuffer9*  mDisturbanceIB;
	
	ID3DXEffect* mPhysicsFX;
	D3DXHANDLE mfxhCopyTexTech;
	D3DXHANDLE mfxhDisturbTech;
	D3DXHANDLE mfxhPhysicsTech;
	D3DXHANDLE mfxhCalcTangentsTech;

	D3DXHANDLE mfxhHeightMap;
	D3DXHANDLE mfxhTangentMap;
	D3DXHANDLE mfxhStepSizeL;
	D3DXHANDLE mfxhTexelSize;
	D3DXHANDLE mfxhPrevStepMap;
	D3DXHANDLE mfxhCurrStepMap;
	D3DXHANDLE mfxhK1;
	D3DXHANDLE mfxhK2;
	D3DXHANDLE mfxhK3;
};

#endif // FLUID_PHYSICS_H