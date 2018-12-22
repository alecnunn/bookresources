

#include "filter.h"





/*static*/ Image *FilterLandscapeLayerAlpha::pimageAlpha;



/*virtual*/ void FilterLandscapeLayerAlpha::Render ( Image *pimageDest, Position2D const &p2dOffset, ui32 u32Seed )
{
	int iDiffMipMap = pimageDest->iSizeLog2 - pimage->iSizeLog2;
	// Applied image must have more or same amount of mipmapping than destination.
	// i.e. if anything has ditched mipmap levels, it must be the dest.
	ASSERT ( iDiffMipMap >= 0 );
	if ( iDiffMipMap > pimage->iNumLevels )
	{
		// This has been mipmapped out of existence.
		// Actually, this wraps, so it hasn't - we should really apply a single pixel or something.
		// FIXME.
		ASSERT ( FALSE );
		return;
	}


	int iDiffMipMapAlpha = pimageDest->iSizeLog2 - pimageAlpha->iSizeLog2;
	// Applied image must have more or same amount of mipmapping than destination.
	// i.e. if anything has ditched mipmap levels, it must be the dest.
	ASSERT ( iDiffMipMapAlpha >= 0 );
	if ( iDiffMipMapAlpha > pimageAlpha->iNumLevels )
	{
		// This has been mipmapped out of existence.
		ASSERT ( FALSE );
		return;
	}

	// Find the size of the src in dest pixels.
	int iSizeSrc   = 1 << ( pimage     ->iNumLevels - iDiffMipMap );
	int iSizeAlpha = 1 << ( pimageAlpha->iNumLevels - iDiffMipMapAlpha );
	int iSizeDst   = 1 << pimageDest->iNumLevels;
	int iSrcMask   = iSizeSrc - 1;


	// Clip the dest by the alpha rect.
	// The alpha blob is always centered on the vertex.
	Rect2D rectDst ( 0, 0, iSizeDst, iSizeDst );
	int iHalfSizeAlpha = iSizeAlpha >> 1;
	Rect2D rectAlpha = Rect2D ( -iHalfSizeAlpha, -iHalfSizeAlpha, iHalfSizeAlpha, iHalfSizeAlpha ) + p2dOffset;
	Rect2D rect = rectDst.Intersect ( rectAlpha );
	if ( rect == rect2dNull )
	{
		// Clipped out of existence.
		return;
	}
	rectAlpha = rect - p2dOffset + Position2D ( iHalfSizeAlpha, iHalfSizeAlpha );
	ASSERT ( rectAlpha.tlc.X >= 0 );
	ASSERT ( rectAlpha.tlc.Y >= 0 );
	ASSERT ( rectAlpha.brc.X <= iSizeAlpha );
	ASSERT ( rectAlpha.brc.Y <= iSizeAlpha );

	Pixel *ppixDst   = pimageDest ->ppixLevels[pimageDest ->iNumLevels                   ] + rect.tlc.X + rect.tlc.Y * iSizeDst;
	Pixel *ppixAlpha = pimageAlpha->ppixLevels[pimageAlpha->iNumLevels - iDiffMipMapAlpha] + rectAlpha.tlc.X + rectAlpha.tlc.Y * iSizeAlpha;
	int iAlphaInc = iSizeAlpha + rect.tlc.X - rect.brc.X;
	int iDstInc   = iSizeDst   + rect.tlc.X - rect.brc.X;
	ASSERT ( iAlphaInc >= 0 );
	ASSERT ( iDstInc >= 0 );


	// Find random offset of source.
	int iXSrc = GetRandom ( iSizeSrc, u32Seed, 35934 );
	int iYSrc = GetRandom ( iSizeSrc, u32Seed, 234976 );


	Pixel *ppixSrcBase = pimage->ppixLevels[pimage->iNumLevels - iDiffMipMap];
	for ( int iY = rect.tlc.Y; iY < rect.brc.Y; iY++ )
	{
		Pixel *ppixSrcLine = ppixSrcBase + ( ( iY + iYSrc ) & iSrcMask ) * iSizeSrc;
		for ( int iX = rect.tlc.X; iX < rect.brc.X; iX++ )
		{
			if ( ppixAlpha->cA != 0 )
			{
				// Something to do.
				Pixel *ppixSrc = ppixSrcLine + ( ( iX + iXSrc ) & iSrcMask );
				ui32 u32Alpha = ppixAlpha->cA;
				for ( int i = 0; i <= 2; i++ )
				{
					ui32 u32res = (ui32)ppixSrc->comp[i] * u32Alpha + ( ((ui32)ppixDst->comp[i]) << 8 ) + 0x80;
					if ( u32res >= 0xff00 )
					{
						ppixDst->comp[i] = 0xff;
					}
					else
					{
						ppixDst->comp[i] = u32res >> 8;
					}
				}
				ui32 u32res = u32Alpha + (ui32)ppixDst->cA;
				if ( u32res >= 0xff )
				{
					ppixDst->cA = 0xff;
				}
				else
				{
					ppixDst->cA = (ui8)u32res;
				}
			}
			ppixDst++;
			ppixAlpha++;
		}
		ppixDst   += iDstInc;
		ppixAlpha += iAlphaInc;
	}
}



