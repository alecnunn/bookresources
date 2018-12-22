// SceneManager.cpp: A Scene Manager Class.
// Date: 08/02/05
// Created by: Aurelio Reis

#include "dxstdafx.h"

#include "../Include/SceneManager.h"
#include "../Include/Entity.h"


// Constructor.
CSceneView::CSceneView()
{
	Reset();
}

// Destructor.
CSceneView::~CSceneView() {}

// Render this View.
void CSceneView::RenderView( bool bForceRender )
{
	// If we have a parent, let IT render us out. bForceRender will be true
	// if this views parent is rendering it.
	if ( m_pParentView && !bForceRender )
	{
		return;
	}

	// Ensure no infinite recursions.
	if ( MAX_VIEW_RECURSIONS <= CSceneManager::m_pInstance->m_iViewRecursionLevel )
	{
		return;
	}

	// Render any Sub-Views.
	vector< CSceneView * >::iterator iterSubView = m_SubViews.begin();
	for ( ; iterSubView != m_SubViews.end(); ++iterSubView )
	{
		// Skip disabled views.
		if ( !(*iterSubView)->m_bEnabled )
		{
			continue;
		}

		(*iterSubView)->RenderView( true );
	}

	// Increment the recursion count and set the current view.
	CSceneManager::m_pInstance->m_iViewRecursionLevel++;
	CSceneManager::m_pInstance->m_pCurrentView = this;

	// Render based on the view type.
	switch ( m_ViewType )
	{
		case VIEWTYPE_NORMAL:
			RenderNormalView();
		break;

		case VIEWTYPE_RTT:
			assert( !"NYI" );
		break;

		case VIEWTYPE_CUBE_RTT:
			RenderCubeRTTView();
		break;

#ifdef USE_SH_CONVOLVE
		case VIEWTYPE_PARABOLOID_CONVOLUTION:
			RenderParaboloidConvolveView();
		break;
#endif

		default:
			// TODO: Error.
		break;
	}

	// Decrement the recursion count and reset the view.
	CSceneManager::m_pInstance->m_iViewRecursionLevel--;
	CSceneManager::m_pInstance->m_pCurrentView = NULL;
}

// Render a normal view.
void CSceneView::RenderNormalView()
{
	if ( FAILED( g_pd3dDevice->BeginScene() ) )
	{
		return;
	}

	// Clear the render target and the zbuffer .
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, m_ulClearColor, 1.0f, 0 );

	// Setup the view parameters.
	CSceneManager::m_pInstance->m_pCurrentView->m_matView = *g_Camera.GetViewMatrix();
	CSceneManager::m_pInstance->m_pCurrentView->m_matProj = *g_Camera.GetProjMatrix();

	// First render the depth-pass.
	RenderEntitiesDepth();

	// NOTE: Normally there would be a render all meshs per-light, render non-lit meshs, render
	// translucent meshs, etc...

	// Now render the Entities.
	RenderEntitiesColor();

	// Render the Fog pass.
//	RenderEntitiesFog();

	g_pd3dDevice->EndScene();
}

