#ifndef SGX_CORE_TYPES_DATATYPE_ENDIAN_HPP
#define SGX_CORE_TYPES_DATATYPE_ENDIAN_HPP	1


SGX_INLINE tUINT16 sgxEndianSwap16(tUINT16 var)
{
   return (tUINT16)((((var)&0xff)<<8) | ( ((var)>>8)&0xff));
}

SGX_INLINE tUINT32 sgxEndianSwap32(tUINT32 var)
{
   return ((((var)&0xff)<<24) | ((((var)>>8)&0xff)<<16) |
          ((((var)>>16)&0xff)<<8) | ((((var)>>24)&0xff)) );
}

// Run-time check for safety
// Little endian machines : x86, Xbox, PS2
SGX_INLINE tBOOL isLittleEndian() 
{
	tUINT16 uint16;

    uint16 = 1;
    if (*(tBYTE *)&uint16 == 1) {
		return TRUE;
	} else {
		return FALSE;
	}
}

// Big endian machines : Xbox360, Gamecube, PPC
SGX_INLINE tBOOL isBigEndian() 
{
	return !isLittleEndian();
}

#if SGX_ENDIAN_LITTLE==1
	#define sgxEndianToBig16(_v)          sgxEndianSwap16(_v)
	#define sgxEndianToLittle16(_v)
	#define sgxEndianToBig32(_v)          sgxEndianSwap32(_v)
	#define sgxEndianToLittle32(_v)

	SGX_INLINE tUINT16 sgxMakeInt16(tBYTE byte1, tBYTE byte2)
	{
	   return (byte2 << 8) | byte1;
	}

	SGX_INLINE tUINT16 sgxMakeInt16(tBYTE *ptr)
	{
	   return (*(ptr+1) << 8) | *(ptr+0);
	}

	SGX_INLINE tUINT16 sgxMakeInt32(tBYTE byte1, tBYTE byte2, tBYTE byte3, tBYTE byte4)
	{
	   return (byte4 << 24) | (byte3 << 16) | (byte2 << 8) | byte1;
	}

	SGX_INLINE tUINT16 sgxMakeInt32(tBYTE *ptr)
	{
	   return (*(ptr+3) << 24) | (*(ptr+2) << 16) | (*(ptr+1) << 8) | *(ptr+0);
	}

#elif SGX_ENDIAN_BIG==1
	#define sgxEndianToBig16(_v)
	#define sgxEndianToLittle16(_v)       sgxEndianSwap16(_v)
	#define sgxEndianToBig32(_v)
	#define sgxEndianToLittle32(_v)       sgxEndianSwap32(_v)

	SGX_INLINE tUINT16 sgxMakeInt16(tBYTE byte1, tBYTE byte2)
	{
	   return (byte1 << 8) | byte2;
	}

	SGX_INLINE tUINT16 sgxMakeInt16(tBYTE *ptr)
	{
	   return (*(ptr+0) << 8) | *(ptr+1);
	}

	SGX_INLINE tUINT16 sgxMakeInt32(tBYTE byte1, tBYTE byte2, tBYTE byte3, tBYTE byte4)
	{
	   return (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
	}

	SGX_INLINE tUINT16 sgxMakeInt32(tBYTE *ptr)
	{
	   return (*(ptr+0) << 24) | (*(ptr+1) << 16) | (*(ptr+2) << 8) | *(ptr+4);
	}

#else
	sgxCompileError("No Endian is set");

	#define sgxEndianToBig16(_v)
	#define sgxEndianToLittle16(_v)
	#define sgxEndianToBig32(_v)
	#define sgxEndianToLittle32(_v)

	SGX_INLINE tUINT16 sgxMakeInt16(tBYTE byte1, tBYTE byte2)
	{
		sgxCompileError("No Endian is set - This function may return incorrect results for your platform");
		return (byte1 << 8) | byte2;
	}

	SGX_INLINE tUINT16 sgxMakeInt16(tBYTE *ptr)
	{
		sgxCompileError("No Endian is set - This function may return incorrect results for your platform");
	   return (*(ptr+0) << 8) | *(ptr+1);
	}

	SGX_INLINE tUINT16 sgxMakeInt32(tBYTE byte1, tBYTE byte2, tBYTE byte3, tBYTE byte4)
	{
		sgxCompileError("No Endian is set - This function may return incorrect results for your platform");
	   return (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
	}

	SGX_INLINE tUINT16 sgxMakeInt32(tBYTE *ptr)
	{
		sgxCompileError("No Endian is set - This function may return incorrect results for your platform");
		return (*(ptr+0) << 24) | (*(ptr+1) << 16) | (*(ptr+2) << 8) | *(ptr+4);
	}


#endif

#endif /* SGX_CORE_TYPES_DATATYPE_ENDIAN_HPP */
