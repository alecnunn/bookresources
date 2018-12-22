/* Copyright (C) Steven Woodcock, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steven Woodcock, 2001"
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
#include "glut.h"
#include "mtxlib.h"

//
// flocking debug globals
//

extern bool gDrawAxes;
extern bool gDrawPreyLine;
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
   myprintf("\nCFlock constructor called for %d\n",FlockCount);
#endif

   // initialize internals

   m_id               = FlockCount;

   m_num_members      = 0;
   m_num_live_members = 0;
   m_num_dead_members = 0;

   m_first_member     = NULL;

   ListOfFlocks[FlockCount] = this;

   // increment counter

   FlockCount++;

}

// Destructor.

CFlock::~CFlock (void)
{

#ifdef FLOCK_DEBUG
   myprintf("\nCFlock destructor called for %d\n",m_id);
#endif

   // clear values

   m_id = m_num_members = m_num_live_members = m_num_dead_members = 0;

   m_first_member = NULL;

   // remove it from the list

   ListOfFlocks[FlockCount] = NULL;

   // decrement counter

   FlockCount--;

}

/////////////////////
// flocking functions
/////////////////////

// Update.
// Updates all members of a flock.

void CFlock::Update (void)
{

   CBoid *ptr;

   // loop over all members of this flock, invoke
   // their flocking method, then draw them

   ptr = m_first_member;

   while (ptr != NULL) {

      // flock the boid

      ptr->FlockIt(m_id,m_first_member);

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

   vector *pos, *orient, *prey_vec;

   float scale;
   float color_r, color_g, color_b;

#ifdef DEBUG_FLOCK
   myprintf("now rendering boids in flock %d\n",m_id);
#endif

   // select this flock's color and scaling factor
   // NOTE:  Yes these are pretty much hardcoded for the purposes of the demo.

   switch (m_id) 
   {
      case 0:                             // yellow (Flies)
         color_r = 1.0f;
         color_g = 0.7f;
         color_b = 0.0f;
         scale   = 0.5f;
         break;
      case 1:                             // red (Sparrows)
         color_r = 1.0f;
         color_g = 0.0f;
         color_b = 0.0f;
         scale   = 1.0f;
         break;
      case 2:                             // blue (Hawks)
         color_r = 0.0f;
         color_g = 0.3f;
         color_b = 0.7f;
         scale   = 2.0f;
         break;
      default:                             // red (unspecified)
         color_r = 1.0f;
         color_g = 0.0f;
         color_b = 0.0f;
         scale   = 1.0f;
         break;
   }

   // loop over all members of this flock, extract
   // their position and orientation, and then draw them

   ptr = m_first_member;

   while (ptr != NULL) {

      // test:  is this boid alive?

      if (ptr->GetState()) {

         // yes--go ahead and draw him

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
               glutWireSphere (ptr->GetPerceptionRange(), 12, 12);
            }

            // draw keepaway distance spheres?

            if (gDrawKeepawayDist) {
               glColor3f (1.0f, 0.0f, 0.0f);
               glutWireSphere (ptr->GetKeepawayDistance(), 12, 12);
            }

            // draw separation distance spheres?

            if (gDrawSeparationDist) {
               glColor3f (color_r, color_g, color_b);
               glutWireSphere (ptr->GetPreferredSep(), 12, 12);
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

            // draw hunting indicator?

            if (((gDrawPreyLine) && (ptr->GetPreyPtr()) != NULL)) {

               prey_vec = ptr->GetPreyVector();

               glBegin(GL_LINES);

                  glColor3f(1.0f, 0.0f, 0.0f);
                  glVertex3f(0.0f, 0.0f, 0.0f);
                  // lengthen a bit so they're easier to see
                  glVertex3f(prey_vec->x * 2.0f, prey_vec->y * 2.0f, -prey_vec->z * 2.0f);

               glEnd();

            }

            // set this boid's color

            glColor3f(color_r, color_g, color_b);

            // draw the boid

            glBegin(GL_TRIANGLES);

               // back upper right

               glVertex3f( 0.0,  0.25*scale, 0.0);
               glVertex3f(-0.25*scale, 0.0,  0.0);
               glVertex3f( 0.0,  0.0,  0.0);

               // back lower right

               glVertex3f( 0.0, -0.25*scale, 0.0);
               glVertex3f(-0.25*scale, 0.0,  0.0);
               glVertex3f( 0.0,  0.0,  0.0);

               // back upper left

               glVertex3f(0.0,  0.25*scale, 0.0);
               glVertex3f(0.25*scale, 0.0,  0.0);
               glVertex3f(0.0,  0.0,  0.0);

               // back lower left

               glVertex3f(0.0, -0.25*scale, 0.0);
               glVertex3f(0.25*scale, 0.0,  0.0);
               glVertex3f(0.0,  0.0,  0.0);

               // make tops slightly darker

               glColor3f(color_r - 0.1f, color_g - 0.1f, color_b - 0.1f);

               // top left side

               glVertex3f(0.0,  0.25*scale, 0.0);
               glVertex3f(0.0,  0.0,  1.0*scale);
               glVertex3f(0.25*scale, 0.0,  0.0);

               // top right side

               glVertex3f( 0.0,  0.25*scale, 0.0);
               glVertex3f( 0.0,  0.0,  1.0*scale);
               glVertex3f(-0.25*scale, 0.0,  0.0);

               // make bottoms slightly lighter

               glColor3f(color_r + 0.1f, color_g + 0.1f, color_b + 0.1f);

               // bottom left side

               glVertex3f(0.0, -.25*scale, 0.0);
               glVertex3f(0.0,  0.0, 1.0*scale);
               glVertex3f(0.25*scale, 0.0, 0.0);

               // bottom right side

               glVertex3f( 0.0, -0.25*scale, 0.0);
               glVertex3f( 0.0,  0.0,  1.0*scale);
               glVertex3f(-0.25*scale, 0.0,  0.0);


            glEnd();

         // Pop the rotation matrix from the MODELVIEW stack

         glPopMatrix();

      }

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

   // increment live/dead count

   if (member->GetState()) {
      m_num_live_members++;
   } else {
      m_num_dead_members++;
   }

#ifdef FLOCK_DEBUG
   myprintf(" Num members total/live/dead = %d %d %d\n",
      m_num_members, m_num_live_members, m_num_dead_members);
#endif

}

// GetCount.
// Returns the # of members in a given flock.

int CFlock::GetCount (void)
{

   return (m_num_members);

}

// GetLiveCount.
// Returns the # of live boids in a given flock.

int CFlock::GetLiveCount (void)
{

   return (m_num_live_members);

}

// SetLiveCount.
// Increments the # of live boids in a given flock.

void CFlock::SetLiveCount (int inc_v)
{

   m_num_live_members += inc_v;

}

// GetDeadCount.
// Returns the # of dead boids in a given flock.

int CFlock::GetDeadCount (void)
{

   return (m_num_dead_members);

}

// SetDeadCount.
// Increments the # of dead boids in a given flock.

void CFlock::SetDeadCount (int inc_v)
{

   m_num_dead_members += inc_v;

}

// PrintData.
// Dumps all data describing a given flock.

void CFlock::PrintData (void)
{

   CBoid *ptr;

   myprintf("\n");
   myprintf("=== Flock # %d ===\n",m_id);

   // print the flock data

   myprintf("Number of members = %d\n", m_num_members);
   myprintf("Number of live members = %d\n",m_num_live_members);
   myprintf("Number of dead members = %d\n",m_num_dead_members);
   
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

      // decrement live/dead count

      if (member->GetState()) {
         m_num_live_members--;
      } else {
         m_num_dead_members--;
      }

#ifdef FLOCK_DEBUG
      myprintf(" Num members total/live/dead = %d %d %d\n",
         m_num_members, m_num_live_members, m_num_dead_members);
#endif

   } else {

#ifdef FLOCK_DEBUG
      myprintf("not found\n");
#endif

   }

#ifdef FLOCK_DEBUG
   myprintf("first member is now = %x\n",m_first_member);
#endif

}