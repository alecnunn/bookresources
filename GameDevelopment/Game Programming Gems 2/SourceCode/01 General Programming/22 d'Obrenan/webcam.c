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
// WEBCAM.C
/////////////////////////////////////////////////////////////////////
#include "main_includes.h"
#include "webcam_includes.h"

/////////////////////////////////////////////////////////////////////
// GLOBALS
/////////////////////////////////////////////////////////////////////
BOOL predator_vision;  //Do we just want to see the delta changes?
webcam_data webcam;
webcam_settings settings;
HWND hWndCam = NULL;

/////////////////////////////////////////////////////////////////////
// WEBCAM_INIT
/////////////////////////////////////////////////////////////////////
BOOL webcam_init(HWND hWnd, BOOL retry)
{ 
  CAPDRIVERCAPS	caps;
  BITMAPINFO capwnd;
  
  if(settings.wco_use_webcam)
  { 
    // Get the handle of the capture window
    hWndCam = 
			capCreateCaptureWindow("Webcam Window",
														 WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
														 0,0,
														 1,
														 1,
														 hWnd,
														 0);
    
    if(hWndCam)
    { 
      // Connect the cam to the driver
      webcam.driver = capDriverConnect(hWndCam, 1);
      
      // Get the capabilities of the capture driver
      if(webcam.driver)
      { 
        capDriverGetCaps(hWndCam, &caps, sizeof(caps));
        
        // Set the video stream callback function
        capSetCallbackOnFrame(hWndCam, webcam_callback);
        
        // Set the preview rate in milliseconds
        capPreviewRate(hWndCam, settings.wco_cam_updates);
        
        // Disable preview mode
        capPreview(hWndCam, FALSE);
        
        // Setup the data we want returned to us
        memset(&capwnd,0,sizeof(BITMAPINFO));
        capwnd.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        capwnd.bmiHeader.biWidth  = settings.wco_cam_width;
        capwnd.bmiHeader.biHeight = settings.wco_cam_height;
        capwnd.bmiHeader.biPlanes = 1;
        capwnd.bmiHeader.biBitCount = 24;
        capwnd.bmiHeader.biCompression = BI_RGB;
        capwnd.bmiHeader.biSizeImage = settings.wco_cam_width * settings.wco_cam_height * 3;
        capwnd.bmiHeader.biXPelsPerMeter = 100;
        capwnd.bmiHeader.biYPelsPerMeter = 100;
        
        if(capSetVideoFormat(hWndCam, &capwnd, sizeof(BITMAPINFO)) == FALSE)
        { 
          if(!retry)
          { 
            printf("Can't set the webcamera!");
            return FALSE;
          } 
          else
          { 
            capSetCallbackOnFrame(hWndCam, NULL); // Disable the callback function
            DestroyWindow(hWndCam);
            hWndCam = NULL;
            
            printf("Can't set the webcamera!");
            return FALSE;
          } 
        } 
        else
        { 
          webcam_set_vars();
          
          /////////////////////////////////////////////////////////////////////
          // Init webcam textures
          /////////////////////////////////////////////////////////////////////
          
          // WEBCAM RGB texture
          glGenTextures(1, &webcam.gl_bgr);
          glBindTexture(GL_TEXTURE_2D, webcam.gl_bgr);
          glTexImage2D(GL_TEXTURE_2D, 0, 3, webcam.size, webcam.size, 0, GL_BGR_EXT,GL_UNSIGNED_BYTE, webcam.bgr);
          
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
          
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          
          // GREYSCALE texture
          glGenTextures(1, &webcam.gl_grey);
          glBindTexture(GL_TEXTURE_2D, webcam.gl_grey);
          glTexImage2D(GL_TEXTURE_2D, 0, 1, webcam.size, webcam.size, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, webcam.greyscale);
          
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
          
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        } 
      } 
      else
      { 
        DestroyWindow(hWndCam);
        hWndCam = NULL;
        printf("Can't set the webcamera!");
        
        return FALSE;
      } 
    } 
    else
    { 
      DestroyWindow(hWndCam);
      hWndCam = NULL;
      printf("Can't set the webcamera!");
      
      return FALSE;
    } 
  } 
  else
		return FALSE;
  
  return TRUE;
} 

