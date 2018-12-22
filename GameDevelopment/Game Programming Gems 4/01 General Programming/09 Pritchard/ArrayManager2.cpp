#include "arraymanager2.h"
#include "arraymanagerdefs.h"

#include <windows.h>
#include <assert.h>



// ============================================================================
// CAddressSpaceArrayManager constuctor (no arguments)
// ============================================================================

CAddressSpaceArrayManager::CAddressSpaceArrayManager(void)
{
   // This just initializes all the class's member variables
   // which we could also do in the constructor list

   ResetVariables();

}


// ============================================================================
// CAddressSpaceArrayManager constuctor w/ built in Initialize call
// ============================================================================

CAddressSpaceArrayManager::CAddressSpaceArrayManager(int Maximum_Size, int Initial_Size)
{

   ResetVariables();

   // Combine the Intialize Call into the constructor

   Initialize(Maximum_Size, Initial_Size);

}


// ============================================================================
// CAddressSpaceArrayManager destructor
// ============================================================================

CAddressSpaceArrayManager::~CAddressSpaceArrayManager(void)
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
void  CAddressSpaceArrayManager::ResetVariables(void)
{
   // We clear all the member variables to a known state

   m_Element_Size             = 0;
   m_Maximum_Num_Elements     = 0;
   m_Maximum_Pages_Commitable = 0;
   m_Array_Data               = NULL;
   m_Num_Elements_Used        = 0;
   m_Num_Pages_Committed      = 0;
   m_Num_Elements_Committable = 0;

}


// ============================================================================
// Initialize - this is where we reserver our range of address space
//   and set up memory to begin holding array data.  We probably want to
//   Add checks to make sure we don't call Initialize twice without
//   first calling Deinitialize, at least in our debug builds
// ============================================================================

void CAddressSpaceArrayManager::Initialize(int Maximum_Size, int Initial_Size)
{
   // Initialize the Array to the specified capacity
   // (This could be moved into the constructor)
   m_Element_Size = sizeof (CArrayItem);


   // Note: We could round this up to next page size
   m_Maximum_Num_Elements = Maximum_Size;


   // Calculate number of pages we want to reserve
   m_Maximum_Pages_Commitable =
     (( m_Element_Size * m_Maximum_Num_Elements )
        + OS_Page_Size - 1) / OS_Page_Size;

   // Reserve address space for this array instance
   m_Array_Data = (CArrayItem*) VirtualAlloc(NULL,
       m_Element_Size * m_Maximum_Num_Elements,
       MEM_RESERVE, PAGE_READWRITE);

   if (!m_Array_Data)
       assert(!"Address allocation failed");

   // Commit Memory for an initial # of array elements

   // Calculate amout of memory to commit
   m_Num_Pages_Committed =
      ((Initial_Size * m_Element_Size)
         + OS_Page_Size - 1)/ OS_Page_Size;

   // Round up commitable array element count
   // to the next page size
   m_Num_Elements_Committable =
      (m_Num_Pages_Committed * OS_Page_Size) /
       m_Element_Size;

   // Commit the memory
   void* result = VirtualAlloc(m_Array_Data,
         m_Num_Pages_Committed * OS_Page_Size,
         MEM_COMMIT, PAGE_READWRITE);

   if (!result)
       assert(!"Memory commit failed");

   m_Num_Elements_Used = 0; // No elements added yet
}


// ============================================================================
// Deinitalize - this is where we release the memory we're using for
//     the array, and release the address range we had reserved
// ============================================================================

void CAddressSpaceArrayManager::Deinitialize(void)
{
   // First we tell the OS that we don't need the memory behind
   // all the pages we had committed.

   VirtualFree(m_Array_Data, m_Num_Pages_Committed * OS_Page_Size, MEM_DECOMMIT);

   // Now, we release the range of address space we had reserved

   VirtualFree(m_Array_Data, m_Maximum_Pages_Commitable * OS_Page_Size, MEM_RELEASE);

   // And we clear our member variables

   ResetVariables();

}


// ============================================================================
// AddItem - this copys a loose CArrayItem onto the end of the array
// ============================================================================

void CAddressSpaceArrayManager::AddItem(CArrayItem* the_Element)
{
  if (m_Num_Elements_Used >= m_Maximum_Num_Elements)
  {
     // Must decide what to do here
     assert(!"Array exceeded maximum size");
  }

  // Committed memory Full?
  if (m_Num_Elements_Used==m_Num_Elements_Committable)
  {
     // Add more committed to memory to
     // the Array's address range

     // Many ways we could calculate this...
     int  Num_Pages_To_Grow = 1;

     void* result = VirtualAlloc(
         (byte*)m_Array_Data +
          m_Num_Pages_Committed * OS_Page_Size,
          Num_Pages_To_Grow * OS_Page_Size,
          MEM_COMMIT, PAGE_READWRITE);

     assert (result != NULL);

     m_Num_Pages_Committed = m_Num_Pages_Committed +
                             Num_Pages_To_Grow;

     // Update number of elements we have memory for
     m_Num_Elements_Committable =
         (m_Num_Pages_Committed * OS_Page_Size)
           / m_Element_Size;
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

void CAddressSpaceArrayManager::AddItems(CArrayItem* the_Element, int num_items_to_add)
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
CArrayItem* CAddressSpaceArrayManager::GetItem(int the_Element_index)
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

void CAddressSpaceArrayManager::RemoveItem (int the_Element_Index)
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

   // See if we want to release any committed pages
   int Unused_Memory = (m_Num_Elements_Committable -
        m_Num_Elements_Used) * OS_Page_Size;
   if (Unused_Memory > OS_Page_Size)
   {
      // Free the last page of committed memory
      m_Num_Pages_Committed--;
      VirtualFree((byte*)m_Array_Data +
              m_Num_Pages_Committed * OS_Page_Size,
              OS_Page_Size, MEM_DECOMMIT);

      // Recalculate the number of items we
      // have committed memory for
      m_Num_Elements_Committable =
              (m_Num_Pages_Committed * OS_Page_Size) /
              m_Element_Size;
   }

}


// ============================================================================
// ClearAllItems - this empties out the array data, but does not deinitialize
//   the array.  Typically this is used when the array is used to gather of
//   a list of things to be processed - and once processed they are discared
//   en mass.    This method shows another example of using the Win32 API
//   to manage address space
// ============================================================================

void CAddressSpaceArrayManager::ClearAllItems(void)
{

   // First we tell the OS that we don't need the memory behind
   // all the pages we had committed.  We're dumping it all

   VirtualFree(m_Array_Data, m_Num_Pages_Committed * OS_Page_Size, MEM_DECOMMIT);

   // We reset our variable so that the next time AddItem is called,
   // A memory page will be automatically committed to hold the data.

   m_Num_Elements_Used        = 0;
   m_Num_Pages_Committed      = 0;
   m_Num_Elements_Committable = 0;

}
