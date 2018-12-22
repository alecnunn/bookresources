#include "StdAfx.h"
#include ".\resourcehandler.h"

extern HINSTANCE hInst;

// static declarations
HDC resourceHandler::hDC_Back = 0;
HBITMAP resourceHandler::backBmp = 0;
HDC resourceHandler::hDC_Front = 0;
int resourceHandler::screen_width = 0;
int resourceHandler::screen_height = 0;

// initializes the resourceHander
bool resourceHandler::init(HWND hWnd, int width, int height)
{
	screen_width = width;
	screen_height = height;

	hDC_Front = GetDC(hWnd);
	// create an offscreen context to draw to
	hDC_Back = CreateCompatibleDC(hDC_Front);
			
	// create an offscreen bitmap
	backBmp = CreateCompatibleBitmap(hDC_Front, width, height);
	if (backBmp != NULL)
	{
		// select the bitmap into the offscreen context
		SelectObject(hDC_Back, backBmp);
	}

	return true;
}

// releases the objects used by the resourceHandler
void resourceHandler::shutdown(HWND hWnd)
{
	// delete the offscreen bitmap and context
	DeleteObject(backBmp);

	// release the back buffer
	DeleteDC(hDC_Back);

	// release the window device context
	ReleaseDC(hWnd, hDC_Front);
}

// handles presenting the back buffer to the screen
void resourceHandler::flip(void)
{
	BitBlt(hDC_Front, 0, 0, screen_width, screen_height, hDC_Back, 0, 0, SRCCOPY);
}

bool resourceHandler::loadImageFromDisk(const std::string& filename, resourceImage *pImage)
{
	// make sure there is a valid resourceImage object
	if (pImage)
	{
		// load the bitmap from disk
		pImage->hBitmap = (HBITMAP)LoadImage(hInst, filename.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

		// confirm that the bitmap loaded
		if (pImage->hBitmap != NULL)
		{
			BITMAP Bitmap;
			GetObject(pImage->hBitmap, sizeof(BITMAP), &Bitmap);
			pImage->width = Bitmap.bmWidth;
			pImage->height = Bitmap.bmHeight;

			return true;
		}
		else
			pImage = NULL;
	}

	return false;
}

void resourceHandler::releaseImage(resourceImage *Image)
{
	if (Image)
		delete Image;
}