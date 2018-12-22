#include "sgx/core/core.hpp"
#include "sgx/filesystem/filesystem.hpp"
#include "sgx/utils/imagelib/imagelib.hpp"

CImageData::CImageData()
{
	iWidth = 0;
	iHeight = 0;
	iSpan = 0;
	iFormat = eRGBA;
	pBitmap = NULL;
	bOurBitmap = FALSE;
}

CImageData::CImageData(const CImageData &other)
{
	Create(other.iWidth, other.iHeight, other.iFormat);
	Blit(other);
}

CImageData::CImageData(const CImageData &opaque, const CImageData &alpha)
{
	sgxAssert(opaque.iWidth  == alpha.iWidth);
	sgxAssert(opaque.iHeight == alpha.iHeight);

	Create(opaque.iWidth, opaque.iHeight, eRGBA);

	Blit(opaque);
	ApplyAlpha(alpha);
}

CImageData::CImageData(const tMEMSIZE w, const tMEMSIZE h, const tFormat fmt)
{
	Create(w, h, fmt);
}

CImageData::CImageData(const tMEMSIZE w, const tMEMSIZE h, const tFormat fmt, const tMEMSIZE span, tBYTE *bmp)
{
	// This of this like an inplace create. This is why you have
	// to provide the span manually.
	iWidth = w;
	iHeight = h;
	iSpan = span;
	iFormat = fmt;
	pBitmap = bmp;
	bOurBitmap = FALSE;
}

CImageData::~CImageData()
{
	Release();
}

void
CImageData::Create(const tMEMSIZE w, const tMEMSIZE h, const tFormat fmt)
{
	iWidth = w;
	iHeight = h;
	iFormat = fmt;
	iSpan = w * bpp();
	pBitmap = new tBYTE[iSpan * iHeight];
	bOurBitmap = TRUE;
}

void
CImageData::Release()
{
	if (bOurBitmap) {
		delete []pBitmap;
		pBitmap = NULL;
	}
}

void
CImageData::Blit(const CImageData &src_image)
{
	sgxAssert(src_image.Validate());

	tBYTE *pSource = (tBYTE *)src_image.pBitmap;
	tBYTE *pDest = (tBYTE *)pBitmap;

	for(tMEMSIZE y=0;y<iHeight;++y) {
		for(tMEMSIZE x=0;x<iWidth;++x) {
			pDest[0] = pSource[0];
			pDest[1] = pSource[1];
			pDest[2] = pSource[2];
			pDest += bpp();
			pSource += src_image.bpp();
		}
		pDest -= iWidth*bpp();
		pDest += iSpan;

		pSource -= src_image.iWidth*src_image.bpp();
		pSource += src_image.iSpan;
	}
}

tBOOL CImageData::IsTransparent()
{
	// i.e. are there any transparent pixels present
	if(iFormat == eRGB) {
		return FALSE;
	}

	tBYTE *p = (tBYTE *)pBitmap;
	for(tMEMSIZE y=0;y<iHeight;++y) {
		for(tMEMSIZE x=0;x<iWidth;++x) {
			if (p[3]) {
				return TRUE;
			}
			p += bpp();
		}
		p -= iWidth*bpp();
		p += iSpan;
	}

	return FALSE;
}

tBOOL CImageData::IsTranslucent()
{
	// i.e. are there any partially transparent pixels present
	if(iFormat == eRGB) {
		return FALSE;
	}

	tBYTE *p = (tBYTE *)pBitmap;
	for(tMEMSIZE y=0;y<iHeight;++y) {
		for(tMEMSIZE x=0;x<iWidth;++x) {
			if (p[3] != 0 && p[3] != 255) {
				return TRUE;
			}
			p += bpp();
		}
		p -= iWidth*bpp();
		p += iSpan;
	}

	return FALSE;
}

void CImageData::WashAlpha(tBYTE alpha)
{
	sgxAssert(iFormat == eRGBA);

	tBYTE *p = (tBYTE *)pBitmap;
	for(tMEMSIZE y=0;y<iHeight;++y) {
		for(tMEMSIZE x=0;x<iWidth;++x) {
			p[3] = alpha;
			p += bpp();
		}
		p -= iWidth*bpp();
		p += iSpan;
	}
}


void CImageData::WashColor(const tBYTE red, const tBYTE green, const tBYTE blue)
{
	tBYTE *p = (tBYTE *)pBitmap;

	for(tMEMSIZE y=0;y<iHeight;++y) {
		for(tMEMSIZE x=0;x<iWidth;++x) {
			p[0] = red;
			p[1] = green;
			p[2] = blue;
			p += bpp();
		}
		p -= iWidth*bpp();
		p += iSpan;
	}
}

