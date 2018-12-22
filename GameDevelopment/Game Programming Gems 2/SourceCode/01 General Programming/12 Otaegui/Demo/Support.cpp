/*********************************************************************
 * Support.cpp - Support Functions V.2.15.001
 *
 * (c) Copyright 1996-2000 Sabarasa Entertainment
 * For internal use only.
 *********************************************************************/               

/*
	("Abandon all hope, ye who enter here")

						-- Dante. Inferno
*/

#include <dos.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <iostream.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <stdarg.h>

#include "mainptah7.h"

#include "support.hpp"
#include "input.hpp"
#include "blitbuf.hpp"
#include "prim.hpp"
#include "stack.h"

/* ----------------- Globals --------------------- */

LPDIRECTDRAWSURFACE7	vscreen=NULL;			// Back buffer
LPDIRECTDRAWSURFACE7	lpDDSPrimary=NULL;		// DirectDraw primary surface

int PhysicalWidth=PHYSICAL_WIDTH;
int PhysicalHeight=PHYSICAL_HEIGHT;
int PhysicalBPP=PHYSICAL_BPP;

// These variables are set in StorePixelFormat and should be defined
// elsewhere(global, member variable, etc...)
WORD NumberRedBits, NumberGreenBits, NumberBlueBits;
WORD LowRedBit, LowGreenBit, LowBlueBit;
DWORD RBitMask, GBitMask, BBitMask;

/* ---------------- Globales privadas ------------- */

// Last time we called FillTime()
static unsigned long last_fill_time=0;

// DirectDraw Globals
LPDIRECTDRAW7 lpDD=NULL;           // DirectDraw object

/* -------------- Timing Functions --------------- */

void
delay( int time )
{
	Sleep( time );
}

void
FillTime( int interval )
{
	unsigned long a;

	a=GetTickCount();

	if( a<(interval+last_fill_time) )
		Sleep( interval-a+last_fill_time );

	last_fill_time = a;
}

/*------------------------- Interface Functions ----------------------------*/

/* ------------------------ Private ---------------------- */

#define MLANG_ABORTING_DD				1000
#define MLANG_ABORTING_EXCLUSIVE		1001
#define MLANG_ABORTING_VIDEO_MODE		1002
#define MLANG_ABORTING_PRIMARY_SURFACE	1003
#define MLANG_ABORTING_BACKBUFFER		1004
#define MLANG_ABORTING_PIXEL_FORMAT		1005

// Returns a string with the respective language version
char *
mlang_message( int codigo )
{
	// Initialization messages

	if( idioma==0 )	// Castellano
	{
		switch( codigo )
		{
		case MLANG_ABORTING_DD:
			return "No se pudo crear DirectDraw. Abortando.\0";
		case MLANG_ABORTING_EXCLUSIVE:
			return "No se pudo setear modo exclusivo en DirectDraw. Abortando.";
		case MLANG_ABORTING_VIDEO_MODE:
			return "Modo gráfico no soportado. Abortando.";
		case MLANG_ABORTING_PRIMARY_SURFACE:
			return "No se pudo crear la superficie primaria. Abortando.";			
		case MLANG_ABORTING_BACKBUFFER:
			return "No se pudo asociar el backbuffer. Abortando.";
		case MLANG_ABORTING_PIXEL_FORMAT:
			return "No se pudo identificar el formato de pixel. Abortando.";
		default:
			return "Codigo de mensaje desconocido.";
		}
	}
	else	// Ingles, Portugues, Chino
	{
		switch( codigo )
		{
		case MLANG_ABORTING_DD:
			return "Couldn't create DirectDraw. Aborting.";
		case MLANG_ABORTING_EXCLUSIVE:
			return "Couldn't set DirectDraw's exclusive mode. Aborting.";
		case MLANG_ABORTING_VIDEO_MODE:
			return "Unsupported graphical mode. Aborting.";
		case MLANG_ABORTING_PRIMARY_SURFACE:
			return "Couldn't create the primary surface. Aborting.";
		case MLANG_ABORTING_BACKBUFFER:
			return "Couldn't create the backbuffer. Aborting.";
		case MLANG_ABORTING_PIXEL_FORMAT:
			return "Couldn't identify the pixel format. Aborting.";
		default:
			return "Unknown message code.";
		}
	}
}

