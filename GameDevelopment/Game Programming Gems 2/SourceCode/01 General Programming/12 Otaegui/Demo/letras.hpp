/************************************************************
 * Font Functions Header File V1.03.003
 * (c) Copyright 1999-2000 Sabarasa Entertainment
 * For internal use only.
 ************************************************************/               

#ifndef FONT_FUNCTIONS

#define FONT_FUNCTIONS

#include "blitbuf.hpp"

struct FUENTE
{
	int height;
	int promediowidth; // en desuso  
	
	blitbuf * letras[256];
	FUENTE(char *fname = NULL);
};

extern FUENTE fuentestandard;

// Carga la fuente desde el pcx a la struct
void LoadLetras(FUENTE & fuente, char * pcxfontfilename);

void VGAputch(int x, int y, char c, pixelT color);

void VGAputs(int x, int y, char *string, pixelT color);

void Center (int subrayado, int y, char *string, pixelT color);

void Right(int subrayado, int x, int y, char *string, pixelT color);

int StdStrlen( char *string );

void VGAPutString (FUENTE & fuente, char *string, short x, short y, pixelT color);

void VGAPutStringCenter(int subrayado, FUENTE & fuente,char *str,int y, pixelT col);


#endif