//=============================================================================
// Fluid.h by Frank Luna (C) 2005 All Rights Reserved.
//=============================================================================

#ifndef FLUID_H
#define FLUID_H

#include "d3dUtil.h"
#include "DrawableTex2D.h"
#include "FluidPhysics.h"

class Fluid
{
public:
	// Dimensions should be powers of 2.
	Fluid(int m, int n, float dx, float dt, float speed, float damping);
	~Fluid();

	int numRows()const;
	int numCols()const;

	int numVertices()const;
	int numTriangles()const;

	float width()const;
	float depth()const;

	void onLostDevice();
	void onResetDevice();

	void disturb(float x, float y, float radius, float height); // Specified on projection window [-1, 1].
	void update(float dt);

	void draw(const D3DXMATRIX& W, const D3DXMATRIX& V, const D3DXMATRIX& P, const D3DXVECTOR3& eyePosW,
		IDirect3DTexture9* refractMap, IDirect3DTexture9* reflectMap);

private:
	// This class is not designed to be copied.
	Fluid(const Fluid& rhs);
	Fluid& operator=(const Fluid& rhs);

	void buildGridMesh();
	void buildRenderingFX();
	
private:
	FluidPhysics* mPhysics;
	ID3DXMesh* mGridMesh;
	int mRows;
	int mCols;
	float mDX;

	ID3DXEffect* mRenderingFX;
	D3DXHANDLE mfxhFluidRenderingTech;
	D3DXHANDLE mfxhWorld;
	D3DXHANDLE mfxhWorldInv;
	D3DXHANDLE mfxhWVP;
	D3DXHANDLE mfxhEyePosW;
	D3DXHANDLE mfxhDispMap;
	D3DXHANDLE mfxhRefractMap;
	D3DXHANDLE mfxhReflectMap;
	D3DXHANDLE mfxhNormMap0;
	D3DXHANDLE mfxhNormMap1;
	D3DXHANDLE mfxhTexOffsets;

	D3DXVECTOR4 mTexOffsets; 
	IDirect3DTexture9* mNormMap0;
	IDirect3DTexture9* mNormMap1;
};

#endif // FLUID_H