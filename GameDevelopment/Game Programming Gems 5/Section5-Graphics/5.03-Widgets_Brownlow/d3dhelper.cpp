#include <stdafx.h>
#include <assert.h>

//--------------------------------------------------------
//	constructor
//--------------------------------------------------------
CD3DHelper::CD3DHelper()
{
	memset(this,0,sizeof(*this));
}

CD3DHelper::~CD3DHelper()
{
	Destroy();
}

//--------------------------------------------------------
//	create/destroy
//--------------------------------------------------------
bool CD3DHelper::Create( HWND hWnd )
{
	bool	bRet = false;
	HRESULT	hr;

	do
	{
		m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
		if( m_pD3D==0 )
			break;

		SetupPresent(hWnd);
		hr = m_pD3D->CreateDevice(	D3DADAPTER_DEFAULT,
									D3DDEVTYPE_HAL,
									hWnd,
									D3DCREATE_MULTITHREADED|D3DCREATE_HARDWARE_VERTEXPROCESSING,
									&m_present,
									&m_pDevice);
// Note From Kim Pallister: I didn't test the change, but the above flags could most
//  likely be changed to D3DCREATE_MIXED_VERTEXPROCESSING. The multi-threaded is only 
//  needed if multiple threads plan on accessing the D3D device, and the Mixed vertex 
//  processing should work on a wider array of hardware.

		if( hr!=D3D_OK )
		{
			Error(hr,"CD3DHelper::Create","CreateDevice");
			break;
		}

		SetDefaults();

		bRet = true;

	} while( 0 );

	return bRet;
}

void CD3DHelper::Destroy()
{
	RELEASE(m_pDevice);
	RELEASE(m_pD3D);
}

//--------------------------------------------------------
//	set defaults
//--------------------------------------------------------
void CD3DHelper::SetDefaults()
{
	D3DXMatrixPerspectiveFovLH(	&m_mtxProj,
								PI*0.25f,
								1.0f,
								1.0f,
								1000.0f );

	// set identity world and view matrices
	SetWorldMatrix(0,FALSE);
	SetViewMatrix(0,FALSE,FALSE);
	SetProjectionMatrix(&m_mtxProj,TRUE);

	// set a fullscreen viewport
	if( m_pDevice )
	{
		CalcViewport(m_defaultViewport);
		m_pDevice->SetViewport(&m_defaultViewport);

		m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
		m_pDevice->SetRenderState(D3DRS_ALPHAREF,16);
		m_pDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATEREQUAL);
		m_pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
		m_pDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
		m_pDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);

		// trilinear filtering
		for( DWORD i=0;i<4;i++ )
		{
			m_pDevice->SetSamplerState(i,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
			m_pDevice->SetSamplerState(i,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
			m_pDevice->SetSamplerState(i,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);
		}
	
	}
}

//--------------------------------------------------------
//	setup the present parameters
//--------------------------------------------------------
void CD3DHelper::SetupPresent( HWND hWnd )
{
	memset(&m_present,0,sizeof(m_present));
	m_present.BackBufferFormat			= D3DFMT_UNKNOWN;
	m_present.MultiSampleType			= D3DMULTISAMPLE_NONE;
	m_present.SwapEffect				= D3DSWAPEFFECT_DISCARD;
	m_present.Windowed					= TRUE;
	m_present.EnableAutoDepthStencil	= TRUE;
	m_present.AutoDepthStencilFormat	= D3DFMT_D24X8;
	m_present.hDeviceWindow = hWnd;
}

//--------------------------------------------------------
//	display error text
//--------------------------------------------------------
HRESULT	CD3DHelper::Error( HRESULT hr, const char *title, const char *fmt, ... )
{
	char	sztitle[256];
	if( hr!=D3D_OK )
	{
		sprintf(sztitle,"Error in %s",title);

		MessageBox(m_present.hDeviceWindow,fmt,sztitle,MB_OK);
		assert(0);
	}
	return hr;
}

HRESULT	CD3DHelper::Error( HRESULT hr, const char *title, ID3DXBuffer *pErrors, const char *filename )
{
	char	*buffer;
	char	msg[1024];
	if( pErrors )
	{
		buffer = (char*)pErrors->GetBufferPointer();
		if( filename )
		{
			sprintf(msg,"In file: %s\n\r%s",filename,buffer);
			buffer = msg;
		}

		Error(hr,title,buffer);
	}
	return hr;
}

