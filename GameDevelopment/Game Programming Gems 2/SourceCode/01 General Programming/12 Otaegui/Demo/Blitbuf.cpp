/************************************************************
 * Blitbuf.cpp - BlitBuf Functions V2.16.001 - HiColor
 *
 * (c) Copyright 1996-2000 Sabarasa Entertainment
 * For internal use only.					
 ************************************************************/               

/* 
	'When you steal from one author, it's plagiarism; if you steal from many, it's research.'  

													-Wilson Mizner
*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <stdlib.h>
#include <stdarg.h>

#include <dos.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <conio.h>

#include "mymalloc.h" 
#include "fplog.h"

#include "blitbuf.hpp"
#include "support.hpp"
#include "prim.hpp"

/* Pure Color LUTs para load_blitbufPCX() */

static int LUTinicializado=0;
static pixelT PureRedLUT[256];
static pixelT PureGreenLUT[256];
static pixelT PureBlueLUT[256];

/* ----- Funciones privadas */

/* Pone un pixel en un blitbuf - Poco Eficiente */

void
blitbuf_putPixel(blitbuf *buf, int x, int y, pixelT color)
{
	RECT rect;
	DDBLTFX ddbltfx;

	rect.left=x;
	rect.top=y;
	rect.right=x+1;
	rect.bottom=y+1;

	ddbltfx.dwSize = sizeof( ddbltfx );
	ddbltfx.dwFillColor = color;

	buf->surface->Blt( &rect, NULL, NULL,
		DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx );
}

/* Inicializa los LUTs que se usan en load_blitbufPCX() */
void
initLUTs( void )
{
	int x;

	fplog( "initLUTs()\n" ); 

	for( x=0; x<256; x++ )
	{
		PureRedLUT[x]=Make16(x,0,0);
		PureGreenLUT[x]=Make16(0,x,0);
		PureBlueLUT[x]=Make16(0,0,x);
	}

	LUTinicializado=1;
}

/* -------------- Public BlitBuf Functions */

/* Limpia y borra el blitbuf */

void
clear_blitbuf(blitbuf *buf) 
{
	if( buf->surface )
	{
		buf->surface->Release();
		buf->surface=NULL;
	}
}

/* Llena el blitbuf con determinado color */

void
fill_blitbuf(pixelT color, blitbuf *buf)
{
	DDBLTFX ddbltfx;

	ddbltfx.dwSize= sizeof( ddbltfx );
	ddbltfx.dwFillColor=color;

	buf->surface->Blt( NULL, NULL, NULL,
		DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx );    
}

// alloc_blitbuf
// Creamos el nuevo BlitBuf
//
// donde	0 en vidmem
//			1 en sysmem

