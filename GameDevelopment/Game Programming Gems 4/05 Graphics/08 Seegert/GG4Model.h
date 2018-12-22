#ifndef __GG4MODEL_H_
#define __GG4MODEL_H_


/* includes */
#include <d3d9.h>
#include <d3dx9.h>


/* type definitions */
typedef unsigned long	uint32;
typedef unsigned short	uint16;
typedef unsigned char	uint8;
typedef long			int32;
typedef short			int16;
typedef char			int8;
typedef unsigned char	memory;


/* enums */
enum GG4TeamColorTechnique
{
	kgg4tctPolygonTinting				= 0,
	kgg4tctTextureMaskingMultiPass		= 1,
	kgg4tctTextureMaskingMultiTexture	= 2,
	kgg4tctVertexAndPixelShader			= 3,
	kgg4tctMax							= 4
};

enum GG4TeamColorValidationState
{
	kgg4tcvsUnknown		= 0,
	kgg4tcvsValid		= 1,
	kgg4tcvsInvalid		= 2
};

enum GG4VertexShaderConstants
{
	kgg4vscZero					= 0,
	kgg4vscOne					= 1,
	kgg4vscEyePosition			= 2,
	kgg4vscTransformWorld		= 3,
	kgg4vscTransformProjection	= 7,
	kgg4vscTransformView		= 11,
	kgg4vscLight1Direction		= 15,
	kgg4vscLight1Color			= 16,
	kgg4vscLight2Direction		= 17,
	kgg4vscLight2Color			= 18,
	kgg4vscLightAmbientColor	= 19,
	kgg4vscLightTeamColorMajor	= 20,
	kgg4vscLightTeamColorMinor	= 21
};

enum GG4PixelShaderConstants
{
	kgg4pscZero					= 0,
	kgg4pscOne					= 1,
	kgg4pscLightTeamColorMajor	= 2,
	kgg4pscLightTeamColorMinor	= 3
};


/* the vertex structure for loading */
struct MODELLOADDATA
{
    float	mPositionX;
	float	mPositionY;
	float	mPositionZ;
    float	mNormalX;
	float	mNormalY;
	float	mNormalZ;
	uint32	mColor;
	float	mU;
	float	mV;
};

/* the vertex structure for rendering */
struct D3DVERTEX_GG4MODEL
{
    float	mPositionX;
	float	mPositionY;
	float	mPositionZ;
    float	mNormalX;
	float	mNormalY;
	float	mNormalZ;
	uint32	mColor;
	float	mU0;
	float	mV0;
	float	mU1;
	float	mV1;
};

/* the FVF format */
#define D3DVERTEXFORMAT_GG4MODEL		(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX2)


/*
 * GG4ModelMaterial
 *
 * Defines "material" data for a model -
 * basically a subset of a model used
 * for splitting up rendering.
 */
class GG4ModelMaterial
{
public:
	uint32	mPolygonRangeLength;
	uint32	mPolygonStartingIndex;
	uint32	mVertexRangeLength;
	uint32	mVertexStartingIndex;
};


/*
 * GG4Model
 *
 * Defines a model, which is simply the
 * data needed to draw it, and some
 * knowledge of how it should be drawn.
 */
class GG4Model
{
public:
									 GG4Model(void);
	virtual							~GG4Model(void);

	inline GG4TeamColorTechnique	GetTeamColorTechnique(void) const
										{ return this->mTeamColorTechnique; }
								
	void							Initialize(void);
									
	void							Load(LPDIRECT3DDEVICE9 inDevice, const char* inModelName, const char* inTexture1Name, const char* inTexture2Name);
	void							LoadShaders(LPDIRECT3DDEVICE9 inDevice, const char* inVertexShader, const char* inPixelShader);
									
	void							Release(void);
									
	bool							Render(LPDIRECT3DDEVICE9 inDevice);
									
	inline void						SetTeamColorTechnique(GG4TeamColorTechnique inTeamColorTechnique)
										{ this->mTeamColorTechnique = inTeamColorTechnique; }
	inline void						SetTeamColorMajor(float inRed, float inGreen, float inBlue)
										{ this->mTeamColorMajor.r = inRed;
										  this->mTeamColorMajor.g = inGreen;
										  this->mTeamColorMajor.b = inBlue;
										  this->mTeamColorMajor.a = 1.0F; }
	inline void						SetTeamColorMinor(float inRed, float inGreen, float inBlue)
										{ this->mTeamColorMinor.r = inRed;
										  this->mTeamColorMinor.g = inGreen;
										  this->mTeamColorMinor.b = inBlue;
										  this->mTeamColorMinor.a = 1.0F; }
	inline void						SetTransform(const D3DXMATRIX& inTransform)
										{ ::D3DXMatrixTranspose(&this->mTransform, &inTransform); }
	
protected:

	bool							RenderWithPolyTinting(LPDIRECT3DDEVICE9 inDevice);
	bool							RenderWithTextureMaskMultiTexture(LPDIRECT3DDEVICE9 inDevice);
	bool							RenderWithTextureMaskMultiPass(LPDIRECT3DDEVICE9 inDevice);
	bool							RenderWithVertexAndPixelShaders(LPDIRECT3DDEVICE9 inDevice);

	bool							ValidateTechnique(LPDIRECT3DDEVICE9 inDevice);

	GG4TeamColorValidationState		mValidatedTechniques[kgg4tctMax];
	D3DXMATRIX						mTransform;
	D3DCOLORVALUE					mTeamColorMajor;
	D3DCOLORVALUE					mTeamColorMinor;
	LPDIRECT3DINDEXBUFFER9			mIndexBuffer;
	LPDIRECT3DPIXELSHADER9			mPixelShader;
	LPDIRECT3DTEXTURE9				mTexture1;
	LPDIRECT3DTEXTURE9				mTexture2;
	LPDIRECT3DVERTEXBUFFER9			mVertexBuffer;
	LPDIRECT3DVERTEXDECLARATION9	mVertexDeclaration;
	LPDIRECT3DVERTEXSHADER9			mVertexShader;
	GG4ModelMaterial**				mMaterials;
	GG4TeamColorTechnique			mTeamColorTechnique;
	uint32							mMaterialCount;
	uint32							mPolygonCount;
	uint32							mVertexCount;
};


#endif	/* __GG4MODEL_H_ */