
#include "TextureResource.h"
#include <GL/glut.h>
#include <cstdio>
#include <cassert>


TextureResource* CreateFromFile(const char* filename)
{
    FILE* fin = fopen(filename, "rb");
    if (fin == NULL)
        return NULL;
        
    TextureResource header;
    if (fread(&header, sizeof(TextureResource), 1, fin) != 1)
    {
        fclose(fin);
        return NULL;
    }
    if (header.magic != TextureResource::Magic || header.version != TextureResource::Version)
    {
        fclose(fin);
        return NULL;
    }
    
    TextureResource* texture = (TextureResource*)new char[sizeof(TextureResource) + header.dataSize];
    *texture = header;
    
    if (fread(texture->imageData, texture->dataSize, 1, fin) != 1)
    {
        fclose(fin);
        return NULL;
    }        
    
    fclose(fin);
    
    UploadOpenGLTexture(*texture);
    return texture;
}



void UploadOpenGLTexture (TextureResource& texture)
{
	glGenTextures(1, &texture.openGLID);

	glBindTexture(GL_TEXTURE_2D, texture.openGLID);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	
    GLuint type = (texture.bpp != 24) ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_2D, 0, type, texture.width, texture.height, 0, type, GL_UNSIGNED_BYTE, texture.imageData);
}


void RemoveOpenGLTexture(TextureResource& texture)
{
    glDeleteTextures(1, &texture.openGLID);
}