/////////////////////////////////////////////////////////////////////
// WEBCAM_SET_VARS
/////////////////////////////////////////////////////////////////////
void webcam_set_vars(void)
{ 
  // Setup the capture for the camera -- note, these always need to be a bit larger than the height,
  // we loose the top few pixels, but its worth it, as we just have to do a memcpy, instead of actually
  // resizing the texture
  switch(settings.wco_cam_height)
  { 
		case 120:
      webcam.size = 128;
      break;
      
    case 240:
      webcam.size = 256;
      break;
      
    case 480:
      webcam.size = 512;
      break;
  } 
  
  webcam_init_cartoon(&webcam.cartoon);
  
  webcam.which_buffer = 1;
  
  // Assign mem
  webcam.bgr          = (unsigned char *) malloc(sizeof(unsigned char)* webcam.size * webcam.size * 3);
  webcam.greyscale    = (unsigned char *) malloc(sizeof(unsigned char)* webcam.size * webcam.size);
  webcam.delta_buffer = (unsigned char *) malloc(sizeof(unsigned char)* webcam.size * webcam.size);
  
  webcam.back_buffer[0] = (unsigned char *) malloc(sizeof(unsigned char)* settings.wco_cam_width * settings.wco_cam_height);
  webcam.back_buffer[1] = (unsigned char *) malloc(sizeof(unsigned char)* settings.wco_cam_width * settings.wco_cam_height);
  
  // Clear everything
  memset(webcam.bgr,           0,sizeof(unsigned char) * webcam.size * webcam.size * 3);
  memset(webcam.greyscale,     0,sizeof(unsigned char) * webcam.size * webcam.size);
  memset(webcam.delta_buffer,  0,sizeof(unsigned char) * webcam.size * webcam.size);
  
  memset(webcam.back_buffer[0],0,sizeof(unsigned char) * settings.wco_cam_width * settings.wco_cam_height);
  memset(webcam.back_buffer[1],0,sizeof(unsigned char) * settings.wco_cam_width * settings.wco_cam_height);
} 

/////////////////////////////////////////////////////////////////////
// WEBCAM_CALLBACK
// Process video callbacks here
/////////////////////////////////////////////////////////////////////
LRESULT CALLBACK webcam_callback(HWND hwnd, LPVIDEOHDR video_hdr)
{ 
  // Calculate movement based off of threshold
  if(webcam_calc_movement(video_hdr, 
    webcam.delta_buffer, 
    settings.wco_cam_width, 
    settings.wco_cam_height, 
    webcam.size, 
    settings.wco_cam_threshold))
  { 
    webcam_make_texture(video_hdr, settings.wco_cam_rendering);
  } 
  
  return TRUE;
} 

/////////////////////////////////////////////////////////////////////
// WEBCAM_MAKE_TEXTURE
/////////////////////////////////////////////////////////////////////
fs_void webcam_make_texture(LPVIDEOHDR video, webcam_draw_mode mode)
{ 
  // Build the color first
  webcam_make_bgr(webcam.bgr, video->lpData, settings.wco_cam_width, settings.wco_cam_height, webcam.size);
  
  if(mode == GREYSCALE || mode == CARTOON)
    webcam_make_greyscale(webcam.greyscale, webcam.bgr, webcam.size);
  
  if(mode == CARTOON)
  { 
    // Here we just pass in the delta buffer (webcam.delta_buffer) instead of the greyscale
    if(predator_vision)
      webcam_make_cartoon(webcam.bgr, webcam.cartoon, webcam.delta_buffer, webcam.size);
    else
      webcam_make_cartoon(webcam.bgr, webcam.cartoon, webcam.greyscale, webcam.size);
  }
  
  /////////////////////////////////////////////////////////////////////
  
  // Upload the greyscale version to OpenGL
  if(mode == GREYSCALE)  
  { 
    glBindTexture(GL_TEXTURE_2D, webcam.gl_grey);
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0, webcam.size, webcam.size, GL_LUMINANCE, GL_UNSIGNED_BYTE, webcam.greyscale);
  } 
  // Upload the color version to OpenGL
  else
  { 
    glBindTexture(GL_TEXTURE_2D, webcam.gl_bgr);
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0, webcam.size, webcam.size, GL_BGR_EXT, GL_UNSIGNED_BYTE, webcam.bgr);
  } 
} 

