// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3PLATFORMS_H
#define WM3PLATFORMS_H

// Platform-specific information goes in this header file.  The defines to
// control which platform is included are:
//
// WIN32      :  Microsoft Windows 2000/XP
// __APPLE__  :  Macintosh OS X (10.2.3 or higher required)
// <none>     : Linux
//
// Add others as needed.

//----------------------------------------------------------------------------
// Microsoft Windows 2000/XP platform
//----------------------------------------------------------------------------
#if defined(WIN32)

// for a DLL library
#if defined(WM3_DLL_EXPORT)
#define WM3_ITEM __declspec(dllexport)

// for a client of the DLL library
#elif defined(WM3_DLL_IMPORT)
#define WM3_ITEM __declspec(dllimport)

// for a static library
#else
#define WM3_ITEM

#endif

#if defined(_MSC_VER)

// Microsoft Visual C++ specific pragmas.  MSVC6 appears to be version 1200
// and MSVC7 appears to be version 1300.
#if _MSC_VER < 1300
#define WM3_USING_VC6
#else
#define WM3_USING_VC7
#endif

#if defined(WM3_USING_VC6)

// Disable the warning about truncating the debug names to 255 characters.
// This warning shows up often with STL code in MSVC6, but not MSVC7.
#pragma warning( disable : 4786 )

// This warning is disabled because MSVC6 warns about not finding
// implementations for the pure virtual functions that occur in the template
// classes 'template <class Real>' when explicity instantiating the classe.
// NOTE:  If you create your own template classes that will be explicitly
// instantiated, you should re-enable the warning to make sure that in fact
// all your member data and functions have been defined and implemented.
#pragma warning( disable : 4661 )

#endif

// Enable the warning: "loop control variable declared in the for-loop is
// used outside the for-loop scope.  The default level 3 warnings do not
// enable this (level 4 does), but should since allowing the outside scope
// is a Microsoft extension.
// #pragma warning( error : 4289 )

#endif

// Specialized instantiation of static members in template classes before or
// after the class itself is instantiated is not a problem with Visual Studio
// .NET 2003 (VC 7.1), but VC 6 likes the specialized instantiation to occur
// after the class instantiation.
// #define WM3_INSTANTIATE_BEFORE
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Macintosh OS X platform
//----------------------------------------------------------------------------
#elif defined(__APPLE__)

#define WM3_BIG_ENDIAN

// Macro used for Microsoft Windows systems to support dynamic link libraries.
// Not needed for the Macintosh.
#define WM3_ITEM

// g++ wants specialized template instantiations to occur after the explicit
// class instantiations.  CodeWarrior wants them to occur before.
#ifdef __MWERKS__
#define WM3_INSTANTIATE_BEFORE
#endif
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Linux platform
//----------------------------------------------------------------------------
#else

// Macro used for Microsoft Windows systems to support dynamic link libraries.
// Not needed for Linux.
#define WM3_ITEM

// Linux on a PC. Red Hat 8.x g++ has problems with specialized instantiation
// of static members in template classes *before* the class itself is
// explicitly instantiated.  The problem is not consistent; for example, Math
// Vector*, and Matrix* classes compile fine, but not Integrate1 or
// BSplineRectangle.  So the following macro is *not* defined for this
// platform.  If you have a Linux system that does appear to require the
// instantiation before, then enable this macro.
// #define WML_INSTANTIATE_BEFORE
// Macro used for Microsoft Windows systems to support dynamic link libraries.
// Not needed for Linux.
#define WM3_ITEM

// Linux on a PC. Red Hat 8.x g++ has problems with specialized instantiation
// of static members in template classes *before* the class itself is
// explicitly instantiated.  The problem is not consistent; for example, Math
// Vector*, and Matrix* classes compile fine, but not Integrate1 or
// BSplineRectangle.  So the following macro is *not* defined for this
// platform.  If you have a Linux system that does appear to require the
// instantiation before, then enable this macro.
// #define WML_INSTANTIATE_BEFORE

#endif
//----------------------------------------------------------------------------

#endif
