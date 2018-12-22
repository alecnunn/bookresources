//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITOR'S CUSTOM MESSAGES                                            //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_CUSTOMMSGS_H_
#define   _GPG4_CUSTOMMSGS_H_

//----------------------------------------------- DEFINES ------------------------------------------------------------//

enum 
  {
  UNKNOWN_MSG = 0,
  IDLE,

  // 3DView

  CHANGE_CURSOR,

  // CommandView

  // doc

  DOC_SELECTION_CHANGED,
  DOC_SELECTION_CLEARED,

  // LogReportView

  LOGREPORT_ADDTXT,
  LOGREPORT_CLEAR,

  // PropView

  PROP_CHANGED,                                             // props have changed
  PROP_UPDATE,                                              // update needed in PropView

  // SceneView

  SCENE_DBLCLK,
  SCENE_NEWOBJECT,                                          // new object created
  SCENE_SELECT,                                             // select object in scene tree
  SCENE_SELECTION,                                          // item selected in scene tree

  // tools

  TOOL_CHANGED,
  TOOL_NONE,
  TOOL_MOVECAMERA,
  TOOL_MOVESELECTION,
  };

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_CUSTOMMSGS_H_
