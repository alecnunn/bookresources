#ifndef SGX_CORE_TYPES_CASTING_HPP
#define SGX_CORE_TYPES_CASTING_HPP	1

#ifdef __cplusplus
#define SGX_CAST(casttype, castclass, var)	((castclass)(var))
#else
#define SGX_CAST(casttype, castclass, var)	(casttype<castclass>(var))
#endif

#define SGX_STATIC_CAST(castclass, var)      SGX_CAST(static_cast, castclass, var)
#define SGX_DYNAMIC_CAST(castclass, var)     SGX_CAST(dynamic_cast, castclass, var)
#define SGX_REINTERPRET_CAST(castclass, var) SGX_CAST(reinterpret_cast, castclass, var)
#define SGX_CONST_CAST(castclass, var)       SGX_CAST(const_cast, castclass, var)

#endif /* SGX_CORE_TYPES_CASTING_HPP */
