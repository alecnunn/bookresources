//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Subproject:			SimpleShaderDemo
//*				File:				Scene.cpp
//*
//********************************************************************************************************************
//*
//*				Author:				Kurt Pelzer
//*									Copyright (c) 2002, 2003 Kurt Pelzer
//*									All rights reserved.
//*
//********************************************************************************************************************
//********************************************************************************************************************




#include		"Scene.h"
#include		"math.h"
#include		"d3dx8.h"
#include		<direct.h>




//********************************************************************************************************************
//*				QUAD
//********************************************************************************************************************

SCENE::SCENE													(	IGN_ENUM_GFXINSTANCETYPE a_InstanceType,
																	IGN_OBJIF_GfxContext*	a_pGfxContextInterface,
																	IGN_OBJIF_GfxMixer*		a_pMixer )
{
	//----------------------------------------------------------|----------------------------------------------SCENE-|
	// public method
	// Constructor
	
	m_hTmrLibrary				= NULL;
	m_pTmrInstanceInterface		= NULL;
	m_pTmrContextInterface		= NULL;
	m_pTimerInterface			= NULL;
	m_pAmbientLight				= NULL;
	for( IGNINT i = 0; i<constLightCount; i++ )
		m_pLight[i]				= NULL;
	for( i = constObjectCount-1; i>=0; i-- )	
	{
		if( m_aryVisualObject[i].m_pVisual )
			m_aryVisualObject[i].m_pVisual		= NULL;
		if( m_aryVisualObject[i].m_pVertexData )
			m_aryVisualObject[i].m_pVertexData	= NULL;
		if( m_aryVisualObject[i].m_pIndexData )
			m_aryVisualObject[i].m_pIndexData	= NULL;
		if( m_aryVisualObject[i].m_pShader )
			m_aryVisualObject[i].m_pShader		= NULL;
		if( m_aryVisualObject[i].m_pMaterial )
			m_aryVisualObject[i].m_pMaterial	= NULL;
		if( m_aryVisualObject[i].m_pTextureBlock0 )
			m_aryVisualObject[i].m_pTextureBlock0= NULL;
		if( m_aryVisualObject[i].m_pTextureBlock1 )
			m_aryVisualObject[i].m_pTextureBlock1= NULL;
		if( m_aryVisualObject[i].m_pTextureBlock2 )
			m_aryVisualObject[i].m_pTextureBlock2= NULL;
		if( m_aryVisualObject[i].m_pTextureBlock3 )
			m_aryVisualObject[i].m_pTextureBlock3= NULL;
	}
	m_pMixer						= a_pMixer;
	m_u32VisualAniStep				= 0;
	m_u32LightAniStep				= 0;
	m_bEnabledAdditionalLights		= true;
	m_bEnabledShadows				= true;
	m_bEnabledSecondLight			= true;
	m_bEnabledColoredLight			= false;
	m_ShadowTechnique				= ENUM_TECHNIQUE_REDUCEDCOMBINEDBUFFER;
	m_pReducedCombinedBufferTec		= NULL;
	m_pCompleteCombinedBufferTec	= NULL;

	//
	// Initialize the system
	//
	Init( a_InstanceType, a_pGfxContextInterface );
}


SCENE::~SCENE													()
{
	//----------------------------------------------------------|---------------------------------------------~SCENE-|
	// public method
	// Destructor

	//
	// Finalize the system
	//
	Finit();
}


