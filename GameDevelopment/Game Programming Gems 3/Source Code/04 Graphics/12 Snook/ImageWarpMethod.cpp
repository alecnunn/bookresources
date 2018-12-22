/****************************************************************************************\

	ImageWarpMethod.cpp

	Simple rendering method interface

	Part of the Gems III sample code library.
	Created 12/27/2001 by Greg Snook

    ------------------------------------------------------------------------------------- 
	Notes/Revisions:

	This code shows the Texture Warping method in use. It was written for clarity, not
	speed. (it's really, really slow)

\****************************************************************************************/
#include "ImageWarpMethod.h"
#include <tchar.h>
#include <stdio.h>
#include <assert.h>

struct CUSTOMVERTEX
{
    float x, y, z;
    DWORD color;
    float tu, tv;
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#define TOTAL_PLANES 1
#define TOTAL_VERTS (TOTAL_PLANES*4)
#define TOTAL_INDEXES (TOTAL_PLANES*6)

static CUSTOMVERTEX g_Vertices[TOTAL_VERTS] = 
{
    //  x      y     z     color      u     v    
    { -0.75f, -0.5f, 0.0f, 0xffffffff, 0.0f, 1.0f},
    { -0.75f, +0.5f, 0.0f, 0xffffffff, 0.0f, 0.0f},
    { +0.75f, +0.5f, 0.0f, 0xffffffff, 1.0f, 0.0f},
    { +0.75f, -0.5f, 0.0f, 0xffffffff, 1.0f, 1.0f},
};

static D3DXVECTOR3 g_CellBounds[4] =
{
				//  x      y     z     
    D3DXVECTOR3( -0.75f, 0.0f, 0.0f),
    D3DXVECTOR3( +0.75f, 0.0f, 0.0f),
    D3DXVECTOR3( -0.75f, 0.0f, 1.0f),
    D3DXVECTOR3( +0.75f, 0.0f, 1.0f),
};

HRESULT ImageWarpMethod::InitDeviceObjects( LPDIRECT3DDEVICE8 pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	// create the vertex buffer
	HRESULT hr;
	if (SUCCEEDED(hr = m_pd3dDevice->CreateVertexBuffer(sizeof(g_Vertices), D3DUSAGE_WRITEONLY, D3DFVF_CUSTOMVERTEX, D3DPOOL_MANAGED, &m_pVBuffer)))
	{
		CUSTOMVERTEX *pData;

		if (SUCCEEDED(hr = m_pVBuffer->Lock(0, sizeof(g_Vertices), (BYTE**)&pData, D3DLOCK_NOSYSLOCK)))
		{
			memcpy(pData, &g_Vertices[0], sizeof(g_Vertices));
			m_pVBuffer->Unlock();
		}
		else return (hr);
	}
	else return (hr);

    D3DXCreateTextureFromFile( m_pd3dDevice, "bunny_warp.tga", &m_pTexture);
    D3DXCreateTextureFromFile( m_pd3dDevice, "bunny_warp.tga", &m_pTextureBuffer);
    D3DXCreateTextureFromFile( m_pd3dDevice, "bunny_warp.tga", &m_pSourceImage);

	return(S_OK);
}

HRESULT ImageWarpMethod::RestoreDeviceObjects()
{
	HRESULT hr;
	CUSTOMVERTEX *pData;

	if (SUCCEEDED(hr = m_pVBuffer->Lock(0, sizeof(g_Vertices), (BYTE**)&pData, D3DLOCK_NOSYSLOCK)))
	{
		memcpy(pData, &g_Vertices[0], sizeof(g_Vertices));
		m_pVBuffer->Unlock();
	}
	else return (hr);

	return(S_OK);
}

HRESULT ImageWarpMethod::InvalidateDeviceObjects()
{
	return(S_OK);
}

HRESULT ImageWarpMethod::DeleteDeviceObjects()
{
	SAFE_RELEASE(m_pVBuffer);
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pTextureBuffer);
	SAFE_RELEASE(m_pSourceImage);
	return(S_OK);
}