void
alloc_blitbuf(blitbuf *buf, unsigned short xsize, unsigned short ysize,
			  unsigned int donde ) 
{
    DDSURFACEDESC2		ddsd;
	HRESULT				ddrval;
	DDSCAPS2			ddscaps;

	if( donde==0 ) // En vidmem
	{
		// Creo la superficie principal
		ZeroMemory( &ddsd, sizeof(ddsd) );
	    ddsd.dwSize = sizeof( ddsd );
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_CKSRCBLT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		ddsd.dwHeight=ysize;
		ddsd.dwWidth=xsize;

		ddsd.ddckCKSrcBlt.dwColorSpaceLowValue=0;
		ddsd.ddckCKSrcBlt.dwColorSpaceHighValue=0;
    
		ddrval = lpDD->CreateSurface( &ddsd, &(buf->surface), NULL );
		if( ddrval != DD_OK )
	    {
			chau();
		}
	}
	else		
	{
		// Creo la superficie en sysmem
		ZeroMemory( &ddsd, sizeof(ddsd) );
	    ddsd.dwSize = sizeof( ddsd );
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_CKSRCBLT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
		ddsd.dwHeight=ysize;
		ddsd.dwWidth=xsize;

		ddsd.ddckCKSrcBlt.dwColorSpaceLowValue=0;
		ddsd.ddckCKSrcBlt.dwColorSpaceHighValue=0;
    
		ddrval = lpDD->CreateSurface( &ddsd, &(buf->surface), NULL );
		if( ddrval != DD_OK )
		{
			chau();
		}
	}

	buf->surface->GetSurfaceDesc( &ddsd );

//	fplog( "Superficie creada en %s Memory: %d\n", (ddsd.ddsCaps.dwCaps&DDSCAPS_SYSTEMMEMORY)? "System":"Video",
//		xsize*ysize*sizeof(pixelT) );
	buf->xsize=xsize;
	buf->ysize=ysize;
	buf->insystem=ddsd.ddsCaps.dwCaps&DDSCAPS_SYSTEMMEMORY;

	// Veo cuanta vidmem tengo
	unsigned long dwTotal, dwFree;

	ZeroMemory( &ddscaps, sizeof( ddscaps ) );
	ddscaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY |
					DDSCAPS_LOCALVIDMEM;

	lpDD->GetAvailableVidMem( &ddscaps, &dwTotal, &dwFree );

//	fplog( "Vidmem Total: %ld, Free: %ld\n", dwTotal, dwFree );
}

/* Copia seccion del BlitBuf a virtual screen */

void
blitbuf2vs(blitbuf *buf, unsigned short x1, unsigned short y1, 	
	unsigned short x2, unsigned short y2,
	unsigned short destx, unsigned short desty)
{
	RECT source;

	source.left=x1;
	source.top=y1;
	source.bottom=y2;
	source.right=x2;

	vscreen->BltFast( destx, desty, buf->surface, &source, DDBLTFAST_WAIT );
}	

/* Copia seccion del Virtual Screen a un Blitbuf */

void
vs2blitbuf(blitbuf *buf, signed short x, signed short y)
{
	RECT source;

	source.left=x;
	source.top=y;
	source.bottom=y+buf->ysize;
	source.right=x+buf->xsize;

	buf->surface->BltFast( 0, 0, vscreen, &source, DDBLTFAST_WAIT );
}

/* Copia BlitBuf a virtual screen */

void
display_blitbuf(blitbuf *buf, signed short x1, signed short y1)
{
	RECT r_origen;
	RECT r_dest;

	r_dest.left=x1;
	r_dest.top=y1;
	r_dest.right=x1+buf->xsize;
	r_dest.bottom=y1+buf->ysize;

	r_origen.left=0;
	r_origen.top=0;
	r_origen.right=buf->xsize;
	r_origen.bottom=buf->ysize;

	if( x1<0 )
	{
		r_dest.left=0;
		r_origen.left=(-x1);
	}
	if( y1<0 )
	{
		r_dest.top=0;
		r_origen.top=(-y1);
	}
	if( (x1+buf->xsize)>PhysicalWidth )
	{
		r_dest.right=PhysicalWidth;
		r_origen.right=PhysicalWidth-x1;
	}
	if( (y1+buf->ysize)>PhysicalHeight )
	{
		r_dest.bottom=PhysicalHeight;
		r_origen.bottom=PhysicalHeight-y1;
	}

	vscreen->Blt( &r_dest, buf->surface, &r_origen, DDBLT_WAIT, NULL );
}

/* Copia BlitBuf a virtual screen TRANSPARENTE con el color 0 */

