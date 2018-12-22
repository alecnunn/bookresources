//--------------------------------------------------------------------------------------------------------------------//
//                                                PROPERTIES AS RTTI EXTRA DATA                                       //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_EXTRAPROP_H_
#define   _GPG4_EXTRAPROP_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "ExtraData.h"
#include  "PropList.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                CExtraProp                                                          //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CExtraProp : public CExtraData
  {
  DECLARE_RTTI;

  // typedefs

  public:

    typedef bool            (*pfnInitList)        (void);

  // public methods

  public:

                              CExtraProp          (pfnInitList pfnInit);
    virtual                  ~CExtraProp          (void);

    CPropList*                GetPropList         (void);

  // protected data

  protected:

    CPropList                 m_PropList;
  };
}

//----------------------------------------------- MACROS -------------------------------------------------------------//

// put this macro in the .h of the class that defines its properties

#define   DECLARE_PROPERTIES(ClassName,ExtraClass)                                             \
  public:                                                                                      \
    static  ExtraClass  m_Prop;                                                                \
    static  CPropList*        GetPropList         (void)    { return m_Prop.GetPropList(); }   \
  private:                                                                                     \
    static  bool              DefineProperties    (void);

// put this macro in the .cpp of the class that defines its properties

#define  IMPLEMENT_PROPERTIES(ClassName,ExtraClass)  \
  ExtraClass ClassName::m_Prop(ClassName::DefineProperties);

// use this macro in DefineProperties

#define  REGISTER_PROP(Type,Class,Var,Name,Flags,Help)  \
  GetPropList()->RegisterProp(CProperties::Type,Name,offsetof(Class,Var),Flags,Help);

//----------------------------------------------- INLINES ------------------------------------------------------------//

#include  "ExtraProp.inl"

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_EXTRAPROP_H_
