/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
#define STRICT 1

#include <stdio.h>
#include <math.h>
#include <D3DX8.h>
#include "D3DApp.h"
#include "D3DFile.h"
#include "D3DFont.h"
#include "D3DUtil.h"
#include "DXUtil.h"


#pragma comment(lib, "d3dx8dt.lib") 
#pragma comment(lib, "d3d8.lib") 
#pragma comment(lib, "winmm.lib") 
#pragma comment(lib, "d3dxof.lib") 
#pragma comment(lib, "dxguid.lib") 


#define ID_ERROR_ODDDIMENSIONS          61217
#define ID_ERROR_NOTPOW2                61218
#define ID_ERROR_WRONGDIMENSIONS        61219
#define ID_ERROR_GENERATEALPHAFAILED    61220
#define ID_ERROR_PREMULTALPHA           61221
#define ID_ERROR_PREMULTTODXT1          61222
#define ID_ERROR_CANTCREATEDEVICE       61223
#define ID_ERROR_CANTCREATETEXTURE      61239
#define ID_ERROR_D3DCREATEFAILED        61240

#include <io.h>
#include <dds.h>
#include <string>
                    
#ifndef ReleasePpo
	#define ReleasePpo(ppo) \
		if (*(ppo) != NULL) \
		{ \
			(*(ppo))->Release(); \
			*(ppo) = NULL; \
		} \
		else (VOID)0
#endif


typedef std::string    CSTR;

class CArchive
{
    FILE * fp;
public:
    int Write(void * p, int n)
    {
        return fwrite(p, 1, n, fp);
    }
    int Read(void * p, int n)
    {
        return fread(p, 1, n, fp);
    }
    int Open(const char * name, const char * attr)
    {
        fp = fopen(name, attr);
        if (fp == 0)
            return -1;
        return 0;
    }
    void Close()
    {
        if (fp)
            fclose(fp);
    }
};

enum TextureType
{
    TEXTURE2D,
    CUBE_TEXTURE,
    VOLUME_TEXTURE
};

class CDDS
{
public:
	CDDS();

    LPDIRECT3DDEVICE8 m_pd3ddev;
    void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU);

    BOOL MakeTexture(TextureType iTexType, DWORD dwWidth, DWORD dwHeight, int numMips,
                      D3DFORMAT fmt, int dwDepth);
    
    void AfxMessageBox(int)
    {
    }



    HRESULT SaveTexture(LPDIRECT3DTEXTURE8 ptex, const char * filename);
    HRESULT SaveCubeMap(LPDIRECT3DCUBETEXTURE8 ptex, const char * filename);
    HRESULT SaveVolumeTexture(LPDIRECT3DVOLUMETEXTURE8 pvoltex, const char * filename);
    

	HRESULT LoadAlphaBmp(CSTR& strPath);
	VOID GenerateMipMaps(VOID);
	HRESULT Compress(D3DFORMAT fmt, BOOL bSwitchView);
	DWORD NumMips(VOID);
	LPDIRECT3DBASETEXTURE8 PtexOrig(VOID) { return m_ptexOrig; }
	LPDIRECT3DBASETEXTURE8 PtexNew(VOID) { return m_ptexNew; }
	DWORD DwWidth(VOID) { return m_dwWidth; }
	DWORD DwHeight(VOID) { return m_dwHeight; }
	DWORD DwDepth(VOID) { return m_dwDepth; }
	DWORD DwDepthAt(LONG lwMip);
	//BOOL TitleModsChanged(VOID) { return m_bTitleModsChanged; }
	//VOID ClearTitleModsChanged(VOID) { m_bTitleModsChanged = FALSE; }
	virtual ~CDDS();
	void OpenSubsurface(CSTR fileName, D3DCUBEMAP_FACES FaceType, LONG lwMip, LONG lwSlice);
	void OpenAlphaSubsurface(CSTR fileName, D3DCUBEMAP_FACES FaceType, LONG lwMip, LONG lwSlice);
	void OpenCubeFace(CSTR fileName, D3DCUBEMAP_FACES FaceType);
	void OpenAlphaCubeFace(CSTR fileName, D3DCUBEMAP_FACES FaceType);
	BOOL IsCubeMap(VOID) { return (m_dwCubeMapFlags > 0); }
	BOOL IsVolumeMap(VOID) { return (m_dwDepth > 0); }


	HRESULT LoadBmp(CSTR& strPath);
	HRESULT LoadAlphaIntoSurface(CSTR& strPath, LPDIRECT3DSURFACE8 psurf);
	HRESULT LoadVolumeSliceFromSurface(LPDIRECT3DVOLUME8 pVolume, UINT iSlice, LPDIRECT3DSURFACE8 pSurf);
	HRESULT LoadSurfaceFromVolumeSlice(LPDIRECT3DVOLUME8 pVolume, UINT iSlice, LPDIRECT3DSURFACE8 psurf);
	HRESULT BltAllLevels(D3DCUBEMAP_FACES FaceType, LPDIRECT3DBASETEXTURE8 ptexSrc, 
		LPDIRECT3DBASETEXTURE8 ptexDest);

	HRESULT SaveAllMipSurfaces(LPDIRECT3DBASETEXTURE8 ptex, D3DCUBEMAP_FACES FaceType, CArchive& ar);
	HRESULT SaveAllVolumeSurfaces(LPDIRECT3DVOLUMETEXTURE8 pvoltex, CArchive& ar);
	HRESULT LoadAllMipSurfaces(LPDIRECT3DBASETEXTURE8 ptex, D3DCUBEMAP_FACES FaceType, CArchive& ar);
	HRESULT LoadAllVolumeSurfaces(LPDIRECT3DVOLUMETEXTURE8 pvoltex, CArchive& ar);

	HRESULT SaveDDS(LPDIRECT3DBASETEXTURE8 ptex, const char * filename);
	HRESULT LoadDDS(LPDIRECT3DBASETEXTURE8* pptex, const char * filename);

private:
	LPDIRECT3DBASETEXTURE8 m_ptexOrig;
	LPDIRECT3DBASETEXTURE8 m_ptexNew;
	DWORD m_dwWidth;
	DWORD m_dwHeight;
	DWORD m_dwDepth; // For volume textures
	DWORD m_numMips;
	DWORD m_dwCubeMapFlags;
    bool m_bAllocatedInternally;
	//BOOL m_bTitleModsChanged;


    CSTR m_strPathName;
};