HRESULT ImageWarpMethod::Render(const D3DXMATRIX& world_matrix, const D3DXMATRIX& camera_matrix, const D3DXMATRIX& proj_matrix, bool wireframe)
{
	// setup the render states
	m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE );
	m_pd3dDevice->SetRenderState(D3DRS_LIGHTING , FALSE );

	if (wireframe)
	{
		m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	}
	else
	{
		m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		m_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, 1);
		m_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

		m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND , D3DBLEND_SRCALPHA);
		m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA);
	}

	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	WarpTexture(world_matrix, camera_matrix);

    m_pd3dDevice->SetTexture( 0, m_pTexture);

	// render the object
	if (SUCCEEDED(m_pd3dDevice->SetPixelShader(0)))
	{
		if (SUCCEEDED(m_pd3dDevice->SetVertexShader(D3DFVF_CUSTOMVERTEX)))
		{
			if (SUCCEEDED(m_pd3dDevice->SetStreamSource(0,m_pVBuffer, sizeof(CUSTOMVERTEX))))
			{
				return(m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 ));
			}
		}
	}

	return(S_OK);
}

struct FixedColor
{
	long a,r,g,b;
};

void D3DToFixedColor(FixedColor& fixed, const D3DCOLOR& color)
{
	fixed.a = (color>>8) & 0x00ff0000;
	fixed.r = color& 0x00ff0000;
	fixed.g = (color<<8) & 0x00ff0000;
	fixed.b = (color<<16) & 0x00ff0000;
}

void FixedToD3DColor(D3DCOLOR& color, const FixedColor& fixed)
{
	color = (fixed.a<<8) & 0xff000000;
	color += fixed.r  & 0x00ff0000;
	color += (fixed.g>>8) & 0x0000ff00;
	color += (fixed.b>>16) & 0x000000ff;
}

void AddFixedColor(FixedColor& base, const FixedColor& addition)
{
	base.a += addition.a;
	base.r += addition.r;
	base.g += addition.g;
	base.b += addition.b;
}

void ComputeDeltaColor(FixedColor& delta, const FixedColor& start, const FixedColor& end, int steps)
{
	delta.a = (end.a - start.a)/steps;
	delta.r = (end.r - start.r)/steps;
	delta.g = (end.g - start.g)/steps;
	delta.b = (end.b - start.b)/steps;
}

inline void FastCast(int* pint, float input)
{
  __asm  fld  input
  __asm  mov  edx,pint
  __asm  FRNDINT
  __asm  fistp dword ptr [edx];
}

void ImageWarpMethod::WarpRight(D3DCOLOR* pSrc, 
								D3DCOLOR* pDest, 
								float z_scale,
								float front_ratio,
								float front_step,
								float back_ratio,
								float back_step,
								int count)
{
	FixedColor lastColor = {0,0,0,0};
	D3DCOLOR* pLastOut = pDest;

	FixedColor fixed_src;
	FixedColor deltaColor;
	D3DCOLOR src_color;
	D3DCOLOR output_color;

	float z_bounds = (1.0f/z_scale)*count;
	float depth;
	float ratio;
	float ratio_delta;
	int offset;
	unsigned char alpha;

	D3DCOLOR* pEnd = pDest+count-1;
	D3DCOLOR* pOut;

	while (count && front_ratio>0.0f)
	{
		// sample the source color and Z depth
		src_color = *pSrc;

		// extract the alpha value (our depth) and replace it with opacity
		alpha = (unsigned char)(src_color>>24);
		src_color &= 0x00ffffff;
		if (alpha < 0xf7)
		{
			if (!lastColor.a && fixed_src.a)
			{
				src_color |= 0x80000000;
			}
			else
			{
				src_color |= 0xff000000;
			}
		}

		// convert the alpha value into a depth scale
		depth = (float)alpha/255.0f;
		
		// compute the offset ratio
		ratio_delta = back_ratio-front_ratio;
		ratio = front_ratio + (ratio_delta*depth);

		// compute the output position
		FastCast(&offset, depth * z_bounds * ratio);
		pOut = pDest + offset;

		// make sure we clip to the edge of the texture
		if (pOut > pEnd)
		{
			pOut = pEnd;
		}

		// compute our color to fixed point values
		D3DToFixedColor(fixed_src, src_color);

		if (fixed_src.a)
		{
			if (pOut>pLastOut+1)
			{
				int span = pOut-pLastOut;
				ComputeDeltaColor(deltaColor, lastColor, fixed_src, span);

				++pLastOut;

				while(pLastOut <= pOut)
				{
					AddFixedColor(lastColor, deltaColor);
					FixedToD3DColor(output_color, lastColor);
					*(pLastOut++) = output_color;
				}
			}
			else
			{
				*pOut = src_color;
			}
		}
		pLastOut = pOut;
		lastColor = fixed_src;

		front_ratio += front_step;
		back_ratio += back_step;
		++pDest;
		++pSrc;
		--count;
	}
}

