//--------------------------------------------------------------------------------------------------------------------//
//                                                REFERENCE COUNTING CLASS                                            //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//                                                                                                                    //
// most of the time this class is going to be used with the TSmartPtr template                                        //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_REFCOUNT_H_
#define   _GPG4_REFCOUNT_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "BasicTypes.h"
#include  "Defines.h"
#include  "RTTI.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                CRefCount                                                           //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CRefCount
  {
  DECLARE_ROOT_RTTI;

  // public methods

  public:

    // counting

    void            AddRef              (void);
    void            Release             (void);
    u32             GetRefCount         (void) const;

  // protected methods

  protected:

    // constructors & destructor

                    CRefCount           (void);
                    CRefCount           (const CRefCount& RefCount);
    virtual        ~CRefCount           (void);

    // operators

    CRefCount&      operator =          (const CRefCount& RefCount);

  // private data

  private:

    u32             m_u32RefCount;
  };
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "RefCount.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_REFCOUNT_H_
