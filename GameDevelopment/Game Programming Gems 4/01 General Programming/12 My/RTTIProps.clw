; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CDlgItemName
LastTemplate=CFormView
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "RTTIProps.h"
LastPage=0

ClassCount=13
Class1=CRTTIPropsApp
Class2=CRTTIPropsDoc
Class3=CRTTIPropsView
Class4=CMainFrame
Class7=C3DView

ResourceCount=19
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDR_RTTIPRTYPE
Resource4=IDD_COMMANDVIEW
Resource5=IDD_SCENEVIEW
Resource6=IDR_RTTIPRTYPE (English (U.S.))
Resource7=IDD_PROPVIEW
Resource8=IDD_CHOOSEMAT
Resource9=IDD_ITEMNAME
Resource10=IDR_CONTEXT_SCENE
Resource11=IDD_EDITSPHERE
Resource12=IDD_LOGREPORT
Resource13=IDR_CONTEXT_3DCOMMANDS
Resource14=IDR_MAINFRAME (English (U.S.))
Class5=CChildFrame
Class6=CAboutDlg
Class8=CCommandView
Class9=CDlgItemName
Class10=CLogReportView
Class11=CPropView
Class12=CDlgRotation
Class13=CSceneView
Resource15=IDD_ROTATION
Resource16=IDD_ABOUTBOX (English (U.S.))
Resource17=IDD_CHOOSETEX
Resource18=IDD_EDITCAMERA
Resource19=IDD_EDITBOX

[CLS:CRTTIPropsApp]
Type=0
HeaderFile=RTTIProps.h
ImplementationFile=RTTIProps.cpp
Filter=N

[CLS:CRTTIPropsDoc]
Type=0
HeaderFile=RTTIPropsDoc.h
ImplementationFile=RTTIPropsDoc.cpp
Filter=N

[CLS:CRTTIPropsView]
Type=0
HeaderFile=RTTIPropsView.h
ImplementationFile=RTTIPropsView.cpp
Filter=C


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T


[CLS:CChildFrame]
Type=0
HeaderFile=ChildFrm.h
ImplementationFile=ChildFrm.cpp
Filter=M


[CLS:CAboutDlg]
Type=0
HeaderFile=RTTIProps.cpp
ImplementationFile=RTTIProps.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Class=CAboutDlg

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command4=ID_APP_EXIT
Command5=ID_VIEW_STATUS_BAR
CommandCount=6

CommandCount=6
Command3=ID_FILE_MRU_FILE1
Command6=ID_APP_ABOUT

[MNU:IDR_RTTIPRTYPE]
Type=1
Class=CRTTIPropsView
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_VIEW_STATUS_BAR
Command13=ID_WINDOW_NEW
Command14=ID_WINDOW_CASCADE
CommandCount=17
Command6=ID_FILE_MRU_FILE1
Command7=ID_APP_EXIT
Command8=ID_EDIT_UNDO
Command15=ID_WINDOW_TILE_HORZ
Command16=ID_WINDOW_ARRANGE
Command17=ID_APP_ABOUT

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_NEXT_PANE
CommandCount=13
Command4=ID_EDIT_UNDO
Command13=ID_PREV_PANE


