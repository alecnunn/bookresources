// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// misc.h

// Misc macros and other stuff

#ifndef INCLUDED_MISC_H
#define INCLUDED_MISC_H

// compile-time assertion

template <bool b> struct COMPILE_ASSERT_FAILURE; // fwd
template <> struct COMPILE_ASSERT_FAILURE<true> { }; // specialize for "true"

#define COMPILE_ASSERT(name, expr) \
  struct name : COMPILE_ASSERT_FAILURE<(bool)(expr)> { }

#endif // !INCLUDED_MISC_H
