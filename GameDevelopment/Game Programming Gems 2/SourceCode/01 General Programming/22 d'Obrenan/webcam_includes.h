/* Copyright (C) Nathan d'Obrenan, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Nathan d'Obrenan, 2001"
 */
#ifndef __webcam_includes_h__
#define __webcam_includes_h__

#ifdef __cplusplus
extern "C" {
#endif

// INCLUDES//////////////////////////////////////////////////////////
// Remember to include <vfw.h> and link with <vfw32.lib> if you start a 
// new project!

// STRUCTS///////////////////////////////////////////////////////////
typedef enum
{ 
  COLOR,
  CARTOON,
  GREYSCALE
}webcam_draw_mode;

typedef struct cartoon_data_struct
{ 
  // For simplicities state I label the vars, you could just make it a big old 6x3 dim array
  unsigned char bottom[3], // The colors for the different levels
                minimum[3], 
                lowest[3],
                medium[3],
                highest[3],
                maximum[3];
}cartoon_data;

// WEBCAM DATA
typedef struct webcam_data_struct
{ 
  BOOL driver;              // (fs_bool) Do they have a webcam attached?
  BOOL use_webcam;          // (fs_bool) Do we even want to test for the webcam?
  
  unsigned int gl_bgr;      // (fs_uint) The color version of the webcam
  unsigned int gl_grey;     // (fs_uint) The greyscale texture object
  unsigned int gl_na;       // (fs_uint) Can't find the webcam, so we display this bitmap
  
  int width;                // (fs_int) Temporary storage var for the width (for the real one, call er_settings.wco_cam_width)
  int height;               // (fs_int) Temporary storage var for the height (for the real one, call er_settings.wco_cam_height)
  int size;                 // (fs_int) The size of the OpenGL texture
  
  unsigned char *bgr;       // (fs_uchar) The actual rgb picture from the camera
  unsigned char *greyscale; // (fs_uchar) Greyscale
  cartoon_data cartoon;     // (cartoon_data) Crazy webcamera cartoon mode
  
  unsigned char *delta_buffer;   // (fs_uchar) Contains the difference between frames
  unsigned char *back_buffer[2]; // (fs_uchar) Used to compare previous frames
  
  int which_buffer;
}webcam_data;

typedef struct webcam_settings_struct
{ 
  // WEBCAM SETTINGS
  BOOL wco_use_webcam;                // (fs_bool) If they have a webcam, use it!
  int wco_cam_width, wco_cam_height;  // (fs_int) Width of the web camera
  int wco_cam_rendering;              // (fs_int) What type of webcam rendering do we want to do
  int wco_cam_updates;                // (fs_int) The number of updates the camera does / milli second
  int wco_cam_threshold;              // (fs_int) The movement threshold
}webcam_settings;

// EXTERNS///////////////////////////////////////////////////////////
extern webcam_data webcam;
extern webcam_settings settings;
extern BOOL predator_vision;

// WEBCAM.C -- PROTOTYPES////////////////////////////////////////////
BOOL webcam_init(HWND hWnd, BOOL retry);
void webcam_set_vars(void);
LRESULT CALLBACK webcam_callback(HWND hwnd, LPVIDEOHDR video_hdr);
void webcam_make_texture(LPVIDEOHDR video, webcam_draw_mode mode);
void webcam_make_bgr(unsigned char *bgr_tex, unsigned char *vid_data, int webcam_width, int webcam_height, int glsize);
void webcam_make_greyscale(unsigned char *grey, unsigned char *color, int dim);
void webcam_init_cartoon(cartoon_data *cartoon_tex);
void webcam_make_cartoon(unsigned char *cartoon, cartoon_data cartoon_tex, unsigned char *data, int dim);
BOOL webcam_calc_movement(LPVIDEOHDR video_hdr, unsigned char *delta_buff, int webcam_width, int webcam_height, int gl_size, int thresh);
void webcam_destroy(void);

#ifdef __cplusplus
}
#endif

#endif