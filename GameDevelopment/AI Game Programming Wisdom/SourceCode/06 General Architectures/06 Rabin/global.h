/* Copyright (C) Steve Rabin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steve Rabin, 2001"
 */

#ifndef __GLOBAL_H__
#define __GLOBAL_H__



#define g_time Time::GetSingleton()
#define g_factory Factory::GetSingleton()
#define g_database Database::GetSingleton()
#define g_msgroute MsgRoute::GetSingleton()
#define g_debuglog DebugLog::GetSingleton()


#define INVALID_OBJECT_ID 0

typedef unsigned int objectID;



#endif	// __GLOBAL_H__