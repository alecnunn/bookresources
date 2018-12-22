// DirectX Light

#pragma once

#include "System/Light.h"
#include "Mathematics/Color.h"


namespace DirectX
{
    using namespace Mathematics;

    /// %DirectX implementation of %Light

    class Light : public System::Light
	{	
	private:

		D3DLIGHT9 _light;           ///< d3d light struct

	public:

        /// default constructor.
        /// clears d3d light struct data to defaults.

		Light()
		{
			ZeroMemory(&_light, sizeof(D3DLIGHT9));
            _light.Type = D3DLIGHT_DIRECTIONAL; //D3DLIGHT_POINT;
            _light.Range = 10000;
            _light.Attenuation0 = 1;
		}

		virtual void diffuse(const Color &color)
		{
			_light.Diffuse.r = color.r;
			_light.Diffuse.g = color.g;
			_light.Diffuse.b = color.b;
			_light.Diffuse.a = color.a;
		}

		virtual void specular(const Color &color)
		{
			_light.Specular.r = color.r;
			_light.Specular.g = color.g;
			_light.Specular.b = color.b;
			_light.Specular.a = color.a;
		}

		virtual void position(const Vector &vector)
		{
			_light.Position = D3DXVECTOR3(vector.x, vector.y, vector.z);
		}

		virtual void direction(const Vector &vector)
		{
			_light.Direction = D3DXVECTOR3(vector.x, vector.y, vector.z);
		}

		virtual Color diffuse() const
		{
			return Color(_light.Diffuse.r, _light.Diffuse.g, _light.Diffuse.b, _light.Diffuse.a);
		}

		virtual Color specular() const
		{
			return Color(_light.Specular.r, _light.Specular.g, _light.Specular.b, _light.Specular.a);
		}

		virtual Vector position() const
		{
			return Vector(_light.Position.x, _light.Position.y, _light.Position.z);
		}

		virtual Vector direction() const
		{
			return Vector(_light.Direction.x, _light.Direction.y, _light.Direction.z);
		}

        /// get d3d light struct.
        /// used internally by DirectX::Display.

		const D3DLIGHT9& light() const
		{
			return _light;
		}
	};
}