// Returns a DirectDraw error string
char*
ddReturn(HRESULT error)
{
	switch(error)
	{
	case DD_OK:
		return "No error.\0";
	case DDERR_ALREADYINITIALIZED:
		return "This object is already initialized.\0";
	case DDERR_BLTFASTCANTCLIP:
		return "Return if a clipper object is attached to the source surface passed into a BltFast call.\0";
	case DDERR_CANNOTATTACHSURFACE:
		return "This surface can not be attached to the requested surface.\0";
	case DDERR_CANNOTDETACHSURFACE:
		return "This surface can not be detached from the requested surface.\0";
	case DDERR_CANTCREATEDC:
		return "Windows can not create any more DCs.\0";
	case DDERR_CANTDUPLICATE:
		return "Can't duplicate primary & 3D surfaces, or surfaces that are implicitly created.\0";
	case DDERR_CANTLOCKSURFACE :
		return "Access to this surface is refused because an attempt was made to lock the primary surface without DCI support. \0";
	case DDERR_CANTPAGELOCK :
		return "An attempt to page-lock a surface failed. Page lock does not work on a display-memory surface or an emulated primary surface. \0";
	case DDERR_CANTPAGEUNLOCK :
		return "An attempt to page-unlock a surface failed. Page unlock does not work on a display-memory surface or an emulated primary surface. \0";
	case DDERR_CLIPPERISUSINGHWND:
		return "An attempt was made to set a cliplist for a clipper object that is already monitoring an hwnd.\0";
	case DDERR_COLORKEYNOTSET:
		return "No src color key specified for this operation.\0";
	case DDERR_CURRENTLYNOTAVAIL:
		return "Support is currently not available.\0";
	case DDERR_DEVICEDOESNTOWNSURFACE :
		return "Surfaces created by one DirectDraw device cannot be used directly by another DirectDraw device.\0";
	case DDERR_DIRECTDRAWALREADYCREATED :
		return "A DirectDraw object representing this driver has already been created for this process.\0";
	case DDERR_EXCEPTION:
		return "An exception was encountered while performing the requested operation.\0";
	case DDERR_EXCLUSIVEMODEALREADYSET:
		return "An attempt was made to set the cooperative level when it was already set to exclusive.\0";
	case DDERR_EXPIRED :
		return "The data has expired and is therefore no longer valid.\0";
	case DDERR_GENERIC:
		return "Generic failure.\0";
	case DDERR_HEIGHTALIGN:
		return "Height of rectangle provided is not a multiple of reqd alignment.\0";
	case DDERR_HWNDALREADYSET:
		return "The CooperativeLevel HWND has already been set. It can not be reset while the process has surfaces or palettes created.\0";
	case DDERR_HWNDSUBCLASSED:
		return "HWND used by DirectDraw CooperativeLevel has been subclassed, this prevents DirectDraw from restoring state.\0";
	case DDERR_IMPLICITLYCREATED:
		return "This surface can not be restored because it is an implicitly created surface.\0";
	case DDERR_INCOMPATIBLEPRIMARY:
		return "Unable to match primary surface creation request with existing primary surface.\0";
	case DDERR_INVALIDCAPS:
		return "One or more of the caps bits passed to the callback are incorrect.\0";
	case DDERR_INVALIDCLIPLIST:
		return "DirectDraw does not support the provided cliplist.\0";
	case DDERR_INVALIDDIRECTDRAWGUID:
		return "The GUID passed to DirectDrawCreate is not a valid DirectDraw driver identifier.\0";
	case DDERR_INVALIDMODE:
		return "DirectDraw does not support the requested mode.\0";
	case DDERR_INVALIDOBJECT:
		return "DirectDraw received a pointer that was an invalid DIRECTDRAW object.\0";
	case DDERR_INVALIDPARAMS:
		return "One or more of the parameters passed to the function are incorrect.\0";
	case DDERR_INVALIDPIXELFORMAT:
		return "The pixel format was invalid as specified.\0";
	case DDERR_INVALIDPOSITION:
		return "Returned when the position of the overlay on the destination is no longer legal for that destination.\0";
	case DDERR_INVALIDRECT:
		return "Rectangle provided was invalid.\0";
	case DDERR_INVALIDSTREAM :
		return "The specified stream contains invalid data. \0";
	case DDERR_INVALIDSURFACETYPE :
		return "The requested operation could not be performed because the surface was of the wrong type.\0";
	case DDERR_LOCKEDSURFACES:
		return "Operation could not be carried out because one or more surfaces are locked.\0";
	case DDERR_MOREDATA :
		return "There is more data available than the specified buffer size can hold.\0";
	case DDERR_NO3D:
		return "There is no 3D present.\0";
	case DDERR_NOALPHAHW:
		return "Operation could not be carried out because there is no alpha accleration hardware present or available.\0";
	case DDERR_NOBLTHW:
		return "No blitter hardware present.\0";
	case DDERR_NOCLIPLIST:
		return "No cliplist available.\0";
	case DDERR_NOCLIPPERATTACHED:
		return "No clipper object attached to surface object.\0";
	case DDERR_NOCOLORCONVHW:
		return "Operation could not be carried out because there is no color conversion hardware present or available.\0";
	case DDERR_NOCOLORKEY:
		return "Surface doesn't currently have a color key\0";
	case DDERR_NOCOLORKEYHW:
		return "Operation could not be carried out because there is no hardware support of the destination color key.\0";
	case DDERR_NOCOOPERATIVELEVELSET:
		return "Create function called without DirectDraw object method SetCooperativeLevel being called.\0";
	case DDERR_NODC:
		return "No DC was ever created for this surface.\0";
	case DDERR_NODDROPSHW:
		return "No DirectDraw ROP hardware.\0";
	case DDERR_NODIRECTDRAWHW:
		return "A hardware-only DirectDraw object creation was attempted but the driver did not support any hardware.\0";
	case DDERR_NOEMULATION:
		return "Software emulation not available.\0";
	case DDERR_NOEXCLUSIVEMODE:
		return "Operation requires the application to have exclusive mode but the application does not have exclusive mode.\0";
	case DDERR_NOFLIPHW:
		return "Flipping visible surfaces is not supported.\0";
	case DDERR_NOFOCUSWINDOW :
		return "An attempt was made to create or set a device window without first setting the focus window.\0";
	case DDERR_NOGDI:
		return "There is no GDI present.\0";
	case DDERR_NOHWND:
		return "Clipper notification requires an HWND or no HWND has previously been set as the CooperativeLevel HWND.\0";
	case DDERR_NOMIPMAPHW :
		return "No mipmap-capable texture mapping hardware is present or available. \0";
	case DDERR_NOMIRRORHW:
		return "Operation could not be carried out because there is no hardware present or available.\0";
	case DDERR_NONONLOCALVIDMEM :
		return "An attempt was made to allocate nonlocal video memory from a device that does not support nonlocal video memory. \0";
	case DDERR_NOOPTIMIZEHW :
		return "The device does not support optimized surfaces. \0";
	case DDERR_NOOVERLAYDEST:
		return "Returned when GetOverlayPosition is called on an overlay that UpdateOverlay has never been called on to establish a destination.\0";
	case DDERR_NOOVERLAYHW:
		return "Operation could not be carried out because there is no overlay hardware present or available.\0";
	case DDERR_NOPALETTEATTACHED:
		return "No palette object attached to this surface.\0";
	case DDERR_NOPALETTEHW:
		return "No hardware support for 16 or 256 color palettes.\0";
	case DDERR_NORASTEROPHW:
		return "Operation could not be carried out because there is no appropriate raster op hardware present or available.\0";
	case DDERR_NOROTATIONHW:
		return "Operation could not be carried out because there is no rotation hardware present or available.\0";
	case DDERR_NOSTEREOHARDWARE :
		return "No stereo hardware is present or available.\0";
	case DDERR_NOSTRETCHHW:
		return "Operation could not be carried out because there is no hardware support for stretching.\0";
	case DDERR_NOSURFACELEFT :
		return "No hardware is present that supports stereo surfaces. \0";
	case DDERR_NOT4BITCOLOR:
		return "DirectDrawSurface is not in 4 bit color palette and the requested operation requires 4 bit color palette.\0";
	case DDERR_NOT4BITCOLORINDEX:
		return "DirectDrawSurface is not in 4 bit color index palette and the requested operation requires 4 bit color index palette.\0";
	case DDERR_NOT8BITCOLOR:
		return "DirectDrawSurface is not in 8 bit color mode and the requested operation requires 8 bit color.\0";
	case DDERR_NOTAOVERLAYSURFACE:
		return "Returned when an overlay member is called for a non-overlay surface.\0";
	case DDERR_NOTEXTUREHW:
		return "Operation could not be carried out because there is no texture mapping hardware present or available.\0";
	case DDERR_NOTFLIPPABLE:
		return "An attempt has been made to flip a surface that is not flippable.\0";
	case DDERR_NOTFOUND:
		return "Requested item was not found.\0";
	case DDERR_NOTLOCKED:
		return "Surface was not locked. An attempt to unlock a surface that was not locked at all, or by this process, has been attempted.\0";
	case DDERR_NOTPAGELOCKED :
		return "An attempt was made to page-unlock a surface with no outstanding page locks. \0";
	case DDERR_NOTPALETTIZED:
		return "The surface being used is not a palette-based surface.\0";
	case DDERR_NOVSYNCHW:
		return "Operation could not be carried out because there is no hardware support for vertical blank synchronized operations.\0";
	case DDERR_NOZBUFFERHW:
		return "Operation could not be carried out because there is no hardware support for zbuffer blitting.\0";
	case DDERR_NOZOVERLAYHW:
		return "Overlay surfaces could not be z layered based on their BltOrder because the hardware does not support z layering of overlays.\0";
	case DDERR_OUTOFCAPS:
		return "The hardware needed for the requested operation has already been allocated.\0";
	case DDERR_OUTOFMEMORY:
		return "DirectDraw does not have enough memory to perform the operation.\0";
	case DDERR_OUTOFVIDEOMEMORY:
		return "DirectDraw does not have enough memory to perform the operation.\0";
	case DDERR_OVERLAPPINGRECTS :
		return "The source and destination rectangles are on the same surface and overlap each other. \0";
	case DDERR_OVERLAYCANTCLIP:
		return "The hardware does not support clipped overlays.\0";
	case DDERR_OVERLAYCOLORKEYONLYONEACTIVE:
		return "Can only have ony color key active at one time for overlays.\0";
	case DDERR_OVERLAYNOTVISIBLE:
		return "Returned when GetOverlayPosition is called on a hidden overlay.\0";
	case DDERR_PALETTEBUSY:
		return "Access to this palette is being refused because the palette is already locked by another thread.\0";
	case DDERR_PRIMARYSURFACEALREADYEXISTS:
		return "This process already has created a primary surface.\0";
	case DDERR_REGIONTOOSMALL:
		return "Region passed to Clipper::GetClipList is too small.\0";
	case DDERR_SURFACEALREADYATTACHED:
		return "This surface is already attached to the surface it is being attached to.\0";
	case DDERR_SURFACEALREADYDEPENDENT:
		return "This surface is already a dependency of the surface it is being made a dependency of.\0";
	case DDERR_SURFACEBUSY:
		return "Access to this surface is being refused because the surface is already locked by another thread.\0";
	case DDERR_SURFACEISOBSCURED:
		return "Access to surface refused because the surface is obscured.\0";
	case DDERR_SURFACELOST:
		return "Access to this surface is being refused because the surface memory is gone. The DirectDrawSurface object representing this surface should have Restore called on it.\0";
	case DDERR_SURFACENOTATTACHED:
		return "The requested surface is not attached.\0";
	case DDERR_TOOBIGHEIGHT:
		return "Height requested by DirectDraw is too large.\0";
	case DDERR_TOOBIGSIZE:
		return "Size requested by DirectDraw is too large, but the individual height and width are OK.\0";
	case DDERR_TOOBIGWIDTH:
		return "Width requested by DirectDraw is too large.\0";
	case DDERR_UNSUPPORTED:
		return "Action not supported.\0";
	case DDERR_UNSUPPORTEDFORMAT:
		return "FOURCC format requested is unsupported by DirectDraw.\0";
	case DDERR_UNSUPPORTEDMASK:
		return "Bitmask in the pixel format requested is unsupported by DirectDraw.\0";
	case DDERR_VERTICALBLANKINPROGRESS:
		return "Vertical blank is in progress.\0";
	case DDERR_VIDEONOTACTIVE :
		return "The video port is not active. \0";
	case DDERR_WASSTILLDRAWING:
		return "Informs DirectDraw that the previous Blt which is transfering information to or from this Surface is incomplete.\0";
	case DDERR_WRONGMODE:
		return "This surface can not be restored because it was created in a different mode.\0";
	case DDERR_XALIGN:
		return "Rectangle provided was not horizontally aligned on required boundary.\0";
	default:
		return "Unrecognized error value.\0";
	}
}

