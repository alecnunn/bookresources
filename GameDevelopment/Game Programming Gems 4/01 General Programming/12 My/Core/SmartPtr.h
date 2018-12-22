//--------------------------------------------------------------------------------------------------------------------//
//                                                SMART POINTER TEMPLATE                                              //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//                                                                                                                    //
// smart pointer to T : T must inherit from CRefCount                                                                 //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_SMARTPTR_H_
#define   _GPG4_SMARTPTR_H_

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                TSmartPtr                                                           //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  template <class CObj> class TSmartPtr
  {
  // public methods

  public:
                    TSmartPtr           (CObj* pObj=NULL);
                    TSmartPtr           (const TSmartPtr& Smart);
                   ~TSmartPtr           (void);

    // operators

    TSmartPtr&      operator =          (const TSmartPtr& Smart);
    TSmartPtr&      operator =          (CObj* pObj);

    CObj*           operator ->         (void) const;
    CObj&           operator *          (void) const;
    operator        CObj*               (void) const;

    bool            operator ==         (CObj* pObj) const;
    bool            operator !=         (CObj* pObj) const;
    bool            operator ==         (const TSmartPtr& Smart) const;
    bool            operator !=         (const TSmartPtr& Smart) const;

    // dumb pointer

    CObj*           GetDumb             (void) const;

  // protected data

  protected:

    CObj*           m_pDumbPtr;                             // pointed object
  };
}

//----------------------------------------------- MACROS -------------------------------------------------------------//

// SMART_POINTER(Class); => defines the ClassSP type (smart ptr to an object of class 'Class')

#define   SMART_POINTER(Class)    \
  class   Class;                  \
  typedef GPG4::TSmartPtr<Class> Class##SP;

//----------------------------------------------- INLINES ------------------------------------------------------------//

#include  "SmartPtr.inl"

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_SMARTPTR_H_
