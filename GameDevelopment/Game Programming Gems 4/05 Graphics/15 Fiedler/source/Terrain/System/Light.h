// System Light class

#pragma once

#include "ObjectAdapter.h"
#include "Mathematics/Color.h"
#include "Mathematics/Vector.h"


namespace System
{
    using namespace Mathematics;

    /// A directional light source.
    /// Create a light instance using the Display::createLight factory method.
    /// Call Display::selectLight to use this light object when rendering.

	class Light : public ObjectAdapter
	{
	public:

        /// set diffuse color.

		virtual void diffuse(const Color &color) = 0;

        /// set specular color.

		virtual void specular(const Color &color) = 0;

        /// set light position.

		virtual void position(const Vector &vector) = 0;

        /// set light direction.

		virtual void direction(const Vector &vector) = 0;

        /// get diffuse color.

		virtual Color diffuse() const = 0;

        /// get specular color.

		virtual Color specular() const = 0;

        /// get light position.

		virtual Vector position() const = 0;

        /// get light direction.

		virtual Vector direction() const = 0;
	};
}
