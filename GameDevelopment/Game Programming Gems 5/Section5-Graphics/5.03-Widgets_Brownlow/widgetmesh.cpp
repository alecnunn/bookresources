//-----------------------------------------------------------------
//	widget mesh
//-----------------------------------------------------------------
#include <stdafx.h>
#include <assert.h>
#include "widgetmesh.h"


// instance buffer
u32				CWidgetMesh::s_uNumInstances = 0;
D3DXVECTOR4		CWidgetMesh::s_vInstanceData[WIDGET_MAXINSTANCES][2];

// vertex format and shader
IDirect3DVertexDeclaration9	*CWidgetMesh::s_pVFmt = 0;
IDirect3DVertexShader9		*CWidgetMesh::s_pVShader = 0;
IDirect3DPixelShader9		*CWidgetMesh::s_pPShader = 0;


const static D3DVERTEXELEMENT9 _widgetvtx[] =
{
	{	0,		offsetof(WIDGETVTX,pos),		D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,		0	},
	{	0,		offsetof(WIDGETVTX,uv),			D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,		0	},
	{	0,		offsetof(WIDGETVTX,mtxIndex),	D3DDECLTYPE_SHORT2,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_BLENDINDICES,	0	},
	D3DDECL_END()
};



//-----------------------------------------------------------------
//-----------------------------------------------------------------
CWidgetMesh::CWidgetMesh()
{
	memset(this,0,sizeof(*this));
}

CWidgetMesh::~CWidgetMesh()
{
	Destroy();
}

//-----------------------------------------------------------------
//-----------------------------------------------------------------
bool CWidgetMesh::Create( CD3DHelper *pD3D, u32 uNumV, const WIDGETVTX *pv, u32 uNumI, const u16 *pi, const char *textureName, float scale )
{
	bool	bRet = false;

	m_pD3D = pD3D;
	do
	{
		// load the texture
		m_pTexture = pD3D->LoadTexture(textureName);
		if( m_pTexture==0 )
			break;

		// create vertex format, vertex shader, pixel shader
		// if not already done so
		if( s_pVFmt==0 )
		{
			// create the vertex format
			s_pVFmt = m_pD3D->CreateVertexFormat(_widgetvtx);
			if( s_pVFmt==0 )
				break;

			// create the vertex shader
			s_pVShader = m_pD3D->CreateVertexShader("data\\widget.hlsl","VS_Main");
			if( s_pVShader==0 )
				break;

			// create the pixel shader
			s_pPShader = m_pD3D->CreatePixelShader("data\\widget.hlsl","PS_Main");
			if( s_pPShader==0 )
				break;
		}
		else
		{
			s_pVFmt->AddRef();
			s_pVShader->AddRef();
			s_pPShader->AddRef();
		}

		// fill the vertex buffer
		if( !CreateVertices(uNumV,pv,scale) )
		{
		}


		// fill the index buffer
		if( !CreateIndices(uNumI,pi) )
		{
		}

		SetFadeDistance();

		bRet = true;

	} while( 0 );

	return bRet;
}

void CWidgetMesh::Destroy()
{
	RELEASE(m_pIB);
	RELEASE(m_pVB);
	RELEASE(s_pVFmt);
	RELEASE(s_pVShader);
	RELEASE(s_pPShader);
	RELEASE(m_pTexture);
	memset(this,0,sizeof(*this));
}

//-----------------------------------------------------------------
//-----------------------------------------------------------------
bool CWidgetMesh::CreateVertices( u32 uNumV, const WIDGETVTX *pv, float scale )
{
	bool		bRet = FALSE;
	HRESULT		hr;
	WIDGETVTX	*ptr;
	u32			i,j;

	assert(m_pVB==0);
	do
	{
		m_uNumVerticesPerWidget = uNumV;

		// create the vertex buffer
		m_pVB = m_pD3D->CreateVertexBuffer(sizeof(WIDGETVTX),uNumV*WIDGET_MAXINSTANCES);
		if( m_pVB==0 )
			break;

		// lock the vertex buffer
		hr = m_pVB->Lock(0,0,(void**)&ptr,0);
		if( hr!=D3D_OK )
		{
			m_pD3D->Error(hr,"CWidgetMesh::CreateVertices","IDirect3DVertexBuffer9::Lock");
			break;
		}

		// copy the source vertices WIDGET_MAXINSTANCES times
		for( i=0;i<WIDGET_MAXINSTANCES;i++ )
		{
			for( j=0;j<uNumV;j++ )
			{
				ptr[j] = pv[j];
				ptr[j].pos[0] *= scale;
				ptr[j].pos[1] *= scale;
				ptr[j].pos[2] *= scale;
				ptr[j].uv[0]  *= 0.8f;
				ptr[j].mtxIndex[0] = i*2;		// correct skinning matrix
			}
			ptr += uNumV;
		}

		m_pVB->Unlock();
		bRet = TRUE;


	} while(0);

	return bRet;
}

//-----------------------------------------------------------------
//-----------------------------------------------------------------
static u32 CreateWidgetIndices( u16 *pOutput, u32 nw, u32 nv, u32 ni, const u16 *pIndices )
{
	u32		i,basev,j;
	u16		*pout;

    // base vertex = 0
    basev = 0;
    pout = pOutput;

	// for each widget
	for( i=1;i<=nw;i++ )
	{
		// copy the widget's indices, offset by base vertex
		for(j=0;j<ni;j++ )
		{
			pout[j] = pIndices[j] + basev;
		}
		pout += ni;

		// if the widget is an odd length
		if( ni&1 )
		{   // repeat the last index
			pout[0] = pout[-1];
			pout++;
		}

		// if we're not the last widget, add degenerates
		if( i!=nw )
		{
			// create degenerate tris:
			// repeat the last index
			pout[0] = pout[-1];

			// increase the base vertex
			basev += nv;

			// repeat the first vertex of the next widget
			pout[1] = pIndices[0] + basev;
			pout += 2;
		}
	}
	// return the # of indices
	return pout - pOutput;
}

