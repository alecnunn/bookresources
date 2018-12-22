#include "arraymanager.h"
#include "arraymanagerdefs.h"

#include <stdlib.h>
#include <memory.h>
#include <assert.h>



// ============================================================================
// CArrayManager constuctor (no arguments)
// ============================================================================

CArrayManager::CArrayManager(void)
{
   // This just initializes all the class's member variables
   // which we could also do in the constructor list

   ResetVariables();

}


// ============================================================================
// CArrayManager constuctor w/ built in Initialize call
// ============================================================================

CArrayManager::CArrayManager(int Initial_Size)
{

   ResetVariables();

   // Combine the Intialize Call into the constructor

   Initialize(Initial_Size);

}


// ============================================================================
// CArrayManager destructor
// ============================================================================

CArrayManager::~CArrayManager(void)
{
   // Free up any memory and release any address ranges we have reserved

   Deinitialize();

   // Not necessary to clear our variables on destruction, but I like
   // to do it in case someone still has a pointer to this object.
   // In that case, if they try to access it after this, they'll
   // get a NULL pointer violation

   ResetVariables();

}


// ============================================================================
// ResetVariables is a helper method to avoid adding the same code to
//     several methods
// ============================================================================
void  CArrayManager::ResetVariables(void)
{
   // We clear all the member variables to a known state

   m_Maximum_Num_Elements     = 0;
   m_Array_Data               = NULL;
   m_Num_Elements_Used        = 0;

}


// ============================================================================
// Initialize - this is where we initialize our array storage.
//   We probably want to add checks to make sure we don't call
//   Initialize twice without first calling Deinitialize, at
//   least in our debug builds
// ============================================================================

void CArrayManager::Initialize(int Initial_Size)
{

   // This is how many elements we can hold
   m_Maximum_Num_Elements = Initial_Size;

   // Allocate our initial array

   m_Array_Data = new CArrayItem[Initial_Size];

   // None of our elements are used yet

   m_Num_Elements_Used = 0;
}


// ============================================================================
// Deinitalize - this is where we release the memory we're using for
//     the array, and release the address range we had reserved
// ============================================================================

void CArrayManager::Deinitialize(void)
{
   // Clear our array

   delete[]  m_Array_Data;

   // And we clear our member variables

   ResetVariables();

}


// ============================================================================
// AddItem - this copys a loose CArrayItem onto the end of the array
// ============================================================================

void CArrayManager::AddItem(CArrayItem* the_Element)
{
  if (m_Num_Elements_Used >= m_Maximum_Num_Elements)
  {
     // We need to expand the array now

     // Compute new size, by doubling capacity
     int new_array_size = m_Maximum_Num_Elements * 2;

     // Allocate the new buffer
     CArrayItem* new_Array_Data = new CArrayItem[new_array_size];

     // copy the data over

     memcpy (new_Array_Data, m_Array_Data, sizeof(CArrayItem) *  m_Num_Elements_Used);

     // delete the old array

     delete[] m_Array_Data;

     // and update our array variables

     m_Array_Data =  new_Array_Data;
     m_Maximum_Num_Elements = new_array_size;

     // and continue on

  }

  // Add element to the Array
  m_Array_Data[m_Num_Elements_Used] = *the_Element;

  // Update number of elements in use
  m_Num_Elements_Used++;
}


// ============================================================================
// AddItems - a helper function to add mutiple items to the array in
//       a single call.
// ============================================================================

void CArrayManager::AddItems(CArrayItem* the_Element, int num_items_to_add)
{
   for (int n = 0; n < num_items_to_add; n++)
   {
      AddItem(the_Element + n);
   }
}


// ============================================================================
// GetItem - returns a pointer to a specific item in the array
//       a single call.
// ============================================================================
CArrayItem* CArrayManager::GetItem(int the_Element_index)
{
   if ( the_Element_index < 0 || the_Element_index >= m_Num_Elements_Used)
   {
      assert(!"GetItem - index out of bounds");
   }

   return (&m_Array_Data[the_Element_index]);
}


// ============================================================================
// RemoveItem - This deletes an item from the array.  The strategy to keep
//   the array data contigious is to copy the last item into the position
//   formerly held by the deleted item.  This does result in the order of
//   items getting changed.  To avoid this, we could changed the method
//   copy each item down one position.   The approach here was chosen
//   simply for speed and simplicity, as this is just a demonstration class.
// ============================================================================


void CArrayManager::RemoveItem (int the_Element_Index)
{
   if (the_Element_Index < 0 ||
       the_Element_Index >= m_Num_Elements_Used)
   {
      assert(!"Bad Array Element Index");
   }

   // Remove requested item, and fill in gap
   // with the last array element
   m_Num_Elements_Used--;

   if (the_Element_Index < m_Num_Elements_Used)
   {
      m_Array_Data[the_Element_Index] =
         m_Array_Data[m_Num_Elements_Used];
   }

}


// ============================================================================
// ClearAllItems - this empties out the array data, but does not deinitialize
//   the array.  In this class, we just set the number of elements to 0.
//   This leaves the memory allocated.. which could be undesirable
// ============================================================================

void CArrayManager::ClearAllItems(void)
{

   // We have nothing usable in the array now...
   
   m_Num_Elements_Used        = 0;

}
