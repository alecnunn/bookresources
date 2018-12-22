#include "GG4Model.h"
#include "assert.h"


/*
 * constructor
 *
 */
GG4Model::GG4Model(void)
{ 
	/* init */
	this->Initialize();
};


/*
 * destructor
 *
 */
GG4Model::~GG4Model(void)
{ 
	/* clean up */
	this->Release();	
};


/*
 * Initialize
 *
 */
void
GG4Model::Initialize(void)
{
	uint32 theLoop;

	/* clear values */
	this->mMaterialCount		= 0;
	this->mPolygonCount			= 0;
	this->mVertexCount			= 0;
	this->mIndexBuffer			= NULL;
	this->mMaterials			= NULL;
	this->mPixelShader			= NULL;
	this->mTexture1				= NULL;
	this->mTexture2				= NULL;
	this->mVertexBuffer			= NULL;
	this->mVertexDeclaration	= NULL;
	this->mVertexShader			= NULL;

	/* set team color */
	this->SetTeamColorMajor(1.0F, 0.1F, 0.0F);
	this->SetTeamColorMinor(0.0F, 0.0F, 1.0F);

	/* set team technique */
	this->SetTeamColorTechnique(kgg4tctPolygonTinting);

	/* clear technique validation flags */
	for (theLoop = 0; theLoop < kgg4tctMax; theLoop++)
		this->mValidatedTechniques[theLoop] = kgg4tcvsUnknown;
}


/*
 * Load
 *
 * Loads a model and texture from disk.
 */
