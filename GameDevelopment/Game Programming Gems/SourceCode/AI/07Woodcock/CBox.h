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
// Name:     CBox.h: 
// Purpose:  Class definitions and prototypes for the CBox class.
//*********************************************************************

#ifndef _CBOX_H
#define _CBOX_H

class CBox  
{

public:

   ///////////////////
   // static variables
   ///////////////////

   static CBox * WorldPtr;

   ///////////////////////////////
   // constructors and destructors
   ///////////////////////////////
   
   // Constructor #1.
   // Creates a Box with default values of 50 meters on any side 
   // not specified.

	CBox (float lv=50.0, float wv=50.0, float hv=50.0);

   // Destructor

   virtual ~CBox();

   ////////////////////
   // rendering methods
   ////////////////////

   void Draw (void);

   //////////////////////////
   // miscellaneous functions
   //////////////////////////

   // GetBoxLength.
   // Returns the length of the Box, in meters.

   float GetBoxLength (void);

   // GetBoxWidth..
   // Returns the width of the Box, in meters.

   float GetBoxWidth (void);

   // GetBoxHeight.
   // Returns the height of the Box, in meters.

   float GetBoxHeight (void);

private:

   float m_Length;
   float m_Width;
   float m_Height;

};

#endif