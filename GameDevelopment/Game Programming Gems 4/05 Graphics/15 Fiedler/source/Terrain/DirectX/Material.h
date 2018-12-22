// DirectX Material

#pragma once

#include "System/Material.h"


namespace DirectX
{
    /// %DirectX implementation of %Material

	class Material : public System::Material
	{	
	private:

		D3DMATERIAL9 _material;         ///< d3d material struct

	public:

        /// default constructor.
        /// clears d3d material struct to zeros.

		Material()
		{
			ZeroMemory(&_material, sizeof(D3DMATERIAL9));
		}

		virtual void ambient(const Color &color)
		{
			_material.Ambient.r = color.r;
			_material.Ambient.g = color.g;
			_material.Ambient.b = color.b;
			_material.Ambient.a = color.a;
		}

		virtual void diffuse(const Color &color)
		{
			_material.Diffuse.r = color.r;
			_material.Diffuse.g = color.g;
			_material.Diffuse.b = color.b;
			_material.Diffuse.a = color.a;
		}

		virtual void specular(const Color &color)
		{
			_material.Specular.r = color.r;
			_material.Specular.g = color.g;
			_material.Specular.b = color.b;
			_material.Specular.a = color.a;
		}

		virtual void emissive(const Color &color)
		{
			_material.Emissive.r = color.r;
			_material.Emissive.g = color.g;
			_material.Emissive.b = color.b;
			_material.Emissive.a = color.a;
		}

		virtual void power(float value)
		{
			_material.Power = value;
		}

		virtual Color ambient() const
		{
			return Color(_material.Ambient.r, _material.Ambient.g, _material.Ambient.b, _material.Ambient.a);
		}

		virtual Color diffuse() const
		{
			return Color(_material.Diffuse.r, _material.Diffuse.g, _material.Diffuse.b, _material.Diffuse.a);
		}

		virtual Color specular() const
		{
			return Color(_material.Specular.r, _material.Specular.g, _material.Specular.b, _material.Specular.a);
		}

		virtual Color emissive() const
		{
			return Color(_material.Emissive.r, _material.Emissive.g, _material.Emissive.b, _material.Emissive.a);
		}

		virtual float power() const

		{
			return _material.Power;
		}

        /// get d3d material struct.
        /// used internally by DirectX::Display.

		const D3DMATERIAL9& material() const
		{
			return _material;
		}
	};
}