//--------------------------------------------------------
//	creating vertex formats
//--------------------------------------------------------
IDirect3DVertexDeclaration9	*CD3DHelper::CreateVertexFormat( const D3DVERTEXELEMENT9 *pFmt )
{
	HRESULT						hr;
	IDirect3DVertexDeclaration9	*pRet = 0;

	do
	{
		assert(m_pDevice!=0);
		if( m_pDevice==0 )
			break;

		hr = m_pDevice->CreateVertexDeclaration(pFmt,&pRet);
		if( hr!=D3D_OK )
		{
			Error(hr,"CreateVertexDeclaration","CreateVertexDeclaration");
			break;
		}

	} while(0);

	return pRet;
}

//--------------------------------------------------------
//	creating vertex shaders
//--------------------------------------------------------
IDirect3DVertexShader9	*CD3DHelper::CreateVertexShader( const char *filename, const char *entryPoint )
{
	ID3DXBuffer				*pShader = 0;
	ID3DXBuffer				*pErrors = 0;
	IDirect3DVertexShader9	*pRet = 0;
	DWORD					*pToken;
	HRESULT					hr;
	D3DXMACRO				aMacros[2];

	do
	{
		assert(m_pDevice!=0);
		if( m_pDevice==0 )
			break;

		aMacros[0].Name = "VSHADER";
		aMacros[0].Definition = "1";
		aMacros[1].Name = 0;
		aMacros[1].Definition = 0;

		hr = D3DXCompileShaderFromFile(	filename,
										aMacros,
										NULL,
										entryPoint,
										"vs_1_1",
										0,
										&pShader,
										&pErrors,
										NULL );
		if( hr!=D3D_OK )
		{
			Error(hr,"CreateVertexShader",pErrors,filename);
			break;
		}

		pToken = (DWORD*)pShader->GetBufferPointer();
		hr = m_pDevice->CreateVertexShader(pToken,&pRet);
		if( hr!=D3D_OK )
		{
			Error(hr,"CreateVertexShader","Error in file %s",filename);
			break;
		}
	} while(0);

	RELEASE(pShader);
	RELEASE(pErrors);
	return pRet;
}

//--------------------------------------------------------
//	creating pixel shaders
//--------------------------------------------------------
IDirect3DPixelShader9	*CD3DHelper::CreatePixelShader( const char *filename, const char *entryPoint )
{
	ID3DXBuffer				*pShader = 0;
	ID3DXBuffer				*pErrors = 0;
	IDirect3DPixelShader9	*pRet = 0;
	DWORD					*pToken;
	HRESULT					hr;
	D3DXMACRO				aMacros[2];

	do
	{
		assert(m_pDevice!=0);
		if( m_pDevice==0 )
			break;

		aMacros[0].Name = "PSHADER";
		aMacros[0].Definition = "1";
		aMacros[1].Name = 0;
		aMacros[1].Definition = 0;

		hr = D3DXCompileShaderFromFile(	filename,
										aMacros,
										NULL,
										entryPoint,
										"ps_1_1",
										0,
										&pShader,
										&pErrors,
										NULL );
		if( hr!=D3D_OK )
		{
			Error(hr,"CreatePixelShader",pErrors,filename);
			break;
		}

		pToken = (DWORD*)pShader->GetBufferPointer();
		hr = m_pDevice->CreatePixelShader(pToken,&pRet);
		if( hr!=D3D_OK )
		{
			Error(hr,"CreatePixelShader","Error in file %s",filename);
			break;
		}
	} while(0);

	RELEASE(pShader);
	RELEASE(pErrors);
	return pRet;
}

//--------------------------------------------------------
//	creating vertex buffers
//--------------------------------------------------------
IDirect3DVertexBuffer9 *CD3DHelper::CreateVertexBuffer( DWORD uVertexSize, DWORD uNumVertices )
{
	HRESULT					hr;
	IDirect3DVertexBuffer9	*pRet = 0;

	do
	{
		assert(m_pDevice!=0);
		if( m_pDevice==0 )
			break;

		hr = m_pDevice->CreateVertexBuffer(	uVertexSize*uNumVertices,
											D3DUSAGE_WRITEONLY,
											0,
											D3DPOOL_MANAGED,
											&pRet,
											NULL);
		if( hr!=D3D_OK )
		{
			Error(hr,"CreateVertexBuffer","CreateVertexBuffer");
			break;
		}

	} while(0);

	return pRet;
}

//--------------------------------------------------------
//	creating index buffers
//--------------------------------------------------------
IDirect3DIndexBuffer9 *CD3DHelper::CreateIndexBuffer( DWORD uNumIndices )
{
	HRESULT					hr;
	IDirect3DIndexBuffer9	*pRet = 0;

	do
	{
		assert(m_pDevice!=0);
		if( m_pDevice==0 )
			break;

		hr = m_pDevice->CreateIndexBuffer(	uNumIndices*2,
											D3DUSAGE_WRITEONLY,
											D3DFMT_INDEX16,
											D3DPOOL_DEFAULT,
											&pRet,
											NULL);
		if( hr!=D3D_OK )
		{
			Error(hr,"CreateIndexBuffer","CreateIndexBuffer");
			break;
		}

	} while(0);

	return pRet;
}