void
t_blitbuf(blitbuf *buf, signed short x1, signed short y1)
{
	RECT r_origen;
	RECT r_dest;

	r_dest.left=x1;
	r_dest.top=y1;
	r_dest.right=x1+buf->xsize;
	r_dest.bottom=y1+buf->ysize;

	r_origen.left=0;
	r_origen.top=0;
	r_origen.right=buf->xsize;
	r_origen.bottom=buf->ysize;

	if( x1<0 )
	{
		r_dest.left=0;
		r_origen.left=(-x1);
	}
	if( y1<0 )
	{
		r_dest.top=0;
		r_origen.top=(-y1);
	}
	if( (x1+buf->xsize)>PhysicalWidth )
	{
		r_dest.right=PhysicalWidth;
		r_origen.right=PhysicalWidth-x1;
	}
	if( (y1+buf->ysize)>PhysicalHeight )
	{
		r_dest.bottom=PhysicalHeight;
		r_origen.bottom=PhysicalHeight-y1;
	}

	vscreen->Blt( &r_dest, buf->surface, &r_origen, DDBLT_WAIT | DDBLT_KEYSRC, NULL );
}

/* Copia BlitBuf a virtual screen TRANSPARENTE con el color 0 a mitad de tamaño */
/* ESTA FUNCION NO ANDA BIEN EN LOS BORDES!!!!! */

void
s_blitbuf(blitbuf *buf, signed short x1, signed short y1)
{
	RECT dest;

	dest.top=y1;
	dest.left=x1;
	dest.bottom=y1+buf->ysize/2;
	dest.right=x1+buf->xsize/2;

	vscreen->Blt( &dest, buf->surface, NULL, DDBLT_WAIT | DDBLT_KEYSRC, NULL );
}

/* Copia BlitBuf a virtual screen enmascarado con el color col */

void
m_blitbuf(blitbuf *buf, signed short x1, signed short y1, pixelT col)
{
	DDSURFACEDESC2 origen, dest;
	pixelT *buf_origen, *buf_dest;
	int deltax, deltay;

	// Defino limites
	RECT r_origen;
	RECT r_dest;

	r_dest.left=x1;
	r_dest.top=y1;
	r_dest.right=x1+buf->xsize;
	r_dest.bottom=y1+buf->ysize;

	r_origen.left=0;
	r_origen.top=0;
	r_origen.right=buf->xsize;
	r_origen.bottom=buf->ysize;

	if( x1<0 )
	{
		r_dest.left=0;
		r_origen.left=(-x1);
	}
	if( y1<0 )
	{
		r_dest.top=0;
		r_origen.top=(-y1);
	}
	if( (x1+buf->xsize)>PhysicalWidth )
	{
		r_dest.right=PhysicalWidth;
		r_origen.right=PhysicalWidth-x1;
	}
	if( (y1+buf->ysize)>PhysicalHeight )
	{
		r_dest.bottom=PhysicalHeight;
		r_origen.bottom=PhysicalHeight-y1;
	}

	deltax=r_origen.right-r_origen.left;
	deltay=r_origen.bottom-r_origen.top;

	// Preparo variables
	ZeroMemory( &origen, sizeof( origen ) );
	origen.dwSize=sizeof(origen);

	ZeroMemory( &dest, sizeof( dest ) );
	dest.dwSize=sizeof(dest);

	// Lockeo superficies
	if(buf->surface->Lock( &r_origen, &origen, DDLOCK_WAIT, NULL )!=DD_OK)
		return;
	if(vscreen->Lock( &r_dest, &dest, DDLOCK_WAIT, NULL )!=DD_OK)
		return;

	// Tomo punteros
	buf_dest=(pixelT *)dest.lpSurface;
	buf_origen=(pixelT *)origen.lpSurface;

	// Armo los pixels correspondientes
	for( int y=0; y<deltay; y++ )
	{
		for( int x=0; x<deltax; x++ )
		{
			if( *(buf_origen+x) )
				*(buf_dest+x)=col;
		}
		buf_dest+=dest.lPitch/sizeof(pixelT);
		buf_origen+=origen.lPitch/sizeof(pixelT);
	}

	// Deslockeo superficies
	buf->surface->Unlock( &r_origen );
	vscreen->Unlock( &r_dest );
}

/* Copia seccion del BlitBuf a otra seccion de otro BlitBuf */