void
GG4Model::Load(LPDIRECT3DDEVICE9 inDevice, const char* inModelName, const char* inTexture1Name, const char* inTexture2Name)
{
	HANDLE				theFileHandle;
	HRESULT				theResult;
	MODELLOADDATA*		theLoadData;
	MODELLOADDATA*		theTempLoadData;
	D3DVERTEX_GG4MODEL*	theVertexData;
	D3DVERTEX_GG4MODEL*	theTempVertexData;
	uint16*				theIndexData;
	uint32				theAmountRead;
	uint32				theLoop;

	/* make sure we've released and reinitialized, making subsequent loads safe */
	this->Release();
	this->Initialize();

	/* define ugly macro here */
	#define GG4MODELREAD(s)		::ReadFile(theFileHandle, (memory*)(&s), sizeof(s), &theAmountRead, NULL)
	
	/* open the file */
	theFileHandle = ::CreateFile(inModelName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	
	/* open ok? */
	if (theFileHandle != INVALID_HANDLE_VALUE)
	{
		/* read the counts */
		GG4MODELREAD(this->mMaterialCount);
		GG4MODELREAD(this->mPolygonCount);
		GG4MODELREAD(this->mVertexCount);
		
		/* create the material array */
		this->mMaterials = new GG4ModelMaterial*[this->mMaterialCount];
		
		/* load the materials */
		for (theLoop = 0; theLoop < this->mMaterialCount; theLoop++)
		{
			/* create a new material */
			this->mMaterials[theLoop] = new GG4ModelMaterial;
			
			/* read the counts */
			GG4MODELREAD(this->mMaterials[theLoop]->mPolygonRangeLength);
			GG4MODELREAD(this->mMaterials[theLoop]->mPolygonStartingIndex);
			GG4MODELREAD(this->mMaterials[theLoop]->mVertexRangeLength);
			GG4MODELREAD(this->mMaterials[theLoop]->mVertexStartingIndex);
		}
		
		/* create the vb */
		theResult = inDevice->CreateVertexBuffer(this->mVertexCount * sizeof(D3DVERTEX_GG4MODEL), 0, 
			D3DVERTEXFORMAT_GG4MODEL, D3DPOOL_MANAGED, 
			&this->mVertexBuffer, NULL);
		
		if (SUCCEEDED(theResult))
		{
			/* lock it */
			theResult = this->mVertexBuffer->Lock(0, 0, (VOID**)&theVertexData, 0);
			
			if (SUCCEEDED(theResult))
			{
				/* create a temporary store to load the data in.  we need to do this 
				 * rather than load it right into the vertex data since the data was 
				 * saved in a different format =( */
				theLoadData = new MODELLOADDATA[this->mVertexCount];

				/* read the data into the temp store */
				::ReadFile(theFileHandle, (memory*)theLoadData, this->mVertexCount * sizeof(MODELLOADDATA), &theAmountRead, NULL);

				/* init ptrs */
				theTempLoadData		= theLoadData;
				theTempVertexData	= theVertexData;

				/* now set it in the real vb */
				for (theLoop = 0; theLoop < this->mVertexCount; theLoop++)
				{
					theTempVertexData->mPositionX	= theTempLoadData->mPositionX;
					theTempVertexData->mPositionY	= theTempLoadData->mPositionY;
					theTempVertexData->mPositionZ	= theTempLoadData->mPositionZ;
					theTempVertexData->mNormalX		= theTempLoadData->mNormalX;
					theTempVertexData->mNormalY		= theTempLoadData->mNormalY;
					theTempVertexData->mNormalZ		= theTempLoadData->mNormalZ;
					theTempVertexData->mColor		= theTempLoadData->mColor;
					theTempVertexData->mU0			= theTempLoadData->mU;
					theTempVertexData->mV0			= theTempLoadData->mV;
					theTempVertexData->mU1			= theTempLoadData->mU;
					theTempVertexData->mV1			= theTempLoadData->mV;

					theTempLoadData++;
					theTempVertexData++;
				}
				
				/* unlock the vb */
				theResult = this->mVertexBuffer->Unlock();

				/* delete temp store */
				delete[] theLoadData;
				
				if (SUCCEEDED(theResult))
				{
					/* create the ib */
					theResult = inDevice->CreateIndexBuffer(this->mPolygonCount * sizeof(uint16) * 3,
						D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, 
						D3DPOOL_MANAGED, &this->mIndexBuffer, NULL);
					
					if (SUCCEEDED(theResult))
					{
						/* lock the ib */
						theResult = this->mIndexBuffer->Lock(0, 0, (VOID**)&theIndexData, 0);
						
						if (SUCCEEDED(theResult))
						{
							/* read the data into the ib */
							::ReadFile(theFileHandle, (memory*)theIndexData, this->mPolygonCount * sizeof(uint16) * 3, &theAmountRead, NULL);
							
							/* unlock the ib */
							theResult = this->mIndexBuffer->Unlock();
							
							/* only try to load the texture if we could load a model */
							if (SUCCEEDED(theResult))
							{
								/* load the texture */
								theResult = D3DXCreateTextureFromFileEx(inDevice, inTexture1Name, 
									D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_A8R8G8B8, 
									D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
									D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 0, NULL, NULL, &this->mTexture1);

								/* only try to load the texture 2 if we could load texture 1 */
								if (SUCCEEDED(theResult))
								{
									/* load the texture */
									theResult = D3DXCreateTextureFromFileEx(inDevice, inTexture2Name, 
										D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_A8R8G8B8, 
										D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
										D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 0, NULL, NULL, &this->mTexture2);
								}
							}
						}
					}
				}
			}
		}
		
		/* close the file */
		::CloseHandle(theFileHandle); 
	}
}


/*
 * LoadShaders
 *
 * Loads the shaders to use for this model.
 */
void
GG4Model::LoadShaders(LPDIRECT3DDEVICE9 inDevice, const char* inVertexShader, const char* inPixelShader)
{
	HRESULT				theResult;
	LPD3DXBUFFER		theErrorBuffer			= NULL;
	LPD3DXBUFFER		theShaderBuffer			= NULL;
	D3DVERTEXELEMENT9	theShaderDeclaration[]	=
	{
		{0,  0, D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,	0}, 
		{0, 12, D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_NORMAL,	0}, 
		{0, 24, D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_COLOR,		0}, 
		{0, 28, D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,	0}, 
		D3DDECL_END()
	};

	/* create the shader declaration */
	theResult = inDevice->CreateVertexDeclaration(theShaderDeclaration, &this->mVertexDeclaration);

	if (SUCCEEDED(theResult))
	{
		/* assemble the shader from the file */
		theResult = ::D3DXAssembleShaderFromFile(inVertexShader, NULL, NULL, 0, &theShaderBuffer, &theErrorBuffer);

		if (SUCCEEDED(theResult))
		{
			/* create the shader */
			theResult = inDevice->CreateVertexShader((DWORD*)theShaderBuffer->GetBufferPointer(), &this->mVertexShader);

			if (SUCCEEDED(theResult))
				;
			else
			{
				OutputDebugString("******* Unable To Create Vertex Shader *******\n");
			}
		}
		else
		{
			/* dump error to output */
			OutputDebugString("******* Vertex Shader Compilation Error *******\n");
			OutputDebugString((char*)theErrorBuffer->GetBufferPointer());
		}

		/* release the buffers */
		if (theShaderBuffer)
			theShaderBuffer->Release();
		if (theErrorBuffer)
			theErrorBuffer->Release();
	}

	if (SUCCEEDED(theResult))
	{
		/* assemble the shader from the file */
		theResult = ::D3DXAssembleShaderFromFile(inPixelShader, NULL, NULL, 0, &theShaderBuffer, &theErrorBuffer);

		if (SUCCEEDED(theResult))
		{
			/* create the shader */
			theResult = inDevice->CreatePixelShader((DWORD*)theShaderBuffer->GetBufferPointer(), &this->mPixelShader);

			if (SUCCEEDED(theResult))
				;
			else
			{
				OutputDebugString("******* Unable To Create Pixel Shader *******\n");
			}
		}
		else
		{
			/* dump error to output */
			OutputDebugString("******* Pixel Shader Compilation Error *******\n");
			OutputDebugString((char*)theErrorBuffer->GetBufferPointer());
		}

		/* release the buffers */
		if (theShaderBuffer)
			theShaderBuffer->Release();
		if (theErrorBuffer)
			theErrorBuffer->Release();
	}
}


/*
 * Release
 *
 * Cleans up and releases any allocated data.
 */
void
GG4Model::Release(void)
{
	/* cleanup materials */
	if (this->mMaterials)
	{
		for (uint32 theLoop = 0; theLoop < this->mMaterialCount; theLoop++)
			delete this->mMaterials[theLoop];
		
		delete[] this->mMaterials;
	}
	
	/* release data */
	if (this->mIndexBuffer)
		this->mIndexBuffer->Release();
	if (this->mPixelShader)
		this->mPixelShader->Release();
	if (this->mTexture1)
		this->mTexture1->Release();
	if (this->mTexture2)
		this->mTexture2->Release();
	if (this->mVertexBuffer)
		this->mVertexBuffer->Release();
	if (this->mVertexDeclaration)
		this->mVertexDeclaration->Release();
	if (this->mVertexShader)
		this->mVertexShader->Release();
}


/*
 * Render
 *
 * Renders this model using the appropriate technique.
 */
bool
GG4Model::Render(LPDIRECT3DDEVICE9 inDevice)
{
	bool theResult = false;

	/* call the appropriate render */
	switch (this->mTeamColorTechnique)
	{
	case kgg4tctPolygonTinting:
		theResult = this->RenderWithPolyTinting(inDevice);
		break;

	case kgg4tctTextureMaskingMultiTexture:
		theResult = this->RenderWithTextureMaskMultiTexture(inDevice);
		break;

	case kgg4tctTextureMaskingMultiPass:
		theResult = this->RenderWithTextureMaskMultiPass(inDevice);
		break;

	case kgg4tctVertexAndPixelShader:
		theResult = this->RenderWithVertexAndPixelShaders(inDevice);
		break;

	default:
		assert(false);
		break;
	}

	/* return the result */
	return theResult;
}


/*
 * RenderWithPolyTinting
 *
 * Renders this model using the polygon tinting
 * technique. Performance has been sacrificed
 * for clarity; i.e. setting redundant render states
 * and textures galore.
 */
bool
GG4Model::RenderWithPolyTinting(LPDIRECT3DDEVICE9 inDevice)
{
	D3DMATERIAL9	theMaterial;
	bool			theResult = false;

	/* setup materials to pure white */
	::ZeroMemory(&theMaterial, sizeof(D3DMATERIAL9));
	theMaterial.Diffuse.r = theMaterial.Ambient.r = 1.0F;
	theMaterial.Diffuse.g = theMaterial.Ambient.g = 1.0F;
	theMaterial.Diffuse.b = theMaterial.Ambient.b = 1.0F;
	theMaterial.Diffuse.a = theMaterial.Ambient.a = 1.0F;
	inDevice->SetMaterial(&theMaterial);

	/* setup vertex color rendering */
	inDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1);
	inDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);

	/* setup render states */
	inDevice->SetRenderState(D3DRS_COLORVERTEX,		TRUE);
	inDevice->SetRenderState(D3DRS_CULLMODE,		D3DCULL_CCW);
	inDevice->SetRenderState(D3DRS_ZENABLE,			D3DZB_TRUE);
	inDevice->SetRenderState(D3DRS_ZWRITEENABLE,	TRUE);
	inDevice->SetRenderState(D3DRS_ZFUNC,			D3DCMP_LESSEQUAL);

	/* enable alpha blending */
	inDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,	TRUE);
	inDevice->SetRenderState(D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA);
	inDevice->SetRenderState(D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA);
	inDevice->SetRenderState(D3DRS_ALPHAFUNC,			D3DCMP_GREATEREQUAL);

	/* setup filtering for our stages */
	inDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	inDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	inDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	/* Texture Stage 0: Draw the model normally. */
	inDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	inDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	inDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	inDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
	inDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	inDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	inDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	inDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	/* Texture Stage 1: Disabled. */
	inDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	inDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	/* set the textures */
	inDevice->SetTexture(0, this->mTexture1);
	
	/* setup shaders */
	inDevice->SetFVF(D3DVERTEXFORMAT_GG4MODEL);
	inDevice->SetVertexShader(NULL);
	
	/* setup vertex and index data */
	inDevice->SetIndices(this->mIndexBuffer);
	inDevice->SetStreamSource(0, this->mVertexBuffer, 0, sizeof(D3DVERTEX_GG4MODEL));

	/* render if this is a valid technique */
	if (this->ValidateTechnique(inDevice))
	{
		/********************************************************************/
		/* PASS 1 - Render only the non team color polygons of the model. */
		/********************************************************************/
		
		/* render */
		inDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
			this->mMaterials[0]->mVertexStartingIndex, 
			0, 
			this->mMaterials[0]->mVertexRangeLength, 
			this->mMaterials[0]->mPolygonStartingIndex * 3,
			this->mMaterials[0]->mPolygonRangeLength);

		/******************************************************************/
		/* PASS 2 - Render the team color polygons of the model tinted. */
		/******************************************************************/
		
		/* setup material rendering */
		inDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
		inDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);

		/* set the team color to the material */
		theMaterial.Diffuse.r = theMaterial.Ambient.r = this->mTeamColorMajor.r;
		theMaterial.Diffuse.g = theMaterial.Ambient.g = this->mTeamColorMajor.g;
		theMaterial.Diffuse.b = theMaterial.Ambient.b = this->mTeamColorMajor.b;
		theMaterial.Diffuse.a = theMaterial.Ambient.a = this->mTeamColorMajor.a;
		inDevice->SetMaterial(&theMaterial);

		/* render */
		inDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
			this->mMaterials[1]->mVertexStartingIndex, 
			0, 
			this->mMaterials[1]->mVertexRangeLength, 
			this->mMaterials[1]->mPolygonStartingIndex * 3,
			this->mMaterials[1]->mPolygonRangeLength);

		/* we were successful */
		theResult = true;
	}

	/* return the result */
	return theResult;
}


