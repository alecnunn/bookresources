// Drawing code

#include "stdafx.h"

extern int g_useTextures;

bool DrawCell( cGameCell* pCell )
{
	static sLitVertex v[16];

	int i,j;

	LPDIRECT3DDEVICE8 pDevice = Graphics()->GetDevice();

	if( g_useTextures )
	{
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	}

	Graphics()->SetWorldMatrix( matrix4::Identity );

	int nPolys = pCell->m_polyList.size();
	for( i=0; i<nPolys; i++ )
	{
		/**
		 * Set up the texture.
		 */

		if( g_useTextures )
		{
			cTexture* pTex = (cTexture*)Resources()->Get( 
				pCell->m_polyList[i].m_texID );
			pDevice->SetTexture( 0, pTex->GetTexture() );
		}

		/**
		 * Cycle around the polygon grabbing the vertices
		 */
		int nVerts = pCell->m_polyList[i].m_nVerts;
		for( j=0; j<nVerts; j++ )
		{
			v[j] = sLitVertex(
				pCell->m_ptList[ pCell->m_polyList[i].m_vList[j].m_ind ],
				pCell->m_polyList[i].m_vList[j].m_col,
				0,
				pCell->m_polyList[i].m_vList[j].m_u,
				pCell->m_polyList[i].m_vList[j].m_v );
		}

		/**
		 * Draw the polygon as a triangle fan.
		 */
		/*pDevice->DrawPrimitive( 
			D3DPT_TRIANGLEFAN,
			D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1, 
			v,
			pCell->m_polyList[i].m_nVerts,
			0 );*/
		pDevice->SetVertexShader( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1 );

		pDevice->DrawPrimitiveUP(
			D3DPT_TRIANGLEFAN,
			pCell->m_polyList[i].m_nVerts - 2,
			v,
			sizeof( sLitVertex ) );

	}


	return true;
}


bool DrawEnt( cGameEnt* pEnt )
{
	if( pEnt->GetStatus() == gesMortis )
	{
		DebPrint("Mortis object found");
	}

	/**
	 * Switch based on the segment constants
	 */
	switch( GetIDSegment( pEnt->GetID() ) )
	{

	case c_spawnSegment:
		// don't draw spawn ents.
		{
			pEnt->RebuildMatrix();
			matrix4 mat = pEnt->GetMatrix();
			cModel* pModel = (cModel*)Resources()->Get(303);

			if( g_useTextures )
			{
				cTexture* pTex = (cTexture*)Resources()->Get(111);
				Graphics()->GetDevice()->SetTexture( 0, pTex->GetTexture() );
			}

			pModel->Draw( mat );
			break;
		}
		break;

	case c_playerSegment:
		// If it isn't us, Then draw it.
		if( pEnt->GetID() != GetClientID() )
		{
			pEnt->RebuildMatrix();
			matrix4 mat = pEnt->GetMatrix();
			cModel* pModel = (cModel*)Resources()->Get(303);

			if( g_useTextures )
			{
				cTexture* pTex = (cTexture*)Resources()->Get(111);
				Graphics()->GetDevice()->SetTexture( 0, pTex->GetTexture() );
			}
			
			pModel->Draw( mat );
			break;
		}
		break;

	case c_projSegment:
		{
			if( pEnt->GetStatus() == gesAlive )
			{
				pEnt->RebuildMatrix();
				matrix4 mat = pEnt->GetMatrix();
				cModel* pModel = (cModel*)Resources()->Get(304);

				if( g_useTextures )
				{
					cTexture* pTex = (cTexture*)Resources()->Get(112);
					Graphics()->GetDevice()->SetTexture( 0, pTex->GetTexture() );
				}
					
				pModel->Draw( mat );
			}
		}
		break;

	default:
		LogPrint("Bad object to draw");
		break;
	}
	return true;
}


void InitLights()
{
	LPDIRECT3DDEVICE8 pDevice = Graphics()->GetDevice();
    
    sLight light;

	// Light 0
	light = sLight::Directional( 
		point3(0,-4,2).Normalized(),
		0.7f * color3::White,
		color3::Black,
		0.5f * color3::White);

    // Set the light
    pDevice->SetLight( 0, &light );
	pDevice->LightEnable(0, TRUE);

    sMaterial mat( 
		0.f, 
		color3(0.8f,0.8f,0.8f),
		color3(0.4f,0.4f,0.4f),
		color3(0.5f,0.5f,0.5f) );

    pDevice->SetMaterial(&mat);
}