
#ifndef D3D8FONT_H_T
#define D3D8FONT_H_T
#include <tchar.h>
#include <D3D8.h>


// Font creation flags
#define D3D8FONT_BOLD        0x0001
#define D3D8FONT_ITALIC      0x0002
#define D3D8FONT_ZENABLE     0x0004

// Font rendering flags
#define D3D8FONT_CENTERED    0x0001
#define D3D8FONT_TWOSIDED    0x0002
#define D3D8FONT_FILTERED    0x0004




//-----------------------------------------------------------------------------
// Name: class CD3DFont
// Desc: Texture-based font class for doing text in a 3D scene.
//-----------------------------------------------------------------------------
class D3D8Font_t
{
public:
    // Constructor / destructor
    D3D8Font_t( TCHAR* strFontName, DWORD dwHeight, DWORD dwFlags=0L );
    ~D3D8Font_t();

    // 2D text drawing function
    HRESULT DrawText( FLOAT x, FLOAT y, DWORD dwColor, const TCHAR* strText, DWORD dwFlags=0L );
	// 2D box drawing function
	HRESULT DrawBox( FLOAT fXPos, FLOAT fYPos, FLOAT fXSize, FLOAT fYSize, DWORD dwColor );

    // Function to get extent of text
    HRESULT GetTextExtent( TCHAR* strText, SIZE* pSize );

    // Initializing and destroying device-dependent objects
    HRESULT InitDeviceObjects( LPDIRECT3DDEVICE8 pd3dDevice );
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
    HRESULT DeleteDeviceObjects();

private:
    TCHAR   m_strFontName[80];            // Font properties
    DWORD   m_dwFontHeight;
    DWORD   m_dwFontFlags;

    LPDIRECT3DDEVICE8       m_pd3dDevice; // A D3DDevice used for rendering
    LPDIRECT3DTEXTURE8      m_pTexture;   // The d3d texture for this font
    LPDIRECT3DVERTEXBUFFER8 m_pVB;        // VertexBuffer for rendering text
    DWORD   m_dwTexWidth;                 // Texture dimensions
    DWORD   m_dwTexHeight;
    FLOAT   m_fTextScale;
    FLOAT   m_fTexCoords[128-32][4];

    // Stateblocks for setting and restoring render states
    DWORD   m_dwSavedStateBlock;
    DWORD   m_dwDrawTextStateBlock;

};




#endif