/*
 * RenderWithTextureMaskMultiTexture
 *
 * Renders this model using the texture mask multi-texture
 * technique. Performance has been sacrificed
 * for clarity; i.e. setting redundant render states
 * and textures galore.
 */
bool
GG4Model::RenderWithTextureMaskMultiTexture(LPDIRECT3DDEVICE9 inDevice)
{
	D3DMATERIAL9	theMaterial;
	uint32			theLoop;
	bool			theResult = false;

	/* setup materials to pure white */
	::ZeroMemory(&theMaterial, sizeof(D3DMATERIAL9));
	theMaterial.Diffuse.r = theMaterial.Ambient.r = 1.0F;
	theMaterial.Diffuse.g = theMaterial.Ambient.g = 1.0F;
	theMaterial.Diffuse.b = theMaterial.Ambient.b = 1.0F;
	theMaterial.Diffuse.a = theMaterial.Ambient.a = 1.0F;
	inDevice->SetMaterial(&theMaterial);

	/* setup vertex color rendering */
	inDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1);
	inDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);

	/* setup render states */
	inDevice->SetRenderState(D3DRS_COLORVERTEX,		TRUE);
	inDevice->SetRenderState(D3DRS_CULLMODE,		D3DCULL_CCW);
	inDevice->SetRenderState(D3DRS_ZENABLE,			D3DZB_TRUE);
	inDevice->SetRenderState(D3DRS_ZWRITEENABLE,	TRUE);
	inDevice->SetRenderState(D3DRS_ZFUNC,			D3DCMP_LESSEQUAL);

	/* enable alpha blending */
	inDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,	TRUE);
	inDevice->SetRenderState(D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA);
	inDevice->SetRenderState(D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA);
	inDevice->SetRenderState(D3DRS_ALPHAFUNC,			D3DCMP_GREATEREQUAL);

	/* setup filtering for our stages */
	inDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	inDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	inDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	inDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	inDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	inDevice->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	/* put the team color in TFACTOR */
	inDevice->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_COLORVALUE(this->mTeamColorMajor.r, this->mTeamColorMajor.g, this->mTeamColorMajor.b, this->mTeamColorMajor.a));

	/* Texture Stage 0: Blend between the texture color
	 * and TFACTOR based on the textures alpha.  We use
	 * only the diffuse for alpha. */
	inDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_BLENDTEXTUREALPHA);
	inDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	inDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
	inDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	inDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	inDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	inDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	inDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	/* Texture Stage 1: Modulate current with diffuse to get lighting.
	 * Do nothing with alpha. */
	inDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	inDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	inDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	inDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	inDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
	inDevice->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
	inDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);
	inDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	/* Texture Stage 2: Disabled. */
	inDevice->SetTextureStageState(2, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	inDevice->SetTextureStageState(2, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	/* set the textures */
	inDevice->SetTexture(0, this->mTexture1);
	inDevice->SetTexture(1, this->mTexture1);
	
	/* setup shaders */
	inDevice->SetFVF(D3DVERTEXFORMAT_GG4MODEL);
	inDevice->SetVertexShader(NULL);
	
	/* setup vertex and index data */
	inDevice->SetIndices(this->mIndexBuffer);
	inDevice->SetStreamSource(0, this->mVertexBuffer, 0, sizeof(D3DVERTEX_GG4MODEL));

	/* render if this is a valid technique */
	if (this->ValidateTechnique(inDevice))
	{
		/*****************************************************************************************/
		/* PASS 1 - This technique requires only one pass if alpha is not used for transparency. */
		/*****************************************************************************************/
		for (theLoop = 0; theLoop < this->mMaterialCount; theLoop++)
		{
			/* draw */
			inDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
				this->mMaterials[theLoop]->mVertexStartingIndex, 
				0, 
				this->mMaterials[theLoop]->mVertexRangeLength, 
				this->mMaterials[theLoop]->mPolygonStartingIndex * 3,
				this->mMaterials[theLoop]->mPolygonRangeLength);
		}

		/* we were successful */
		theResult = true;
	}

	/* return the result */
	return theResult;
}


