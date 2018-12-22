#include "sgx/core/core.hpp"
#include "safety.hpp"

#if SGX_DEBUG
void sgxSafetyCheck(void)
{
tUINT16 uint16 = 1;
tUINT32 uint32 = 1;

	/* Size Checks */
	sgxCompileAssert(sizeof(tBYTE) == 1);
	sgxCompileAssert(sizeof(tCHAR8) == 1);
	sgxCompileAssert(sizeof(tUCHAR8) == 1);
	sgxCompileAssert(sizeof(tINT16) == 2);
	sgxCompileAssert(sizeof(tUINT16) == 2);
	sgxCompileAssert(sizeof(tINT32) == 4);
	sgxCompileAssert(sizeof(tUINT32) == 4);

	/* Signed/unsigned checks */
	sgxCompileAssert((tBYTE)-1 == 0xff);
	sgxCompileAssert((tCHAR8)-1 == -1);
	sgxCompileAssert((tUCHAR8)-1 == 0xff);
	sgxCompileAssert((tINT16)-1 == -1);
	sgxCompileAssert((tUINT16)-1 == 0xffff);
	sgxCompileAssert((tINT32)-1 == -1);
	sgxCompileAssert((tUINT32)-1 == 0xffffffff);

	/* Check endian */
	if (isLittleEndian()) {
		sgxTrace("Little Endian (x86, PS2)");
#if SGX_ENDIAN_BIG==1
		sgxTrace("ERROR! SGX_ENDIAN_BIG macro is set");
#endif
	} else {
		sgxTrace("Big Endian (GC)");
#if SGX_ENDIAN_LITTLE==1
		sgxTrace("ERROR! SGX_ENDIAN_LITTLE macro is set");
#endif
	}

	/* Check swap macros */
	uint16 = 0x1234;
	uint16 = sgxEndianSwap16(uint16);
	sgxAssert(uint16 == 0x3412);

	uint32 = 0x12345678;
	uint32 = sgxEndianSwap32(uint32);
	sgxAssert(uint32 == 0x78563412);
}
#endif

