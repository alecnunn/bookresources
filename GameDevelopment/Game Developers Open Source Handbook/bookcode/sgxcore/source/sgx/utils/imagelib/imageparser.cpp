#include "sgx/core/core.hpp"
#include "sgx/filesystem/filesystem.hpp"
#include "sgx/utils/imagelib/imagelib.hpp"

/*
All image parsers must follow the pattern dictated by this
class.

See the bitmap code for a real-world example.
*/
CImageParser *CImageParser::m_pSingleton = NULL;

// This is the basic creation routine that must always be called in 
// order to register the parser with the load routine.
void
CImageParser::Initialize()
{
	if (!m_pSingleton) {
		m_pSingleton = new CImageParser;
		if (m_pSingleton) {
			CImageLoader::Get()->Register(m_pSingleton);
		}
	}
}

// This should either parser the file contents, or the file name
// and pass TRUE if it is capable of loading this file.
// It is preferred that the file contents are read.
// The file pointer will be corrected by the CImageLoader class.
tBOOL CImageParser::IsRecognised(CSGXFile &file) const
{
	return FALSE;
}

// Load the image data into the members dictated by 'properties'.
// There is a 'pInternal' member for use by the parser, if necessary.
tBOOL CImageParser::Open(CSGXFile &file, CImageData &properties) const
{
	return FALSE;
}

// Free any resources.
// DO NOT attempt to close the file.
tBOOL CImageParser::Close(CImageData &properties) const
{
	return FALSE;
}
