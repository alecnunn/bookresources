
#include "tgahelpers.h"
#include "../viewer/TextureResource.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


TextureResource* LoadTGA(const char* filename)
{    
	unsigned char TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};
	unsigned char TGAcompare[12];
	unsigned char header[6];

	FILE* file = fopen(filename, "rb");

	if(	file==NULL ||
		fread(TGAcompare,1,sizeof(TGAcompare),file)!=sizeof(TGAcompare) ||
		memcmp(TGAheader,TGAcompare,sizeof(TGAheader))!=0 ||
		fread(header,1,sizeof(header),file)!=sizeof(header))
	{
		if (file == NULL)
			return false;
		else
		{
			fclose(file);
			return false;
		}
	}

	const int width  = header[1] * 256 + header[0];			
	const int height = header[3] * 256 + header[2];			
    
 	if(	width	<=0	||								
		height	<=0	||								
		(header[4]!=24 && header[4]!=32))					
	{
		fclose(file);										
		return false;										
	}

	const int bpp	= header[4];							
	const int bytesPerPixel	= bpp/8;						
	const int imageSize		= width*height*bytesPerPixel;

    TextureResource* texture = (TextureResource*)new char[sizeof(TextureResource) + imageSize];
    texture->magic = TextureResource::Magic;
    texture->version = TextureResource::Version;
    texture->bpp = bpp;
    texture->width = width;
    texture->height = height;
    texture->openGLID = 0;
    texture->dataSize = imageSize;    
    
	if(	(int)fread(texture->imageData, 1, imageSize, file)!=imageSize)	
	{
		delete texture;
		fclose(file);
		return false;
	}

	fclose (file);
    
	for(int i=0; i< imageSize; i+=bytesPerPixel)		
	{														
		int temp=texture->imageData[i];							
		texture->imageData[i] = texture->imageData[i + 2];	
		texture->imageData[i + 2] = temp;					
	}
    
	return texture;
}
