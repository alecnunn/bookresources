#include "priv_precompiled.h"

/*
Copyright 2001 Bruce Dawson, Cygnus Software
For more information, visit http://www.cygnus-software.com/papers/
or e-mail: comments@cygnus-software.com

This code may be redistributed as long as this notice is kept intact.
*/

#include "csbitmap.h"

// See the header file for a full description of how this function
// behaves.
void CompositeBitmap(CSBitmap* Bottom, const CSBitmap* Top, int startX, int startY, uint8_t globalOpacity /* = 255*/)
{
	assert(Bottom);
	assert(Top);
	assert(Bottom->HasBitmap());
	assert(Top->HasBitmap());

	// If this is zero then there is nothing to do - our top bitmap will be
	// completely transparent.
	if (globalOpacity == 0)
		return;
	int topChannels = Top->GetChannels();
	int bottomChannels = Bottom->GetChannels();

	// Setup coordinates to specify our starting location on the top and
	// bottom bitmap, and the composite size, so that we can adjust these
	// as we do our clipping.
	int bottomStartX = startX;
	int bottomStartY = startY;
	int topStartX = 0;
	int topStartY = 0;
	int compWidth = Top->GetWidth();
	int compHeight = Top->GetHeight();
	// Check for being off the left edge of the Bottom bitmap.
	if (bottomStartX < 0)
	{
		// Shift over our Top and Bottom x coordinates as needed, and
		// adjust our width.
		topStartX = -bottomStartX;	// Shrink the width.
		compWidth += bottomStartX;
		bottomStartX = 0;
	}
	// Check for being off the top edge of the Bottom bitmap.
	if (bottomStartY < 0)
	{
		// Shift over our Top and Bottom t coordinates as needed, and
		// adjust our height.
		topStartY = -bottomStartY;
		compHeight += bottomStartY;	// Shrink the height.
		bottomStartY = 0;
	}

	// Check for being off the right edge of the Bottom bitmap.
	if (bottomStartX + compWidth > Bottom->GetWidth())
		compWidth = Bottom->GetWidth() - bottomStartX;
	// Check for being off the bottom edge of the Bottom bitmap.
	if (bottomStartY + compHeight > Bottom->GetHeight())
		compHeight = Bottom->GetHeight() - bottomStartY;

	RGBQUAD	palette[256];	// Needed for compositing from an eight bit bitmap.
	// Get the palette outside of the loops.
	if (topChannels == 1)
		Top->GetPalette(0, 256, palette);
	// We need to check for compWidth being zero because the memcpy()
	// optimization below will fail otherwise.
	if (compWidth <= 0)
		return;
	// compHeight may be negative or zero at this point. That's OK.
	// If it is, the loop will run zero times.
	for (int y = 0; y < compHeight; y++)
	{
		uint8_t* pSourceLine = Top->GetPixelPtr(topStartX, y + topStartY);
		uint8_t* pDestLine = Bottom->GetPixelPtr(bottomStartX, y + bottomStartY);
		if (topChannels == 1)
		{
			// Simple optimization - avoid the globalOpacity multiplications if
			// they aren't needed.
			if (globalOpacity == 255)
			{
				// Simple 8-bit case with no globalOpacity support.
				for (int x = 0; x < compWidth; x++)
				{
					// Get the appropriate palette entry for this pixel.
					RGBQUAD	SourcePixel = palette[pSourceLine[0]];
					// And copy it to the destination.
					pDestLine[RED_BITMAP_OFFSET] = SourcePixel.rgbRed;
					pDestLine[GREEN_BITMAP_OFFSET] = SourcePixel.rgbGreen;
					pDestLine[BLUE_BITMAP_OFFSET] = SourcePixel.rgbBlue;
					pDestLine += bottomChannels;
					pSourceLine += topChannels;
				}
			}
			else
			{
				// Simple 8-bit case with globalOpacity support.
				for (int x = 0; x < compWidth; x++)
				{
					// Get the appropriate palette entry for this pixel.
					RGBQUAD	SourcePixel = palette[pSourceLine[0]];
					// And copy it to the destination.
					pDestLine[RED_BITMAP_OFFSET] = pDestLine[RED_BITMAP_OFFSET] + ((SourcePixel.rgbRed - pDestLine[RED_BITMAP_OFFSET]) * globalOpacity + 127) / 255;
					pDestLine[GREEN_BITMAP_OFFSET] = pDestLine[GREEN_BITMAP_OFFSET] + ((SourcePixel.rgbGreen - pDestLine[GREEN_BITMAP_OFFSET]) * globalOpacity + 127) / 255;
					pDestLine[BLUE_BITMAP_OFFSET] = pDestLine[BLUE_BITMAP_OFFSET] + ((SourcePixel.rgbBlue - pDestLine[BLUE_BITMAP_OFFSET]) * globalOpacity + 127) / 255;
					pDestLine += bottomChannels;
					pSourceLine += topChannels;
				}
			}
		}
		else if (topChannels == 3)
		{
			if (bottomChannels == 3 && globalOpacity == 255)
			{
				// This is the one case where a raw memory copy can be
				// used. There's no alpha channel, no palette, globalOpacity
				// is 255 and the destination
				// is also 24-bits. Memcpy doubles the performance in this case.
				// Might as well make this case go REALLY fast.
				memcpy(pDestLine, pSourceLine, compWidth * topChannels);
			}
			else
			{
				// Simple optimization - avoid the globalOpacity multiplications if
				// they aren't needed.
				if (globalOpacity == 255)
				{
					// Simple 24-bit case with no globalOpacity support.
					for (int x = 0; x < compWidth; x++)
					{
						pDestLine[0] = pSourceLine[0];
						pDestLine[1] = pSourceLine[1];
						pDestLine[2] = pSourceLine[2];
						pDestLine += bottomChannels;
						pSourceLine += topChannels;
					}
				}
				else
				{
					// Simple 24-bit case with globalOpacity support.
					for (int x = 0; x < compWidth; x++)
					{
						pDestLine[0] = pDestLine[0] + ((pSourceLine[0] - pDestLine[0]) * globalOpacity + 127) / 255;
						pDestLine[1] = pDestLine[1] + ((pSourceLine[1] - pDestLine[1]) * globalOpacity + 127) / 255;
						pDestLine[2] = pDestLine[2] + ((pSourceLine[2] - pDestLine[2]) * globalOpacity + 127) / 255;
						pDestLine += bottomChannels;
						pSourceLine += topChannels;
					}
				}
			}
		}
		else	// topChannels must equal 4 - yes?
		{
			assert(topChannels == 4);
			for (int x = 0; x < compWidth; x++)
			{
				uint8_t Alpha = pSourceLine[ALPHA_BITMAP_OFFSET];
				if (Alpha)
				{
					Alpha = (Alpha * globalOpacity + 127) / 255;
					// The Alpha==255 and Alpha==0 cases are well worth
					// checking for because they are very common, and let
					// you avoid three multiplies and divides.
					if (Alpha == 255)
					{
						pDestLine[0] = pSourceLine[0];
						pDestLine[1] = pSourceLine[1];
						pDestLine[2] = pSourceLine[2];
					}
					else if (Alpha > 0)
					{
						// Simple 32-bit case.
						pDestLine[0] = pDestLine[0] + ((pSourceLine[0] - pDestLine[0]) * Alpha + 127) / 255;
						pDestLine[1] = pDestLine[1] + ((pSourceLine[1] - pDestLine[1]) * Alpha + 127) / 255;
						pDestLine[2] = pDestLine[2] + ((pSourceLine[2] - pDestLine[2]) * Alpha + 127) / 255;
					}
				}
				pDestLine += bottomChannels;
				pSourceLine += topChannels;
			}
		}
	}
}
