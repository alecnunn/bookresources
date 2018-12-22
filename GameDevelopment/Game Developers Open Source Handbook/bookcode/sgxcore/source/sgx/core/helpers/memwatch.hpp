#ifndef SGX_CORE_HELPERS_MEMWATCH_HPP
#define SGX_CORE_HELPERS_MEMWATCH_HPP	1
	
#if defined(SGX_DEBUG) || defined(SGX_FORCE_MEMORY_HELPERS)

// Initialisation of individual memory blocks
void sgxRegisterMemoryBlock(void *ptr, tMEMSIZE size);

// Validation
void sgxCheckCheckSums();
void sgxCheckForOverwrites(void *ptr, tMEMSIZE size);

template <typename T>
void sgxRegisterMemoryBlock(const T &var)
{
    sgxRegisterMemoryBlock((void *)&var, sizeof(T));
}

#else	// SGX_RELEASE

  #define	sgxRegisterMemoryBlock	0
  #define	sgxCheckCheckSums		0
  #define	sgxCheckForOverwrites	0

#endif

#endif /* SGX_CORE_HELPERS_MEMWATCH_HPP */