/*virtual*/ void FilterLandscapeLayerBase::Render ( Image *pimageDest, Position2D const &p2dOffset, ui32 u32Seed )
{
	int iDiffMipMap = pimageDest->iSizeLog2 - pimage->iSizeLog2;
	// Applied image must have more or same amount of mipmapping than destination.
	// i.e. if anything has ditched mipmap levels, it must be the dest.
	ASSERT ( iDiffMipMap >= 0 );
	if ( iDiffMipMap > pimage->iNumLevels )
	{
		// This has been mipmapped out of existence.
		// Actually, this wraps, so it hasn't - we should really apply a single pixel or something.
		// FIXME.
		ASSERT ( FALSE );
		return;
	}


	int iDiffMipMapAlpha = pimageDest->iSizeLog2 - pimageAlpha->iSizeLog2;
	// Applied image must have more or same amount of mipmapping than destination.
	// i.e. if anything has ditched mipmap levels, it must be the dest.
	ASSERT ( iDiffMipMapAlpha >= 0 );
	if ( iDiffMipMapAlpha > pimageAlpha->iNumLevels )
	{
		// This has been mipmapped out of existence.
		ASSERT ( FALSE );
		return;
	}

	// Find the size of the src in dest pixels.
	int iSizeSrc   = 1 << ( pimage     ->iNumLevels - iDiffMipMap );
	int iSizeAlpha = 1 << ( pimageAlpha->iNumLevels - iDiffMipMapAlpha );
	int iSizeDst   = 1 << pimageDest->iNumLevels;
	int iSrcMask   = iSizeSrc - 1;


	// Clip the dest by the alpha rect.
	// The alpha blob is always centered on the vertex.
	Rect2D rectDst ( 0, 0, iSizeDst, iSizeDst );
	int iHalfSizeAlpha = iSizeAlpha >> 1;
	Rect2D rectAlpha = Rect2D ( -iHalfSizeAlpha, -iHalfSizeAlpha, iHalfSizeAlpha, iHalfSizeAlpha ) + p2dOffset;
	Rect2D rect = rectDst.Intersect ( rectAlpha );
	if ( rect == rect2dNull )
	{
		// Clipped out of existence.
		return;
	}
	rectAlpha = rect - p2dOffset + Position2D ( iHalfSizeAlpha, iHalfSizeAlpha );
	ASSERT ( rectAlpha.tlc.X >= 0 );
	ASSERT ( rectAlpha.tlc.Y >= 0 );
	ASSERT ( rectAlpha.brc.X <= iSizeAlpha );
	ASSERT ( rectAlpha.brc.Y <= iSizeAlpha );

	Pixel *ppixDst   = pimageDest ->ppixLevels[pimageDest ->iNumLevels                   ] + rect.tlc.X + rect.tlc.Y * iSizeDst;
	Pixel *ppixAlpha = pimageAlpha->ppixLevels[pimageAlpha->iNumLevels - iDiffMipMapAlpha] + rectAlpha.tlc.X + rectAlpha.tlc.Y * iSizeAlpha;
	int iAlphaInc = iSizeAlpha + rect.tlc.X - rect.brc.X;
	int iDstInc   = iSizeDst   + rect.tlc.X - rect.brc.X;
	ASSERT ( iAlphaInc >= 0 );
	ASSERT ( iDstInc >= 0 );


	// Find random offset of source.
	int iXSrc = GetRandom ( iSizeSrc, u32Seed, 35934 );
	int iYSrc = GetRandom ( iSizeSrc, u32Seed, 234976 );


	Pixel *ppixSrcBase = pimage->ppixLevels[pimage->iNumLevels - iDiffMipMap];
	for ( int iY = rect.tlc.Y; iY < rect.brc.Y; iY++ )
	{
		Pixel *ppixSrcLine = ppixSrcBase + ( ( iY + iYSrc ) & iSrcMask ) * iSizeSrc;
		for ( int iX = rect.tlc.X; iX < rect.brc.X; iX++ )
		{
			Pixel *ppixSrc = ppixSrcLine + ( ( iX + iXSrc ) & iSrcMask );
			ui32 u32Alpha = ppixAlpha->cA;
			for ( int i = 0; i <= 2; i++ )
			{
				ui32 u32res = (ui32)ppixSrc->comp[i] * u32Alpha + 0x80;
				ppixDst->comp[i] = u32res >> 8;
			}
			ppixDst->cA = ppixAlpha->cA;
			ppixDst++;
			ppixAlpha++;
		}
		ppixDst   += iDstInc;
		ppixAlpha += iAlphaInc;
	}
}



