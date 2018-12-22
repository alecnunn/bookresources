/* Copyright (C) Steve Rabin, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steve Rabin, 2000"
 */
//*********************************************************************
// Name:     CFlock.cpp
// Purpose:  Class definitions and method prototypes for the flocks
//           themselves.
//*********************************************************************

//
// includes
//

#include "CFlock.h" 
#include "GL/glut.h"
#include "mtxlib.h"
#include "util.h"

//
// flocking debug globals
//

extern bool gDrawAxes;
extern bool gDrawPerceptionDist;
extern bool gDrawKeepawayDist;
extern bool gDrawSeparationDist;

//
// static variable initialization
//

int CFlock::FlockCount = 0;

CFlock * CFlock::ListOfFlocks[] = {NULL};

//
// constructor and destructor methods
//

// Constructor.
// Creates a new flock.

CFlock::CFlock (void)
{

#ifdef FLOCK_DEBUG
   myprintf("\nCFlock contructor called for %d\n",FlockCount);
#endif

   // initialize internals

   m_id           = FlockCount;

   m_num_members  = 0;

   m_first_member = NULL;

   ListOfFlocks[FlockCount] = this;

   // increment counter

   FlockCount++;

   return;

}

// Destructor.

CFlock::~CFlock (void)
{

#ifdef FLOCK_DEBUG
   myprintf("\nCFlock destructor called for %d\n",m_id);
#endif

   // clear values

   m_id = m_num_members = 0;

   m_first_member = NULL;

   // remove it from the list

   ListOfFlocks[FlockCount] = NULL;

   // decrement counter

   FlockCount--;

   return;

}

/////////////////////
// flocking functions
/////////////////////

// Update.
// Updates all members of a flock.

void CFlock::Update (float deltaTime)
{

   CBoid *ptr;

   // loop over all members of this flock, invoke
   // their flocking method, then draw them

   ptr = m_first_member;

   while (ptr != NULL) {

      // flock the boid

      ptr->FlockIt(m_id,m_first_member,deltaTime);

      // get next boid

      ptr = ptr->GetNext();
   }
}

//////////////////////
// rendering functions
//////////////////////

void CFlock::Draw (void)
{

   CBoid *ptr;

   vector *pos, *orient;

   float color_r, color_g, color_b;

#ifdef DEBUG_FLOCK
   myprintf("now rendering boids in flock %d\n",m_id);
#endif

   // select this flock's color

   switch (m_id) 
   {
      case 0:                             // red 
         color_r = 1.0f;
         color_g = 0.1f;
         color_b = 0.0f;
         break;
      case 1:                             // orange
         color_r = 1.0f;
         color_g = 0.5f;
         color_b = 0.0f;
         break;
      case 2:                             // magenta
         color_r = 1.0f;
         color_g = 0.0f;
         color_b = 1.0f;
         break;
      case 3:                             // blue 
         color_r = 0.0f;
         color_g = 0.0f;
         color_b = 1.0f;
         break;
      case 4:                             // green 
         color_r = 0.0f;
         color_g = 1.0f;
         color_b = 0.0f;
         break;
      default:                             // black 
         color_r = 0.0f;
         color_g = 0.0f;
         color_b = 0.0f;
         break;
   }

   // loop over all members of this flock, extract
   // their position and orientation, and then draw them

   ptr = m_first_member;

   while (ptr != NULL) {

      pos = ptr->GetPos();
      orient = ptr->GetOrient();

      // push our world matrix down

      glPushMatrix();

         // translate to where this boid is

         glTranslatef(pos->x, pos->y, pos->z);

         // rotate to the boid's frame

         glRotatef(RadToDeg(orient->z), 0.0, 0.0, 1.0);   // roll
         glRotatef(RadToDeg(orient->y), 0.0, 1.0, 0.0);   // yaw
         glRotatef(RadToDeg(orient->x), 1.0, 0.0, 0.0);   // pitch

         // test for any debug options

         // draw perception distance spheres?

         if (gDrawPerceptionDist) {
            glColor3f (1.0f, 1.0f, 1.0f);
            glutWireSphere (Default_Perception_Range, 12, 12);
         }

         // draw keepaway distance spheres?

         if (gDrawKeepawayDist) {
            glColor3f (1.0f, 0.0f, 0.0f);
            glutWireSphere (KeepAwayDist, 12, 12);
         }

         // draw separation distance spheres?

         if (gDrawSeparationDist) {
            glColor3f (color_r, color_g, color_b);
            glutWireSphere (SeparationDist, 12, 12);
         }

         // draw local (boid space) XYZ axes?

         if (gDrawAxes) {

            glBegin(GL_LINES);

               glColor3f(1.0f, 0.0f, 0.0f);
               glVertex3f(0.0f, 0.0f, 0.0f);
               glVertex3f(2.0f, 0.0f, 0.0f);

               glColor3f(0.0f, 1.0f, 0.0f); 
               glVertex3f(0.0f, 0.0f, 0.0f);
               glVertex3f(0.0f, 2.0f, 0.0f);

               glColor3f(0.0f, 0.0f, 1.0f); 
               glVertex3f(0.0f, 0.0f, 0.0f);
               glVertex3f(0.0f, 0.0f, 2.0f);

            glEnd();
         }

         // set this boid's color

         glColor3f(color_r, color_g, color_b);

         // draw the boid

         glBegin(GL_TRIANGLES);

            // back upper right

            glVertex3f( 0.0,  0.25, 0.0);
            glVertex3f(-0.25, 0.0,  0.0);
            glVertex3f( 0.0,  0.0,  0.0);

            // back lower right

            glVertex3f( 0.0, -0.25, 0.0);
            glVertex3f(-0.25, 0.0,  0.0);
            glVertex3f( 0.0,  0.0,  0.0);

            // back upper left

            glVertex3f(0.0,  0.25, 0.0);
            glVertex3f(0.25, 0.0,  0.0);
            glVertex3f(0.0,  0.0,  0.0);

            // back lower left

            glVertex3f(0.0, -0.25, 0.0);
            glVertex3f(0.25, 0.0,  0.0);
            glVertex3f(0.0,  0.0,  0.0);

            // make tops slightly darker

            glColor3f(color_r - 0.1f, color_g - 0.1f, color_b - 0.1f);

            // top left side

            glVertex3f(0.0,  0.25, 0.0);
            glVertex3f(0.0,  0.0,  1.0);
            glVertex3f(0.25, 0.0,  0.0);

            // top right side

            glVertex3f( 0.0,  0.25, 0.0);
            glVertex3f( 0.0,  0.0,  1.0);
            glVertex3f(-0.25, 0.0,  0.0);

            // make bottoms slightly lighter

            glColor3f(color_r + 0.1f, color_g + 0.1f, color_b + 0.1f);

            // bottom left side

            glVertex3f(0.0, -.25, 0.0);
            glVertex3f(0.0,  0.0, 1.0);
            glVertex3f(0.25, 0.0, 0.0);

            // bottom right side

            glVertex3f( 0.0, -0.25, 0.0);
            glVertex3f( 0.0,  0.0,  1.0);
            glVertex3f(-0.25, 0.0,  0.0);


         glEnd();

         // Pop the rotation matrix from the MODELVIEW stack

         glPopMatrix();

      // get next boid

      ptr = ptr->GetNext();
   }
}

