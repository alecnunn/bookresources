#ifndef SGX_CORE_HELPERS_SAFETY_HPP
#define SGX_CORE_HELPERS_SAFETY_HPP	1

#if SGX_DEBUG
  void sgxSafetyCheck(void);
#else // SGX_RELEASE
  #define sgxSafetyCheck	0
#endif

#endif /* SGX_CORE_HELPERS_SAFETY_HPP */
