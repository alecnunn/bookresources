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

#ifndef __DXUtil_h__
#define __DXUtil_h__

#include <dinput.h>

class CInterfacePtrList : public CPtrList
{
public:
	CInterfacePtrList() : CPtrList() {}

	// WARNING! Multiple inheritance breaks if the pointer is not casted properly before this function is called
	// (Casting to IEventListener here also breaks multiple inheritance because there can be more than one IEventListener in the inheritance tree.)
	void AddListener(void *pListener)
	{
		for(CPtrListNode *pNode = GetHead(); pNode->IsInList(); pNode = pNode->GetNext())
		{
			if(pListener == pNode->GetData())
				return;
		}
		AddTail(pListener);
	}

	// WARNING! Multiple inheritance breaks if the pointer is not casted properly before this function is called
	// (Casting to IEventListener here also breaks multiple inheritance because there can be more than one IEventListener in the inheritance tree.)
	void RemoveListener(void *pListener)
	{
		for(CPtrListNode *pNode = GetHead(); pNode->IsInList(); pNode = pNode->GetNext())
		{
			if(pListener == pNode->GetData())
			{
				delete pNode;
				return;
			}
		}
	}
};

interface IKeyboardListener
{
	virtual void OnKeyDown(int nKey) = 0;
	virtual void OnKeyUp(int nKey) = 0;
};

interface IMouseListener
{
	virtual void OnMouseButtonDown(int nButton) = 0;
	virtual void OnMouseButtonUp(int nButton) = 0;
};

class CInputEngine
{
// Attributes
protected:
	CInterfacePtrList m_iplKeyboardListeners;
	CInterfacePtrList m_iplMouseListeners;

	IDirectInput *m_pDirectInput;
	IDirectInputDevice *m_pKeyboard;
	IDirectInputDevice *m_pMouse;
	char m_kState[256];
	DIMOUSESTATE m_mState;

// Operations
public:
	CInputEngine()	{ m_pKeyboard = m_pMouse = NULL; }
	bool Attach();
	void Detach();
	void Pause();
	bool Restore();
	bool Poll();
	bool GetKeyState(int n)			{ return (m_kState[n] & 0x80) != 0; }
	bool GetMouseButtonState(int n)	{ return (m_mState.rgbButtons[n] & 0x80) != 0; }
	int GetMouseXAxis()				{ return m_mState.lX; }
	int GetMouseYAxis()				{ return m_mState.lY; }
	int GetMouseZAxis()				{ return m_mState.lZ; }

	void AddKeyboardListener(IKeyboardListener *pListener)		{ m_iplKeyboardListeners.AddListener(pListener); }
	void RemoveKeyboardListener(IKeyboardListener *pListener)	{ m_iplKeyboardListeners.RemoveListener(pListener); }
	void AddMouseListener(IMouseListener *pListener)			{ m_iplMouseListeners.AddListener(pListener); }
	void RemoveMouseListener(IMouseListener *pListener)			{ m_iplMouseListeners.RemoveListener(pListener); }
};

#endif // __DXUtil_h__