//-----------------------------------------------------------------
//-----------------------------------------------------------------
bool CWidgetMesh::CreateIndices( u32 uNumI, const u16 *pIndices )
{
	bool		bRet = FALSE;
	HRESULT		hr;
	u16			*ptr;
	u32			uNumMade;
	u32			uMaxIndices;

	assert(m_pIB==0);
	assert(m_pVB!=0);
	do
	{
		m_uNumIndicesPerWidget  = (uNumI+1)&~1;	// make this an even number

		// calculate max indices needed
		uMaxIndices = m_uNumIndicesPerWidget*WIDGET_MAXINSTANCES + 2*(WIDGET_MAXINSTANCES-1);

		// create the index buffer
		m_pIB = m_pD3D->CreateIndexBuffer(uMaxIndices);
		if( m_pIB==0 )
			break;

		// lock the vertex buffer
		hr = m_pIB->Lock(0,0,(void**)&ptr,0);
		if( hr!=D3D_OK )
		{
			m_pD3D->Error(hr,"CWidgetMesh::CreateVertices","IDirect3DIndexBuffer9::Lock");
			break;
		}

		uNumMade = CreateWidgetIndices(ptr,WIDGET_MAXINSTANCES,m_uNumVerticesPerWidget,uNumI,pIndices);
		assert(uNumMade==uMaxIndices);

		m_pIB->Unlock();
		bRet = TRUE;


	} while(0);

	return bRet;
}


//-----------------------------------------------------------------
//-----------------------------------------------------------------
void	CWidgetMesh::UpdateFadePlane()
{
	D3DXMATRIX		viewMtx;
	float			fadePlane[4];
	float			fadeOrigin[3];
	float			oofadeDist;

	m_pD3D->GetViewMatrix(viewMtx,TRUE);

	fadeOrigin[0] = viewMtx(3,0) + m_fadeFar*viewMtx(2,0);
	fadeOrigin[1] = viewMtx(3,1) + m_fadeFar*viewMtx(2,1);
	fadeOrigin[2] = viewMtx(3,2) + m_fadeFar*viewMtx(2,2);

	oofadeDist = 1.0f/(m_fadeNear-m_fadeFar);
	fadePlane[0] =	viewMtx(2,0) * oofadeDist;
	fadePlane[1] =	viewMtx(2,1) * oofadeDist;
	fadePlane[2] =	viewMtx(2,2) * oofadeDist;
	fadePlane[3] =	-(fadeOrigin[0] * fadePlane[0] + 
					  fadeOrigin[1] * fadePlane[1] + 
					  fadeOrigin[2] * fadePlane[2]);

	m_pD3D->SetVertexShaderConstants(fadePlane,15,1);

}

//-----------------------------------------------------------------
//-----------------------------------------------------------------
void	CWidgetMesh::BeginDraw()
{
	if( m_pD3D )
	{

		assert(s_uNumInstances==0);
		s_uNumInstances = 0;

		// identity world matrix
		m_pD3D->SetWorldMatrix(0);

		// set vertex format
		m_pD3D->SetVertexFormat(s_pVFmt);

		// set vertex and pixel shaders
		m_pD3D->SetVertexShader(s_pVShader);
		m_pD3D->SetPixelShader(s_pPShader);

		// set vertex and index buffers
		m_pD3D->SetVertexBuffer(0,sizeof(WIDGETVTX),m_pVB);
		m_pD3D->SetIndexBuffer(m_pIB);

		// set alpha blending operation
		m_pD3D->SetAlphaBlend(D3DBLEND_SRCALPHA,D3DBLENDOP_ADD,D3DBLEND_INVSRCALPHA);
		m_pD3D->AlphaBlendEnable(true);

		m_pD3D->SetTextureClamp(0,true);

		// set textures
		m_pD3D->SetTexture(0,m_pTexture);

		UpdateFadePlane();
	}
}

//-----------------------------------------------------------------
//-----------------------------------------------------------------
void	CWidgetMesh::FlushInstances()
{
	u32		uNumV,uNumI;

	if( s_uNumInstances!=0 )
	{
		// set instance matrices
		m_pD3D->SetVertexShaderConstants(s_vInstanceData,WIDGET_FIRSTINSTANCE,s_uNumInstances*2);

		// calculate # vertices and indices
		uNumV  = m_uNumVerticesPerWidget*s_uNumInstances;
		uNumI  = m_uNumIndicesPerWidget*s_uNumInstances;
		uNumI += (s_uNumInstances-1)*2; // account for degenerates

		// draw a single strip including all the instances
		m_pD3D->DrawTriangleStrip(0,uNumI,0,uNumV);

		s_uNumInstances = 0;
	}
}

//-----------------------------------------------------------------
//-----------------------------------------------------------------
void	CWidgetMesh::EndDraw()
{
	// flush any pending instances
	FlushInstances();
	m_pD3D->AlphaBlendEnable(false);
}

//-----------------------------------------------------------------
//-----------------------------------------------------------------
void CWidgetMesh::SetFadeDistance( float fFullAlpha, float fZeroAlpha )
{
	m_fadeNear = fFullAlpha;
	m_fadeFar  = fZeroAlpha;

	if( m_fadeNear==m_fadeFar )
	{
		m_fadeFar += 0.1f;
	}
}
