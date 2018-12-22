/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

/******************************************************************************\
*
*                                 ASPI32LN.H
*
\******************************************************************************/

/******************************************************************************\
*                              SYMBOLIC CONSTANTS
\******************************************************************************/

#define IDM_START_READ_LOOP				 	100 /* menu identifiers */
#define IDM_START_THREAD_READ_LOOP			101
#define IDM_OPTIONS_DISPLAYREADRESULTS   	102
#define IDM_OPTIONS_SENDPOSTTOMESSAGEQUEUE	103
#define IDM_ABOUT        					106

#define WM_ASPI32_POST	WM_USER+10

/* max scsi target id */
#define MAX_ID           		8

/* ASPI SRB packet length */
#define ASPI_SRB_LEN			0x100

/* inquiry buffer length */
#define INQUIRY_BUFFER_LENGTH 	0x20

/* data transfer length for reads */
#define DATA_BUFFER_LENGTH		0x10000

/* disk sector size */
#define DISK_SEC_SIZE  			0x200

/* disk sector size */
#define CDROM_SEC_SIZE  		0x800

/* maximum start block */
#define HIGH_BLOCK_START		0x4000
#define BLOCK_INCREMENT			0x80
/******************************************************************************\
*                                  GLOBAL VARS
\******************************************************************************/

HWND ghwndMain;

BYTE  AdapterCount;
DWORD ASPI32Status;

/* Global loop flag */
BOOL  StartRead;

/* Global start block */
DWORD	BlockStart;

/* Data buffer size for all devices */
DWORD	DataBufferSize;

BOOL	EnableDisplayReadInfo;
BOOL	PostToMessageQueue;

short cxChar, cyChar;

/******************************************************************************\
*                              FUNCTION PROTOTYPES
\******************************************************************************/

LRESULT CALLBACK MainWndProc  (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AboutDlgProc (HWND, UINT, WPARAM, LPARAM);
BOOL ToggleMenuItemCheck (HMENU, WPARAM);
void ASPI32Post (LPVOID);
void DisplaySCSIID ( HWND hwnd, void *);
void WINAPI ASPI32ReadPost (void *);
void DisplayReadInfo ( HWND hwnd, void *);
void SendInquiry ( BYTE adapter_id, BYTE target_id);
void SendRead ( BYTE adapter_id, BYTE target_id, BYTE device_type);

