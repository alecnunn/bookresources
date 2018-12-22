/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: Mouse.cpp
 *    Desc: Wrapper of a DirectInput mouse object
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#include "stdafx.h"
#include "InputLayer.h"
#include "Window.h"

#include "Mouse.h"

cMouse::cMouse(  HWND hWnd, bool bExclusive )
{
	m_pTarget = NULL;

	HRESULT hr;

	/**
	 * Create the device
	 *
	 */
	hr = Input()->GetDInput()->CreateDevice( GUID_SysMouse, &m_pDevice, NULL );
	if( FAILED( hr ))
	{
		throw cGameError("[cMouse::Init]: Couldn't create the device!\n");
	}

	/**
	 * Set the data format
	 */
	hr = m_pDevice->SetDataFormat(&c_dfDIMouse);
 	if( FAILED( hr ))
	{
		SafeRelease( m_pDevice );
		throw cGameError("[cMouse::Init]: SetDataFormat failed\n");
	}

	/**
	 * Set the cooperative level
	 */
	if( bExclusive )
	{
		hr = m_pDevice->SetCooperativeLevel( hWnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND );
	}
	else
	{
		hr = m_pDevice->SetCooperativeLevel( hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	}

	if( FAILED( hr ))
	{
		SafeRelease( m_pDevice );
		throw cGameError("[cMouse::Init]: SetCooperativeLevel failed\n");
	}


	m_lastState.lX = 0;
	m_lastState.lY = 0;
	m_lastState.lZ = 0;
	m_lastState.rgbButtons[0] = 0;
	m_lastState.rgbButtons[1] = 0;
	m_lastState.rgbButtons[2] = 0;
	m_lastState.rgbButtons[3] = 0;
}

cMouse::~cMouse()
{
	if( m_pDevice )
	{
		m_pDevice->Unacquire();
		SafeRelease( m_pDevice );
	}
}


void cMouse::SetReceiver( iMouseReceiver* pTarget )
{
	m_pTarget = pTarget;
}


eResult cMouse::Update()
{
	DIMOUSESTATE currState;
    HRESULT  hr; 
 
    hr = m_pDevice->Poll(); 
	hr = m_pDevice->GetDeviceState( sizeof(DIMOUSESTATE), (void*)&currState );

    if( FAILED(hr) )
    { 
		hr = m_pDevice->Acquire();
		if( FAILED( hr ) )
		{
			return resFailed;
		}

	    hr = m_pDevice->Poll(); 
	    hr = m_pDevice->GetDeviceState( sizeof(DIMOUSESTATE),(void*)&currState ); 
		if( FAILED( hr ) )
		{
			return resFailed;
		}
    } 
	if( m_pTarget )
	{
		int dx = currState.lX;
		int dy = currState.lY;
		if( dx || dy )
		{
			m_pTarget->MouseMoved( dx, dy );
		}
		if( currState.rgbButtons[0] & 0x80 )
		{
			// the button got pressed.
			m_pTarget->MouseButtonDown( 0 );
		}
		if( currState.rgbButtons[1] & 0x80 )
		{
			// the button got pressed.
			m_pTarget->MouseButtonDown( 1 );
		}
		if( currState.rgbButtons[2] & 0x80 )
		{
			// the button got pressed.
			m_pTarget->MouseButtonDown( 2 );
		}
		if( !(currState.rgbButtons[0] & 0x80) && (m_lastState.rgbButtons[0] & 0x80) )
		{
			// the button got released.
			m_pTarget->MouseButtonUp( 0 );
		}
		if( !(currState.rgbButtons[1] & 0x80) && (m_lastState.rgbButtons[1] & 0x80) )
		{
			// the button got released.
			m_pTarget->MouseButtonUp( 1 );
		}
		if( !(currState.rgbButtons[2] & 0x80) && (m_lastState.rgbButtons[2] & 0x80) )
		{
			// the button got released.
			m_pTarget->MouseButtonUp( 2 );
		}
	}
	m_lastState = currState;
	return resAllGood;
}


eResult cMouse::Acquire()
{
	HRESULT hr = m_pDevice->Acquire();
	if( FAILED(hr) )
	{
		return resFailed;
	}
	return resAllGood;
}


void cMouse::UnAcquire()
{
	m_pDevice->Unacquire();
}
