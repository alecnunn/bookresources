//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITOR MATERIAL CLASS                                               //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_EDITORMATERIAL_INL_
#define   _GPG4_EDITORMATERIAL_INL_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "ImgMenu.h"

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CEditorMaterial::CEditorMaterial()
  {}

INLINE CEditorMaterial::~CEditorMaterial()
  {}

//--------------------------------------------------------------------------------------------------------------------//
//                                                GET/SET                                                             //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CStdString CEditorMaterial::GetDefaultName() const
  {
  return "material";
  }

INLINE u32 CEditorMaterial::GetIcon() const
  {
  return CImgMenu::GetImgID("_IMG_MATERIAL_");
  }

INLINE u32 CEditorMaterial::GetMenu() const
  {
  return CImgMenu::GetMenuID("_MENU_MATERIAL_");
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_EDITORMATERIAL_INL_
