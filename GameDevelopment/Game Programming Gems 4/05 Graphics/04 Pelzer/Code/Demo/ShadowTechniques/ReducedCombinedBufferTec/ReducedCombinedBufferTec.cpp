//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Subproject:			ShadowsDemo
//*				File:				ReducedCombinedBufferTec.cpp
//*
//********************************************************************************************************************
//*
//*				Author:				Kurt Pelzer
//*									Copyright (c) 2002 Kurt Pelzer
//*									All rights reserved.
//*
//********************************************************************************************************************
//********************************************************************************************************************




#include		"ReducedCombinedBufferTec.h"
#include		"d3dx8.h"




//********************************************************************************************************************
//*				REDUCEDCOMBINEDBUFFERTEC
//********************************************************************************************************************

REDUCEDCOMBINEDBUFFERTEC::REDUCEDCOMBINEDBUFFERTEC				(	IGN_OBJIF_GfxContext*		a_pContextInterface,
																	IGN_OBJIF_GfxMixer*			a_pMixer )
{
	//----------------------------------------------------------|---------------------------REDUCEDCOMBINEDBUFFERTEC-|
	// public method
	// Constructor

	m_pContextInterface	= a_pContextInterface;
	m_pMixer			= a_pMixer;
	m_pContextInterface->CreateCamera( m_pLightCamera );

	//
	// Create the special ID material
	//
	m_pContextInterface->CreateTextureBlock( m_IDShader.m_pTextureBlock0 );
	if( NULL!=m_IDShader.m_pTextureBlock0 )
	{
		m_IDShader.m_pTextureBlock0->SetColorCalc( 
			IGN_ENUM_TEXOPERATION_SELECTARG1, 
			IGN_ENUM_TEXARGUMENT_COLORFACTOR, 
			IGN_ENUM_TEXARGUMENT_CURRENT );
		m_IDShader.m_pTextureBlock0->SetAlphaCalc( 
			IGN_ENUM_TEXOPERATION_SELECTARG1, 
			IGN_ENUM_TEXARGUMENT_COLORFACTOR, 
			IGN_ENUM_TEXARGUMENT_CURRENT );
	}
	m_pContextInterface->CreateTextureBlock( m_IDShader.m_pTextureBlock1 );
	if( NULL!=m_IDShader.m_pTextureBlock1 )
	{
		m_IDShader.m_pTextureBlock1->SetColorCalc( 
			IGN_ENUM_TEXOPERATION_DISABLE, 
			IGN_ENUM_TEXARGUMENT_CURRENT, 
			IGN_ENUM_TEXARGUMENT_CURRENT );
		m_IDShader.m_pTextureBlock1->SetAlphaCalc( 
			IGN_ENUM_TEXOPERATION_DISABLE, 
			IGN_ENUM_TEXARGUMENT_CURRENT, 
			IGN_ENUM_TEXARGUMENT_CURRENT );
	}
	m_pContextInterface->CreateMaterial( m_IDShader.m_pMaterial );
	if( NULL!=m_IDShader.m_pMaterial )
	{
		m_IDShader.m_pMaterial->AttachTextureBlock( 0, m_IDShader.m_pTextureBlock0 );
		m_IDShader.m_pMaterial->AttachTextureBlock( 1, m_IDShader.m_pTextureBlock1 );
	}
	m_pContextInterface->CreateShader( m_IDShader.m_pShader );
	if( NULL!=m_IDShader.m_pShader )
	{
		m_IDShader.m_pShader->AttachMaterial( m_IDShader.m_pMaterial );
	}

	//
	// Create the material for the 'reduced combined buffer' shadow pass
	//
	m_pContextInterface->CreateImageData( m_RedCombBufTecShader.m_pIDImage );
	if( NULL!=m_RedCombBufTecShader.m_pIDImage )
	{
		IGN_STRUCT_LayersDesc layersDesc;
		layersDesc.m_uiWidth	= constImageWidth;
		layersDesc.m_uiHeight	= constImageHeight;
		layersDesc.m_uiDepth	= 0;
		layersDesc.m_bCubeMap	= false;
		layersDesc.m_Format		= IGN_ENUM_PIXELFORMAT_A8R8G8B8;
		layersDesc.m_uiLevels	= 1;
		layersDesc.m_dwUsage	= IGNUSAGE_RENDERTARGET;
		layersDesc.m_Pool		= IGN_ENUM_POOLTYPE_DEFAULT;
		m_RedCombBufTecShader.m_pIDImage->CreateLayers( layersDesc );
	}
	m_pMixer->CreateSpecialDepthBuffer( constImageWidth, constImageHeight, IGN_ENUM_DEPTHFORMAT_D24S8, m_RedCombBufTecShader.m_hDepthBuffer );
	m_pContextInterface->CreateTextureBlock( m_RedCombBufTecShader.m_pTextureBlock0 );
	if( NULL!=m_RedCombBufTecShader.m_pTextureBlock0 )
	{
		m_RedCombBufTecShader.m_pTextureBlock0->SetColorCalc( 
			IGN_ENUM_TEXOPERATION_SUBTRACT, 
			IGN_ENUM_TEXARGUMENT_COLORFACTOR, 
			IGN_ENUM_TEXARGUMENT_TEXTURE );
		m_RedCombBufTecShader.m_pTextureBlock0->SetAlphaCalc( 
			IGN_ENUM_TEXOPERATION_SELECTARG1, 
			IGN_ENUM_TEXARGUMENT_COLORFACTOR, 
			IGN_ENUM_TEXARGUMENT_TEXTURE );
		m_RedCombBufTecShader.m_pTextureBlock0->SetTextureCoordinateCalc( IGN_ENUM_TEXCOORDCALC_CAMERASPACEPOSITION );
		m_RedCombBufTecShader.m_pTextureBlock0->SetTextureCoordinateTrafo( IGNTEXCOORDTRAFO_COUNT3|IGNTEXCOORDTRAFO_PROJECTED );
		m_RedCombBufTecShader.m_pTextureBlock0->SetTextureFiltering( 
			IGN_ENUM_TEXFILTER_POINT, 
			IGN_ENUM_TEXFILTER_POINT, 
			IGN_ENUM_TEXFILTER_POINT );
		m_RedCombBufTecShader.m_pTextureBlock0->SetTextureAdressing( 
			IGN_ENUM_TEXADDRESS_CLAMP, 
			IGN_ENUM_TEXADDRESS_CLAMP, 
			IGN_ENUM_TEXADDRESS_CLAMP );
		m_RedCombBufTecShader.m_pTextureBlock0->AttachImageData( m_RedCombBufTecShader.m_pIDImage );
	}
	m_pContextInterface->CreateTextureBlock( m_RedCombBufTecShader.m_pTextureBlock1 );
	if( NULL!=m_RedCombBufTecShader.m_pTextureBlock1 )
	{		m_RedCombBufTecShader.m_pTextureBlock1->SetColorCalc( 
			IGN_ENUM_TEXOPERATION_ADD, 
			IGN_ENUM_TEXARGUMENT_CURRENT,
			static_cast<IGN_ENUM_TEXARGUMENT>( IGN_ENUM_TEXARGUMENT_ALPHAREPLICATE|IGN_ENUM_TEXARGUMENT_CURRENT ) );
		m_RedCombBufTecShader.m_pTextureBlock1->SetAlphaCalc( 
			IGN_ENUM_TEXOPERATION_SELECTARG1, 
			IGN_ENUM_TEXARGUMENT_CURRENT, 
			IGN_ENUM_TEXARGUMENT_CURRENT );
	}
	m_pContextInterface->CreateTextureBlock( m_RedCombBufTecShader.m_pTextureBlock2 );
	if( NULL!=m_RedCombBufTecShader.m_pTextureBlock2 )
	{
		m_RedCombBufTecShader.m_pTextureBlock2->SetColorCalc( 
			IGN_ENUM_TEXOPERATION_DOTPRODUCT3, 
			IGN_ENUM_TEXARGUMENT_CURRENT, 
			IGN_ENUM_TEXARGUMENT_CURRENT );
		m_RedCombBufTecShader.m_pTextureBlock2->SetAlphaCalc( 
			IGN_ENUM_TEXOPERATION_SELECTARG1, 
			IGN_ENUM_TEXARGUMENT_CURRENT, 
			IGN_ENUM_TEXARGUMENT_CURRENT );
	}
	m_pContextInterface->CreateTextureBlock( m_RedCombBufTecShader.m_pTextureBlock3 );
	if( NULL!=m_RedCombBufTecShader.m_pTextureBlock3 )
	{
		m_RedCombBufTecShader.m_pTextureBlock3->SetColorCalc( 
			IGN_ENUM_TEXOPERATION_DISABLE,
			IGN_ENUM_TEXARGUMENT_CURRENT, 
			IGN_ENUM_TEXARGUMENT_CURRENT );
		m_RedCombBufTecShader.m_pTextureBlock3->SetAlphaCalc( 
			IGN_ENUM_TEXOPERATION_DISABLE,
			IGN_ENUM_TEXARGUMENT_CURRENT, 
			IGN_ENUM_TEXARGUMENT_CURRENT );
	}
	m_pContextInterface->CreateMaterial( m_RedCombBufTecShader.m_pMaterial );
	if( NULL!=m_RedCombBufTecShader.m_pMaterial )
	{
		m_RedCombBufTecShader.m_pMaterial->AttachTextureBlock( 0, m_RedCombBufTecShader.m_pTextureBlock0 );
		m_RedCombBufTecShader.m_pMaterial->AttachTextureBlock( 1, m_RedCombBufTecShader.m_pTextureBlock1 );
		m_RedCombBufTecShader.m_pMaterial->AttachTextureBlock( 2, m_RedCombBufTecShader.m_pTextureBlock2 );
		m_RedCombBufTecShader.m_pMaterial->AttachTextureBlock( 3, m_RedCombBufTecShader.m_pTextureBlock3 );
	}
	m_pContextInterface->CreateShader( m_RedCombBufTecShader.m_pShader );
	if( NULL!=m_RedCombBufTecShader.m_pShader )
	{
		m_RedCombBufTecShader.m_pShader->AttachMaterial( m_RedCombBufTecShader.m_pMaterial );
	}
}


