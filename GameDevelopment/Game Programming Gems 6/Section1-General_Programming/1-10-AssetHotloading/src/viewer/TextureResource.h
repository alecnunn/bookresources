#ifndef TEXTURERESOURCE_H_
#define TEXTURERESOURCE_H_

struct TextureResource
{
    enum { Magic = 0xCAFECAFE };
    enum { Version = 0x01 };

    unsigned int magic;
    unsigned int version;
    int bpp;
    int width;
    int height;
    unsigned int openGLID;
    int dataSize;

#pragma warning(push)
#pragma warning(disable:4200)
	unsigned char imageData[0];
#pragma warning(pop)
};


TextureResource* CreateFromFile(const char* filename);

void UploadOpenGLTexture (TextureResource& texture);
void RemoveOpenGLTexture(TextureResource& texture);


#endif