/*virtual*/ void FilterLandscapeLayerFinal::Render ( Image *pimageDest, Position2D const &p2dOffset, ui32 u32Seed )
{
	int iDiffMipMap = pimageDest->iSizeLog2 - pimage->iSizeLog2;
	// Applied image must have more or same amount of mipmapping than destination.
	// i.e. if anything has ditched mipmap levels, it must be the dest.
	ASSERT ( iDiffMipMap >= 0 );
	if ( iDiffMipMap > pimage->iNumLevels )
	{
		// This has been mipmapped out of existence.
		// Actually, this wraps, so it hasn't - we should really apply a single pixel or something.
		// FIXME.
		ASSERT ( FALSE );
		return;
	}


	int iDiffMipMapAlpha = pimageDest->iSizeLog2 - pimageAlpha->iSizeLog2;
	// Applied image must have more or same amount of mipmapping than destination.
	// i.e. if anything has ditched mipmap levels, it must be the dest.
	ASSERT ( iDiffMipMapAlpha >= 0 );
	if ( iDiffMipMapAlpha > pimageAlpha->iNumLevels )
	{
		// This has been mipmapped out of existence.
		ASSERT ( FALSE );
		return;
	}

	// Find the size of the src in dest pixels.
	int iSizeSrc   = 1 << ( pimage     ->iNumLevels - iDiffMipMap );
	int iSizeAlpha = 1 << ( pimageAlpha->iNumLevels - iDiffMipMapAlpha );
	int iSizeDst   = 1 << pimageDest->iNumLevels;
	int iSrcMask   = iSizeSrc - 1;


	// Clip the dest by the alpha rect.
	// The alpha blob is always centered on the vertex.
	// We don't use the alpha for rendering, but it does clip the rendering rect.
	Rect2D rectDst ( 0, 0, iSizeDst, iSizeDst );
	int iHalfSizeAlpha = iSizeAlpha >> 1;
	Rect2D rectAlpha = Rect2D ( -iHalfSizeAlpha, -iHalfSizeAlpha, iHalfSizeAlpha, iHalfSizeAlpha ) + p2dOffset;
	Rect2D rect = rectDst.Intersect ( rectAlpha );
	if ( rect == rect2dNull )
	{
		// Clipped out of existence.
		return;
	}
	Pixel *ppixDst   = pimageDest ->ppixLevels[pimageDest ->iNumLevels                   ] + rect.tlc.X + rect.tlc.Y * iSizeDst;
	int iDstInc   = iSizeDst   + rect.tlc.X - rect.brc.X;
	ASSERT ( iDstInc >= 0 );


	// Find random offset of source.
	int iXSrc = GetRandom ( iSizeSrc, u32Seed, 35934 );
	int iYSrc = GetRandom ( iSizeSrc, u32Seed, 234976 );


	Pixel *ppixSrcBase = pimage->ppixLevels[pimage->iNumLevels - iDiffMipMap];
	for ( int iY = rect.tlc.Y; iY < rect.brc.Y; iY++ )
	{
		Pixel *ppixSrcLine = ppixSrcBase + ( ( iY + iYSrc ) & iSrcMask ) * iSizeSrc;
		for ( int iX = rect.tlc.X; iX < rect.brc.X; iX++ )
		{
			if ( ppixDst->cA != 0xff )
			{
				// Something to do.
				Pixel *ppixSrc = ppixSrcLine + ( ( iX + iXSrc ) & iSrcMask );
				if ( ppixDst->cA == 0x00 )
				{
					// Just a copy.
					*ppixDst= *ppixSrc;
				}
				else
				{
					// An actual blend.
					ui16 u16Alpha = ppixDst->cA;
					ui16 u16OneMinusAlpha = 0x100 - u16Alpha;
					for ( int i = 0; i <= 2; i++ )
					{
						ui16 u16res = (ui16)ppixSrc->comp[i] * u16OneMinusAlpha + ( ((ui16)ppixDst->comp[i]) << 8 );
						ppixDst->comp[i] = u16res >> 8;
					}
				}
			}
			ppixDst++;
		}
		ppixDst   += iDstInc;
	}
}









