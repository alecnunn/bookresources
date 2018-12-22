// File: gui.h

#ifndef GUI_H
#define GUI_H



#define IDC_SECTOR           3001
#define IDC_SMMESH           3002
#define IDC_SMPOLYGON        3003
#define IDC_SMVERTEX         3004
#define IDC_SMPORTAL         3005
#define IDC_SMENTITY         3006
#define IDC_SMLIGHT          3007
#define IDC_SMSPAWN          3008
#define IDC_SALL             3009
#define IDC_SINV             3010
#define IDC_SNONE            3011
#define IDC_HSEL             3012
#define IDC_HUNS             3013
#define IDC_HINV             3014
#define IDC_HNONE            3015
#define IDC_H3D              3016
#define IDC_SNUM             3017
#define IDC_TSEL             3018
#define IDC_TTRANS           3019
#define IDC_TROTA            3020
#define IDC_TPOLY            3021
#define IDC_MOUSE            3022
#define IDC_TCX              3023
#define IDC_TCY              3024
#define IDC_TCZ              3025
#define IDC_TCDLG            3026
#define IDC_TCDLG2           3027
#define IDC_SKLIST           3028
#define IDC_SKEDIT           3029
#define IDC_SKAPPLY          3030
#define IDC_SKNEW            3031
#define IDC_SKMAT            3032
#define IDC_DUMMY_1          3033
#define IDC_DUMMY_2          3034

#define SECTOR                  0
#define CSM_MESH                1
#define CSM_POLYGON             2
#define CSM_VERTEX              3
#define CSM_PORTAL              4
#define CSM_ENTITY              5
#define CSM_LIGHT               6
#define CSM_SPAWN               7
#define S1                      8
#define GRP1                    9
#define CS_ALL                 10
#define CS_NONE                11
#define CS_INV                 12
#define CS_NUM                 13
#define S2                     14
#define GRP2                   15
#define CH_SEL                 16
#define CH_UNS                 17
#define CH_NONE                18
#define CH_INV                 19
#define CH_3D                  20
#define S3                     21
#define GRP3                   22
#define CT_SEL                 23
#define CT_TRANS               24
#define CT_ROTA                25
#define CT_POLY                26
#define S4                     27
#define S5                     28
#define MOUSE                  29
#define S6                     30
#define GRP4                   31
#define CTC_X                  32
#define CTC_Y                  33
#define CTC_Z                  34
#define CTC_DLG                35
#define CTC_DLG2               36
#define GRP5                   37
#define CSK_LIST               38
#define CSK_APPLY              39
#define CSK_EDIT               40
#define CSK_NEW                41
#define CSK_MAT                42
#define CSM_DUMMY_1            43
#define CSM_DUMMY_2            44
//--------------------------------
#define MAX_HWND               45


// create a static window
#define STC(a,x,y,xd,yd,id) CreateWindow("static",a,WS_CHILD|WS_VISIBLE,x,y,xd,yd,m_G.hWnd,(HMENU)id,m_G.hInst,NULL);

#endif