/*
 * RenderWithTextureMaskMultiPass
 *
 * Renders this model using the texture mask multi-pass
 * technique. Performance has been sacrificed
 * for clarity; i.e. setting redundant render states
 * and textures galore.
 */
bool
GG4Model::RenderWithTextureMaskMultiPass(LPDIRECT3DDEVICE9 inDevice)
{
	D3DMATERIAL9	theMaterial;
	uint32			theLoop;
	bool			theResult = false;

	/* setup materials to pure white */
	::ZeroMemory(&theMaterial, sizeof(D3DMATERIAL9));
	theMaterial.Diffuse.r = theMaterial.Ambient.r = 1.0F;
	theMaterial.Diffuse.g = theMaterial.Ambient.g = 1.0F;
	theMaterial.Diffuse.b = theMaterial.Ambient.b = 1.0F;
	theMaterial.Diffuse.a = theMaterial.Ambient.a = 1.0F;
	inDevice->SetMaterial(&theMaterial);

	/* setup vertex color rendering */
	inDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1);
	inDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);

	/* setup render states */
	inDevice->SetRenderState(D3DRS_COLORVERTEX,		TRUE);
	inDevice->SetRenderState(D3DRS_CULLMODE,		D3DCULL_CCW);
	inDevice->SetRenderState(D3DRS_ZENABLE,			D3DZB_TRUE);
	inDevice->SetRenderState(D3DRS_ZWRITEENABLE,	TRUE);
	inDevice->SetRenderState(D3DRS_ZFUNC,			D3DCMP_LESSEQUAL);

	/* enable alpha blending */
	inDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,	TRUE);
	inDevice->SetRenderState(D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA);
	inDevice->SetRenderState(D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA);
	inDevice->SetRenderState(D3DRS_ALPHAFUNC,			D3DCMP_GREATEREQUAL);

	/* setup filtering for our stages */
	inDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	inDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	inDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	/* Texture Stage 0: Draw the model normally, letting 
	 * just the diffuse alpha through. */
	inDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	inDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	inDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	inDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2);
	inDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	inDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	inDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	inDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	/* Texture Stage 1: Disabled. */
	inDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	inDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	/* set the textures */
	inDevice->SetTexture(0, this->mTexture1);
	
	/* setup shaders */
	inDevice->SetFVF(D3DVERTEXFORMAT_GG4MODEL);
	inDevice->SetVertexShader(NULL);
	
	/* setup vertex and index data */
	inDevice->SetIndices(this->mIndexBuffer);
	inDevice->SetStreamSource(0, this->mVertexBuffer, 0, sizeof(D3DVERTEX_GG4MODEL));

	/* render if this is a valid technique */
	if (this->ValidateTechnique(inDevice))
	{
		/*****************************/
		/* PASS 1 - Render normally. */
		/*****************************/
		
		/* loop through the models "materials" */
		for (theLoop = 0; theLoop < this->mMaterialCount; theLoop++)
		{
			/* draw */
			inDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
				this->mMaterials[theLoop]->mVertexStartingIndex, 
				0, 
				this->mMaterials[theLoop]->mVertexRangeLength, 
				this->mMaterials[theLoop]->mPolygonStartingIndex * 3,
				this->mMaterials[theLoop]->mPolygonRangeLength);
		}


		/*************************************************/
		/* PASS 2 - Invert blend modes and render again. */
		/*************************************************/
		
		/* instead of changing the src and dest blends, we could
		 * set the "complement" flag on the alpha op for the 0th
		 * texture stage. */
		inDevice->SetRenderState(D3DRS_SRCBLEND,	D3DBLEND_INVSRCALPHA);
		inDevice->SetRenderState(D3DRS_DESTBLEND,	D3DBLEND_SRCALPHA);

		/* setup material rendering */
		inDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
		inDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);

		/* set the team color to the material */
		theMaterial.Diffuse.r = theMaterial.Ambient.r = this->mTeamColorMajor.r;
		theMaterial.Diffuse.g = theMaterial.Ambient.g = this->mTeamColorMajor.g;
		theMaterial.Diffuse.b = theMaterial.Ambient.b = this->mTeamColorMajor.b;
		theMaterial.Diffuse.a = theMaterial.Ambient.a = this->mTeamColorMajor.a;
		inDevice->SetMaterial(&theMaterial);

		/* Texture Stage 0: Use just the diffuse for color, and texture for alpha. */
		inDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2);
		inDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		inDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		inDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
		inDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		inDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		/* render only the second of the models "materials" */
		inDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
			this->mMaterials[1]->mVertexStartingIndex, 
			0, 
			this->mMaterials[1]->mVertexRangeLength, 
			this->mMaterials[1]->mPolygonStartingIndex * 3,
			this->mMaterials[1]->mPolygonRangeLength);

		/* we were successful */
		theResult = true;
	}

	/* return the result */
	return theResult;
}


