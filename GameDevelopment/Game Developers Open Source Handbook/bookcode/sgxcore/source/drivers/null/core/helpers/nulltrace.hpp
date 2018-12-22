#ifndef SGXDRV_NULL_CORE_HELPERS_WINTRACE_HPP
#define SGXDRV_NULL_CORE_HELPERS_WINTRACE_HPP	1

class CNullOutput {
public:
	static void Create();
	static void Trace(const char *pMessage);
};


#endif /* SGXDRV_NULL_CORE_HELPERS_WINTRACE_HPP */
