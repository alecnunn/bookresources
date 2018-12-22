// System classes

#pragma once

#pragma warning( disable : 4127 )  // conditional expression is constant
#pragma warning( disable : 4100 )  // unreferenced formal parameter
#pragma warning( disable : 4702 )  // unreachable code

namespace Mathematics {};

/// Interfaces defining system functionality.
/// Create a display instance with the System::createDisplay method,
/// then using the Display object, contruct instances of other objects
/// such as Light, Material, Texture and Mesh.

namespace System
{
    using namespace Mathematics;

    class Error;
    class Vertex;
    class Key;
    class KeyboardListener;
    class Mouse;
    class MouseListener;
    class WindowListener;
    class Object;
    class Light;
    class Material;
    class Texture;
    class Mesh;
    class Timer;
    class Display;

    /// %Display factory method.
    /// Creates an system specific instance of a display object, for Win32 the
    /// instance will be a DirectX::Display, we plan to add an OpenGL::Display
    /// for both Windows and MacOS in the future.

    Display* createDisplay();

    /// Write a message to the log.

    void log(const char message[]);

    /// Write a formatted message to the log.
    /// note: printf style formatting.

    void logf(const char format[], ...);
}