REDUCEDCOMBINEDBUFFERTEC::~REDUCEDCOMBINEDBUFFERTEC				()
{
	//----------------------------------------------------------|--------------------------~REDUCEDCOMBINEDBUFFERTEC-|
	// public method
	// Destructor

	//
	// Destroy camera and materials
	//
	if( NULL!=m_pLightCamera )
		m_pLightCamera->Release();
	if( NULL!=m_IDShader.m_pTextureBlock0 )
		m_IDShader.m_pTextureBlock0->Release();
	if( NULL!=m_IDShader.m_pTextureBlock1 )
		m_IDShader.m_pTextureBlock1->Release();
	if( NULL!=m_IDShader.m_pMaterial )
		m_IDShader.m_pMaterial->Release();
	if( NULL!=m_IDShader.m_pShader )
		m_IDShader.m_pShader->Release();
	if( NULL!=m_RedCombBufTecShader.m_pIDImage )
	{
		m_RedCombBufTecShader.m_pIDImage->DestroyLayers();
		m_RedCombBufTecShader.m_pIDImage->Release();
	}
	if( NULL!=m_RedCombBufTecShader.m_pTextureBlock0 )
		m_RedCombBufTecShader.m_pTextureBlock0->Release();
	if( NULL!=m_RedCombBufTecShader.m_pTextureBlock1 )
		m_RedCombBufTecShader.m_pTextureBlock1->Release();
	if( NULL!=m_RedCombBufTecShader.m_pTextureBlock2 )
		m_RedCombBufTecShader.m_pTextureBlock2->Release();
	if( NULL!=m_RedCombBufTecShader.m_pTextureBlock3 )
		m_RedCombBufTecShader.m_pTextureBlock3->Release();
	if( NULL!=m_RedCombBufTecShader.m_pMaterial )
		m_RedCombBufTecShader.m_pMaterial->Release();
	if( NULL!=m_RedCombBufTecShader.m_pShader )
		m_RedCombBufTecShader.m_pShader->Release();
	m_pMixer->DestroySpecialDepthBuffer( m_RedCombBufTecShader.m_hDepthBuffer );
}


