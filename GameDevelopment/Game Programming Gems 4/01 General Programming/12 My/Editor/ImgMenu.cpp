//--------------------------------------------------------------------------------------------------------------------//
//                                                IMAGES & MENUS ENUMS                                                //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "ImgMenu.h"

#include  "ImgEnum.h"
#include  "MenuEnum.h"

#include  "StdString.h"

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//

u32 CImgMenu::GetMenuID(const char* pszItem)
  {
  CStdString strItem(pszItem);

  _CMP_ID_(_MENU_FOLDER_);
  _CMP_ID_(_MENU_SEPARATOR_);
  _CMP_ID_(_MENU_CONSOLE_);
  _CMP_ID_(_MENU_LOG_);

  _CMP_ID_(_MENU_CAMERA_);
  _CMP_ID_(_MENU_AMBIENT_);
  _CMP_ID_(_MENU_DIREC_);
  _CMP_ID_(_MENU_OMNI_);
  _CMP_ID_(_MENU_SPOT_);
  _CMP_ID_(_MENU_SOUND_);
  _CMP_ID_(_MENU_MESH_);
  _CMP_ID_(_MENU_BOX_);
  _CMP_ID_(_MENU_TEXTURE_);
  _CMP_ID_(_MENU_BHV_);
  _CMP_ID_(_MENU_CHR_);
  _CMP_ID_(_MENU_ANIM_);
  _CMP_ID_(_MENU_NODE_);

  _CMP_ID_(_MENU_GROUP_);
  _CMP_ID_(_MENU_RECYCLE_);
  _CMP_ID_(_MENU_GRID_);
  _CMP_ID_(_MENU_SPHERE_);
  _CMP_ID_(_MENU_FSM_);
  _CMP_ID_(_MENU_ACTOR_);
  _CMP_ID_(_MENU_SENSOR_);
  _CMP_ID_(_MENU_CONDITION_);
  _CMP_ID_(_MENU_TRANSITION_);
  _CMP_ID_(_MENU_STATE_);

  _CMP_ID_(_MENU_MATERIAL_);
  _CMP_ID_(_MENU_RECTANGLE_);
  _CMP_ID_(_MENU_TEXT_);
  _CMP_ID_(_MENU_TERRAIN_);
  _CMP_ID_(_MENU_COUNTER_);

  return(_MENU_NONE_);
  }

//

u32 CImgMenu::GetImgID(const char* pszItem)
  {
  CStdString strItem(pszItem);

  _CMP_ID_(_IMG_EMPTY_);
  _CMP_ID_(_IMG_FOLDER_);
  _CMP_ID_(_IMG_OPEN_FOLDER_);

  _CMP_ID_(_IMG_CAMERA_);
  _CMP_ID_(_IMG_AMBIENT_);
  _CMP_ID_(_IMG_DIREC_);
  _CMP_ID_(_IMG_OMNI_);
  _CMP_ID_(_IMG_SPOT_);
  _CMP_ID_(_IMG_SOUND_);
  _CMP_ID_(_IMG_MESH_);
  _CMP_ID_(_IMG_BOX_);
  _CMP_ID_(_IMG_TEXTURE_);
  _CMP_ID_(_IMG_BHV_);
  _CMP_ID_(_IMG_CHR_);
  _CMP_ID_(_IMG_ANIM_);
  _CMP_ID_(_IMG_NODE_);

  _CMP_ID_(_IMG_CONSOLE_);
  _CMP_ID_(_IMG_LOG_);
  _CMP_ID_(_IMG_GROUP_);
  _CMP_ID_(_IMG_RECYCLE_);
  _CMP_ID_(_IMG_GRID_);
  _CMP_ID_(_IMG_SPHERE_);
  _CMP_ID_(_IMG_FSM_);
  _CMP_ID_(_IMG_ACTOR_);
  _CMP_ID_(_IMG_SENSOR_);
  _CMP_ID_(_IMG_CONDITION_);
  _CMP_ID_(_IMG_TRANSITION_);
  _CMP_ID_(_IMG_STATE_);

  _CMP_ID_(_IMG_MATERIAL_);
  _CMP_ID_(_IMG_RECTANGLE_);
  _CMP_ID_(_IMG_TEXT_);
  _CMP_ID_(_IMG_TERRAIN_);
  _CMP_ID_(_IMG_COUNTER_);

  return(_IMG_EMPTY_);
  }

//--------------------------------------------------------------------------------------------------------------------//
 
} // namespace
