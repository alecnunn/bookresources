//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    DirectDraw.cpp
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "DirectDraw.h"
#include <stdio.h>

//----------------------------------------------------------------------------------------------
// The following functions are from some old, long forgotten personal DX library. I couldn't 
// find the courage to document them properly, but take a look if you like...
//----------------------------------------------------------------------------------------------


extern BOOL RestoreAll(BOOL reinit);

LPDD			lpdd	= NULL;
LPDDS			lpddps	= NULL;
LPDDS			lpddss	= NULL;

HRESULT WINAPI EnumSurfacesCallback2(LPDIRECTDRAWSURFACE7 lpDDSurface,  LPDDSURFACEDESC2 lpDDSurfaceDesc,  LPVOID lpContext){
	if (lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_BACKBUFFER){
		lpddss =  lpDDSurface;
		return DDENUMRET_CANCEL;
	}
	return DDENUMRET_OK;
}

BOOL InitDD(HWND hWnd, DWORD dwWidth, DWORD dwHeight, DWORD dwBitsPerPixel, DWORD dwBackBufferCount) {
	LPDIRECTDRAW gpDD;
	HRESULT	ddrval;
	DDSD ddsd;

	ddrval = DirectDrawCreate(NULL, &gpDD, NULL);
    if (ddrval != DD_OK)
        return FALSE;

    ddrval = gpDD->QueryInterface(IID_IDirectDraw7, (LPVOID *) & lpdd);
    if (ddrval != DD_OK)
        return FALSE;

	gpDD->Release();

    ddrval = lpdd->SetCooperativeLevel(hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT);
    if (ddrval != DD_OK) {
		MessageBox(NULL, "Could not set cooperative level!", "", 0);
        return FALSE;
	}

    ddrval = lpdd->SetDisplayMode(dwWidth, dwHeight, dwBitsPerPixel, 0, 0);
    if (ddrval != DD_OK) {
		MessageBox(NULL, "Could not set display mode!", "", 0);
        return FALSE;
	}

    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    
	if (dwBackBufferCount>0) {
		ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
		ddsd.dwBackBufferCount = dwBackBufferCount;
	}
	else {
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	}
    
    ddrval = lpdd->CreateSurface(&ddsd, &lpddps, NULL);
    if (ddrval != DD_OK) {
		MessageBox(NULL, "Could not get primary surface!", "", 0);
        return FALSE;
	}

	if (dwBackBufferCount>0) {
		lpddps->EnumAttachedSurfaces(0,&EnumSurfacesCallback2);
	} else {
		lpddss = NULL;
	}		
#if 0 // there appears to be a bug with GetAttachedSurfaces on some versions of DirectX...
	DDSCAPS2 ddscaps;
	if (dwBackBufferCount>0) {
		ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
		ddrval = lpddps->GetAttachedSurface(&ddscaps, &lpddss);
		if (ddrval != DD_OK) {
			MessageBox(NULL, "Could not get secondary surface!", "", 0);
			return FALSE;
		}
	} else {
		lpddss = NULL;
	}		
#endif
	return TRUE;
}


BOOL ShutdownDD() {
	if (lpdd != NULL) {
		if (lpddps != NULL) {
			lpddps->Release();
			lpddps = NULL;
		}
		lpdd->RestoreDisplayMode();
		lpdd->Release();
		lpdd = NULL;
	}
	return TRUE;
}


BOOL CreateSurface(LPDDS &lpdds, DWORD dwWidth, DWORD dwHeight) {
	DDSD ddsd;
	HRESULT	ddrval;

	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;

	ddsd.dwWidth = dwWidth;
	ddsd.dwHeight = dwHeight;

	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	
	ddrval = lpdd->CreateSurface(&ddsd,&lpdds,NULL);
	return (ddrval == DD_OK);
}


BOOL CopyBitmap(LPDDS lpdds, HBITMAP hbm, int x, int y, int dx, int dy) {
    HDC         hdcImage;
    HDC         hdc;
    BITMAP      bm;
    DDSD		ddsd;
    HRESULT     ddrval;

    if (hbm == NULL || lpdds == NULL)
		return FALSE;

    lpdds->Restore();

    hdcImage = CreateCompatibleDC(NULL);
    if (!hdcImage)
		return FALSE;
	
    SelectObject(hdcImage, hbm);

    GetObject(hbm, sizeof(bm), &bm);
    dx = dx == 0 ? bm.bmWidth  : dx;
    dy = dy == 0 ? bm.bmHeight : dy;

    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_HEIGHT | DDSD_WIDTH;
    ddrval = lpdds->GetSurfaceDesc(&ddsd);
	if (ddrval != DD_OK) 
		return FALSE;

	ddrval = lpdds->GetDC(&hdc);
    if (ddrval == DD_OK) {	
		StretchBlt(hdc, 0, 0, ddsd.dwWidth, ddsd.dwHeight, hdcImage, x, y, dx, dy, SRCCOPY);
		lpdds->ReleaseDC(hdc);
	}
    DeleteDC(hdcImage);

    return (ddrval==DD_OK);
}


