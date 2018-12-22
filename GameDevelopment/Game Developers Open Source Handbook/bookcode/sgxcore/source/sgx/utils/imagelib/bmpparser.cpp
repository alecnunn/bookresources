#include "sgx/core/core.hpp"
#include "sgx/filesystem/filesystem.hpp"
#include "sgx/utils/imagelib/imagelib.hpp"

//
// Generic parser code
//
CBMPParser *CBMPParser::m_pSingleton = NULL;

void
CBMPParser::Initialize()
{
	if (!m_pSingleton) {
		m_pSingleton = new CBMPParser;
		if (m_pSingleton) {
			CImageLoader::Get()->Register(m_pSingleton);
		}
	}
}

//
// BMP-specific structures
//
typedef struct {
	tINT16	type;
	tUINT32	size;
	tINT16	res0, res1;
	tUINT32 offset;
} sgxBMPHeader;

typedef struct {
	tUINT32	size;
	tUINT32	width, height;
	tUINT16	planes;
	tUINT16	bpp;
	tUINT32	compression;
	tUINT32	image_size;
	tUINT32	xppm, yppm;		// pixels per meter
	tUINT32	colors;
	tUINT32	important_colors;
} sgxBMPInfoHeader;

//
// BMP-specific source code
//
tBOOL CBMPParser::IsRecognised(CSGXFile &file) const
{
	char id[2];

	file.Read((tBYTE *)&id, sizeof(id));
	if (id[0] == 'B' && id[1] == 'M') {
		return TRUE;
	}
	
	return FALSE;
}

tBOOL CBMPParser::Open(CSGXFile &file, CImageData &properties) const
{
	sgxBMPHeader bm_fileheader;
	sgxBMPInfoHeader bm_infoheader;
	tBYTE hdr[14];

	properties.Release();

	// Special code to load the header, which is 14 bytes
	file.Read((tBYTE *)&hdr, sizeof(hdr));
	// Note: This is not endian safe
	bm_fileheader.type = sgxMakeInt16(&hdr[0]);
	bm_fileheader.size = sgxMakeInt32(&hdr[2]);
	bm_fileheader.res0 = sgxMakeInt16(&hdr[6]);
	bm_fileheader.res1 = sgxMakeInt16(&hdr[8]);
	bm_fileheader.offset = sgxMakeInt16(&hdr[10]);

	// Bring in info header
	file.Read((tBYTE *)&bm_infoheader, sizeof(bm_infoheader));

	// Note: Normally this information would come from our tool
	// chain and be pre-swapped. This code assumes a pre-swapped
	// bitmap format.
	file.Seek(bm_fileheader.offset, CSGXDeviceBlockState::eBegin);


	if (bm_infoheader.bpp == 32) {
		properties.Create(bm_infoheader.width, bm_infoheader.height, CImageData::eRGBA);
	} else if (bm_infoheader.bpp == 24) {
		properties.Create(bm_infoheader.width, bm_infoheader.height, CImageData::eRGB);
	} else {
		return FALSE;
	}

	tBYTE *ptr = properties.Bitmap();

	ptr += properties.Span() * properties.Height();

	for(tUINT32 y=0;y<bm_infoheader.height;++y) {
		
		ptr -= properties.Span();
	
		file.Read(ptr, properties.bpl());
		for(tUINT32 x=0;x<bm_infoheader.width;++x) {
			
			// Swap RGB
			tBYTE t = *(ptr+0);
			*(ptr+0) = *(ptr+2);
			*(ptr+2) = t;
			
			// This let's us read RGB and RGBA surfaces. We could also have
			// used .bpp()
			ptr += bm_infoheader.bpp/8;
		}

		ptr -= properties.bpl();
	}

	return TRUE;
}

tBOOL CBMPParser::Close(CImageData &properties) const
{
	return TRUE;
}
