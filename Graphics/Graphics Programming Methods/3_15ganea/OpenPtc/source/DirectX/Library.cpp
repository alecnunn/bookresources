//
// DirectX Library class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include "Core/Log.h"
#include "Core/Error.h"
#include "DirectX/Check.h"
#include "DirectX/Library.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif




DirectXLibrary::DirectXLibrary()
{
    // defaults
    m_lpDD    = 0;
    m_lpDD2   = 0;
    m_library = 0;
    
    try
    {
        // log library loading
        LOG("loading ddraw.dll");

        // load "ddraw.dll"
        m_library = (HMODULE) LoadLibrary("ddraw.dll");
        if (!m_library) throw Error("could not load ddraw.dll");

        // log function importing
        LOG("importing DirectDrawCreate");

        // "DirectDrawCreate" function pointer type
        typedef HRESULT (WINAPI *FUNCTION) (GUID FAR *lpGUID,LPDIRECTDRAW FAR *lplpDD,IUnknown FAR *pUnkOuter);

        // get directdraw create function address
        FUNCTION DirectDrawCreate = (FUNCTION) GetProcAddress(m_library,"DirectDrawCreate");
        if (!DirectDrawCreate) throw Error("could not get address of DirectDrawCreate");

        // log lpDD creation
        LOG("creating lpDD");

        // create directdraw interface
        DirectXCheck(DirectDrawCreate(0,&m_lpDD,0));

        // setup IID_IDirectDraw2
        GUID IID_IDirectDraw2;
        IID_IDirectDraw2.Data1 = 3014063072;
        IID_IDirectDraw2.Data2 = 11075;
        IID_IDirectDraw2.Data3 = 4559;
        IID_IDirectDraw2.Data4[0] = 162;
        IID_IDirectDraw2.Data4[1] = 222;
        IID_IDirectDraw2.Data4[2] = 0;
        IID_IDirectDraw2.Data4[3] = 170;
        IID_IDirectDraw2.Data4[4] = 0;
        IID_IDirectDraw2.Data4[5] = 185;
        IID_IDirectDraw2.Data4[6] = 51;
        IID_IDirectDraw2.Data4[7] = 86;

        // log ddraw interface query
        LOG("querying lpDD2");

        // query IDirectDraw2 interface
        DirectXCheck(m_lpDD->QueryInterface(IID_IDirectDraw2,(LPVOID*)&m_lpDD2));
    }
    catch (Error &error)
    {
        // close
        close();

        // rethrow
        throw Error("could not initialize ddraw",error);
    }
}


DirectXLibrary::~DirectXLibrary()
{
    // close
    close();
}




void DirectXLibrary::close()
{
    // check lpDD2
    if (m_lpDD2)
    {
        // log lpDD2 release
        LOG("releasing lpDD2");

        // release interface
        m_lpDD2->Release();

        // clear pointer
        m_lpDD2 = 0;
    }

    // check lpDD
    if (m_lpDD)
    {
        // log lpDD release
        LOG("releasing lpDD");

        // release interface
        m_lpDD->Release();

        // clear pointer
        m_lpDD = 0;
    }

    // check library
    if (m_library)
    {
        // log library close
        LOG("closing ddraw.dll");

        // free library module
        FreeLibrary(m_library);

        // clear library
        m_library = 0;
    }
}




LPDIRECTDRAW DirectXLibrary::lpDD() const
{
    // get lpDD
    return m_lpDD;
}


LPDIRECTDRAW2 DirectXLibrary::lpDD2() const
{
    // get lpDD2
    return m_lpDD2;
}
