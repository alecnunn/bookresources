; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=TestFSMDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "GameGems.h"
LastPage=0

ClassCount=5
Class1=CGameGemsApp
Class3=CMainFrame
Class4=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Class2=CChildView
Class5=TestFSMDlg
Resource3=IDD_TESTFSMDLG

[CLS:CGameGemsApp]
Type=0
HeaderFile=GameGems.h
ImplementationFile=GameGems.cpp
Filter=N

[CLS:CChildView]
Type=0
HeaderFile=ChildView.h
ImplementationFile=ChildView.cpp
Filter=W
BaseClass=CWnd 
VirtualFilter=WC
LastObject=IDOK

[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=IDM_TESTFSM




[CLS:CAboutDlg]
Type=0
HeaderFile=GameGems.cpp
ImplementationFile=GameGems.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=IDM_TESTFSM
Command2=ID_VIEW_STATUS_BAR
Command3=ID_APP_ABOUT
Command4=ID_APP_EXIT
CommandCount=4

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_EDIT_COPY
Command2=ID_EDIT_PASTE
Command3=ID_EDIT_UNDO
Command4=ID_EDIT_CUT
Command5=ID_NEXT_PANE
Command6=ID_PREV_PANE
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_CUT
Command10=ID_EDIT_UNDO
CommandCount=10

[DLG:IDD_TESTFSMDLG]
Type=1
Class=TestFSMDlg
ControlCount=8
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_CURRENTSTATE,edit,1350633600
Control5=IDC_STATIC,static,1342308352
Control6=IDC_OUTPUTSTATE,edit,1350633600
Control7=IDC_INPUTSCOMBO,combobox,1344339970
Control8=IDC_STATIC,static,1342308352

[CLS:TestFSMDlg]
Type=0
HeaderFile=TestFSMDlg.h
ImplementationFile=TestFSMDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_INPUTSCOMBO

