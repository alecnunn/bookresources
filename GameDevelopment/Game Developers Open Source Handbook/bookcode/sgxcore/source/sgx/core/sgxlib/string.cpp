#include "sgx/core/sgxlib/string.hpp"
#include "sgx/core/helpers/memwatch.hpp"
#define _CRT_SECURE_NO_DEPRECATE
#include <string.h>

//
// C string wrappers
//
size_t sgxStrlen(const char *pString)
{	
	if (!pString) {
		return 0;
	}
    return strlen(pString);
}

char *sgxStrcpy(char *pDest, const char *pSrc)
{	
    sgxCheckForOverwrites(pDest, sgxStrlen(pSrc));
    return strcpy(pDest, pSrc);
}

char *sgxStrcat(char *pDest, const char *pSrc)
{	
    sgxCheckForOverwrites(pDest, sgxStrlen(pSrc));
    return strcat(pDest, pSrc);
}

int sgxStrcmp(const char *pSrc1, const char *pSrc2)
{	
    return strcmp(pSrc1, pSrc2);
}

int sgxStricmp(const char *pSrc1, const char *pSrc2)
{	
    return stricmp(pSrc1, pSrc2);
}

const char *sgxStrchr(const char *pStr, int c)
{
	return strchr(pStr, c);
}
