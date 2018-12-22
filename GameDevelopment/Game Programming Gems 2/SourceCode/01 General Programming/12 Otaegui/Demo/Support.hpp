/************************************************************
 * Support.hpp - Support Functions Header File V2.11.002
 * (c) Copyright 1996-2000 Sabarasa Entertainment
 * For internal use only.
 ************************************************************/               

#ifndef support_h
#define support_h

#include <ddraw.h>

/*-------------- Macros, constantes y tipos ------------------ */

#define Make16(Red,Green,Blue) ((((Red)	>> (8 - NumberRedBits )) << LowRedBit ) \
	   | (((Green)	>> (8 - NumberGreenBits )) << LowGreenBit) \
	   | (((Blue)   >> (8 - NumberBlueBits )) << LowBlueBit))

#define FRAME_DURATION	0  // (en mseg) -> 30 frames por segundo -> 33

typedef unsigned short pixelT;

/*------------------------------- Globals ----------------------------------*/

extern LPDIRECTDRAW7			lpDD;
extern LPDIRECTDRAWSURFACE7		lpDDSPrimary;
extern LPDIRECTDRAWSURFACE7		vscreen;
extern LPDIRECTDRAWGAMMACONTROL	lpDDGammaControl;
extern LPDIRECTDRAWCOLORCONTROL	lpDDColorControl;

extern int PhysicalHeight, PhysicalWidth;
extern int PhysicalBPP;

// These variables are set in StorePixelFormat and should be defined
// elsewhere(global, member variable, etc...)
extern WORD NumberRedBits, NumberGreenBits, NumberBlueBits;
extern WORD LowRedBit, LowGreenBit, LowBlueBit;
extern DWORD RBitMask, GBitMask, BBitMask;

/*------------------------- Interface Functions ----------------------------*/

void delay( int time );
void FillTime( int interval );

int initVideoSystem(int width, int height, int bpp);
void unInitVideoSystem(void);
int restoreAll( void );

int display_vscreen( int frame_duration=FRAME_DURATION );
void restore_vscreen(void);
void WaitRetrace(void);

/*------------------- Error Handling Functions -------------------*/
char* ddReturn(HRESULT error);
void chau(void);

#endif