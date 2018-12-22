#ifndef _D3DHELPER_H
#define _D3DHELPER_H

#include <d3d9.h>
#include <d3dx9.h>

#ifndef PI
#define PI	D3DX_PI
#endif

class CD3DHelper
{
public:
	CD3DHelper();
	~CD3DHelper();

	bool	Create( HWND hWnd );
	void	Destroy();
	HRESULT	Error( HRESULT hr, const char *title, const char *fmt, ... );
	HRESULT	Error( HRESULT hr, const char *title, ID3DXBuffer *pErrors, const char *filename=0 );

	void	CalcViewport( D3DVIEWPORT9 &rViewport );

	void	SetDefaults();

	bool	BeginFrame();
	void	EndFrame();

	IDirect3DVertexDeclaration9	*CreateVertexFormat( const D3DVERTEXELEMENT9 *pFmt );
	IDirect3DVertexShader9		*CreateVertexShader( const char *filename, const char *entryPoint );
	IDirect3DPixelShader9		*CreatePixelShader( const char *filename, const char *entryPoint );
	IDirect3DTexture9			*LoadTexture( const char *filename );
	IDirect3DVertexBuffer9		*CreateVertexBuffer( DWORD uVertexSize, DWORD uNumVertices );
	IDirect3DIndexBuffer9		*CreateIndexBuffer( DWORD uNumIndices );


	void	SetAlphaBlend( D3DBLEND src, D3DBLENDOP op, D3DBLEND dst );
	void	AlphaBlendEnable( bool bEnable );

	void	SetWorldMatrix( const D3DXMATRIX *pMatrix=0, BOOL bUpdate=TRUE );
	void	SetViewMatrix( const D3DXMATRIX *pMatrix=0, BOOL bInverse=FALSE, BOOL bUpdate=TRUE );
	void	SetProjectionMatrix( const D3DXMATRIX *pMatrix, BOOL bUpdate=TRUE );
	void	UpdateMatrices();

	void	GetViewMatrix( D3DXMATRIX &rOutMatrix, BOOL bGetInverse=FALSE );

	void	SetTextureClamp( DWORD uIndex, bool bClamp );
	void	SetTexture( DWORD uIndex, IDirect3DTexture9 *pTexture );
	void	SetVertexBuffer( DWORD uIndex, DWORD uStride, IDirect3DVertexBuffer9 *pVB );
	void	SetVertexFormat( IDirect3DVertexDeclaration9 *pVFmt );
	void	SetIndexBuffer( IDirect3DIndexBuffer9 *pIB );
	void	SetPixelShader( IDirect3DPixelShader9 *pPShader );
	void	SetVertexShader( IDirect3DVertexShader9 *pVShader );
	void	SetVertexShaderConstants( void *pConstants, u32 uFirst, u32 uNum );

	void	DrawTriangleStrip( DWORD uFirstIndex, DWORD uNumIndices, DWORD uFirstVertex, DWORD uNumVertices );

protected:

	void	SetupPresent( HWND hWnd );

	IDirect3D9				*m_pD3D;
	IDirect3DDevice9		*m_pDevice;
	D3DPRESENT_PARAMETERS	m_present;
	D3DVIEWPORT9			m_defaultViewport;

	D3DXMATRIX				m_mtxWorld;
	D3DXMATRIX				m_mtxView;
	D3DXMATRIX				m_mtxProj;
	D3DXMATRIX				m_mtxWorldViewProj;
};

#endif
