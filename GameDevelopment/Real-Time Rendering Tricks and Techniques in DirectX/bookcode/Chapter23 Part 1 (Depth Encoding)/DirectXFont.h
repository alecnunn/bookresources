/***************************************************************
* DirectXFont.h                                                *
*                                                              *
* This file contains the header information for the class      *
* CDirectXFont.  For this file to compile correctly, the       *
* path for the DX include files must be added to your path.    *
*                                                              *
***************************************************************/
#include <d3d8.h>
#include <d3dx8.h>

struct FONTVERTEX 
{ 
	float x, y, z, rhw;
	DWORD color;
	float u, v;
};

#define D3DFVF_FONTVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

class CDirectXFont  
{
public:
	void FillCharacter(TCHAR Character, FONTVERTEX **ppVertices,
		               float XStartPosition, float *pXPosition,
					   float *pYPosition, DWORD Color);
	void DestroyStaticText(LPDIRECT3DVERTEXBUFFER8 pBuffer);
	void DrawStaticText(LPDIRECT3DVERTEXBUFFER8 pStaticText, long StartChar,
		                long NumChars);
	HRESULT CreateFont(LPDIRECT3DDEVICE8 pDevice, TCHAR *pFontName, long FontSize);
	void DestroyFont();

	void DrawText(float XPosition, float YPosition, TCHAR *pText, DWORD Color);

	LPDIRECT3DVERTEXBUFFER8 CreateStaticText(float XPosition, float YPosition,
		                                     TCHAR *pText, DWORD Color);

	void FillVertex(FONTVERTEX *pVertex, float x, float y, float u,
		            float v, DWORD color);
	void DrawDebug(float XPosition, float YPosition);
	CDirectXFont();
	virtual ~CDirectXFont();

	//Used for sizing and uv coordinates
	long m_TextureSize;
    float               m_TexCoords[96][4];

    //The basic building blocks
	LPDIRECT3DTEXTURE8      m_pTexture;
	LPDIRECT3DVERTEXBUFFER8 m_pVertexBuffer;
	LPDIRECT3DDEVICE8       m_pDevice;

	//State block handles
	DWORD m_TextStates;
	DWORD m_SavedStates;
};
