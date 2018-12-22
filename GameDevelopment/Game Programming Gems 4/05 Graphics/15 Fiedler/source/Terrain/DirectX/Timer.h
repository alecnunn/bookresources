// DirectX Timer

#pragma once

#include "System/Timer.h"

namespace DirectX
{
    /// %DirectX implementation of %Timer.
    /// In the future we should find a solution that allows
    /// sharing of the Windows implementation of Timer across DirectX and OpenGL.
    /// Uses QueryPerformanceCounter internally.

    class Timer : public System::Timer
    {
        public:

            /// default constructor.
            /// clears time and delta to 0.0f.

		    Timer()
		    {
                QueryPerformanceFrequency((LARGE_INTEGER*)&_frequency);
			    QueryPerformanceCounter((LARGE_INTEGER*)&_counter);
			    _time = 0.0;
                _delta = 0.0;
		    }

		    double time()
		    {
			    __int64 new_counter;
			    QueryPerformanceCounter((LARGE_INTEGER*)&new_counter);
			    __int64 delta = new_counter - _counter;
                _delta = delta / (float) _frequency;
			    _time += _delta;
			    _counter = new_counter;
			    return _time;
		    }

            double delta()
            {
                return _delta;
            }

        private:

            double _time;               ///< current time in seconds
            double _delta;              ///< current delta in seconds
            __int64 _counter;           ///< raw 64bit timer counter
            __int64 _frequency;         ///< raw 64bit timer frequency
    };
}