//--------------------------------------------------------
//	creating textures
//--------------------------------------------------------
IDirect3DTexture9 *CD3DHelper::LoadTexture( const char *filename )
{
	HRESULT				hr;
	IDirect3DTexture9	*pRet = 0;

	do
	{
		hr = D3DXCreateTextureFromFileEx(	m_pDevice,
											filename,
											0,0,0,0,
											D3DFMT_A8R8G8B8,
											D3DPOOL_MANAGED,
											D3DX_DEFAULT,
											D3DX_FILTER_BOX,
											0xff000000,
											0,0,&pRet);
		if( hr!=D3D_OK )
		{
			Error(hr,"LoadTexture","D3DXCreateTextureFromFile");
			break;
		}

	} while(0);

	return pRet;
}

//--------------------------------------------------------
//	calculate the full-screen viewport
//--------------------------------------------------------
void CD3DHelper::CalcViewport( D3DVIEWPORT9 &rViewport )
{
	RECT	rect;

	::GetClientRect(m_present.hDeviceWindow,&rect);
	rViewport.X = rect.left;
	rViewport.Y = rect.top;
	rViewport.Width = rect.right - rect.left;
	rViewport.Height = rect.bottom - rect.top;
	rViewport.MinZ   = 0.0f;
	rViewport.MaxZ	 = 1.0f;
}

//--------------------------------------------------------
//	begin/end frame
//--------------------------------------------------------
bool CD3DHelper::BeginFrame()
{
	HRESULT	hr;
	bool	bRet = false;

	do
	{
		if( m_pDevice==0 )
			break;

		hr = m_pDevice->BeginScene();
		if( hr!=D3D_OK )
			break;

		SetDefaults();

		// clear the screen
		m_pDevice->Clear(0,0,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,0,1.0f,0);

		bRet = true;

	} while(0);
		
	return bRet;
}

void CD3DHelper::EndFrame()
{
	if( m_pDevice )
	{
		m_pDevice->EndScene();
		m_pDevice->Present(0,0,0,0);
	}
}

//--------------------------------------------------------
//	matrices
//--------------------------------------------------------
void CD3DHelper::SetWorldMatrix( const D3DXMATRIX *pMatrix, BOOL bUpdate )
{
	if( pMatrix )
	{
		m_mtxWorld = *pMatrix;
	}
	else
	{
		D3DXMatrixIdentity(&m_mtxWorld);
	}
	if( bUpdate )
	{
		UpdateMatrices();
	}
}

void CD3DHelper::GetViewMatrix( D3DXMATRIX &rOut, BOOL bInvert )
{
	if( bInvert )
	{
		D3DXMatrixInverse(&rOut,0,&m_mtxView);
	}
	else
	{
		rOut = m_mtxView;
	}
}

void CD3DHelper::SetViewMatrix( const D3DXMATRIX *pMatrix, BOOL bInverseNeeded, BOOL bUpdate )
{
	if( pMatrix )
	{
		if( bInverseNeeded )
		{
			D3DXMatrixInverse(&m_mtxView,0,pMatrix);
		}
		else
		{
			m_mtxView = *pMatrix;
		}
	}
	else
	{
		D3DXMatrixIdentity(&m_mtxView);
	}
	if( bUpdate )
	{
		UpdateMatrices();
	}
}

void CD3DHelper::SetProjectionMatrix( const D3DXMATRIX *pMatrix, BOOL bUpdate )
{
	m_mtxProj = *pMatrix;
	if( bUpdate )
	{
		UpdateMatrices();
	}
}

void CD3DHelper::UpdateMatrices()
{
	D3DXMATRIX	WMxVM;

	D3DXMatrixMultiply( &WMxVM, &m_mtxWorld, &m_mtxView );
	D3DXMatrixMultiply( &m_mtxWorldViewProj, &WMxVM, &m_mtxProj );

	if( m_pDevice )
	{
		m_pDevice->SetTransform(D3DTS_PROJECTION,&m_mtxProj);
		m_pDevice->SetTransform(D3DTS_WORLDMATRIX(0),&m_mtxWorld);
		m_pDevice->SetTransform(D3DTS_VIEW,&m_mtxView);

		m_pDevice->SetVertexShaderConstantF(0,(float*)&m_mtxWorldViewProj,4);
	}
}

//--------------------------------------------------------
//	setting textures
//--------------------------------------------------------
void CD3DHelper::SetTexture( DWORD uIndex, IDirect3DTexture9 *pTexture )
{
	HRESULT	hr;

	if( m_pDevice )
	{
		hr = m_pDevice->SetTexture(uIndex,pTexture);
		if( hr!=D3D_OK )
		{
			Error(hr,"SetTexture","SetTexture");
			return;
		}
	}
}