//////////////////////////
// miscellaneous functions
//////////////////////////

// AddTo.
// Adds the indicated member to the flock.

void CFlock::AddTo (CBoid * member)
{

   CBoid *ptr;

#ifdef FLOCK_DEBUG
   myprintf("adding %x to flock %d\n",member,m_id);
#endif

   // if this is the first member added to this
   // flock, store him in the first_member slot

   if (!m_first_member) {
      
      m_first_member = member;

      member->SetNext(NULL);
      member->SetPrev(NULL);

   } else {
   
      // not the first member...follow the list
      // of members and add this guy to the end

      ptr = m_first_member;

      while (ptr->GetNext() != NULL) ptr = ptr->GetNext();
         
      // we should now be at the end...add our new guy

      ptr->SetNext(member);
      member->SetPrev(ptr);

   }

   // increment # of members in this flock

   m_num_members++;

}

// GetCount.
// Returns the # of members in a given flock.

int CFlock::GetCount (void)
{

   return (m_num_members);

}

// PrintData.
// Dumps all data describing a given flock.

void CFlock::PrintData (void)
{

   CBoid *ptr;

#ifdef FLOCK_DEBUG
   myprintf("\n");
   myprintf("=== Flock # %d ===\n",m_id);

   // print the flock data

   myprintf("Number of members = %d\n", m_num_members);
#endif
   
   // print data on each member

   ptr = m_first_member;

   while (ptr != NULL) {

      ptr->PrintData();

      ptr = ptr->GetNext();

   }
}

// GetFirstMember.
// Returns a pointer to the first member of a given flock (if any).

CBoid * CFlock::GetFirstMember (void)
{

   // return pointer to first member of flock

   return (m_first_member);

}

// RemoveFrom.
// Removes the indicated member from the flock.

void CFlock::RemoveFrom (CBoid * member)
{

   CBoid *ptr = m_first_member;

#ifdef FLOCK_DEBUG
   myprintf("\ntrying to remove %x\n",member);
#endif

   // search for the indicated member

   while (ptr != NULL)
   {

#ifdef FLOCK_DEBUG
      myprintf("searching, ptr = %x\n",ptr);
#endif

      // test:  find it yet?

      if (ptr == member) break;

      ptr = ptr->GetNext();

   }

   // if we found it, remove it from the list

   if (ptr) {

#ifdef FLOCK_DEBUG
      myprintf("removing...\n");
#endif

      // found it...see if we have to tweak first member

      if (ptr == m_first_member) {

         if (m_num_members == 1) {
            m_first_member = NULL;
         } else {
            m_first_member = ptr->GetNext();
         }
      }

      // now delink it

      ptr->LinkOut();

      // cleanup

      ptr->SetNext(NULL);
      ptr->SetPrev(NULL);

      // decrement counter

      m_num_members--;

   } else {

#ifdef FLOCK_DEBUG
      myprintf("not found\n");
#endif

   }

#ifdef FLOCK_DEBUG
   myprintf("first member is now = %x\n",m_first_member);
#endif

}
