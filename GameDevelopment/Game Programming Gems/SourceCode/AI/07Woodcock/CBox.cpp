/* Copyright (C) Steven Woodcock, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steven Woodcock, 2000"
 */
//*********************************************************************
// Name:     CBox.cpp: 
// Purpose:  Class methods for the CBox class.
//*********************************************************************

//
// includes
//

#include "CBox.h"
#include "util.h"
#include "glut.h"

//
// flocking debug globals
//

extern bool gDrawAxes;

//
// static variable initialization
//

CBox * CBox::WorldPtr = NULL;

///////////////////////////
// Construction/Destruction
///////////////////////////

CBox::CBox (float lv, float wv, float hv)
{

#ifdef BOX_DEBUG
   myprintf("\nInside Box Contructor\n");
   myprintf("Length = %f Width = %f Height = %f\n",lv, wv, hv);
#endif

   m_Length = lv;
   m_Width  = wv;
   m_Height = hv;

   WorldPtr = this;

}

CBox::~CBox ()
{

#ifdef BOX_DEBUG
   myprintf("\nInside CBox Destructor\n");
#endif

}

////////////////////
// rendering methods
////////////////////

void CBox::Draw (void)
{

   glPushMatrix();

      glColor3f(1.0f, 0.0f, 0.0f);
      glutWireCube (m_Height);

      // draw world XYZ axes?

      if (gDrawAxes) {

         glBegin(GL_LINES);

            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(5.0f, 0.0f, 0.0f);

            glColor3f(0.0f, 1.0f, 0.0f); 
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, 5.0f, 0.0f);
   
            glColor3f(0.0f, 0.0f, 1.0f); 
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, 0.0f, 5.0f);

         glEnd();
      }

   glPopMatrix();

}

//////////////////////////
// miscellaneous functions
//////////////////////////

// GetBoxLength.
// Returns the length of the Box, in meters.

float CBox::GetBoxLength (void)
{

   return (m_Length);

}

// GetBoxWidth..
// Returns the width of the Box, in meters.

float CBox::GetBoxWidth (void)
{

   return (m_Width);

}

// GetBoxHeight.
// Returns the height of the Box, in meters.

float CBox::GetBoxHeight (void)
{

   return (m_Height);

}