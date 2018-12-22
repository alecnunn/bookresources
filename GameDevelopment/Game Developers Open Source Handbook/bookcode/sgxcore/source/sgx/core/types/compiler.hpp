#ifndef SGX_CORE_TYPES_COMPILER_HPP 
#define SGX_CORE_TYPES_COMPILER_HPP

//
// These macros use the (compiler-specific) project settings
// to create independant versions for the game.
//
#ifdef  _MSC_VER
 #include "sgx/core/types/compiler/msvc.hpp"
#endif

#ifdef  __MWERKS__
 #include "sgx/core/types/compiler/mwerks.hpp"
#endif

#ifdef  __GNUC__
 #include "sgx/core/types/compiler/gnu.hpp"
#endif


//
// Add any generic macros the compiler doesn't support
//

// For manually aligning data
#ifndef SGX_PACKDATA_BEGIN
  #define SGX_PACKDATA_BEGIN(align)
#endif

#ifndef SGX_PACKDATA_END
  #define SGX_PACKDATA_END(align)
#endif

// Compiler support
#ifndef SGX_INLINE
  #define SGX_INLINE        inline
#endif

#ifndef SGX_FORCE_INLINE
  #define SGX_FORCE_INLINE  inline
#endif

#ifndef SGXAPI
  #define SGXAPI
#endif


#endif /* SGX_CORE_TYPES_COMPILER_HPP */
