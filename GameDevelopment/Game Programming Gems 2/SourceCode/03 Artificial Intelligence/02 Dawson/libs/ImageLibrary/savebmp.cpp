#include "priv_precompiled.h"

/*
Copyright 2001 Bruce Dawson, Cygnus Software
For more information, visit http://www.cygnus-software.com/papers/
or e-mail: comments@cygnus-software.com

This code may be redistributed as long as this notice is kept intact.
*/

#include "csbitmap.h"
#include "bmpdefs.h"

// This file should be upgraded to use iostreams instead of standard io. Later.

// Returns true if the entire write succeeds, false otherwise.
static bool Write(FILE* pFile, void* ptr, size_t Amount)
{
	return fwrite(ptr, 1, Amount, pFile) == Amount;
}

static bool WriteBMPLine(FILE* pFile, CSBitmap *DestBitmap, int y)
{
	// A buffer to use to write the padding bytes from.
	char	PaddingBuffer[3] = {0};
	// We have to flip the y-coordinate because our bitmap class
	// puts line zero at the top of the screen, and the BMP format
	// on disk does the opposite.
	uint8_t *LinePtr = DestBitmap->GetLinePtr(DestBitmap->GetHeight() - 1 - y);
	unsigned int BytesPerLine = ((DestBitmap->GetWidth() * DestBitmap->GetChannels() + 3) & ~3);
	unsigned int UsedBytes = DestBitmap->GetWidth() * DestBitmap->GetChannels();
	unsigned int Padding = BytesPerLine - UsedBytes;
	// Read the precise number of bytes that the line requires into the bitmap.
	// Don't read the padding bytes, because the in memory alignment requirements
	// may vary - we don't want our reading code to depend on our bitmap class
	// implementation.
	if (!Write(pFile, LinePtr, UsedBytes))
		return false;
	// Write out any padding bytes.
	if (!Write(pFile, PaddingBuffer, Padding))
		return false;
	return true;
}

static bool InternalSaveBMPFile(FILE* pFile, CSBitmap *SourceBitmap)
{
	BITMAPFILEHEADER fileheader = {0};
	BITMAPINFOHEADER infoheader = {0};

	memcpy(&fileheader.bfType, "BM", 2);
	fileheader.bfOffBits = sizeof(fileheader) + sizeof(infoheader);
	if (SourceBitmap->GetChannels() == 1)
		fileheader.bfOffBits += 256 * sizeof(RGBQUAD);

	infoheader.biSize = sizeof(infoheader);
	infoheader.biPlanes = 1;
	infoheader.biBitCount = SourceBitmap->GetChannels() * 8;
	infoheader.biWidth = SourceBitmap->GetWidth();
	infoheader.biHeight = SourceBitmap->GetHeight();
	infoheader.biCompression = BI_RGB;	// I'm not compressing it!
	// I don't need to set biSizeImage or biClrUsed.

	// Read the file header.
	if (!Write(pFile, &fileheader, sizeof(fileheader)))
		return false;

	// Read the info header.
	if (!Write(pFile, &infoheader, sizeof(infoheader)))
		return false;

	// Read the color map, if any.
	if (infoheader.biBitCount == 8)
	{
		RGBQUAD		Palette[256];
		if (!SourceBitmap->GetPalette(0, 256, Palette))
			return false;
		if (!Write(pFile, Palette, sizeof(Palette)))
			return false;
	}

	for (int y = 0; y < infoheader.biHeight; y++)
		if (!WriteBMPLine(pFile, SourceBitmap, y))
			return false;

	return true;
}

bool SaveBMPFile(const char* FileName, CSBitmap* SourceBitmap)
{
	bool	Result = false;
	assert(FileName);
	assert(SourceBitmap);
	FILE* pFile = fopen(FileName, "wb");

	// This is the cleanest way of making sure that the file always gets
	// closed.
	if (pFile)
	{
		Result = InternalSaveBMPFile(pFile, SourceBitmap);
		fclose(pFile);
	}
	return Result;
}
