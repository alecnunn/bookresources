#pragma once
#include <string>

// forward class declaration
class resourceImage;

class resourceHandler
{
public:
	resourceHandler(void) = 0;
	~resourceHandler(void);

	static bool init(HWND hWnd, int width, int height);

	static void shutdown(HWND hWnd);
	
	// static functions	
	static bool loadImageFromDisk(const std::string& filename, resourceImage *pImage);

	static void releaseImage(resourceImage *Image);	

	// flip the front and back buffers
	static void flip(void);
	
	static HDC hDC_Front;				// the device context of the front buffer
	static HDC hDC_Back;				// the device context for the back buffer
private:
	static HBITMAP backBmp;				// bitmap used for the back buffer	
	
	static int screen_width;
	static int screen_height;
};

class resourceImage 
{
public:
	resourceImage() 
	{
		width = 0;
		height = 0;
		hBitmap = 0;
	};
	~resourceImage()
	{
		if (hBitmap)
			DeleteObject(hBitmap);
	};
	
	void drawImage(int locX, int locY, int XOffset, int YOffset, int width, int height)
	{
		HDC tempDC;		

		// create a temporary device context for the image
		tempDC = CreateCompatibleDC(resourceHandler::hDC_Back);
		if ((tempDC != NULL) && (hBitmap))
		{
			// select the image into the context
			(HBITMAP)SelectObject(tempDC, hBitmap);

			// draw the image to the screen
			BitBlt(resourceHandler::hDC_Back, locX, locY, width, height, tempDC, XOffset, YOffset, SRCCOPY);			
		}		
		DeleteDC(tempDC);
	}

	int		width;
	int		height;
	HBITMAP hBitmap;
};