// Render to a cubemap.
void CSceneView::RenderCubeRTTView()
{
	if ( !m_RenderTarget.m_pRenderTarget ||
		 !m_RenderTarget.m_pRenderTargetDepthStencil )
	{
		return;
	}

	// Don't update the view if it's not time to yet.
	if ( m_fNextRender > g_fTime )
	{
		return;
	}

	HRESULT hr;
	D3DXMATRIXA16 mView;
	D3DXMATRIXA16 mProj;

    // The projection matrix has a FOV of 90 degrees and aspect ratio of 1.
    D3DXMatrixPerspectiveFovLH( &mProj, D3DX_PI * 0.5f, 1.0f, 0.01f, 10000.0f );

	// Get the old render target (the current backbuffer).
	LPDIRECT3DSURFACE9 pRTOld = NULL;
	V( g_pd3dDevice->GetRenderTarget( 0, &pRTOld ) );

	// If the device has a depth/stencil buffer, use the cube map's instead.
	LPDIRECT3DSURFACE9 pDSOld = NULL;
	if ( SUCCEEDED( g_pd3dDevice->GetDepthStencilSurface( &pDSOld ) ) )
	{
		V( g_pd3dDevice->SetDepthStencilSurface( m_RenderTarget.m_pRenderTargetDepthStencil ) );
	}

	LPDIRECT3DSURFACE9 pSurf;
	IDirect3DCubeTexture9 *pCubeTxtr = (IDirect3DCubeTexture9 *)m_RenderTarget.m_pRenderTarget;

	for ( int iFace = 0; iFace < 6; ++iFace )
	{
		// Set the render target to this cube face.
		// NOTE: May be quicker to pre fetch and store these then release later.
		V( pCubeTxtr->GetCubeMapSurface( (D3DCUBEMAP_FACES) iFace, 0, &pSurf ) );
		V( g_pd3dDevice->SetRenderTarget( 0, pSurf ) );
		SAFE_RELEASE( pSurf );

		// Get the correct view matrix for this cube face.
		mView = DXUTGetCubeMapViewMatrix( iFace );

		// Setup the view parameters.
		CSceneManager::m_pInstance->m_pCurrentView->m_matView = mView;
		CSceneManager::m_pInstance->m_pCurrentView->m_matProj = mProj;

		// Clear the face.
		V( g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, m_ulClearColor, 1.0f, 0 ) );

		// Begin the scene.
		if ( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
		{
			// First render the depth-pass.
			RenderEntitiesDepth();

			// Now render the Entities.
			RenderEntitiesColor();

			// Render the Fog pass.
//			RenderEntitiesFog();

			// End the scene.
			g_pd3dDevice->EndScene();
		}
	}

    // Restore depth-stencil buffer and render target.
    if ( pDSOld )
    {
        V( g_pd3dDevice->SetDepthStencilSurface( pDSOld ) );
        SAFE_RELEASE( pDSOld );
    }

    V( g_pd3dDevice->SetRenderTarget( 0, pRTOld ) );
    SAFE_RELEASE( pRTOld );

	// Update the time until the next update.
	m_fNextRender = g_fTime + m_fNextRenderDelta;

	//D3DXSaveTextureToFileA( "output.dds", D3DXIFF_DDS, pCubeTxtr, NULL );
}


HRESULT Draw2D( BOOL bTexture )
{
    LPDIRECT3DSURFACE9 pSurfRT;
    D3DSURFACE_DESC surfDesc;
    g_pd3dDevice->GetRenderTarget( 0, &pSurfRT );
    pSurfRT->GetDesc( &surfDesc );
    pSurfRT->Release();
    g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

    if ( bTexture )
    {
        struct vtx
        {
            D3DXVECTOR4 position;
            D3DXVECTOR2 texcoord;
        };
        vtx fsQuad[3];
        fsQuad[0].position = D3DXVECTOR4( -0.5f, -0.5f, 0.5f, 1.f );
        fsQuad[1].position = D3DXVECTOR4( 2.f*(FLOAT)surfDesc.Width - 0.5f, -0.5f, 0.5f, 1.f );
        fsQuad[2].position = D3DXVECTOR4( -0.5f, 2.f*(FLOAT)surfDesc.Height - 0.5f, 0.5f, 1.f );
        fsQuad[0].texcoord = D3DXVECTOR2(0.f, 0.f);
        fsQuad[1].texcoord = D3DXVECTOR2(2.f, 0.f);
        fsQuad[2].texcoord = D3DXVECTOR2(0.f, 2.f);
        g_pd3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0));
        g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, fsQuad, sizeof(vtx));
    }
    else
    {
        D3DXVECTOR4 fsQuad[3];
        fsQuad[0] = D3DXVECTOR4( -0.5f, -0.5f, 0.5f, 1.f );
        fsQuad[1] = D3DXVECTOR4( 2.f*(FLOAT)surfDesc.Width - 0.5f, -0.5f, 0.5f, 1.f );
        fsQuad[2] = D3DXVECTOR4( -0.5f, 2.f*(FLOAT)surfDesc.Height - 0.5f, 0.5f, 1.f );
        g_pd3dDevice->SetFVF(D3DFVF_XYZRHW);
        g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, fsQuad, sizeof(D3DXVECTOR4));
    }

    g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
    return S_OK;
}

