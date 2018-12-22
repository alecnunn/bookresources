//--------------------------------------------------------------------------------------------------------------------//
//                                                ENGINE MATERIAL CLASS                                               //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_ENGINEMATERIAL_H_
#define   _GPG4_ENGINEMATERIAL_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "EngineObj.h"
#include  "EngineTexture.h"
#include  "VectorMatrix.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                CEngineMaterial                                                     //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CEngineMaterial : public CEngineObj
  {
  DECLARE_RTTI;
  DECLARE_PROPERTIES(CEngineMaterial,CExtraProp);

  // public enums

  public:

    typedef enum
      {
      NOP                     = 0,
      MODULATE                = 1,
      } TEXTURE_OP;

    enum                    { NbSlots = 2 };                // max textures

  // public methods

  public:

    // constructors & destructor

                              CEngineMaterial     (void);
    virtual                  ~CEngineMaterial     (void);

    // get/set

    virtual CStdString        GetEditName         (void) const;

    CVect4D                   GetDiffuse          (void) const;
    void                      SetDiffuse          (const CVect4D& v4Color);

    bool                      IsLighted           (void) const;
    void                      EnableLighting      (const bool boLighting);
    bool                      IsZEnabled          (void) const;
    void                      EnableZTest         (const bool boZTest);
    bool                      IsCulled            (void) const;
    void                      EnableCulling       (const bool boCull);

    CEngineTexture*           GetTexture          (const u32 u32Slot=0) const;
    bool                      SetTexture          (CEngineTexture* pTex,const u32 u32Slot=0);
    TEXTURE_OP                GetTexOp            (const u32 u32Slot=0) const;
    bool                      SetTexOp            (TEXTURE_OP Op,const u32 u32Slot=0);

  // protected data

  protected:

    CVect4D                   m_v4Diffuse;
    bool                      m_boLighted;
    bool                      m_boZTest;
    bool                      m_boCulling;

    CEngineTextureSP          m_spTex[NbSlots];
    TEXTURE_OP                m_TexOp[NbSlots];
  };

  // smart ptr

  SMART_POINTER(CEngineMaterial);
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "EngineMaterial.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_ENGINEMATERIAL_H_
