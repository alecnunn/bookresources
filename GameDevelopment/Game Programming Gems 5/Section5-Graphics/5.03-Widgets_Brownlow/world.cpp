#include <stdafx.h>
#include <assert.h>
#include "world.h"

//-----------------------------------------------------------------
//	world vertices
//-----------------------------------------------------------------
typedef struct
{
	float		pos[3];
	float		normal[3];
	float		uv[2];
	D3DCOLOR	color;

} WORLDVTX;

const static D3DVERTEXELEMENT9 _worldvtx[] =
{
	{	0,		offsetof(WORLDVTX,pos),		D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,	0	},
	{	0,		offsetof(WORLDVTX,normal),	D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_NORMAL,	0	},
	{	0,		offsetof(WORLDVTX,uv),		D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,	0	},
	{	0,		offsetof(WORLDVTX,color),	D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_COLOR,		0	},
	D3DDECL_END()
};

//-----------------------------------------------------------------
//	texture filenames
//-----------------------------------------------------------------
const static char *texFilenames[3] =
{
	"data\\grass.jpg",
	"data\\rock.jpg",
	"data\\dirt.jpg"
};


//-----------------------------------------------------------------
//	load a binary file
//-----------------------------------------------------------------
void *LoadBinFile( const char *filename, u32 &uOutLength )
{
	FILE	*fp;
	void	*ret = 0;

	fp = fopen(filename,"rb");
	if( fp!=0 )
	{
		// get the file length
		fseek(fp,0,SEEK_END);
		uOutLength = ftell(fp);
		fseek(fp,0,SEEK_SET);

		// allocate memory
		ret = malloc(uOutLength);
		if( ret )
		{	// read the file
			fread(ret,uOutLength,1,fp);
		}
		// close the file
		fclose(fp);
	}
	return ret;
}


//---------------------------------------------------------------
//	world constructor
//---------------------------------------------------------------
CWorld::CWorld()
{
	memset(this,0,sizeof(*this));
	m_uSize[0] = m_uSize[1] = 128;
	m_uNumVertices = m_uSize[0]*m_uSize[1];
	m_uNumIndices  = (m_uSize[0]+1)*2*(m_uSize[1]-1) - 2;

	m_vBoundingBox[0][0] = -300.0f;
	m_vBoundingBox[0][1] = 0.0f;
	m_vBoundingBox[0][2] = -300.0f;
	m_vBoundingBox[1][0] = 300.0f;
	m_vBoundingBox[1][1] = 100.0f;
	m_vBoundingBox[1][2] = 300.0f;
	m_vStep[0] = (m_vBoundingBox[1][0]-m_vBoundingBox[0][0])/(float)(m_uSize[0]-1);
	m_vStep[1] = (m_vBoundingBox[1][1]-m_vBoundingBox[0][1])/255.0f;
	m_vStep[2] = (m_vBoundingBox[1][2]-m_vBoundingBox[0][2])/(float)(m_uSize[1]-1);
}
CWorld::~CWorld()
{
	Destroy();
}

//---------------------------------------------------------------
//	destroy the world
//---------------------------------------------------------------
void CWorld::Destroy()
{
	RELEASE(m_pVShader);
	RELEASE(m_pPShader);
	RELEASE(m_pVFmt);
	RELEASE(m_pVB);
	RELEASE(m_pIB);
	RELEASE(m_pTextures[0]);
	RELEASE(m_pTextures[1]);
	RELEASE(m_pTextures[2]);

	FREE(m_pHeightMap);
	FREE(m_pColorMap);
}

//---------------------------------------------------------------
//	create the world
//---------------------------------------------------------------
bool CWorld::Create( CD3DHelper *pD3D )
{
	bool	bRet = false;
	u32		i;
	do
	{
		m_pD3D = pD3D;

		// load the textures
		for( i=0;i<3;i++ )
		{
			m_pTextures[i] = m_pD3D->LoadTexture(texFilenames[i]);
			if( m_pTextures[i]==0 )
				break;
		}

		// create the vertex format
		m_pVFmt = m_pD3D->CreateVertexFormat(_worldvtx);
		if( m_pVFmt==0 )
			break;

		// create the vertex shader
		m_pVShader = m_pD3D->CreateVertexShader("data\\scape.hlsl","VS_Main");
		if( m_pVShader==0 )
			break;

		// create the pixel shader
		m_pPShader = m_pD3D->CreatePixelShader("data\\scape.hlsl","PS_Main");
		if( m_pPShader==0 )
			break;

		// create the vertex buffer
		m_pVB = m_pD3D->CreateVertexBuffer(sizeof(WORLDVTX),m_uNumVertices);
		if( m_pVB==0 )
			break;

		// create the index buffer
		m_pIB = m_pD3D->CreateIndexBuffer(m_uNumIndices);
		if( m_pIB==0 )
			break;

		if( !LoadWorld() )
			break;


		bRet = true;

	} while(0);

	if( !bRet )
	{
		Destroy();
	}

	return bRet;
}

