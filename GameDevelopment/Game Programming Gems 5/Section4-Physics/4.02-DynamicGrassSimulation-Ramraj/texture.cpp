//----------------------------------------------------------------------------
//
//texture.cpp
//
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------

//··········································································//
//    Source :: Inclusions
//··········································································//

#include <windows.h>
#include <gl/gl.h>
#include <olectl.h>
#include <math.h>

#pragma comment( lib, "opengl32.lib")

#include "texture.h"

//··········································································//
//    Source :: Definitions
//··········································································//

//··········································································//
//    Source :: Structures
//··········································································//

//··········································································//
//    Source :: Class Defs
//··········································································//

//··········································································//
//    Source :: Private Function Defs
//··········································································//

//··········································································//
//    Source :: Private Vars
//··········································································//

//··········································································//
//    Source :: External Function Implementation
//··········································································//

bool ipic_build_tex(char * path, GLuint & texid)
{
	//generate path
	char abs_path[MAX_PATH+1];

	if (strstr(path, "http://"))
		strcpy(abs_path, path);	
	else
	{
		GetCurrentDirectory(MAX_PATH, abs_path);
		char * end_char = abs_path + strlen(abs_path) - 1;
		if(*end_char != '\\')
			strcat(abs_path, "\\");
		strcat(abs_path, path);
	}
	OLECHAR ole_path[MAX_PATH+1];
	MultiByteToWideChar(CP_ACP, 0, abs_path, -1, ole_path, MAX_PATH);

	//Load ipicture

	IPicture * ipic;
	HRESULT hr = OleLoadPicturePath(ole_path, 0, 0, 0, IID_IPicture, (void**)&ipic);

	if(FAILED(hr))
		return false;

	//calculate picture size
	HDC	hDC = CreateCompatibleDC(GetDC(0));
	if(!hDC)
	{
		ipic->Release();
		return false;
	}
	

	long w_ipic;
	long h_ipic;
	long w_pixs;
	long h_pixs;
	
	ipic->get_Width(&w_ipic);
	w_pixs = MulDiv(w_ipic, GetDeviceCaps(hDC, LOGPIXELSX), 2540);
	ipic->get_Height(&h_ipic);
	h_pixs = MulDiv(h_ipic, GetDeviceCaps(hDC, LOGPIXELSY), 2540);	

	{
		GLint max_dim;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_dim);	

		if (w_pixs <= max_dim)
			w_pixs = 1 << (int)floor((log((double)w_pixs)/log(2.0f)) + 0.5f); 
		else
			w_pixs = max_dim;
 
		if (h_pixs <= max_dim)
			h_pixs = 1 << (int)floor((log((double)h_pixs)/log(2.0f)) + 0.5f);
		else 
			h_pixs = max_dim;
	}

	HBITMAP		hBMP;	
	BITMAPINFO	bi = {0};
	DWORD		*pBits = 0;

	bi.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biBitCount		= 32;
	bi.bmiHeader.biWidth		= w_pixs;
	bi.bmiHeader.biHeight		= h_pixs;
	bi.bmiHeader.biCompression	= BI_RGB;
	bi.bmiHeader.biPlanes		= 1;

	hBMP = CreateDIBSection(hDC, &bi, DIB_RGB_COLORS, (void**)&pBits, 0, 0);
	
	if(!hBMP)
	{
		DeleteDC(hDC);
		ipic->Release();
		return false;
	}

	SelectObject(hDC, hBMP);
	//ipic->Render(hDC, 0, 0, w_pixs, h_pixs, 0, 0, w_ipic, h_ipic, 0); // produces rendering anomolies
	ipic->Render(hDC, 0, 0, w_pixs, h_pixs, 0, h_ipic, w_ipic, -h_ipic, 0);	

	//convert From BGR To RGB Format And Add An Alpha Value Of 255
	for(long i = 0; i < w_pixs * h_pixs; i++)
	{
		BYTE* pPixel	= (BYTE*)(&pBits[i]);
		BYTE  temp		= pPixel[0];
		pPixel[0]		= pPixel[2];
		pPixel[2]		= temp;

		if ((pPixel[0] == 0) && (pPixel[1] == 0) && (pPixel[2] == 0))
			pPixel[3]	=   0;
		else
			pPixel[3]	= 255;
	}

	//create the gl texture
	glGenTextures(1, &texid);	
	glBindTexture(GL_TEXTURE_2D, texid);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w_pixs, h_pixs, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBits);

	DeleteObject(hBMP);
	DeleteDC(hDC);
	ipic->Release();

	return true;
}

//··········································································//
//    Source :: External Class Implementation
//··········································································//

//··········································································//
//    Source :: Internal Function Implementation
//··········································································//

//··········································································//
//    Source :: Internal Class Implementation
//··········································································//

//----------------------------------------------------------------------------
//texture.cpp
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------
