//--------------------------------------------------------------------------------------------------------------------//
//                                                DX8 MESH CLASS                                                      //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_MESHDX8_H_
#define   _GPG4_MESHDX8_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#define   D3D_OVERLOADS
#include  <d3d8.h>

#include  "EngineMesh.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                CMeshDX8                                                            //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CMeshDX8 : public CEngineMesh
  {
  DECLARE_RTTI;

  // public methods

  public:

    // constructors & destructor

                              CMeshDX8            (const LPDIRECT3DDEVICE8 pDevice);
    virtual                  ~CMeshDX8            (void);

    // management

    virtual CEngineMesh*      Clone               (IRenderer* pRenderer);

  // protected data

  protected:

    LPDIRECT3DDEVICE8         m_pDevice;
  };

  // smart ptr

  SMART_POINTER(CMeshDX8);
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "MeshDX8.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_MESHDX8_H_
