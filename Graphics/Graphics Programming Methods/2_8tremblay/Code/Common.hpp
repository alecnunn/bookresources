/*

	Visual Terrain Grid Determination Using Frustrum
	written by Christopher Tremblay
	Last revision (26 Jan / 2003)

	This source code is a complement to the book:
	"Graphics Programming Methods" by 
	Charles River Media

	All rights reserved
*/
#ifndef COMMON_HPP

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#define SCALE 150	// Debug frustrum scale

// Debugging variables
extern bool ShowFill;
extern bool ShowLines;
extern bool	ShowPlanes;
extern bool	ClipPlanes;
extern unsigned short ShowID;

// OpenGL virtual pointer
extern PFNGLLOCKARRAYSEXTPROC		glLockArraysEXT;
extern PFNGLUNLOCKARRAYSEXTPROC		glUnlockArraysEXT;

// Defines debug macros
#ifdef _DEBUG
	#ifdef WIN32
		#include <windows.h>
		#include <crtdbg.h>
		#include <stdio.h>

		#define REPORT(TXT)																				\
		{																								\
			if (MessageBox(NULL, TXT, "Error Report", MB_OKCANCEL |										\
				MB_ICONEXCLAMATION | MB_SYSTEMMODAL) == IDCANCEL) {										\
				__asm int 3h																			\
			}																							\
		}																								

		#define ASSERT(x)																				\
		{																								\
			if (!(x)) {																					\
				char txt[256];																			\
																										\
				sprintf(txt, "%s, %s, line %d", #x, __FILE__, __LINE__);								\
				_CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, "WaveWars", "ASSERT failed (%s)\n", x);	\
																										\
				REPORT(txt);																			\
			}																							\
		}																								
	#else
		#define REPORT(TXT)	
		{																								\
			fprintf(stdout, "%s\n", TXT);																\
			fflush(stderr);																				\
		}																								

		#define ASSERT(x)																				\
		{																								\
			if (!(x))																					\
			{																							\
				fprintf(	stderr,																		\
							"ASSERT failed @ (%s)"														\
							"\n\tCODE:<%s>"																\
							"\n\tFILE:<%s>"																\
							"\n\tLINE:<%d>"																\
							"\n\tMSG):<%s>"																\
							, __DATE__																	\
							, #x																		\
							, __FILE__																	\
							, __LINE__																	\
						);																				\
				fflush(stderr);																			\
			}																							\
		}																								
	#endif
#else

	#define ASSERT(x)
	#define REPORT(TXT)	
#endif

#endif
