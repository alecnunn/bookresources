#ifndef SGX_CORE_MEMORY_MEMLIB_HPP
#define SGX_CORE_MEMORY_MEMLIB_HPP

void   *sgxMemmove(void *pDest, const void *pSrc, tMEMSIZE iSize);
void   *sgxMemcpy (void *pDest, const void *pSrc, tMEMSIZE iSize);
void   *sgxMemset (void *pSrc, int iCharacter, tMEMSIZE iNum);
int     sgxMemcmp (const void *pSrc1, const void *pSrc2, tMEMSIZE iNum);
const void   *sgxMemchr (const void *pSrc,  int iCharacter, tMEMSIZE iNum);
void	sgxMemSwap(const void *pSrc1, const void *pSrc2, tMEMSIZE iNum) ;

#endif /* SGX_CORE_MEMORY_MEMLIB_HPP */
