// System Object class

#pragma once


namespace System
{
    /// Reference counted object interface.
    /// This serves as the base class for all managed display objects
    /// such as Material, Texture, Mesh and Light. The reference counting interface
    /// defined here allows the Display to take ownership of the object and automatically 
    /// garbage collect it when the object is longer in use.

	class Object
	{
	public:

        /// virtual destructor.

		virtual ~Object() {};

        /// add a reference to this object.

		virtual void reference() = 0;

        /// remove a reference to this object.

		virtual void release() = 0;

        /// get number of references to this object.

		virtual int references() = 0;
	};
}