[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_MRU_FILE1
Command4=ID_APP_EXIT
Command5=ID_VIEW_STATUS_BAR
Command6=ID_APP_ABOUT
CommandCount=6

[MNU:IDR_RTTIPRTYPE (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_FILE_MRU_FILE1
Command7=ID_APP_EXIT
Command8=ID_VIEW_STATUS_BAR
Command9=ID_WINDOW_NEW
Command10=ID_WINDOW_CASCADE
Command11=ID_WINDOW_TILE_HORZ
Command12=ID_WINDOW_ARRANGE
Command13=ID_APP_ABOUT
CommandCount=13

[ACL:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_NEXT_PANE
Command13=ID_PREV_PANE
CommandCount=13

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=CAboutDlg
ControlCount=7
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308353
Control6=IDC_CODEPROJECT,button,1342242816
Control7=IDC_FAIRYENGINE,button,1342242816

[CLS:C3DView]
Type=0
HeaderFile=3DView.h
ImplementationFile=3DView.cpp
BaseClass=CView
Filter=C
LastObject=C3DView

[DLG:IDD_COMMANDVIEW]
Type=1
Class=CCommandView
ControlCount=6
Control1=IDC_CLEARSEL,button,1342242816
Control2=IDC_MOVESEL,button,1342246915
Control3=IDC_READONLYPROPS,button,1342246915
Control4=IDC_MOVECAM,button,1342246915
Control5=IDC_STATIC,static,1342308352
Control6=IDC_GRAPH,SysTreeView32,1350635559

[DLG:IDD_ITEMNAME]
Type=1
Class=CDlgItemName
ControlCount=3
Control1=IDC_NAME,edit,1350631552
Control2=IDOK,button,1342242817
Control3=IDCANCEL,button,1342242816

[DLG:IDD_LOGREPORT]
Type=1
Class=CLogReportView
ControlCount=1
Control1=IDC_REPORT,SysListView32,1350631433

[DLG:IDD_PROPVIEW]
Type=1
Class=CPropView
ControlCount=2
Control1=IDC_TAB,SysTabControl32,1342177280
Control2=IDC_NAME,static,1342308354

[DLG:IDD_ROTATION]
Type=1
Class=CDlgRotation
ControlCount=9
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=65535,button,1342177287
Control4=65535,static,1342308352
Control5=65535,static,1342308352
Control6=65535,static,1342308352
Control7=IDC_EULERX,edit,1350631552
Control8=IDC_EULERY,edit,1350631552
Control9=IDC_EULERZ,edit,1350631552

[DLG:IDD_SCENEVIEW]
Type=1
Class=CSceneView
ControlCount=1
Control1=IDC_TREE,SysTreeView32,1350635811

[CLS:CCommandView]
Type=0
HeaderFile=CommandView.h
ImplementationFile=CommandView.cpp
BaseClass=CFormView
Filter=D
LastObject=CCommandView
VirtualFilter=VWC

[CLS:CDlgItemName]
Type=0
HeaderFile=Dialogs\DlgItemName.h
ImplementationFile=Dialogs\DlgItemName.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_NAME
VirtualFilter=dWC

[CLS:CLogReportView]
Type=0
HeaderFile=LogReportView.h
ImplementationFile=LogReportView.cpp
BaseClass=CFormView
Filter=D
LastObject=CLogReportView

[CLS:CPropView]
Type=0
HeaderFile=PropView.h
ImplementationFile=PropView.cpp
BaseClass=CFormView
Filter=D
LastObject=CPropView

[CLS:CDlgRotation]
Type=0
HeaderFile=Dialogs\DlgRotation.h
ImplementationFile=Dialogs\DlgRotation.cpp
BaseClass=CDialog
Filter=D
LastObject=65535

[CLS:CSceneView]
Type=0
HeaderFile=SceneView.h
ImplementationFile=SceneView.cpp
BaseClass=CFormView
Filter=D
LastObject=CSceneView

[DLG:IDD_CHOOSETEX]
Type=1
Class=?
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_TEXLIST,SysListView32,1350647837
Control4=IDC_NOTEX,button,1342242817

[DLG:IDD_CHOOSEMAT]
Type=1
Class=?
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_MATLIST,SysListView32,1350647837
Control4=IDC_NOMAT,button,1342242817

[DLG:IDD_EDITSPHERE]
Type=1
Class=?
ControlCount=17
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,button,1342177287
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_POSX,edit,1350631552
Control8=IDC_POSY,edit,1350631552
Control9=IDC_POSZ,edit,1350631552
Control10=IDC_NEWMAT,button,1342242819
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_RADIUS,edit,1350631552
Control14=IDC_LOD,edit,1350631552
Control15=IDC_STATIC,static,1342308352
Control16=IDC_CHOOSEMAT,button,1342242816
Control17=IDC_MATNAME,static,1342308352

[DLG:IDD_EDITCAMERA]
Type=1
Class=?
ControlCount=25
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,button,1342177287
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_POSX,edit,1350631552
Control8=IDC_POSY,edit,1350631552
Control9=IDC_POSZ,edit,1350631552
Control10=IDC_STATIC,button,1342177287
Control11=IDC_DIR0,button,1342308361
Control12=IDC_DIR1,button,1342177289
Control13=IDC_DIR2,button,1342177289
Control14=IDC_DIR3,button,1342177289
Control15=IDC_DIR4,button,1342177289
Control16=IDC_DIR5,button,1342177289
Control17=IDC_STATIC,static,1342308352
Control18=IDC_STATIC,static,1342308352
Control19=IDC_STATIC,static,1342308352
Control20=IDC_FOV,edit,1350631552
Control21=IDC_NEAR,edit,1350631552
Control22=IDC_FAR,edit,1350631552
Control23=IDC_STATIC,static,1342308352
Control24=IDC_STATIC,static,1342308352
Control25=IDC_STATIC,static,1342308352

[DLG:IDD_EDITBOX]
Type=1
Class=?
ControlCount=28
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,button,1342177287
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_DIMX,edit,1350631552
Control8=IDC_DIMY,edit,1350631552
Control9=IDC_DIMZ,edit,1350631552
Control10=IDC_STATIC,button,1342177287
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC,static,1342308352
Control14=IDC_POSX,edit,1350631552
Control15=IDC_POSY,edit,1350631552
Control16=IDC_POSZ,edit,1350631552
Control17=IDC_POSXMIN,button,1342308361
Control18=IDC_POSXMED,button,1342177289
Control19=IDC_POSXMAX,button,1342177289
Control20=IDC_POSYMIN,button,1342308361
Control21=IDC_POSYMED,button,1342177289
Control22=IDC_POSYMAX,button,1342177289
Control23=IDC_POSZMIN,button,1342308361
Control24=IDC_POSZMED,button,1342177289
Control25=IDC_POSZMAX,button,1342177289
Control26=IDC_NEWMAT,button,1342242819
Control27=IDC_CHOOSEMAT,button,1342242816
Control28=IDC_MATNAME,static,1342308352

[MNU:IDR_CONTEXT_SCENE]
Type=1
Class=?
Command1=ID_CREATE_FOLDER
Command2=ID_CREATE_BOX
Command3=ID_CREATE_CAMERA
Command4=ID_CREATE_MATERIAL
Command5=ID_CREATE_NODE
Command6=ID_CREATE_SPHERE
Command7=ID_CREATE_TEXTURE
Command8=ID_MENU_RENAME
Command9=ID_MENU_RENAME
Command10=ID_MENU_RENAME
Command11=ID_MENU_RENAME
Command12=ID_MENU_EDIT
Command13=ID_CAMERA_PERSPECTIVE
Command14=ID_MENU_RENAME
Command15=ID_MENU_RENAME
Command16=ID_MENU_RENAME
Command17=ID_MENU_RENAME
Command18=ID_MENU_RENAME
Command19=ID_MENU_RENAME
Command20=ID_MESH_SAVE
Command21=ID_MENU_RENAME
Command22=ID_MENU_EDIT
Command23=ID_MENU_RENAME
Command24=ID_MENU_RENAME
Command25=ID_MENU_RENAME
Command26=ID_MENU_RENAME
Command27=ID_MENU_RENAME
Command28=ID_MENU_RENAME
Command29=ID_MENU_RENAME
Command30=ID_RECYCLE_EMPTY
Command31=ID_GRID_CHANGESTEPS
Command32=ID_MENU_EDIT
Command33=ID_MENU_RENAME
Command34=ID_MENU_RENAME
Command35=ID_MENU_RENAME
Command36=ID_MENU_RENAME
Command37=ID_MENU_RENAME
Command38=ID_MENU_RENAME
Command39=ID_MENU_RENAME
Command40=ID_MENU_RENAME
Command41=ID_MENU_EDIT
Command42=ID_MENU_RENAME
Command43=ID_MENU_EDIT
Command44=ID_MENU_RENAME
Command45=ID_MENU_RENAME
Command46=ID_MENU_RENAME
Command47=ID_MENU_RENAME
CommandCount=47

[MNU:IDR_CONTEXT_3DCOMMANDS]
Type=1
Class=?
Command1=ID_3D_MOVECAMERA
Command2=ID_3D_PERSPECTIVE
Command3=ID_3D_MOVESEL
CommandCount=3

