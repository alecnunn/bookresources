/* Copyright (C) Guy W. Lecky-Thompson, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Guy W. Lecky-Thompson, 2000"
 */
//example1.h 
// Make instance handle available to other modules 
extern HINSTANCE hInstance; 
extern HWND hAppWnd; 
// Define Icon Ressource IDs 
#define IDC_CANCEL	102
#define IDC_QUAD_SIZE	103
#define IDC_START	101
#define DIALOG_1	1
#define ICON_APPWND 100
#define IDC_STATIC -1

// Menus
#define IDM_QUIT			200
#define IDM_HELP_ABOUT	201
#define IDM_QUAD_AVE			202
#define IDM_FAULTLINES			203
#define IDM_FUZZY			204
#define IDM_TERRAIN_PROPERTIES 205
#define IDM_METAFILE 206
#define IDM_QUAD			207

// Dlg stuff
#define IDD_OK				900
#define IDD_CANCEL			901
#define IDC_WATER			902
#define IDC_PLAINS			903
#define IDC_HILLS			904
#define IDC_MOUNTAINS		905
#define IDC_MAXHEIGHT		906
#define IDC_SEED			907
#define IDC_QUADSIZE		908
