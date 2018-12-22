/***
 * File:   new_on.h - Header File
 *         -----------------------------------------------------
 * Author: Peter Dalton
 * Date:   4/10/2001 2:08:48 PM
 *
 * Description:
			This header allows for the Memory Manager to be turned on/off seamlessly.  Including
			this header will turn the Memory Manager on.
 *
 * Copyright (C) Peter Dalton, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied warranties. You may freely copy 
 * and compile this source into applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 *                  "Portions Copyright (C) Peter Dalton, 2001"
 */

/*******************************************************************************************/
// These #defines are the core of the memory manager.  This overrides standard memory 
// allocation and de-allocation routines and replaces them with the memory manager's versions.
// This allows for memory tracking and statistics to be generated.

#define new              new( __FILE__, __LINE__ )
#define delete           (setOwner( __FILE__, __LINE__ ), false) ? setOwner( "", 0 ) : delete
#define malloc(sz)       AllocateMemory(   __FILE__, __LINE__, sz,     MM_MALLOC       )
#define calloc(num, sz)  AllocateMemory(   __FILE__, __LINE__, sz*num, MM_CALLOC       )
#define realloc(ptr, sz) AllocateMemory(   __FILE__, __LINE__, sz,     MM_REALLOC, ptr )
#define free(sz)         deAllocateMemory( __FILE__, __LINE__, sz,     MM_FREE         )

// ***** End of new_on.h
/*******************************************************************************************/
/*******************************************************************************************/