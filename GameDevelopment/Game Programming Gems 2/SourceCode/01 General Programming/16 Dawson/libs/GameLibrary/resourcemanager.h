/* Copyright (C) Bruce Dawson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Bruce Dawson, 2001"
 */
#ifndef	RESOURCEMANAGER_H
#define	RESOURCEMANAGER_H

class CSBitmap;

// The resource manager will load bitmaps as requested. 
// Resources will automatically be freed at shutdown
// or whenever it is appropriate.
// This function will never return zero unless you specify
// 'allowFailure'.
const CSBitmap* LoadBitmapImage(const char* filename, bool allowFailure = false);

#endif
