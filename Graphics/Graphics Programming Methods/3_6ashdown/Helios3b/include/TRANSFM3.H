////////////////////////////////////////////////////////////
//
//  TRANSFM3.H - 3-D Linear Transformation Class
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/12/16 - Version 1.01A release.
//              95/02/05 - Version 1.02A release.
//              95/07/21 - Version 1.02B release.
//              96/02/14 - Version 1.02C release.
//              96/04/01 - Version 1.03A release.
//              97/04/01 - Version 1.03B release.
//
//  Compilers:  Microsoft Visual C/C++ Professional V5.0
//              Borland C++ Version 5.0
//
//  Author:     Ian Ashdown, P.Eng.
//              byHeart Consultants Limited
//              620 Ballantree Road
//              West Vancouver, B.C.
//              Canada V7S 1W3
//              Tel. (604) 922-6148
//              Fax. (604) 987-7621
//
//  Copyright 1994-2003 byHeart Consultants Limited
//
//  The following source code has been derived from:
//
//    Ashdown, I. 1994. Radiosity: A Programmer's
//    Perspective. New York, NY: John Wiley & Sons.
//
//    (Available on CD-ROM from www.helios32.com.)
//
//  It may be freely copied, redistributed, and/or modified
//  for personal use ONLY, as long as the copyright notice
//  is included with all source code files.
//
////////////////////////////////////////////////////////////

#ifndef _3D_TRANS_H
#define _3D_TRANS_H

#include "vector3.h"

class Transform3        // 3-D linear transformation
{
  private:
    double scale_x;     // x-axis scaling factor
    double scale_y;     // y-axis scaling factor
    double scale_z;     // z-axis scaling factor
    double trans_x;     // x-axis translation distance
    double trans_y;     // y-axis translation distance
    double trans_z;     // z-axis translation distance
    double rot_x;       // x-axis rotation (in radians)
    double rot_y;       // y-axis rotation (in radians)
    double rot_z;       // z-axis rotation (in radians)
    double m[3][4];     // Transformation matrix

    void Identity()     // Initialize identity matrix
    {
      int i, j;         // Loop indices

      for (i = 0; i < 3; i++)
        for (j = 0; j < 4; j++)
        {
          if (i == j)
            m[i][j] = 1.0;
          else
            m[i][j] = 0.0;
        }
    }

    // Note: s_val is sine of rotation angle
    //       c_val is cosine of rotation angle

    // Rotate counterclockwise about x-axis
    void RotateX( double s_val, double c_val )
    {
      int i;        // Loop index
      double temp;  // Temporary variable

      for (i = 0; i < 4; i++)
      {
        temp = m[1][i] * c_val - m[2][i] * s_val;
        m[2][i] = m[1][i] * s_val + m[2][i] * c_val;
        m[1][i] = temp;
      }
    }

    // Rotate counterclockwise about y-axis
    void RotateY( double s_val, double c_val )
    {
      int i;        // Loop index
      double temp;  // Temporary variable

      for (i = 0; i < 4; i++)
      {
        temp = m[0][i] * c_val + m[2][i] * s_val;
        m[2][i] = -m[0][i] * s_val + m[2][i] * c_val;
        m[0][i] = temp;
      }
    }

    // Rotate counterclockwise about z-axis
    void RotateZ( double s_val, double c_val )
    {
      int i;        // Loop index
      double temp;  // Temporary variable

      for (i = 0; i < 4; i++)
      {
        temp = m[0][i] * c_val - m[1][i] * s_val;
        m[1][i] = m[0][i] * s_val + m[1][i] * c_val;
        m[0][i] = temp;
      }
    }

    void Scale()
    {
      m[0][0] *= scale_x;
      m[1][1] *= scale_y;
      m[2][2] *= scale_z;
    }

    void Translate()
    {
      m[0][3] += trans_x;
      m[1][3] += trans_y;
      m[2][3] += trans_z;
    }

   public:
    Transform3()
    {
      scale_x = scale_y = scale_z = 1.0;
      trans_x = trans_y = trans_z = 0.0;
      rot_x = rot_y = rot_z = 0.0;

      Identity();
    }

    // Set scaling factors
    void SetScale( double sx, double sy, double sz )
    { scale_x = sx; scale_y = sy; scale_z = sz; }

    // Set translation distances
    void SetTranslation( double tx, double ty, double tz )
    { trans_x = tx; trans_y = ty; trans_z = tz; }

    // Set rotation angles
    void SetRotation( double rx, double ry, double rz )
    { rot_x = rx; rot_y = ry; rot_z = rz; }

    void BuildTransform()
    {
      Identity();       // Initialize identity matrix

      Scale();          // Concatenate scale matrix

      // Concatenate rotation matrices
      RotateX(sin(rot_x), cos(rot_x));
      RotateY(sin(rot_y), cos(rot_y));
      RotateZ(sin(rot_z), cos(rot_z));

      Translate();      // Concatenate translation matrix
    }

    // Premultiply point by 3-D transformation matrix
    void Transform( Point3 *pp )
    {
      Point3 temp;      // Temporary 3-D point

      temp.SetX(m[0][0] * pp->GetX() + m[0][1] * pp->GetY()
          + m[0][2] * pp->GetZ() + m[0][3]);
      temp.SetY(m[1][0] * pp->GetX() + m[1][1] * pp->GetY()
          + m[1][2] * pp->GetZ() + m[1][3]);
      temp.SetZ(m[2][0] * pp->GetX() + m[2][1] * pp->GetY()
          + m[2][2] * pp->GetZ() + m[2][3]);

      pp->SetX(temp.GetX());
      pp->SetY(temp.GetY());
      pp->SetZ(temp.GetZ());
    }
};

#endif

