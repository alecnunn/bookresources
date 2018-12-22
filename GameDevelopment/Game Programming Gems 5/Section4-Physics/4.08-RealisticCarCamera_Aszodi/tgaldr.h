//************************************************************************* //
// Targa Image class for loading TGA images
//
// Authors: Aszódi, Barnabás
//          Czuczor, Szabolcs
// Created: July, 2004., Budapest, Hungary
//
// Budapest University of Technology and Economics,
// Department of Control Engineering and Information Technology (BME-IIT)
//************************************************************************* //
#include <stdio.h>

#ifndef TGALOADER_H
#define TGALOADER_H

struct TGAHeader {                                // The header of a standard TGA (Targa) image file
	unsigned char ImageIDLength;
	unsigned char ColorMapType;
	unsigned char ImageType;
	unsigned short FirstEntryIndex;
	unsigned short ColorMapLength;
	unsigned char ColorMapEntrySize;
	unsigned short XOrigin;
	unsigned short YOrigin;
	unsigned short Width;
	unsigned short Height;
	unsigned char PixelDepth;
	unsigned char ImageDescriptor;
};


//============================================================================
class TGAImage {
//============================================================================
	unsigned char * pc;                           // Pixel components of an image [R, G, B, R, G, B, R, G, B etc.]
	int width, height, pxNum, cmpNum;
public:
	
	TGAImage(int w = 1, int h = 1, unsigned char r = 0, unsigned char g = 0, unsigned char b = 0) {
		width = w;
		height = h;
		pxNum = width * height;                   // Number of pixels
		cmpNum = 3 * pxNum;                       // Number of RGB components
        pc = new unsigned char[cmpNum];
		for (int i = 0; i < cmpNum; i += 3) {
			pc[i] = r;
			pc[i + 1] = g;
			pc[i + 2] = b;
		}
	}

	~TGAImage() {
		delete [] pc;
	}

	int getWidth() { return width; }
	int getHeight() { return height; }

	unsigned char * getImageDataPointer() { return pc; }

	bool loadFromFile(char * filename) {          // Load image from a true color uncompressed TGA file
		FILE * TGAFile = fopen(filename, "rb");
		if (TGAFile == NULL) {
			printf("  Can't open the file \"%s\".\n", TGAFile);
			return false;
		}
		TGAHeader imageHeader;
		unsigned char * imageID;
		unsigned char * colorMapData;
		unsigned char * imageData;
		fread(&imageHeader.ImageIDLength, 1, 1, TGAFile);
		fread(&imageHeader.ColorMapType, 1, 1, TGAFile);
		fread(&imageHeader.ImageType, 1, 1, TGAFile);
		fread(&imageHeader.FirstEntryIndex, 2, 1, TGAFile);
		fread(&imageHeader.ColorMapLength, 2, 1, TGAFile);
		fread(&imageHeader.ColorMapEntrySize, 1, 1, TGAFile);
		fread(&imageHeader.XOrigin, 2, 1, TGAFile);
		fread(&imageHeader.YOrigin, 2, 1, TGAFile);
		fread(&imageHeader.Width, 2, 1, TGAFile);
		fread(&imageHeader.Height, 2, 1, TGAFile);
		fread(&imageHeader.PixelDepth, 1, 1, TGAFile);
		fread(&imageHeader.ImageDescriptor, 1, 1, TGAFile);
		if (imageHeader.ImageIDLength) {
			imageID = new unsigned char[imageHeader.ImageIDLength];
			fread(imageID, 1, imageHeader.ImageIDLength, TGAFile);
			delete [] imageID;
		}
		if (imageHeader.ColorMapType) {
			int numOfBytes = imageHeader.ColorMapLength * (imageHeader.ColorMapEntrySize >> 3);
			colorMapData = new unsigned char[numOfBytes];
			fread(colorMapData, 1, numOfBytes, TGAFile);
			delete [] colorMapData;
		}
		if (imageHeader.ImageType) {
			width = imageHeader.Width;
			height = imageHeader.Height;
			pxNum = width * height;
			cmpNum = 3 * pxNum;
			delete [] pc;
			pc = new unsigned char[cmpNum];
			int numOfBytes = imageHeader.Width * imageHeader.Height * (imageHeader.PixelDepth >> 3);
			imageData = new unsigned char[numOfBytes];
			fread(imageData, 1, numOfBytes, TGAFile);
			if (imageHeader.PixelDepth == 24) {
				int i, j;
				int top2down = (imageHeader.ImageDescriptor >> 5) & 1;
				int left2right = 1 - (imageHeader.ImageDescriptor >> 4) & 1;
				for (int y = 0; y < height; y++)
					for (int x = 0; x < width; x++) {
						i = 3 * ((top2down?(y):(height - 1 - y)) * width + (left2right?(x):(width - 1 - x)));
						j = 3 * (y * width + x);
						pc[i] = imageData[j + 2];
						pc[i + 1] = imageData[j + 1];
						pc[i + 2] = imageData[j];
					}
			} else {
				// Here we should process the not 24 bit image data
			}
			delete [] imageData;
		}
		fclose(TGAFile);
		return true;
	}

	bool saveToFile(char * filename) {            // Save image into a true color uncompressed TGA file
		FILE * TGAFile = fopen(filename, "wb");
		if (TGAFile == NULL) {
			printf("  Can't open the file \"%s\".\n", TGAFile);
			return false;
		}
		TGAHeader imageHeader;
		unsigned char * imageData;
		imageHeader.ImageIDLength = 0;
		imageHeader.ColorMapType = 0;
		imageHeader.ImageType = 2;
		imageHeader.FirstEntryIndex = 0;
		imageHeader.ColorMapLength = 0;
		imageHeader.ColorMapEntrySize = 24;
		imageHeader.XOrigin = 0;
		imageHeader.YOrigin = 0;
		imageHeader.Width = width;
		imageHeader.Height = height;
		imageHeader.PixelDepth = 24;
		imageHeader.ImageDescriptor = 32;
		fwrite(&imageHeader.ImageIDLength, 1, 1, TGAFile);
		fwrite(&imageHeader.ColorMapType, 1, 1, TGAFile);
		fwrite(&imageHeader.ImageType, 1, 1, TGAFile);
		fwrite(&imageHeader.FirstEntryIndex, 2, 1, TGAFile);
		fwrite(&imageHeader.ColorMapLength, 2, 1, TGAFile);
		fwrite(&imageHeader.ColorMapEntrySize, 1, 1, TGAFile);
		fwrite(&imageHeader.XOrigin, 2, 1, TGAFile);
		fwrite(&imageHeader.YOrigin, 2, 1, TGAFile);
		fwrite(&imageHeader.Width, 2, 1, TGAFile);
		fwrite(&imageHeader.Height, 2, 1, TGAFile);
		fwrite(&imageHeader.PixelDepth, 1, 1, TGAFile);
		fwrite(&imageHeader.ImageDescriptor, 1, 1, TGAFile);
		imageData = new unsigned char[cmpNum];
		for (int i = 0; i < cmpNum; i += 3) {
			imageData[i] = pc[i + 2];
			imageData[i + 1] = pc[i + 1];
			imageData[i + 2] = pc[i];
		}
		fwrite(imageData, 1, cmpNum, TGAFile);
		fclose(TGAFile);
		delete [] imageData;
		return true;
	}
};

#endif