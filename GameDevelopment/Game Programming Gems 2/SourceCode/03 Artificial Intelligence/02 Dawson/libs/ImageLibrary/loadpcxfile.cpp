#include "priv_precompiled.h"

/*
Copyright 2001 Bruce Dawson, Cygnus Software
For more information, visit http://www.cygnus-software.com/papers/
or e-mail: comments@cygnus-software.com

This code may be redistributed as long as this notice is kept intact.
*/

#include <fstream>
#include <vector>
using namespace std;
#include "csbitmap.h"

#pragma pack(push, 1)

struct PCXHeader
{
	char	Manufacturer;	// Always 10.
	char	Version;		// 5 - 0 = Ver2.5, 2 = Ver2.8 w/palette, 3 = Ver2.8 w/o palette, 5 = Ver 3.0
	char	Encoding;		// 1 - PCX RLE
	char	BitsPerPixel;	// per plane
	short	xMin, yMin, xMax, yMax;
	short	HDPI, VDPI;
	char	Colormap[48];	// Huh?
	char	Reserved1;
	char	NPlanes;
	short	BytesPerLine;
};

#pragma pack(pop)

bool InternalLoadPCXFile(istream& inputfile, CSBitmap* DestBitmap)
{
	char	buffer[128];
	if (!inputfile.read(buffer, sizeof(buffer)))
		return false;
	assert(sizeof(PCXHeader) == 68);
	struct PCXHeader* pHeader = (PCXHeader*)buffer;
	int Width = pHeader->xMax + 1 - pHeader->xMin;
	int Height = pHeader->yMax + 1 - pHeader->yMin;
	if (Width <= 0 || Height <= 0)
		return false;
	if (pHeader->BitsPerPixel != 8 || pHeader->NPlanes != 1)
		return false;
	if (pHeader->BytesPerLine < Width)
		return false;
	DestBitmap->SetSize(Width, Height, 1);

	unsigned char* decodebuffer = new unsigned char[pHeader->BytesPerLine];
	if (!decodebuffer)
		return false;
/*
The encoding method is:
    FOR  each  byte,  X,  read from the file
        IF the top two bits of X are  1's then
            count = 6 lowest bits of X
            data = next byte following X
        ELSE
            count = 1
            data = X
Since the overhead this technique requires is, on average,  25% 
of the non-repeating data and is at least offset whenever bytes are 
repeated, the file storage savings are usually considerable.
*/
	for (int y = 0; y < Height && inputfile; y++)
	{
		int x = 0;
		while (x < pHeader->BytesPerLine)
		{
			char	ch;
			inputfile.read(&ch, sizeof(ch));
			if ((ch & 0xC0) == 0xC0)
			{
				int count = ch & 0x3F;
				inputfile.read(&ch, sizeof(ch));
				if (count + x > Width)
					return false;
				for (int i = 0; i < count; i++)
					decodebuffer[x+i] = ch;
				x += count;
			}
			else
				decodebuffer[x++] = ch;
		}
		unsigned char* pLine = DestBitmap->GetLinePtr(y);
		memmove(pLine, decodebuffer, Width);
	}
	delete [] decodebuffer;
	size_t pos = inputfile.tellg();
	inputfile.seekg(-769, ios_base::end);
	size_t pos2 = inputfile.tellg();
	char	ch;
	inputfile.read(&ch, sizeof(ch));
	if (ch != 12)
		return false;
	char	Palette[768];
	inputfile.read(Palette, sizeof(Palette));
	if (!inputfile)
		return false;
	RGBQUAD	Colors[256];
	for (int i = 0; i < 256; i++)
	{
		Colors[i].rgbRed = Palette[i * 3 + 0];
		Colors[i].rgbGreen = Palette[i * 3 + 1];
		Colors[i].rgbBlue = Palette[i * 3 + 2];
	}
	DestBitmap->SetPalette(0, 256, Colors);
	return true;
}

bool LoadPCXFile(const char* FileName, CSBitmap* DestBitmap)
{
	bool	Result = false;
	assert(FileName);
	assert(DestBitmap);
	ifstream	inputfile(FileName, ios_base::binary);
	
	if (inputfile)
		Result = InternalLoadPCXFile(inputfile, DestBitmap);
	if (!Result)
		DestBitmap->FreeBitmap();
	return Result;
}
