#ifndef SGX_CORE_TYPES_COMPILER_MSVC_H 
#define SGX_CORE_TYPES_COMPILER_MSVC_H 

#ifdef _DEBUG
  #define SGX_DEBUG                   1
  #define SGX_RELEASE                 0
#else
  #define SGX_DEBUG                   0
  #define SGX_RELEASE                 1
#endif

#define SGX_PACKDATA_BEGIN(align)    __declspec(align(align)) 
#define SGX_PACKDATA_END(align)        

#define SGX_INLINE                   __inline
#define SGX_FORCE_INLINE             __forceinline
    
//
// Switch off the most annoying warnings
//
#pragma warning(disable : 4100)      // unreferenced formal parameter
#pragma warning(disable : 4511)      // copy constructor could not be generated
#pragma warning(disable : 4512)      // assignment constructor could not be generated
#pragma warning(disable : 4663)      // C++ language change
#pragma warning(disable : 4786)      // identifier was truncated to '255' characters


#endif /* SGX_CORE_TYPES_COMPILER_MSVC_H */
