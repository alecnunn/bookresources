// System factory methods

#include <stdio.h>
#include <stdlib.h>

#include "System.h"
#include "DirectX/Display.h"


System::Display* System::createDisplay()
{
    return new DirectX::Display();
}


void System::log(const char message[])
{
    printf("%s\n", message);
}


void System::logf(const char format[], ...)
{
    char buffer[1024];

	va_list args;
	va_start(args, format);
	_vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

    printf("%s\n", buffer);
}


/// \mainpage Virtual Go
///
/// \section intro Introduction
///
/// This is the technical documentation for Virtual Go.
