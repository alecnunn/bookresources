////////////////////////////////////////////////////////////
//
//  RESOURCE.H - HELIOS.RC Include File
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/09/23 - Added IDC_NUMSETEP, IDC_CONVERGE,
//                         IDC_ELAPSED, IDC_MAXSTEP and
//                         IDC_MINSTOP definitions.
//              94/09/27 - Added IDC_UPDATE definition.
//              94/10/02 - Added IDD_UPD_DISPLAY definition.
//              94/10/12 - Added IDC_INTENS and
//                         IDC_INTENS_EN definitions.
//              94/11/29 - Added IDC_COLRES_8 and
//                         IDC_COLRES_24 definitions.
//              94/12/04 - Added IDC_RENDERMSG definition.
//              94/12/08 - Added IDM_HELP_CONTENTS,
//                         IDM_HELP_HELP, IDM_HELP_BOOK,
//                         IDC_HELP_SETCAMERA,
//                         IDC_HELP_SETVIEW,
//                         IDC_HELP_SETCONVERGE,
//                         IDC_HELP_SETDISPLAY, and
//                         IDC_HELP_SETDIR definitions.
//              94/12/16 - Version 1.01A release.
//              94/12/24 - Added IDC_PERSPECT, IDC_SCALE,
//                         and IDC_PARALLEL definitions.
//              95/02/05 - Version 1.02A release.
//              95/03/26 - Added IDC_MAXPASS, IDC_NUMPASS,
//                         IDC_HELP_SETCOLOR, IDM_SETCOLOR,
//                         IDC_FILTERSEL and IDC_FILTERTYPE
//                         definitions.
//                       - Deleted IDC_RENDERMSG definition.
//              95/07/21 - Version 1.02B release.
//              96/02/14 - Version 1.02C release.
//              96/03/30 - Added IDC_NUM_INSTANCES,
//                         IDC_NUM_SURFACES,
//                         IDC_NUM_PATCHES,
//                         IDC_NUM_ELEMENTS,
//                         IDC_NUM_VERTICES, IDC_CAMTILT,
//                         IDM_DEFVIEW, IDM_SETDOLLY,
//                         IDM_SETORBIT, IDM_SETPAN,
//                         IDM_SETROTATE, IDM_SETTILT,
//                         IDM_SETZOOM, IDC_FOCUS_X,
//                         IDC_FOCUS_Y, IDC_FOCUS_Z,
//                         IDM_STATISTICS, MENU_CAMERA_POS
//                         and MENU_INTERACTIVE_POS
//                         definitions.
//                       - Deleted IDC_VDVH, IDC_VDVV,
//                         IDC_VUPH and IDC_VUPV
//                         definitions.
//                       - Changed IDC_XPOS, IDC_YPOS and
//                         IDC_ZPOS definitions to
//                         IDC_EYE_X, IDC_EYE_Y and
//                         IDC_EYE_Z.
//              96/04/01 - Version 1.03A release.
//              97/04/01 - Version 1.03B release.
//
//  Compilers:  Microsoft Visual C/C++ Professional V5.0
//              Borland C++ Version 5.0
//
//  Author:     Ian Ashdown, P.Eng.
//              byHeart Consultants Limited
//              620 Ballantree Road
//              West Vancouver, B.C.
//              Canada V7S 1W3
//              Tel. (604) 922-6148
//              Fax. (604) 987-7621
//
//  Copyright 1994-2003 byHeart Consultants Limited
//
//  The following source code has been derived from:
//
//    Ashdown, I. 1994. Radiosity: A Programmer's
//    Perspective. New York, NY: John Wiley & Sons.
//
//    (Available on CD-ROM from www.helios32.com.)
//
//  It may be freely copied, redistributed, and/or modified
//  for personal use ONLY, as long as the copyright notice
//  is included with all source code files.
//
////////////////////////////////////////////////////////////

#ifndef _RESOURCE_H
#define _RESOURCE_H

// Menu position indices
#define MENU_CAMERA_POS         1       // Camera
#define MENU_INTERACTIVE_POS    5       // Interactive

#define IDM_FILEOPEN            100
#define IDM_SAVEAS              101
#define IDM_EXIT                102
#define IDM_DIRECTORY           103
#define IDM_STATISTICS          104
#define IDM_SETCAMERA           105
#define IDM_SETVIEW             106
#define IDM_SETDOLLY            107
#define IDM_SETORBIT            108
#define IDM_SETPAN              109
#define IDM_SETROTATE           110
#define IDM_SETTILT             111
#define IDM_SETZOOM             112
#define IDM_DEFVIEW             113
#define IDM_WIREFRAME           114
#define IDM_SHADED              115
#define IDM_RENDER              116
#define IDM_REDISPLAY           117
#define IDM_SETCOLOR            118
#define IDM_SETCONVERGE         119
#define IDM_SETDISPLAY          120
#define IDM_HELP_CONTENTS       121
#define IDM_HELP_HELP           122
#define IDM_HELP_BOOK           123
#define IDM_ABOUT               124
#define IDC_STATIC              -1
#define IDC_EYE_X               1000
#define IDC_EYE_Y               1001
#define IDC_EYE_Z               1002
#define IDC_FOCUS_X             1003
#define IDC_FOCUS_Y             1004
#define IDC_FOCUS_Z             1005
#define IDC_VDIST               1006
#define IDC_FDIST               1007
#define IDC_BDIST               1008
#define IDC_HPIXSZ              1009
#define IDC_VPIXSZ              1010
#define IDC_CAMTILT             1011
#define IDC_PERSPECT            1012
#define IDC_PARALLEL            1013
#define IDC_SCALE               1014
#define IDC_ENTITY              1015
#define IDC_GAMMA               1016
#define IDC_GAMMA_EN            1017
#define IDC_JITTER              1018
#define IDC_JITTER_EN           1019
#define IDC_MSTEP               1020
#define IDC_STOPC               1021
#define IDC_AMBIENT_EN          1022
#define IDC_OVER_EN             1023
#define IDC_RGB                 1024
#define IDC_MONO                1025
#define IDC_PSEUDO              1026
#define IDC_NUMSTEP             1027
#define IDC_CONVERGE            1028
#define IDC_ELAPSED             1029
#define IDC_MAXSTEP             1030
#define IDC_MINSTOP             1031
#define IDC_UPDATE              1032
#define IDC_INTENS              1033
#define IDC_INTENS_EN           1034
#define IDC_CONTRAST            1035
#define IDC_CONTRAST_EN         1036
#define IDC_COLRES_24           1037
#define IDC_COLRES_8            1038
#define IDC_MAXPASS             1039
#define IDC_NUMPASS             1040
#define IDC_FILTERTYPE          1041
#define IDC_FILTERSEL           1042
#define IDC_NUM_INSTANCES       1043
#define IDC_NUM_SURFACES        1044
#define IDC_NUM_PATCHES         1045
#define IDC_NUM_ELEMENTS        1046
#define IDC_NUM_VERTICES        1047
#define IDC_HELP_SETCAMERA      1048
#define IDC_HELP_SETCOLOR       1049
#define IDC_HELP_SETCONVERGE    1050
#define IDC_HELP_SETDIR         1051
#define IDC_HELP_SETDISPLAY     1052
#define IDC_HELP_SETVIEW        1053
#define IDD_UPD_DISPLAY         2000

#endif

