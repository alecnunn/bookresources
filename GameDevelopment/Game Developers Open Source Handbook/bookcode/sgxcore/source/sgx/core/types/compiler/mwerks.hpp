#ifndef SGX_CORE_TYPES_COMPILER_METROWERKS_HPP
#define SGX_CORE_TYPES_COMPILER_METROWERKS_HPP 

#define SGX_PACKDATA_BEGIN(align)
#define SGX_PACKDATA_END(align)        __attribute__ ((aligned(align)))

#define SGX_INLINE      __inline

#endif /* SGX_CORE_TYPES_COMPILER_METROWERKS_HPP */