void SCENE::Init												(	IGN_ENUM_GFXINSTANCETYPE a_InstanceType,
																	IGN_OBJIF_GfxContext*	a_pGfxContextInterface )
{
	//----------------------------------------------------------|-----------------------------------------------Init-|
	// public method
	// Initializes the tester.

	_chdir( "System" );
	IGNRESULT hResult = 0;

	//
	// Load IGN_timer_sub.dll and create objects
	//
	IGN_STRUCT_IFSubTimer* pSubTimer = NULL;
	m_hTmrLibrary = LoadLibrary( "Ign_timer_sub.dll" );
	IGN_METHOD_GetSubHookTimer* 
			TmrGetInterface = reinterpret_cast<IGN_METHOD_GetSubHookTimer*>( GetProcAddress( m_hTmrLibrary, "GetInterface" ) );
	if( NULL!=TmrGetInterface )
		hResult = TmrGetInterface( &pSubTimer );
	if( NULL!=pSubTimer )
		hResult = pSubTimer->m_pCreateInstance( IGN_ENUM_TMRINSTANCETYPE_WINDOWSMULTIMEDIA, m_pTmrInstanceInterface );
	if( NULL!=m_pTmrInstanceInterface )
		m_pTmrInstanceInterface->CreateContext( 1, m_pTmrContextInterface );
	if( NULL!=m_pTmrContextInterface )
		m_pTmrContextInterface->CreateTimer( m_pTimerInterface );
	if( NULL!=m_pTimerInterface )
	{
		m_pTimerInterface->Reset();
		m_pTimerInterface->Resume();
	}

	_chdir( "..\\" );

	//
	// Create objects in IGN_graphic_sub.dll
	//
	if( NULL!=a_pGfxContextInterface )
	{
		//
		// Create shadow techniques handles
		//
		m_pReducedCombinedBufferTec		= new REDUCEDCOMBINEDBUFFERTEC( a_pGfxContextInterface, m_pMixer );
		m_pCompleteCombinedBufferTec	= new COMPLETECOMBINEDBUFFERTEC( a_pGfxContextInterface, m_pMixer );
		//
		// Create light sources
		//
		a_pGfxContextInterface->CreateLight( m_pAmbientLight );
		m_pAmbientLight->SetAmbientLighting( 0xff303030 );
		for( IGNINT i = 0; i<constLightCount; i++ )	
		{
			a_pGfxContextInterface->CreateLight( m_pLight[i] );
			switch( i )
			{
			case	0:
					{
						m_pLight[i]->SetVertexLighting( 0, true );
						IGN_STRUCT_LightData lightData;
						lightData.m_Type = IGN_ENUM_LIGHTTYPE_SPOT;
						lightData.m_Diffuse.m_fRed		= 0.6f;
						lightData.m_Diffuse.m_fGreen	= 0.6f;
						lightData.m_Diffuse.m_fBlue		= 0.6f;
						lightData.m_Diffuse.m_fAlpha	= 1.0f;
						lightData.m_Specular.m_fRed		= 0.0f;
						lightData.m_Specular.m_fGreen	= 0.0f;
						lightData.m_Specular.m_fBlue	= 0.0f;
						lightData.m_Specular.m_fAlpha	= 0.0f;
						lightData.m_Ambient.m_fRed		= 0.0f;
						lightData.m_Ambient.m_fGreen	= 0.0f;
						lightData.m_Ambient.m_fBlue		= 0.0f;
						lightData.m_Ambient.m_fAlpha	= 0.0f;
						lightData.m_Position.m_fX		= 0.0f;
						lightData.m_Position.m_fY		= 0.0f;
						lightData.m_Position.m_fZ		= 0.0f;
						lightData.m_Direction.m_fX		= 0.0f;
						lightData.m_Direction.m_fY		= 0.0f;
						lightData.m_Direction.m_fZ		= 1.0f;
						lightData.m_fRange				= 2000.0f;
						lightData.m_fFalloff			= 1.0f;
						lightData.m_fTheta				= 3.1416f/4.0f;
						lightData.m_fPhi				= 3.1416f/3.0f;
						lightData.m_fAttenuation0		= 1.0f;
						lightData.m_fAttenuation1		= 0.0f;
						lightData.m_fAttenuation2		= 0.0f;
						m_pLight[i]->SetVertexLightingData( 0, lightData );
					}
					break;
			case	1:
					{
						m_pLight[i]->SetVertexLighting( 0, true );
						IGN_STRUCT_LightData lightData;
						lightData.m_Type = IGN_ENUM_LIGHTTYPE_SPOT;
						lightData.m_Diffuse.m_fRed		= 0.6f;
						lightData.m_Diffuse.m_fGreen	= 0.6f;
						lightData.m_Diffuse.m_fBlue		= 0.6f;
						lightData.m_Diffuse.m_fAlpha	= 1.0f;
						lightData.m_Specular.m_fRed		= 0.0f;
						lightData.m_Specular.m_fGreen	= 0.0f;
						lightData.m_Specular.m_fBlue	= 0.0f;
						lightData.m_Specular.m_fAlpha	= 0.0f;
						lightData.m_Ambient.m_fRed		= 0.0f;
						lightData.m_Ambient.m_fGreen	= 0.0f;
						lightData.m_Ambient.m_fBlue		= 0.0f;
						lightData.m_Ambient.m_fAlpha	= 0.0f;
						lightData.m_Position.m_fX		= 0.0f;
						lightData.m_Position.m_fY		= 0.0f;
						lightData.m_Position.m_fZ		= 0.0f;
						lightData.m_Direction.m_fX		= 0.0f;
						lightData.m_Direction.m_fY		= 0.0f;
						lightData.m_Direction.m_fZ		= 1.0f;
						lightData.m_fRange				= 2000.0f;
						lightData.m_fFalloff			= 1.0f;
						lightData.m_fTheta				= 3.1416f/5.0f;
						lightData.m_fPhi				= 3.1416f/4.5f;
						lightData.m_fAttenuation0		= 1.0f;
						lightData.m_fAttenuation1		= 0.0f;
						lightData.m_fAttenuation2		= 0.0f;
						m_pLight[i]->SetVertexLightingData( 0, lightData );
					}
					break;
			}
		}

		//
		// Create visual objects
		//
		IGNU32 u32PolyCount;
		structVertexFormat* pSrc;
		for( i = constObjectCount-1; i>=0; i-- )	
		{


// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
//
//       Some hot coder graphics ....
//			(Yep, it's much easier to import a little scene ... but some of us like to be a masochist ;-))
//
//
/**/
/**/
/**/		switch( i )
/**/		{
/**/		case	0:
/**/
/**/				//
/**/				// The bottom
/**/				//
/**/
/**/				u32PolyCount = 20*20*4*2;
/**/				m_aryVisualObject[i].m_ObjectSphere.m_ObjectStandardCenterVector.m_fX	= 
/**/				m_aryVisualObject[i].m_ObjectSphere.m_ObjectCurrentCenterVector.m_fX	= 0.0f;
/**/				m_aryVisualObject[i].m_ObjectSphere.m_ObjectStandardCenterVector.m_fY	= 
/**/				m_aryVisualObject[i].m_ObjectSphere.m_ObjectCurrentCenterVector.m_fY	= 0.0f;
/**/				m_aryVisualObject[i].m_ObjectSphere.m_ObjectStandardCenterVector.m_fZ	= 
/**/				m_aryVisualObject[i].m_ObjectSphere.m_ObjectCurrentCenterVector.m_fZ	= 400.0f;
/**/				m_aryVisualObject[i].m_ObjectSphere.m_fObjectRadius						= 360.0f;
/**/				m_aryVisualObject[i].m_ObjectSphere.m_fBiasZ							= 10.0f;
/**/				a_pGfxContextInterface->CreateVertexData( m_aryVisualObject[i].m_pVertexData );
/**/				pSrc = new structVertexFormat[u32PolyCount*2];
/**/				if( NULL!=m_aryVisualObject[i].m_pVertexData )
/**/				{
/**/					IGN_STRUCT_VertexBufDesc VertexBufferDescription;
/**/					VertexBufferDescription.m_uiLength	= (u32PolyCount*2) * (10*4);
/**/					VertexBufferDescription.m_dwUsage	= IGNUSAGE_WRITEONLY;
/**/					VertexBufferDescription.m_dwFVF		= IGNFVF_XYZ|IGNFVF_NORMAL|IGNFVF_DIFFUSE|IGNFVF_TEX1|IGNFVF_TEXCOORDSIZE3_0;
/**/					VertexBufferDescription.m_Pool		= IGN_ENUM_POOLTYPE_MANAGED;
/**/					m_aryVisualObject[i].m_pVertexData->CreateBuffer( VertexBufferDescription );
/**/					unsigned char* pData = NULL;
/**/					m_aryVisualObject[i].m_pVertexData->LockBuffer( 0, 0, 0, &pData );
/**/					if( (NULL!=pSrc)&&(NULL!=pData) )
/**/					{
/**/						for( IGNU32 j = 0; j<20*20; j++)
/**/						{
/**/							float fCX	= 237.5f - (j%20)*25.0f;
/**/							float fCY	= 237.5f - (j/20)*25.0f;
/**/							(pSrc+j*16+0)->X		= fCX;			(pSrc+j*16+4)->X		= fCX;			(pSrc+j*16+8)->X		= fCX - 12.5f;	(pSrc+j*16+12)->X		= fCX - 12.5f;
/**/							(pSrc+j*16+0)->Y		= fCY;			(pSrc+j*16+4)->Y		= fCY - 12.5f;	(pSrc+j*16+8)->Y		= fCY;			(pSrc+j*16+12)->Y		= fCY - 12.5f;
/**/							(pSrc+j*16+0)->Z		= 400.0f;		(pSrc+j*16+4)->Z		= 400.0f;		(pSrc+j*16+8)->Z		= 400.0f;		(pSrc+j*16+12)->Z		= 400.0f;
/**/							(pSrc+j*16+0)->NX		= 0.0f;			(pSrc+j*16+4)->NX		= 0.0f;			(pSrc+j*16+8)->NX		= 0.0f;			(pSrc+j*16+12)->NX		= 0.0f;
/**/							(pSrc+j*16+0)->NY		= 0.0f;			(pSrc+j*16+4)->NY		= 0.0f;			(pSrc+j*16+8)->NY		= 0.0f;			(pSrc+j*16+12)->NY		= 0.0f;
/**/							(pSrc+j*16+0)->NZ		= -1.0f;		(pSrc+j*16+4)->NZ		= -1.0f;		(pSrc+j*16+8)->NZ		= -1.0f;		(pSrc+j*16+12)->NZ		= -1.0f;
/**/							(pSrc+j*16+0)->COLOR	= 0xffffff00;	(pSrc+j*16+4)->COLOR	= 0xffff00ff;	(pSrc+j*16+8)->COLOR	= 0xff00ffff;	(pSrc+j*16+12)->COLOR	= 0xffffffff;
/**/							(pSrc+j*16+1)->X		= fCX + 12.5f;	(pSrc+j*16+5)->X		= fCX + 12.5f;	(pSrc+j*16+9)->X		= fCX;			(pSrc+j*16+13)->X		= fCX;
/**/							(pSrc+j*16+1)->Y		= fCY;			(pSrc+j*16+5)->Y		= fCY - 12.5f;	(pSrc+j*16+9)->Y		= fCY;			(pSrc+j*16+13)->Y		= fCY - 12.5f;
/**/							(pSrc+j*16+1)->Z		= 400.0f;		(pSrc+j*16+5)->Z		= 400.0f;		(pSrc+j*16+9)->Z		= 400.0f;		(pSrc+j*16+13)->Z		= 400.0f;
/**/							(pSrc+j*16+1)->NX		= 0.0f;			(pSrc+j*16+5)->NX		= 0.0f;			(pSrc+j*16+9)->NX		= 0.0f;			(pSrc+j*16+13)->NX		= 0.0f;
/**/							(pSrc+j*16+1)->NY		= 0.0f;			(pSrc+j*16+5)->NY		= 0.0f;			(pSrc+j*16+9)->NY		= 0.0f;			(pSrc+j*16+13)->NY		= 0.0f;
/**/							(pSrc+j*16+1)->NZ		= -1.0f;		(pSrc+j*16+5)->NZ		= -1.0f;		(pSrc+j*16+9)->NZ		= -1.0f;		(pSrc+j*16+13)->NZ		= -1.0f;
/**/							(pSrc+j*16+1)->COLOR	= 0xffffff00;	(pSrc+j*16+5)->COLOR	= 0xffff00ff;	(pSrc+j*16+9)->COLOR	= 0xff00ffff;	(pSrc+j*16+13)->COLOR	= 0xffffffff;
/**/							(pSrc+j*16+2)->X		= fCX + 12.5f;	(pSrc+j*16+6)->X		= fCX + 12.5f;	(pSrc+j*16+10)->X		= fCX;			(pSrc+j*16+14)->X		= fCX;
/**/							(pSrc+j*16+2)->Y		= fCY + 12.5f;	(pSrc+j*16+6)->Y		= fCY;			(pSrc+j*16+10)->Y		= fCY + 12.5f;	(pSrc+j*16+14)->Y		= fCY;
/**/							(pSrc+j*16+2)->Z		= 400.0f;		(pSrc+j*16+6)->Z		= 400.0f;		(pSrc+j*16+10)->Z		= 400.0f;		(pSrc+j*16+14)->Z		= 400.0f;
/**/							(pSrc+j*16+2)->NX		= 0.0f;			(pSrc+j*16+6)->NX		= 0.0f;			(pSrc+j*16+10)->NX		= 0.0f;			(pSrc+j*16+14)->NX		= 0.0f;
/**/							(pSrc+j*16+2)->NY		= 0.0f;			(pSrc+j*16+6)->NY		= 0.0f;			(pSrc+j*16+10)->NY		= 0.0f;			(pSrc+j*16+14)->NY		= 0.0f;
/**/							(pSrc+j*16+2)->NZ		= -1.0f;		(pSrc+j*16+6)->NZ		= -1.0f;		(pSrc+j*16+10)->NZ		= -1.0f;		(pSrc+j*16+14)->NZ		= -1.0f;
/**/							(pSrc+j*16+2)->COLOR	= 0xffffff00;	(pSrc+j*16+6)->COLOR	= 0xffff00ff;	(pSrc+j*16+10)->COLOR	= 0xff00ffff;	(pSrc+j*16+14)->COLOR	= 0xffffffff;
/**/							(pSrc+j*16+3)->X		= fCX;			(pSrc+j*16+7)->X		= fCX;			(pSrc+j*16+11)->X		= fCX - 12.5f;	(pSrc+j*16+15)->X		= fCX - 12.5f;
/**/							(pSrc+j*16+3)->Y		= fCY + 12.5f;	(pSrc+j*16+7)->Y		= fCY;			(pSrc+j*16+11)->Y		= fCY + 12.5f;	(pSrc+j*16+15)->Y		= fCY;
/**/							(pSrc+j*16+3)->Z		= 400.0f;		(pSrc+j*16+7)->Z		= 400.0f;		(pSrc+j*16+11)->Z		= 400.0f;		(pSrc+j*16+15)->Z		= 400.0f;
/**/							(pSrc+j*16+3)->NX		= 0.0f;			(pSrc+j*16+7)->NX		= 0.0f;			(pSrc+j*16+11)->NX		= 0.0f;			(pSrc+j*16+15)->NX		= 0.0f;
/**/							(pSrc+j*16+3)->NY		= 0.0f;			(pSrc+j*16+7)->NY		= 0.0f;			(pSrc+j*16+11)->NY		= 0.0f;			(pSrc+j*16+15)->NY		= 0.0f;
/**/							(pSrc+j*16+3)->NZ		= -1.0f;		(pSrc+j*16+7)->NZ		= -1.0f;		(pSrc+j*16+11)->NZ		= -1.0f;		(pSrc+j*16+15)->NZ		= -1.0f;
/**/							(pSrc+j*16+3)->COLOR	= 0xffffff00;	(pSrc+j*16+7)->COLOR	= 0xffff00ff;	(pSrc+j*16+11)->COLOR	= 0xff00ffff;	(pSrc+j*16+15)->COLOR	= 0xffffffff;
/**/						}
/**/						memcpy( pData, pSrc, (u32PolyCount*2) * (10*4) );
/**/					}
/**/					m_aryVisualObject[i].m_pVertexData->UnlockBuffer();
/**/					m_aryVisualObject[i].m_pVertexData->SetStreamNumber( 0 );
/**/				}
/**/				a_pGfxContextInterface->CreateIndexData( m_aryVisualObject[i].m_pIndexData );
/**/				if( NULL!=m_aryVisualObject[i].m_pIndexData )
/**/				{
/**/					m_aryVisualObject[i].m_u32IndexCount = u32PolyCount*3;
/**/					IGN_STRUCT_IndexBufDesc IndexBufferDescription;
/**/					IndexBufferDescription.m_uiLength	= m_aryVisualObject[i].m_u32IndexCount*4;
/**/					IndexBufferDescription.m_dwUsage	= IGNUSAGE_WRITEONLY;
/**/					IndexBufferDescription.m_Format		= IGN_ENUM_INDEXFORMAT_INDEX32;
/**/					IndexBufferDescription.m_Pool		= IGN_ENUM_POOLTYPE_MANAGED;
/**/					m_aryVisualObject[i].m_pIndexData->CreateBuffer( IndexBufferDescription );
/**/					unsigned char* pData = NULL;
/**/					m_aryVisualObject[i].m_pIndexData->LockBuffer( 0, 0, 0, &pData );
/**/					IGNU32 u32Data[6*20*20*4];
/**/					if( NULL!=pData )
/**/					{
/**/						for( IGNU32 j = 0; j<m_aryVisualObject[i].m_u32IndexCount/6; j++ )
/**/						{
/**/							u32Data[j*6 + 0] = j*4 + 0;
/**/							u32Data[j*6 + 1] = j*4 + 1;
/**/							u32Data[j*6 + 2] = j*4 + 2;
/**/							u32Data[j*6 + 3] = j*4 + 0;
/**/							u32Data[j*6 + 4] = j*4 + 2;
/**/							u32Data[j*6 + 5] = j*4 + 3;
/**/						}
/**/						memcpy( pData, u32Data, m_aryVisualObject[i].m_u32IndexCount*4 );
/**/					}
/**/					m_aryVisualObject[i].m_pIndexData->UnlockBuffer();
/**/					m_aryVisualObject[i].m_pIndexBufferForSilhouetteDetection = new IGNU32[m_aryVisualObject[i].m_u32IndexCount];
/**/					memcpy( m_aryVisualObject[i].m_pIndexBufferForSilhouetteDetection, u32Data, m_aryVisualObject[i].m_u32IndexCount*4 );
/**/					IGNU32 u32IndexJ = 0;
/**/					IGNU32 u32IndexK = 0;
/**/					for( IGNU32 j = 0; j<m_aryVisualObject[i].m_u32IndexCount; j++ )
/**/					{
/**/						for( IGNU32 k = j+1; k<m_aryVisualObject[i].m_u32IndexCount; k++ )
/**/						{
/**/ 							if( IGN_ENUM_INDEXFORMAT_INDEX32==IndexBufferDescription.m_Format )
/**/							{
/**/								if( (u32IndexJ!=u32IndexK)&&
/**/									((reinterpret_cast<IGN_STRUCT_3DVector*>( pSrc+u32IndexJ))->m_fX==(reinterpret_cast<IGN_STRUCT_3DVector*>( pSrc+u32IndexK))->m_fX)&&
/**/									((reinterpret_cast<IGN_STRUCT_3DVector*>( pSrc+u32IndexJ))->m_fY==(reinterpret_cast<IGN_STRUCT_3DVector*>( pSrc+u32IndexK))->m_fY)&&
/**/									((reinterpret_cast<IGN_STRUCT_3DVector*>( pSrc+u32IndexJ))->m_fZ==(reinterpret_cast<IGN_STRUCT_3DVector*>( pSrc+u32IndexK))->m_fZ) )
/**/									*((m_aryVisualObject[i].m_pIndexBufferForSilhouetteDetection)+k) = u32IndexJ;
/**/							}
/**/						}
/**/					}
/**/				}
/**/				a_pGfxContextInterface->CreateTextureBlock( m_aryVisualObject[i].m_pTextureBlock0 );
/**/				if( NULL!=m_aryVisualObject[i].m_pTextureBlock0 )
/**/				{
/**/					m_aryVisualObject[i].m_pTextureBlock0->SetColorCalc( IGN_ENUM_TEXOPERATION_SELECTARG2, IGN_ENUM_TEXARGUMENT_COLORFACTOR, IGN_ENUM_TEXARGUMENT_DIFFUSE );
/**/					m_aryVisualObject[i].m_pTextureBlock0->SetAlphaCalc( IGN_ENUM_TEXOPERATION_SELECTARG1, IGN_ENUM_TEXARGUMENT_COLORFACTOR, IGN_ENUM_TEXARGUMENT_DIFFUSE );
/**/				}
/**/				a_pGfxContextInterface->CreateTextureBlock( m_aryVisualObject[i].m_pTextureBlock1 );
/**/				if( NULL!=m_aryVisualObject[i].m_pTextureBlock1 )
/**/				{
/**/					m_aryVisualObject[i].m_pTextureBlock1->SetColorCalc( IGN_ENUM_TEXOPERATION_DISABLE, IGN_ENUM_TEXARGUMENT_COLORFACTOR, IGN_ENUM_TEXARGUMENT_DIFFUSE );
/**/					m_aryVisualObject[i].m_pTextureBlock1->SetAlphaCalc( IGN_ENUM_TEXOPERATION_DISABLE, IGN_ENUM_TEXARGUMENT_COLORFACTOR, IGN_ENUM_TEXARGUMENT_DIFFUSE );
/**/				}
/**/				a_pGfxContextInterface->CreateMaterial( m_aryVisualObject[i].m_pMaterial );
/**/				if( NULL!=m_aryVisualObject[i].m_pMaterial )
/**/				{
/**/					m_aryVisualObject[i].m_pMaterial->SetAmbientMaterialSource( IGN_ENUM_MATERIALCOLORSRC_COLOR1 );
/**/					m_aryVisualObject[i].m_pMaterial->AttachTextureBlock( 0, m_aryVisualObject[i].m_pTextureBlock0 );
/**/					m_aryVisualObject[i].m_pMaterial->AttachTextureBlock( 1, m_aryVisualObject[i].m_pTextureBlock1 );
/**/				}
/**/				a_pGfxContextInterface->CreateShader( m_aryVisualObject[i].m_pShader );
/**/				if( NULL!=m_aryVisualObject[i].m_pShader )
/**/				{
/**/					m_aryVisualObject[i].m_pShader->AttachMaterial( m_aryVisualObject[i].m_pMaterial );
/**/				}
/**/				a_pGfxContextInterface->CreateVisual( m_aryVisualObject[i].m_pVisual );
/**/				if( NULL!=m_aryVisualObject[i].m_pVisual )
/**/				{
/**/					IGN_STRUCT_DrawCallDesc	DrawCallDesc;
/**/					DrawCallDesc.m_bIndexedDrawCall		= true;
/**/					DrawCallDesc.m_PrimitiveType		= IGN_ENUM_PRIMITIVETYPE_TRIANGLELIST;
/**/					DrawCallDesc.m_u32PrimitiveCount	= u32PolyCount;
/**/					DrawCallDesc.m_u32StartVertex		= 0;
/**/					DrawCallDesc.m_u32MinIndex			= 0;
/**/					DrawCallDesc.m_u32NumberOfVertices	= u32PolyCount*2;
/**/					DrawCallDesc.m_u32StartIndex		= 0;
/**/					DrawCallDesc.m_u32BaseVertexIndex	= 0;
/**/					m_aryVisualObject[i].m_pVisual->SetDrawCallDescription( DrawCallDesc );
/**/					m_aryVisualObject[i].m_pVisual->SetDepthTesting( IGN_ENUM_CMPFUNC_LESSEQUAL );
/**/					m_aryVisualObject[i].m_pVisual->SetCullingMode( IGN_ENUM_CULLINGMODE_CW );
/**/					m_aryVisualObject[i].m_pVisual->AttachVertexData( m_aryVisualObject[i].m_pVertexData );
/**/					m_aryVisualObject[i].m_pVisual->AttachIndexData( m_aryVisualObject[i].m_pIndexData );
/**/					m_aryVisualObject[i].m_pVisual->AttachShader( m_aryVisualObject[i].m_pShader );
/**/				}
/**/				delete[] pSrc;
/**/				break;
/**/		case	1:
/**/
/**/				//
/**/				// The blue object (object No.1)
/**/				//
/**/
/**/				u32PolyCount = 6*2;
/**/				m_aryVisualObject[i].m_ObjectSphere.m_ObjectStandardCenterVector.m_fX	= 
/**/				m_aryVisualObject[i].m_ObjectSphere.m_ObjectCurrentCenterVector.m_fX	= 0.0f;
/**/				m_aryVisualObject[i].m_ObjectSphere.m_ObjectStandardCenterVector.m_fY	= 
/**/				m_aryVisualObject[i].m_ObjectSphere.m_ObjectCurrentCenterVector.m_fY	= 0.0f;
/**/				m_aryVisualObject[i].m_ObjectSphere.m_ObjectStandardCenterVector.m_fZ	= 
/**/				m_aryVisualObject[i].m_ObjectSphere.m_ObjectCurrentCenterVector.m_fZ	= 345.0f;
/**/				m_aryVisualObject[i].m_ObjectSphere.m_fObjectRadius						= 150.0f;
/**/				m_aryVisualObject[i].m_ObjectSphere.m_fBiasZ							= 10.0f;
/**/				a_pGfxContextInterface->CreateVertexData( m_aryVisualObject[i].m_pVertexData );
/**/				pSrc = new structVertexFormat[u32PolyCount*2];
/**/				if( NULL!=m_aryVisualObject[i].m_pVertexData )
/**/				{
/**/					IGN_STRUCT_VertexBufDesc VertexBufferDescription;
/**/					VertexBufferDescription.m_uiLength	= (u32PolyCount*2) * (10*4);
/**/					VertexBufferDescription.m_dwUsage	= IGNUSAGE_WRITEONLY;
/**/					VertexBufferDescription.m_dwFVF		= IGNFVF_XYZ|IGNFVF_NORMAL|IGNFVF_DIFFUSE|IGNFVF_TEX1|IGNFVF_TEXCOORDSIZE3_0;
/**/					VertexBufferDescription.m_Pool		= IGN_ENUM_POOLTYPE_MANAGED;
/**/					m_aryVisualObject[i].m_pVertexData->CreateBuffer( VertexBufferDescription );
/**/					unsigned char* pData = NULL;
/**/					m_aryVisualObject[i].m_pVertexData->LockBuffer( 0, 0, 0, &pData );
/**/					if( (NULL!=pSrc)&&(NULL!=pData) )
/**/					{
/**/						(pSrc+0)->X		= -100.0f;		(pSrc+4)->X		= -100.0f;		(pSrc+8)->X		= -100.0f;		(pSrc+12)->X	= -100.0f;		(pSrc+16)->X	= 100.0f;		(pSrc+20)->X	= -100.0f;
/**/						(pSrc+0)->Y		= -100.0f;		(pSrc+4)->Y		= -100.0f;		(pSrc+8)->Y		= 100.0f;		(pSrc+12)->Y	= -100.0f;		(pSrc+16)->Y	= -100.0f;		(pSrc+20)->Y	= -100.0f;
/**/						(pSrc+0)->Z		= 340.0f;		(pSrc+4)->Z		= 350.0f;		(pSrc+8)->Z		= 340.0f;		(pSrc+12)->Z	= 340.0f;		(pSrc+16)->Z	= 340.0f;		(pSrc+20)->Z	= 340.0f;
/**/						(pSrc+0)->NX	= 0.0f;			(pSrc+4)->NX	= 0.0f;			(pSrc+8)->NX	= 0.0f;			(pSrc+12)->NX	= 0.0f;			(pSrc+16)->NX	= 1.0f;			(pSrc+20)->NX	= -1.0f;
/**/						(pSrc+0)->NY	= 0.0f;			(pSrc+4)->NY	= 0.0f;			(pSrc+8)->NY	= 1.0f;			(pSrc+12)->NY	= -1.0f;		(pSrc+16)->NY	= 0.0f;			(pSrc+20)->NY	= 0.0f;
/**/						(pSrc+0)->NZ	= -1.0f;		(pSrc+4)->NZ	= 1.0f;			(pSrc+8)->NZ	= 0.0f;			(pSrc+12)->NZ	= 0.0f;			(pSrc+16)->NZ	= 0.0f;			(pSrc+20)->NZ	= 0.0f;
/**/						(pSrc+0)->COLOR	= 0xff0000ff;	(pSrc+4)->COLOR	= 0xff0000ff;	(pSrc+8)->COLOR	= 0xff0000ff;	(pSrc+12)->COLOR= 0xff0000ff;	(pSrc+16)->COLOR= 0xff0000ff;	(pSrc+20)->COLOR= 0xff0000ff;
/**/						(pSrc+1)->X		= 100.0f;		(pSrc+5)->X		= -100.0f;		(pSrc+9)->X		= 100.0f;		(pSrc+13)->X	= -100.0f;		(pSrc+17)->X	= 100.0f;		(pSrc+21)->X	= -100.0f;
/**/						(pSrc+1)->Y		= -100.0f;		(pSrc+5)->Y		= 100.0f;		(pSrc+9)->Y		= 100.0f;		(pSrc+13)->Y	= -100.0f;		(pSrc+17)->Y	= -100.0f;		(pSrc+21)->Y	= 100.0f;
/**/						(pSrc+1)->Z		= 340.0f;		(pSrc+5)->Z		= 350.0f;		(pSrc+9)->Z		= 340.0f;		(pSrc+13)->Z	= 350.0f;		(pSrc+17)->Z	= 350.0f;		(pSrc+21)->Z	= 340.0f;
/**/						(pSrc+1)->NX	= 0.0f;			(pSrc+5)->NX	= 0.0f;			(pSrc+9)->NX	= 0.0f;			(pSrc+13)->NX	= 0.0f;			(pSrc+17)->NX	= 1.0f;			(pSrc+21)->NX	= -1.0f;
/**/						(pSrc+1)->NY	= 0.0f;			(pSrc+5)->NY	= 0.0f;			(pSrc+9)->NY	= 1.0f;			(pSrc+13)->NY	= -1.0f;		(pSrc+17)->NY	= 0.0f;			(pSrc+21)->NY	= 0.0f;
/**/						(pSrc+1)->NZ	= -1.0f;		(pSrc+5)->NZ	= 1.0f;			(pSrc+9)->NZ	= 0.0f;			(pSrc+13)->NZ	= 0.0f;			(pSrc+17)->NZ	= 0.0f;			(pSrc+21)->NZ	= 0.0f;
/**/						(pSrc+1)->COLOR	= 0xff0000ff;	(pSrc+5)->COLOR	= 0xff0000ff;	(pSrc+9)->COLOR	= 0xff0000ff;	(pSrc+13)->COLOR= 0xff0000ff;	(pSrc+17)->COLOR= 0xff0000ff;	(pSrc+21)->COLOR= 0xff0000ff;
/**/						(pSrc+2)->X		= 100.0f;		(pSrc+6)->X		= 100.0f;		(pSrc+10)->X	= 100.0f;		(pSrc+14)->X	= 100.0f;		(pSrc+18)->X	= 100.0f;		(pSrc+22)->X	= -100.0f;
/**/						(pSrc+2)->Y		= 100.0f;		(pSrc+6)->Y		= 100.0f;		(pSrc+10)->Y	= 100.0f;		(pSrc+14)->Y	= -100.0f;		(pSrc+18)->Y	= 100.0f;		(pSrc+22)->Y	= 100.0f;
/**/						(pSrc+2)->Z		= 340.0f;		(pSrc+6)->Z		= 350.0f;		(pSrc+10)->Z	= 350.0f;		(pSrc+14)->Z	= 350.0f;		(pSrc+18)->Z	= 350.0f;		(pSrc+22)->Z	= 350.0f;
/**/						(pSrc+2)->NX	= 0.0f;			(pSrc+6)->NX	= 0.0f;			(pSrc+10)->NX	= 0.0f;			(pSrc+14)->NX	= 0.0f;			(pSrc+18)->NX	= 1.0f;			(pSrc+22)->NX	= -1.0f;
/**/						(pSrc+2)->NY	= 0.0f;			(pSrc+6)->NY	= 0.0f;			(pSrc+10)->NY	= 1.0f;			(pSrc+14)->NY	= -1.0f;		(pSrc+18)->NY	= 0.0f;			(pSrc+22)->NY	= 0.0f;
/**/						(pSrc+2)->NZ	= -1.0f;		(pSrc+6)->NZ	= 1.0f;			(pSrc+10)->NZ	= 0.0f;			(pSrc+14)->NZ	= 0.0f;			(pSrc+18)->NZ	= 0.0f;			(pSrc+22)->NZ	= 0.0f;
/**/						(pSrc+2)->COLOR	= 0xff0000ff;	(pSrc+6)->COLOR	= 0xff0000ff;	(pSrc+10)->COLOR= 0xff0000ff;	(pSrc+14)->COLOR= 0xff0000ff;	(pSrc+18)->COLOR= 0xff0000ff;	(pSrc+22)->COLOR= 0xff0000ff;
/**/						(pSrc+3)->X		= -100.0f;		(pSrc+7)->X		= 100.0f;		(pSrc+11)->X	= -100.0f;		(pSrc+15)->X	= 100.0f;		(pSrc+19)->X	= 100.0f;		(pSrc+23)->X	= -100.0f;
/**/						(pSrc+3)->Y		= 100.0f;		(pSrc+7)->Y		= -100.0f;		(pSrc+11)->Y	= 100.0f;		(pSrc+15)->Y	= -100.0f;		(pSrc+19)->Y	= 100.0f;		(pSrc+23)->Y	= -100.0f;
/**/						(pSrc+3)->Z		= 340.0f;		(pSrc+7)->Z		= 350.0f;		(pSrc+11)->Z	= 350.0f;		(pSrc+15)->Z	= 340.0f;		(pSrc+19)->Z	= 340.0f;		(pSrc+23)->Z	= 350.0f;
/**/						(pSrc+3)->NX	= 0.0f;			(pSrc+7)->NX	= 0.0f;			(pSrc+11)->NX	= 0.0f;			(pSrc+15)->NX	= 0.0f;			(pSrc+19)->NX	= 1.0f;			(pSrc+23)->NX	= -1.0f;
/**/						(pSrc+3)->NY	= 0.0f;			(pSrc+7)->NY	= 0.0f;			(pSrc+11)->NY	= 1.0f;			(pSrc+15)->NY	= -1.0f;		(pSrc+19)->NY	= 0.0f;			(pSrc+23)->NY	= 0.0f;
/**/						(pSrc+3)->NZ	= -1.0f;		(pSrc+7)->NZ	= 1.0f;			(pSrc+11)->NZ	= 0.0f;			(pSrc+15)->NZ	= 0.0f;			(pSrc+19)->NZ	= 0.0f;			(pSrc+23)->NZ	= 0.0f;
/**/						(pSrc+3)->COLOR	= 0xff0000ff;	(pSrc+7)->COLOR	= 0xff0000ff;	(pSrc+11)->COLOR= 0xff0000ff;	(pSrc+15)->COLOR= 0xff0000ff;	(pSrc+19)->COLOR= 0xff0000ff;	(pSrc+23)->COLOR= 0xff0000ff;
/**/						memcpy( pData, pSrc, (u32PolyCount*2) * (10*4) );
/**/					}
/**/					m_aryVisualObject[i].m_pVertexData->UnlockBuffer();
/**/					m_aryVisualObject[i].m_pVertexData->SetStreamNumber( 0 );
/**/				}
/**/				a_pGfxContextInterface->CreateIndexData( m_aryVisualObject[i].m_pIndexData );
/**/				if( NULL!=m_aryVisualObject[i].m_pIndexData )
/**/				{
/**/					m_aryVisualObject[i].m_u32IndexCount = u32PolyCount*3;
/**/					IGN_STRUCT_IndexBufDesc IndexBufferDescription;
/**/					IndexBufferDescription.m_uiLength	= m_aryVisualObject[i].m_u32IndexCount*4;
/**/					IndexBufferDescription.m_dwUsage	= IGNUSAGE_WRITEONLY;
/**/					IndexBufferDescription.m_Format		= IGN_ENUM_INDEXFORMAT_INDEX32;
/**/					IndexBufferDescription.m_Pool		= IGN_ENUM_POOLTYPE_MANAGED;
/**/					m_aryVisualObject[i].m_pIndexData->CreateBuffer( IndexBufferDescription );
/**/					unsigned char* pData = NULL;
/**/					m_aryVisualObject[i].m_pIndexData->LockBuffer( 0, 0, 0, &pData );
/**/					IGNU32 u32Data[36] = { 0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23 };
/**/					if( NULL!=pData )
/**/					{
/**/						memcpy( pData, u32Data, m_aryVisualObject[i].m_u32IndexCount*4);
/**/					}
/**/					m_aryVisualObject[i].m_pIndexData->UnlockBuffer();
/**/					m_aryVisualObject[i].m_pIndexBufferForSilhouetteDetection = new IGNU32[m_aryVisualObject[i].m_u32IndexCount];
/**/					memcpy( m_aryVisualObject[i].m_pIndexBufferForSilhouetteDetection, u32Data, m_aryVisualObject[i].m_u32IndexCount*4 );
/**/					IGNU32 u32IndexJ = 0;
/**/					IGNU32 u32IndexK = 0;
/**/					for( IGNU32 j = 0; j<m_aryVisualObject[i].m_u32IndexCount; j++ )
/**/					{
/**/						for( IGNU32 k = j+1; k<m_aryVisualObject[i].m_u32IndexCount; k++ )
/**/						{
/**/							u32IndexJ = u32Data[j];
/**/							u32IndexK = u32Data[k];
/**/ 							if( IGN_ENUM_INDEXFORMAT_INDEX32==IndexBufferDescription.m_Format )
/**/							{
/**/								if( (u32IndexJ!=u32IndexK)&&
/**/									((reinterpret_cast<IGN_STRUCT_3DVector*>( pSrc+u32IndexJ))->m_fX==(reinterpret_cast<IGN_STRUCT_3DVector*>( pSrc+u32IndexK))->m_fX)&&
/**/									((reinterpret_cast<IGN_STRUCT_3DVector*>( pSrc+u32IndexJ))->m_fY==(reinterpret_cast<IGN_STRUCT_3DVector*>( pSrc+u32IndexK))->m_fY)&&
/**/									((reinterpret_cast<IGN_STRUCT_3DVector*>( pSrc+u32IndexJ))->m_fZ==(reinterpret_cast<IGN_STRUCT_3DVector*>( pSrc+u32IndexK))->m_fZ) )
/**/									*((m_aryVisualObject[i].m_pIndexBufferForSilhouetteDetection)+k) = u32IndexJ;
/**/							}
/**/						}
/**/					}
/**/				}
/**/				a_pGfxContextInterface->CreateTextureBlock( m_aryVisualObject[i].m_pTextureBlock0 );
/**/				if( NULL!=m_aryVisualObject[i].m_pTextureBlock0 )
/**/				{
/**/					m_aryVisualObject[i].m_pTextureBlock0->SetColorCalc( IGN_ENUM_TEXOPERATION_SELECTARG2, IGN_ENUM_TEXARGUMENT_COLORFACTOR, IGN_ENUM_TEXARGUMENT_DIFFUSE );
/**/					m_aryVisualObject[i].m_pTextureBlock0->SetAlphaCalc( IGN_ENUM_TEXOPERATION_SELECTARG1, IGN_ENUM_TEXARGUMENT_COLORFACTOR, IGN_ENUM_TEXARGUMENT_DIFFUSE );
/**/				}
/**/				a_pGfxContextInterface->CreateTextureBlock( m_aryVisualObject[i].m_pTextureBlock1 );
/**/				if( NULL!=m_aryVisualObject[i].m_pTextureBlock1 )
/**/				{
/**/					m_aryVisualObject[i].m_pTextureBlock1->SetColorCalc( IGN_ENUM_TEXOPERATION_DISABLE, IGN_ENUM_TEXARGUMENT_COLORFACTOR, IGN_ENUM_TEXARGUMENT_DIFFUSE );
/**/					m_aryVisualObject[i].m_pTextureBlock1->SetAlphaCalc( IGN_ENUM_TEXOPERATION_DISABLE, IGN_ENUM_TEXARGUMENT_COLORFACTOR, IGN_ENUM_TEXARGUMENT_DIFFUSE );
/**/				}
/**/				a_pGfxContextInterface->CreateMaterial( m_aryVisualObject[i].m_pMaterial );
/**/				if( NULL!=m_aryVisualObject[i].m_pMaterial )
/**/				{
/**/					m_aryVisualObject[i].m_pMaterial->SetAmbientMaterialSource( IGN_ENUM_MATERIALCOLORSRC_COLOR1 );
/**/					m_aryVisualObject[i].m_pMaterial->AttachTextureBlock( 0, m_aryVisualObject[i].m_pTextureBlock0 );
/**/					m_aryVisualObject[i].m_pMaterial->AttachTextureBlock( 1, m_aryVisualObject[i].m_pTextureBlock1 );
/**/				}
/**/				a_pGfxContextInterface->CreateShader( m_aryVisualObject[i].m_pShader );
/**/				if( NULL!=m_aryVisualObject[i].m_pShader )
/**/				{
/**/					m_aryVisualObject[i].m_pShader->AttachMaterial( m_aryVisualObject[i].m_pMaterial );
/**/				}
/**/				a_pGfxContextInterface->CreateVisual( m_aryVisualObject[i].m_pVisual );
/**/				if( NULL!=m_aryVisualObject[i].m_pVisual )
/**/				{
/**/					IGN_STRUCT_DrawCallDesc	DrawCallDesc;
/**/					DrawCallDesc.m_bIndexedDrawCall		= true;
/**/					DrawCallDesc.m_PrimitiveType		= IGN_ENUM_PRIMITIVETYPE_TRIANGLELIST;
/**/					DrawCallDesc.m_u32PrimitiveCount	= u32PolyCount;
/**/					DrawCallDesc.m_u32StartVertex		= 0;
/**/					DrawCallDesc.m_u32MinIndex			= 0;
/**/					DrawCallDesc.m_u32NumberOfVertices	= u32PolyCount*2;
/**/					DrawCallDesc.m_u32StartIndex		= 0;
/**/					DrawCallDesc.m_u32BaseVertexIndex	= 0;
/**/					m_aryVisualObject[i].m_pVisual->SetDrawCallDescription( DrawCallDesc );
/**/					m_aryVisualObject[i].m_pVisual->SetDepthTesting( IGN_ENUM_CMPFUNC_LESSEQUAL );
/**/					m_aryVisualObject[i].m_pVisual->SetCullingMode( IGN_ENUM_CULLINGMODE_CW );
/**/					m_aryVisualObject[i].m_pVisual->AttachVertexData( m_aryVisualObject[i].m_pVertexData );
/**/					m_aryVisualObject[i].m_pVisual->AttachIndexData( m_aryVisualObject[i].m_pIndexData );
/**/					m_aryVisualObject[i].m_pVisual->AttachShader( m_aryVisualObject[i].m_pShader );
/**/				}
/**/				delete[] pSrc;
/**/				break;
/**/		case	2:
/**/
/**/				//
/**/				// The red object (object No.2)
/**/				//
/**/
/**/				u32PolyCount = 8*2;
/**/				m_aryVisualObject[i].m_ObjectSphere.m_ObjectStandardCenterVector.m_fX	= 
/**/				m_aryVisualObject[i].m_ObjectSphere.m_ObjectCurrentCenterVector.m_fX	= 30.0f;
/**/				m_aryVisualObject[i].m_ObjectSphere.m_ObjectStandardCenterVector.m_fY	= 
/**/				m_aryVisualObject[i].m_ObjectSphere.m_ObjectCurrentCenterVector.m_fY	= 30.0f;
/**/				m_aryVisualObject[i].m_ObjectSphere.m_ObjectStandardCenterVector.m_fZ	= 
/**/				m_aryVisualObject[i].m_ObjectSphere.m_ObjectCurrentCenterVector.m_fZ	= 325.0f;
/**/				m_aryVisualObject[i].m_ObjectSphere.m_fObjectRadius						= 220.0f;
/**/				m_aryVisualObject[i].m_ObjectSphere.m_fBiasZ							= 1.5f;
/**/				a_pGfxContextInterface->CreateVertexData( m_aryVisualObject[i].m_pVertexData );
/**/				pSrc = new structVertexFormat[u32PolyCount*2];
/**/				if( NULL!=m_aryVisualObject[i].m_pVertexData )
/**/				{
/**/					IGN_STRUCT_VertexBufDesc VertexBufferDescription;
/**/					VertexBufferDescription.m_uiLength	= (u32PolyCount*2) * (10*4);
/**/					VertexBufferDescription.m_dwUsage	= IGNUSAGE_WRITEONLY;
/**/					VertexBufferDescription.m_dwFVF		= IGNFVF_XYZ|IGNFVF_NORMAL|IGNFVF_DIFFUSE|IGNFVF_TEX1|IGNFVF_TEXCOORDSIZE3_0;
/**/					VertexBufferDescription.m_Pool		= IGN_ENUM_POOLTYPE_MANAGED;
/**/					m_aryVisualObject[i].m_pVertexData->CreateBuffer( VertexBufferDescription );
/**/					unsigned char* pData = NULL;
/**/					m_aryVisualObject[i].m_pVertexData->LockBuffer( 0, 0, 0, &pData );
/**/					if( (NULL!=pSrc)&&(NULL!=pData) )
/**/					{
/**/						(pSrc+0)->X		= -90.0f;		(pSrc+4)->X		= 130.0f;		(pSrc+8)->X		= -90.0f;		(pSrc+12)->X	= -90.0f;		(pSrc+16)->X	= -70.0f;		(pSrc+20)->X	= -90.0f;		(pSrc+24)->X	= -65.0f;		(pSrc+28)->X	= -65.0f;			
/**/						(pSrc+0)->Y		= -90.0f;		(pSrc+4)->Y		= 130.0f;		(pSrc+8)->Y		= -70.0f;		(pSrc+12)->Y	= -90.0f;		(pSrc+16)->Y	= -90.0f;		(pSrc+20)->Y	= -90.0f;		(pSrc+24)->Y	= -65.0f;		(pSrc+28)->Y	= -65.0f;		
/**/						(pSrc+0)->Z		= 250.0f;		(pSrc+4)->Z		= 400.0f;		(pSrc+8)->Z		= 250.0f;		(pSrc+12)->Z	= 250.0f;		(pSrc+16)->Z	= 250.0f;		(pSrc+20)->Z	= 250.0f;		(pSrc+24)->Z	= 300.0f;		(pSrc+28)->Z	= 300.0f;		
/**/						(pSrc+0)->NX	= 0.0f;			(pSrc+4)->NX	= 0.0f;			(pSrc+8)->NX	= 0.0f;			(pSrc+12)->NX	= 0.0f;			(pSrc+16)->NX	= 150.f/266.27f;(pSrc+20)->NX	=-150.f/266.27f;(pSrc+24)->NX	= 0.0f;			(pSrc+28)->NX	= 0.0f;			
/**/						(pSrc+0)->NY	= 0.0f;			(pSrc+4)->NY	= 0.0f;			(pSrc+8)->NY	= 150.f/266.27f;(pSrc+12)->NY	=-150.f/266.27f;(pSrc+16)->NY	= 0.0f;			(pSrc+20)->NY	= 0.0f;			(pSrc+24)->NY	= 0.0f;			(pSrc+28)->NY	= 0.0f;			
/**/						(pSrc+0)->NZ	= -1.0f;		(pSrc+4)->NZ	= 1.0f;			(pSrc+8)->NZ	=-220.f/266.27f;(pSrc+12)->NZ	= 220.f/266.27f;(pSrc+16)->NZ	=-220.f/266.27f;(pSrc+20)->NZ	= 220.f/266.27f;(pSrc+24)->NZ	= -1.0f;		(pSrc+28)->NZ	= 1.0f;		
/**/						(pSrc+0)->COLOR	= 0xffff0000;	(pSrc+4)->COLOR	= 0xffff0000;	(pSrc+8)->COLOR	= 0xffff0000;	(pSrc+12)->COLOR= 0xffff0000;	(pSrc+16)->COLOR= 0xffff0000;	(pSrc+20)->COLOR= 0xffff0000;	(pSrc+24)->COLOR= 0xffff0000;	(pSrc+28)->COLOR= 0xffff0000;	
/**/						(pSrc+1)->X		= -70.0f;		(pSrc+5)->X		= 130.0f;		(pSrc+9)->X		= -70.0f;		(pSrc+13)->X	= 130.0f;		(pSrc+17)->X	= 150.0f;		(pSrc+21)->X	= -90.0f;		(pSrc+25)->X	= 15.0f;		(pSrc+29)->X	= -65.0f;		
/**/						(pSrc+1)->Y		= -90.0f;		(pSrc+5)->Y		= 150.0f;		(pSrc+9)->Y		= -70.0f;		(pSrc+13)->Y	= 130.0f;		(pSrc+17)->Y	= 130.0f;		(pSrc+21)->Y	= -70.0f;		(pSrc+25)->Y	= -65.0f;		(pSrc+29)->Y	= 15.0f;		
/**/						(pSrc+1)->Z		= 250.0f;		(pSrc+5)->Z		= 400.0f;		(pSrc+9)->Z		= 250.0f;		(pSrc+13)->Z	= 400.0f;		(pSrc+17)->Z	= 400.0f;		(pSrc+21)->Z	= 250.0f;		(pSrc+25)->Z	= 300.0f;		(pSrc+29)->Z	= 300.0f;		
/**/						(pSrc+1)->NX	= 0.0f;			(pSrc+5)->NX	= 0.0f;			(pSrc+9)->NX	= 0.0f;			(pSrc+13)->NX	= 0.0f;			(pSrc+17)->NX	= 150.f/266.27f;(pSrc+21)->NX	=-150.f/266.27f;(pSrc+25)->NX	= 0.0f;			(pSrc+29)->NX	= 0.0f;			
/**/						(pSrc+1)->NY	= 0.0f;			(pSrc+5)->NY	= 0.0f;			(pSrc+9)->NY	= 150.f/266.27f;(pSrc+13)->NY	=-150.f/266.27f;(pSrc+17)->NY	= 0.0f;			(pSrc+21)->NY	= 0.0f;			(pSrc+25)->NY	= 0.0f;			(pSrc+29)->NY	= 0.0f;			
/**/						(pSrc+1)->NZ	= -1.0f;		(pSrc+5)->NZ	= 1.0f;			(pSrc+9)->NZ	=-220.f/266.27f;(pSrc+13)->NZ	= 220.f/266.27f;(pSrc+17)->NZ	=-220.f/266.27f;(pSrc+21)->NZ	= 220.f/266.27f;(pSrc+25)->NZ	= -1.0f;		(pSrc+29)->NZ	= 1.0f;		
/**/						(pSrc+1)->COLOR	= 0xffff0000;	(pSrc+5)->COLOR	= 0xffff0000;	(pSrc+9)->COLOR	= 0xffff0000;	(pSrc+13)->COLOR= 0xffff0000;	(pSrc+17)->COLOR= 0xffff0000;	(pSrc+21)->COLOR= 0xffff0000;	(pSrc+25)->COLOR= 0xffff0000;	(pSrc+29)->COLOR= 0xffff0000;	
/**/						(pSrc+2)->X		= -70.0f;		(pSrc+6)->X		= 150.0f;		(pSrc+10)->X	= 150.0f;		(pSrc+14)->X	= 150.0f;		(pSrc+18)->X	= 150.0f;		(pSrc+22)->X	= 130.0f;		(pSrc+26)->X	= 15.0f;		(pSrc+30)->X	= 15.0f;		
/**/						(pSrc+2)->Y		= -70.0f;		(pSrc+6)->Y		= 150.0f;		(pSrc+10)->Y	= 150.0f;		(pSrc+14)->Y	= 130.0f;		(pSrc+18)->Y	= 150.0f;		(pSrc+22)->Y	= 150.0f;		(pSrc+26)->Y	= 15.0f;		(pSrc+30)->Y	= 15.0f;		
/**/						(pSrc+2)->Z		= 250.0f;		(pSrc+6)->Z		= 400.0f;		(pSrc+10)->Z	= 400.0f;		(pSrc+14)->Z	= 400.0f;		(pSrc+18)->Z	= 400.0f;		(pSrc+22)->Z	= 400.0f;		(pSrc+26)->Z	= 300.0f;		(pSrc+30)->Z	= 300.0f;		
/**/						(pSrc+2)->NX	= 0.0f;			(pSrc+6)->NX	= 0.0f;			(pSrc+10)->NX	= 0.0f;			(pSrc+14)->NX	= 0.0f;			(pSrc+18)->NX	= 150.f/266.27f;(pSrc+22)->NX	=-150.f/266.27f;(pSrc+26)->NX	= 0.0f;			(pSrc+30)->NX	= 0.0f;			
/**/						(pSrc+2)->NY	= 0.0f;			(pSrc+6)->NY	= 0.0f;			(pSrc+10)->NY	= 150.f/266.27f;(pSrc+14)->NY	=-150.f/266.27f;(pSrc+18)->NY	= 0.0f;			(pSrc+22)->NY	= 0.0f;			(pSrc+26)->NY	= 0.0f;			(pSrc+30)->NY	= 0.0f;			
/**/						(pSrc+2)->NZ	= -1.0f;		(pSrc+6)->NZ	= 1.0f;			(pSrc+10)->NZ	=-220.f/266.27f;(pSrc+14)->NZ	= 220.f/266.27f;(pSrc+18)->NZ	=-220.f/266.27f;(pSrc+22)->NZ	= 220.f/266.27f;(pSrc+26)->NZ	= -1.0f;		(pSrc+30)->NZ	= 1.0f;		
/**/						(pSrc+2)->COLOR	= 0xffff0000;	(pSrc+6)->COLOR	= 0xffff0000;	(pSrc+10)->COLOR= 0xffff0000;	(pSrc+14)->COLOR= 0xffff0000;	(pSrc+18)->COLOR= 0xffff0000;	(pSrc+22)->COLOR= 0xffff0000;	(pSrc+26)->COLOR= 0xffff0000;	(pSrc+30)->COLOR= 0xffff0000;	
/**/						(pSrc+3)->X		= -90.0f;		(pSrc+7)->X		= 150.0f;		(pSrc+11)->X	= 130.0f;		(pSrc+15)->X	= -70.0f;		(pSrc+19)->X	= -70.0f;		(pSrc+23)->X	= 130.0f;		(pSrc+27)->X	= -65.0f;		(pSrc+31)->X	= 15.0f;		
/**/						(pSrc+3)->Y		= -70.0f;		(pSrc+7)->Y		= 130.0f;		(pSrc+11)->Y	= 150.0f;		(pSrc+15)->Y	= -90.0f;		(pSrc+19)->Y	= -70.0f;		(pSrc+23)->Y	= 130.0f;		(pSrc+27)->Y	= 15.0f;		(pSrc+31)->Y	= -65.0f;		
/**/						(pSrc+3)->Z		= 250.0f;		(pSrc+7)->Z		= 400.0f;		(pSrc+11)->Z	= 400.0f;		(pSrc+15)->Z	= 250.0f;		(pSrc+19)->Z	= 250.0f;		(pSrc+23)->Z	= 400.0f;		(pSrc+27)->Z	= 300.0f;		(pSrc+31)->Z	= 300.0f;		
/**/						(pSrc+3)->NX	= 0.0f;			(pSrc+7)->NX	= 0.0f;			(pSrc+11)->NX	= 0.0f;			(pSrc+15)->NX	= 0.0f;			(pSrc+19)->NX	= 150.f/266.27f;(pSrc+23)->NX	=-150.f/266.27f;(pSrc+27)->NX	= 0.0f;			(pSrc+31)->NX	= 0.0f;			
/**/						(pSrc+3)->NY	= 0.0f;			(pSrc+7)->NY	= 0.0f;			(pSrc+11)->NY	= 150.f/266.27f;(pSrc+15)->NY	=-150.f/266.27f;(pSrc+19)->NY	= 0.0f;			(pSrc+23)->NY	= 0.0f;			(pSrc+27)->NY	= 0.0f;			(pSrc+31)->NY	= 0.0f;			
/**/						(pSrc+3)->NZ	= -1.0f;		(pSrc+7)->NZ	= 1.0f;			(pSrc+11)->NZ	=-220.f/266.27f;(pSrc+15)->NZ	= 220.f/266.27f;(pSrc+19)->NZ	=-220.f/266.27f;(pSrc+23)->NZ	= 220.f/266.27f;(pSrc+27)->NZ	= -1.0f;		(pSrc+31)->NZ	= 1.0f;		
/**/						(pSrc+3)->COLOR	= 0xffff0000;	(pSrc+7)->COLOR	= 0xffff0000;	(pSrc+11)->COLOR= 0xffff0000;	(pSrc+15)->COLOR= 0xffff0000;	(pSrc+19)->COLOR= 0xffff0000;	(pSrc+23)->COLOR= 0xffff0000;	(pSrc+27)->COLOR= 0xffff0000;	(pSrc+31)->COLOR= 0xffff0000;	
/**/						memcpy( pData, pSrc, (u32PolyCount*2) * (10*4) );
/**/					}
/**/					m_aryVisualObject[i].m_pVertexData->UnlockBuffer();
/**/					m_aryVisualObject[i].m_pVertexData->SetStreamNumber( 0 );
/**/				}
/**/				a_pGfxContextInterface->CreateIndexData( m_aryVisualObject[i].m_pIndexData );
/**/				if( NULL!=m_aryVisualObject[i].m_pIndexData )
/**/				{
/**/					m_aryVisualObject[i].m_u32IndexCount = u32PolyCount*3;
/**/					IGN_STRUCT_IndexBufDesc IndexBufferDescription;
/**/					IndexBufferDescription.m_uiLength	= m_aryVisualObject[i].m_u32IndexCount*4;
/**/					IndexBufferDescription.m_dwUsage	= IGNUSAGE_WRITEONLY;
/**/					IndexBufferDescription.m_Format		= IGN_ENUM_INDEXFORMAT_INDEX32;
/**/					IndexBufferDescription.m_Pool		= IGN_ENUM_POOLTYPE_MANAGED;
/**/					m_aryVisualObject[i].m_pIndexData->CreateBuffer( IndexBufferDescription );
/**/					unsigned char* pData = NULL;
/**/					m_aryVisualObject[i].m_pIndexData->LockBuffer( 0, 0, 0, &pData );
/**/					IGNU32 u32Data[48] = { 0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23, 24, 25, 26, 24, 26, 27, 28, 29, 30, 28, 30, 31 };
/**/					if( NULL!=pData )
/**/					{
/**/						memcpy( pData, u32Data, m_aryVisualObject[i].m_u32IndexCount*4);
/**/					}
/**/					m_aryVisualObject[i].m_pIndexData->UnlockBuffer();
/**/					m_aryVisualObject[i].m_pIndexBufferForSilhouetteDetection = new IGNU32[m_aryVisualObject[i].m_u32IndexCount];
/**/					memcpy( m_aryVisualObject[i].m_pIndexBufferForSilhouetteDetection, u32Data, m_aryVisualObject[i].m_u32IndexCount*4 );
/**/					IGNU32 u32IndexJ = 0;
/**/					IGNU32 u32IndexK = 0;
/**/					for( IGNU32 j = 0; j<m_aryVisualObject[i].m_u32IndexCount; j++ )
/**/					{
/**/						for( IGNU32 k = j+1; k<m_aryVisualObject[i].m_u32IndexCount; k++ )
/**/						{
/**/							u32IndexJ = u32Data[j];
/**/							u32IndexK = u32Data[k];
/**/ 							if( IGN_ENUM_INDEXFORMAT_INDEX32==IndexBufferDescription.m_Format )
/**/							{
/**/								if( (u32IndexJ!=u32IndexK)&&
/**/									((reinterpret_cast<IGN_STRUCT_3DVector*>( pSrc+u32IndexJ))->m_fX==(reinterpret_cast<IGN_STRUCT_3DVector*>( pSrc+u32IndexK))->m_fX)&&
/**/									((reinterpret_cast<IGN_STRUCT_3DVector*>( pSrc+u32IndexJ))->m_fY==(reinterpret_cast<IGN_STRUCT_3DVector*>( pSrc+u32IndexK))->m_fY)&&
/**/									((reinterpret_cast<IGN_STRUCT_3DVector*>( pSrc+u32IndexJ))->m_fZ==(reinterpret_cast<IGN_STRUCT_3DVector*>( pSrc+u32IndexK))->m_fZ) )
/**/									*((m_aryVisualObject[i].m_pIndexBufferForSilhouetteDetection)+k) = u32IndexJ;
/**/							}
/**/						}
/**/					}
/**/				}
/**/				a_pGfxContextInterface->CreateTextureBlock( m_aryVisualObject[i].m_pTextureBlock0 );
/**/				if( NULL!=m_aryVisualObject[i].m_pTextureBlock0 )
/**/				{
/**/					m_aryVisualObject[i].m_pTextureBlock0->SetColorCalc( IGN_ENUM_TEXOPERATION_SELECTARG2, IGN_ENUM_TEXARGUMENT_COLORFACTOR, IGN_ENUM_TEXARGUMENT_DIFFUSE );
/**/					m_aryVisualObject[i].m_pTextureBlock0->SetAlphaCalc( IGN_ENUM_TEXOPERATION_SELECTARG1, IGN_ENUM_TEXARGUMENT_COLORFACTOR, IGN_ENUM_TEXARGUMENT_DIFFUSE );
/**/				}
/**/				a_pGfxContextInterface->CreateTextureBlock( m_aryVisualObject[i].m_pTextureBlock1 );
/**/				if( NULL!=m_aryVisualObject[i].m_pTextureBlock1 )
/**/				{
/**/					m_aryVisualObject[i].m_pTextureBlock1->SetColorCalc( IGN_ENUM_TEXOPERATION_DISABLE, IGN_ENUM_TEXARGUMENT_COLORFACTOR, IGN_ENUM_TEXARGUMENT_DIFFUSE );
/**/					m_aryVisualObject[i].m_pTextureBlock1->SetAlphaCalc( IGN_ENUM_TEXOPERATION_DISABLE, IGN_ENUM_TEXARGUMENT_COLORFACTOR, IGN_ENUM_TEXARGUMENT_DIFFUSE );
/**/				}
/**/				a_pGfxContextInterface->CreateMaterial( m_aryVisualObject[i].m_pMaterial );
/**/				if( NULL!=m_aryVisualObject[i].m_pMaterial )
/**/				{
/**/					m_aryVisualObject[i].m_pMaterial->SetAmbientMaterialSource( IGN_ENUM_MATERIALCOLORSRC_COLOR1 );
/**/					m_aryVisualObject[i].m_pMaterial->AttachTextureBlock( 0, m_aryVisualObject[i].m_pTextureBlock0 );
/**/					m_aryVisualObject[i].m_pMaterial->AttachTextureBlock( 1, m_aryVisualObject[i].m_pTextureBlock1 );
/**/				}
/**/				a_pGfxContextInterface->CreateShader( m_aryVisualObject[i].m_pShader );
/**/				if( NULL!=m_aryVisualObject[i].m_pShader )
/**/				{
/**/					m_aryVisualObject[i].m_pShader->AttachMaterial( m_aryVisualObject[i].m_pMaterial );
/**/				}
/**/				a_pGfxContextInterface->CreateVisual( m_aryVisualObject[i].m_pVisual );
/**/				if( NULL!=m_aryVisualObject[i].m_pVisual )
/**/				{
/**/					IGN_STRUCT_DrawCallDesc	DrawCallDesc;
/**/					DrawCallDesc.m_bIndexedDrawCall		= true;
/**/					DrawCallDesc.m_PrimitiveType		= IGN_ENUM_PRIMITIVETYPE_TRIANGLELIST;
/**/					DrawCallDesc.m_u32PrimitiveCount	= u32PolyCount;
/**/					DrawCallDesc.m_u32StartVertex		= 0;
/**/					DrawCallDesc.m_u32MinIndex			= 0;
/**/					DrawCallDesc.m_u32NumberOfVertices	= u32PolyCount*2;
/**/					DrawCallDesc.m_u32StartIndex		= 0;
/**/					DrawCallDesc.m_u32BaseVertexIndex	= 0;
/**/					m_aryVisualObject[i].m_pVisual->SetDrawCallDescription( DrawCallDesc );
/**/					m_aryVisualObject[i].m_pVisual->SetDepthTesting( IGN_ENUM_CMPFUNC_LESSEQUAL );
/**/					m_aryVisualObject[i].m_pVisual->SetCullingMode( IGN_ENUM_CULLINGMODE_CW );
/**/					m_aryVisualObject[i].m_pVisual->AttachVertexData( m_aryVisualObject[i].m_pVertexData );
/**/					m_aryVisualObject[i].m_pVisual->AttachIndexData( m_aryVisualObject[i].m_pIndexData );
/**/					m_aryVisualObject[i].m_pVisual->AttachShader( m_aryVisualObject[i].m_pShader );
/**/				}
/**/				delete[] pSrc;
/**/				break;
/**/		case	3:
/**/
/**/				//
/**/				// The green object (object No.3)
/**/				//
/**/
/**/				u32PolyCount = 21*2;
/**/				m_aryVisualObject[i].m_ObjectSphere.m_ObjectStandardCenterVector.m_fX	= 
/**/				m_aryVisualObject[i].m_ObjectSphere.m_ObjectCurrentCenterVector.m_fX	= -70.0f;
/**/				m_aryVisualObject[i].m_ObjectSphere.m_ObjectStandardCenterVector.m_fY	= 
/**/				m_aryVisualObject[i].m_ObjectSphere.m_ObjectCurrentCenterVector.m_fY	= -70.0f;
/**/				m_aryVisualObject[i].m_ObjectSphere.m_ObjectStandardCenterVector.m_fZ	= 
/**/				m_aryVisualObject[i].m_ObjectSphere.m_ObjectCurrentCenterVector.m_fZ	= 205.0f;
/**/				m_aryVisualObject[i].m_ObjectSphere.m_fObjectRadius						= 80.0f;
/**/				m_aryVisualObject[i].m_ObjectSphere.m_fBiasZ							= 10.0f;
/**/				a_pGfxContextInterface->CreateVertexData( m_aryVisualObject[i].m_pVertexData );
/**/				pSrc = new structVertexFormat[u32PolyCount*2];
/**/				if( NULL!=m_aryVisualObject[i].m_pVertexData )
/**/				{
/**/					IGN_STRUCT_VertexBufDesc VertexBufferDescription;
/**/					VertexBufferDescription.m_uiLength	= (u32PolyCount*2) * (10*4);
/**/					VertexBufferDescription.m_dwUsage	= IGNUSAGE_WRITEONLY;
/**/					VertexBufferDescription.m_dwFVF		= IGNFVF_XYZ|IGNFVF_NORMAL|IGNFVF_DIFFUSE|IGNFVF_TEX1|IGNFVF_TEXCOORDSIZE3_0;
/**/					VertexBufferDescription.m_Pool		= IGN_ENUM_POOLTYPE_MANAGED;
/**/					m_aryVisualObject[i].m_pVertexData->CreateBuffer( VertexBufferDescription );
/**/					unsigned char* pData = NULL;
/**/					m_aryVisualObject[i].m_pVertexData->LockBuffer( 0, 0, 0, &pData );
/**/					if( (NULL!=pSrc)&&(NULL!=pData) )
/**/					{
/**/						for( IGNU32 j = 0; j<4*4; j++)
/**/						{
/**/							float fCX	= -107.5f + (j%4)*25.0f;
/**/							float fCY	= -107.5f + (j/4)*25.0f;
/**/							(pSrc+j*4+0)->X		= fCX-12.5f;	(pSrc+j*4+1)->X		= fCX+12.5f;	(pSrc+j*4+2)->X		= fCX+12.5f;	(pSrc+j*4+3)->X		= fCX-12.5f;			
/**/							(pSrc+j*4+0)->Y		= fCY-12.5f;	(pSrc+j*4+1)->Y		= fCY-12.5f;	(pSrc+j*4+2)->Y		= fCY+12.5f;	(pSrc+j*4+3)->Y		= fCY+12.5f;		
/**/							(pSrc+j*4+0)->Z		= 200.0f;		(pSrc+j*4+1)->Z		= 200.0f;		(pSrc+j*4+2)->Z		= 200.0f;		(pSrc+j*4+3)->Z		= 200.0f;		
/**/							(pSrc+j*4+0)->NX	= 0.0f;			(pSrc+j*4+1)->NX	= 0.0f;			(pSrc+j*4+2)->NX	= 0.0f;			(pSrc+j*4+3)->NX	= 0.0f;			
/**/							(pSrc+j*4+0)->NY	= 0.0f;			(pSrc+j*4+1)->NY	= 0.0f;			(pSrc+j*4+2)->NY	= 0.0f;			(pSrc+j*4+3)->NY	= 0.0f;			
/**/							(pSrc+j*4+0)->NZ	= -1.0f;		(pSrc+j*4+1)->NZ	= -1.0f;		(pSrc+j*4+2)->NZ	= -1.0f;		(pSrc+j*4+3)->NZ	= -1.0f;		
/**/							(pSrc+j*4+0)->COLOR	= 0xff00ff00;	(pSrc+j*4+1)->COLOR	= 0xff00ff00;	(pSrc+j*4+2)->COLOR	= 0xff00ff00;	(pSrc+j*4+3)->COLOR	= 0xff00ff00;	
/**/						}
/**/						(pSrc+64)->X	= -120.0f;		(pSrc+68)->X	= -20.0f;		(pSrc+72)->X	= -120.0f;		(pSrc+76)->X	= -120.0f;		(pSrc+80)->X	= -120.0f;	
/**/						(pSrc+64)->Y	= -120.0f;		(pSrc+68)->Y	= -120.0f;		(pSrc+72)->Y	= -120.0f;		(pSrc+76)->Y	= -20.0f;		(pSrc+80)->Y	= -120.0f;	
/**/						(pSrc+64)->Z	= 200.0f;		(pSrc+68)->Z	= 200.0f;		(pSrc+72)->Z	= 200.0f;		(pSrc+76)->Z	= 200.0f;		(pSrc+80)->Z	= 210.0f;	
/**/						(pSrc+64)->NX	= 0.0f;			(pSrc+68)->NX	= 1.0f;			(pSrc+72)->NX	= -1.0f;		(pSrc+76)->NX	= 0.0f;			(pSrc+80)->NX	= 0.0f;		
/**/						(pSrc+64)->NY	= -1.0f;		(pSrc+68)->NY	= 0.0f;			(pSrc+72)->NY	= 0.0f;			(pSrc+76)->NY	= 1.0f;			(pSrc+80)->NY	= 0.0f;	
/**/						(pSrc+64)->NZ	= 0.0f;			(pSrc+68)->NZ	= 0.0f;			(pSrc+72)->NZ	= 0.0f;			(pSrc+76)->NZ	= 0.0f;			(pSrc+80)->NZ	= 1.0f;		
/**/						(pSrc+64)->COLOR= 0xff00ff00;	(pSrc+68)->COLOR= 0xff00ff00;	(pSrc+72)->COLOR= 0xff00ff00;	(pSrc+76)->COLOR= 0xff00ff00;	(pSrc+80)->COLOR= 0xff00ff00;
/**/						(pSrc+65)->X	= -120.0f;		(pSrc+69)->X	= -20.0f;		(pSrc+73)->X	= -120.0f;		(pSrc+77)->X	= -20.0f;		(pSrc+81)->X	= -120.0f;	
/**/						(pSrc+65)->Y	= -120.0f;		(pSrc+69)->Y	= -120.0f;		(pSrc+73)->Y	= -20.0f;		(pSrc+77)->Y	= -20.0f;		(pSrc+81)->Y	= -20.0f;	
/**/						(pSrc+65)->Z	= 210.0f;		(pSrc+69)->Z	= 210.0f;		(pSrc+73)->Z	= 200.0f;		(pSrc+77)->Z	= 200.0f;		(pSrc+81)->Z	= 210.0f;	
/**/						(pSrc+65)->NX	= 0.0f;			(pSrc+69)->NX	= 1.0f;			(pSrc+73)->NX	= -1.0f;		(pSrc+77)->NX	= 0.0f;			(pSrc+81)->NX	= 0.0f;		
/**/						(pSrc+65)->NY	= -1.0f;		(pSrc+69)->NY	= 0.0f;			(pSrc+73)->NY	= 0.0f;			(pSrc+77)->NY	= 1.0f;			(pSrc+81)->NY	= 0.0f;	
/**/						(pSrc+65)->NZ	= 0.0f;			(pSrc+69)->NZ	= 0.0f;			(pSrc+73)->NZ	= 0.0f;			(pSrc+77)->NZ	= 0.0f;			(pSrc+81)->NZ	= 1.0f;		
/**/						(pSrc+65)->COLOR= 0xff00ff00;	(pSrc+69)->COLOR= 0xff00ff00;	(pSrc+73)->COLOR= 0xff00ff00;	(pSrc+77)->COLOR= 0xff00ff00;	(pSrc+81)->COLOR= 0xff00ff00;
/**/						(pSrc+66)->X	= -20.0f;		(pSrc+70)->X	= -20.0f;		(pSrc+74)->X	= -120.0f;		(pSrc+78)->X	= -20.0f;		(pSrc+82)->X	= -20.0f;	
/**/						(pSrc+66)->Y	= -120.0f;		(pSrc+70)->Y	= -20.0f;		(pSrc+74)->Y	= -20.0f;		(pSrc+78)->Y	= -20.0f;		(pSrc+82)->Y	= -20.0f;	
/**/						(pSrc+66)->Z	= 210.0f;		(pSrc+70)->Z	= 210.0f;		(pSrc+74)->Z	= 210.0f;		(pSrc+78)->Z	= 210.0f;		(pSrc+82)->Z	= 210.0f;	
/**/						(pSrc+66)->NX	= 0.0f;			(pSrc+70)->NX	= 1.0f;			(pSrc+74)->NX	= -1.0f;		(pSrc+78)->NX	= 0.0f;			(pSrc+82)->NX	= 0.0f;		
/**/						(pSrc+66)->NY	= -1.0f;		(pSrc+70)->NY	= 0.0f;			(pSrc+74)->NY	= 0.0f;			(pSrc+78)->NY	= 1.0f;			(pSrc+82)->NY	= 0.0f;	
/**/						(pSrc+66)->NZ	= 0.0f;			(pSrc+70)->NZ	= 0.0f;			(pSrc+74)->NZ	= 0.0f;			(pSrc+78)->NZ	= 0.0f;			(pSrc+82)->NZ	= 1.0f;		
/**/						(pSrc+66)->COLOR= 0xff00ff00;	(pSrc+70)->COLOR= 0xff00ff00;	(pSrc+74)->COLOR= 0xff00ff00;	(pSrc+78)->COLOR= 0xff00ff00;	(pSrc+82)->COLOR= 0xff00ff00;
/**/						(pSrc+67)->X	= -20.0f;		(pSrc+71)->X	= -20.0f;		(pSrc+75)->X	= -120.0f;		(pSrc+79)->X	= -120.0f;		(pSrc+83)->X	= -20.0f;	
/**/						(pSrc+67)->Y	= -120.0f;		(pSrc+71)->Y	= -020.0f;		(pSrc+75)->Y	= -120.0f;		(pSrc+79)->Y	= -20.0f;		(pSrc+83)->Y	= -120.0f;	
/**/						(pSrc+67)->Z	= 200.0f;		(pSrc+71)->Z	= 200.0f;		(pSrc+75)->Z	= 210.0f;		(pSrc+79)->Z	= 210.0f;		(pSrc+83)->Z	= 210.0f;	
/**/						(pSrc+67)->NX	= 0.0f;			(pSrc+71)->NX	= 1.0f;			(pSrc+75)->NX	= -1.0f;		(pSrc+79)->NX	= 0.0f;			(pSrc+83)->NX	= 0.0f;		
/**/						(pSrc+67)->NY	= -1.0f;		(pSrc+71)->NY	= 0.0f;			(pSrc+75)->NY	= 0.0f;			(pSrc+79)->NY	= 1.0f;			(pSrc+83)->NY	= 0.0f;	
/**/						(pSrc+67)->NZ	= 0.0f;			(pSrc+71)->NZ	= 0.0f;			(pSrc+75)->NZ	= 0.0f;			(pSrc+79)->NZ	= 0.0f;			(pSrc+83)->NZ	= 1.0f;		
/**/						(pSrc+67)->COLOR= 0xff00ff00;	(pSrc+71)->COLOR= 0xff00ff00;	(pSrc+75)->COLOR= 0xff00ff00;	(pSrc+79)->COLOR= 0xff00ff00;	(pSrc+83)->COLOR= 0xff00ff00;
/**/						memcpy( pData, pSrc, (u32PolyCount*2) * (10*4) );
/**/					}
/**/					m_aryVisualObject[i].m_pVertexData->UnlockBuffer();
/**/					m_aryVisualObject[i].m_pVertexData->SetStreamNumber( 0 );
/**/				}
/**/				a_pGfxContextInterface->CreateIndexData( m_aryVisualObject[i].m_pIndexData );
/**/				if( NULL!=m_aryVisualObject[i].m_pIndexData )
/**/				{
/**/					m_aryVisualObject[i].m_u32IndexCount = u32PolyCount*3;
/**/					IGN_STRUCT_IndexBufDesc IndexBufferDescription;
/**/					IndexBufferDescription.m_uiLength	= m_aryVisualObject[i].m_u32IndexCount*4;
/**/					IndexBufferDescription.m_dwUsage	= IGNUSAGE_WRITEONLY;
/**/					IndexBufferDescription.m_Format		= IGN_ENUM_INDEXFORMAT_INDEX32;
/**/					IndexBufferDescription.m_Pool		= IGN_ENUM_POOLTYPE_MANAGED;
/**/					m_aryVisualObject[i].m_pIndexData->CreateBuffer( IndexBufferDescription );
/**/					unsigned char* pData = NULL;
/**/					m_aryVisualObject[i].m_pIndexData->LockBuffer( 0, 0, 0, &pData );
/**/					IGNU32 u32Data[126];
/**/					if( NULL!=pData )
/**/					{
/**/						for( IGNU32 j = 0; j<4*4; j++ )
/**/						{
/**/							u32Data[j*6 + 0] = j*4 + 0;
/**/							u32Data[j*6 + 1] = j*4 + 1;
/**/							u32Data[j*6 + 2] = j*4 + 3;
/**/							u32Data[j*6 + 3] = j*4 + 1;
/**/							u32Data[j*6 + 4] = j*4 + 2;
/**/							u32Data[j*6 + 5] = j*4 + 3;
/**/						}						
/**/						for( j = 4*4; j<4*4+5; j++ )
/**/						{
/**/							u32Data[j*6 + 0] = j*4 + 0;
/**/							u32Data[j*6 + 1] = j*4 + 1;
/**/							u32Data[j*6 + 2] = j*4 + 2;
/**/							u32Data[j*6 + 3] = j*4 + 0;
/**/							u32Data[j*6 + 4] = j*4 + 2;
/**/							u32Data[j*6 + 5] = j*4 + 3;
/**/						}
/**/						memcpy( pData, u32Data, m_aryVisualObject[i].m_u32IndexCount*4);
/**/					}
/**/					m_aryVisualObject[i].m_pIndexData->UnlockBuffer();
/**/					m_aryVisualObject[i].m_pIndexBufferForSilhouetteDetection = new IGNU32[m_aryVisualObject[i].m_u32IndexCount];
/**/					memcpy( m_aryVisualObject[i].m_pIndexBufferForSilhouetteDetection, u32Data, m_aryVisualObject[i].m_u32IndexCount*4 );
/**/					IGNU32 u32IndexJ = 0;
/**/					IGNU32 u32IndexK = 0;
/**/					for( IGNU32 j = 0; j<m_aryVisualObject[i].m_u32IndexCount; j++ )
/**/					{
/**/						for( IGNU32 k = j+1; k<m_aryVisualObject[i].m_u32IndexCount; k++ )
/**/						{
/**/							u32IndexJ = u32Data[j];
/**/							u32IndexK = u32Data[k];
/**/ 							if( IGN_ENUM_INDEXFORMAT_INDEX32==IndexBufferDescription.m_Format )
/**/							{
/**/								if( (u32IndexJ!=u32IndexK)&&
/**/									((reinterpret_cast<IGN_STRUCT_3DVector*>( pSrc+u32IndexJ))->m_fX==(reinterpret_cast<IGN_STRUCT_3DVector*>( pSrc+u32IndexK))->m_fX)&&
/**/									((reinterpret_cast<IGN_STRUCT_3DVector*>( pSrc+u32IndexJ))->m_fY==(reinterpret_cast<IGN_STRUCT_3DVector*>( pSrc+u32IndexK))->m_fY)&&
/**/									((reinterpret_cast<IGN_STRUCT_3DVector*>( pSrc+u32IndexJ))->m_fZ==(reinterpret_cast<IGN_STRUCT_3DVector*>( pSrc+u32IndexK))->m_fZ) )
/**/									*((m_aryVisualObject[i].m_pIndexBufferForSilhouetteDetection)+k) = u32IndexJ;
/**/							}
/**/						}
/**/					}
/**/				}
/**/				a_pGfxContextInterface->CreateTextureBlock( m_aryVisualObject[i].m_pTextureBlock0 );
/**/				if( NULL!=m_aryVisualObject[i].m_pTextureBlock0 )
/**/				{
/**/					m_aryVisualObject[i].m_pTextureBlock0->SetColorCalc( IGN_ENUM_TEXOPERATION_SELECTARG2, IGN_ENUM_TEXARGUMENT_COLORFACTOR, IGN_ENUM_TEXARGUMENT_DIFFUSE );
/**/					m_aryVisualObject[i].m_pTextureBlock0->SetAlphaCalc( IGN_ENUM_TEXOPERATION_SELECTARG1, IGN_ENUM_TEXARGUMENT_COLORFACTOR, IGN_ENUM_TEXARGUMENT_DIFFUSE );
/**/				}
/**/				a_pGfxContextInterface->CreateTextureBlock( m_aryVisualObject[i].m_pTextureBlock1 );
/**/				if( NULL!=m_aryVisualObject[i].m_pTextureBlock1 )
/**/				{
/**/					m_aryVisualObject[i].m_pTextureBlock1->SetColorCalc( IGN_ENUM_TEXOPERATION_DISABLE, IGN_ENUM_TEXARGUMENT_COLORFACTOR, IGN_ENUM_TEXARGUMENT_DIFFUSE );
/**/					m_aryVisualObject[i].m_pTextureBlock1->SetAlphaCalc( IGN_ENUM_TEXOPERATION_DISABLE, IGN_ENUM_TEXARGUMENT_COLORFACTOR, IGN_ENUM_TEXARGUMENT_DIFFUSE );
/**/				}
/**/				a_pGfxContextInterface->CreateMaterial( m_aryVisualObject[i].m_pMaterial );
/**/				if( NULL!=m_aryVisualObject[i].m_pMaterial )
/**/				{
/**/					m_aryVisualObject[i].m_pMaterial->SetAmbientMaterialSource( IGN_ENUM_MATERIALCOLORSRC_COLOR1 );
/**/					m_aryVisualObject[i].m_pMaterial->AttachTextureBlock( 0, m_aryVisualObject[i].m_pTextureBlock0 );
/**/					m_aryVisualObject[i].m_pMaterial->AttachTextureBlock( 1, m_aryVisualObject[i].m_pTextureBlock1 );
/**/				}
/**/				a_pGfxContextInterface->CreateShader( m_aryVisualObject[i].m_pShader );
/**/				if( NULL!=m_aryVisualObject[i].m_pShader )
/**/				{
/**/					m_aryVisualObject[i].m_pShader->AttachMaterial( m_aryVisualObject[i].m_pMaterial );
/**/				}
/**/				a_pGfxContextInterface->CreateVisual( m_aryVisualObject[i].m_pVisual );
/**/				if( NULL!=m_aryVisualObject[i].m_pVisual )
/**/				{
/**/					IGN_STRUCT_DrawCallDesc	DrawCallDesc;
/**/					DrawCallDesc.m_bIndexedDrawCall		= true;
/**/					DrawCallDesc.m_PrimitiveType		= IGN_ENUM_PRIMITIVETYPE_TRIANGLELIST;
/**/					DrawCallDesc.m_u32PrimitiveCount	= u32PolyCount;
/**/					DrawCallDesc.m_u32StartVertex		= 0;
/**/					DrawCallDesc.m_u32MinIndex			= 0;
/**/					DrawCallDesc.m_u32NumberOfVertices	= u32PolyCount*2;
/**/					DrawCallDesc.m_u32StartIndex		= 0;
/**/					DrawCallDesc.m_u32BaseVertexIndex	= 0;
/**/					m_aryVisualObject[i].m_pVisual->SetDrawCallDescription( DrawCallDesc );
/**/					m_aryVisualObject[i].m_pVisual->SetDepthTesting( IGN_ENUM_CMPFUNC_LESSEQUAL );
/**/					m_aryVisualObject[i].m_pVisual->SetCullingMode( IGN_ENUM_CULLINGMODE_CW );
/**/					m_aryVisualObject[i].m_pVisual->AttachVertexData( m_aryVisualObject[i].m_pVertexData );
/**/					m_aryVisualObject[i].m_pVisual->AttachIndexData( m_aryVisualObject[i].m_pIndexData );
/**/					m_aryVisualObject[i].m_pVisual->AttachShader( m_aryVisualObject[i].m_pShader );
/**/				}
/**/				delete[] pSrc;
/**/				break;
/**/		}
/**/
/**/
//
//
//
//       End of 'Coder Art'
//
// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------


		}
	}
}


