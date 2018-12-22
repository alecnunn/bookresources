#include "priv_precompiled.h"

/*
Copyright 2001 Bruce Dawson, Cygnus Software
For more information, visit http://www.cygnus-software.com/papers/
or e-mail: comments@cygnus-software.com

This code may be redistributed as long as this notice is kept intact.
*/

#include "csbitmap.h"
#include "bmpdefs.h"
#include <fstream>
#include <vector>
using namespace std;

// Reads in the colour map, returns the number of bytes of colour map read, or -1 for error.
static short ReadColorMap(istream& inputfile, CSBitmap *DestBitmap, BITMAPINFOHEADER *infoheader)
{
	RGBQUAD ColorMap[256];

	/* Calculate the number of colours there should be in the colour map. */
	/* This will be in ClrUsed.  If ClrUsed is zero then the number of */
	/* colours is just 1 ^ bitsperpixel. */

	unsigned int ColorMapSize = infoheader->biClrUsed;
	if (infoheader->biClrUsed == 0)
		ColorMapSize = 1 << infoheader->biBitCount;
	if (ColorMapSize > 256)
		return -1;

	if (ColorMapSize)
	{
		if (!inputfile.read((char*)ColorMap, sizeof(ColorMap[0]) * ColorMapSize))
			return -1;
		// Set the palette in the bitmap, or something like that.
		DestBitmap->SetPalette(0, ColorMapSize, ColorMap);
	}
	return ColorMapSize * sizeof(ColorMap[0]);
}

static int DecompressToBitmap(CSBitmap *DestBitmap, const char *CompressedData, int DataLength)
{
	const char *DataEnd = CompressedData + DataLength;
	const char *NearEnd = DataEnd - 2;	// Handy.
	int y;
	for (y = 0; y < DestBitmap->GetHeight(); y++)
	{
		int x = 0;
		uint8_t *Output = DestBitmap->GetLinePtr(DestBitmap->GetHeight() - y - 1);
		while (x < DestBitmap->GetWidth())
		{
			uint8_t Input;
			// Make sure I have at least two bytes left.
			if (CompressedData > NearEnd)
			{
				assert(0);
				return 0;	// Not enough data left.
			}
			Input = *CompressedData++;
			if (Input == 0)
			{
				Input = *CompressedData++;
				switch (Input)
				{
					case 0:
						// End of line. I watch for this at the end of lines
						// and I have no interest in handling it in the middle
						// of lines.
						assert(0);
						return 0;
					case 1:
						// End of image. I watch for this at the end of the image
						// and I have no interest in handling it in the middle
						// of the image.
						assert(!"I don't handle this.");
						return 0;
					case 2:
						// Skip to new location! This is ill defined in on disk
						// bitmaps and I have not tested it. Therefore I will not
						// try to handle it.
						assert(!"I don't handle this!!!");
						return 0;
					default:
						if (x + Input > DestBitmap->GetWidth())
						{
							assert(0);
							return 0;
						}
						if (CompressedData + Input >= DataEnd)
						{
							assert(0);
							return 0;
						}
						memcpy(Output + x, CompressedData, Input);
						x += Input;
						CompressedData += Input;
						if ((Input & 1) != 0)
							CompressedData++;
						break;
				}
			}
			else
			{
				if (x + Input > DestBitmap->GetWidth())
				{
					assert(0);
					return 0;
				}
				memset(Output + x, *CompressedData++, Input);
				x += Input;
			}
		}
		if (CompressedData <= NearEnd && CompressedData[0] == 0 && CompressedData[1] == 0)
			CompressedData += 2;	// Skip over the end of line data.
		else
		{
			// Out of data or missing end of line stuff.
			// After complaining we will return a failure code - unless we just finished
			// the last line, in which case we'll cut them some slack.
			// PaintShopPro seems to save bitmaps like this.
			if (y != DestBitmap->GetHeight() - 1)
			{
				assert(0);
				return 0;
			}
		}
	}
	if (CompressedData <= NearEnd && CompressedData[0] == 0 && CompressedData[1] == 1)
		CompressedData += 2;
	else
	{
		// Out of data or missing end of bitmap stuff.
		assert(0);
		return 0;
	}
	assert(CompressedData == DataEnd);
	return DestBitmap->GetHeight();
}

static bool ReadBMPLine(istream& inputfile, CSBitmap *DestBitmap, int y)
{
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
	if (!inputfile.read((char*)LinePtr, UsedBytes))
		return false;
	// Skip over any padding bytes.
	if (!inputfile.seekg(Padding, ios_base::cur))
		return false;
	return true;
}

static bool InternalLoadBMPFile(istream& inputfile, CSBitmap *DestBitmap)
{
	BITMAPFILEHEADER fileheader;
	BITMAPINFOHEADER infoheader;

	// Read the file header.
	if (!inputfile.read((char*)&fileheader, sizeof(fileheader)))
		return false;

	// Check the type field to make sure we have a BMP file.
	if (fileheader.bfType != ('M' * 256 + 'B'))
		return false;

	// Read the info header.
	if (!inputfile.read((char*)&infoheader, sizeof(infoheader)))
		return false;

	// Sanity check the info header.
	if (infoheader.biSize != sizeof(infoheader))
		return false;

	if (infoheader.biPlanes != 1)
		return false;

	// These are the only depths I'm interested in handling. 1 and 4 bits per pixel
	// are also legal, but too much hassle.
	if (infoheader.biBitCount != 8 && infoheader.biBitCount != 24 && infoheader.biBitCount != 32)
		return false;

	if (!DestBitmap->SetSize(infoheader.biWidth, infoheader.biHeight, infoheader.biBitCount / 8))
		return false;

	// Read the color map, if any.
	if (infoheader.biBitCount == 8 && ReadColorMap(inputfile, DestBitmap, &infoheader) <= 0)
	{
		// Illegal ColorMap value
		return false;
	}

	// Jump to the location where the bitmap data is stored.
	if (!inputfile.seekg(fileheader.bfOffBits, ios_base::beg))
		return false;

	// Read in the bitmap data.
	if (infoheader.biCompression == BI_RLE8)
	{
		vector<char> CompressedData;
		int NumLines;
		// We only support 8-bit RLE8 files - logically enough.
		if (infoheader.biBitCount != 8)
			return false;
		// Allocate enough space for all of the compressed data.
		CompressedData.resize(infoheader.biSizeImage);
		// Read all of the compressed data - this is much easier than constantly
		// worrying about fread() failures.
		if (!inputfile.read(&CompressedData[0], infoheader.biSizeImage))
			return false;
		// Decompress.
		NumLines = DecompressToBitmap(DestBitmap, &CompressedData[0], infoheader.biSizeImage);
		if (NumLines != DestBitmap->GetHeight())
			return false;
	}
	else if (infoheader.biCompression == BI_RGB)
	{
		int y;
		for (y = 0; y < infoheader.biHeight; y++)
			if (!ReadBMPLine(inputfile, DestBitmap, y))
				return false;
	}
	else
		return false;

	return true;
}

bool LoadBMPFile(const char* FileName, CSBitmap* DestBitmap)
{
	bool	Result = false;
	assert(FileName);
	assert(DestBitmap);
	ifstream	inputfile(FileName, ios_base::binary);

	if (inputfile)
		Result = InternalLoadBMPFile(inputfile, DestBitmap);
	if (!Result)
		DestBitmap->FreeBitmap();
	return Result;
}