void
copy_blitbuf(blitbuf *source, blitbuf *dest, 
		unsigned short x1, unsigned short y1, 	
		unsigned short x2, unsigned short y2,
		unsigned short destx, unsigned short desty)
{
	RECT rect;

	rect.left=x1;
	rect.top=y1;
	rect.right=x2;
	rect.bottom=y2;

	if( (dest->xsize-destx-1)<x2 )
		x2=dest->xsize-destx-1;
	if( (dest->ysize-desty-1)<y2 )
		y2=dest->ysize-desty-1;

	// Copio de vidmem a vidmem
	dest->surface->BltFast( destx, desty, source->surface, &rect, DDBLTFAST_WAIT );
}	
		
/* Copia seccion del BlitBuf a otro BlitBuf nuevo */
/* Los extremos x2 e y2 no entran!!!!! */

void
new_blitbuf(blitbuf *source, blitbuf *dest,
		unsigned short x1, unsigned short y1,
		unsigned short x2, unsigned short y2) 
{
	alloc_blitbuf(dest, (x2-x1), (y2-y1) );				// Seria correcto sumar 1 para que entren los bordes bien
	copy_blitbuf(source, dest, x1, y1, x2, y2, 0, 0);
}

/* Salva un BlitBuf en un PCX */
/* NO IMPLEMENTADA */
void
save_blitbufPCX(char *fname, blitbuf *buf)
{
/*    FILE *fp;
    unsigned int i, size, temp_int;
    BYTE VGA_pal[768];
	BYTE VGA_pal2[1024];
    BYTE *buf_ptr;
    BYTE temp_char, match, count;

    fp = fopen(fname, "wb");

    if (fp != NULL) {
        // Write manufacturer's byte
        temp_char = 10;
        fwrite(&temp_char, 1, 1, fp);

        // Write version of PCX.  5 = 256 color (PCX Version 5.0)
        temp_char = 5;
        fwrite(&temp_char, 1, 1, fp);

        // Write encoding type, always 1 for RLE.
        temp_char = 1;
        fwrite(&temp_char, 1, 1, fp);

        // Write bits_per_pixel = 8.
        temp_char = 8;
        fwrite(&temp_char, 1, 1, fp);

        // Write starting X and Y coords
        temp_int = 0;
        fwrite(&temp_int, 2, 1, fp);
        fwrite(&temp_int, 2, 1, fp);

        // Write X size
        temp_int = (buf->xsize - 1);
        fwrite(&temp_int, 2, 1, fp);

        // Write Y size
        temp_int = (buf->ysize - 1);
        fwrite(&temp_int, 2, 1, fp);

        // Do HRES and VRES **
        temp_int = buf->xsize;
        fwrite(&temp_int, 2, 1, fp);
        temp_int = buf->ysize;
        fwrite(&temp_int, 2, 1, fp);

        // Write 16 color palette, not used.
        temp_int = 0;
        i=24;
        while (i--) {
            fwrite(&temp_int, 2, 1, fp);
        }

        // Write vmode byte.
        temp_char = 0;
        fwrite(&temp_char, 1, 1, fp);

        // Write bit_planes
        temp_char = 1;
        fwrite(&temp_char, 1, 1, fp);

        // Write bytes_per_line
        temp_int = buf->xsize;
        fwrite(&temp_int, 2, 1, fp);

        // Write palette type
        temp_int = 1;
        fwrite(&temp_int, 2, 1, fp);

        // Write junk filler
        temp_int = 0;
        i=29;
        while (i--) {
            fwrite(&temp_int, 2, 1, fp);
        }

        // Write the actual image
        buf_ptr = buf->image;
        size = (buf->xsize * buf->ysize);

        count = 0;
        match = *buf_ptr;

        i=size;
        while (i--) {
            temp_char = *buf_ptr++;

            if ((temp_char == match) && (count < 63)) {
               count++;
            } else {
                if ((count == 1) && (match < 192)) {
                    // Write single byte
                    fwrite(&match,1,1,fp);
                } else {
                    // Write run of pixels
                    count += 192;
                    fwrite(&count, 1, 1, fp);
                    fwrite(&match, 1, 1, fp);
                }
                count = 1;
                match = temp_char;
            }
        }

        if ((count == 1) && (match < 192)) {
            // Write single byte
            fwrite(&match,1,1,fp);
        } else {
            // Write run of pixels
            count += 192;
            fwrite(&count, 1, 1, fp);
            fwrite(&match, 1, 1, fp);
        }

        // Write palette verification byte
        temp_char = 12;
        fwrite(&temp_char, 1, 1, fp);

		GetPalette( (LPPALETTEENTRY) VGA_pal2 );

		for( int x=0; x<256; x++ )
		{
			VGA_pal[x*3]=VGA_pal2[x*4];
			VGA_pal[x*3+1]=VGA_pal2[x*4+1];
			VGA_pal[x*3+2]=VGA_pal2[x*4+2];
		}
//			fwrite(VGA_pal+x*4, 1, 3, fp);
  
		
//		get_paletteX(VGA_pal);
    
		// Write 256 color palette

        
		fwrite(VGA_pal, 1, 768, fp);

        fclose(fp);
    }
*/
}

