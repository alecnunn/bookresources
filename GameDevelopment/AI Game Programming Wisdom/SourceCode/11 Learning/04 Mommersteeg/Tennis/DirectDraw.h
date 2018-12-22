//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    DirectDraw.h
//----------------------------------------------------------------------------------------------

#ifndef __DIRECTDRAW_H
#define __DIRECTDRAW_H

//----------------------------------------------------------------------------------------------
// Global type definitions (new version independant names for weird DX identifiers)
//----------------------------------------------------------------------------------------------

typedef LPDIRECTDRAW7				LPDD;
typedef LPDIRECTDRAWSURFACE7		LPDDS;
typedef LPDIRECTDRAWCLIPPER			LPDDC;
typedef DDSURFACEDESC2				DDSD;
typedef DDPIXELFORMAT				DDPF;

//----------------------------------------------------------------------------------------------
// External declarations
//----------------------------------------------------------------------------------------------

extern LPDD		lpdd;
extern LPDDS	lpddps;
extern LPDDS	lpddss;

//----------------------------------------------------------------------------------------------
// Exported DirectDraw helper functions
//----------------------------------------------------------------------------------------------

extern BOOL InitDD(HWND hWnd, DWORD dwWidth, DWORD dwHeight, DWORD dwBitsPerPixel, DWORD dwBackBufferCount = 0);
extern BOOL ShutdownDD();
extern BOOL CreateSurface(LPDDS &lpdds, DWORD dwWidth, DWORD dwHeight);
extern BOOL CopyBitmap(LPDDS lpdds, HBITMAP hbm, int x, int y, int dx, int dy);
extern BOOL CreateSurface(LPDDS &lpdds, const char * filename, DWORD dx = 0, DWORD dy = 0);
extern BOOL CreateClipper(LPDIRECTDRAWCLIPPER &Clipper, LPRECT ClipList, DWORD nListSize);
extern BOOL CreateClipper(LPDIRECTDRAWCLIPPER &Clipper, HWND hWnd);
extern BOOL CreatePalette(LPDIRECTDRAWPALETTE &Palette, PALETTEENTRY * Entries);
extern BOOL VerifyDD();
extern BOOL Blit(LPDDS Source, LPDDS Dest, DWORD x, DWORD y);
extern BOOL Blit(LPDDS Source, LPRECT SrcRect, LPDDS Dest, LPRECT DstRect, DWORD ColorKey);
extern BOOL Fill(LPRECT Region, DWORD Color, LPDDS lpdds);
extern BOOL Fill(DWORD x1, DWORD y1, DWORD x2, DWORD y2, DWORD Color, LPDDS lpdds);
extern BOOL Flip();
extern BOOL LoadSurface(const char * filename, DWORD dx, DWORD dy, LPDDS lpdds);
extern BOOL Lock(LPDDS lpdds, DDSD &ddsd);
extern BOOL Unlock(LPDDS lpdds);
extern DWORD ColorMatch(COLORREF rgb, LPDDS lpdds);
extern BOOL GetSurfaceSize(LPDDS lpdds, LPRECT Size);

//----------------------------------------------------------------------------------------------
#endif

