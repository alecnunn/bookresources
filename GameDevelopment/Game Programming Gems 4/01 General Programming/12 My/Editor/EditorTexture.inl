//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITOR TEXTURE CLASS                                                //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_EDITORTEXTURE_INL_
#define   _GPG4_EDITORTEXTURE_INL_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "ImgMenu.h"

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CEditorTexture::CEditorTexture()
  {}

INLINE CEditorTexture::~CEditorTexture()
  {}

//--------------------------------------------------------------------------------------------------------------------//
//                                                GET/SET                                                             //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CStdString CEditorTexture::GetDefaultName() const
  {
  return "texture";
  }

INLINE u32 CEditorTexture::GetIcon() const
  {
  return CImgMenu::GetImgID("_IMG_TEXTURE_");
  }

INLINE u32 CEditorTexture::GetMenu() const
  {
  return CImgMenu::GetMenuID("_MENU_TEXTURE_");
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_EDITORTEXTURE_INL_
