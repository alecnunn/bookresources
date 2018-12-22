#ifndef SGX_UTILS_IMAGEDATA_HPP
#define SGX_UTILS_IMAGEDATA_HPP		1

#include "sgx/domains/graphics/color/color.hpp"

class CImageParser;

// An ImageData block is a cross-platform, easy-to-handle, texture surface.
// It is always in memory and available for use, and it is always in the
// same formats: RGB:888, and RGBA:8888

// It supports a number of basic 2D operations
class CImageData {
public:
	typedef enum {
		eRGB,
		eRGBA,
		// The following are the same memory formats as
		// above, although hold a distinction for gfx
		// engines
		eLuminence,
		eAlpha,
	} tFormat;

	class Mask {
	public:
		Mask();
		Mask(const tREAL32 r, const tREAL32 g, const tREAL32 b);
		Mask(const tBYTE r, const tBYTE g, const tBYTE b, const tBYTE delta);
		Mask(const tBYTE r, const tBYTE g, const tBYTE b, const tBYTE delta_r, const tBYTE delta_g, const tBYTE delta_b);
		Mask(const sgxColorRGB &col);
		
		SGX_INLINE tBOOL Valid() const { return m_Valid; }
		SGX_INLINE tBYTE GetAlpha(const tBYTE &r, const tBYTE &g, const tBYTE &b) const;

	private:
		tBOOL			m_Valid;
		tBOOL			m_RangeValid;

		tBYTE			m_MaskRed, m_MaskGreen, m_MaskBlue;
		tBYTE			m_MaskMinRed, m_MaskMinBlue, m_MaskMinGreen;
		tBYTE			m_MaskMaxRed, m_MaskMaxBlue, m_MaskMaxGreen;

	};

	CImageData();
	CImageData(const CImageData &other);		// copy ctor performs a clone operation
	CImageData(const CImageData &opaque, const CImageData &alpha);
	CImageData(const tMEMSIZE w, const tMEMSIZE h, const tFormat fmt);
	CImageData(const tMEMSIZE w, const tMEMSIZE h, const tFormat fmt, const tMEMSIZE span, tBYTE *pBitmap);
	~CImageData();



	void		Create(const tMEMSIZE w, const tMEMSIZE h, const tFormat fmt);
	void		Release();

	tMEMSIZE	Width()  const { return pBitmap ? iWidth : 0; }
	tMEMSIZE	Height() const { return pBitmap ? iHeight : 0; }
	tFormat		Format() const { return iFormat; }
	tBYTE *		Bitmap() const { return pBitmap; }

	tBOOL		IsValid() const { return pBitmap ? TRUE : FALSE; }

	tBOOL		IsTransparent();
	tBOOL		IsTranslucent();

	tMEMSIZE	Span() const { return iSpan; }	// one row of pixels, in bytes

	tMEMSIZE	bpp() const { return bpp(iFormat); }
	tMEMSIZE	bpl() const { return bpp() * iWidth; }	// data bytes only. You will usually want 'Span'
	
	static tMEMSIZE bpp(tFormat fmt) { return fmt==eRGBA ? 4 : 3; }

	void	Blit(const CImageData &src_image);
	tBOOL	GetPixelAt(sgxColorRGBA &col, const tMEMSIZE x, const tMEMSIZE y);
	tBOOL	SetPixelAt(const sgxColorRGBA &col, const tMEMSIZE x, const tMEMSIZE y);

	void	ConvertFormat(tFormat new_fmt);

	// These functions generate new data in situ
	void	WashAlpha(tBYTE alpha);
	void	WashColor(const sgxColorRGBA &col) { WashColor(col.r, col.g, col.b); }
	void	WashColor(const tREAL32 red, const tREAL32 green, const tREAL32 blue) { WashColor(tBYTE(red*255.0f), tBYTE(green*255.0f), tBYTE(blue*255.0f)); }
	void	WashColor(const tBYTE red, const tBYTE green, const tBYTE blue);
	
	void	BuildMask(const CImageData::Mask &mask);
	void	ApplyMask(const CImageData::Mask &mask);
	void	ApplyAlpha(const CImageData &alpha);

	// These functions affect the data already present.
	void	FlipVertical();

	// Debug
	tBOOL	Validate() const { return TRUE; }

private:
	tMEMSIZE				iWidth;
	tMEMSIZE				iHeight;
	tFormat					iFormat;
	tBYTE					*pBitmap;
	// iSpan is always width*bpp - UNLESS you create the bitmap manually
	tMEMSIZE				iSpan;
	tBOOL					bOurBitmap;	// this is so we don't destroy the users bmp,
										// and it saves us from copying data on assignment

};


#endif /* SGX_UTILS_IMAGEDATA_HPP */
