#ifndef __DXCONTEXT__
#define __DXCONTEXT__

#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9mesh.h>
#include <dsound.h>               
#include <map>

class OggPlayer;
class CSoundManager;
class CSound;

class DXBase 
{
    public:
		static DXBase *GetInstance() {return m_pDXBase;}
		void GetScreenDimensions(int &w, int &h) {w = width; h = height;}
        LPDIRECT3D9 GetD3D()			{return lpD3D;}
        LPDIRECT3DDEVICE9 Get3DDevice() {return lpDevice;}
		CSoundManager *GetSoundManager() {return m_pSoundManager;}

        void SetRenderFunc(void (*ptr)(float));
        DXBase(HINSTANCE hInstance,LPCSTR pClass,LPCSTR pCaption,WNDPROC pProc);
        ~DXBase();
        BOOL Init(WORD wWidth,WORD wHeight,WORD wDepth,WORD wBackBuffers,BOOL bWindowed);
        int Run(void (*ptr)(float));
        void Cleanup();

		bool PlayMusic(int id, const char *fname);
		void StopMusic(int id);

		bool PlaySound(int id, const char *fname);
		void StopSound(int id);

        HRESULT lastErr;
        BOOL active;

	private:
        BOOL TestDepth(D3DFORMAT fmt);
        BOOL TestFormat(D3DFORMAT fmt);
        void (*renderPtr)(float);

        HRESULT hRes;
        D3DFORMAT format;
        D3DFORMAT zFormat;
        WORD height;
        WORD width;

        HWND hwnd;

		static DXBase *m_pDXBase;
        LPDIRECT3D9 lpD3D;
        LPDIRECT3DDEVICE9 lpDevice;
		CSoundManager *m_pSoundManager;
		std::map<int,OggPlayer *> m_MusicMap;
		std::map<int,CSound *> m_SoundMap;
};

#define DXC_ERR_REGWIN -2
#define DXC_ERR_CREATEWIN -3
#define DXC_ERR_CREATE3D -4
#define DXC_ERR_GETFORMAT -5
#define DXC_ERR_FORMAT -6
#define DXC_ERR_CREATEDEV -7

#include <dxsprite.h>
#include <dxfont.h>

#endif