/* Carga un PCX en un BlitBuf */

int
load_blitbufPCX(char *fname, blitbuf *buf) 
{

    FILE *fp;
    int size;
    unsigned char VGA_pal[1024];
    unsigned char PCX_byte, RLE_byte;
	unsigned char *image;
    unsigned char *buf_ptr;
    unsigned char *end_of_buf;
	int r, g, b, act;

	if( !LUTinicializado )
		initLUTs();

	fplog( "loadPCX: %s\n", fname );

    fp = fopen(fname, "rb");

    if (fp == NULL) 
    {
		fplog( "ERROR: No se encontro %s\n", fname );

        return 0;
    } 
    else 
    {
        fseek(fp, 8, SEEK_SET);
        fread(&buf->xsize, 2, 1, fp);
        fread(&buf->ysize, 2, 1, fp);

        buf->xsize++;
        buf->ysize++;

		fplog( "load xsize %d ysize %d\n", buf->xsize, buf->ysize );

        size = (buf->xsize * buf->ysize);

		// Seteo los pointers
        image = (unsigned char *) mymalloc( size );
		if( image==NULL ) return 0;

		buf_ptr=image;
        end_of_buf = buf_ptr + size;

        // Load 256 color PCX palette
        fseek(fp, -768, SEEK_END);
		for( int i=0; i<256; i++)
			fread(VGA_pal+i*4, 1, 3, fp);

        fseek(fp, 128, SEEK_SET);

        while (buf_ptr < end_of_buf)
		{
            // Read next packet
            fread(&PCX_byte, 1, 1, fp);

            if (PCX_byte < 192)
			{
                // Raw Pixel
                *buf_ptr++ = PCX_byte;
            }
			else
			{
                // RLE Pixels
                PCX_byte = (PCX_byte & 0x3F);
                fread(&RLE_byte, 1, 1, fp);
                memset(buf_ptr, RLE_byte, PCX_byte);
                buf_ptr += PCX_byte;
            }
        }
        fclose(fp);

		DDSURFACEDESC2 ddsd;
		pixelT *buffer;

		ZeroMemory( &ddsd, sizeof(ddsd) );
		ddsd.dwSize=sizeof(ddsd);
		if( buf->surface->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL ) )
			return 0;

		buffer=(pixelT *)ddsd.lpSurface;

		// Convierto buf_ptr en una superficie
		for( int y=0; y<(int)ddsd.dwHeight; y++ )
		{
			for( int x=0; x<(int)ddsd.dwWidth; x++ )
			{
				act=*(image+y*buf->xsize+x);
				r=(int) *(VGA_pal+act*4);
				g=(int) *(VGA_pal+act*4+1);
				b=(int) *(VGA_pal+act*4+2);

				*(buffer+x)=PureRedLUT[r] | PureGreenLUT[g] | PureBlueLUT[b];
			}
			buffer+=(ddsd.lPitch/sizeof(pixelT));
		}

		buf->surface->Unlock( NULL );

		free( image );

        return 1;
    }
}

