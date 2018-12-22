// DirectX Error

#pragma once

#include <string>
#include "System/Error.h"


namespace DirectX
{
    /// %DirectX implementation of %Error.
    /// In the future we should find a solution that allows
    /// sharing of the Windows implementation of Error across DirectX and OpenGL.

	class Error : public System::Error
	{
	public:

        Error(const char message[]) : System::Error(message)
		{
		}

		virtual int report()
		{
			MessageBox(0, message(), title(), MB_OK | MB_ICONERROR | MB_SETFOREGROUND | MB_TOPMOST);
            return 1;
		}

		virtual const char* title() const
		{
			return "DirectX Error";
		}
	};
}
