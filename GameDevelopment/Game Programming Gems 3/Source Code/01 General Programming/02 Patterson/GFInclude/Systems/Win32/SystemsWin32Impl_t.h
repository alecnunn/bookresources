/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// SystemsWin32Impl_t.h: interface for the SystemsWin32Impl_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYSTEMSWIN32IMPL_T_H__1E1E279B_4407_433E_A0E7_7772369D399D__INCLUDED_)
#define AFX_SYSTEMSWIN32IMPL_T_H__1E1E279B_4407_433E_A0E7_7772369D399D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Systems/Systems_t.h"

class SystemsWin32Impl_t : public Systems_t  
{
public:
	SystemsWin32Impl_t();
	virtual ~SystemsWin32Impl_t();

	virtual bool Init( ProcessInfo_t *pProcessInfo );
	virtual void Shutdown();

	virtual void Process();

	virtual ProcessInfo_t * GetProcessInfo() { return m_pProcessInfo; }
	virtual TimeSys_t * GetTimeSys() { return m_pTimeSys; }
	virtual FactorySys_t * GetFactorySys() { return m_pFactorySys; }
	virtual ResourceSys_t * GetResourceSys() { return m_pResourceSys; }
	virtual TaskSys_t * GetTaskSys() { return m_pTaskSys; }
	virtual LogSys_t * GetLogSys() { return m_pLogSys; }
	virtual ErrorSys_t * GetErrorSys() { return m_pErrorSys; }
	virtual WindowSys_t * GetWindowSys() { return m_pWindowSys; }
	virtual FrameSys_t * GetFrameSys() { return m_pFrameSys; }
	virtual InputSys_t * GetInputSys() { return m_pInputSys; }
	virtual VisualSys_t * GetVisualSys() { return m_pVisualSys; }
	virtual AudioSys_t * GetAudioSys() { return m_pAudioSys; }
	virtual NetworkSys_t * GetNetworkSys() { return m_pNetworkSys; }

	virtual void SetVisualSys( VisualSys_t *pVisualSys ) { m_pVisualSys = pVisualSys; }
protected:
	ProcessInfo_t *m_pProcessInfo;
	TimeSys_t *m_pTimeSys;
	FactorySys_t *m_pFactorySys;
	ResourceSys_t *m_pResourceSys;
	TaskSys_t *m_pTaskSys;
	LogSys_t *m_pLogSys;
	ErrorSys_t *m_pErrorSys;
	WindowSys_t *m_pWindowSys;
	FrameSys_t *m_pFrameSys;
	InputSys_t *m_pInputSys;
	VisualSys_t *m_pVisualSys;
	AudioSys_t *m_pAudioSys;
	NetworkSys_t *m_pNetworkSys;
};

#endif // !defined(AFX_SYSTEMSWIN32IMPL_T_H__1E1E279B_4407_433E_A0E7_7772369D399D__INCLUDED_)
