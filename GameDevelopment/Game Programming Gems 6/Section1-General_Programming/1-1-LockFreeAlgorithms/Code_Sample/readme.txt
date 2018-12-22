Game Programming Gems 6
Lock free multithreaded algorithms
By Toby Jones (thjones@microsoft.com)

The included code (lockfree.cpp) supports both Microsoft Visual C++ and GCC for Windows, but with a bit of tweaking, could support other systems.  The primary area of interest when porting would be the implementation of the CAS and CAS2 code, which tends to be system specific.  Additionally, the code assumes a pointer size of 32-bits, and porting to a 64-bit architecture will require workarounds noted in the associated article.

lockfree.cpp includes the code from the article in complete form, as well as implementations for CAS and CAS2 using assembly language, compiler intrinsics, and Windows API calls.  Inline assembly language is available for both Visual C++ and GCC.  Code is also included to demonstrate and test the algorithms.

Visual Studio
-------------

The included code was developed on Microsoft Visual Studio 2005 Beta 2.  To build the code, open lockfree.sln in Visual Studio, and run Build->Build Solution.  Lockfree.exe will be built.  The code may be built on earlier versions of Visual Studio by creating a new console project, and adding the lockfree.cpp file to the project.

The current Visual Studio website is at lab.msdn.microsoft.com/vs2005.

GCC
---

The included code has been tested with GCC 3.4.2, using the MinGW port.  To compile the code:

g++ -Wall -Wextra lockfree.cpp -o lockfree.exe

The official MinGW website is www.mingw.org, but a very good distribution can be found at www.nuwen.net.  The official GCC website is gcc.gnu.org.
