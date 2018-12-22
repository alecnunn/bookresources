#include "OGLWindow.h"

/* ***************************************************************** *\
|  CONSTRUCTION / DESTRUCTION
\* ***************************************************************** */
cOGLWindow::cOGLWindow(HINSTANCE p_Instance)
	: m_hInstance(p_Instance),
 	  m_hWnd(NULL),
	  m_hDC(NULL),
	  m_hRC(NULL),
	  m_bIsFullScreen(false),
	  m_bIsVisible(true),
	  m_iFrames(0),
	  m_iNumTriangles(0),
	  m_iHeight(0),
	  m_iWidth(0)
{
	memset(m_bKeyDown, 0, sizeof(m_bKeyDown));
}

cOGLWindow::~cOGLWindow()
{
	// cleanup and destroy window
	destroy();
}

/* ***************************************************************** *\
|  create
|  *****************************************************************
|  Creates a window ready to be used for rendering.
\* ***************************************************************** */
boolean cOGLWindow::create(TCHAR *p_Title, int32 p_Width, int32 p_Height, boolean p_Fullscreen)
{
	DWORD windowStyle = WS_OVERLAPPEDWINDOW;
	DWORD windowExtendedStyle = WS_EX_APPWINDOW;

	PIXELFORMATDESCRIPTOR pfd = { sizeof (PIXELFORMATDESCRIPTOR), 1, 
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, 
		PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		24, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0 };

	RECT windowRect = {0, 0, p_Width, p_Height};
	GLuint PixelFormat;

	if(p_Fullscreen)
	{
		if(!changeScreenResolution(p_Width, p_Height))
		{
			// Fullscreen Mode Failed.  Run In Windowed Mode Instead
			MessageBox (HWND_DESKTOP, "Mode Switch Failed.\nRunning In Windowed Mode.", "Error", MB_OK | MB_ICONEXCLAMATION);
			p_Fullscreen = false;
		}
		else
		{
			ShowCursor(FALSE);
			windowStyle = WS_POPUP;
			windowExtendedStyle |= WS_EX_TOPMOST;
		}
	}
	else
	{
		// Adjust Window, Account For Window Borders
		AdjustWindowRectEx (&windowRect, windowStyle, 0, windowExtendedStyle);
	}

	// Create The OpenGL Window
	m_hWnd = CreateWindowEx (windowExtendedStyle, m_szClassName, p_Title, windowStyle, 0, 0,
		windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, HWND_DESKTOP,
		NULL, m_hInstance, this);

	if(!m_hWnd)
		return false;

	// get device context from this window
	m_hDC = GetDC(m_hWnd);
	if(!m_hDC)
	{
		// Failed
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
		return false;
	}

	// find a comp. pixel format
	PixelFormat = ChoosePixelFormat(m_hDC, &pfd);
	if(!PixelFormat)
	{
		// Failed
		ReleaseDC (m_hWnd, m_hDC);
		m_hDC = NULL;
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
		return false;
	}

	// set the pixel format
	if(!SetPixelFormat(m_hDC, PixelFormat, &pfd))		// Try To Set The Pixel Format
	{
		// Failed
		ReleaseDC (m_hWnd, m_hDC);
		m_hDC = NULL;
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
		return false;
	}

	// get an ogl rendering context
	m_hRC = wglCreateContext(m_hDC);
	if(!m_hRC)
	{
		// Failed
		ReleaseDC (m_hWnd, m_hDC);
		m_hDC = NULL;
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
		return false;
	}

	// set current rendering context
	if(!wglMakeCurrent(m_hDC, m_hRC))
	{
		// Failed
		wglDeleteContext (m_hRC);
		m_hRC = NULL;
		ReleaseDC (m_hWnd, m_hDC);
		m_hDC = NULL;
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
		return false;
	}

	// create a console for error/status output
	if(!AllocConsole())
		MessageBox(NULL, _T("Failed to create a console!"), _T("Error"), MB_OK | MB_ICONERROR);
	else
		_tfreopen(_T("CONOUT$"), _T("wb"), stdout);

	// show our window
	ShowWindow(m_hWnd, SW_NORMAL);	
	m_bIsVisible = true;

	// save window properties
	m_iWidth  = p_Width;
	m_iHeight = p_Height;
	m_bIsFullScreen = p_Fullscreen;

	return true;
}

