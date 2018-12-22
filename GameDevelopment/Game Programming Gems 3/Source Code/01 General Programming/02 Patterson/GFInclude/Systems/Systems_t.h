/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// Systems_t.h: interface for the Systems_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYSTEMS_T_H__7F8E83C3_FDF0_4018_858B_69A00ADB22CD__INCLUDED_)
#define AFX_SYSTEMS_T_H__7F8E83C3_FDF0_4018_858B_69A00ADB22CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ProcessInfo_t;
class LogSys_t;
class ErrorSys_t;
class TimeSys_t;
class FactorySys_t;
class ResourceSys_t;
class TaskSys_t;
class WindowSys_t;
class FrameSys_t;
class InputSys_t;
class VisualSys_t;
class AudioSys_t;
class NetworkSys_t;

class Systems_t  
{
public:
	Systems_t() {};
	virtual ~Systems_t() {};

	virtual bool Init( ProcessInfo_t *pProcessInfo ) = 0;
	virtual void Shutdown() = 0;

	virtual void Process() = 0;

	virtual ProcessInfo_t * GetProcessInfo() = 0;
	virtual LogSys_t * GetLogSys() = 0;
	virtual ErrorSys_t * GetErrorSys() = 0;
	virtual TimeSys_t * GetTimeSys() = 0;
	virtual FactorySys_t * GetFactorySys() = 0;
	virtual ResourceSys_t * GetResourceSys() = 0;
	virtual TaskSys_t * GetTaskSys() = 0;
	virtual WindowSys_t * GetWindowSys() = 0;
	virtual FrameSys_t * GetFrameSys() = 0;
	virtual InputSys_t * GetInputSys() = 0;
	virtual VisualSys_t * GetVisualSys() = 0;
	virtual AudioSys_t * GetAudioSys() = 0;
	virtual NetworkSys_t * GetNetworkSys() = 0;

	virtual void SetVisualSys( VisualSys_t *pVisualSys ) = 0;
};

#endif // !defined(AFX_SYSTEMS_T_H__7F8E83C3_FDF0_4018_858B_69A00ADB22CD__INCLUDED_)