/////////////////////////////////////////////////////////////////////
// WEBCAM_MAKE_BGR
/////////////////////////////////////////////////////////////////////
void webcam_make_bgr(unsigned char *bgr_tex, unsigned char *vid_data, int webcam_width, int webcam_height, int glsize)
{ 
  int i;
  
  for(i=0; i<webcam_height; i++)
  { 
    memcpy(&bgr_tex[i*(glsize*3)], &vid_data[i*(webcam_width*3)], sizeof(unsigned char)*webcam_width*3);
  } 
} 

/////////////////////////////////////////////////////////////////////
// WEBCAM_MAKE_GREYSCALE
/////////////////////////////////////////////////////////////////////
void webcam_make_greyscale(unsigned char *grey, unsigned char *color, int dim)
{ 
  int i, j;
  
  // Greyscale = RED * 0.3f + GREEN * 0.4f + BLUE * 0.3f
  for(i=0, j=0; j<dim*dim; i+=3, j++)
  { 
    grey[j] = (fs_uchar)float_to_int(0.3f * color[i] + 0.4f * color[i+1] + 0.3f * color[i+2]);
  } 
} 

/////////////////////////////////////////////////////////////////////
// WEBCAM_INIT_CARTOON
// Pick a random color between 0 and 255 for our different cartoon levels
/////////////////////////////////////////////////////////////////////
void webcam_init_cartoon(cartoon_data *cartoon_tex)
{ 
  char i;
  
  srand((unsigned)time(NULL));
  
  for(i=0; i<3; i++)
  { 
    cartoon_tex->bottom[i]  = rand()%255;
    cartoon_tex->minimum[i]  = rand()%255;
    cartoon_tex->lowest[i]  = rand()%255;
    cartoon_tex->medium[i]  = rand()%255;
    cartoon_tex->highest[i] = rand()%255;
    cartoon_tex->maximum[i]  = rand()%255;
  } 
} 

#define  MIN_CAM_HEAT   50
#define  LOW_CAM_HEAT   75
#define  MED_CAM_HEAT  100
#define HIGH_CAM_HEAT  125
#define  MAX_CAM_HEAT  150

/////////////////////////////////////////////////////////////////////
// WEBCAM_MAKE_CARTOON
/////////////////////////////////////////////////////////////////////
void webcam_make_cartoon(unsigned char *cartoon, cartoon_data cartoon_tex, unsigned char *data, int dim)
{ 
  int i, j, n;
  
  for(i=0, j=0; j<dim*dim; i+=3, j++)
  { 
    if(data[j] < MIN_CAM_HEAT)
    { 
      for(n=0; n<3; n++)
        cartoon[i+n] = cartoon_tex.bottom[n];
    } 
    if(data[j] > MIN_CAM_HEAT && data[j] < LOW_CAM_HEAT)
    { 
      for(n=0; n<3; n++)
        cartoon[i+n] = cartoon_tex.minimum[n];
    } 
    if(data[j] > LOW_CAM_HEAT && data[j] < MED_CAM_HEAT)
    { 
      for(n=0; n<3; n++)
        cartoon[i+n] = cartoon_tex.lowest[n];
    } 
    if(data[j] > MED_CAM_HEAT && data[j] < HIGH_CAM_HEAT)
    { 
      for(n=0; n<3; n++)
        cartoon[i+n] = cartoon_tex.medium[n];
    } 
    if(data[j] > HIGH_CAM_HEAT && data[j] < MAX_CAM_HEAT)
    { 
      for(n=0; n<3; n++)
        cartoon[i+n] = cartoon_tex.highest[n];
    } 
    if(data[j] > MAX_CAM_HEAT)
    { 
      for(n=0; n<3; n++)
        cartoon[i+n] = cartoon_tex.maximum[n];
    } 
  } 
}