/* Carga un PCX en un BlitBuf y lo aloca */

int
allocload_blitbufPCX(char *fname, blitbuf *buf) 
{
    FILE *fp;
    int size;
    unsigned char VGA_pal[1024];
    unsigned char PCX_byte, RLE_byte;
	unsigned char *image;
    unsigned char *buf_ptr;
    unsigned char *end_of_buf;
	int r, g, b, act;
	unsigned short xs, ys;

	if( !LUTinicializado )
		initLUTs();

	fplog( "allocloadPCX: %s\n", fname );

	// Abro el PCX y guardo en *image
    fp = fopen(fname, "rb");

    if (fp == NULL) 
    {
		fplog( "ERROR: No se encontro %s\n", fname );

        return 0;
    } 
    else 
    {
        fseek(fp, 8, SEEK_SET);
        fread(&xs, 2, 1, fp);
        fread(&ys, 2, 1, fp);

        xs++;
        ys++;

		fplog( "load xsize %d ysize %d\n", xs, ys );

        size = (xs * ys);

		// Seteo los pointers
        image = (unsigned char *) mymalloc( size, "allocload_blitbufPCX->image" );
		if( image==NULL ) return 0;

		buf_ptr=image;
        end_of_buf = buf_ptr + size;

        // Load 256 color PCX palette
        fseek(fp, -768, SEEK_END);
		for( int i=0; i<256; i++)
			fread(VGA_pal+i*4, 1, 3, fp);

        fseek(fp, 128, SEEK_SET);

        while (buf_ptr < end_of_buf)
		{
            // Read next packet
            fread(&PCX_byte, 1, 1, fp);

            if (PCX_byte < 192)
			{
                // Raw Pixel
                *buf_ptr++ = PCX_byte;
            }
			else
			{
                // RLE Pixels
                PCX_byte = (PCX_byte & 0x3F);
                fread(&RLE_byte, 1, 1, fp);
                memset(buf_ptr, RLE_byte, PCX_byte);
                buf_ptr += PCX_byte;
            }
        }
        fclose(fp);

		// Creo el nuevo blitbuf
		alloc_blitbuf( buf, xs, ys );
		
		// Convierto a superficies
		DDSURFACEDESC2 ddsd;
		pixelT *buffer;

		ZeroMemory( &ddsd, sizeof(ddsd) );
		ddsd.dwSize=sizeof(ddsd);
		if( buf->surface->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL ) )
			return 0;

		buffer=(pixelT *)ddsd.lpSurface;

		// Convierto buf_ptr en una superficie
		for( int y=0; y<(int)ddsd.dwHeight; y++ )
		{
			for( int x=0; x<(int)ddsd.dwWidth; x++ )
			{
				act=*(image+y*buf->xsize+x);
				r=(int) *(VGA_pal+act*4);
				g=(int) *(VGA_pal+act*4+1);
				b=(int) *(VGA_pal+act*4+2);

				*(buffer+x)=PureRedLUT[r] | PureGreenLUT[g] | PureBlueLUT[b];
			}
			buffer+=(ddsd.lPitch/sizeof(pixelT));
		}

		buf->surface->Unlock( NULL );

		myfree( image, "allocload_blitbufPCX->image" );

        return 1;
    }
}
               
/* Da vuelta un blitbuf verticalmente */
/* NO IMPLEMENTADA */

void
flip_vertical_blitbuf(blitbuf *buf)
{
}

/* Da vuelta un blitbuf horizontalmente */
/* NO IMPLEMENTADA */

void
flip_horizontal_blitbuf(blitbuf *buf)
{
}
