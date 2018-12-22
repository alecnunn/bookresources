#ifndef SGX_CORE_STL_HPP
#define SGX_CORE_STL_HPP	1

//#define _STLP_NO_IOSTREAMS	1
#define _STLP_NO_NEW_IOSTREAMS	1

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <stdexcept>


#define sgxString       std::string
#define sgxVector		std::vector
#define sgxMap		    std::map

#endif /* SGX_CORE_STL_HPP */