HRESULT Draw2D( LPDIRECT3DTEXTURE9 pTexture )
{
    UINT uPasses;
    HRESULT hr = S_OK;

    V_RETURN( g_pParaboloidEffect->SetTechnique( "Simple2DRender" ) );
    V_RETURN( g_pParaboloidEffect->SetTexture( "CubeTexture", pTexture ) );

    if ( SUCCEEDED( g_pParaboloidEffect->Begin( &uPasses, 0 ) ) )
    {
        V_RETURN( g_pParaboloidEffect->BeginPass( 0 ) );

        Draw2D( true );

        V_RETURN(g_pParaboloidEffect->EndPass());

        V_RETURN(g_pParaboloidEffect->End());
    }

    return S_OK;

}


HRESULT RenderParaboloidEnvMap( IDirect3DCubeTexture9 *pRenderCube )
{
    UINT uPasses;
    HRESULT hr = S_OK;

    for ( UINT face=0; face<2; face++ )
    {
        LPDIRECT3DSURFACE9 pRTSurf = NULL;
        V_RETURN(g_pParaboloidMap[face]->GetSurfaceLevel( 0, &pRTSurf ));	
        V_RETURN(g_pd3dDevice->SetRenderTarget(0, pRTSurf));
        V_RETURN(g_pd3dDevice->SetDepthStencilSurface(NULL));
        
        g_pd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0xff,0x7f,0x7f,0xff), 1.0f, 0L);
        V_RETURN(g_pParaboloidEffect->SetTechnique("ConvertHemisphere"));
        V_RETURN(g_pParaboloidEffect->SetTexture("CubeMap", pRenderCube));

        D3DXVECTOR4 vDirectionVec(0.f, 0.f, (face)?1.f : -1.f, 1.f);
        V_RETURN(g_pParaboloidEffect->SetVector("DirectionVec", &vDirectionVec));

        if ( SUCCEEDED(g_pParaboloidEffect->Begin(&uPasses, 0)) )
        {
            for ( UINT i=0; i<uPasses; i++ )
            {
                V_RETURN(g_pParaboloidEffect->BeginPass(i));
                V_RETURN( Draw2D( FALSE ) );
                V_RETURN(g_pParaboloidEffect->EndPass());
            }
            g_pParaboloidEffect->End();
        }
        pRTSurf->Release();
    }

    return S_OK;
}

HRESULT ProjectParaboloidToSH()
{
    UINT uPasses;
    HRESULT hr = S_OK;

    LPDIRECT3DSURFACE9 pRTSurf = NULL;
    V_RETURN(g_pIrradianceSHCoefficients->GetSurfaceLevel(0, &pRTSurf));
    V_RETURN(g_pd3dDevice->SetRenderTarget(0, pRTSurf));
    V_RETURN(g_pd3dDevice->SetDepthStencilSurface(NULL));
    g_pd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET, 0x00000000, 1.f, 0L);
    
    V_RETURN(g_pParaboloidEffect->SetTechnique("ProjectDualParaboloidToSH"));
    V_RETURN(g_pParaboloidEffect->SetTexture("SH_Convolve_dE_0", g_pParaboloidMap[0]));
    V_RETURN(g_pParaboloidEffect->SetTexture("SH_Convolve_dE_1", g_pParaboloidMap[1]));
    V_RETURN(g_pParaboloidEffect->SetTexture("SH_Convolve_Ylm_dW_0", g_pParaboloidSHWeights[0]));
    V_RETURN(g_pParaboloidEffect->SetTexture("SH_Convolve_Ylm_dW_1", g_pParaboloidSHWeights[1]));

    if ( SUCCEEDED(g_pParaboloidEffect->Begin(&uPasses, 0)) )
    {
        for ( UINT i=0; i<uPasses; i++ )
        {
            V_RETURN(g_pParaboloidEffect->BeginPass(i));
            V_RETURN( Draw2D( FALSE ) );
            V_RETURN(g_pParaboloidEffect->EndPass());
        }
        g_pParaboloidEffect->End();
    }
    pRTSurf->Release();
    return S_OK;
}

