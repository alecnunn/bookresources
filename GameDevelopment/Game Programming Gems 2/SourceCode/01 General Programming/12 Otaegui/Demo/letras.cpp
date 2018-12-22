/************************************************************
 * letras.cpp - Font Functions V1.06.001
 *
 * (c) Copyright 1999-2000 Sabarasa Entertainment		        
 * For internal use only.					
 ************************************************************/               

#define FONT_FUNCTIONS

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <mmsystem.h>

#include "blitbuf.hpp"
#include "prim.hpp"
#include "support.hpp"

#include "letras.hpp"
#include "fplog.h"

//--------------------------------- Sistema de Fuentes -----------------------------

struct FUENTE
{
	int height;
	int promediowidth; // en desuso  
	
	blitbuf * letras[256];
	FUENTE(char *fname = NULL);
};

#define COLORDIVISOR (Make16(255,0,0)) // que color es el divisor dentro del pcx

// Carga la fuente desde el pcx a la struct
void
LoadLetras(FUENTE & fuente, char * pcxfontfilename)
{
	int i , i2;
	blitbuf tempcx;
	DDSURFACEDESC2 ddsd;
	pixelT *image;
	int x1[256], y1[256], xo[256], yo[256];

	fplog( "ini LoadLetras\n" );
	
	// Cargamos el pcx con la fuente
	allocload_blitbufPCX(pcxfontfilename, &tempcx);
	
	// Anotamos el alto de la letra
	fuente.height = (tempcx.ysize-16)/16;

	// Lockeamos la superficie
	ddsd.dwSize=sizeof( ddsd );
	tempcx.surface->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL );

	//Ahora seteamos algunas variables para el proceso...
	image = (pixelT *) ddsd.lpSurface;
	int widthdetecter = 0;
	int currentwidth;
	
	int curline = 0;

	// Para cada pointer en la fuente
	for(i = 0; i < 256; i++)
	{

		if( !( i % 16) ) // si completo una linea pasamos a la siguiente
		{
			widthdetecter = curline * (ddsd.lPitch/sizeof(pixelT)) * (fuente.height+1);
			curline++;
		}	

		// Anotamos las coordenadas de origen de la letra
		x1[i]=widthdetecter%(ddsd.lPitch/sizeof(pixelT));
		y1[i]=widthdetecter/(ddsd.lPitch/sizeof(pixelT));

		// Obtenemos el ancho de la letra 
		currentwidth = 0;
		i2 = x1[i];

		while( image[widthdetecter] != COLORDIVISOR)
		{
			widthdetecter++;
			currentwidth++;
			if(i2++ > tempcx.xsize) 
				break;
		}

		widthdetecter++; //prepara el detector para el sig loop		

		//Anotamos el ancho de la letra
		//Anotamos el alto de la letra (que es fijo)
		xo[i]=currentwidth;
		yo[i]=fuente.height;
		
	} //end for

	tempcx.surface->Unlock( NULL );

	for( i=0; i<256; i++ )
	{
		// Si no existe de antemano
		if( fuente.letras[i]==NULL )
		{
			//Si el ancho es 0 , nulleamos el pointer y vamos al siguiente loop
			if(!xo[i])
			{
				fuente.letras[i] = NULL;
				continue; //vamos al siguiente loop
			}
		
			//Si no, Malloqueamos el espacio para la blitbuf struct 
			fuente.letras[i] = new blitbuf;
		}
		else
		{
			//Si el ancho es 0 , nulleamos el pointer y vamos al siguiente loop
			if(!xo[i])
			{
				fuente.letras[i] = NULL;
				continue; //vamos al siguiente loop
			}
		
			//Si no, Malloqueamos el espacio para la blitbuf struct 
			clear_blitbuf(fuente.letras[i]);
		}

		alloc_blitbuf( fuente.letras[i], xo[i], yo[i], 0 );

		copy_blitbuf( &tempcx, fuente.letras[i], x1[i], y1[i], x1[i]+xo[i], y1[i]+yo[i], 0, 0 );
	}


	//Borramos de la memoria el tempcx
	clear_blitbuf(&tempcx);

	fplog( "fin LoadLetras\n" );
}


//Autoinicializador de una fuente
FUENTE::FUENTE(char *fname)
{
	int i=0;
	FUENTE &curfont = *this;

	for( i=0; i<256; i++ )
		curfont.letras[i]=NULL;

	if(fname)
		  LoadLetras( curfont, fname);		
}

//Pone una letra en la vscreen con la determinada fuente devolviendo el ancho
//Si la fuente no tiene el caracter devuelve 0
int
VGAPutChar(FUENTE & fuente, unsigned char character, signed short x1, signed short y1, pixelT col)
{
	if(fuente.letras[character])
	{
//		m_blitbuf(fuente.letras[character], x1, y1,col);
		t_blitbuf(fuente.letras[character], x1, y1);
		return fuente.letras[character]->xsize;
	}
	else
		return 0;
}

void
VGAPutString (FUENTE & fuente, char *string, short x, short y, pixelT color)
{         
	int i, b=0;
	int xx = x;
	int yy = y;
    
	for(i = 0; i < (int) strlen(string); i++)
    {
		b++;
		if(string[i]=='\n' || string[i]=='\r')
		{
			yy += fuente.height;
			xx = x;
			b=0;
		} 
		else
		{
			xx += VGAPutChar(fuente,string[i], xx, yy, color);
		}
		
	}
}

int
VGAStrlen(FUENTE & fuente, char *str)
{
	int i, width = 0;
	for (i = 0; i < (int) strlen(str); i++)
	{
		if (fuente.letras[(unsigned char)str[i]])
			width += fuente.letras[(unsigned char)str[i]]->xsize;
	}
	return width;
}

void
VGAPutStringCenter(int subrayado, FUENTE & fuente, char *str, int y, pixelT col)
{
	int stlen = VGAStrlen(fuente, str);
	VGAPutString ( fuente, str, (PhysicalWidth-stlen)/2, y, col);

    if(subrayado==1)
		Hline(((PhysicalWidth-stlen)/2), (PhysicalWidth+stlen)/2, y+fuente.height, col);
}

void
VGAPutStringR(int subrayado, FUENTE & fuente, char *str, int x, int y, pixelT col)
{
	int stlen = VGAStrlen(fuente, str);
	VGAPutString ( fuente, str, x - stlen, y, col);

    if(subrayado==1)
		Hline(x, x+stlen, y+fuente.height, col);
}

//--------------------------- LAYERIZADO  ----------------------------------------

FUENTE fuentestandard;

void VGAputch(int x, int y, char c, pixelT color)
{
	VGAPutChar(fuentestandard,c, x,y, color);
}

void VGAputs(int x, int y, char *string, pixelT color)
{
	VGAPutString(fuentestandard, string, x,y, color);
}

void Center (int subrayado, int y, char *string, pixelT color)
{
	VGAPutStringCenter(subrayado, fuentestandard,  string, y, color);
}

void Right(int subrayado, int x, int y, char *string, pixelT color)
{
	VGAPutStringR(subrayado, fuentestandard,  string, x, y, color);
}

int
StdStrlen( char *string )
{
	return VGAStrlen( fuentestandard, string );
}