/*
 * RenderWithVertexAndPixelShaders
 *
 * Renders this model using the advanced vertex
 * and pixel shader technique. Performance has been sacrificed
 * for clarity; i.e. setting redundant render states
 * and textures galore.
 */
bool
GG4Model::RenderWithVertexAndPixelShaders(LPDIRECT3DDEVICE9 inDevice)
{
	float	theTeamColorMajor[4];
	float	theTeamColorMinor[4];
	uint32	theLoop;
	bool	theResult = false;

	/* only render if we have shaders */
	if ((this->mPixelShader) && 
		(this->mVertexShader))
	{
		/* setup vertex color rendering */
		inDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1);
		inDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);

		/* setup render states */
		inDevice->SetRenderState(D3DRS_COLORVERTEX,		TRUE);
		inDevice->SetRenderState(D3DRS_CULLMODE,		D3DCULL_CCW);
		inDevice->SetRenderState(D3DRS_ZENABLE,			D3DZB_TRUE);
		inDevice->SetRenderState(D3DRS_ZWRITEENABLE,	TRUE);
		inDevice->SetRenderState(D3DRS_ZFUNC,			D3DCMP_LESSEQUAL);

		/* enable alpha blending */
		inDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,	TRUE);
		inDevice->SetRenderState(D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA);
		inDevice->SetRenderState(D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA);
		inDevice->SetRenderState(D3DRS_ALPHAFUNC,			D3DCMP_GREATEREQUAL);

		/* setup filtering for our stages */
		inDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		inDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		inDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		inDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		inDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		inDevice->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		/* set the textures */
		inDevice->SetTexture(0, this->mTexture1);
		inDevice->SetTexture(1, this->mTexture2);
		
		/* setup shaders */
		inDevice->SetPixelShader(this->mPixelShader);
		inDevice->SetVertexDeclaration(this->mVertexDeclaration);
		inDevice->SetVertexShader(this->mVertexShader);

		/* setup shader constants */
		theTeamColorMajor[0] = this->mTeamColorMajor.r;
		theTeamColorMajor[1] = this->mTeamColorMajor.g;
		theTeamColorMajor[2] = this->mTeamColorMajor.b;
		theTeamColorMajor[3] = this->mTeamColorMajor.a;
		theTeamColorMinor[0] = this->mTeamColorMinor.r;
		theTeamColorMinor[1] = this->mTeamColorMinor.g;
		theTeamColorMinor[2] = this->mTeamColorMinor.b;
		theTeamColorMinor[3] = this->mTeamColorMinor.a;
		inDevice->SetVertexShaderConstantF	(kgg4vscTransformWorld,			(float*)&this->mTransform,	4);
		inDevice->SetVertexShaderConstantF	(kgg4vscLightTeamColorMajor,	theTeamColorMajor,			1);
		inDevice->SetVertexShaderConstantF	(kgg4vscLightTeamColorMinor,	theTeamColorMinor,			1);
		inDevice->SetPixelShaderConstantF	(kgg4pscLightTeamColorMajor,	theTeamColorMajor,			1);
		inDevice->SetPixelShaderConstantF	(kgg4pscLightTeamColorMinor,	theTeamColorMinor,			1);
		
		/* setup vertex and index data */
		inDevice->SetIndices(this->mIndexBuffer);
		inDevice->SetStreamSource(0, this->mVertexBuffer, 0, sizeof(D3DVERTEX_GG4MODEL));

		/* render if this is a valid technique */
		if (this->ValidateTechnique(inDevice))
		{
			/********************************************************************/
			/* PASS 1                                                           */
			/********************************************************************/
			for (theLoop = 0; theLoop < this->mMaterialCount; theLoop++)
			{
				/* draw */
				inDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
					this->mMaterials[theLoop]->mVertexStartingIndex, 
					0, 
					this->mMaterials[theLoop]->mVertexRangeLength, 
					this->mMaterials[theLoop]->mPolygonStartingIndex * 3,
					this->mMaterials[theLoop]->mPolygonRangeLength);
			}

			/* we were successful */
			theResult = true;
		}
	}

	/* return the result */
	return theResult;
}


/*
 * ValidateTechnique
 *
 * Attempts to validate the current team color technique.
 */
bool
GG4Model::ValidateTechnique(LPDIRECT3DDEVICE9 inDevice)
{
	uint32 theNumPasses;

	/* is this technique valid? */
	if (this->mValidatedTechniques[this->mTeamColorTechnique] == kgg4tcvsValid)
		;
	else
	{
		/* do we need to try to validate? */
		if (this->mValidatedTechniques[this->mTeamColorTechnique] == kgg4tcvsUnknown)
		{
			/* attempt to validate the device */
			if (inDevice->ValidateDevice(&theNumPasses) == D3D_OK)
			{
				/* this technique is valid */
				this->mValidatedTechniques[this->mTeamColorTechnique] = kgg4tcvsValid;
			}
			else
			{
				/* this technique is invalid */
				this->mValidatedTechniques[this->mTeamColorTechnique] = kgg4tcvsInvalid;
			}
		}
	}

	/* return true if we can do this technique */
	return (this->mValidatedTechniques[this->mTeamColorTechnique] == kgg4tcvsValid);
}