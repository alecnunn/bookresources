/* 
   Copyright (C) Nathan d'Obrenan, 2001. 
   All rights reserved worldwide.
   
   This software is provided "as is" without expressed or implied
   warranties. You may freely copy and compile this source into
   your applications, however this program is not in the public domain.
   Have fun with it!
   
   Game Programming Gems 2
   Web Cam in Video Games
   
   Nathan d'Obrenan
   Firetoad Software, Inc.
   http://www.firetoads.com/coding
   nathand@firetoads.com
*/ 

/////////////////////////////////////////////////////////////////////
// MAIN.C
/////////////////////////////////////////////////////////////////////
#include "main_includes.h"
#include "webcam_includes.h"

/////////////////////////////////////////////////////////////////////
// GLOBALS
/////////////////////////////////////////////////////////////////////
camera game_cam;

HWND  hwnd;
HWND webcam_window;
HGLRC glRC;
HDC hDC;

HINSTANCE	hInst;

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int video_source;
int video_format;
int video_display;

char appname[] = "glWebCam";

/////////////////////////////////////////////////////////////////////
// INIT
/////////////////////////////////////////////////////////////////////
void init(void)
{ 
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_TEXTURE_2D);
  
  settings_init("webcam.ini");
  cam_init(&game_cam);
  settings.wco_use_webcam = webcam_init(hwnd, FALSE);
	
	if(!settings.wco_use_webcam)
	{ 
		// Just use the aux lib to grab bitmap data
    char *filename = "NA.bmp";
    AUX_RGBImageRec *image;
    
    image = auxDIBImageLoad(filename);
    
    if(image != NULL)
    { 
      glGenTextures(1, &webcam.gl_na);
      
      glBindTexture(GL_TEXTURE_2D, webcam.gl_na);
      glTexImage2D(GL_TEXTURE_2D, 0, 3, image->sizeX, image->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
      
      free(image->data);
      free(image);
    } 
	} 
} 

/////////////////////////////////////////////////////////////////////
// DRAW_SCREEN
/////////////////////////////////////////////////////////////////////
void draw_screen(void)
{ 
  float wall = 20.0f;
  
  float ground_data[4 * 3] = 
  { 
    -wall, 0.0, -wall,
    -wall, 0.0,  wall,
     wall, 0.0,  wall,
     wall, 0.0, -wall
  };
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0f, 1.0f, 0.5f, 3000.0f);
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  /////////////////////////////////////////////////////////////////////
  // Calc our glcamera
  cam_position(&game_cam);
  cam_move(&game_cam);
  /////////////////////////////////////////////////////////////////////
  
  /////////////////////////////////////////////////////////////////////
  // Bottom square
  glPushMatrix();
  if(settings.wco_use_webcam)
  {
    if(settings.wco_cam_rendering < GREYSCALE)
  	  glBindTexture(GL_TEXTURE_2D, webcam.gl_bgr);
    else
  	  glBindTexture(GL_TEXTURE_2D, webcam.gl_grey);
  }
  else
    glBindTexture(GL_TEXTURE_2D, webcam.gl_na);
  
  glTranslatef(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 1.0f);
      glVertex3fv(&ground_data[3 * 0]);
      glTexCoord2f(0.0f, 0.0f);
      glVertex3fv(&ground_data[3 * 1]);
      glTexCoord2f(1.0f, 0.0f);
      glVertex3fv(&ground_data[3 * 2]);
      glTexCoord2f(1.0f, 1.0f);
      glVertex3fv(&ground_data[3 * 3]);
    glEnd();
  glPopMatrix();
  
  // Top square
  glPushMatrix();
  glTranslatef(0.0f, 37.0f, 0.0f);
    glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 1.0f);
      glVertex3fv(&ground_data[3 * 0]);
      glTexCoord2f(0.0f, 0.0f);
      glVertex3fv(&ground_data[3 * 1]);
      glTexCoord2f(1.0f, 0.0f);
      glVertex3fv(&ground_data[3 * 2]);
      glTexCoord2f(1.0f, 1.0f);
      glVertex3fv(&ground_data[3 * 3]);
    glEnd();
  glPopMatrix();
  
  // Front Square
  glPushMatrix();
  glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
  glTranslatef(0.0f, -17.0f, -20.0f);
    glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 1.0f);
      glVertex3fv(&ground_data[3 * 0]);
      glTexCoord2f(0.0f, 0.0f);
      glVertex3fv(&ground_data[3 * 1]);
      glTexCoord2f(1.0f, 0.0f);
      glVertex3fv(&ground_data[3 * 2]);
      glTexCoord2f(1.0f, 1.0f);
      glVertex3fv(&ground_data[3 * 3]);
    glEnd();
  glPopMatrix();
  
  // Right square
  glPushMatrix();
  glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
  glTranslatef(20.0f, -20.0f, 0.0f);
    glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 0.0f);
      glVertex3fv(&ground_data[3 * 0]);
      glTexCoord2f(1.0f, 0.0f);
      glVertex3fv(&ground_data[3 * 1]);
      glTexCoord2f(1.0f, 1.0f);
      glVertex3fv(&ground_data[3 * 2]);
      glTexCoord2f(0.0f, 1.0f);
      glVertex3fv(&ground_data[3 * 3]);
    glEnd();
  glPopMatrix();
  
  // Left square
  glPushMatrix();
  glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
  glTranslatef(20.0f, 20.0f, 0.0f);
    glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 0.0f);
      glVertex3fv(&ground_data[3 * 0]);
      glTexCoord2f(1.0f, 0.0f);
      glVertex3fv(&ground_data[3 * 1]);
      glTexCoord2f(1.0f, 1.0f);
      glVertex3fv(&ground_data[3 * 2]);
      glTexCoord2f(0.0f, 1.0f);
      glVertex3fv(&ground_data[3 * 3]);
    glEnd();
  glPopMatrix();
  /////////////////////////////////////////////////////////////////////
  
  SwapBuffers(hDC);
} 

