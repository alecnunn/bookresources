/* Copyright (C) Bryon Hapgood, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Bryon Hapgood, 2001"
 */
/*	зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©
	Ё STACK.H					THREADS (C) 1997 - 2001 Bryon Hapgood. All rights reserved.	Ё
	цддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд╢
	Ё Revision History:																		Ё
	Ё	3/9/01	Started																		Ё
	Ё																						Ё
	Ё																						Ё
	Ё																						Ё
	Ё																						Ё
	Ё																						Ё
	Ё																						Ё
	Ё																						Ё
	юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды	*/

#ifndef _STACK_H_
#define _STACK_H_

	#include <types.h>

/*	зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©
	Ё Register context.																		Ё
	юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды	*/

	struct SAFE{
		void*__eax;
		void*__ebx;
		void*__esi;
		void*__edi;
		void*__esp;
		void*__ebp;
		void*__ret;
	};

/*	зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©
	Ё Stack winding functions.																Ё
	юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды	*/

	void SafeEnter(SAFE&,...);
	void SafeExit(SAFE&);

#endif
