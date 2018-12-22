
#ifndef OPENGLFONT_H
#define OPENGLFONT_H
#include <tchar.h>


// Font creation flags
#define OPENGLFONT_BOLD        0x0001
#define OPENGLFONT_ITALIC      0x0002

//-----------------------------------------------------------------------------
// Name: class OpenGLFont_t
// Desc: Texture-based font class for doing text in a 3D scene.
//-----------------------------------------------------------------------------
class OpenGLFont_t
{
    TCHAR   m_strFontName[80];            // Font properties
    DWORD   m_dwFontHeight;
    DWORD   m_dwFontFlags;

    DWORD   m_dwTexWidth;                 // Texture dimensions
    DWORD   m_dwTexHeight;
    FLOAT   m_fTextScale;
    FLOAT   m_fTexCoords[128-32][4];

	unsigned int m_nFontID;
public:
    // 2D text drawing function
    HRESULT DrawText( FLOAT x, FLOAT y, DWORD dwColor, const TCHAR* strText, DWORD dwFlags=0L );
	// 2D box drawing function
	HRESULT DrawBox( FLOAT fXPos, FLOAT fYPos, FLOAT fXSize, FLOAT fYSize, DWORD dwColor );

    // Function to get extent of text
    HRESULT GetTextExtent( TCHAR* strText, SIZE* pSize );

    // Initializing and destroying device-dependent objects
    HRESULT InitDeviceObjects();
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
    HRESULT DeleteDeviceObjects();

    // Constructor / destructor
    OpenGLFont_t( TCHAR* strFontName, DWORD dwHeight, DWORD dwFlags=0L );
    ~OpenGLFont_t();
};


#endif