BOOL CreateSurface(LPDDS &lpdds, const char * filename, DWORD dx, DWORD dy) {
	HRESULT ddrval;
    HBITMAP hbm;
    BITMAP bm;
    DDSD ddsd;

    hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), filename, IMAGE_BITMAP, dx, dy, LR_CREATEDIBSECTION);

    if (hbm == NULL)
	hbm = (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, dx, dy, LR_LOADFROMFILE|LR_CREATEDIBSECTION);

    if (hbm == NULL)
		return FALSE;
    
    GetObject(hbm, sizeof(bm), &bm);

    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    ddsd.dwWidth = bm.bmWidth;
    ddsd.dwHeight = bm.bmHeight;

	ddrval = lpdd->CreateSurface(&ddsd, &lpdds, NULL);
    if (ddrval != DD_OK)
		return FALSE;
		
    CopyBitmap(lpdds, hbm, 0, 0, 0, 0);
    DeleteObject(hbm);

	return TRUE;
}


BOOL CreateClipper(LPDIRECTDRAWCLIPPER &Clipper, LPRECT ClipList, DWORD nListSize) {
	HRESULT ddrval;
	LPRGNDATA regiondata;

	ddrval = lpdd->CreateClipper(0,&Clipper,NULL);
	if (ddrval != DD_OK)
		return FALSE;

	regiondata = (LPRGNDATA)malloc(sizeof(RGNDATAHEADER) +
				  nListSize*sizeof(RECT));

	memcpy(regiondata->Buffer, ClipList, sizeof(RECT)*nListSize);
	regiondata->rdh.dwSize = sizeof(RGNDATAHEADER);
	regiondata->rdh.iType = RDH_RECTANGLES;
	regiondata->rdh.nCount = nListSize;
	regiondata->rdh.nRgnSize = nListSize*sizeof(RECT);

	regiondata->rdh.rcBound.left = 64000;
	regiondata->rdh.rcBound.top = 64000;
	regiondata->rdh.rcBound.right = -64000;
	regiondata->rdh.rcBound.bottom = -64000;

	for (int i=0; i<(int)nListSize; i++) {
		if (ClipList[i].left < regiondata->rdh.rcBound.left) 
			regiondata->rdh.rcBound.left = ClipList[i].left;
		if (ClipList[i].top < regiondata->rdh.rcBound.top) 
			regiondata->rdh.rcBound.top = ClipList[i].top;
		if (ClipList[i].right > regiondata->rdh.rcBound.right) 
			regiondata->rdh.rcBound.right = ClipList[i].right;
		if (ClipList[i].bottom > regiondata->rdh.rcBound.bottom) 
			regiondata->rdh.rcBound.bottom = ClipList[i].bottom;
	}

	if (Clipper->SetClipList(regiondata,0) != DD_OK) {
		free(regiondata);
		return FALSE;
	}

	free(regiondata);
	return TRUE;
}


BOOL CreateClipper(LPDIRECTDRAWCLIPPER &Clipper, HWND hWnd) {
	HRESULT ddrval;

	ddrval = lpdd->CreateClipper(0,&Clipper,NULL);
	if (ddrval != DD_OK)
		return FALSE;

	ddrval = Clipper->SetHWnd(0,hWnd);
	if (ddrval != DD_OK) 
		return FALSE;

	return TRUE;
}


BOOL CreatePalette(LPDIRECTDRAWPALETTE &Palette, PALETTEENTRY * Entries) {
	HRESULT ddrval;
	ddrval = lpdd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_INITIALIZE |
							     DDPCAPS_ALLOW256 , Entries, &Palette, NULL);
	
	return (ddrval == DD_OK);
}


BOOL VerifyDD() {
	HRESULT ddrval;
	ddrval = lpddps->IsLost();
	if (ddrval != DD_OK) {
		if (ddrval == DDERR_SURFACELOST) {
			ddrval = lpdd->RestoreAllSurfaces();
			if (ddrval != DD_OK && ddrval != DDERR_WRONGMODE) 
				return FALSE;

			return (RestoreAll(ddrval == DDERR_WRONGMODE));			
		} 
		else {
			return FALSE;
		}
	}
	return TRUE;
}


