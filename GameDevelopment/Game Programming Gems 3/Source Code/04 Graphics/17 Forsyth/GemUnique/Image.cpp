



#include "image.h"




LPDIRECT3DDEVICEn pd3ddev;



// Generate the mipmap levels from the topmost level.
// Any previous data in the mipmaps is discarded.
void Image::GenerateMipmaps ( void )
{

	int iSizeSrc = 1 << iNumLevels;
	for ( int iLev = iNumLevels; iLev > 0; iLev-- )
	{
		Pixel *ppixSrcLine0 = ppixLevels[iLev];
		Pixel *ppixSrcLine1 = ppixLevels[iLev] + iSizeSrc;
		Pixel *ppixDst		= ppixLevels[iLev-1];

		for ( int iY = 0; iY < iSizeSrc; iY += 2 )
		{
			for ( int iX = 0; iX < iSizeSrc; iX += 2 )
			{
				for ( int iComp = 0; iComp < 4; iComp++ )
				{
					// Fix rounding.
					ui16 wTemp = 2;
					// Box filter.
					wTemp += (ui16)ppixSrcLine0[0].comp[iComp];
					wTemp += (ui16)ppixSrcLine0[1].comp[iComp];
					wTemp += (ui16)ppixSrcLine1[0].comp[iComp];
					wTemp += (ui16)ppixSrcLine1[1].comp[iComp];
					ppixDst->comp[iComp] = (ui8)( wTemp >> 2 );
				}

				ppixSrcLine0 += 2;
				ppixSrcLine1 += 2;
				ppixDst++;
			}
			ppixSrcLine0 += iSizeSrc;
			ppixSrcLine1 += iSizeSrc;
		}
		iSizeSrc >>= 1;
	}
}



/*static*/ Image *Image::LoadFromFile ( char *pcFilename, bool bTakeAlphaFromLuminance /*= false*/ )
{
	Image *pimage = NULL;

	D3DXIMAGE_INFO info;
	LPDIRECT3DTEXTURE8 ptex;
	HRESULT hres = D3DXCreateTextureFromFileEx (
						pd3ddev,
						pcFilename,
						D3DX_DEFAULT, D3DX_DEFAULT, 1,
						0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM,
						D3DX_DEFAULT, D3DX_DEFAULT, 0,
						&info, NULL, &ptex );
	if ( FAILED ( hres ) )
	{
		ASSERT ( FALSE );
	}
	else
	{
		int iSize = info.Width;
		// Must be square and power-of-two.
		ASSERT ( info.Width == info.Height );
		ASSERT ( ( iSize & ( iSize - 1 ) ) == 0 );


		int iLevels = 0;
		int iTempSize = 1;
		while ( iTempSize < iSize )
		{
			iTempSize <<= 1;
			iLevels++;
		}
		ASSERT ( ( 1 << iLevels ) == iSize );
		pimage = new Image ( iLevels );


		Pixel *ppixDst = pimage->ppixLevels[iLevels];

		D3DLOCKED_RECT rect;
		hres = ptex->LockRect ( 0, &rect, NULL, D3DLOCK_NOSYSLOCK );
		if ( SUCCEEDED ( hres ) )
		{
			char *pcDstLine = (char *)rect.pBits;
			if ( bTakeAlphaFromLuminance )
			{
				for ( int i = 0; i < iSize; i++ )
				{
					DWORD *pdwDst = (DWORD*)pcDstLine;
					pcDstLine += rect.Pitch;
					for ( int j = 0; j < iSize; j++ )
					{
						DWORD dw = *pdwDst;
						// Very very bad luminance. But it's quick.
						DWORD dwLum;
						dwLum  = ( dw >> 0 ) & 0xff;
						dwLum += ( dw >> 7 ) & 0x1fe;	// Twice the green.
						dwLum += ( dw >> 16 ) & 0xff;
						dwLum >>= 2;

						ppixDst->dword = ( dw & 0xffffff ) | ( dwLum << 24 );
						pdwDst++;
						ppixDst++;
					}
				}
			}
			else
			{
				for ( int i = 0; i < iSize; i++ )
				{
					DWORD *pdwDst = (DWORD*)pcDstLine;
					pcDstLine += rect.Pitch;
					for ( int j = 0; j < iSize; j++ )
					{
						ppixDst->dword = *pdwDst;
						pdwDst++;
						ppixDst++;
					}
				}
			}

			ptex->UnlockRect ( 0 );
		}
		SAFE_RELEASE ( ptex );

		pimage->GenerateMipmaps();
	}

	return pimage;
}



/*static*/ void Image::ChangeD3DDevice ( LPDIRECT3DDEVICEn pd3ddevNew )
{
	pd3ddev = pd3ddevNew;
}



void Image::CopyToD3DTex ( LPDIRECT3DTEXTURE8 pd3dtex )
{
	int iCurLevelSize = 1 << iNumLevels;
	for ( int i = 0; i <= iNumLevels; i++ )
	{
		// For D3D, level 0 is the biggest level.
		D3DLOCKED_RECT rect;
		HRESULT hres = pd3dtex->LockRect (
				i,
				&rect,
				NULL,
				0 /*D3DLOCK_DISCARD would use this for dynamic textures*/ );
		ASSERT ( SUCCEEDED ( hres ) );

		char *pcDst = (char*)rect.pBits;
		Pixel *ppixSrc = ppixLevels[iNumLevels-i];
		for ( int iY = 0; iY < iCurLevelSize; iY++ )
		{
			// For us, level 0 is the smallest.
			memcpy ( pcDst, ppixSrc, sizeof(Pixel) * iCurLevelSize );
			pcDst += rect.Pitch;
			ppixSrc += iCurLevelSize;
		}

		hres = pd3dtex->UnlockRect ( i );
		ASSERT ( SUCCEEDED ( hres ) );

		iCurLevelSize >>= 1;
	}
}




