// System Timer class

#pragma once

#include "System/ObjectAdapter.h"


namespace System
{
    /// %Timer interface.
    /// An extremely simple high resolution timer interface.

    class Timer : public ObjectAdapter
	{
	public:

		virtual ~Timer() {};                   ///< virtual destructor

		virtual double time() = 0;             ///< get time in seconds since timer object created
        virtual double delta() = 0;            ///< get delta time in seconds between last two calls to time
	};
}