//---------------------------------------------------------------
//	load the world
//---------------------------------------------------------------
bool CWorld::LoadWorld()
{
	u32		uHMLength,uCMLength;

	// load the height map
	m_pHeightMap = (BYTE*)LoadBinFile("data\\heights.raw",uHMLength);
	if( m_pHeightMap==0 || uHMLength!=m_uNumVertices )
		return false;

	// load the color map
	m_pColorMap = (BYTE*)LoadBinFile("data\\color.raw",uCMLength);
	if( m_pColorMap==0 || uCMLength!=m_uNumVertices*3 )
		return false;

	// normalize all of the colors
	NormalizeColorMap();

	// now create the vertices for the world
	FillVertexBuffer();
	FillIndexBuffer();

	return true;
}

//---------------------------------------------------------------
//	normalize the color map
//---------------------------------------------------------------
void CWorld::NormalizeColorMap()
{
	u32		i;
	u32		sum;
	float	oosqrt;
	u8		*pColor;

	pColor = m_pColorMap;
	for( i=0;i<m_uNumVertices;i++,pColor+=3 )
	{
		sum =	pColor[0]*pColor[0] +
				pColor[1]*pColor[1] +
				pColor[2]*pColor[2];

		if( sum!=0 )
		{
			oosqrt = 255.0f/sqrtf((float)sum);

			pColor[0] = (BYTE)(pColor[0]*oosqrt);
			pColor[1] = (BYTE)(pColor[1]*oosqrt);
			pColor[2] = (BYTE)(pColor[2]*oosqrt);
		
		}
		else
		{
			pColor[0] = 255;
			pColor[1] = 0;
			pColor[2] = 0;
		}
	}
}

//---------------------------------------------------------------
//	fill the vertex buffer
//---------------------------------------------------------------
void CWorld::FillVertexBuffer()
{
	WORLDVTX	*ptr;
	u8			*pColor;
	u8			*pHeight[3];		// prev, this, next
	u32			i,j;
	float		x,z;
	HRESULT		hr;

	D3DXVECTOR3	temp[3];

	hr = m_pVB->Lock(0,0,(void**)&ptr,0);
	if( hr!=D3D_OK )
	{
		m_pD3D->Error(hr,"CWorld::FillVertexBuffer","IDirect3DVertexBuffer9::Lock");
		return;
	}

	temp[0][2] = 0.0f;
	temp[0][0] = 2*m_vStep[0];
	temp[1][0] = 0.0f;
	temp[1][2] = 2*m_vStep[2];

	pHeight[0] = pHeight[1] = m_pHeightMap;
	pHeight[2] = m_pHeightMap + m_uSize[0];

	pColor  = m_pColorMap;
	for( i=0,z=m_vBoundingBox[0][2];i<m_uSize[1];i++,z+=m_vStep[2] )
	{

		for( j=0,x=m_vBoundingBox[0][0];j<m_uSize[0];j++,pColor+=3,x+=m_vStep[0] )
		{
			ptr->pos[0] = x;
			ptr->pos[1] = m_vBoundingBox[0][1] + m_vStep[1]*pHeight[1][j];
			ptr->pos[2] = z;
			ptr->uv[0]  = (float)j/2.0f;
			ptr->uv[1]  = (float)i/2.0f;
			ptr->color  = D3DCOLOR_ARGB(255,pColor[0],pColor[1],pColor[2]);

			if( j==0 )
			{
				temp[0][1] = (pHeight[1][j] - pHeight[1][j+1])*m_vStep[1];
			}
			else if( j==m_uSize[0]-1 )
			{
				temp[0][1] = (pHeight[1][j-1] - pHeight[1][j])*m_vStep[1];
			}
			temp[1][1] = (pHeight[0][j] - pHeight[2][j])*m_vStep[1];

			D3DXVec3Cross(&temp[2],&temp[1],&temp[0]);
			D3DXVec3Normalize(&temp[2],&temp[2]);

			ptr->normal[0] = temp[2][0];
			ptr->normal[1] = temp[2][1];
			ptr->normal[2] = temp[2][2];
			
			ptr++;
		}

		pHeight[0] = pHeight[1];
		pHeight[1] = pHeight[2];
		if( i!=m_uSize[1]-1 )
		{
			pHeight[2] += m_uSize[0];
		}
	}

	m_pVB->Unlock();
}