// Small utility function to find the LowBit and Number of Bits in a supplied Mask
void
ProcessBits( DWORD Mask, WORD* LowBit, WORD* NumBits )
{
   DWORD TestMask = 1;
   for( *LowBit = 0; *LowBit < 32; ( *LowBit )++ )
   {
      if( Mask & TestMask )
         break;
      TestMask <<= 1;
   }

   TestMask <<= 1;
   for( *NumBits = 1; *NumBits < 32; ( *NumBits )++ )
   {
      if( !( Mask & TestMask ))
         break;
      TestMask <<= 1;
   }
}

// Guarda en las globales correspondientes, el Pixel Format
BOOL
StorePixelFormat()
{
   // Define a PixelFormat variable, clear it and set the dwSize variable, as usual.
   DDPIXELFORMAT DDPixelFormat;
   ZeroMemory( &DDPixelFormat, sizeof( DDPixelFormat ));
   DDPixelFormat.dwSize = sizeof( DDPixelFormat );

   // Fill the PixelFormat from the information for the Primary Surface
   if( FAILED( lpDDSPrimary->GetPixelFormat( &DDPixelFormat)))
   {
      // Fatal error. The program should exit nicely
      return FALSE;
   }

   // Salvo los masks
   RBitMask=DDPixelFormat.dwRBitMask;
   GBitMask=DDPixelFormat.dwGBitMask;
   BBitMask=DDPixelFormat.dwBBitMask;

   // Save the Low Bit and Number of Bits
   ProcessBits( DDPixelFormat.dwRBitMask, &LowRedBit, &NumberRedBits);
   ProcessBits( DDPixelFormat.dwGBitMask, &LowGreenBit, &NumberGreenBits);
   ProcessBits( DDPixelFormat.dwBBitMask, &LowBlueBit, &NumberBlueBits);

   fplog( "\nPixel Format:\nRed:\t%d %d\nGreen:\t%d %d\nBlue:\t%d %d\n\n",
	   LowRedBit, NumberRedBits, LowGreenBit, NumberGreenBits, LowBlueBit, NumberBlueBits);
  
   return TRUE;
}

