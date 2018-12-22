// System Texture class

#pragma once

#include "ObjectAdapter.h"


namespace System
{
    /// A texture map.
    /// Create a texture instance using the Display::createTexture factory method.
    /// To select this texture for rendering call Display::selectTexture.

    class Texture : public ObjectAdapter
	{
	public:

        /// check if texture object is valid.
        /// todo: is this still required?

        virtual bool valid() const = 0;

        /// get width of texture in pixels.

		virtual int width() const = 0;

        /// get height of texture in pixels.

		virtual int height() const = 0;
	};
}