void SCENE::Finit												(	void )
{
	//----------------------------------------------------------|----------------------------------------------Finit-|
	// public method
	// Finalizes the tester.

	//
	// Release objects in IGN_timer_sub.dll
	//
	if( NULL!=m_pTimerInterface )
		m_pTimerInterface->Release();
	if( NULL!=m_pTmrContextInterface )
		m_pTmrContextInterface->Release();
	if( NULL!=m_pTmrInstanceInterface )
		m_pTmrInstanceInterface->Release();
	if( NULL!=m_hTmrLibrary )
		FreeLibrary( m_hTmrLibrary );

	//
	// Release objects in IGN_graphic_sub.dll
	//
	if( NULL!=m_pReducedCombinedBufferTec )
		delete m_pReducedCombinedBufferTec;
	if( NULL!=m_pCompleteCombinedBufferTec )
		delete m_pCompleteCombinedBufferTec;
	for( IGNINT i = constObjectCount-1; i>=0; i-- )	
	{
		if( NULL!=m_aryVisualObject[i].m_pVisual )
			m_aryVisualObject[i].m_pVisual->Release();
		if( NULL!=m_aryVisualObject[i].m_pVertexData )
			m_aryVisualObject[i].m_pVertexData->Release();
		if( NULL!=m_aryVisualObject[i].m_pIndexData )
			m_aryVisualObject[i].m_pIndexData->Release();
		if( NULL!=m_aryVisualObject[i].m_pShader )
			m_aryVisualObject[i].m_pShader->Release();
		if( NULL!=m_aryVisualObject[i].m_pMaterial )
			m_aryVisualObject[i].m_pMaterial->Release();
		if( NULL!=m_aryVisualObject[i].m_pTextureBlock0 )
			m_aryVisualObject[i].m_pTextureBlock0->Release();
		if( NULL!=m_aryVisualObject[i].m_pTextureBlock1 )
			m_aryVisualObject[i].m_pTextureBlock1->Release();
		if( NULL!=m_aryVisualObject[i].m_pTextureBlock2 )
			m_aryVisualObject[i].m_pTextureBlock2->Release();
		if( NULL!=m_aryVisualObject[i].m_pTextureBlock3 )
			m_aryVisualObject[i].m_pTextureBlock3->Release();
		if( NULL!=m_aryVisualObject[i].m_pIndexBufferForSilhouetteDetection )
			delete[] m_aryVisualObject[i].m_pIndexBufferForSilhouetteDetection;
	}
	for( i = 0; i<constLightCount; i++ )
	{
		if( NULL!=m_pLight[i] )
			m_pLight[i]->Release();
	}
	if( NULL!=m_pAmbientLight )
		m_pAmbientLight->Release();
}