IGNBOOL REDUCEDCOMBINEDBUFFERTEC::PreProcess					(	CAMERA*						a_pCamera,
																	IGN_OBJIF_GfxLight*			a_pLight, 
																	structVisualObject*			a_pVisualObject,
																	IGNU32						a_u32ObjectCount )			
{
	//----------------------------------------------------------|-----------------------------------------PreProcess-|
	// public method
	// Preprocesses the 'reduced combined buffer' technique.

	//
	// Calculate the light camera
	//
	IGN_STRUCT_LightData lightData;
	a_pLight->GetVertexLightingData( 0, lightData );
	D3DXMATRIX d3dxLightViewMatrix;
	D3DXVECTOR3 d3dxVectorEye( lightData.m_Position.m_fX, lightData.m_Position.m_fY, lightData.m_Position.m_fZ );
	D3DXVECTOR3 d3dxVectorAt( 
		lightData.m_Position.m_fX+lightData.m_Direction.m_fX,
		lightData.m_Position.m_fY+lightData.m_Direction.m_fY,
		lightData.m_Position.m_fZ+lightData.m_Direction.m_fZ );
	D3DXVECTOR3 d3dxVectorUp( 
		lightData.m_Direction.m_fY*lightData.m_Direction.m_fZ,
		lightData.m_Direction.m_fX*lightData.m_Direction.m_fZ,
		lightData.m_Direction.m_fX*lightData.m_Direction.m_fY );
	if( (0==lightData.m_Position.m_fX)&&(0==lightData.m_Position.m_fY)||
		(0==lightData.m_Position.m_fX)&&(0==lightData.m_Position.m_fZ) )
	{
		d3dxVectorUp.x	= 1.0f;
		d3dxVectorUp.y	= 0.0f;
		d3dxVectorUp.z	= 0.0f;
	}
	if( (0==lightData.m_Position.m_fY)&&(0==lightData.m_Position.m_fZ) )
	{
		d3dxVectorUp.x	= 0.0f;
		d3dxVectorUp.y	= 1.0f;
		d3dxVectorUp.z	= 0.0f;
	}
	D3DXMatrixLookAtLH( &d3dxLightViewMatrix, &d3dxVectorEye, &d3dxVectorAt, &d3dxVectorUp );
	D3DXMATRIX d3dxLightProjectionMatrix( 
		static_cast<float>( 1/tan(lightData.m_fPhi/2.0f) ), 0.0f, 0.0f, 0.0f, 
		0.0f, static_cast<float>( 1/tan(lightData.m_fPhi/2.0f) ), 0.0f, 0.0f, 
		0.0f, 0.0f, 1.0f, 1.0f, 
		0.0f, 0.0f, -0.3f, 0.0f );
	IGN_STRUCT_4x4Matrix lightCameraViewMatrix;
	memcpy( &lightCameraViewMatrix, &d3dxLightViewMatrix, 4*4*4 );
	m_pLightCamera->SetViewMatrix( lightCameraViewMatrix );
	IGN_STRUCT_4x4Matrix lightCameraProjectionMatrix;
	memcpy( &lightCameraProjectionMatrix, &d3dxLightProjectionMatrix, 4*4*4 );
	m_pLightCamera->SetProjectionMatrix( lightCameraProjectionMatrix );

	//
	// Get the current viewport
	//
	IGN_STRUCT_Viewport originalViewport;
	m_pMixer->GetViewport( originalViewport );

	//
	// Prepare the new render target (IDImage)
	//
	IGNHANDLE hOriginalRenderTarget;
	IGNHANDLE hOriginalDepthBuffer;
	m_pMixer->GetRenderTarget( hOriginalRenderTarget, hOriginalDepthBuffer );
	IGNHANDLE hImageRenderTargetHandle;
	m_RedCombBufTecShader.m_pIDImage->GetRenderTargetHandle( IGN_ENUM_CUBEMAPFACE_NONE, hImageRenderTargetHandle );
	m_pMixer->SetRenderTarget( hImageRenderTargetHandle, m_RedCombBufTecShader.m_hDepthBuffer );
	IGN_STRUCT_Viewport lightCameraViewport;
	lightCameraViewport.m_u32X		= 0;
	lightCameraViewport.m_u32Y		= 0;
	lightCameraViewport.m_u32Width	= constImageWidth;
	lightCameraViewport.m_u32Height	= constImageHeight;
	lightCameraViewport.m_fMinZ		= 0.0f;
	lightCameraViewport.m_fMaxZ		= 1.0f;
	m_pMixer->SetViewport( lightCameraViewport );
	m_pMixer->ClearViewport( static_cast<IGN_ENUM_VIEWPORTCLEAR>( 
		IGN_ENUM_VIEWPORTCLEAR_COLOR|IGN_ENUM_VIEWPORTCLEAR_DEPTH ), 
		0x00000000, 
		1.0f, 
		0 );
	m_pMixer->ExecuteCamera( m_pLightCamera );
	IGN_ENUM_MASTERBLENDING originalSourceBlend;
	IGN_ENUM_MASTERBLENDING originalDestinationBlend;
	m_pMixer->GetMasterBlending( originalSourceBlend, originalDestinationBlend );
	m_pMixer->SetMasterBlending( IGN_ENUM_MASTERBLENDING_MATERIAL, IGN_ENUM_MASTERBLENDING_MATERIAL );
	
	//
	// Disable all lights
	//
	IGNBOOL bOriginalLighting;
	m_pMixer->GetMasterLighting( bOriginalLighting );
	m_pMixer->SetMasterLighting( false );

	//
	// Set the spacial material
	//
	m_pMixer->SetMasterShaderExec( m_IDShader.m_pShader );

	//
	// Render the involved objekts into the render target with i element from [0,42874]
	//
	for( IGNINT i = a_u32ObjectCount-1; i>=0; i-- )	
	{
		if( NULL!=(a_pVisualObject+i)->m_pVisual )
		{
			m_IDShader.m_pMaterial->SetColorFactor( 
				(((i/(35*35))*7)<<16) +
				((((i%(35*35))/35)*7)<<8) +
				((i%35)*7) );
			(a_pVisualObject+i)->m_pVisual->Execute();
		}
	}

	//
	// Reset the original state
	//
	m_pMixer->SetMasterShaderExec( NULL );
	m_pMixer->SetMasterBlending( originalSourceBlend, originalDestinationBlend );
	m_pMixer->SetRenderTarget( hOriginalRenderTarget, hOriginalDepthBuffer );
	m_pMixer->SetMasterLighting( bOriginalLighting );
	m_pMixer->SetViewport( originalViewport );

	//
	// Set the matrix for texture trafo (Stage 0) in proceed method
	//
	IGN_STRUCT_4x4Matrix camViewMatrix;
	if( NULL!=a_pCamera )
		a_pCamera->GetViewMatrix( camViewMatrix );
	D3DXMATRIX d3dxCamViewMatrix;
	memcpy( &d3dxCamViewMatrix, &camViewMatrix, 4*4*4 );
	float fDeterminant;
	D3DXMATRIX d3dxInvertedCamViewMatrix;
	D3DXMatrixInverse( &d3dxInvertedCamViewMatrix, &fDeterminant, &d3dxCamViewMatrix );
	IGN_STRUCT_4x4Matrix invertedCamViewMatrix;
	memcpy( &invertedCamViewMatrix, &d3dxInvertedCamViewMatrix, 4*4*4 );
	D3DXMATRIX d3dxTextureMatrix( 
		0.5f, 0.0f, 0.0f, 0.0f, 
		0.0f, -0.5f, 0.0f, 0.0f, 
		0.0f, 0.0f, 0.0f, 0.0f, 
		0.5f, 0.5f, 1.0f, 1.0f );
	D3DXMatrixMultiply( &d3dxTextureMatrix, &d3dxLightProjectionMatrix, &d3dxTextureMatrix );
	D3DXMatrixMultiply( &d3dxTextureMatrix, &d3dxLightViewMatrix, &d3dxTextureMatrix );
	D3DXMatrixMultiply( &d3dxTextureMatrix, &d3dxInvertedCamViewMatrix, &d3dxTextureMatrix );
	IGN_STRUCT_4x4Matrix textureMatrix;
	memcpy( &textureMatrix, &d3dxTextureMatrix, 4*4*4 );
	m_RedCombBufTecShader.m_pTextureBlock0->SetTextureMatrix( textureMatrix );
	return true;
}


