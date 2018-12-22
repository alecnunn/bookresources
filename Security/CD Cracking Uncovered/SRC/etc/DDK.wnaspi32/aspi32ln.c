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
*  PROGRAM:     ASPI32LN.C
*
*  PURPOSE:     A sample ASPI for WIN32 program linking the WNASPI32.DLL
*               export functions. The sample application first queries the
*               system to detect ASPI for WIN32 support and the number of
*				SCSI host adapters present.  Once proper ASPI for WIN32 support
*				is detected, the application may perform read loops on each
*				block device found using SendASPI32Command.
*
\******************************************************************************/

#include <windows.h>
#include "aspi32ln.h"
#include "scsidefs.h"
#include "wnaspi32.h"

/******************************************************************************\
*
*  FUNCTION:    WinMain (standard WinMain INPUTS/RETURNS)
*
*  GLOBAL VARS: ghwndMain - handle of main app window
*
*  LOCAL VARS:  msg - msg to get/dispatch
*
\******************************************************************************/

int PASCAL WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine, int nCmdShow)
{
    MSG         msg;
    
  if (!hPrevInstance)
  {
    WNDCLASS wc;

    wc.style         = 0;
    wc.lpfnWndProc   = (WNDPROC)MainWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor (NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject (WHITE_BRUSH);
    wc.lpszMenuName  = (LPCTSTR) "Menu";
    wc.lpszClassName = (LPCTSTR) "LINKTEST";

    if (!RegisterClass (&wc))
    {
      MessageBox (NULL, (LPCTSTR) "WinMain(): RegisterClass() failed",
                  (LPCTSTR) "Error - ASPI for Win32", MB_OK | MB_ICONEXCLAMATION);
      return(FALSE);
    }
  }

  if (!(ghwndMain = CreateWindow ("LINKTEST",
                                  "ASPI for WIN32 Sample Application",
                                  WS_OVERLAPPEDWINDOW,
                                  CW_USEDEFAULT, CW_USEDEFAULT,
                                  CW_USEDEFAULT, CW_USEDEFAULT,
                                  NULL, NULL, hInstance, NULL)))
    return (0);

  ShowWindow (ghwndMain, nCmdShow);

  DataBufferSize		= 0x10000;
  BlockStart			= 0;
  EnableDisplayReadInfo = TRUE;
  PostToMessageQueue	= TRUE;

  ASPI32Status 			= GetASPI32SupportInfo();
  AdapterCount 			= (LOBYTE(LOWORD(ASPI32Status)));


  if ((AdapterCount != 0) && (HIBYTE(LOWORD(ASPI32Status)) == SS_COMP)) {

  	while (GetMessage (&msg, NULL, 0, 0)) {
    	TranslateMessage (&msg);
    	DispatchMessage  (&msg);
  		}

  } else {
	MessageBox (NULL, (LPCTSTR) "No ASPI for WIN32 support detected.",
                  (LPCTSTR) "ASPI for WIN32 sample", MB_OK);
  }	

  return (msg.wParam);
}



/******************************************************************************\
*
*  FUNCTION:    MainWndProc (standard window procedure INPUTS/RETURNS)
*
*  COMMENTS:    IDM_START_READ_LOOP starts read loop on all block devices.
*               IDM_STOP_READ_LOOP puts on the brakes and stops all reads.
*               IDM_OPTIONS_DISPLAYREADRESULTS toggles enabling of read results
*               to window.
*				IDM_OPTIONS_SENDPOSTTOMESSAGEQUEUE toggles whether sending of
*				next SRB read command is done inside the post procedure or
*				signaled through the message queue.
*				
*
\******************************************************************************/

LRESULT CALLBACK MainWndProc (HWND hwnd, UINT message, WPARAM wParam,
                              LPARAM lParam)
{
	HDC			hdc;
	TEXTMETRIC	tm;
	PAINTSTRUCT	ps;
	char		adapterid,targetid;
	HMENU		hMenu;
	
  switch (message)
  {

	case WM_CREATE:
		hdc = GetDC(hwnd);
		SelectObject (hdc, GetStockObject (SYSTEM_FIXED_FONT));
		GetTextMetrics (hdc, &tm);
		cxChar = (short)tm.tmAveCharWidth;
		cyChar = (short)tm.tmHeight;
		ReleaseDC (hwnd, hdc);

	case WM_PAINT:
		InvalidateRect (hwnd, NULL, TRUE);
		hdc = BeginPaint (hwnd, &ps);
		EndPaint (hwnd,&ps);

    case WM_COMMAND:

	  hMenu = GetMenu(hwnd);

      switch (LOWORD(wParam))
      {

		/* Start Read loop */
        case IDM_START_READ_LOOP:
			StartRead = !ToggleMenuItemCheck(hMenu, wParam);
			if (StartRead) {
				for (adapterid = 0; adapterid < AdapterCount; adapterid++)
					for (targetid = 0; targetid < MAX_ID; targetid++)
						SendInquiry(adapterid,targetid);
				} /* if (StartRead) .. */
        	break;

		/* Start thread READ loop */
        case IDM_START_THREAD_READ_LOOP:
			break;

		/* Toggle Display Read Info flag */
        case IDM_OPTIONS_DISPLAYREADRESULTS:
			EnableDisplayReadInfo =  !ToggleMenuItemCheck(hMenu, wParam);
        	break;

		/* Post to message queue. If enabled, posting
		 * is done by sending message to message queue.
		 * If disabled, next request is sent from the
		 * post routine.
		 */
        case IDM_OPTIONS_SENDPOSTTOMESSAGEQUEUE:
			PostToMessageQueue = !ToggleMenuItemCheck(hMenu, wParam);
          	break;


        case IDM_ABOUT:

          DialogBox (GetModuleHandle (NULL), (LPCTSTR)"About", hwnd,
                     (DLGPROC) AboutDlgProc);
          break;

        default:

          break;
    	}
    	break;

    case WM_DESTROY:

    	PostQuitMessage(0);
    	break;

	case WM_ASPI32_POST:

		/* send another command */
		SendRead(((PSRB_ExecSCSICmd) lParam)->SRB_HaId,
				 ((PSRB_ExecSCSICmd) lParam)->SRB_Target,
				 DTYPE_DASD);

		/* free up resources */		
		free (((PSRB_ExecSCSICmd) lParam)->SRB_BufPointer);
		free ((void *)lParam);
		break;

    default:

      return (DefWindowProc(hwnd, message, wParam, lParam));
  }
  return (0);
}



/******************************************************************************\
*
*  FUNCTION:    ToggleMenuItemCheck
*
*  DESCRIPTION:
*				Toggles the specified menu item's checked state.
*
*  ENTRY:
*				hMenu, wParam
*  EXIT:
*				TRUE	item is checked.
*				FALSE	item is not checked
*
\******************************************************************************/
BOOL ToggleMenuItemCheck (HMENU hMenu, WPARAM wParam)

{
	UINT MenuItemStatus;

	/* fetch item's state */
	MenuItemStatus = GetMenuState (hMenu, wParam, MF_BYCOMMAND) & MF_CHECKED;

	/* toggle item's state */
	CheckMenuItem (hMenu, wParam, MF_BYCOMMAND |
									(MenuItemStatus ? MF_UNCHECKED:MF_CHECKED) );

	return (MenuItemStatus == MF_CHECKED);

} /* ToggleMenuItemCheck */

/******************************************************************************\
*
*  FUNCTION:    AboutDlgProc (standard dialog procedure INPUTS/RETURNS)
*
*  COMMENTS:    Displays "about" message
*
\******************************************************************************/

BOOL CALLBACK AboutDlgProc (HWND hwnd, UINT message, WPARAM wParam,
                               LPARAM lParam)
{ switch (message)
  {
    case WM_COMMAND:

      if (LOWORD(wParam) == IDOK)
      {
        EndDialog(hwnd, TRUE);
        return (TRUE);
      }
      return (TRUE);
  }
  return (FALSE);
}

/****************************************************************************
* Function:	SendInquiry	
*
* Description:	Sends SCSI INQUIRY command to a device
*
* Parameters:
*				adapterid   - adapter ID to send command.
*				targetid	- target ID to send command.
*
* Returns:  Nothing
****************************************************************************/
void SendInquiry (BYTE adapter_id, BYTE target_id)

{
	void	*InqSRB;			/* temporary inquiry ASPI SRB pointer */
	DWORD 	tempd;

    InqSRB = malloc(ASPI_SRB_LEN);
	memset(InqSRB, 0, ASPI_SRB_LEN);
	((PSRB_ExecSCSICmd) InqSRB)->SRB_Cmd			= SC_EXEC_SCSI_CMD;
	((PSRB_ExecSCSICmd) InqSRB)->SRB_HaId			= adapter_id;
	((PSRB_ExecSCSICmd) InqSRB)->SRB_Flags		= SRB_DIR_IN | SRB_POSTING;
	((PSRB_ExecSCSICmd) InqSRB)->SRB_Target		= target_id;
	((PSRB_ExecSCSICmd) InqSRB)->SRB_BufPointer	= malloc(INQUIRY_BUFFER_LENGTH);
	((PSRB_ExecSCSICmd) InqSRB)->SRB_BufLen		= INQUIRY_BUFFER_LENGTH;
	((PSRB_ExecSCSICmd) InqSRB)->SRB_SenseLen		= SENSE_LEN;
	((PSRB_ExecSCSICmd) InqSRB)->SRB_CDBLen		= 6;
	((PSRB_ExecSCSICmd) InqSRB)->CDBByte [0]		= SCSI_INQUIRY;
	((PSRB_ExecSCSICmd) InqSRB)->CDBByte [4]		= INQUIRY_BUFFER_LENGTH;
	((PSRB_ExecSCSICmd) InqSRB)->SRB_PostProc		= (void *) ASPI32Post;

	tempd = SendASPI32Command(InqSRB);
}
//***************************************************************************
// Function:	SendRead
//
// Description:	Sends READ command to a disk
//
// Parameters:
//				target_id   - target ID to send command.
//
// Returns:  Nothing
//***************************************************************************
void SendRead (BYTE adapter_id, BYTE read_id, BYTE device_type)

{
	void	*ReadExecSRB;

    ReadExecSRB = malloc(ASPI_SRB_LEN);
	memset(ReadExecSRB, 0, ASPI_SRB_LEN);

	((PSRB_ExecSCSICmd) ReadExecSRB)->SRB_Cmd         = SC_EXEC_SCSI_CMD;
	((PSRB_ExecSCSICmd) ReadExecSRB)->SRB_HaId		= adapter_id;
	((PSRB_ExecSCSICmd) ReadExecSRB)->SRB_Flags		= SRB_DIR_IN | SRB_POSTING;
	((PSRB_ExecSCSICmd) ReadExecSRB)->SRB_Target		= read_id;
	((PSRB_ExecSCSICmd) ReadExecSRB)->SRB_BufPointer	= malloc(DataBufferSize);
	((PSRB_ExecSCSICmd) ReadExecSRB)->SRB_BufLen		= DataBufferSize;
	((PSRB_ExecSCSICmd) ReadExecSRB)->SRB_SenseLen	= SENSE_LEN;
	((PSRB_ExecSCSICmd) ReadExecSRB)->SRB_CDBLen		= 10;
	((PSRB_ExecSCSICmd) ReadExecSRB)->CDBByte [0]	    = 0x28;
	((PSRB_ExecSCSICmd) ReadExecSRB)->CDBByte [2]	    = HIBYTE(HIWORD(BlockStart));
	((PSRB_ExecSCSICmd) ReadExecSRB)->CDBByte [3]	    = LOBYTE(HIWORD(BlockStart));
	((PSRB_ExecSCSICmd) ReadExecSRB)->CDBByte [4]	    = HIBYTE(LOWORD(BlockStart));
	((PSRB_ExecSCSICmd) ReadExecSRB)->CDBByte [5]	    = LOBYTE(LOWORD(BlockStart));
	BlockStart = (BlockStart + BLOCK_INCREMENT) & (HIGH_BLOCK_START);
	((PSRB_ExecSCSICmd) ReadExecSRB)->SRB_PostProc    = (void *) ASPI32Post;

	if (device_type == DTYPE_DASD) {
		((PSRB_ExecSCSICmd) ReadExecSRB)->CDBByte [7] = HIBYTE(LOWORD(DataBufferSize/DISK_SEC_SIZE)); /* block count */ 
		((PSRB_ExecSCSICmd) ReadExecSRB)->CDBByte [8] = LOBYTE(LOWORD(DataBufferSize/DISK_SEC_SIZE)); /* block count */ 
		}
	
	ASPI32Status = SendASPI32Command(ReadExecSRB);

} // End SendRead

/****************************************************************************
* Function:		ASPI32Post	
*
* Description:	Callback function called when ASPI WIN32 request completes.
*
* Parameters:
*				Srb	- pointer to ASPI SRB completed
*
* Returns:  Nothing
****************************************************************************/
void ASPI32Post (void *Srb)

{

	/* Process SCSI INQUIRY command */
	if ((((PSRB_ExecSCSICmd) Srb)->CDBByte [0]) == SCSI_INQUIRY) {

		/* for block devices present, send READ commands */
		if ((((PSRB_ExecSCSICmd) Srb)->SRB_Status) == SS_COMP) {

			/* disk device */
			if (*(((PSRB_ExecSCSICmd) Srb)->SRB_BufPointer) == DTYPE_DASD)
				SendRead(((PSRB_ExecSCSICmd) Srb)->SRB_HaId,
						 ((PSRB_ExecSCSICmd) Srb)->SRB_Target,
						 DTYPE_DASD);

			/* CDROM device */
			else if (*(((PSRB_ExecSCSICmd) Srb)->SRB_BufPointer) == DTYPE_CROM)
				SendRead(((PSRB_ExecSCSICmd) Srb)->SRB_HaId,
						 ((PSRB_ExecSCSICmd) Srb)->SRB_Target,
						 DTYPE_CROM);

			/* add any other devices here */
			}

		/* display INQUIRY result */
    	DisplaySCSIID (ghwndMain, Srb);

		/* free up INQUIRY resources since it's not needed anymore */
		free (((PSRB_ExecSCSICmd) Srb)->SRB_BufPointer);
		free (Srb);
		}


	/* Process READ commands if global flag didn't put break on */
	else if ((((PSRB_ExecSCSICmd) Srb)->CDBByte [0]) == SCSI_READ10) {
	
		if (StartRead) {

	   		/* display read result if enabled */
			if (EnableDisplayReadInfo)
    			DisplayReadInfo (ghwndMain, Srb);

			/* send another request by posting to message queue.. */
			if (PostToMessageQueue)
				PostMessage(ghwndMain, WM_ASPI32_POST, 0, (LPARAM) Srb);
			/* or send another request from our post routine */ 
			else {
				SendRead(((PSRB_ExecSCSICmd) Srb)->SRB_HaId,
						 ((PSRB_ExecSCSICmd) Srb)->SRB_Target,
						 DTYPE_DASD);

				/* free up resources */		
				free (((PSRB_ExecSCSICmd) Srb)->SRB_BufPointer);
				free ((void *)Srb);
				}

			}

		else {

			/* free up resources */
			free (((PSRB_ExecSCSICmd) Srb)->SRB_BufPointer);
			free (Srb);
			}
		}
	
}

//***************************************************************************
// Function:		DisplaySCSIID
//
// Description:	This function will scroll the window down and display
//						the device name of the target we just sent a SCSI Inquiry
//						command.
//
// hwnd    - This parameter contains the window handle where the device name
//				 should be displayed.
//
// ExecSRB - This parameter points to the ASPI SCSI Request Block (SRB) which
//				 has completed.
//
// Returns:  Nothing
//***************************************************************************
void DisplaySCSIID ( HWND hwnd, void *Srb )
	{
	HDC	hdc;
	char	string_buf[80];
	int	string_len;

	hdc = GetDC (hwnd);
	SelectObject (hdc, GetStockObject (SYSTEM_FIXED_FONT));
	ScrollDC (hdc, 0, cyChar, NULL, NULL, NULL, NULL);

	if ((((PSRB_ExecSCSICmd) Srb)->SRB_Status) == SS_COMP)
		{
		wsprintf (	string_buf,
						"Host Adapter #%d - Target #%d:  ",
						(int) ((PSRB_ExecSCSICmd) Srb)->SRB_HaId,
						(int) ((PSRB_ExecSCSICmd) Srb)->SRB_Target );

		lstrcat ( string_buf, &((PSRB_ExecSCSICmd) Srb)->SRB_BufPointer[8] );
		string_len = lstrlen ( string_buf );
		}
	else
		{
		string_len = wsprintf ( string_buf,
										"Host Adapter #%d - Target #%d:  %-32s",
										(int) ((PSRB_ExecSCSICmd) Srb)->SRB_HaId,
										(int) ((PSRB_ExecSCSICmd) Srb)->SRB_Target,
										(LPSTR) "no device present." );
		}

	// TextOut (hdc, cxChar, cyChar/2, string_buf, string_len );
	TextOut (hdc, cxChar, cyChar, string_buf, string_len );

	ReleaseDC (hwnd,hdc);
	return;
	}

//***************************************************************************
// Function:		DisplayReadInfo
//
// Description:	This function will scroll the window down and display
//				the device id of the target that performed the SCSI READ
//				command.
//
// hwnd    - This parameter contains the window handle where the device name
//				 should be displayed.
//
// ExecSRB - This parameter points to the ASPI SCSI Request Block (SRB) which
//				 has completed.
//
// Returns:  Nothing
//***************************************************************************
void DisplayReadInfo ( HWND hwnd, void *Srb )
	{
	HDC	hdc;
	char	string_buf[80];
	int	string_len;

	hdc = GetDC (hwnd);
	SelectObject (hdc, GetStockObject (SYSTEM_FIXED_FONT));
	ScrollDC (hdc, 0, cyChar, NULL, NULL, NULL, NULL);

	if (((PSRB_ExecSCSICmd) Srb)->SRB_Status==SS_COMP)
		{
		string_len = wsprintf ( string_buf,
										"Host Adapter #%d - Target #%d:  %-32s",
										(int) ((PSRB_ExecSCSICmd) Srb)->SRB_HaId,
										(int) ((PSRB_ExecSCSICmd) Srb)->SRB_Target,
										(LPSTR) "read successful." );

		}
	else if (((PSRB_ExecSCSICmd) Srb)->SRB_Status==SS_NO_DEVICE)
		{
		string_len = wsprintf ( string_buf,
										"Host Adapter #%d - Target #%d:  %-32s",
										(int) ((PSRB_ExecSCSICmd) Srb)->SRB_HaId,
										(int) ((PSRB_ExecSCSICmd) Srb)->SRB_Target,
										(LPSTR) "no device installed." );
		}

	else 
		{
		string_len = wsprintf ( string_buf,
										"Host Adapter #%d - Target #%d:  %-32s",
										(int) ((PSRB_ExecSCSICmd) Srb)->SRB_HaId,
										(int) ((PSRB_ExecSCSICmd) Srb)->SRB_Target,
										(LPSTR) "read error." );
		}


	TextOut (hdc, cxChar, cyChar, string_buf, string_len );

	ReleaseDC (hwnd,hdc);
	return;
	}

