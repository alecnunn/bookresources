#ifndef TGAHELPERS_H_
#define TGAHELPERS_H_

struct TextureResource;

struct TextureImage
{
    unsigned char* imageData;
	int bpp;
	int width;
	int height;
	unsigned int texID;
};


TextureResource* LoadTGA(const char* filename);


#endif
