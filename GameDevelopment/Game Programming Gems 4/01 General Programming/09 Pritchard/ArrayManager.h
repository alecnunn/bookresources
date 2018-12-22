// ============================================================================
// ArrayManager.h
//
// provides the CArrayManager Dynamic Array Manager class
// ============================================================================


#ifndef _ARRAYMANAGER_H_
#define _ARRAYMANAGER_H_


class CArrayItem;


// ============================================================================
//
// The CArrayManager Class.
//
// This class manages a dynamic array of CArrayItem elements.  The number
// of items in the array is expected to grow and shrink over the course
// of the program's execution.  During these operations, the class may have
// to allocate additional memory to store the data.  The method used to
// choose how much memory to allocate is to double the previous allocation
// size.  While this minimizes the number of reallocatrions as the array
// grows, it does result in significant "slack space" (memory that is
// allocated, but unused).  It also results in the memory address as which
// the array data starts at to be moved around.

// The methods and memory variables in the class are deliberatly kept
// simple and stripped down to a minimum.  The purpose is to illustrate
// a most basic implementaion.  In practical use, a programmer will
// probably add additional functionality to the class.
//
// ============================================================================
class CArrayManager
{
   private:

      // This is the maximum number of elements that can be stored in
      // in the array.  This value grows as needed
      int            m_Maximum_Num_Elements;

      // This is the pointer to the actual array of CArrayItem elements
      CArrayItem*    m_Array_Data;

      // This is the number of CArrayItem elements that we actually
      // have in our array at the current moment, aka Current Size
      int            m_Num_Elements_Used;


   public:
      // Our constructors and destructor.  For convienence, we have a
      // constructor that combines the ::Initialize call into it.

                     CArrayManager(void);
                     CArrayManager(int Initial_Size);

                    ~CArrayManager(void);

      // Initialization and Deinitialization methods.  These set up the array
      // and clear it.  ResetVariables is a helper function.

      void           ResetVariables(void);

      void           Initialize (int Initial_Size);

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