/* Flips la main surface with the backbuffer */

void
FlipSurfaces(void)
{
    HRESULT ddrval;

	// Si la ejecucion esta suspendida, espero hasta que se reactive
	WaitForSingleObject(task_wakeup_event, INFINITE);

	// dibujo la pantalla
	ddrval = lpDDSPrimary->Flip( NULL, DDFLIP_WAIT );

	if( ddrval == DDERR_SURFACELOST )
		fplog("Primary Surface Lost!\n" );
}

/* ----------------------- Publicas ---------------------- */

/* Initialize video system */
int
initVideoSystem(int width, int height, int bpp)
{
    DDSURFACEDESC2		ddsd;
    DDSCAPS2			ddscaps;
    HRESULT             ddrval;
	DDCAPS				ddCaps;

	/*
     * create the main DirectDraw object
     */
    ddrval = DirectDrawCreateEx( NULL, (void **) &lpDD, IID_IDirectDraw7, NULL );
    if( ddrval != DD_OK )
    {
		// "No se pudo crear DirectDraw. Abortando."
		return initFail(hwnd, mlang_message( MLANG_ABORTING_DD ) );
    }

	// Get DirectDraw HAL capabilities
	ddCaps.dwSize=sizeof(ddCaps);
	lpDD->GetCaps( &ddCaps, NULL );

	// Soporte de aceleracion 3d
	if( ddCaps.dwCaps & DDCAPS_3D )
		fplog( "HAL: Soporta aceleracion 3d\n" );
	else
		fplog( "HAL: No soporta aceleracion 3d\n" );

	// Soporte de alpha-only textures
	if( ddCaps.dwCaps & DDCAPS_ALPHA )
		fplog( "HAL: Soporta alpha-only textures\n" );
	else
		fplog( "HAL: No soporta alpha-only textures\n" );

    // Get exclusive mode
    ddrval = lpDD->SetCooperativeLevel( hwnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_MULTITHREADED );
    if( ddrval != DD_OK )
    {
		// "No se pudo setear modo exclusivo. Abortando."
		return initFail(hwnd, mlang_message( MLANG_ABORTING_EXCLUSIVE ) );
    }

    // Set the video mode
	ddrval = lpDD->SetDisplayMode( width, height, bpp*8, 0, 0 );
    if( ddrval != DD_OK )
    {
		// "Modo gráfico no soportado. Abortando."
		return initFail(hwnd, mlang_message( MLANG_ABORTING_VIDEO_MODE ) ); 
    }

    // Create the primary surface with 1 back buffer
	ZeroMemory( &ddsd, sizeof( ddsd ) );
    ddsd.dwSize = sizeof( ddsd );
    ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
			  DDSCAPS_FLIP |
			  DDSCAPS_COMPLEX |
			  DDSCAPS_3DDEVICE;
    ddsd.dwBackBufferCount = 1;
    ddrval = lpDD->CreateSurface( &ddsd, &lpDDSPrimary, NULL );
    if( ddrval != DD_OK )
    {
		// "No se pudo crear la superficie primaria. Abortando."
		return initFail(hwnd, mlang_message( MLANG_ABORTING_PRIMARY_SURFACE ) );
    }

	// Create the backbuffer
	ZeroMemory( &ddscaps, sizeof( ddscaps ) );
    ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
	ddrval = lpDDSPrimary->GetAttachedSurface(&ddscaps, &vscreen);
    if( ddrval != DD_OK )
    {
		// "No se pudo asociar el backbuffer. Abortando."
		return initFail(hwnd, mlang_message( MLANG_ABORTING_BACKBUFFER ) );
    }

	// Set pixel format globals
	if( FAILED(StorePixelFormat() ) )
	{
		// "No se pudo identificar el formato de pixel. Abortando." 
		return initFail(hwnd, mlang_message( MLANG_ABORTING_PIXEL_FORMAT) );
	}

	// Check primary display capabilities

	// Veo cuanta vidmem tengo
	unsigned long dwTotal, dwFree;

	ZeroMemory( &ddscaps, sizeof( ddscaps ) );
	ddscaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY |
					DDSCAPS_LOCALVIDMEM;

	lpDD->GetAvailableVidMem( &ddscaps, &dwTotal, &dwFree );

	fplog( "Vidmem Total: %ld, Free: %ld\n", dwTotal, dwFree );
	
	// Clear v_vscreen
	FillScreen( Make16(0,0,0));
	display_vscreen();

	return 0;
}