void SCENE::ActivateStencilTest									(	void )
{
	//----------------------------------------------------------|--------------------------------ActivateStencilTest-|
	// public method
	// Activates the stencil test to enable shadowed light.

	m_StencilData.m_StencilFail			= IGN_ENUM_STENCILOP_KEEP;
	m_StencilData.m_StencilZFail		= IGN_ENUM_STENCILOP_KEEP;
	m_StencilData.m_StencilPass			= IGN_ENUM_STENCILOP_KEEP;
	m_StencilData.m_StencilFunc			= IGN_ENUM_CMPFUNC_EQUAL;
	m_StencilData.m_u32StencilMask		= 0xffffffff;
	m_StencilData.m_u32StencilRef		= 0x00000000;
	m_StencilData.m_u32StencilWriteMask	= 0xffffffff;
	m_pMixer->SetMasterStencilTesting(
		IGN_ENUM_MASTERSTENCIL_ENABLE,
		m_StencilData );
}


void SCENE::DeactivateStencilTest								(	void )
{
	//----------------------------------------------------------|------------------------------DeactivateStencilTest-|
	// public method
	// Deactivates the stencil test to disable shadowed light.

	m_pMixer->SetMasterStencilTesting(
		IGN_ENUM_MASTERSTENCIL_VISUAL,
		m_StencilData );
}


