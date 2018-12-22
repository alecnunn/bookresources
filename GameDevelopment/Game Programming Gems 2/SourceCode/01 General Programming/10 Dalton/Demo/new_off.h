/***
 * File:   new_off.h - Header File
 *         -----------------------------------------------------
 * Author: Peter Dalton
 * Date:   4/10/2001 2:06:41 PM
 *
 * Description:
			This header allows for the Memory Manager to be turned on/off seamlessly.  Including
			this header will turn the Memory Manager off.
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
// Undefine all symbols defined by the Memory Manager.  This is important when including 
// header files that also overwrite the new/delete operators to avoid circular definitions
// that will result in unexpected behavior and syntax errors from the compiler.
 
#undef new
#undef delete
#undef malloc
#undef calloc
#undef realloc
#undef free

// ***** End of new_off.h
/*******************************************************************************************/
/*******************************************************************************************/