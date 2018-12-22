#ifndef SGX_CORE_SGXLIB_CTYPE_HPP
#define SGX_CORE_SGXLIB_CTYPE_HPP

//
// Wrappers for the stdlib and ctype functions
//
template <typename T>	T sgxToUpper(T c) { return toupper(c); }
template <typename T>	T sgxToLower(T c) { return tolower(c); }
template <typename T>	T sgxIsUpper(T c) { return isupper(c); }
template <typename T>	T sgxIsLower(T c) { return islower(c); }
template <typename T>	T sgxIsDigit(T c) { return isdigit(c); }
template <typename T>	T sgxIsXDigit(T c) { return isxdigit(c); }
template <typename T>	T sgxIsSpace(T c) { return isspace(c); }
template <typename T>	T sgxIsPunct(T c) { return ispunct(c); }
template <typename T>	T sgxIsAlNum(T c) { return isalnum(c); }
template <typename T>	T sgxIsPrint(T c) { return isprint(c); }

#endif /* SGX_CORE_SGXLIB_CTYPE_HPP */
