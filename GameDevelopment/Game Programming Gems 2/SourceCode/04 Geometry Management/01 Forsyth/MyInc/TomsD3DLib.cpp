/* Copyright (C) Tom Forsyth, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Tom Forsyth, 2001"
 */


#include "TomsD3DLib.h"




// Used by the universal blit.
// By TomF.
#if DXVERSION==7
DWORD SamplePoint ( int iX, int iY, DDSURFACEDESC2 &ddsdSrc )
#else
DWORD SamplePoint ( int iX, int iY, int iBitsPerPixel, D3DSURFACE_DESC &ddsdSrc, D3DLOCKED_RECT &locked_rect )
#endif
{
	// Clamp to texture size.
	if ( iX < 0 )
	{
		iX = 0;
	}
#if DXVERSION==7
	else if ( iX >= (signed)ddsdSrc.dwWidth )
	{
		iX = (signed)ddsdSrc.dwWidth - 1;
	}
#else
	else if ( iX >= (signed)ddsdSrc.Width )
	{
		iX = (signed)ddsdSrc.Width - 1;
	}
#endif
	if ( iY < 0 )
	{
		iY = 0;
	}
#if DXVERSION==7
	else if ( iY >= (signed)ddsdSrc.dwHeight )
	{
		iY = (signed)ddsdSrc.dwHeight - 1;
	}
#else
	else if ( iY >= (signed)ddsdSrc.Height )
	{
		iY = (signed)ddsdSrc.Height - 1;
	}
#endif


	DWORD dwRes;

#if DXVERSION==7
	switch ( ddsdSrc.ddpfPixelFormat.dwRGBBitCount )
	{
	case 8:
		dwRes = (DWORD)( *( (BYTE*)ddsdSrc.lpSurface + (iX) + (iY) * ddsdSrc.lPitch ) );
		break;
	case 15:
	case 16:
		dwRes = (DWORD)( *(WORD*)( (BYTE*)ddsdSrc.lpSurface + (iX) * 2 + (iY) * ddsdSrc.lPitch ) );
		break;
	case 32:
		dwRes = *(DWORD*)( (BYTE*)ddsdSrc.lpSurface + (iX) * 4 + (iY) * ddsdSrc.lPitch );
		break;
	default:
		ASSERT ( FALSE );
		break;
	}
#else
	switch ( iBitsPerPixel )
	{
	case 8:
		dwRes = (DWORD)( *(WORD*)( (BYTE*)locked_rect.pBits + (iX) + (iY) * locked_rect.Pitch ) );
		break;
	case 16:
		dwRes = (DWORD)( *(WORD*)( (BYTE*)locked_rect.pBits + (iX) * 2 + (iY) * locked_rect.Pitch ) );
		break;
	case 32:
		dwRes = *(DWORD*)( (BYTE*)locked_rect.pBits + (iX) * 4 + (iY) * locked_rect.Pitch );
		break;
	default:
		ASSERT ( FALSE );
		break;
	}
#endif

	return dwRes;

}