/* ***************************************************************** *\
|  destroy
|  *****************************************************************
|  Destroys a previously created window.
\* ***************************************************************** */
void cOGLWindow::destroy()
{
	// destroy ab-tree
	m_ABT.release();

	if(m_hWnd)
	{
		if(m_hDC)
		{
			// reset rendering context
			wglMakeCurrent(m_hDC, 0);
			if(m_hRC)
			{
				// delete rendering context
				wglDeleteContext (m_hRC);
				m_hRC = NULL;

			}
			// release the device context
			ReleaseDC(m_hWnd, m_hDC);
			m_hDC = NULL;
		} 

		// destroy the window
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}

	// don't forget to restore the mouse cursor!
	if(m_bIsFullScreen)
	{
		// reset display settings (resolution)
		ChangeDisplaySettings(NULL,0);
		ShowCursor(TRUE);
	}	

	// free the console
	fclose(stdout);
	FreeConsole();
}


/* ***************************************************************** *\
|  registerClass
|  *****************************************************************
|  Registers our window's class.
\* ***************************************************************** */
boolean cOGLWindow::registerClass()
{
	// Register A Window Class
	WNDCLASSEX windowClass;
	ZeroMemory (&windowClass, sizeof (WNDCLASSEX));
	windowClass.cbSize			= sizeof (WNDCLASSEX);
	windowClass.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc		= (WNDPROC)(WindowProc);
	windowClass.hInstance		= m_hInstance;
	windowClass.hbrBackground	= (HBRUSH)NULL;
	windowClass.hCursor			= LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName	= m_szClassName;

	// register class
	if(RegisterClassEx(&windowClass) == 0)
	{
		// NOTE: Failure, Should Never Happen
		MessageBox(HWND_DESKTOP, _T("RegisterClassEx Failed!"), _T("Error"), MB_OK | MB_ICONERROR);
		return false;
	}
	return true;
}

/* ***************************************************************** *\
|  unregisterClass
|  *****************************************************************
|  Unregisters a previously registered class
\* ***************************************************************** */
void cOGLWindow::unregisterClass()
{
	// UnRegister Window Class
	UnregisterClass(m_szClassName, m_hInstance);
}

/* ***************************************************************** *\
|  init
|  *****************************************************************
|  Initializes the OpenGL rendering context
\* ***************************************************************** */
boolean cOGLWindow::init()
{
	// init our ABT
	m_ABT.create();

	// init gl states
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glDisable(GL_TEXTURE_2D);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	GLfloat lightDiffuse[]= { 0.8f, 0.8f, 0.8f, 1.0f };					 // Diffuse Light Values 
	GLfloat	lightPosition[]= { 0.0f, 0.0f, 2.0f, 1.0f };				 // Light Position   

	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	// init viewport and matrices
	resizeGL(m_iWidth, m_iHeight);

	// init fps counter
	m_iLastFPS = GetTickCount();

	return true;
}

/* ***************************************************************** *\
|  update
|  *****************************************************************
|  Updates the scene
\* ***************************************************************** */
void cOGLWindow::update(uint32 p_Milliseconds)
{
	float32 fTransX = 0.0f;
	float32 fTransY = 0.0f;
	float32 fTransZ = 0.0f;
	float32 fRotX = 0.0f;
	float32 fRotY = 0.0f;

	if(m_bKeyDown[VK_UP])
	{
		fTransZ = ((float) ( p_Milliseconds ) / 10.0f * 25.0f);
	}
	if(m_bKeyDown[VK_DOWN])
	{
		fTransZ = -(float) ( p_Milliseconds ) / 10.0f * 25.0f;
	}

	if(m_bKeyDown[VK_RIGHT])
	{
		fTransX = -((float) ( p_Milliseconds ) / 10.0f * 25.0f);
	}
	if(m_bKeyDown[VK_LEFT])
	{
		fTransX = (float) ( p_Milliseconds ) / 10.0f * 25.0f;
	}

	if(m_bKeyDown[VK_PRIOR])
	{
		fTransY = -((float) ( p_Milliseconds ) / 10.0f * 25.0f);
	}
	if(m_bKeyDown[VK_NEXT])
	{
		fTransY = (float) ( p_Milliseconds ) / 10.0f * 25.0f;
	}

	if(m_bKeyDown['W'])
	{
		fRotX = -(float) ( p_Milliseconds ) / 1000.0f * 25.0f;
	}
	if(m_bKeyDown['S'])
	{
		fRotX = ((float) ( p_Milliseconds ) / 1000.0f * 25.0f);
	}

	if(m_bKeyDown['D'])
	{
		fRotY = ((float) ( p_Milliseconds ) / 1000.0f * 25.0f);
	}
	if(m_bKeyDown['A'])
	{
		fRotY = -((float) ( p_Milliseconds ) / 1000.0f * 25.0f);
	}
	// on escape close the application
	if(m_bKeyDown[VK_ESCAPE])
	{
		PostMessage(m_hWnd, WM_QUIT, 0, 0);
	}

	// update the camera pos/orientation
	GLfloat	curMat[16];

	glGetFloatv(GL_MODELVIEW_MATRIX, curMat);
    
	glLoadIdentity();
	glRotatef(fRotY, 0.0f, 1.0f, 0.0f);
	glTranslatef(fTransX, fTransY, fTransZ);
	glRotatef(fRotX, 1.0f, 0.0f, 0.0f);
	glMultMatrixf(curMat);
}

