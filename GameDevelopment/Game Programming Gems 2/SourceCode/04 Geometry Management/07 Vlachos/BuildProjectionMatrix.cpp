/* Copyright (C) Alex Vlachos and Evan Hart, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Alex Vlachos and Evan Hart, 2001"
 */
const float GPG_PI = 3.14159265f;
inline float GpgDegToRad(float D) { return ((D) * (GPG_PI/180.0f)); }
void GpgPerspective (double fovy, double aspect, double Near, double Far, int subrect)
{
   double fov2, left, right, bottom, top;
   
   fov2 = GpgDegToRad(fovy) * 0.5;
   
   top = Near/(cos(fov2)/sin(fov2));
   bottom = -top;
   right = top*aspect;
   left = -right;

   if (subrect == -1) //Regular full screen
      GpgFrustum (left, right, bottom, top, Near, Far);
   else if (subrect == 0) //UL
      GpgFrustum(left, left/3.0, top/3.0, top, Near, Far);
   else if (subrect == 1) //UC
      GpgFrustum(left/3.0, right/3.0, top/3.0, top, Near, Far);
   else if (subrect == 2) //UR
      GpgFrustum(right/3.0, right, top/3.0, top, Near, Far);
   else if (subrect == 3) //ML
      GpgFrustum(left, left/3.0, bottom/3.0, top/3.0, Near, Far);
   else if (subrect == 4) //MC
      GpgFrustum(left/3.0, right/3.0, bottom/3.0, top/3.0, Near, Far);
   else if (subrect == 5) //MR
      GpgFrustum(right/3.0, right, bottom/3.0, top/3.0, Near, Far);
   else if (subrect == 6) //BL
      GpgFrustum(left, left/3.0, bottom, bottom/3.0, Near, Far);
   else if (subrect == 7) //BC
      GpgFrustum(left/3.0, right/3.0, bottom, bottom/3.0, Near, Far);
   else if (subrect == 8) //BR
      GpgFrustum(right/3.0, right, bottom, bottom/3.0, Near, Far);
}

void GpgFrustum (double left, double right, double bottom, double top, double zNear, double zFar)
{
   float matrix[16] = { 1.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 1.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 1.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 1.0f };

#ifdef GPG_OPENGL_API
   matrix[0]  = (float)(2.0*zNear/(right-left));
   matrix[5]  = (float)(2.0*zNear/(top-bottom));

   matrix[8]  = (float)((right+left)/(right-left));
   matrix[9]  = (float)((top+bottom)/(top-bottom));
   matrix[10] = (float)(-(zFar+zNear)/(zFar-zNear));
   matrix[11] = (float)(-1.0);

   matrix[14] = (float)(-(2.0*zFar*zNear)/(zFar-zNear));
#else //Direct3D
   matrix[0]  = (float)(2.0*zNear/(right-left));
   matrix[5]  = (float)(2.0*zNear/(top-bottom));

   matrix[8]  = (float)((right+left)/(right-left));
   matrix[9]  = (float)((top+bottom)/(top-bottom));
   matrix[10] = (float)(-(zFar)/(zFar-zNear));
   matrix[11] = (float)(-1.0);

   matrix[14] = (float32)(-(zFar*zNear)/(zFar-zNear));
#endif

   //Now set this matrix as the current projection matrix
}  