/*virtual*/ void FilterAlphaColourLayer::Render ( Image *pimageDest, Position2D const &p2dOffset, ui32 u32Seed )
{
	int iDiffMipMapSrc = pimageDest->iSizeLog2 - pimage->iSizeLog2;
	// Applied image must have more or same amount of mipmapping than destination.
	// i.e. if anything has ditched mipmap levels, it must be the dest.
	ASSERT ( iDiffMipMapSrc >= 0 );
	if ( iDiffMipMapSrc > pimage->iNumLevels )
	{
		// This has been mipmapped out of existence.
		ASSERT ( FALSE );
		return;
	}

	// Find the size of the src in dest pixels.
	int iSizeSrc   = 1 << ( pimage->iNumLevels - iDiffMipMapSrc );
	int iSizeDst   = 1 << pimageDest->iNumLevels;

	// Total dest position of the layer.
	Position2D p2dSrc = p2dOffset + ( p2dPosition >> iDiffMipMapSrc );

	// Clip the dest by the alpha rect.
	Rect2D rectDst ( 0, 0, iSizeDst, iSizeDst );
	int iHalfSizeSrc = iSizeSrc >> 1;
	Rect2D rectSrc = Rect2D ( -iHalfSizeSrc, -iHalfSizeSrc, iHalfSizeSrc, iHalfSizeSrc ) + p2dSrc;
	Rect2D rect = rectDst.Intersect ( rectSrc );
	if ( rect == rect2dNull )
	{
		// Clipped out of existence.
		return;
	}
	rectSrc = rect - p2dSrc + Position2D ( iHalfSizeSrc, iHalfSizeSrc );
	ASSERT ( rectSrc.tlc.X >= 0 );
	ASSERT ( rectSrc.tlc.Y >= 0 );
	ASSERT ( rectSrc.brc.X <= iSizeSrc );
	ASSERT ( rectSrc.brc.Y <= iSizeSrc );

	Pixel *ppixDst = pimageDest->ppixLevels[pimageDest->iNumLevels                 ] + rect.tlc.X + rect.tlc.Y * iSizeDst;
	Pixel *ppixSrc = pimage    ->ppixLevels[pimage    ->iNumLevels - iDiffMipMapSrc] + rectSrc.tlc.X + rectSrc.tlc.Y * iSizeSrc;
	int iSrcInc = iSizeSrc + rect.tlc.X - rect.brc.X;
	int iDstInc = iSizeDst + rect.tlc.X - rect.brc.X;
	ASSERT ( iSrcInc >= 0 );
	ASSERT ( iDstInc >= 0 );

	for ( int iY = rect.tlc.Y; iY < rect.brc.Y; iY++ )
	{
		for ( int iX = rect.tlc.X; iX < rect.brc.X; iX++ )
		{
			if ( ppixSrc->cA != 0 )
			{
				// Something to do.
				if ( ppixSrc->cA == 0xff )
				{
					// Just a copy - alpha of 255 is treated as 256.
					*ppixDst = pixColour;
				}
				else
				{
					// An actual blend.
					ui16 u16Alpha = ppixSrc->cA;
					ui16 u16OneMinusAlpha = 0x100 - u16Alpha;
					for ( int i = 0; i <= 2; i++ )
					{
						ui16 u16res = (ui16)pixColour.comp[i] * u16Alpha + (ui16)ppixDst->comp[i] * u16OneMinusAlpha;
						ppixDst->comp[i] = ( u16res + 0x80 ) >> 8;
					}
				}
			}
			ppixDst++;
			ppixSrc++;
		}
		ppixDst += iDstInc;
		ppixSrc += iSrcInc;
	}
}








