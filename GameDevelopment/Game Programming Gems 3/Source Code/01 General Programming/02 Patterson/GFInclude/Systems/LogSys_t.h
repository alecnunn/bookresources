/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// LogSys_t.h: interface for the LogSys_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGSYS_T_H__A8D156C8_BB5B_4DEC_8414_292F4EABC364__INCLUDED_)
#define AFX_LOGSYS_T_H__A8D156C8_BB5B_4DEC_8414_292F4EABC364__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum {
	LOGSYS_CHANNEL_LOGSYS = 0x0001,
	LOGSYS_CHANNEL_ERRORSYS = 0x0002,
	LOGSYS_CHANNEL_TIMESYS = 0x0004,
	LOGSYS_CHANNEL_FACTORYSYS = 0x0008,
	LOGSYS_CHANNEL_RESOURCESYS = 0x0010,
	LOGSYS_CHANNEL_TASKSYS  = 0x0020,
	LOGSYS_CHANNEL_WINDOWSYS = 0x0040,
	LOGSYS_CHANNEL_FRAMESYS = 0x0080,
	LOGSYS_CHANNEL_INPUTSYS = 0x0100,
	LOGSYS_CHANNEL_VISUALSYS = 0x0200,
	LOGSYS_CHANNEL_AUDIOSYS = 0x0400,
	LOGSYS_CHANNEL_NETWORKSYS = 0x0800,
	LOGSYS_CHANNEL_TASK = 0x1000,
	LOGSYS_CHANNEL_LOGICLAYER = 0x2000,
	LOGSYS_CHANNEL_AVLAYER = 0x4000,
};

enum {
	LOGSYS_DETAIL_ERROR,
	LOGSYS_DETAIL_LOW,
	LOGSYS_DETAIL_MED1,
	LOGSYS_DETAIL_MED2,
	LOGSYS_DETAIL_MED3,
	LOGSYS_DETAIL_HIGH,
};

enum {
	LOGSYS_OUTPUTMODE_NONE,
	LOGSYS_OUTPUTMODE_WIN32CONSOLE,
	LOGSYS_OUTPUTMODE_FILE,
};

class Systems_t;

class LogSys_t  
{
public:
	LogSys_t() {};
	virtual ~LogSys_t() {};

	virtual bool Init( Systems_t *pSystems ) = 0;
	virtual void Shutdown() = 0;

	virtual void SetOutputMode(int nOutputMode) = 0;
	virtual int GetOutputMode() = 0;
	virtual void SetChannelMask(int nChannelMask) = 0;
	virtual int GetChannelMask() = 0;
	virtual void SetDetailLevel(int nDetailLevel) = 0;
	virtual int GetDetailLevel() = 0;

	virtual void WriteOutput(int nChannelMask, int nDetailLevel, char* pFormat, ...) = 0;
};

#endif // !defined(AFX_LOGSYS_T_H__A8D156C8_BB5B_4DEC_8414_292F4EABC364__INCLUDED_)
