#ifndef SGX_CORE_SGXLIB_STRING_HPP
#define SGX_CORE_SGXLIB_STRING_HPP

//
// C string wrappers
//
#define sgxSprintf  sprintf
#define sgxAtoi		atoi
#define sgxAtof		atof
#define sgxAtol		atol

size_t  sgxStrlen(const char *pString);
char *  sgxStrcpy(char *pDest, const char *pSrc);
char *  sgxStrcat(char *pDest, const char *pSrc);
int     sgxStrcmp(const char *pStr1, const char *pStr2);
int     sgxStricmp(const char *pStr1, const char *pStr2);
const char *sgxStrchr(const char *pStr1, int c);


#endif /* SGX_CORE_SGXLIB_STRING_HPP */

