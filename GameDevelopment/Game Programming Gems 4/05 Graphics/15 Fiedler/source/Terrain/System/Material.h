// System Material class

#pragma once

#include "ObjectAdapter.h"


namespace System
{
    /// Describes surface properties for rendering.
    /// Create a material instance using the Display::createMaterial factory method.
    /// Select the material for rendering using Display::selectMaterial.

	class Material : public ObjectAdapter
	{
	public:

        /// set ambient color.

		virtual void ambient(const Color &color) = 0;

        /// set diffuse color.

		virtual void diffuse(const Color &color) = 0;

        /// set specular color.

		virtual void specular(const Color &color) = 0;

        /// set emissive color.

		virtual void emissive(const Color &color) = 0;

        /// set specular power.

		virtual void power(float value) = 0;

        /// get ambient color.

		virtual Color ambient() const = 0;

        /// get diffuse color.

		virtual Color diffuse() const = 0;

        /// get specular color.

		virtual Color specular() const = 0;

        /// get emissive color.

		virtual Color emissive() const = 0;

        /// get specular power.

		virtual float power() const = 0;
	};
}
