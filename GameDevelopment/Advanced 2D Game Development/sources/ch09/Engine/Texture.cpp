#include "Advanced2D.h"

namespace Advanced2D {


Texture::Texture()
{
	this->texture = NULL;
}

Texture::~Texture()
{
	Release();
}

int Texture::getWidth() 
{ 
	return info.Width; 
};

int Texture::getHeight() 
{ 
	return info.Height; 
};

bool Texture::Load(std::string filename, D3DCOLOR transcolor)
{
    //standard Windows return value
    HRESULT result;
    
    //get width and height from bitmap file
    result = D3DXGetImageInfoFromFile(filename.c_str(), &info);
    if (result != D3D_OK) 	{
        texture = NULL;
		return false;
	}

    //create the new texture by loading a bitmap image file
	D3DXCreateTextureFromFileEx( 
        g_engine->getDevice(), //Direct3D device object
        filename.c_str(),      //bitmap filename
        info.Width,            //bitmap image width
        info.Height,           //bitmap image height
        1,                     //mip-map levels (1 for no chain)
        D3DPOOL_DEFAULT,       //the type of surface (standard)
        D3DFMT_UNKNOWN,        //surface format (default)
        D3DPOOL_DEFAULT,       //memory class for the texture
        D3DX_DEFAULT,          //image filter
        D3DX_DEFAULT,          //mip filter
        transcolor,            //color key for transparency
        &info,                 //bitmap file info (from loaded file)
        NULL,                  //color palette
        &texture );            //destination texture

    //make sure the bitmap textre was loaded correctly
    if (result != D3D_OK) 	{
        texture = NULL;
		return false;
	}

	return true;
}

void Texture::Release()
{
	if (texture != NULL)
		texture->Release();
}


}
