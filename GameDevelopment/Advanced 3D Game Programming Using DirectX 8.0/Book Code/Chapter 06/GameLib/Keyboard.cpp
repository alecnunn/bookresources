/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: Keyboard.cpp
 *    Desc: Wrapper of a DirectInput keyboard object
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#include "stdafx.h"
#include "InputLayer.h"
#include "window.h"

#include <stack>
using namespace std;

#include "Keyboard.h"


cKeyboard::cKeyboard( HWND hWnd )
{
	m_pTarget = NULL;
	
	HRESULT hr;

	/**
	 * Get the DInput interface pointer
	 */
	LPDIRECTINPUT8 pDI = Input()->GetDInput();

	/**
	 * Create the keyboard device
	 *
     */
	hr = Input()->GetDInput()->CreateDevice( GUID_SysKeyboard, &m_pDevice, NULL );
    if( FAILED(hr) )
    { 
        throw cGameError("Keyboard could not be created\n");
    } 
 
	/**
	 * Set the keyboard data format
	 */
    hr = m_pDevice->SetDataFormat(&c_dfDIKeyboard); 
    if( FAILED(hr) )
    { 
		SafeRelease( m_pDevice );
        throw cGameError("Keyboard could not be created\n");
    } 
 
    /**
	 * Set the cooperative level 
	 */
    hr = m_pDevice->SetCooperativeLevel(
		hWnd,
		DISCL_FOREGROUND | DISCL_NONEXCLUSIVE); 
    if( FAILED(hr) )
    { 
		SafeRelease( m_pDevice );
        throw cGameError("Keyboard coop level could not be changed\n");
    } 

	memset( m_keyState, 0, 256*sizeof(bool) );	
}

cKeyboard::~cKeyboard()
{
	if( m_pDevice )
	{
		m_pDevice->Unacquire();
		SafeRelease( m_pDevice );
	}
}



void cKeyboard::SetReceiver( iKeyboardReceiver* pTarget )
{
	// Set the new target.
	m_pTarget = pTarget;
}


bool cKeyboard::Poll( int key )
{
	// stuff goes in here.
	if( m_keyState[key] & 0x80 )
		return true;
	return false;
}


eResult cKeyboard::Update()
{
	char     newState[256]; 
    HRESULT  hr; 
 
    hr = m_pDevice->Poll(); 
    hr = m_pDevice->GetDeviceState(sizeof(newState),(LPVOID)&newState); 

    if( FAILED(hr) )
    { 
		hr = m_pDevice->Acquire();
		if( FAILED( hr ) )
		{
			return resFailed;
		}

	    hr = m_pDevice->Poll(); 
	    hr = m_pDevice->GetDeviceState(sizeof(newState),(LPVOID)&newState); 
		if( FAILED( hr ) )
		{
			return resFailed;
		}
    } 


	if( m_pTarget )
	{
		int i;
		for( i=0; i< 256; i++ )
		{
			if( m_keyState[i] != newState[i] )
			{
				// Something happened to this key since the last time we checked
				if( !(newState[i] & 0x80) )
				{
					// It was Released
					m_pTarget->KeyUp( i );
				}
				else
				{
					// Do nothing; it was just pressed, it'll get a keydown 
					// in a bit, and we dont' want to send the signal to the 
					// input target twice
				}
			}

			// copy the state over (we could do a memcpy at the end, but this
			// will have better cache performance)
			m_keyState[i] = newState[i];

			if( Poll( i ) )
			{
				// It was pressed
				m_pTarget->KeyDown( i );
			}
		}
	}
	else
	{
		// copy the new states over.
		memcpy( m_keyState, newState, 256 );
	}
 
	return resAllGood;
}


