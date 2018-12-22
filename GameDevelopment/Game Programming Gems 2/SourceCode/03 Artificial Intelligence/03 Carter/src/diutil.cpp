//-----------------------------------------------------------------------------
// File: DIUtil.cpp
//
// Desc: DirectInput framework class using semantic mapping.  Feel free to use 
//       this class as a starting point for adding extra functionality.
//
// Copyright (C) 1995-2000 Microsoft Corporation. All Rights Reserved.
//-----------------------------------------------------------------------------
#define STRICT
#include <basetsd.h>
#include <tchar.h>
#include <stdio.h>
#include "DIUtil.h"
#include "DXUtil.h"



 

//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
CInputDeviceManager::CInputDeviceManager()
{
    //call CoInitialize();
    CoInitialize(NULL);

    m_dwNumDevices = 0;
    m_pDI          = NULL;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
CInputDeviceManager::~CInputDeviceManager()
{
    // Release() all devices
    for( DWORD i=0; i<m_dwNumDevices; i++ )
    {
        m_pdidDevices[i]->Unacquire();
        m_pdidDevices[i]->Release();
        m_pdidDevices[i] = NULL;
    }

    // Release() base object
    SAFE_RELEASE( m_pDI );

    //call CoUninitialize();
    CoUninitialize();
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT CInputDeviceManager::GetDevices( LPDIRECTINPUTDEVICE8** ppDevice, 
                                         DWORD* pdwCount )
{
    if( NULL==ppDevice || NULL==pdwCount )
        return E_INVALIDARG;

    (*ppDevice) = m_pdidDevices;
    (*pdwCount) = m_dwNumDevices;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT CInputDeviceManager::AddDevice( const DIDEVICEINSTANCE* pdidi, 
                                        const LPDIRECTINPUTDEVICE8 pdidDevice )
{
    HRESULT hr;
    DWORD   dwDeviceType = pdidi->dwDevType;

    pdidDevice->Unacquire();

    // Set the device's coop level
    if( GET_DIDEVICE_TYPE(dwDeviceType) == DI8DEVTYPE_MOUSE )
        hr = pdidDevice->SetCooperativeLevel( m_hWnd, DISCL_EXCLUSIVE|DISCL_FOREGROUND );
    else
        hr = pdidDevice->SetCooperativeLevel( m_hWnd, DISCL_NONEXCLUSIVE|DISCL_BACKGROUND );
    if( FAILED(hr) )
        return hr;

    // Set relative mode for mouse
    if( GET_DIDEVICE_TYPE(dwDeviceType) == DI8DEVTYPE_MOUSE )
    {
        DIPROPDWORD dipdw;
        dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
        dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        dipdw.diph.dwObj        = 0;
        dipdw.diph.dwHow        = DIPH_DEVICE;
        dipdw.dwData            = DIPROPAXISMODE_REL;
        hr = pdidDevice->SetProperty( DIPROP_AXISMODE, &dipdw.diph );
    }

    m_pdidDevices[m_dwNumDevices] = pdidDevice;
    m_pdidDevices[m_dwNumDevices]->AddRef();

    hr = m_pdidDevices[m_dwNumDevices]->BuildActionMap( &m_diaf, m_strUserName, 0 );
    hr = m_pdidDevices[m_dwNumDevices]->SetActionMap( &m_diaf, m_strUserName, 0 );

    m_dwNumDevices++;

    // Continue enumerating suitable devices
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
BOOL CALLBACK EnumSuitableDevicesCB( LPCDIDEVICEINSTANCE pdidi, 
                                     LPDIRECTINPUTDEVICE8A pdidDevice, 
                                     DWORD dwFlags, DWORD dwDeviceRemaining,
                                     VOID* pContext )
{
    // Add the device to the device manager's internal list
    ((CInputDeviceManager*)pContext)->AddDevice( pdidi, pdidDevice );

    // Continue enumerating suitable devices
    return DIENUM_CONTINUE;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT CInputDeviceManager::SetActionFormat( DIACTIONFORMAT& diaf, BOOL bReenumerate )
{
    HRESULT hr = S_OK;

    // Store the new action format
    m_diaf = diaf;

    // Only destroy and re-enumerate devices if the caller explicitly wants to. The 
    // device list may be used within a loop, and kicking off an enumeration while 
    // the device array is in use would cause problems.
    if( bReenumerate )
    {
        // Cleanup any previously enumerated devices
        for( DWORD i=0; i<m_dwNumDevices; i++ )
        {
            m_pdidDevices[i]->Unacquire();
            m_pdidDevices[i]->Release();
            m_pdidDevices[i] = NULL;
        }
        m_dwNumDevices = 0;

        // Enumerate suitable DirectInput devices
        hr = m_pDI->EnumDevicesBySemantics( m_strUserName, &m_diaf, 
                                            EnumSuitableDevicesCB, this, 0L );
    }
    else // Just apply the new maps.
    {
        // Devices must be unacquired to have a new action map set.
        UnacquireDevices();

        // Apply the new action map to the current devices.
        for( DWORD i=0; i<m_dwNumDevices; i++ )
        {
            m_pdidDevices[i]->BuildActionMap( &m_diaf, m_strUserName, 0 );
            m_pdidDevices[i]->SetActionMap( &m_diaf, m_strUserName, 0 );
        }
    }

    if( FAILED(hr) )
        return hr;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Create()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CInputDeviceManager::Create( HWND hWnd, TCHAR* strUserName, 
                                     DIACTIONFORMAT& diaf )
{
    HRESULT hr;

    // Store data
    m_hWnd        = hWnd;
    m_strUserName = strUserName;
    
    // Create the base DirectInput object
    hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
                              IID_IDirectInput8, (VOID**)&m_pDI, NULL );
    if( FAILED(hr) )
        return hr;

    SetActionFormat( diaf, TRUE );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Create()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CInputDeviceManager::ConfigureDevices( HWND hWnd, IUnknown* pSurface,
                                               VOID* ConfigureDevicesCB,
                                               DWORD dwFlags )
{
    HRESULT hr;

    // Initialize all the colors here
    DICOLORSET dics;
    ZeroMemory(&dics, sizeof(DICOLORSET));
    dics.dwSize = sizeof(DICOLORSET);

    // Fill in all the params
    DICONFIGUREDEVICESPARAMS dicdp;
    ZeroMemory(&dicdp, sizeof(dicdp));
    dicdp.dwSize = sizeof(dicdp);
    dicdp.dwcFormats     = 1;
    dicdp.lprgFormats    = &m_diaf;
    dicdp.hwnd           = hWnd;
    dicdp.lpUnkDDSTarget = pSurface;

    if( m_strUserName )
    {
        dicdp.dwcUsers       = 1;
        dicdp.lptszUserNames = m_strUserName;
    }

    // Unacquire the devices so that mouse doesn't control the game while in control panel
    UnacquireDevices();

    hr = m_pDI->ConfigureDevices( (LPDICONFIGUREDEVICESCALLBACK)ConfigureDevicesCB, 
                                  &dicdp, dwFlags, NULL );

    if( dwFlags & DICD_EDIT )
    {
        // Re-set up the devices
        hr = SetActionFormat( m_diaf, TRUE );
    }

    return hr;
}



//-----------------------------------------------------------------------------
// Name: UnacquireDevices()
// Desc:
//-----------------------------------------------------------------------------
VOID CInputDeviceManager::UnacquireDevices()
{
    for( DWORD i=0; i<m_dwNumDevices; i++ )
        m_pdidDevices[i]->Unacquire();
}