HRESULT EvaluateConvolvedSH( LPDIRECT3DCUBETEXTURE9 pResultCube, const LPDIRECT3DTEXTURE9* pEvalSHFunction )
{
    UINT uPasses;
    HRESULT hr = S_OK;

    for ( UINT face=0; face<6; face++ )
    {
        LPDIRECT3DSURFACE9 pRTSurf = NULL;
        V_RETURN(pResultCube->GetCubeMapSurface((D3DCUBEMAP_FACES)face, 0, &pRTSurf));
        V_RETURN(g_pd3dDevice->SetRenderTarget(0, pRTSurf));
        V_RETURN(g_pd3dDevice->SetDepthStencilSurface(NULL));
        g_pd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET, 0x00000000, 1.f, 0L);

        const D3DXHANDLE EvalTextureNames[6] = { "SH_Integrate_Ylm_Al_xpos",
                                                 "SH_Integrate_Ylm_Al_xneg",
                                                 "SH_Integrate_Ylm_Al_ypos",
                                                 "SH_Integrate_Ylm_Al_yneg",
                                                 "SH_Integrate_Ylm_Al_zpos",
                                                 "SH_Integrate_Ylm_Al_zneg" };
        
        V_RETURN(g_pParaboloidEffect->SetTechnique("EvaluateSHCubemap"));
        V_RETURN(g_pParaboloidEffect->SetTexture(EvalTextureNames[face], pEvalSHFunction[face]));
        V_RETURN(g_pParaboloidEffect->SetTexture("SH_Coefficients", g_pIrradianceSHCoefficients));
        
        if ( SUCCEEDED(g_pParaboloidEffect->Begin(&uPasses,0)) )
        {
            V_RETURN(g_pParaboloidEffect->BeginPass(face));
            V_RETURN( Draw2D( FALSE ) );
            V_RETURN(g_pParaboloidEffect->EndPass());
            g_pParaboloidEffect->End();
        }
        pRTSurf->Release();
    }
    return S_OK;
}

// Render to a Paraboloid then convolve.
void CSceneView::RenderParaboloidConvolveView()
{
	// Don't update the view if it's not time to yet.
	if ( m_fNextRender > g_fTime )
	{
		return;
	}

	HRESULT hr;

	// Get the old render target (the current backbuffer).
	LPDIRECT3DSURFACE9 pRTOld = NULL;
	V( g_pd3dDevice->GetRenderTarget( 0, &pRTOld ) );

	// If the device has a depth/stencil buffer, use the cube map's instead.
	LPDIRECT3DSURFACE9 pDSOld = NULL;
	if ( SUCCEEDED( g_pd3dDevice->GetDepthStencilSurface( &pDSOld ) ) )
	{
		V( g_pd3dDevice->SetDepthStencilSurface( m_RenderTarget.m_pRenderTargetDepthStencil ) );
	}

	// First render the cube map to a paraboloid.
	RenderParaboloidEnvMap( g_pSkyCubeTexture );

	// Then project the new paraboloid env map into SH space.
	ProjectParaboloidToSH();
	
	// Finally convolve in Sh space, the convert back to a cube map.
	EvaluateConvolvedSH( g_pDiffuseIrradianceSkyCubeTexture, g_pLambertSHEval );

    // Restore depth-stencil buffer and render target.
    if ( pDSOld )
    {
        V( g_pd3dDevice->SetDepthStencilSurface( pDSOld ) );
        SAFE_RELEASE( pDSOld );
    }

    V( g_pd3dDevice->SetRenderTarget( 0, pRTOld ) );
    SAFE_RELEASE( pRTOld );


	// Update the time until the next update.
	m_fNextRender = g_fTime + m_fNextRenderDelta;
}

