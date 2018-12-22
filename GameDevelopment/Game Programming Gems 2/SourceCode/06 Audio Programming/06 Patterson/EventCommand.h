/* Copyright (C) Scott Patterson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2001"
 */

// EventCommand.h

enum enumEventType
{
    EVENT_TYPE_NOTEOFF,
    EVENT_TYPE_NOTEON,
	EVENT_TYPE_PAN,
	EVENT_TYPE_SETINSTRUMENT,
    EVENT_TYPE_TRACKEND,
    EVENT_TYPE_COUNT
};