//---------------------------------------------------------------
//	fill the index buffer
//---------------------------------------------------------------
void CWorld::FillIndexBuffer()
{
	u16			*ptr;
	HRESULT		hr;
	u32			i,j,idx;

	hr = m_pIB->Lock(0,0,(void**)&ptr,0);
	if( hr!=D3D_OK )
	{
		m_pD3D->Error(hr,"CWorld::FillIndexBuffer","IDirect3DIndexBuffer9::Lock");
		return;
	}

	for( i=1,idx=0;i<m_uSize[1];i++ )
	{
		if( i!=1 )
		{	// repeat last vertex and next for degenerates
			ptr[0] = ptr[-1];
			ptr[1] = (SHORT)idx;
			ptr   += 2;
		}
		for( j=0;j<m_uSize[0];j++,idx++ )
		{
			*(ptr++) = (SHORT)idx;
			*(ptr++) = (SHORT)(idx + m_uSize[0]);
		}

	}

	m_pIB->Unlock();
}


//---------------------------------------------------------------
//	draw the world
//---------------------------------------------------------------
void CWorld::Draw()
{
	if( m_pD3D )
	{
		// identity world matrix
		m_pD3D->SetWorldMatrix(0);

		// set vertex format
		m_pD3D->SetVertexFormat(m_pVFmt);

		// set vertex and pixel shaders
		m_pD3D->SetVertexShader(m_pVShader);
		m_pD3D->SetPixelShader(m_pPShader);

		// set vertex and index buffers
		m_pD3D->SetVertexBuffer(0,sizeof(WORLDVTX),m_pVB);
		m_pD3D->SetIndexBuffer(m_pIB);

		// set textures
		m_pD3D->SetTexture(0,m_pTextures[0]);
		m_pD3D->SetTexture(1,m_pTextures[1]);
		m_pD3D->SetTexture(2,m_pTextures[2]);

		m_pD3D->SetTextureClamp(0,false);
		m_pD3D->SetTextureClamp(1,false);
		m_pD3D->SetTextureClamp(2,false);

		// draw the world
		m_pD3D->DrawTriangleStrip(0,m_uNumIndices,0,m_uNumVertices);
	}
}

//---------------------------------------------------------------
//	get the bounding box of the world
//---------------------------------------------------------------
void CWorld::GetBoundingBox( D3DXVECTOR3 extents[2] )
{
	extents[0] = m_vBoundingBox[0];
	extents[1] = m_vBoundingBox[1];
}

//---------------------------------------------------------------
//	convert a position to index/fraction
//---------------------------------------------------------------
bool CWorld::GetPosition( const float *pPos, s32 *pIndex, float *pFraction )
{
	pFraction[0] = (pPos[0] - m_vBoundingBox[0][0])/m_vStep[0];
	pFraction[1] = (pPos[2] - m_vBoundingBox[0][2])/m_vStep[2];

	pIndex[0] = (s32)pFraction[0];
	pIndex[1] = (s32)pFraction[1];

	pFraction[0] -= pIndex[0];
	pFraction[1] -= pIndex[1];

	if( pIndex[0]<0 || pIndex[1]<0 )
		return false;
	if( pIndex[0]>=(s32)m_uSize[0] || pIndex[1]>=(s32)m_uSize[0] )
		return false;
	
	return true;
}

//---------------------------------------------------------------
//	get the height of a position in the world
//---------------------------------------------------------------
float CWorld::GetHeight( const float *pPosition )
{
	s32		anIndex[2];
	float	afFraction[2];
	u8		*pHeight;
	float	fHeight[4];

	if( GetPosition(pPosition,anIndex,afFraction) )
	{
		pHeight = &m_pHeightMap[anIndex[0] + anIndex[1]*m_uSize[0]];

		fHeight[0] = (float)pHeight[0];
		fHeight[1] = (float)pHeight[1];
		pHeight   += m_uSize[0];
		fHeight[2] = (float)pHeight[0];
		fHeight[3] = (float)pHeight[1];

		return m_vBoundingBox[0][1] + m_vStep[1]*Bilinear(fHeight,afFraction);
	
	}
	return 0;
}


//---------------------------------------------------------------
//	get the color of a position in the world
//---------------------------------------------------------------
float CWorld::GetColorChannel( const float *pPosition, u32 uChannel )
{
	s32		anIndex[2];
	float	afFraction[2];
	u8		*pColor;
	float	aSrcColor[4];

	assert(uChannel<3);

	if( GetPosition(pPosition,anIndex,afFraction) )
	{
		pColor = &m_pColorMap[uChannel + (anIndex[0] + anIndex[1]*m_uSize[0])*3];

		aSrcColor[0] = (float)pColor[0]/255.0f;
		aSrcColor[1] = (float)pColor[3]/255.0f;
		pColor += m_uSize[0]*3;
		aSrcColor[2] = (float)pColor[0]/255.0f;
		aSrcColor[3] = (float)pColor[3]/255.0f;

		return Bilinear(aSrcColor,afFraction);
	}

	return 0;
}