BOOL Blit(LPDDS Source, LPDDS Dest, DWORD x, DWORD y) {
	HRESULT ddrval;
	DDSD ddsd;

	if (!VerifyDD()) 
		return FALSE;

	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddrval = Source->GetSurfaceDesc(&ddsd);
	if (ddrval != DD_OK)
		return FALSE;

	RECT ImageSize = { 0, 0, ddsd.dwWidth-1, ddsd.dwHeight-1 };
	ddrval = Dest->BltFast(x,y,Source,&ImageSize, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
	
	return (ddrval == DD_OK);
}


BOOL Blit(LPDDS Source, LPRECT SrcRect, LPDDS Dest, LPRECT DstRect, DWORD ColorKey) {
	HRESULT ddrval;
	DDCOLORKEY ck;

	ck.dwColorSpaceLowValue = ColorKey;
	ck.dwColorSpaceHighValue = ColorKey;

	VerifyDD();

	ddrval = Source->SetColorKey(DDCKEY_SRCBLT,&ck);
	if (ddrval != DD_OK) 
		return FALSE;

	ddrval = Dest->Blt(DstRect,Source,SrcRect,DDBLT_KEYSRC | DDBLT_WAIT, NULL);

	return (ddrval == DD_OK);
}


BOOL Fill(LPRECT Region, DWORD Color, LPDDS lpdds) {
	DDBLTFX ddbltfx;
	DDSD ddsd;
	HRESULT ddrval;
	RECT Contents;

	if (Region==NULL) {
		ZeroMemory(&ddsd,sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);

		ddrval = lpdds->GetSurfaceDesc(&ddsd);
		if (ddrval != DD_OK) 
			return FALSE;
		Contents.left = 0;
		Contents.top = 0;
		Contents.right = ddsd.dwWidth;
		Contents.bottom = ddsd.dwHeight;
	}
	else {
		Contents = *Region;
	}
	
	ZeroMemory(&ddbltfx, sizeof(ddbltfx));
	ddbltfx.dwSize = sizeof(ddbltfx);
	ddbltfx.dwFillColor = Color;

	VerifyDD();

	ddrval = lpdds->Blt(&Contents, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT,&ddbltfx);	
	return (ddrval == DD_OK);
}


BOOL Fill(DWORD x1, DWORD y1, DWORD x2, DWORD y2, DWORD Color, LPDDS lpdds) {
	RECT Region = { x1, y1, x2, y2 };
	return Fill(&Region, Color, lpdds);
}


BOOL Flip() {
	HRESULT ddrval;
	
	VerifyDD();

	ddrval = lpddps->Flip(NULL,DDFLIP_WAIT);
	return (ddrval == DD_OK) ;
}


BOOL LoadSurface(const char * filename, DWORD dx, DWORD dy, LPDDS lpdds) {
    HBITMAP hbm;
    BOOL rval;

    hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), filename, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

    if (hbm == NULL)
		hbm = (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);

    if (hbm == NULL)
		return FALSE;
    
    rval = CopyBitmap(lpdds, hbm, dx, dy, 0, 0);
    
    DeleteObject(hbm);
    return rval;
}


BOOL Lock(LPDDS lpdds, DDSD &ddsd) {
	HRESULT ddrval;

	ZeroMemory(&ddsd,sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);

	VerifyDD();

	ddrval = lpdds->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
	return (ddrval == DD_OK);
}


BOOL Unlock(LPDDS lpdds) {
	HRESULT ddrval;
	ddrval = lpdds->Unlock(NULL);
	return (ddrval == DD_OK);
}


DWORD ColorMatch(COLORREF rgb, LPDDS lpdds) {
    COLORREF rgbT;
    HDC hdc;
    DWORD dw = CLR_INVALID;
    DDSD ddsd;
    HRESULT ddrval;

    if (rgb != CLR_INVALID && lpdds->GetDC(&hdc) == DD_OK) {
		rgbT = GetPixel(hdc, 0, 0);
		SetPixel(hdc, 0, 0, rgb);
		lpdds->ReleaseDC(hdc);
    }

    ddsd.dwSize = sizeof(ddsd);
    ddrval = lpdds->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);

	if (ddrval == DD_OK) {
		dw  = *(DWORD *)ddsd.lpSurface;
		dw &= (1 << ddsd.ddpfPixelFormat.dwRGBBitCount)-1;
		lpdds->Unlock(NULL);
    }

    if (rgb != CLR_INVALID && lpdds->GetDC(&hdc) == DD_OK) {
		SetPixel(hdc, 0, 0, rgbT);
		lpdds->ReleaseDC(hdc);
    }

    return dw;
}


BOOL GetSurfaceSize(LPDDS lpdds, LPRECT Size) {
	Size->left = 0;
	Size->top = 0;

	DDSD ddsd;
	HRESULT ddrval;

	ZeroMemory(&ddsd,sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddrval = lpdds->GetSurfaceDesc(&ddsd);
	if (ddrval != DD_OK) 
		return FALSE;

	Size->right = ddsd.dwWidth - 1;
	Size->bottom = ddsd.dwHeight - 1;
	return TRUE;
}