void SCENE::ProceedVisualsWithAmbientLight						(	void )
{
	//----------------------------------------------------------|---------------------ProceedVisualsWithAmbientLight-|
	// public method
	// Proceeds the visuals with ambient light.

	//
	// Render visuals with ambient lighting
	//
	if( NULL!=m_pAmbientLight )
	{
		m_pAmbientLight->Execute();
		for( IGNINT i = constObjectCount-1; i>=0; i-- )	
		{
			if( NULL!=m_aryVisualObject[i].m_pVisual )
				m_aryVisualObject[i].m_pVisual->Execute();
		}
	}
}


void SCENE::ProceedVisualsWithAdditionalLights					(	CAMERA*					a_pCamera )
{
	//----------------------------------------------------------|-----------------ProceedVisualsWithAdditionalLights-|
	// public method
	// Proceeds the visuals with additional lights.

	//
	// Render visuals with additional lighting (w/o shadows)
	//
	if( true==m_bEnabledAdditionalLights )
	{
		for( IGNU32 i = 0; i <constLightCount ; i++ )
		{
			//
			// For each additional light do ...
			//
			if( 1<=i )
			{
				m_pMixer->ClearViewport( static_cast<IGN_ENUM_VIEWPORTCLEAR>( 
					IGN_ENUM_VIEWPORTCLEAR_STENCIL ), 
					0x00000000, 
					1.0f, 
					0 );
				if( false==m_bEnabledSecondLight )
					break;
			}
			if( NULL!=m_pLight[i] )
			{
				if( true==m_bEnabledShadows )
				{
					//
					// If shadows are enabled do ...
					//
					SHADOWTECBASE* pShadowTec;
					switch( m_ShadowTechnique )
					{
					case	ENUM_TECHNIQUE_REDUCEDCOMBINEDBUFFER:
							pShadowTec = m_pReducedCombinedBufferTec;
							break;
					case	ENUM_TECHNIQUE_COMPLETECOMBINEDBUFFER:
							pShadowTec = m_pCompleteCombinedBufferTec;
							break;
					default:
							pShadowTec = NULL;
					}
					if( NULL!=pShadowTec )
					{
						//
						// Pre process the shadows
						//
						pShadowTec->PreProcess(
							a_pCamera,
							m_pLight[i],
							m_aryVisualObject,
							constObjectCount );

						//
						// Set the camera again (w/o parameter updates)
						//
						a_pCamera->ProcessAgain();

						//
						// Process the shadows
						//
						pShadowTec->Proceed(
							m_pLight[i],
							m_aryVisualObject,
							constObjectCount );
					}
					ActivateStencilTest();
				}

				//
				// Render the lighted scene (w/o shadows)
				//
				m_pLight[i]->Execute();
				m_pMixer->SetMasterBlending( IGN_ENUM_MASTERBLENDING_ONE, IGN_ENUM_MASTERBLENDING_ONE );
				m_pMixer->SetMasterDepthTesting( IGN_ENUM_MASTERCMPFUNC_EQUAL );
				m_pMixer->SetMasterDepthWrite( IGN_ENUM_MASTERWRITE_DISABLE );
				for( IGNINT i = constObjectCount-1; i>=0; i-- )	
				{
					if( NULL!=m_aryVisualObject[i].m_pVisual )
						m_aryVisualObject[i].m_pVisual->Execute();
				}
				
				m_pMixer->SetMasterDepthTesting( IGN_ENUM_MASTERCMPFUNC_VISUAL );
				m_pMixer->SetMasterDepthWrite( IGN_ENUM_MASTERWRITE_VISUAL );
				if( true==m_bEnabledShadows )
					DeactivateStencilTest();
			}
		}
	}
	m_pMixer->SetMasterBlending( IGN_ENUM_MASTERBLENDING_MATERIAL, IGN_ENUM_MASTERBLENDING_MATERIAL );
}


