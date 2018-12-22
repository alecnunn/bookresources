#ifndef	CSBITMAP_H
#define	CSBITMAP_H

// Copyright © 1999 Bruce Dawson.

#ifdef	WIN32
#ifndef	STRICT
	#error Please #define STRICT, to get more reasonable and consistent behaviour.
#endif
// Windows true-colour bitmaps are in BGR format instead
// of RGB. These defines help to avoid depending on the
// order, and make the intent of code clearer.
#define	BLUE_BITMAP_OFFSET	0
#define	GREEN_BITMAP_OFFSET	1
#define	RED_BITMAP_OFFSET	2
#define	ALPHA_BITMAP_OFFSET	3
#else
#error	Unknown platform. Please specify!
#endif

// These typedefs are here to avoid having to include windows.h.  It compiles
// somewhat faster, and minimizes the dependencies. It does mean that if you
// #include this header and <windows.h>, you must include <windows.h> first to
// avoid redefinitions of the structs.
// If you remove this block then you have to include <windows.h> first anyways,
// just all of the time.

#ifndef	DECLARE_HANDLE
	typedef void *HANDLE;
	#define DECLARE_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name
	DECLARE_HANDLE(HDC);
	DECLARE_HANDLE(HPEN);
	DECLARE_HANDLE(HBRUSH);
	DECLARE_HANDLE(HBITMAP);
	DECLARE_HANDLE(HWND);
	typedef struct tagRGBQUAD {
			uint8_t rgbBlue;
			uint8_t rgbGreen;
			uint8_t rgbRed;
			uint8_t rgbReserved;
	} RGBQUAD;
#endif

class CSBitmap
{
public:
	CSBitmap();
	~CSBitmap();
	// Methods to get and set the size of the bitmap. SetSize() will free
	// previously allocated bitmaps if there are any.
	bool		SetSize(int width, int height, int channels);	// Returns true for success.
	int			GetWidth() const {return mWidth;}
	int			GetHeight() const {return mHeight;}
	int			GetChannels() const {return mChannels;}
	void		FreeBitmap();
	// Returns true if bitmap data is allocated.
	bool		HasBitmap() const {return mBitmapDC != 0;}

	// Asserts if the color table cannot be set, for instance if the bitmap
	// is not a paletted bitmap or if there is no bitmap.
	void		SetPalette(int StartColor, int NumColors, RGBQUAD* pColors);
	// Returns true if getting the palette succeeds. Asserts if used illegally.
	bool		GetPalette(int StartColor, int NumColors, RGBQUAD* pColors) const;


	// Methods to get access to the bitmap data. Some types of
	// Win32 bitmaps allow you to treat them either as an HDC,
	// an HBITMAP or as an array of bytes. While not portable,
	// the HDC and HBITMAP options are too useful to ignore.
	HDC			GetDrawSurface() const;
	HBITMAP		GetDrawBitmap() const;
	uint8_t*	GetLinePtr(int y) const;
	uint8_t*	GetPixelPtr(int x, int y) const;
	int			GetStride() const {return mStride;}

	// Various methods to draw the bitmap to an HDC.
	void		DrawAll(HDC DestDC) const;
	void		Draw(HDC DestDC, int destx, int desty,
							int destwidth, int destheight,
							int sourcex, int sourcey) const;
	void		StretchDraw(HDC DestDC, int destx, int desty, int destwidth, int destheight,
							int sourcex, int sourcey, int sourcewidth, int sourceheight) const;
	void		StretchDrawAll(HDC DestDC, int destx, int desty, int destwidth, int destheight) const
	{
		StretchDraw(DestDC, destx, desty, destwidth, destheight,
					0, 0, GetWidth(), GetHeight());
	}

	// This function swaps two CSBitmap objects. One, both, or neither of
	// them may be initialized. All it does is swap the member variables,
	// thus effectively pointing them at different bitmaps. This is much
	// faster than copying one to another, especially as a way of converting
	// from 8-bit to 24-bit (copy from 8 to 24, then swap back, or vice
	// versa)
	void		Swap(CSBitmap& otherBitmap);

private:
	void		InitDerivedData();
	// Private and unimplemented.
	CSBitmap&	operator=(const CSBitmap& rhs);
				CSBitmap(const CSBitmap& rhs);

	int			mWidth, mHeight, mChannels;

	// Various Win32 specific items.
	HDC			mBitmapDC;
	uint8_t		*mSurfaceBits;
	HBITMAP		mBitmapNew, mBitmapMonochrome;

	// Number of bytes per line, always positive.
	int			mBytesPerLine;
	// These two values are tweaked for easier calculations.
	// They handle the messy upside down bitmaps.
	uint8_t		*mLineZeroPointer;
	int			mStride;
};

// Tries to load the file, using any of the formats it knows about.
bool LoadImageFile(const char* FileName, CSBitmap* DestBitmap);

// Load a file by explicitly specifying the format. Normally it's best
// to use LoadImageFile.
bool LoadBMPFile(const char* FileName, CSBitmap* DestBitmap);
bool LoadTargaFile(const char* FileName, CSBitmap* DestBitmap);
bool LoadPCXFile(const char* FileName, CSBitmap* DestBitmap);

// Tries to save the file in the specified format. These may not
// all be implemented...
bool SaveBMPFile(const char* FileName, const CSBitmap* DestBitmap);
bool SaveTargaFile(const char* FileName, const CSBitmap* DestBitmap);
bool SavePCXFile(const char* FileName, const CSBitmap* DestBitmap);
// etc...

void CompositeBitmap(CSBitmap* Bottom, const CSBitmap* Top, int startX, int startY, uint8_t globalOpacity = 255);

#endif
