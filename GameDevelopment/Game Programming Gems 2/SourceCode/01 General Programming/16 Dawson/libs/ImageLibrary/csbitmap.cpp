#include "priv_precompiled.h"

/*
Copyright 2001 Bruce Dawson, Cygnus Software
For more information, visit http://www.cygnus-software.com/papers/
or e-mail: comments@cygnus-software.com

This code may be redistributed as long as this notice is kept intact.
*/

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>	// If we include windows it has to be before csbitmap.h
//Needed for SelectBitmap, etc.
#include <windowsx.h>

#include "csbitmap.h"

#define	RENAME_WINDOWS_STRUCTS
#include "bmpdefs.h"
CompileTimeAssert(sizeof(BITMAPINFOHEADER) == sizeof(PortableBITMAPINFOHEADER));
CompileTimeAssert(sizeof(BITMAPFILEHEADER) == sizeof(PortableBITMAPFILEHEADER));

static void DisplayError(long LastError, const char *ErrorText /*= 0*/)
{
	char	ErrorBuffer[1000];
	if (ErrorText)
		sprintf(ErrorBuffer, "%s\nError code is %d.", ErrorText, LastError);
	else
		sprintf(ErrorBuffer, "Error code %d encountered.", LastError);
	MessageBox(0, ErrorBuffer, "Message", MB_OK);
}

static void DisplayLastError(const char *ErrorText /*= 0*/)
{
	DisplayError(GetLastError(), ErrorText);
}


CSBitmap::CSBitmap()
{
	mWidth = mHeight = mChannels = -1;
	mSurfaceBits = 0;
	mBitmapNew = 0;
	mBitmapDC = 0;
	mBitmapMonochrome = 0;
}

CSBitmap::~CSBitmap()
{
	FreeBitmap();
}

void CSBitmap::FreeBitmap()
{
	if (mBitmapDC)
	{
		if (mBitmapMonochrome)
		{
			// Select the stock 1x1 monochrome bitmap back in
			HBITMAP hBitmapOld = SelectBitmap(mBitmapDC, mBitmapMonochrome);
			assert(hBitmapOld == mBitmapNew);
		}
		if (mBitmapNew)
			verify(DeleteObject(mBitmapNew));
		if (mBitmapDC)
			verify(DeleteDC(mBitmapDC));
		// Mark the bitmap as unallocated.
		mBitmapDC = 0;
		mWidth = mHeight = mChannels = -1;
	}
}

HDC CSBitmap::GetDrawSurface() const
{
	assert(HasBitmap());
	return mBitmapDC;
}

HBITMAP CSBitmap::GetDrawBitmap() const
{
	assert(HasBitmap());
	return mBitmapNew;
}

// Theoretically we should be paying attention to this:
// From the CreateDIBSection help:
// Windows NT: You need to guarantee that the GDI subsystem has completed any
// drawing to a bitmap created by CreateDIBSection before you draw to the
// bitmap yourself. Access to the bitmap must be synchronized. Do this by
// calling the GdiFlush function. This applies to any use of the pointer
// to the bitmap’s bit values, including passing the pointer in calls to
// functions such as SetDIBits. 
uint8_t *CSBitmap::GetLinePtr(int y) const
{
	assert(HasBitmap());
	assert(y >= 0 && y < mHeight);
	return mLineZeroPointer + y * mStride;
}

uint8_t *CSBitmap::GetPixelPtr(int x, int y) const
{
	assert(HasBitmap());
	assert(y >= 0 && y < mHeight);
	assert(x >= 0 && x < mWidth);
	return GetLinePtr(y) + x * mChannels;
}

void CSBitmap::InitDerivedData()
{
	// Here is where we have to calculate the actual number of bytes per line,
	// respecting the Win32 rules for bitmap allocations. When we requested
	// the bitmap we specified a width, which Windows will have padded to
	// a multiple of four bytes - so we'd better allow for that.
	mBytesPerLine = (mWidth * mChannels + 3) & ~3;
	// Calculate the address of line zero - defined here as the line that is
	// at the top when the bitmap is copied to the screen. This is the last
	// line in memory.
	mLineZeroPointer = mSurfaceBits + (mHeight - 1) * mBytesPerLine;
	// Calculate the distance from line to line to be consistent with
	// mLineZeroPointer.
	mStride = -mBytesPerLine;
}

