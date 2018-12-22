// DirectX Texture

#pragma once

#include <string>
#include <assert.h>
#include "System/System.h"
#include "System/Texture.h"
#include "DirectX/Error.h"


namespace DirectX
{
	typedef System::Texture SystemTexture;

    /// %DirectX implementation of %Texture

    class Texture : public System::Texture
	{
		LPDIRECT3DDEVICE9 _device;          ///< d3d device
		LPDIRECT3DTEXTURE9 _texture;        ///< d3d texture object

	public:

        /// create texture object

		Texture(LPDIRECT3DDEVICE9 device, const char name[])
		{
			assert(device);
			_device = device;

			_texture = load(name);
		}

		~Texture()
		{
			if (_texture)
			{
				_texture->Release();
				_texture = 0;
			}
		}

        virtual bool valid() const
        {
            return _texture!=0;
        }

		virtual int width() const
		{
            if (_texture)
            {
			    D3DSURFACE_DESC description;
			    if (_texture->GetLevelDesc(0, &description) != D3D_OK) throw Error("texture get width failed");
			    return description.Width;
            }
            else
                return 0;
		}

		virtual int height() const
		{
            if (_texture)
            {
			    assert(_texture);
			    D3DSURFACE_DESC description;
			    if (_texture->GetLevelDesc(0, &description) != D3D_OK) throw Error("texture get height failed");
			    return description.Height;
            }
            else
                return 0;
		}

		virtual	void release()
		{
			SystemTexture::release();

			if (!references())
			{
				if (_texture)
				{
					_texture->Release();
					_texture = 0;
				}
			}
		}

        /// get d3d texture object.
        /// used internally by DirectX::Display.

		LPDIRECT3DTEXTURE9 handle() const
		{
			return _texture;
		}

	protected:

        /// load texture into new d3d texture object.
        /// supports loading png, jpg and bmp files.

		LPDIRECT3DTEXTURE9 load(const char filename[])
		{
			LPDIRECT3DTEXTURE9 texture = 0;
			D3DXCreateTextureFromFileA(_device, filename, &texture);
			return texture;
		}
	};
}