void CImageData::ApplyMask(const CImageData::Mask &mask)
{
	sgxAssert(iFormat == eRGBA);

	tBYTE *p = (tBYTE *)pBitmap;
	for(tMEMSIZE y=0;y<iHeight;++y) {
		for(tMEMSIZE x=0;x<iWidth;++x) {
			p[3] = mask.GetAlpha(p[0], p[1], p[2]);
			p += bpp();
		}
		p -= iWidth*bpp();
		p += iSpan;
	}
}


void	
CImageData::ConvertFormat(tFormat new_fmt)
{
	sgxAssert(iFormat != new_fmt);
	sgxAssert(bOurBitmap);		// can't change format if it's somebody elses data

	CImageData replace(iWidth, iHeight, new_fmt);

	tBYTE *pOld = (tBYTE *)pBitmap;
	tBYTE *pNew = (tBYTE *)replace.pBitmap;
	for(tMEMSIZE y=0;y<iHeight;++y) {
		for(tMEMSIZE x=0;x<iWidth;++x) {
			pNew[0] = pOld[0];
			pNew[1] = pOld[1];
			pNew[2] = pOld[2];

			pOld += bpp();
			pNew += replace.bpp();
		}
		pOld -= iWidth*bpp();
		pNew -= iWidth*replace.bpp();
		pOld += iSpan;
		pNew += replace.iSpan;
	}

	// Now just destroy our existing data
	Release();

	// And put the new data in its place
	iWidth  = replace.iWidth;
	iHeight = replace.iHeight;
	iFormat = new_fmt;
	pBitmap = replace.pBitmap;
	iSpan   = replace.iSpan;

	// Fool our replacement into _not_ deleting the pixel data on dtor
	replace.bOurBitmap = FALSE;

}

void CImageData::BuildMask(const CImageData::Mask &mask)
{
	sgxAssert(iFormat == eRGB);

	tBYTE *p = (tBYTE *)pBitmap;
	for(tMEMSIZE y=0;y<iHeight;++y) {
		for(tMEMSIZE x=0;x<iWidth;++x) {
			p[0] = p[1] = p[2] = mask.GetAlpha(p[0], p[1], p[2]);
			p += bpp();
		}
		p -= iWidth*bpp();
		p += iSpan;
	}
}

void CImageData::FlipVertical()
{
	tBYTE *pTop = (tBYTE *)pBitmap;
	tBYTE *pBottom = (tBYTE *)pBitmap;

	pBottom += iHeight * iSpan;

	for(tMEMSIZE y=0;y<iHeight/2;++y) {
		pBottom -= iSpan;
		sgxMemSwap(pTop, pBottom, iSpan);
		pTop += iSpan;
	}
}

void CImageData::ApplyAlpha(const CImageData &alpha)
{
	sgxAssert(iFormat == eRGBA);
	sgxAssert(iWidth  == alpha.iWidth);
	sgxAssert(iHeight == alpha.iHeight);

	tBYTE *pOpaque = (tBYTE *)pBitmap;
	tBYTE *pAlpha = (tBYTE *)alpha.pBitmap;

	for(tMEMSIZE y=0;y<iHeight;++y) {
		for(tMEMSIZE x=0;x<iWidth;++x) {
			// The alpha image is a gray scale so R=G and G=B. We
			// can therefore check any individual component
			pOpaque[3] = pAlpha[0];
			pOpaque += bpp();
			pAlpha += alpha.bpp();
		}
		pOpaque -= iWidth*bpp();
		pOpaque += iSpan;

		pAlpha -= alpha.iWidth*alpha.bpp();
		pAlpha += alpha.iSpan;
	}
}

tBOOL
CImageData::GetPixelAt(sgxColorRGBA &col, const tMEMSIZE x, const tMEMSIZE y)
{
	if (x >= iWidth || y >= iHeight) {
		return FALSE;
	}

	tBYTE *ptr = pBitmap;
	ptr += x * bpp();
	ptr += y * Span();

	tBYTE alpha = 255;
	if (iFormat == eRGBA) {
		alpha = *(ptr+3); 
	}

	col = sgxColorRGBA( *(ptr+0)/255.0f, *(ptr+1)/255.0f, *(ptr+2)/255.0f, alpha/255.0f);

	return TRUE;
}

