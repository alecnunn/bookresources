/* Copyright (C) John W. Ratcliff, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) John W. Ratcliff, 2001"
 */
#ifndef MAIN_H
#define MAIN_H

/***********************************************************************/
/** MAIN        : The windows test application demonstrating the       */
/**               SphereTree system for Game Programming Gems 2        */
/**                                                                    */
/**               Written by John W. Ratcliff jratcliff@att.net        */
/***********************************************************************/



//======================================================================================
class Application;
class CircleFactory;


//======================================================================================
int DrawCircle(int locx, int locy, int radius, int color);
int PrintText(int x, int y, int color, char* output, ...);
int DrawLine(int x1, int y1, int x2, int y2, int color);


//======================================================================================
class Application
{
public:
													Application(int simcount);
													~Application(void);

	int											Startup(HINSTANCE hinstnace);
	int											Shutdown(void);
	int											Process(void);
	int											Render(HDC hdc);

	void										PauseApp(void)	{ mStep = true; mPause = true; };
	void										RunApp(void)		{ mStep = false; mPause = false; };

	bool										ExitApp(void) { return mExitApplication; };
	void										CloseApp(bool close) { mExitApplication = close; };

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	HINSTANCE								mAppInstance;
	WNDCLASSEX							mWindowClass;
	HWND										mWindow;

	static CircleFactory						*mCircleFactory;

	int											mFrameRate;
	unsigned int						mUpdateFrameRate;
	char										mBuffer[32];
	char										mTextBuffer[1024];

	bool										mExitApplication;
	bool										mPause;
	bool										mStep;
  int                     mSimulationCount;
};

#endif MAIN_H