/////////////////////////////////////////////////////////////////////
// SETTINGS_INIT
// A pretty crappy ini loader, but used for simplicity
/////////////////////////////////////////////////////////////////////
void settings_init(char *filename)
{ 
  FILE *fp;
  char temp[64];
  
  fp = fopen(filename, "r");
  
  // USE WEBCAM
  fscanf(fp, "%s %d", &temp, &settings.wco_use_webcam);
  // CAM_WIDTH
  fscanf(fp, "%s %d", &temp, &settings.wco_cam_width);
  // CAM_HEIGHT
  fscanf(fp, "%s %d", &temp, &settings.wco_cam_height);
  // CAM_RENDERING -- Which mode do we render in (color, B&W, or cartoon?)
  fscanf(fp, "%s %d", &temp, &settings.wco_cam_rendering);
  // CAM_UPDATES -- How many milliseconds do we update per second?
  fscanf(fp, "%s %d", &temp, &settings.wco_cam_updates);
  // CAM_THRESHOLD -- The movement threshold
  fscanf(fp, "%s %d", &temp, &settings.wco_cam_threshold);

  fclose(fp);
} 

/////////////////////////////////////////////////////////////////////
// WNDPROC
/////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch(iMsg)
	{
		case WM_CREATE:
		{ 
      
			return TRUE;
		} 

    case WM_KEYDOWN:
		  
      if(wParam == VK_ESCAPE)
         DestroyWindow(hwnd);
      
      if(wParam == VK_UP)
         cam_update(&game_cam, HELICAM_SPEED, 0.2f);
      
      if(wParam == VK_DOWN)
         cam_update(&game_cam, HELICAM_SPEED, -0.2f);
      
      if(wParam == VK_RIGHT)
         cam_update(&game_cam, HELICAM_HEADING, -0.04f);
      
      if(wParam == VK_LEFT)
         cam_update(&game_cam, HELICAM_HEADING, 0.04f);
      
      if(wParam == VK_PRIOR)
         cam_update(&game_cam, HELICAM_ROLL, 0.05f);
      
      if(wParam == VK_NEXT)
        cam_update(&game_cam, HELICAM_ROLL, -0.05f);
      
      if(wParam == VK_END)
        cam_update(&game_cam, HELICAM_STOP, 0.0f);
    break;

	  case WM_CHAR:
      switch(wParam)
			{
        case 'a':
          cam_update(&game_cam, HELICAM_HEIGHT, 0.2f);
        break;
        
        case 'z':
          cam_update(&game_cam, HELICAM_HEIGHT, -0.2f);
        break;
        
        case 's':
          cam_update(&game_cam, HELICAM_PITCH, -0.04f);
        break;
        
        case 'x':
          cam_update(&game_cam, HELICAM_PITCH, 0.04f);
        break;
        
        case 'q':
          cam_update(&game_cam, HELICAM_STRAFE, 0.2f);
        break;
        
        case 'w':
          cam_update(&game_cam, HELICAM_STRAFE, -0.2f);
        break;
        
        case '1':
          settings.wco_cam_rendering = COLOR;
        break;
        
        case '2':
          settings.wco_cam_rendering = CARTOON;
        break;
        
        case '3':
          settings.wco_cam_rendering = GREYSCALE;
        break;
        
        case '4':
          predator_vision = !predator_vision;
        break;
        
        // A shameless plug :)
        case 13:
          ShellExecute(webcam_window, "open","www.firetoads.com/index.htm", NULL, NULL, SW_SHOWNORMAL);
        break;
        
        case 27:
          exit(12);
        break;
			}
		break;
    
		case WM_PAINT:
    {
			HDC hDC;
			PAINTSTRUCT ps;
      
			hDC = BeginPaint(hwnd, &ps);
      
			EndPaint(hwnd, &ps);
    }
		return(0);
    
		case WM_CLOSE:
			capSetCallbackOnFrame(webcam_window, NULL); // Disable the video stream callback
		break;
    
		case WM_DESTROY:
			PostQuitMessage (0);
		return 0 ;
	} 
	return DefWindowProc(hwnd, iMsg, wParam, lParam) ;
} 

