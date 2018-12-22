#ifndef SGX_CORE_TYPES_COMPILER_GNU_HPP
#define SGX_CORE_TYPES_COMPILER_GNU_HPP

#define SGX_PACKDATA_BEGIN(align)
#define SGX_PACKDATA_END(align)        __attribute__ ((__aligned__(align)))

#define SGX_INLINE      __inline__

#endif /* SGX_CORE_TYPES_COMPILER_GNU_HPP */
