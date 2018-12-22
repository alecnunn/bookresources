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
// CAMERA.C
/////////////////////////////////////////////////////////////////////
#include "main_includes.h"

/////////////////////////////////////////////////////////////////////
// GLOBALS
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// CAM_INIT
/////////////////////////////////////////////////////////////////////
fs_void cam_init(camera *cam)
{ 
  vector3f init = { 0.5f, 18.0f, 49.0f };
  
  matrix44_init(&cam->info);
  
  cam->info.mat[3][0] = init.vec[0];
  cam->info.mat[3][1] = init.vec[1];
  cam->info.mat[3][2] = init.vec[2];
  
  cam->view_pos.vec[0] = init.vec[0];
  cam->view_pos.vec[1] = init.vec[1];
  cam->view_pos.vec[2] = init.vec[2];
  
  MAKEVECTOR3(cam->targ_pos.vec, 0.0f, 0.0f, 0.0f);
  MAKEVECTOR3(cam->motion.vec, 0.0f, 0.0f, 0.0f);
  
	matrix44_yrot(M_PI, &cam->info);
} 

/////////////////////////////////////////////////////////////////////
// CAM_UPDATE
/////////////////////////////////////////////////////////////////////
fs_void cam_update(camera *cam, fs_char move_type, fs_float amount)
{ 
  if(move_type <= HELICAM_SPEED)
		cam->motion.vec[move_type] += amount;
  
  switch(move_type)
	{ 
    case HELICAM_PITCH:
			matrix44_xrot(amount, &cam->info);
		break;
    
		case HELICAM_HEADING:
  		matrix44_yrot(amount, &cam->info);
		break;
		
    case HELICAM_ROLL:
			matrix44_zrot(amount, &cam->info);
		break;
		
    case HELICAM_STOP:
			MAKEVECTOR3(cam->motion.vec, 0.0f, 0.0f, 0.0f);
    break;
    
    case HELICAM_180:
	    matrix44_yrot(M_PI, &cam->info);
    break;
  };
} 

/////////////////////////////////////////////////////////////////////
// CAM_POSITION
// Actually display/position the user
/////////////////////////////////////////////////////////////////////
fs_void cam_position(camera *cam)
{ 
  vector4_init(&cam->view_pos);
  vector4_init(&cam->targ_pos);
  vector4_init(&cam->up_pos);
  
  cam->targ_pos.vec[2] = 1.0f;
  cam->up_pos.vec[1] = 1.0;
  
  matrix44_mult_vector4(&cam->info, &cam->view_pos);
  matrix44_mult_vector4(&cam->info, &cam->targ_pos);
  matrix44_mult_vector4(&cam->info, &cam->up_pos);
  
  cam->up_pos.vec[0] -= cam->view_pos.vec[0];
  cam->up_pos.vec[1] -= cam->view_pos.vec[1];
  cam->up_pos.vec[2] -= cam->view_pos.vec[2];
  
	gluLookAt(cam->view_pos.vec[0], cam->view_pos.vec[1],  cam->view_pos.vec[2],
	          cam->targ_pos.vec[0], cam->targ_pos.vec[1],  cam->targ_pos.vec[2],
            cam->up_pos.vec[0],   cam->up_pos.vec[1],    cam->up_pos.vec[2]);
} 

/////////////////////////////////////////////////////////////////////
// CAM_MOVE
// Multiply vectors together to achieve acceleration
/////////////////////////////////////////////////////////////////////
fs_void cam_move(camera *cam)
{ 
  fs_int i;
  vector4f v;
  
  vector4_init(&v);
  
  v.vec[0] = cam->motion.vec[0];
  v.vec[1] = cam->motion.vec[1];
  v.vec[2] = cam->motion.vec[2];
  
  matrix44_mult_vector4(&cam->info, &v);
  
  for(i=0; i<3; i++)
      cam->info.mat[3][i] = v.vec[i];
} 