void SCENE::AnimateVisuals										(	void )
{
	//----------------------------------------------------------|-------------------------------------AnimateVisuals-|
	// public method
	// Animate visuals.

	//
	// Do some simple animation with the visuals
	//
	float fStep = m_u32VisualAniStep*0.0003f;
	IGN_STRUCT_4x4Matrix worldMatrix;
	D3DXVECTOR3 d3dxVector3;
	D3DXVECTOR4 d3dxVector4;
	D3DXMATRIX d3dxWorldMatrix;
	for( IGNINT i = constObjectCount-1; i>=0; i-- )	
	{
		switch( i )
		{
		case	0:
				worldMatrix.m_Elements.m_f11	= 1.0f;
				worldMatrix.m_Elements.m_f12	= 0.0f;
				worldMatrix.m_Elements.m_f13	= 0.0f;
				worldMatrix.m_Elements.m_f14	= 0.0f;
				worldMatrix.m_Elements.m_f21	= 0.0f;
				worldMatrix.m_Elements.m_f22	= 1.0f;
				worldMatrix.m_Elements.m_f23	= 0.0f;
				worldMatrix.m_Elements.m_f24	= 0.0f;
				worldMatrix.m_Elements.m_f31	= 0.0f;
				worldMatrix.m_Elements.m_f32	= 0.0f;
				worldMatrix.m_Elements.m_f33	= 1.0f;
				worldMatrix.m_Elements.m_f34	= 0.0f;
				worldMatrix.m_Elements.m_f41	= 0.0f;
				worldMatrix.m_Elements.m_f42	= 0.0f;
				worldMatrix.m_Elements.m_f43	= 0.0f;
				worldMatrix.m_Elements.m_f44	= 1.0f;
				break;
		case	1:
				worldMatrix.m_Elements.m_f11	= static_cast<IGNFLOAT>( cos( fStep ) );
				worldMatrix.m_Elements.m_f12	= -static_cast<IGNFLOAT>( sin( fStep ) );
				worldMatrix.m_Elements.m_f13	= 0.0f;
				worldMatrix.m_Elements.m_f14	= 0.0f;
				worldMatrix.m_Elements.m_f21	= static_cast<IGNFLOAT>( sin( fStep ) );
				worldMatrix.m_Elements.m_f22	= static_cast<IGNFLOAT>( cos( fStep ) );
				worldMatrix.m_Elements.m_f23	= 0.0f;
				worldMatrix.m_Elements.m_f24	= 0.0f;
				worldMatrix.m_Elements.m_f31	= 0.0f;
				worldMatrix.m_Elements.m_f32	= 0.0f;
				worldMatrix.m_Elements.m_f33	= 1.0f;
				worldMatrix.m_Elements.m_f34	= 0.0f;
				worldMatrix.m_Elements.m_f41	= 0.0f;
				worldMatrix.m_Elements.m_f42	= 0.0f;
				worldMatrix.m_Elements.m_f43	= 0.0f;
				worldMatrix.m_Elements.m_f44	= 1.0f;
				break;
		case	2:
				worldMatrix.m_Elements.m_f11	= static_cast<IGNFLOAT>( cos( -fStep ) );
				worldMatrix.m_Elements.m_f12	= -static_cast<IGNFLOAT>( sin( -fStep ) );
				worldMatrix.m_Elements.m_f13	= 0.0f;
				worldMatrix.m_Elements.m_f14	= 0.0f;
				worldMatrix.m_Elements.m_f21	= static_cast<IGNFLOAT>( sin( -fStep ) );
				worldMatrix.m_Elements.m_f22	= static_cast<IGNFLOAT>( cos( -fStep ) );
				worldMatrix.m_Elements.m_f23	= 0.0f;
				worldMatrix.m_Elements.m_f24	= 0.0f;
				worldMatrix.m_Elements.m_f31	= 0.0f;
				worldMatrix.m_Elements.m_f32	= 0.0f;
				worldMatrix.m_Elements.m_f33	= 1.0f;
				worldMatrix.m_Elements.m_f34	= 0.0f;
				worldMatrix.m_Elements.m_f41	= 0.0f;
				worldMatrix.m_Elements.m_f42	= 0.0f;
				worldMatrix.m_Elements.m_f43	= 0.0f;
				worldMatrix.m_Elements.m_f44	= 1.0f;
				break;
		case	3:
				worldMatrix.m_Elements.m_f11	= 1.0f;
				worldMatrix.m_Elements.m_f12	= 0.0f;
				worldMatrix.m_Elements.m_f13	= 0.0f;
				worldMatrix.m_Elements.m_f14	= 0.0f;
				worldMatrix.m_Elements.m_f21	= 0.0f;
				worldMatrix.m_Elements.m_f22	= 1.0f;
				worldMatrix.m_Elements.m_f23	= 0.0f;
				worldMatrix.m_Elements.m_f24	= 0.0f;
				worldMatrix.m_Elements.m_f31	= 0.0f;
				worldMatrix.m_Elements.m_f32	= 0.0f;
				worldMatrix.m_Elements.m_f33	= 1.0f;
				worldMatrix.m_Elements.m_f34	= 0.0f;
				worldMatrix.m_Elements.m_f41	= 0.0f;
				worldMatrix.m_Elements.m_f42	= 0.0f;
				worldMatrix.m_Elements.m_f43	= static_cast<IGNFLOAT>( cos( fStep*4 )*170.0f );
				worldMatrix.m_Elements.m_f44	= 1.0f;
				break;
		}
		if( NULL!=m_aryVisualObject[i].m_pVisual )
		{
			m_aryVisualObject[i].m_pVisual->SetWorldMatrix( worldMatrix );
			memcpy( &d3dxWorldMatrix, &worldMatrix, 4*4*4 );
			memcpy( &d3dxVector3, &(m_aryVisualObject[i].m_ObjectSphere.m_ObjectStandardCenterVector), 3*4 );
			D3DXVec3Transform( &d3dxVector4, &d3dxVector3, &d3dxWorldMatrix );
			memcpy( &(m_aryVisualObject[i].m_ObjectSphere.m_ObjectCurrentCenterVector), &d3dxVector4, 3*4 );
		}
	}
}