void CSBitmap::SetPalette(int StartColor, int NumColors, RGBQUAD* pColors)
{
	assert(HasBitmap());
	assert(GetChannels() == 1);
	SetDIBColorTable(GetDrawSurface(), StartColor, NumColors, pColors);
}

bool CSBitmap::GetPalette(int StartColor, int NumColors, RGBQUAD* pColors) const
{
	int GotCount;
	assert(HasBitmap());
	assert(GetChannels() == 1);
	// Zero the destination array to make sure all bytes get cleared.
	memset(pColors, 0, sizeof(RGBQUAD) * NumColors);
	GotCount = GetDIBColorTable(GetDrawSurface(), StartColor, NumColors, pColors);
	return GotCount == NumColors;
}

struct DIBData
{
	BITMAPINFOHEADER InfoHeader;
	RGBQUAD ColorTable[256];
};

bool CSBitmap::SetSize(int width, int height, int channels)
{
	// Make sure that the InitBitmap() function was called before calling
	// SetSize() for the first time.
	assert(HasBitmap() || mWidth == -1);
	assert(HasBitmap() || mHeight == -1);
	assert(HasBitmap() || mChannels == -1);
	// This function doesn't support allocating zero size or negative
	// size bitmaps.
	assert(width > 0);
	assert(height > 0);
	assert(channels == 1 || channels == 3 || channels == 4);
	FreeBitmap();
	mWidth = width;
	mHeight = height;
	mChannels = channels;

	// Set up an optimal bitmap
	DIBData	Info = {0};
	Info.InfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	Info.InfoHeader.biWidth = width;
	Info.InfoHeader.biHeight = height;
	Info.InfoHeader.biPlanes = 1;
	Info.InfoHeader.biBitCount = channels * 8;
	Info.InfoHeader.biCompression = BI_RGB;
	// Support for 256 colour bitmaps.
	Info.InfoHeader.biClrUsed = (channels == 1 ? 256 : 0);

	HDC TempDC = CreateDC("Display", 0, 0, 0);
	if (TempDC)
	{
		mBitmapNew = CreateDIBSection(TempDC, (BITMAPINFO *)&Info, DIB_RGB_COLORS,
						(void **)&mSurfaceBits, 0, 0L);
		if (mBitmapNew)
		{
			mBitmapDC = CreateCompatibleDC(TempDC);
			if (mBitmapDC)
				mBitmapMonochrome = SelectBitmap(mBitmapDC, mBitmapNew);
			else
				DisplayLastError("SelectBitmap failed.");
		}
		else
			DisplayLastError("CreateDIBSection failed.");
		DeleteDC(TempDC);
	}
	else
		DisplayLastError("CreateDC failed.");

	// If the allocation failed, clean up.
	if (!mBitmapMonochrome)
		FreeBitmap();

	InitDerivedData();
	return HasBitmap();
}

void CSBitmap::StretchDraw(HDC DestDC, int destx, int desty, int destwidth, int destheight,
									int sourcex, int sourcey, int sourcewidth, int sourceheight) const
{
	if (HasBitmap())
	{
		StretchBlt(DestDC, destx, desty, destwidth, destheight, GetDrawSurface(),
					sourcex, sourcey, sourcewidth, sourceheight, SRCCOPY);
	}
}

void CSBitmap::Draw(HDC DestDC, int destx, int desty, int destwidth, int destheight, int sourcex, int sourcey) const
{
	if (HasBitmap())
	{
		StretchBlt(DestDC, destx, desty, destwidth, destheight, GetDrawSurface(),
					sourcex, sourcey, destwidth, destheight, SRCCOPY);
	}
}

void CSBitmap::DrawAll(HDC DestDC) const
{
	Draw(DestDC, 0, 0, GetWidth(), GetHeight(), 0, 0);
}

void CSBitmap::Swap(CSBitmap& otherBitmap)
{
	std::swap(mWidth, otherBitmap.mWidth);
	std::swap(mHeight, otherBitmap.mHeight);
	std::swap(mChannels, otherBitmap.mChannels);
	std::swap(mBitmapDC, otherBitmap.mBitmapDC);
	std::swap(mSurfaceBits, otherBitmap.mSurfaceBits);
	std::swap(mBitmapNew, otherBitmap.mBitmapNew);
	std::swap(mBitmapMonochrome, otherBitmap.mBitmapMonochrome);

	InitDerivedData();
	otherBitmap.InitDerivedData();
}
