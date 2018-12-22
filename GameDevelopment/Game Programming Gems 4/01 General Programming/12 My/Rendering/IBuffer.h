//--------------------------------------------------------------------------------------------------------------------//
//                                                INDEX BUFFER CLASS                                                  //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_IBUFFER_H_
#define   _GPG4_IBUFFER_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "Persistent.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                CIBuffer                                                            //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CIBuffer : public CRefCount
  {
  DECLARE_RTTI;

  // public methods

  public:

    // constructors & destructor

                              CIBuffer            (void);
    virtual                  ~CIBuffer            (void);

    // get

    u32                       GetCount            (void) const;
    u32                       GetActiveCount      (void) const;
    bool                      IsStatic            (void) const;
    bool                      IsDynamic           (void) const;
    bool                      HasRAMCopy          (void) const;

    u32                       SetActiveCount      (const u32 u32Count);

    // management

    virtual u32               Create              (u16* pBuffer,const u32 u32Count,const bool boStatic=true,const bool boRAMCopy=false);
    virtual u32               CreateCopy          (u16* pBuffer,const u32 u32Count,const bool boStatic=true,const bool boRAMCopy=false);
    virtual u32               Destroy             (void);

    virtual u32               Write               (u16* pBuffer,const u32 u32Count,const u32 u32Pos=0);
    virtual u32               Read                (u16* pBuffer,const u32 u32Count,const u32 u32Pos=0);

    u32                       WriteToRAM          (u16* pBuffer,const u32 u32Count,const u32 u32Pos=0);
    virtual u32               UpdateFromRAM       (             const u32 u32Count,const u32 u32Pos=0);

  // protected data

  protected:

    u16*                      m_pBuffer;
    u32                       m_u32Count;
    u32                       m_u32ActiveCount;

    bool                      m_boStatic;
    bool                      m_boRAMCopy;

  // friends (allowed to access m_pBuffer directly for READING)

    friend class              CBVMesh;
  };

  // smart ptr

  SMART_POINTER(CIBuffer);
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "IBuffer.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_IBUFFER_H_