void ImageWarpMethod::WarpLeft(D3DCOLOR* pSrc, 
								D3DCOLOR* pDest, 
								float z_scale,
								float front_ratio,
								float front_step,
								float back_ratio,
								float back_step,
								int count)
{
	FixedColor lastColor = {0x00ff0000,0x00ff0000,0x00ff0000,0x00ff0000};
	D3DCOLOR* pLastOut = pDest;

	FixedColor fixed_src;
	FixedColor deltaColor;
	D3DCOLOR src_color;
	D3DCOLOR output_color;

	float z_bounds = (1.0f/z_scale)*count;
	float depth;
	float ratio;
	float ratio_delta;
	int offset;
	unsigned char alpha;

	D3DCOLOR* pEnd = pDest-count+1;
	D3DCOLOR* pOut;

	while (count && front_ratio<0.0f)
	{
		// sample the source color and Z depth
		src_color = *pSrc;

/*		// extract the alpha value (our depth) and replace it with opacity
		alpha = (unsigned char)(src_color>>24);
		if (alpha < 0xf7)
		{
			src_color |= 0xff000000;
		}
		else
		{
			src_color &= 0x00ffffff;
		}
*/
		// extract the alpha value (our depth) and replace it with opacity
		alpha = (unsigned char)(src_color>>24);
		src_color &= 0x00ffffff;
		if (alpha < 0xf7)
		{
			if (!lastColor.a && fixed_src.a)
			{
				src_color |= 0x80000000;
			}
			else
			{
				src_color |= 0xff000000;
			}
		}

		// convert the alpha value into a depth scale
		depth = (float)alpha/255.0f;
		
		// compute the offset ratio
		ratio_delta = back_ratio-front_ratio;
		ratio = front_ratio + (ratio_delta*depth);

		// compute the output position
		FastCast(&offset, depth * z_bounds * ratio);
		pOut = pDest + offset;

		// make sure we clip to the edge of the texture
		if (pOut < pEnd)
		{
			pOut = pEnd;
		}

		// compute our color to fixed point values
		D3DToFixedColor(fixed_src, src_color);

		if (fixed_src.a)
		{
			if (pOut<pLastOut-1)
			{
				int span = pLastOut-pOut;
				ComputeDeltaColor(deltaColor, lastColor, fixed_src, span);

				--pLastOut;

				while(pLastOut >= pOut)
				{
					AddFixedColor(lastColor, deltaColor);
					FixedToD3DColor(output_color, lastColor);
					*(pLastOut--) = output_color;
				}
			}
			else
			{
				*pOut = src_color;
			}
		}
		pLastOut = pOut;
		lastColor = fixed_src;

		front_ratio -= front_step;
		back_ratio -= back_step;
		--pDest;
		--pSrc;
		--count;
	}
}