// Render the Depth for any Entities available for this view.
void CSceneView::RenderEntitiesDepth()
{
	// Just write to the depth buffer (no color).
	g_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0x0 );

	// Set the Render Depth Technique.
	g_pSkyEffect->SetTechnique( g_hRenderDepth );

	static D3DXMATRIXA16 mWorld;
	static D3DXMATRIXA16 mView;
	static D3DXMATRIXA16 mProj;
	static D3DXMATRIXA16 mWorldViewProjection;

	mProj = CSceneManager::m_pInstance->m_pCurrentView->m_matProj;
	mView = CSceneManager::m_pInstance->m_pCurrentView->m_matView;

	// Go through all the Scene's Entities and just render out the ones
	// which are allowed to render in this view.
	vector< CEntity * >::iterator iterEnt = CSceneManager::m_pInstance->m_Entities.begin();
	for ( ; iterEnt != CSceneManager::m_pInstance->m_Entities.end(); ++iterEnt )
	{
		if ( !( (*iterEnt)->m_ulSceneViewMask & m_ulViewID ) || !(*iterEnt)->GetVisible() )
		{
			continue;
		}

		CD3DModel *pModel = (*iterEnt)->GetModel();

		// Prepare the Model to be rendered.
		pModel->PrepareForRender();

		// Set the MVP Matrix.
		mWorld = (*iterEnt)->GetTransform();

		// See if they want to apply the special view hack.
		if ( (*iterEnt)->GetViewHack() )
		{
			mView._41 = mView._42 = mView._43 = 0.0f;
			mWorldViewProjection = mWorld * mView * mProj;
			mView = CSceneManager::m_pInstance->m_pCurrentView->m_matView;

			g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
		}
		else
		{
			mWorldViewProjection = mWorld * mView * mProj;
		}

		g_pSkyEffect->SetMatrix( "g_mWorldViewProjection", &mWorldViewProjection );

		// Apply the technique contained in the effect.
		UINT iPass, cPasses;
		g_pSkyEffect->Begin( &cPasses, 0 );
			for ( iPass = 0; iPass < cPasses; iPass++ )
			{
				g_pSkyEffect->BeginPass( iPass );

				pModel->Render();

				g_pSkyEffect->EndPass();
			}
		g_pSkyEffect->End();

		if ( (*iterEnt)->GetViewHack() )
		{
			g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
		}
	}

	// Reset back to normal.
	g_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0x0000000F );
}

// Render the Color for any Entities available for this view.
void CSceneView::RenderEntitiesColor()
{
	// NOTE: An effective culling mechanism should also be used to reduce drawing of
	// non-visible entities.

	// Only write color if depth matchs and no z writes.
	g_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_EQUAL );
	g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

	// Go through all the Scene's Entities and just render out the ones
	// which are allowed to render in this view.
	vector< CEntity * >::iterator iterEnt = CSceneManager::m_pInstance->m_Entities.begin();
	for ( ; iterEnt != CSceneManager::m_pInstance->m_Entities.end(); ++iterEnt )
	{
		if ( !( (*iterEnt)->m_ulSceneViewMask & m_ulViewID ) || !(*iterEnt)->GetVisible() )
		{
			continue;
		}

		CD3DModel *pModel = (*iterEnt)->GetModel();

		// Prepare the Model to be rendered.
		pModel->PrepareForRender();

		// Prepare the Entity to be rendered.
		(*iterEnt)->PrepareRender();

		// Apply the technique contained in the effect.
		UINT iPass, cPasses;
		g_pSkyEffect->Begin( &cPasses, 0 );
			for ( iPass = 0; iPass < cPasses; iPass++ )
			{
				g_pSkyEffect->BeginPass( iPass );

				pModel->Render();

				g_pSkyEffect->EndPass();
			}
		g_pSkyEffect->End();
	}

	// Reset back to normal.
	g_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
	g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
}