void SCENE::AnimateLights										(	void )
{
	//----------------------------------------------------------|--------------------------------------AnimateLights-|
	// public method
	// Animate visuals.

	//
	// Do some simple light animation
	//
	float fStep = m_u32LightAniStep*0.0003f;
	IGN_STRUCT_LightData lightData;
	m_pLight[0]->GetVertexLightingData( 0, lightData );
	if( m_bEnabledColoredLight )
	{
		lightData.m_Diffuse.m_fRed		= 0.3f;
		lightData.m_Diffuse.m_fGreen	= 0.8f;
		lightData.m_Diffuse.m_fBlue		= 0.2f;
	}
	else
	{
		lightData.m_Diffuse.m_fRed		= 0.6f;
		lightData.m_Diffuse.m_fGreen	= 0.6f;
		lightData.m_Diffuse.m_fBlue		= 0.6f;
	}
	m_pLight[0]->SetVertexLightingData( 0, lightData );
	m_pLight[1]->GetVertexLightingData( 0, lightData );
	if( m_bEnabledColoredLight )
	{
		lightData.m_Diffuse.m_fRed		= 0.8f;
		lightData.m_Diffuse.m_fGreen	= 0.2f;
		lightData.m_Diffuse.m_fBlue		= 0.3f;
	}
	else
	{
		lightData.m_Diffuse.m_fRed		= 0.6f;
		lightData.m_Diffuse.m_fGreen	= 0.6f;
		lightData.m_Diffuse.m_fBlue		= 0.6f;
	}
	lightData.m_Position.m_fX	= static_cast<IGNFLOAT>( sin( fStep*(-2.5f) ) )*350.0f;
	lightData.m_Position.m_fY	= static_cast<IGNFLOAT>( cos( fStep*(-2.5f) ) )*350.0f;
	lightData.m_Direction.m_fX	= 0.0f	- lightData.m_Position.m_fX;
	lightData.m_Direction.m_fY	= 0.0f	- lightData.m_Position.m_fY;
	lightData.m_Direction.m_fZ	= 400.0f- lightData.m_Position.m_fZ;
	m_pLight[1]->SetVertexLightingData( 0, lightData );
}


