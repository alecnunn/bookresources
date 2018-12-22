// System Object Adapter class

#pragma once

#include "Object.h"


namespace System
{
    /// Implements basic object reference counting.
    /// Implementing reference counting here in an adapter class saves
    /// writing this code in derived classes: Light, Material, Texture and Mesh.

    class ObjectAdapter : public Object
	{
	public:

        /// creates object with an initial reference count of 1 belonging to whoever initially creates the object.

		ObjectAdapter()
		{
			_references = 1;
		}

		virtual void reference()
		{
			_references++;
		}

		virtual void release()
		{
			_references--;
		}

		virtual int references()
		{
			return _references;
		}

	private:

		int _references;        ///< number of references to this object.
	};
}
