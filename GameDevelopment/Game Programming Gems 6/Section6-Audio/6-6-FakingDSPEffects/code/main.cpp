//----------------------------------------------------------------------------
// File: main.cpp
// Author: Robert Sparks
// Copyright (C) 2005, Robert Sparks
//----------------------------------------------------------------------------
#include <windows.h>
#include <stdio.h>
#include <commctrl.h>
#include "soundsystem.hpp"
#include "soundfile.hpp"
#include "soundstreamer.hpp"
#include "demo.hpp"
#include "resource.h"

//----------------------------------------------------------------------------
// Constants
//----------------------------------------------------------------------------
static const float MAX_WORLD = 30.0f;
static const float ROOM_WALL_LENGTH = 2.0f * MAX_WORLD * 32.0f / 128.0f; 
static const char* PIANO_FILE_NAME = "Brahms Intermezzo in B-Flat Minor, Op. 117, No. 2.wav";

//----------------------------------------------------------------------------
// Local Variables
//----------------------------------------------------------------------------
static Demo* gpDemo = NULL; // The demo
static D3DVECTOR gListenerPos = { 0.0f, 0.0f, ROOM_WALL_LENGTH / 4.0f }; // Listener Position
static HBITMAP ghBitmap = NULL;  // Handle to bitmap in the GUI
static unsigned int gBitmapWidth = 0; // Keep the width of the bitmap in pixels for paintin
static unsigned int gBitmapHeight = 0; // Keep the height of the bitmap in pixels for painting

//-----------------------------------------------------------------------------
// Initialize
//-----------------------------------------------------------------------------
static void Initialize( HWND hDlg )
{
    // We update 30 times a second using a timer
    SetTimer( hDlg, 0, 33, NULL );

    // Prepare the bitmap
    HINSTANCE hInst = (HINSTANCE) GetWindowLong( hDlg, GWL_HINSTANCE );
    ghBitmap = LoadBitmap( hInst, MAKEINTRESOURCE( IDB_ROOM ) );
    BITMAP bmp;
    GetObject( ghBitmap, sizeof(BITMAP), &bmp );
    gBitmapWidth = bmp.bmWidth;
    gBitmapHeight = bmp.bmHeight;

    // Set big and small icons
    HICON hIcon = LoadIcon( hInst, MAKEINTRESOURCE( IDI_FAKINGDSP ) );
    PostMessage( hDlg, WM_SETICON, ICON_BIG, ( LPARAM ) hIcon ); 
    PostMessage( hDlg, WM_SETICON, ICON_SMALL, ( LPARAM ) hIcon );


    // Prepare the listener position slider
    HWND hListenerSlider  = GetDlgItem( hDlg, IDC_LISTENER_SLIDER );
    PostMessage( hListenerSlider, TBM_SETRANGEMIN, TRUE, -100L );
    PostMessage( hListenerSlider, TBM_SETRANGEMAX, TRUE, 100L );
    PostMessage( hListenerSlider, TBM_SETPOS, TRUE, -35L );

    // Initialize the sound system 
    SoundSystem::Initialize( hDlg );
    
    // Create a new demo
    gpDemo = new Demo( ROOM_WALL_LENGTH, PIANO_FILE_NAME );
}

//-----------------------------------------------------------------------------
// Terminate
//-----------------------------------------------------------------------------
static void Terminate( void )
{
    if ( NULL != gpDemo )
    {
        delete gpDemo;
        gpDemo = NULL;
    }
    SoundSystem::Terminate( );
}