/* Uninit the video system */

void
unInitVideoSystem(void)
{
    HRESULT ddrval;

	fplog( "INI unInitVideoSystem\n" );
	
	// Close DirectDraw
	if( lpDD != NULL )
    {
		// Set non-exclusive mode for display 
		ddrval=lpDD->SetCooperativeLevel( hwnd, DDSCL_NORMAL );
		fplog( "SetCooperativeLevel %s\n", ddReturn( ddrval ) );

		// Set previous display mode
		ddrval=lpDD->RestoreDisplayMode();
		fplog( "RestoreDisplayMode %s\n", ddReturn( ddrval ) );

		// Make our exit elegant
		lpDD->FlipToGDISurface(); 

		if( lpDDSPrimary != NULL )
		{
			lpDDSPrimary->Release();
			lpDDSPrimary = NULL;
		}

		lpDD->Release();
		lpDD = NULL;
    }

	fplog( "END unInitVideoSystem\n" );
}

/*
 * restoreAll
 *
 * restore all vidmem lost objects
 */
int
restoreAll( void )
{
	int iRet;
	int i;
	int n;
	CallbackT Cback;

	iRet = (int) lpDD->RestoreAllSurfaces();

	// Reload all graphics that must be reload
	fplog( "Recargo graficos\n" );

	n = gReloadSurfacesStack.StackDepth();
	for( i=n-1; i>=0; i-- )
	{
		gReloadSurfacesStack.GetStackElement( i, &Cback );

		(* (void (*)(void *))Cback.func)( Cback.data );
	}

	fplog( "fin Recargo graficos\n" );

	return iRet;
}

/* WaitRetrace (not used) */

void
WaitRetrace()
{
	lpDD->WaitForVerticalBlank( DDWAITVB_BLOCKBEGIN, NULL );
}

/* This copies a window of the vscreen to the screen. */

int
display_vscreen( int frame_duration )
{
	// Wait to fill in time
	FillTime( frame_duration );

	// Flip the backbuffer
    FlipSurfaces();

	return 0;
}

/* Copies into vscreen the primary surface contents */

void
restore_vscreen(void)
{
	vscreen->BltFast( 0, 0, lpDDSPrimary, NULL, DDBLTFAST_WAIT );
}

/* -------------- Etc Functions --------------- */

// Not enough memory function - only fatal errors
void
chau( void )
{
	fplog( "Error, not enough memory.\n" );
	exit(1);
}