#if DXVERSION==7
// A universal blit. Slow, but handles most formats, and does conversions and different filter modes, etc.
// By Tomf.
BOOL UniversalBlit ( LPDIRECTDRAWSURFACEn pddsSrc, LPDIRECTDRAWSURFACEn pddsDst,
							UblitFilter ufMin,
							UblitFilter ufMag )
{
	DDSURFACEDESC2	ddsdSrc, ddsdDst;

	D3DInit ( ddsdSrc );
	D3DInit ( ddsdDst );

	if ( pddsSrc == NULL )
	{
		ASSERT ( FALSE );
		return FALSE;
	}
	if ( pddsDst == NULL )
	{
		ASSERT ( FALSE );
		return FALSE;
	}

	pddsSrc->GetSurfaceDesc ( &ddsdSrc );
	pddsDst->GetSurfaceDesc ( &ddsdDst );

	DDPIXELFORMAT ddpfSrc = ddsdSrc.ddpfPixelFormat;
	DDPIXELFORMAT ddpfDst = ddsdDst.ddpfPixelFormat;

	// Only handle RGB formats at the moment.
	ASSERT ( ( ddpfSrc.dwFlags & DDPF_RGB ) != 0 );
	ASSERT ( ( ddpfDst.dwFlags & DDPF_RGB ) != 0 );

	// Are minning or magging?
	UblitFilter ufFilter;
	if ( ddsdSrc.dwWidth * ddsdSrc.dwHeight >= ddsdDst.dwWidth * ddsdDst.dwHeight )
	{
		// Minimising.
		ufFilter = ufMin;
	}
	else
	{
		// Magnifying.
		ufFilter = ufMag;
	}


	DWORD dwPixelSizeSrc;
	DWORD dwPixelSizeDst;

	switch ( ddpfSrc.dwRGBBitCount )
	{
	case 8:
		dwPixelSizeSrc = 1;	// In bytes.
		break;
	case 15:
	case 16:
		dwPixelSizeSrc = 2;	// In bytes.
		break;
	case 32:
		dwPixelSizeSrc = 4;	// In bytes.
		break;
	default:
		// Nothing else handled for now.
		ASSERT ( FALSE );
		return FALSE;
		break;
	}


	switch ( ddpfDst.dwRGBBitCount )
	{
	case 8:
		dwPixelSizeDst = 1;	// In bytes.
		break;
	case 15:
	case 16:
		dwPixelSizeDst = 2;	// In bytes.
		break;
	case 32:
		dwPixelSizeDst = 4;	// In bytes.
		break;
	default:
		// Nothing else handled for now.
		ASSERT ( FALSE );
		return FALSE;
		break;
	}


	HRESULT hres;
	hres = pddsSrc->Lock ( NULL, &ddsdSrc, DDLOCK_READONLY | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL );
	ASSERT ( SUCCEEDED ( hres ) );
	hres = pddsDst->Lock ( NULL, &ddsdDst, DDLOCK_WRITEONLY | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL );
	ASSERT ( SUCCEEDED ( hres ) );


	float fXScale = (float)ddsdSrc.dwWidth / (float)ddsdDst.dwWidth;
	float fYScale = (float)ddsdSrc.dwHeight / (float)ddsdDst.dwHeight;
	BYTE *pbDstLine = (BYTE*)ddsdDst.lpSurface;
	for ( int iY = 0; iY < (signed)ddsdDst.dwHeight; iY++ )
	{
		BYTE *pbDst = pbDstLine;
		pbDstLine += ddsdDst.lPitch;
		for ( int iX = 0; iX < (signed)ddsdDst.dwWidth; iX++ )
		{
			float fXSrc = (float)iX * fXScale;
			float fYSrc = (float)iY * fYScale;
			int iXSrc = (int)fXSrc;
			int iYSrc = (int)fYSrc;

			FloatColour fcRes;

			switch ( ufFilter )
			{
			case UBLIT_FILTER_POINT:
				// Just sample from the top-left corner of the sample area.
				fcRes = ConvertPixelDataToFloat ( SamplePoint ( iXSrc, iYSrc, ddsdSrc ), ddsdSrc.ddpfPixelFormat );
				break;
			case UBLIT_FILTER_BILINEAR:
				// Sample the four points and filter them.
				FloatColour fc00 = ConvertPixelDataToFloat ( SamplePoint ( iXSrc + 0, iYSrc + 0, ddsdSrc ), ddsdSrc.ddpfPixelFormat );
				FloatColour fc01 = ConvertPixelDataToFloat ( SamplePoint ( iXSrc + 0, iYSrc + 1, ddsdSrc ), ddsdSrc.ddpfPixelFormat );
				FloatColour fc10 = ConvertPixelDataToFloat ( SamplePoint ( iXSrc + 1, iYSrc + 0, ddsdSrc ), ddsdSrc.ddpfPixelFormat );
				FloatColour fc11 = ConvertPixelDataToFloat ( SamplePoint ( iXSrc + 1, iYSrc + 1, ddsdSrc ), ddsdSrc.ddpfPixelFormat );
				float fXLerp = fXSrc - (float)( iXSrc );
				float fYLerp = fYSrc - (float)( iYSrc );
				ASSERT ( ( fXLerp >= 0.0f ) && ( fXLerp < 1.0f ) );
				ASSERT ( ( fYLerp >= 0.0f ) && ( fYLerp < 1.0f ) );
				fc00 = fc00 * ( 1.0f - fXLerp ) + fc10 * ( fXLerp );
				fc01 = fc01 * ( 1.0f - fXLerp ) + fc11 * ( fXLerp );
				fcRes = fc00 * ( 1.0f - fYLerp ) + fc01 * ( fYLerp );
				break;
			}

			DWORD dwResult = ConvertFloatToPixelData ( fcRes, ddpfDst );

			switch ( dwPixelSizeDst )
			{
			case 2:
				*((WORD *)pbDst) = (WORD)( dwResult & 0xffff );
				break;
			case 4:
				*((DWORD *)pbDst) = dwResult;
				break;
			}

			pbDst += dwPixelSizeDst;
		}
	}

	hres = pddsSrc->Unlock ( NULL );
	ASSERT ( SUCCEEDED ( hres ) );
	hres = pddsDst->Unlock ( NULL );
	ASSERT ( SUCCEEDED ( hres ) );

	return TRUE;
}
#endif //#if DXVERSION==7


