//--------------------------------------------------------------------------------------------------------------------//
//                                                ENGINE CLASS                                                        //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_ENGINE_H_
#define   _GPG4_ENGINE_H_
#pragma    once

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "Persistent.h"
#include  "Renderer.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                CEngine                                                             //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CEngine : public CPersistent
  {
  // public methods

  public:

    // destructor

    virtual                  ~CEngine             (void);

    // get/set

    static CEngine*           GetEngine           (void);   // singleton
    IRenderer*                GetRenderer         (void) const;

    // renderer

    bool                      CreateRenderer      (const HWND hWnd,const u32 u32Flags,const u32 u32Width=0,const u32 u32Height=0);
    void                      ReleaseRenderer     (void);

  // private methods

  private:
                              CEngine             (void);

  // protected data

  public:

    IRendererSP               m_spRenderer;
  };

  // smart ptr

  SMART_POINTER(CEngine);
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_ENGINE_H_
