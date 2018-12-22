// System Vertex class

#pragma once

#include "Mathematics/Vector.h"
#include "Mathematics/Color.h"

namespace System
{
    using namespace Mathematics;

    /// Concrete class used to specify vertices when creating a %Mesh object.
    /// Internally the system driver will convert to a more optimal format. 
    /// Consider this class as an intermediate format only.
    /// See Display::createMesh for details.

	class Vertex
	{
	public:

        /// default constructor.
        /// does nothing for speed.

        Vertex() {}

        /// construct a vertex with specified position, normal and texture coordinates u,v.

        Vertex(const Vector &position, const Vector &normal, float u, float v)
		{
            assert(normal.normalized());

			this->position = position;
			this->normal = normal;
			this->u = u;
			this->v = v;
		}

        /// equals operator.

		bool operator ==(const Vertex &other) const
		{
			if (Float::equal(u,other.u) && Float::equal(v,other.v) && position==other.position && normal==other.normal) return true;
			else return false;
		}

        /// not equals operator.

		bool operator !=(const Vertex &other) const
		{
			return !(*this==other);
		}

		Vector position;        ///< vertex position
		Vector normal;          ///< vertex normal (must be unit length)
		float u;                ///< horizontal texture coordinate
        float v;                ///< vertical texture coordinate
	};
}