// Render a Fog pass for any Entities available for this view.
void CSceneView::RenderEntitiesFog()
{
	g_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_EQUAL );
	g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

	// Add the fog into the scene (so no extinction).
	g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
	g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
	g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	// Set the Render Fog Technique.
	g_pSkyEffect->SetTechnique( g_hRenderFog );

	g_pSkyEffect->SetTexture( "g_FMap", g_pFunctionFTexture );
	g_pSkyEffect->SetValue( g_hSunLightColor, &g_pSkyEntity->GetSunColor(), sizeof( D3DXVECTOR4 ) );

	static D3DXMATRIXA16 mWorld;
	static D3DXMATRIXA16 mView;
	static D3DXMATRIXA16 mProj;
	static D3DXMATRIXA16 mWorldViewProjection;

	mProj = CSceneManager::m_pInstance->m_pCurrentView->m_matProj;
	mView = CSceneManager::m_pInstance->m_pCurrentView->m_matView;

	// Go through all the Scene's Entities and just render out the ones
	// which are allowed to render in this view.
	vector< CEntity * >::iterator iterEnt = CSceneManager::m_pInstance->m_Entities.begin();
	for ( ; iterEnt != CSceneManager::m_pInstance->m_Entities.end(); ++iterEnt )
	{
		if ( !( (*iterEnt)->m_ulSceneViewMask & m_ulViewID ) || !(*iterEnt)->GetVisible() )
		{
			continue;
		}

		CD3DModel *pModel = (*iterEnt)->GetModel();

		// Prepare the Model to be rendered.
		pModel->PrepareForRender();

		// Set the MVP Matrix.
		mWorld = (*iterEnt)->GetTransform();

		// See if they want to apply the special view hack.
		if ( (*iterEnt)->GetViewHack() )
		{
			mView._41 = mView._42 = mView._43 = 0.0f;
			mWorldViewProjection = mWorld * mView * mProj;
			mView = CSceneManager::m_pInstance->m_pCurrentView->m_matView;
		}
		else
		{
			mWorldViewProjection = mWorld * mView * mProj;
		}

		D3DXMATRIXA16 mInvWorld;
		D3DXMatrixInverse( &mInvWorld, NULL, &mWorld );

		D3DXVECTOR3 vEyePos = *g_Camera.GetEyePt();

		D3DXVec3TransformNormal( &vEyePos, &vEyePos, &mInvWorld );
		
		g_pSkyEffect->SetValue( g_hLocalEyePos, &vEyePos, sizeof( D3DXVECTOR3 ) );

		D3DXVECTOR4 vLightDir = D3DXVECTOR4( 100.0f, 100.0f, 100.0f, 1.0f );
		D3DXVec4Transform( &vLightDir, &vLightDir, &mInvWorld );
		//D3DXVec3Normalize( &vLightDir, &vLightDir );
		g_pSkyEffect->SetValue( g_hSunLightDir, &vLightDir, sizeof( D3DXVECTOR4 ) );

		g_pSkyEffect->SetMatrix( "g_mWorldViewProjection", &mWorldViewProjection );

		// Apply the technique contained in the effect.
		UINT iPass, cPasses;
		g_pSkyEffect->Begin( &cPasses, 0 );
			for ( iPass = 0; iPass < cPasses; iPass++ )
			{
				g_pSkyEffect->BeginPass( iPass );

				pModel->Render();

				g_pSkyEffect->EndPass();
			}
		g_pSkyEffect->End();
	}

	// Reset back to normal.
	g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, false );
	g_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
	g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
}