//-----------------------------------------------------------------------------
// Update the drawing of the listener on the bitmap
//----------------------------------------------------------------------------
static void UpdatePicture( HWND hDlg )
{
    static LONG sLastX = 0;
    static LONG sLastY = 0;

    // Don't update the grid if a WM_PAINT will be called soon
    if( GetUpdateRect( hDlg, NULL, false ) )
    {
        return;
    }        

    // Get handles
    HWND hWndGrid = GetDlgItem( hDlg, IDC_PICTURE );
    HDC  hDC      = GetDC( hWndGrid );
    HDC  hDCbmp   = CreateCompatibleDC( hDC );

    // Get the bitmap of the room
    SelectObject( hDCbmp, ghBitmap );

    // Reduce the painting area
    IntersectClipRect( hDC, 0, 0, gBitmapWidth, gBitmapHeight );

    // Restore the bitmap where we drew last time
    BitBlt( hDC, sLastX - 1, sLastY , 4, 7, hDCbmp, sLastX - 1, sLastY - 1, SRCCOPY ); 

    // Calculate our next draw location
    RECT rc;
    GetClientRect( hWndGrid, &rc );
    sLastX = (LONG)( ( -gListenerPos.x / MAX_WORLD * gBitmapWidth / 2.0f ) + ( ( rc.left + rc.right ) / 2 ) );
    sLastY = (LONG)( ( gListenerPos.z / MAX_WORLD * gBitmapHeight / 2.0f ) + ( rc.top + rc.bottom ) / 2 );

    // Draw an 'L' in red to represent the listener
    SetPixel( hDC, sLastX - 1, sLastY + 0, 0x000000ff );
    SetPixel( hDC, sLastX - 1, sLastY + 1, 0x000000ff );
    SetPixel( hDC, sLastX - 1, sLastY + 2, 0x000000ff );
    SetPixel( hDC, sLastX - 1, sLastY + 3, 0x000000ff );
    SetPixel( hDC, sLastX - 1, sLastY + 4, 0x000000ff );
    SetPixel( hDC, sLastX - 1, sLastY + 5, 0x000000ff );
    SetPixel( hDC, sLastX - 1, sLastY + 6, 0x000000ff );
    SetPixel( hDC, sLastX - 0, sLastY + 0, 0x000000ff );
    SetPixel( hDC, sLastX - 0, sLastY + 1, 0x000000ff );
    SetPixel( hDC, sLastX - 0, sLastY + 2, 0x000000ff );
    SetPixel( hDC, sLastX - 0, sLastY + 3, 0x000000ff );
    SetPixel( hDC, sLastX - 0, sLastY + 4, 0x000000ff );
    SetPixel( hDC, sLastX - 0, sLastY + 5, 0x000000ff );
    SetPixel( hDC, sLastX - 0, sLastY + 6, 0x000000ff );
    SetPixel( hDC, sLastX + 1, sLastY + 6, 0x000000ff );
    SetPixel( hDC, sLastX + 1, sLastY + 5, 0x000000ff );
    SetPixel( hDC, sLastX + 2, sLastY + 6, 0x000000ff );
    SetPixel( hDC, sLastX + 2, sLastY + 5, 0x000000ff );

    ReleaseDC( hWndGrid, hDC );
    DeleteDC( hDCbmp );
}

//-----------------------------------------------------------------------------
// Update
//----------------------------------------------------------------------------
static void Update( HWND hDlg )
{
    // Calculate the listener position
    HWND hListenerSlider  = GetDlgItem( hDlg, IDC_LISTENER_SLIDER );
    gListenerPos.x = - MAX_WORLD * SendMessage( hListenerSlider, TBM_GETPOS, 0, 0 ) / 100.0f;
    SoundSystem::Get( )->SetListenerPosition( gListenerPos );
   
    // Update the demo.
    if ( NULL != gpDemo )
    {   
        gpDemo->Update( );
    }
    
    // Draw the listener on the picture
    UpdatePicture( hDlg );
}

//-----------------------------------------------------------------------------
// Handle Windows Messages
//----------------------------------------------------------------------------
INT_PTR CALLBACK MainDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch (msg) 
    {
        case WM_INITDIALOG:
        {
            Initialize( hDlg );
            break;
        }            
        case WM_COMMAND:
        {
            switch ( LOWORD(wParam) )
            {
                case IDCANCEL:
                {
                    PostQuitMessage( 0 );
                    EndDialog( hDlg, IDCANCEL );
                    break;
                }                    
                default:
                {
                    return false;
                }                    
            }
            break;
        }            
        case WM_TIMER:
        {
            Update( hDlg );
            break;
        }        
        case WM_DESTROY:
        {
            Terminate( );        
            break; 
        }
        default:
        {
            return false;
        }            
    }
    return true;
}

//----------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------
INT APIENTRY WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR pCmdLine, INT nCmdShow )
{
    // Initialize common controls because we use sliders
    InitCommonControlsEx( 0 );
    
    // Display the main dialog box.
    DialogBox( hInst, MAKEINTRESOURCE(ID_MAIN), NULL, MainDlgProc );

    return TRUE;
}

