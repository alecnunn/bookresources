/*
s_p_oneil@hotmail.com
Copyright (c) 2000, Sean O'Neil
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
* Neither the name of this project nor the names of its contributors
  may be used to endorse or promote products derived from this software
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#include "Master.h"
#define INITGUID
#include "DXUtil.h"

bool CInputEngine::Attach()
{
	// Initialize COM
	HRESULT hResult;
	if(FAILED(hResult = CoInitialize(NULL)))
		return false;

	if(FAILED(hResult = CoCreateInstance(CLSID_DirectInput, NULL, CLSCTX_INPROC_SERVER, IID_IDirectInput, (void **)&m_pDirectInput)) ||
	   FAILED(hResult = m_pDirectInput->Initialize(GetApp()->GetInstanceHandle(), DIRECTINPUT_VERSION)))
		return false;

	if(FAILED(m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, NULL)) ||
	   FAILED(m_pKeyboard->SetDataFormat(&c_dfDIKeyboard)) ||
	   FAILED(m_pKeyboard->SetCooperativeLevel(GetApp()->m_hWnd, DISCL_NONEXCLUSIVE  | DISCL_FOREGROUND)))
		return false;

	if(FAILED(m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pMouse, NULL)) ||
	   FAILED(m_pMouse->SetDataFormat(&c_dfDIMouse)) ||
	   FAILED(m_pMouse->SetCooperativeLevel(GetApp()->m_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND)))
		return false;

	return true;
}

void CInputEngine::Detach()
{
	RELEASE(m_pMouse);
	RELEASE(m_pKeyboard);
	RELEASE(m_pDirectInput);
	CoUninitialize();
}

void CInputEngine::Pause()
{
	m_pMouse->Unacquire();
	m_pKeyboard->Unacquire();
}

bool CInputEngine::Restore()
{
	if(FAILED(m_pKeyboard->Acquire()) || FAILED(m_pMouse->Acquire()))
		return false;
	if(FAILED(m_pKeyboard->GetDeviceState(256, m_kState)) ||
	   FAILED(m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_mState)))
		return false;
	return true;
}

bool CInputEngine::Poll()
{
	char kState[256];
	DIMOUSESTATE mState = m_mState;
	memcpy(kState, m_kState, 256);
	if(FAILED(m_pKeyboard->GetDeviceState(256, m_kState)) ||
	   FAILED(m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_mState)))
		return false;

	// Look for key state changes, and notify any listeners
	for(int i=0; i<256; i++)
	{
		if((kState[i] & 0x80) != (m_kState[i] & 0x80))
		{
			if(m_kState[i] & 0x80)
			{
				for(CPtrListNode *pNode = m_iplKeyboardListeners.GetHead(); pNode->IsInList(); pNode = pNode->GetNext())
					((IKeyboardListener *)pNode->GetData())->OnKeyDown(i);
			}
			else
			{
				for(CPtrListNode *pNode = m_iplKeyboardListeners.GetHead(); pNode->IsInList(); pNode = pNode->GetNext())
					((IKeyboardListener *)pNode->GetData())->OnKeyUp(i);
			}
		}
	}

	// Look for mouse button state changes, and notify any listeners
	for(i=0; i<3; i++)
	{
		if((mState.rgbButtons[i] & 0x80) != (m_mState.rgbButtons[i] & 0x80))
		{
			if(m_mState.rgbButtons[i] & 0x80)
			{
				for(CPtrListNode *pNode = m_iplMouseListeners.GetHead(); pNode->IsInList(); pNode = pNode->GetNext())
					((IMouseListener *)pNode->GetData())->OnMouseButtonDown(i);
			}
			else
			{
				for(CPtrListNode *pNode = m_iplMouseListeners.GetHead(); pNode->IsInList(); pNode = pNode->GetNext())
					((IMouseListener *)pNode->GetData())->OnMouseButtonUp(i);
			}
		}
	}
	return true;
}