IGNBOOL REDUCEDCOMBINEDBUFFERTEC::Proceed						(	IGN_OBJIF_GfxLight*			a_pLight, 
																	structVisualObject*			a_pVisualObject,
																	IGNU32						a_u32ObjectCount )			
{
	//----------------------------------------------------------|--------------------------------------------Proceed-|
	// public method
	// Proceeds the 'reduced combined buffer' technique.

	//
	// Prepare the detection of alpha test passes within the stencil buffer without color buffer writes
	// 
	IGN_ENUM_MASTERWRITE originalDepthWrite;
	m_pMixer->GetMasterDepthWrite( originalDepthWrite );
	m_pMixer->SetMasterDepthWrite( IGN_ENUM_MASTERWRITE_DISABLE );
	IGN_ENUM_MASTERCMPFUNC originalDepthTest;
	m_pMixer->GetMasterDepthTesting( originalDepthTest );
	m_pMixer->SetMasterDepthTesting( IGN_ENUM_MASTERCMPFUNC_EQUAL );
	IGN_ENUM_MASTERSTENCIL originalStencilMode;
	IGN_STRUCT_StencilData originalStencilData;
	m_pMixer->GetMasterStencilTesting(
		originalStencilMode,
		originalStencilData );
	IGN_STRUCT_StencilData stencilData;
	stencilData.m_StencilFail			= IGN_ENUM_STENCILOP_KEEP;
	stencilData.m_StencilZFail			= IGN_ENUM_STENCILOP_KEEP;
	stencilData.m_StencilPass			= IGN_ENUM_STENCILOP_INCR;
	stencilData.m_StencilFunc			= IGN_ENUM_CMPFUNC_ALWAYS;
	stencilData.m_u32StencilMask		= 0xffffffff;
	stencilData.m_u32StencilRef			= 0x00000000;
	stencilData.m_u32StencilWriteMask	= 0xffffffff;
	m_pMixer->SetMasterStencilTesting(
		IGN_ENUM_MASTERSTENCIL_ENABLE,
		stencilData );
	IGN_ENUM_MASTERBLENDING originalSourceBlend;
	IGN_ENUM_MASTERBLENDING originalDestinationBlend;
	m_pMixer->GetMasterBlending( originalSourceBlend, originalDestinationBlend );
	m_pMixer->SetMasterBlending( IGN_ENUM_MASTERBLENDING_ZERO, IGN_ENUM_MASTERBLENDING_ONE );

	//
	// Set the alpha test
	//
	IGN_ENUM_MASTERALPHATEST	originalAlphaTest;
	IGN_STRUCT_AlphaTestData	originalAlphaTestData;
	m_pMixer->GetMasterAlphaTesting( originalAlphaTest, originalAlphaTestData );
	IGN_STRUCT_AlphaTestData	alphaTestData;
	alphaTestData.m_AlphaTestFunc	= IGN_ENUM_CMPFUNC_NOTEQUAL;
	alphaTestData.m_AlphaTestRef	= 0x00;
	m_pMixer->SetMasterAlphaTesting( IGN_ENUM_MASTERALPHATEST_ENABLE, alphaTestData );

	//
	// Disable all lights
	//
	IGNBOOL bOriginalLighting;
	m_pMixer->GetMasterLighting( bOriginalLighting );
	m_pMixer->SetMasterLighting( false );

	//
	// Set the special material
	//
	m_pMixer->SetMasterShaderExec( m_RedCombBufTecShader.m_pShader );

	//
	// Render the involved objects with i element from [0,42874]
	//
	for( IGNINT i = a_u32ObjectCount-1; i>=0; i-- )	
	{
		if( NULL!=(a_pVisualObject+i)->m_pVisual )
		{
			m_RedCombBufTecShader.m_pMaterial->SetColorFactor( 
				constZeroAlphaShiftVector+
				(((i/(35*35))*7)<<16) +
				((((i%(35*35))/35)*7)<<8) +
				((i%35)*7) );
			(a_pVisualObject+i)->m_pVisual->Execute();
		}
	}

	//
	// Reset the original state
	//
	m_pMixer->SetMasterShaderExec( NULL );
	m_pMixer->SetMasterStencilTesting(
		originalStencilMode,
		originalStencilData );
	m_pMixer->SetMasterBlending( originalSourceBlend, originalDestinationBlend );
	m_pMixer->SetMasterAlphaTesting( originalAlphaTest, originalAlphaTestData );
	m_pMixer->SetMasterDepthTesting( originalDepthTest );
	m_pMixer->SetMasterDepthWrite( originalDepthWrite );
	m_pMixer->SetMasterLighting( bOriginalLighting );
	return true;
}