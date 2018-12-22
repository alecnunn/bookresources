
#ifndef D3D7FONT_H_T
#define D3D7FONT_H_T
#include <tchar.h>
#include <D3D.h>


// Font creation flags
#define D3D7FONT_BOLD        0x0001
#define D3D7FONT_ITALIC      0x0002
#define D3D7FONT_ZENABLE     0x0004

// Font rendering flags
#define D3D7FONT_CENTERED    0x0001
#define D3D7FONT_TWOSIDED    0x0002
#define D3D7FONT_FILTERED    0x0004


class D3D7Font_t
{
public:
    D3D7Font_t( TCHAR* strFontName, DWORD dwHeight, DWORD dwFlags=0L );
    ~D3D7Font_t();

    // 2D text drawing function
    HRESULT DrawText( FLOAT x, FLOAT y, DWORD dwColor, const TCHAR* strText, DWORD dwFlags=0L );
	// 2D box drawing function
	HRESULT DrawBox( FLOAT fXPos, FLOAT fYPos, FLOAT fXSize, FLOAT fYSize, DWORD dwColor );

    // Function to get extent of text
    HRESULT GetTextExtent( TCHAR* strText, SIZE* pSize );

    // Initializing and destroying device-dependent objects
    HRESULT InitDeviceObjects( LPDIRECT3DDEVICE7 pd3dDevice );
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
    HRESULT DeleteDeviceObjects();

private:
    TCHAR   m_strFontName[80];            // Font properties
    DWORD   m_dwFontHeight;
    DWORD   m_dwFontFlags;

    LPDIRECT3DDEVICE7      m_pd3dDevice; // A D3DDevice used for rendering
    LPDIRECTDRAWSURFACE7   m_pddsSurface; // Surface of the texture
    DWORD   m_dwTexWidth;                 // Texture dimensions
    DWORD   m_dwTexHeight;
	HBITMAP m_hbmBitmap;
    DWORD*  m_pBitmapBits;
    FLOAT   m_fTextScale;
    FLOAT   m_fTexCoords[128-32][4];

    // Stateblocks for setting and restoring render states
    DWORD   m_dwSavedStateBlock;
    DWORD   m_dwDrawTextStateBlock;

};




#endif


