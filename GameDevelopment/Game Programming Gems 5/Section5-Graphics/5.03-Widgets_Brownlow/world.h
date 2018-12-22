#ifndef _WORLD_H
#define _WORLD_H

class CWorld
{
public:
	CWorld();
	~CWorld();

	bool		Create( CD3DHelper *pD3D );
	void		Destroy();

	void		Draw();
	void		GetBoundingBox( D3DXVECTOR3 extents[2] );

	float		GetHeight( const float *pPos );
	float		GetColorChannel( const float *pPos, u32 uChannel );

protected:
	bool		LoadWorld();
	void		NormalizeColorMap();
	void		FillVertexBuffer();
	void		FillIndexBuffer();

	bool		GetPosition( const float *pPos, s32 *pIndex, float *pFraction );

protected:
	CD3DHelper					*m_pD3D;
	IDirect3DVertexDeclaration9	*m_pVFmt;
	IDirect3DVertexShader9		*m_pVShader;
	IDirect3DPixelShader9		*m_pPShader;
	IDirect3DVertexBuffer9		*m_pVB;
	IDirect3DIndexBuffer9		*m_pIB;
	IDirect3DTexture9			*m_pTextures[3];

	u8							*m_pHeightMap;
	u8							*m_pColorMap;

	u32							m_uSize[2];
	u32							m_uNumVertices;
	u32							m_uNumIndices;

	D3DXVECTOR3					m_vBoundingBox[2];
	D3DXVECTOR3					m_vStep;

};

#endif