tBOOL
CImageData::SetPixelAt(const sgxColorRGBA &col, const tMEMSIZE x, const tMEMSIZE y)
{
	if (x >= iWidth || y >= iHeight) {
		return FALSE;
	}
	if (x < 0 || y < 0) {
		return FALSE;
	}

	tBYTE *ptr = pBitmap;
	ptr += x * bpp();
	ptr += y * Span();

	*(ptr+0) = (tBYTE)(col.r*255.0f);
	*(ptr+1) = (tBYTE)(col.g*255.0f);
	*(ptr+2) = (tBYTE)(col.b*255.0f);

	if (iFormat == eRGBA) {
		*(ptr+3) = (tBYTE)(col.a*255.0f);
	}

	return TRUE;
}

//
// For the image mask, used when creating alpha channels
//
CImageData::Mask::Mask()
{
	m_Valid = FALSE;
	m_RangeValid = FALSE;
}

CImageData::Mask::Mask(const tREAL32 r, const tREAL32 g, const tREAL32 b)
{
	m_MaskRed   = (tBYTE)(sgxRange<tREAL32>(r,0,1.0f) * 255.0f);
	m_MaskGreen = (tBYTE)(sgxRange<tREAL32>(g,0,1.0f) * 255.0f);
	m_MaskBlue  = (tBYTE)(sgxRange<tREAL32>(b,0,1.0f) * 255.0f);

	m_Valid = TRUE;
	m_RangeValid = FALSE;
}

static SGX_INLINE tBYTE mask_min(const tBYTE col, const tBYTE delta)
{
	// Carefully crafted function to avoid wrapping around
	if (col < delta) {
		return 0;
	}

	return col - delta;
}

static SGX_INLINE tBYTE mask_max(const tBYTE col, const tBYTE delta)
{
	// Carefully crafted function to avoid wrapping around
	if (col > 255-delta) {
		return 255;
	}

	return col + delta;
}

CImageData::Mask::Mask(const tBYTE r, const tBYTE g, const tBYTE b, const tBYTE delta)
{
	m_MaskRed   = r;
	m_MaskGreen = g;
	m_MaskBlue  = b;

	m_Valid = TRUE;

	m_MaskMinRed = mask_min(r, delta);
	m_MaskMinGreen = mask_min(g, delta);
	m_MaskMinBlue  = mask_min(b, delta);
	m_MaskMaxRed = mask_max(r, delta);
	m_MaskMaxGreen = mask_max(g, delta);
	m_MaskMaxBlue = mask_max(b, delta);

	m_RangeValid = TRUE;
}

CImageData::Mask::Mask(const tBYTE r, const tBYTE g, const tBYTE b, const tBYTE delta_r, const tBYTE delta_g, const tBYTE delta_b)
{
	m_MaskRed   = r;
	m_MaskGreen = g;
	m_MaskBlue  = b;

	m_Valid = TRUE;

	m_MaskMinRed = mask_min(r, delta_r);
	m_MaskMinGreen = mask_min(g, delta_g);
	m_MaskMinBlue = mask_min(b, delta_b);
	m_MaskMaxRed = mask_max(r, delta_r);
	m_MaskMaxGreen = mask_max(g, delta_g);
	m_MaskMaxBlue = mask_max(b, delta_b);

	m_RangeValid = TRUE;
}

CImageData::Mask::Mask(const sgxColorRGB &col)
{
	m_MaskRed   = (tBYTE)(sgxRange<tREAL32>(col.r,0,1.0f) * 255.0f);
	m_MaskGreen = (tBYTE)(sgxRange<tREAL32>(col.g,0,1.0f) * 255.0f);
	m_MaskBlue  = (tBYTE)(sgxRange<tREAL32>(col.b,0,1.0f) * 255.0f);

	m_Valid = TRUE;
	m_RangeValid = FALSE;
}

tBYTE
CImageData::Mask::GetAlpha(const tBYTE &r, const tBYTE &g, const tBYTE &b) const
{
	if (!m_Valid) {
		return 255;
	}

	// Easy out case: We're spot on!
	if (r == m_MaskRed && g == m_MaskGreen && g == m_MaskGreen) {
		return 0;
	}

	// If we're not spot on, and there's no room for error, we've failed,
	// and this pixel is opaque
	if (!m_RangeValid) {
		return 255;
	}

	// Check the range
	if (r < m_MaskMinRed || r > m_MaskMaxRed) {
		return 255;
	}
	if (g < m_MaskMinGreen || g > m_MaskMaxGreen) {
		return 255;
	}
	if (b < m_MaskMinBlue || b > m_MaskMaxBlue) {
		return 255;
	}

	return 0;
}