void ImageWarpMethod::WarpTexture(const D3DXMATRIX& world_matrix, const D3DXMATRIX& camera_matrix)
{
	D3DXMATRIX view_matrix;

	D3DXMatrixMultiply(&view_matrix, &camera_matrix, &world_matrix);

    HRESULT hr;
	D3DSURFACE_DESC destDesc;
	D3DSURFACE_DESC srcDesc;
	D3DLOCKED_RECT dest;
	D3DLOCKED_RECT src;

	// get descriptions of our surfaces
	m_pTextureBuffer->GetLevelDesc(0, &destDesc);
	m_pSourceImage->GetLevelDesc(0, &srcDesc);

	// make sure all surfaces are identical size and format
	assert(destDesc.Format == srcDesc.Format);
	assert(destDesc.Format == D3DFMT_A8R8G8B8);

	assert(destDesc.Width == srcDesc.Width);
	assert(destDesc.Height == srcDesc.Height);

	
	// find our bounding box on the XZ plane in camera space
	D3DXVECTOR3 frontLeft, frontRight, backLeft, backRight;

	D3DXVec3TransformCoord(&frontLeft, &g_CellBounds[0], &view_matrix);
	D3DXVec3TransformCoord(&frontRight, &g_CellBounds[1], &view_matrix);
	D3DXVec3TransformCoord(&backLeft, &g_CellBounds[2], &view_matrix);
	D3DXVec3TransformCoord(&backRight, &g_CellBounds[3], &view_matrix);

	// compute the depth of our bounds
	float z_depth = backLeft.z - frontLeft.z;

	// treating these new coordinates as vectors from the camera to the corners of the bounding box,
	// convert each to an offset ratio of X over Z. Meaning, for every unit of depth (z) offset the pixel by the ratio(x)
	float ratio_frontLeft = -frontLeft.x/ frontLeft.z;
	float ratio_frontRight = -frontRight.x/ frontRight.z;
	float ratio_backLeft = -backLeft.x/ backLeft.z;
	float ratio_backRight = -backRight.x/ backRight.z;

	// find the stepping of the ratios across the front and back of the bounding box
	// in terms of the pixel size of the texture we will be applying
	float front_ratioStep = (ratio_frontRight - ratio_frontLeft)/(float)destDesc.Width;
	float back_ratioStep = (ratio_backRight - ratio_backLeft)/(float)destDesc.Width;

	int width = destDesc.Width;
	int height = destDesc.Height;

	// lock the surfaces we need
	if( SUCCEEDED( hr = m_pTextureBuffer->LockRect(0, &dest, 0, 0)))
	{
		if( SUCCEEDED( hr = m_pSourceImage->LockRect(0, &src, 0, D3DLOCK_READONLY )))
		{
			D3DCOLOR* pSrc = (D3DCOLOR*)src.pBits;
			D3DCOLOR* pDest = (D3DCOLOR*)dest.pBits;

			// warp each horizontal scanline of the texture
			for (int y=0;y<height;++y)
			{
				memset(pDest, 0, sizeof(D3DCOLOR)*width);

				if (ratio_frontLeft > 0)
				{
					WarpRight(pSrc, 
							pDest, 
							z_depth,
							ratio_frontLeft,
							front_ratioStep,
							ratio_backLeft,
							back_ratioStep,
							width);
				}
				pSrc += width;
				pDest += width;

			
				if (ratio_frontRight < 0)
				{
					WarpLeft(pSrc, 
							pDest,
							z_depth,
							ratio_frontRight,
							front_ratioStep,
							ratio_backRight,
							back_ratioStep,
							width);
				}				
			}

			m_pSourceImage->UnlockRect(0);
		}
		m_pTextureBuffer->UnlockRect(0);
	}

	// swap textures
	void* pTemp = (void*)m_pTextureBuffer;
	m_pTextureBuffer = m_pTexture;
	m_pTexture = (LPDIRECT3DTEXTURE8)pTemp;

}