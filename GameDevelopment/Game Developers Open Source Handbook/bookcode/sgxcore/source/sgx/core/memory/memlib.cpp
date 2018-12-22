#include "sgx/core/core.hpp"
#include "sgx/core/helpers/helpers.hpp"
#include "sgx/core/helpers/memwatch.hpp"

#define sgxCheckForOverwrites

void *sgxMemmove(void *pDest, const void *pSrc, tMEMSIZE iSize)
{
    sgxCheckForOverwrites(pDest, iSize);
    return memmove(pDest, pSrc, iSize);
}

void *sgxMemcpy(void *pDest, const void *pSrc, tMEMSIZE iSize)
{
    tBYTE *pbDest = (tBYTE *)pDest;
    tBYTE *pbSrc  = (tBYTE *)pSrc;

    if ( ( pbSrc < pbDest &&  pbSrc + iSize > pbDest ) || 
         (pbDest < pbSrc  && pbDest + iSize > pbSrc) ) {
        sgxTrace("sgxMemcpy overlaps! We should use sgxMemmove");
        return sgxMemmove(pDest, pSrc, iSize);
    }

    sgxCheckForOverwrites(pDest, iSize);
    return memcpy(pDest, pSrc, iSize);
}

void *sgxMemset(void *pSrc, int iCharacter, tMEMSIZE iNum)
{
    sgxCheckForOverwrites(pSrc, iNum);
    return memset(pSrc, iCharacter, iNum); 
}

int sgxMemcmp(const void *pSrc1, const void *pSrc2, tMEMSIZE iNum) 
{ 
    return memcmp(pSrc1, pSrc2, iNum); 
}

const void *sgxMemchr(const void *pSrc,  int iCharacter, tMEMSIZE iNum) 
{ 
    return memchr(pSrc, iCharacter, iNum); 
}

void sgxMemSwap(const void *pSrc1, const void *pSrc2, tMEMSIZE iNum) 
{
    tBYTE *pbSrc1 = (tBYTE *)pSrc1;
    tBYTE *pbSrc2  = (tBYTE *)pSrc2;
	tBYTE t;
	tMEMSIZE count = iNum;

	while(count--) {
		t = *pbSrc1;
		*pbSrc1 = *pbSrc2;
		*pbSrc2 = t;
		++pbSrc1;
		++pbSrc2;
	}
}