HGLRC setupGL(void) 
{
  static PIXELFORMATDESCRIPTOR pfd = 
	{
    sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
           1,                       // version number
           PFD_DRAW_TO_WINDOW       // support window
        |  PFD_SUPPORT_OPENGL       // support OpenGL
        |  PFD_DOUBLEBUFFER ,       // double buffered
        PFD_TYPE_RGBA,              // RGBA type
        32,                         // 32-bit color depth
        0, 0, 0, 0, 0, 0,           // color bits ignored
        0,                          // no alpha buffer
        0,                          // shift bit ignored
        0,                          // no accumulation buffer
        0, 0, 0, 0,                 // accum bits ignored
        32,                         // 32-bit z-buffer      
        0,                          // no stencil buffer
        0,                          // no auxiliary buffer
        PFD_MAIN_PLANE,             // main layer
        0,                          // reserved
        0, 0, 0                     // layer masks ignored
    };
    int  pixelFormat;
    HGLRC rv = 0;

    pixelFormat = ChoosePixelFormat(hDC, &pfd);
    if(pixelFormat) 
		{
      if(SetPixelFormat(hDC, pixelFormat, &pfd)) 
			{
        rv = wglCreateContext( hDC );
        if(rv)
				{
          if(!wglMakeCurrent(hDC, rv))
					{
            wglDeleteContext(rv);
            rv = 0;
          }
        }
      }
    }
    return rv;
}

int WINAPI HandledWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
  MSG msg;
  WNDCLASSEX wndclass;

	hInst = hInstance;
  wndclass.cbSize        = sizeof (wndclass);
  wndclass.style         = CS_HREDRAW | CS_VREDRAW;
  wndclass.lpfnWndProc   = WndProc;
  wndclass.cbClsExtra    = 0;
  wndclass.cbWndExtra    = 0;
  wndclass.hInstance     = hInstance;
  wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
  wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
  wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
  wndclass.lpszMenuName  = "menu";
  wndclass.lpszClassName = appname;
  wndclass.hIconSm       = LoadIcon (NULL, IDI_APPLICATION);
  
  RegisterClassEx(&wndclass);
  
  hwnd = CreateWindow(appname, appname, WS_OVERLAPPEDWINDOW,
																				  40,50,   // Window position
																				  800,600, // Window size
																				  NULL,
																					NULL,
																					hInstance,
																					NULL);
  
	hDC  = GetDC(hwnd);
	glRC = setupGL();
	if(!glRC) 
	{
		MessageBox(0, "Failed to Create OpenGL Rendering Context.", appname, MB_ICONERROR);
		DestroyWindow(hwnd);
	}
  
  init();
  
  ShowWindow(hwnd, iCmdShow);
  UpdateWindow(hwnd);
  
  while(GetMessage(&msg, NULL, 0, 0))
  { 
    draw_screen();
    
    TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

// RECORD_EXCEPTION_INFO
int __cdecl record_exception_info(PEXCEPTION_POINTERS data, const char *message)
{ 
	capSetCallbackOnFrame(webcam_window, NULL);
  DestroyWindow(webcam_window);
  DestroyWindow(hwnd);
  PostQuitMessage(0);
  
	return 0;// Make this 1 if we are in release mode
}

// WINMAIN
// Used to free up the windows video capture device (if it crashes)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	int result = -1;
  
	__try
	{
		result = HandledWinMain(hInstance,hPrevInstance,lpCmdLine,nCmdShow);
	}
  
	__except(record_exception_info(GetExceptionInformation(),"main thread"))
	{
	}
  
	return result;
}