/////////////////////////////////////////////////////////////////////
// WEBCAM_CALC_MOVEMENT
// This is a simple motion detection routine that determines if youve 
// moved further than the set threshold
/////////////////////////////////////////////////////////////////////
BOOL webcam_calc_movement(LPVIDEOHDR video_hdr, unsigned char *delta_buff, int webcam_width, int webcam_height, int gl_size, int thresh)
{ 
  unsigned char max_delta=0;
  int i=0, j=0;
  int length;
  unsigned char *temp_delta = (unsigned char*) malloc(sizeof(unsigned char)* webcam_width * webcam_height);
  
  length = webcam_width * webcam_height;
  
  webcam.which_buffer = webcam.which_buffer ? 0 : 1;
  
  if(!video_hdr->lpData)
    return TRUE;
  
  for(i=0; i<length; i++)
  { 
    // Save the current frames data for comparison on the next frame
    // NOTE: Were only comparing the red channel (lpData is BGR), so 
    // in theory if the user was in a solid red room, wearing all red,
    // we wouldn't detect any movement....chances are this isn't the 
    // case :)  For our purposes, it works fine
    webcam.back_buffer[webcam.which_buffer][i] = video_hdr->lpData[i*3];
    
    // Compute the delta buffer from the last frame
    // If its the first frame, it shouldn't blow up given we cleared it to zero upon initialization
    temp_delta[i] = abs(webcam.back_buffer[ webcam.which_buffer][i] - 
      webcam.back_buffer[!webcam.which_buffer][i]);
    
    // Is the difference here greater than our threshold?
    if(temp_delta[i] > max_delta)
      max_delta = temp_delta[i];
    // NOTE: If you don't care about saving the delta buffer, you can just return out right here
  } 
  
  // Fit to be inside a power of 2 texture
  for(i=0; i<webcam_height; i++)
  { 
    memcpy(&delta_buff[i*(gl_size)], &temp_delta[i*(webcam_width)], sizeof(unsigned char)*webcam_width);
  } 
  
  free(temp_delta);
  
  if(max_delta > thresh)
    return TRUE;
  else
    return FALSE;
} 

/////////////////////////////////////////////////////////////////////
// WEBCAM_DESTROY
/////////////////////////////////////////////////////////////////////
void webcam_destroy(void)
{ 
  if(webcam.driver)
  { 
    capSetCallbackOnFrame(hWndCam, NULL);
    DestroyWindow(hWndCam);
    hWndCam = NULL;
    
    if(webcam.bgr)
    { 
      free(webcam.bgr);
      webcam.bgr = 0;
    } 
    
    if(webcam.greyscale)
    { 
      free(webcam.greyscale);
      webcam.greyscale = 0;
    } 
    
    if(webcam.delta_buffer)
    { 
      free(webcam.delta_buffer);
      webcam.delta_buffer = 0;
    } 
    
    if(webcam.back_buffer[0])
    { 
      free(webcam.back_buffer[0]);
      webcam.back_buffer[0] = 0;
    } 
    
    if(webcam.back_buffer[1])
    { 
      free(webcam.back_buffer[1]);
      webcam.back_buffer[1] = 0;
    } 
  } 
} 