// Reset to default values.
void CSceneView::Reset()
{
	m_ViewType = VIEWTYPE_NORMAL;
	m_ulViewID = VIEWID_INVALID;
	m_pParentView = NULL;
	m_SubViews.clear();
	D3DXMatrixIdentity( &m_matView );
	D3DXMatrixIdentity( &m_matProj );
	m_ulClearColor = 0x00000000;
	m_usSkipCubeFaceMask = 0x0000;
	m_fNextRender = 0.0f;
	m_fNextRenderDelta = 0.0f;
	m_bEnabled = true;
}


// Constructor.
CSceneManager::CSceneManager()
{
	m_Views.clear();
	m_iViewRecursionLevel = 0;
}

// Destructor.
CSceneManager::~CSceneManager() {}

// Add a View to the Scene.
void CSceneManager::AddView( CSceneView *pView )
{
	if ( (int)m_Views.size() >= MAX_SCENE_VIEWS )
	{
		PrintMessage( "%s: Cannot add more views than max amount (%d)", __FUNCTION__, MAX_SCENE_VIEWS );
		return;
	}

	m_Views.push_back( pView );
}

// TODO: Remove this View from the Scene.
void CSceneManager::RemoveView( CSceneView *pView )
{
	assert( !"NYI" );
}

// Add an entity to be managed.
void CSceneManager::AddEntity( CEntity *pEnt )
{
	m_Entities.push_back( pEnt );
}

// Remove this Entity from the manager.
void CSceneManager::RemoveEntity( CEntity *pEnt )
{
	vector< CEntity * >::iterator iterEnt = m_Entities.begin();
	for ( ; iterEnt != m_Entities.end(); ++iterEnt )
	{
		if ( (*iterEnt) == pEnt )
		{
			m_Entities.erase( iterEnt, iterEnt );
		}
	}
}

// Release all the managed views.
void CSceneManager::ReleaseViews()
{
	vector< CSceneView * >::iterator iterView = m_Views.begin();
	for ( ; iterView != m_Views.end(); ++iterView )
	{
		delete (*iterView);
	}
	m_Views.clear();
}

// Release all the managed entities.
void CSceneManager::ReleaseEntities()
{
	vector< CEntity * >::iterator iterEnt = m_Entities.begin();
	for ( ; iterEnt != m_Entities.end(); ++iterEnt )
	{
		delete (*iterEnt);
	}
	m_Entities.clear();
}

// Release everything in the scene (views, entities, etc...).
void CSceneManager::ReleaseScene()
{
	ReleaseViews();
	ReleaseEntities();
}

// Render the Scene.
void CSceneManager::RenderScene()
{
	m_iViewRecursionLevel = 0;

	// Render each view separately.
	vector< CSceneView * >::iterator iterView = m_Views.begin();
	for ( ; iterView != m_Views.end(); ++iterView )
	{
		// Skip disabled views.
		if ( !(*iterView)->m_bEnabled )
		{
			continue;
		}

		(*iterView)->RenderView();
	}
}

// Update the Scene.
void CSceneManager::UpdateScene()
{
	// Update all Entities.
	vector< CEntity * >::iterator iterEnts = m_Entities.begin();
	for ( ; iterEnts != m_Entities.end(); ++iterEnts )
	{
		if ( !(*iterEnts)->GetVisible() )
		{
			// FIXME: Always update, but really should check an "AlwaysUpdate" flag
			// and also only update after a certain amount of time.
			//continue;
		}

		// Update the Entity.
		(*iterEnts)->Update();
	}
}