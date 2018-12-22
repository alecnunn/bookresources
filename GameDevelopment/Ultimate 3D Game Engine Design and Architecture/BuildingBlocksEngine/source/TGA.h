/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_TGA_H_
#define _BB_TGA_H_

#include<stdio.h>


unsigned char* LoadTGAFromMemory(char *data, int length, int &width, int &height, int &components);
unsigned char* LoadTGA(char* file, int &width, int &height, int &components);
bool WriteTGA(char *file, int width, int height, int comp, unsigned char *outImage);


#endif