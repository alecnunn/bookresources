#ifndef SGXDRV_WINTEL32_CORE_HELPERS_WINTRACE_HPP
#define SGXDRV_WINTEL32_CORE_HELPERS_WINTRACE_HPP	1

class CWin32Output {
public:
	static void Create();
	static void Trace(const char *pMessage);
};


#endif /* SGXDRV_WINTEL32_CORE_HELPERS_WINTRACE_HPP */