/* ***************************************************************** *\
|  draw
|  *****************************************************************
|  Draws the current scene
\* ***************************************************************** */
void cOGLWindow::draw()
{
	// clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// render the ABT
	int iNumRenderedTris = m_ABT.render();

	// determine FPS
	if(GetTickCount() - m_iLastFPS >= 1000) // When A Second Has Passed...
	{
		m_iLastFPS = GetTickCount();
		TCHAR szTitle[256];
		_stprintf(szTitle, _T("GPG6 - ABT (%d FPS, %d Tris)"), m_iFrames, iNumRenderedTris);
		SetWindowText(m_hWnd, szTitle);
		m_iFrames = 0;
	}
	m_iFrames++;

	// draw the scene
	SwapBuffers(m_hDC);
}

/* ***************************************************************** *\
|  changeScreenResolution
|  *****************************************************************
|  Changes the screen resolution
\* ***************************************************************** */
boolean cOGLWindow::changeScreenResolution(uint32 p_Width, uint32 p_Height)
{
	DEVMODE dmScreenSettings;

	// init new screen settings
	ZeroMemory (&dmScreenSettings, sizeof(DEVMODE));
	dmScreenSettings.dmSize			= sizeof(DEVMODE);
	dmScreenSettings.dmPelsWidth	= p_Width;
	dmScreenSettings.dmPelsHeight	= p_Height;
	dmScreenSettings.dmBitsPerPel	= 32;
	dmScreenSettings.dmFields		= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

	// change resolution
	if(ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		return false;

	return true;
}

/* ***************************************************************** *\
|  resizeGL
|  *****************************************************************
|  Initializes the opengl matrices
\* ***************************************************************** */
void cOGLWindow::resizeGL(uint32 p_Width, uint32 p_Height)
{
	glViewport(0, 0, (GLsizei)(p_Width), (GLsizei)(p_Height));
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// this projection matrix maybe seems akward ... but we get problems with z-fighting
	// otherwise (when using the powerplant model)
	gluPerspective(45.0f, (GLfloat)(p_Width)/(GLfloat)(p_Height), 100.0f, 100000.0f);		
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, 300.0f, 300.0f);
	glScalef(0.5f, 0.5f, 0.5f);
}

/* ***************************************************************** *\
|  WindowProc
|  *****************************************************************
|  Window message procedure
\* ***************************************************************** */
LRESULT CALLBACK cOGLWindow::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Get The Window Context
	cOGLWindow* window = (cOGLWindow*)(GetWindowLongPtr(hWnd, GWL_USERDATA));

	switch (uMsg)														
	{
		case WM_SYSCOMMAND:
			{
				switch (wParam)
				{
					case SC_SCREENSAVE:		// Screensaver Trying To Start?
					case SC_MONITORPOWER:	// Monitor Trying To Enter Powersave?
						return 0;			// Prevent From Happening
				}
				break;
			}
			return 0;

		case WM_CREATE:
			{
				// store a reference to our window class ...
				CREATESTRUCT* creation = (CREATESTRUCT*)(lParam);
				window = (cOGLWindow*)(creation->lpCreateParams);
				SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)window);
			}
			return 0;

		case WM_CLOSE:
			PostMessage(window->m_hWnd, WM_QUIT, 0, 0);
			return 0;

			// window resized
		case WM_SIZE:
			switch(wParam)
			{
				case SIZE_MINIMIZED:
					window->m_bIsVisible = false;
					return 0;

				case SIZE_MAXIMIZED:
					window->m_bIsVisible = true;
					window->resizeGL(LOWORD(lParam), HIWORD(lParam));
					return 0;

				case SIZE_RESTORED:
					window->m_bIsVisible = true;
					window->resizeGL(LOWORD(lParam), HIWORD(lParam));
					return 0;
			}
			break;

		case WM_KEYDOWN:
			if ((wParam >= 0) && (wParam <= 255))
			{
				// Set The Selected Key (wParam) To True
				window->m_bKeyDown[wParam] = true;
				return 0;
			}
			break;

		case WM_KEYUP:
			if ((wParam >= 0) && (wParam <= 255))
			{
				window->m_bKeyDown[wParam] = false;
				return 0;
			}
			break;
	}

	// forward to default window proc ...
	return DefWindowProc (hWnd, uMsg, wParam, lParam);
}
