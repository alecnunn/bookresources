#ifndef _WIDGETMESH_H
#define _WIDGETMESH_H

typedef struct
{
	float	pos[3];
	float	uv[2];
	s16		mtxIndex[2];		// PC needs a multiple of 4 bytes per vertex element

} WIDGETVTX;


#define	WIDGET_FIRSTINSTANCE	(16)
#define	WIDGET_MAXINSTANCES		((96-WIDGET_FIRSTINSTANCE)/2)


//------------------------------------------------------------
//	the widget mesh class
//------------------------------------------------------------
class CWidgetMesh
{
public:
			CWidgetMesh();
			~CWidgetMesh();

			bool	Create( CD3DHelper *pD3D, u32 uNumV, const WIDGETVTX *pv, u32 uNumi, const u16 *pi, const char *textureName, float scale=1.0f );
			void	Destroy();

			void	SetFadeDistance( float fFullAlpha=40.0f, float fZeroAlpha=50.0f );
			void	BeginDraw();
	inline	void	AddInstance( const float *pos, float scale, float sinAngle, float cosAngle );
			void	FlushInstances();
			void	EndDraw();

protected:

			bool	CreateVertices( u32 uNumV, const WIDGETVTX *pv, float scale );
			bool	CreateIndices( u32 uNumI, const u16 *pi );
			void	UpdateFadePlane();

protected:

	CD3DHelper				*m_pD3D;

	IDirect3DVertexBuffer9	*m_pVB;
	IDirect3DIndexBuffer9	*m_pIB;
	IDirect3DTexture9		*m_pTexture;

	u32						m_uNumVerticesPerWidget;
	u32						m_uNumIndicesPerWidget;

	float					m_fadeNear;
	float					m_fadeFar;

	// instance buffer
	static	u32				s_uNumInstances;
	static	D3DXVECTOR4		s_vInstanceData[WIDGET_MAXINSTANCES][2];

	// vertex format and shader
	static IDirect3DVertexDeclaration9	*s_pVFmt;
	static IDirect3DVertexShader9		*s_pVShader;
	static IDirect3DPixelShader9		*s_pPShader;
};

//------------------------------------------------------------
//	add an instance of the widget mesh
//------------------------------------------------------------
void CWidgetMesh::AddInstance( const float *pos, float scale, float sinAngle, float cosAngle )
{
	D3DXVECTOR4	*pInstance = s_vInstanceData[s_uNumInstances++];

	// copy this instance into the buffer
	pInstance[0][0] = pos[0];
	pInstance[0][1] = pos[1];
	pInstance[0][2] = pos[2];
	pInstance[0][3] = scale;
	pInstance[1][0] = sinAngle;
	pInstance[1][1] = cosAngle;
	pInstance[1][2] = -sinAngle;
	pInstance[1][3] = 0.0f;

	// flush if we have filled the buffer
	if( s_uNumInstances==WIDGET_MAXINSTANCES )
	{
		FlushInstances();
	}
}

#endif
