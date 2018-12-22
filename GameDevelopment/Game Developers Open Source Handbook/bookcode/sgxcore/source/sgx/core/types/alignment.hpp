#ifndef SGX_CORE_TYPES_DATATYPE_ALIGNMENT_HPP
#define SGX_CORE_TYPES_DATATYPE_ALIGNMENT_HPP		1

#define sgxAlign(_v, _align)   (((size_t)(_v)+(_align)-1 ) & \
(~((_align)-1)))
#define sgxAlignUp(_v, _align) (sgxAlign((_v), (_align)) + (_align))

#define sgxAlign4(_v)      sgxAlign(_v,4)
#define sgxAlign16(_v)     sgxAlign(_v,16)
#define sgxAlign32(_v)     sgxAlign(_v,32)
#define sgxAlign128(_v)    sgxAlign(_v,128)

#define sgxIsAligned(_v, _align) (((size_t)(_v)&((_align)-1)) == 0)

#endif /* SGX_CORE_TYPES_DATATYPE_ALIGNMENT_HPP */
