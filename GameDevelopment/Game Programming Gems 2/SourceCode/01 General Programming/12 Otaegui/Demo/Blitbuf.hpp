/****************************************************************************
 * Blitbuf.hpp - BlitBuf Functions V.2.14.002
 * (c) Copyright 1996-2000 Sabarasa Entertainment
 * For internal use only.
 ****************************************************************************/               

#ifndef blitbuf_h
#define blitbuf_h

#include <ddraw.h>
#include "support.hpp"

/* ------------------------------ Globals --------------------------------- */
typedef struct
{
	int ysize;
	int xsize;
	int insystem;
	LPDIRECTDRAWSURFACE7 surface;
} blitbuf;

/* ------------------------- BlitBuf Functions ---------------------------- */
/* General Functions */
void clear_blitbuf(blitbuf *buf);
void fill_blitbuf(pixelT color, blitbuf *buf);
void alloc_blitbuf(blitbuf *buf, unsigned short xsize, unsigned short ysize, unsigned int donde=0);

/* Copying Functions */
void blitbuf2vs(blitbuf *buf, unsigned short x1, unsigned short y1, 
		unsigned short x2, unsigned short y2,
		unsigned short destx, unsigned short desty);
void vs2blitbuf(blitbuf *buf, signed short x, signed short y);
void display_blitbuf(blitbuf *buf, signed short x1, signed short y1);
void t_blitbuf(blitbuf *buf, signed short x1, signed short y1);
void s_blitbuf(blitbuf *buf, signed short x1, signed short y1);
void m_blitbuf(blitbuf *buf, signed short x1, signed short y1, pixelT col);

void copy_blitbuf(blitbuf *source, blitbuf *dest, 
		unsigned short x1, unsigned short y1, 	
		unsigned short x2, unsigned short y2,
		unsigned short destx, unsigned short desty);
void new_blitbuf(blitbuf *source, blitbuf *dest,
		unsigned short x1, unsigned short y1,
		unsigned short x2, unsigned short y2);

/* Blitbuf processing Function */
int load_blitbufPCX(char *fname, blitbuf *buf);
int allocload_blitbufPCX(char *fname, blitbuf *buf);
void save_blitbufPCX(char *fname, blitbuf *buf);
void flip_vertical_blitbuf(blitbuf *buf);
void flip_horizontal_blitbuf(blitbuf *buf);

#endif