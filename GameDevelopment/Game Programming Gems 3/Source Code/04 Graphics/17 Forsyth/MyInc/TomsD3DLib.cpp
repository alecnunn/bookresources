

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






// Sets the FVF type
// Explicitely not inlined, because it's a comparitively expensive op, and
// we don't want it to vanish from Vtune's view.
void MyFVFPointer::SetFVFType ( DWORD dwType )
{
	int i;

	// Initialise everything.
	dwFVFType = dwType;
	iOffsetPosition = -1;
	iOffsetRHW = -1;
	iOffsetBlendWeights = -1;
	iOffsetNormal = -1;
	iOffsetPsize = -1;
	iOffsetColour[0] = -1;
	iOffsetColour[1] = -1;
	iSizeOfVertex = 0;
	iNumBlendWeights = 0;
	iNumTexCoords = 0;
	for ( i = 0; i < 8; i++ )
	{
		iOffsetTexCoord[i] = -1;
		iSizeTexCoord[i] = -1;
	}

	if ( dwType == 0 )
	{
		// NULL type - not set up yet.
		return;
	}

	// Now find the components that are present.
	int iOffset = 0;

	// Bits 1,2,3 all code for a position type.
	switch ( dwType & D3DFVF_POSITION_MASK )
	{
	case 0:
		// Invalid.
		ASSERT ( FALSE );
		break;
	case D3DFVF_XYZ:
		iOffsetPosition = iOffset;
		iOffset += 3 * sizeof ( float );
		iNumBlendWeights = 0;
		break;
	case D3DFVF_XYZRHW:
		ASSERT ( ( dwType & D3DFVF_NORMAL ) == 0 );
		iOffsetPosition = iOffset;
		iOffset += 3 * sizeof ( float );
		iOffsetRHW = iOffset;
		iOffset += 1 * sizeof ( float );
		iNumBlendWeights = 0;
		break;
	case D3DFVF_XYZB1:
		iOffsetPosition = iOffset;
		iOffset += 3 * sizeof ( float );
		iOffsetBlendWeights = iOffset;
		iNumBlendWeights = 1;
		iOffset += iNumBlendWeights * sizeof ( float );
		break;
	case D3DFVF_XYZB2:
		iOffsetPosition = iOffset;
		iOffset += 3 * sizeof ( float );
		iOffsetBlendWeights = iOffset;
		iNumBlendWeights = 2;
		iOffset += iNumBlendWeights * sizeof ( float );
		break;
	case D3DFVF_XYZB3:
		iOffsetPosition = iOffset;
		iOffset += 3 * sizeof ( float );
		iOffsetBlendWeights = iOffset;
		iNumBlendWeights = 3;
		iOffset += iNumBlendWeights * sizeof ( float );
		break;
	case D3DFVF_XYZB4:
		iOffsetPosition = iOffset;
		iOffset += 3 * sizeof ( float );
		iOffsetBlendWeights = iOffset;
		iNumBlendWeights = 4;
		iOffset += iNumBlendWeights * sizeof ( float );
		break;
	case D3DFVF_XYZB5:
		iOffsetPosition = iOffset;
		iOffset += 3 * sizeof ( float );
		iOffsetBlendWeights = iOffset;
		iNumBlendWeights = 5;
		iOffset += iNumBlendWeights * sizeof ( float );
		break;
	}

	if ( dwType & D3DFVF_RESERVED1 )
	{
		// A special-case D3DLVERTEX. Make sure it's correct.
		ASSERT ( dwType == D3DFVF_LVERTEX );
		// No offset to mark, just a DWORD to skip.
		iOffset += sizeof ( float );
	}

	if ( dwType & D3DFVF_NORMAL )
	{
		iOffsetNormal = iOffset;
		iOffset += 3 * sizeof ( float );
	}
#if DXVERSION==8
	if ( dwType & D3DFVF_PSIZE )
	{
		iOffsetPsize = iOffset;
		iOffset += 1 * sizeof ( float );
	}
#endif
	if ( dwType & D3DFVF_DIFFUSE )
	{
		iOffsetColour[0] = iOffset;
		iOffset += 1 * sizeof ( DWORD );
	}
	if ( dwType & D3DFVF_SPECULAR )
	{
		iOffsetColour[1] = iOffset;
		iOffset += 1 * sizeof ( DWORD );
	}

	// Now do the textures.
	// Slight hardwiring because the D3D headers don't give quite enough info.
#if D3DFVF_TEXCOORDSIZE1(3) != ( 0x3 << ( 3 * 2 + 16 ) )
#error Hardwired FVF format flags do not match headers.
#endif
	iNumTexCoords = ( dwType & D3DFVF_TEXCOUNT_MASK ) >> D3DFVF_TEXCOUNT_SHIFT;
	DWORD dwTemp = dwType >> 16;
	for ( i = 0; i < iNumTexCoords; i++ )
	{
		// Set up the offsets.
		int iSize = -1;
		switch ( dwTemp & 0x3 )
		{
		case D3DFVF_TEXTUREFORMAT1: iSize = 1; break;
		case D3DFVF_TEXTUREFORMAT2: iSize = 2; break;
		case D3DFVF_TEXTUREFORMAT3: iSize = 3; break;
		case D3DFVF_TEXTUREFORMAT4: iSize = 4; break;
		default:
			// Don't be silly.
			ASSERT ( FALSE );
			break;
		}

		iOffsetTexCoord[i] = iOffset;
		iSizeTexCoord[i] = iSize;

		iOffset += iSizeTexCoord[i] * sizeof ( float );

		dwTemp >>= 2;
	}

	// The size of the whole vertex.
	iSizeOfVertex = iOffset;
#if DXVERSION==8
	// D3DX is almost useful!
	ASSERT ( (unsigned)iSizeOfVertex == D3DXGetFVFVertexSize ( dwFVFType ) );
#endif
}



#ifndef EXTERNAL_PROGS

// Returns a string describing the FVF.
// Not cached - obviously this is very slow.
String MyFVFPointer::GetTextName ( void )
{
	String ret;

	if ( iOffsetPosition >= 0 )
	{
		ret += "XYZ,";
	}
	if ( iOffsetRHW >= 0 )
	{
		ret += "RHW,";
	}
	if ( iOffsetBlendWeights > 0 )
	{
		ret += "B";
		ret += ('0' + (char)iNumBlendWeights );
		ret += ",";
	}
	if ( iOffsetNormal >= 0 )
	{
		ret += "N,";
	}
	if ( iOffsetPsize >= 0 )
	{
		ret += "PS,";
	}
	if ( iOffsetColour[0] >= 0 )
	{
		ret += "C0,";
	}
	if ( iOffsetColour[1] >= 0 )
	{
		ret += "C1,";
	}
	for ( int i = 0; i < iNumTexCoords; i++ )
	{
		ASSERT ( iSizeTexCoord[i] >= 1 );
		ret += "T";
		ret += ('0' + (char)iSizeTexCoord[i] );
		ret += ",";
	}
	// Ditch the final comma.
	ASSERT ( ret.length() > 0 );
	ret[ret.length()-1] = '\0';

	return ret;
}

#endif //#ifndef EXTERNAL_PROGS



