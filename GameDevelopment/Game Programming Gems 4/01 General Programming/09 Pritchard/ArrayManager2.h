// ============================================================================
// ArrayManager2.h
//
// provides the CAddressSpaceArrayManager Dynamic Array Manager class
// ============================================================================


#ifndef _ARRAYMANAGER2_H_
#define _ARRAYMANAGER2_H_


class CArrayItem;


// ============================================================================
//
//  Global Values we need
//
// ============================================================================

const int OS_Page_Size = 4096;      // Size of a Win32 Memory Page in Bytes


// ============================================================================
//
// The CAddressSpaceArrayManager Class.
//
// This class manages a dynamic array of CArrayItem elements.  The number
// of items in the array is expected to grow and shrink over the course
// of the program's execution.  During these operations, this class will
// dynamically commit and decommit physical memory so that there is just
// enough memory in use to meet the current need.  This will result in
// "slack space" being kept to a bare minimum during the run, even if the
// size of the array data gets very very large.  It will also keep the array
// data starting at the same address in memory the whole time.
//
// The methods and memory variables in the class are deliberatly kept
// simple and stripped down to a minimum.  The purpose is to show how the
// address space management process works.  For use in your applications
// you would probably want to add additional functionality and convert
// this class to a template (well, for those who like templates anyway).
//
// The approach this class takes is to commit only the minimuj number of
// memory pages needed to hold the array data.  This results in pages be
// committed and decomitted one at a time.  While relativly quick, some
// situations may be better off if several pages are committed or decommitted
// at one time.   The choice of management strategy for this is left to
// the user who takes this demonstration class and expands upon it.
//
// ============================================================================
class CAddressSpaceArrayManager
{
   private:

      // This is the size, in bytes, of one CArrayItem element
      int            m_Element_Size;

      // This is the maximum number of elements that can be stored in
      // in the array.  This value is supplied by the program
      int            m_Maximum_Num_Elements;

      // This is the maximum number of 4K memory pages we will need.
      // i.e. how many pages used if the array is at maximum size.
      int            m_Maximum_Pages_Commitable;

      // This is the pointer to the actual array of CArrayItem elements
      CArrayItem*    m_Array_Data;

      // This is the number of CArrayItem elements that we actually
      // have in our array at the current moment, aka Current Size
      int            m_Num_Elements_Used;

      // This is the number of memory pages we have committed at the
      // current moment.  It will be at least enough to hold
      // m_Num_Elements_Used items, but could be more
      int            m_Num_Pages_Committed;

      // This is how many CArrayItem elements we can hold in the array
      // at the current moment without having to commit additional
      // memory pages.
      int            m_Num_Elements_Committable;


   public:
      // Our constructors and destructor.  For convienence, we have a
      // constructor that combines the ::Initialize call into it.

                     CAddressSpaceArrayManager(void);
                     CAddressSpaceArrayManager(int Maximum_Size, int Initial_Size);

                    ~CAddressSpaceArrayManager(void);

      // Initialization and Deinitialization methods.  These set up the array
      // and clear it.  ResetVariables is a helper function.

      void           ResetVariables(void);

      void           Initialize (int Maximum_Size, int Initial_Size);

      void           Deinitialize(void);

      // Element manipulation methods. These let us add and remove and
      // access individual items in the array

      void           AddItem (CArrayItem* the_Element);
      void           AddItems(CArrayItem* the_Elements, int num_items_to_add);

      CArrayItem*    GetItem(int the_Element_index);

      void           RemoveItem (int the_Element_Index);
      void           ClearAllItems(void);

      // Information Methods - return information about the array

      int            GetArraySize(void)         {return m_Num_Elements_Used;}


};


// ============================================================================

#endif