void CD3DHelper::SetTextureClamp( DWORD uIndex, bool bClamp )
{
	if( m_pDevice )
	{
		D3DTEXTUREADDRESS	addr = bClamp?D3DTADDRESS_CLAMP:D3DTADDRESS_MIRROR;
		m_pDevice->SetSamplerState(uIndex,D3DSAMP_ADDRESSU,addr);
		m_pDevice->SetSamplerState(uIndex,D3DSAMP_ADDRESSV,addr);
	}
}

//--------------------------------------------------------
//	setting vertex buffer
//--------------------------------------------------------
void CD3DHelper::SetVertexBuffer( DWORD uIndex, DWORD uStride, IDirect3DVertexBuffer9 *pVB )
{
	HRESULT	hr;

	if( m_pDevice )
	{
		hr = m_pDevice->SetStreamSource(uIndex,pVB,0,uStride);
		if( hr!=D3D_OK )
		{
			Error(hr,"SetVertexBuffer","SetStreamSource");
			return;
		}
	}
}

//--------------------------------------------------------
//	setting index buffers
//--------------------------------------------------------
void CD3DHelper::SetIndexBuffer( IDirect3DIndexBuffer9 *pIB )
{
	HRESULT	hr;

	if( m_pDevice )
	{
		hr = m_pDevice->SetIndices(pIB);
		if( hr!=D3D_OK )
		{
			Error(hr,"SetIndexBuffer","SetIndices");
			return;
		}
	}
}

//--------------------------------------------------------
//	setting vertex format
//--------------------------------------------------------
void CD3DHelper::SetVertexFormat( IDirect3DVertexDeclaration9 *pVFmt )
{
	HRESULT	hr;

	if( m_pDevice )
	{
		hr = m_pDevice->SetVertexDeclaration(pVFmt);
		if( hr!=D3D_OK )
		{
			Error(hr,"SetVertexFormat","SetVertexDeclaration");
			return;
		}
	}
}

//--------------------------------------------------------
//	setting vertex shader constants
//--------------------------------------------------------
void CD3DHelper::SetVertexShaderConstants( void *pConstants, u32 uFirst, u32 uNum )
{
	HRESULT	hr;

	if( m_pDevice )
	{
		hr = m_pDevice->SetVertexShaderConstantF(uFirst,(float*)pConstants,uNum);
		if( hr!=D3D_OK )
		{
			Error(hr,"SetVertexShaderConstants","SetVertexShaderConstantF");
			return;
		}
	}
}

//--------------------------------------------------------
//	setting vertex shader
//--------------------------------------------------------
void CD3DHelper::SetVertexShader( IDirect3DVertexShader9 *pVShader )
{
	HRESULT	hr;

	if( m_pDevice )
	{
		hr = m_pDevice->SetVertexShader(pVShader);
		if( hr!=D3D_OK )
		{
			Error(hr,"SetVertexShader","SetVertexShader");
			return;
		}
	}
}

//--------------------------------------------------------
//	setting pixel shader
//--------------------------------------------------------
void CD3DHelper::SetPixelShader( IDirect3DPixelShader9 *pPShader )
{
	HRESULT	hr;

	if( m_pDevice )
	{
		hr = m_pDevice->SetPixelShader(pPShader);
		if( hr!=D3D_OK )
		{
			Error(hr,"SetPixelShader","SetPixelShader");
			return;
		}
	}
}

//--------------------------------------------------------
//	alpha blending
//--------------------------------------------------------
void	CD3DHelper::SetAlphaBlend( D3DBLEND src, D3DBLENDOP op, D3DBLEND dst )
{
	if( m_pDevice )
	{
		m_pDevice->SetRenderState(D3DRS_SRCBLEND,src);
		m_pDevice->SetRenderState(D3DRS_BLENDOP,op);
		m_pDevice->SetRenderState(D3DRS_DESTBLEND,dst);
	}
}
void	CD3DHelper::AlphaBlendEnable( bool bEnable )
{
	if( m_pDevice )
	{
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,bEnable);
	}
}

//--------------------------------------------------------
//	draw a triangle strip
//--------------------------------------------------------
void CD3DHelper::DrawTriangleStrip( DWORD uFirstIndex, DWORD uNumIndices, DWORD uFirstVertex, DWORD uNumVertices )
{
	HRESULT	hr;

	if( m_pDevice )
	{
		hr = m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,0,uFirstVertex,uNumVertices,uFirstIndex,uNumIndices-2);
		if( hr!=D3D_OK )
		{
			Error(hr,"DrawTriangleStrip","DrawIndexedPrimitive");
			return;
		}
	}
}