IGNBOOL SCENE::ProceedScene										(	CAMERA*					a_pCamera )
{
	//----------------------------------------------------------|---------------------------------------ProceedScene-|
	// public method
	// Proceeds the scene.

	//
	// Simple animation
	//
	AnimateVisuals();
	AnimateLights();

	//
	// Ambient lighting
	//
	ProceedVisualsWithAmbientLight();

	//
	// Additioanl lighting (w/o shadows)
	//
	ProceedVisualsWithAdditionalLights( a_pCamera );

	return true;
}


IGNBOOL SCENE::Proceed											(	CAMERA*					a_pCamera,
																	ENUM_TECHNIQUE			a_ShadowTechnique,
																	IGNU32					a_u32LightCount,
																	IGNBOOL					a_bColoredLight,
																	IGNBOOL					a_bEnabledShadows )
{
	//----------------------------------------------------------|--------------------------------------------Proceed-|
	// public method
	// Proceeds the scene.

	//
	// Set internal parameters
	//
	m_ShadowTechnique			= a_ShadowTechnique;
	switch( a_u32LightCount )
	{
	case	1:
			m_bEnabledAdditionalLights	= true;
			m_bEnabledSecondLight		= false;
			break;
	case	2:
			m_bEnabledAdditionalLights	= true;
			m_bEnabledSecondLight		= true;
			break;
	default:
			m_bEnabledAdditionalLights	= false;
	}
	m_bEnabledColoredLight			= a_bColoredLight;
	m_bEnabledShadows				= a_bEnabledShadows;

	//
	// Get the time difference (since last request)
	//
	IGNFLOAT fTimeDiff;
	IGNFLOAT fTime;
	if( NULL!=m_pTimerInterface )
	{
		m_pTimerInterface->GetTimeDifference( fTimeDiff );
		m_pTimerInterface->GetTime( fTime );
		m_u32VisualAniStep	+= static_cast<IGNU32>( floor(fTimeDiff * 1000.0f) );
		m_u32LightAniStep	+= static_cast<IGNU32>( floor(fTimeDiff * 1000.0f) );
	}

	//
	// Proceed with the scene
	//
	return ProceedScene( a_pCamera );
}

