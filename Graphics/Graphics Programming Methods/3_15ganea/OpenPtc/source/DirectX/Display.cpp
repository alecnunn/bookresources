//
// DirectX Display class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include "Core/Log.h"
#include "Core/Error.h"
#include "DirectX/Check.h"
#include "DirectX/Display.h"
#include "DirectX/Translate.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif




DirectXDisplay::DirectXDisplay()
{
    // setup data
    m_open = false;
    m_fullscreen = false;
    m_ddraw = 0;
    m_window = 0;
    m_foreground = 0;
}


DirectXDisplay::~DirectXDisplay()
{
    // close
    close();
}




void DirectXDisplay::setup(LPDIRECTDRAW2 ddraw)
{
    // log display setup
    LOG("setting up display lpDD2");

    // setup ddraw
    m_ddraw = ddraw;

    // temporary platform dependent information fudge!
    wsprintf(m_information,"windows version x.xx.x\nddraw version x.xx\ndisplay driver name xxxxx\ndisplay driver vendor xxxxx\ncertified driver? x\n");

    // zero modes count
    m_modes_count = 0;

    // enumerate all display modes
    DirectXCheck(m_ddraw->EnumDisplayModes(0,0,this,(LPDDENUMMODESCALLBACK)callback));

    // os version info
    OSVERSIONINFO version;
    
    // setup version structure size member
    version.dwOSVersionInfoSize = sizeof version;

    // get os version info
    if (GetVersionEx(&version))
    {
        // check for Windows 95/98 platforms
        if (version.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS)
        {
            // log win95/98 detection
            LOG("detected windows 95//98");

            // clear found flag
            bool found = false;

            // iterate through mode list
            for (int i=0; i<m_modes_count; i++)
            {
                // search for 320x200x8 mode
                if (m_modes[i].width()==320 && m_modes[i].height()==200 && m_modes[i].format()==Format(8)) found = true;
            }

            // check result
            if (!found)
            {
                // log mode list addition
                LOG("adding 320x200x8 to mode list");

                // add 320x200x8 to mode list
                m_modes[m_modes_count] = Mode(320,200,8);

                // increment mode count
                m_modes_count++;
            }

            // clear flag
            found = false;

            // iterate through mode list
            for (i=0; i<m_modes_count; i++)
            {
                // search for 320x240x8 mode
                if (m_modes[i].width()==320 && m_modes[i].height()==240 && m_modes[i].format()==Format(8)) found = true; 
            }

            // check result
            if (!found)
            {
                // log mode list addition
                LOG("adding 320x240x8 to mode list");

                // add 320x240x8 to mode list
                m_modes[m_modes_count] = Mode(320,240,8);

                // increment mode count
                m_modes_count++;
            }
        }
        else if (version.dwPlatformId==VER_PLATFORM_WIN32_NT)
        {
            // log windows nt detection
            LOG("detected windows nt");
        }
    }

    // terminate the mode list
    m_modes[m_modes_count] = Mode();

    // log the number of display modes found
    LOG("number of display modes",m_modes_count);

    // zero resolutions count
    m_resolutions_count = 0;

    // setup the resolutions list
    for (int i=0; i<m_modes_count; i++)
    {
        // match flag
        bool match = false;

        // check against existing resolutions
        for (int j=0; j<m_resolutions_count; j++)
        {
            // compare resolution
            if (m_modes[i].width()==m_resolutions[j].width() &&
                m_modes[i].height()==m_resolutions[j].height())
            {
                // found match
                match = true;

                // stop
                break;
            }
        }

        // check match
        if (!match)
        {
            // new resolution, add it to the list
            m_resolutions[m_resolutions_count] = m_modes[i];

            // increase resolutions count
            m_resolutions_count++;
        }
    }
   
    // terminate resolutions list
    m_resolutions[m_resolutions_count] = Mode();

    // this is a truly disgusting kludge sort... blerghhhhhh!

    // sort the resolutions list
    for (i=0; i<m_resolutions_count; i++)
    {
        // compare against all other resolutions
        for (int j=0; j<m_resolutions_count; j++)
        {
            // check orientation
            if (i<j)
            {
                // check if resolution is greater
                if (m_resolutions[i].width()>m_resolutions[j].width() ||
                    m_resolutions[i].height()>m_resolutions[j].height())
                {
                    // swap items
                    Mode temp(m_resolutions[i]);
                    m_resolutions[i] = m_resolutions[j];
                    m_resolutions[j] = temp;
                }
            }
            else if (i>j)
            {
                // check if resolution is smaller
                if (m_resolutions[i].width()<m_resolutions[j].width() ||
                    m_resolutions[i].height()<m_resolutions[j].height())
                {
                    // swap items
                    Mode temp(m_resolutions[i]);
                    m_resolutions[i] = m_resolutions[j];
                    m_resolutions[j] = temp;
                }
            }
        }
    }

    // log the number of unique resolutions found
    LOG("number of unique resolutions",m_resolutions_count);

    // iterate through all resolutions
    for (i=0; i<m_resolutions_count; i++)
    {
        // setup resolution string
        char resolution[1024];
        wsprintf(resolution,"%d x %d",m_resolutions[i].width(),m_resolutions[i].height());

        // log resolution
        LOG(resolution);
    }
}




const Mode* DirectXDisplay::modes()
{
    // get modes
    return m_modes;
}




bool DirectXDisplay::test(const Mode &mode,bool exact)
{
    // check mode list
    if (m_modes_count==0)
    {
        // mode test will always succeed with empty mode list
        LOG("mode test success with empty mode list");

        // success
        return true;
    }

    // check exact
    if (exact)
    {
        // search through mode list
        for (int i=0; i<m_modes_count; i++)
        {
            // look for match
            if (mode==m_modes[i])
            {
                // log exact mode test success
                LOG("test exact mode success");

                // success
                return true;
            }
        }

        // log exact mode test failure
        LOG("test mode exact failure");

        // failure
        return false;
    }
    else
    {
        // search through resolutions list
        for (int i=0; i<m_resolutions_count; i++)
        {
            // check for fuzzy match
            if (mode.width()<=m_resolutions[i].width() &&
                mode.height()<=m_resolutions[i].height())
            {
                // log nearest mode test success
                LOG("test nearest mode success");

                // success
                return true;
            }
        }

        // log nearest mode failure
        LOG("test nearest mode failure");

        // failure
        return false;
    }
}


void DirectXDisplay::cooperative(HWND window,bool fullscreen)
{
    // set cooperative level
    if (fullscreen)
    {
        // log cooperative level
        LOG("entering exclusive mode");

        // enter exclusive mode
        DirectXCheck(m_ddraw->SetCooperativeLevel(window,DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX));
    }
    else
    {
        // log cooperative level
        LOG("entering normal cooperative mode");

        // set normal cooperative level
        DirectXCheck(m_ddraw->SetCooperativeLevel(window,DDSCL_NORMAL));
    }

    // set window
    m_window = window;

    // set fullscreen flag
    m_fullscreen = fullscreen;
}


void DirectXDisplay::open()
{
    // update flags
    m_open = true;

    // clear mode
    m_mode = Mode();

    // log windowed display open
    LOG("opening windowed display");
}


void DirectXDisplay::open(const Mode &mode,bool exact,int frequency)
{
    // check exact
    if (exact)
    {
        // log fullscreen exact display mode open
        LOG("opening exact fullscreen display mode");

        // internal open exact mode
        internal_open(mode,true,frequency);
    }
    else
    {
        // log fullscreen nearest display mode open
        LOG("opening nearest fullscreen display mode");

        // internal open nearest mode
        internal_open_nearest(mode,false,frequency);
    }

    // log fullscreen display mode open success
    LOG("successfully opened fullscreen display mode");
}


void DirectXDisplay::close()
{
    // check if open
    if (m_open && m_ddraw)
    {
        // log display close
        LOG("closing display");

        // check if fullscreen
        if (m_fullscreen)
        {
            // log display mode restore
            LOG("restoring display mode");

            // restore display mode
            m_ddraw->RestoreDisplayMode();
        
            // log leaving exclusive mode
            LOG("leaving exclusive mode");

            // leave exclusive mode
            m_ddraw->SetCooperativeLevel(m_window,DDSCL_NORMAL);
        }
    }

    // clear flags
    m_open = false;
    m_fullscreen = false;
}




void DirectXDisplay::save()
{
    // log desktop save
    LOG("saving desktop");

    // save foreground window handle
    m_foreground = GetForegroundWindow();

    // save foreground window rect
    GetWindowRect(m_foreground,&m_foreground_rect);

    // save foreground window placement
    m_foreground_placement.length = sizeof WINDOWPLACEMENT;
    GetWindowPlacement(m_foreground,&m_foreground_placement);
}


void DirectXDisplay::restore()
{
    // check foreground window is valid and not the console window
    if (m_foreground && IsWindow(m_foreground) && m_foreground!=m_window)
    {
        // log desktop restore
        LOG("restoring desktop");

        // check console window and fullscreen
        if (IsWindow(m_window) && m_fullscreen)
        {
            // minimize console window
            SetWindowPos(m_window,HWND_BOTTOM,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
        }

        // restore foreground window
        SetForegroundWindow(m_foreground);

        // restore foreground window placement
        SetWindowPlacement(m_foreground,&m_foreground_placement);

        // restore old foreground window position
        SetWindowPos(m_foreground,HWND_TOP,m_foreground_rect.left,m_foreground_rect.top,m_foreground_rect.right-m_foreground_rect.left,m_foreground_rect.bottom-m_foreground_rect.top,SWP_FRAMECHANGED|SWP_NOCOPYBITS);
    
        // clear foreground
        m_foreground = 0;
    }
}




Mode DirectXDisplay::mode() const
{
    // get mode
    return m_mode;
}


bool DirectXDisplay::fullscreen() const
{
    // get fullscreen
    return m_fullscreen;
}


const char* DirectXDisplay::information() const
{
    // get information
    return m_information;
}




void DirectXDisplay::internal_open(const Mode &mode,bool exact,int frequency)
{
    // log internal display open
    LOG("internal display open");
    LOG("mode width",mode.width());
    LOG("mode height",mode.height());
    LOG("mode format",mode.format().bits());
    LOG("mode frequency",frequency);

    // check exact
    if (exact)
    {
        // log exact mode set
        LOG("setting exact mode");

        // set exact display mode
        DirectXCheck(m_ddraw->SetDisplayMode(mode.width(),mode.height(),mode.format().bits(),frequency,0));
    }
    else
    {
        // set fuzzy display mode
        switch (mode.format().bits())
        {
            case 32: 
            {
                // log virtual 32 mode set
                LOG("setting virtual 32 mode");

                // set virtual 32 bit mode
                if (m_ddraw->SetDisplayMode(mode.width(),mode.height(),32,frequency,0)==DD_OK) break;
                if (m_ddraw->SetDisplayMode(mode.width(),mode.height(),24,frequency,0)==DD_OK) break;
                DirectXCheck(m_ddraw->SetDisplayMode(mode.width(),mode.height(),16,frequency,0));
            }
            break;

            case 24:
            {
                // log virtual 24 mode set
                LOG("setting virtual 24 mode");

                // set virtual 24 bit mode
                if (m_ddraw->SetDisplayMode(mode.width(),mode.height(),24,frequency,0)==DD_OK) break;
                if (m_ddraw->SetDisplayMode(mode.width(),mode.height(),32,frequency,0)==DD_OK) break;
                DirectXCheck(m_ddraw->SetDisplayMode(mode.width(),mode.height(),16,frequency,0));
            }
            break;

            case 16: 
            {
                // log virtual 16 mode set
                LOG("setting virtual 16 mode");

                // set virtual 16 bit mode
                if (m_ddraw->SetDisplayMode(mode.width(),mode.height(),16,frequency,0)==DD_OK) break;
                if (m_ddraw->SetDisplayMode(mode.width(),mode.height(),24,frequency,0)==DD_OK) break;
                DirectXCheck(m_ddraw->SetDisplayMode(mode.width(),mode.height(),32,frequency,0));
            }
            break;

            case 8: 
            {
                // log virtual 8 mode set
                LOG("setting virtual 8 mode");

                // set virtual 8 bit mode
                if (m_ddraw->SetDisplayMode(mode.width(),mode.height(),8,frequency,0)==DD_OK)  break;
                if (m_ddraw->SetDisplayMode(mode.width(),mode.height(),32,frequency,0)==DD_OK) break;
                if (m_ddraw->SetDisplayMode(mode.width(),mode.height(),24,frequency,0)==DD_OK) break;
                DirectXCheck(m_ddraw->SetDisplayMode(mode.width(),mode.height(),16,frequency,0));
            }
            break;

            default:
            {
                // error message
                throw Error("unsupported pixel format");
            }
        }
    }

    // log internal display open success
    LOG("internal display open success");

    // update flags
    m_open = true;

    // setup mode
    m_mode = mode;
}


void DirectXDisplay::internal_open_nearest(const Mode &mode,bool exact,int frequency)
{
    // check resolutions count
    if (m_resolutions_count!=0)
    {
        // log nearest mode resolutions search
        LOG("searching for nearest mode in resolutions list");

        // resolutions index
        int index = 0;

        // mode match
        Mode match;
        Mode match_exact;

        // iterate through resolutions list
        for (int i=0; i<m_resolutions_count; i++)
        {
            // get resolution data
            const int width = m_resolutions[i].width();
            const int height = m_resolutions[i].height();

            // check for match
            if (mode.width()<=width && mode.height()<=height)
            {
                // check for exact match
                if (width==mode.width() && height==mode.height())
                {
                    // log exact match find
                    LOG("found an exact match");

                    // exact match
                    match_exact = Mode(width,height,mode.format());
                }

                // check if match is valid
                if (match.valid())
                {
                    // calculate difference between match and mode
                    const int dx1 = match.width() - mode.width();
                    const int dy1 = match.height() - mode.height();

                    // calculate difference between current resolution and mode
                    const int dx2 = width - mode.width();
                    const int dy2 = height - mode.height();

                    // compare differences
                    if (dx2<=dx1 && dy2<=dy1)
                    {
                        // log better nearest match
                        LOG("found a better nearest match");

                        // update match
                        match = Mode(width,height,mode.format());
                    }
                }
                else
                {
                    // log first nearest match find
                    LOG("found first nearest match");

                    // first match
                    match = Mode(width,height,mode.format());
                }
            }
            else
            {
                // log nearest mode search stop
                LOG("stopping nearest mode search");

                // stop
                break;
            }
        }

        // check exact match
        if (match_exact.valid())
        {
            try
            {
                // log exact match
                LOG("trying exact match");

                // try exact match
                internal_open(match_exact,exact,frequency);

                // success
                return;
            }
            catch (Error&)
            {
                // failure
            }
        }

        // check match
        if (match.valid())
        {
            try
            {
                // log nearest match
                LOG("trying nearest match");

                // try nearest match
                internal_open(match,exact,frequency);

                // success
                return;
            }
            catch (Error &)
            {
                // failure
            }
        }
    }
    else
    {
        // no resolutions list for nearest mode search
        LOG("no resolutions list for nearest mode search");
    }

    // first pass nearest mode matching failed...
    LOG("could not find a nearest mode in first pass");

    try
    {
        // log requested mode open
        LOG("trying requested mode first");

        // try requested mode first
        internal_open(mode,exact,frequency);

        // success
        return;
    }
    catch (Error&)
    {
        // fallback to the nearest "standard" mode
        LOG("falling back to nearest standard mode");

        // check if 320x200 is a good match
        if (mode.width()<=320 && mode.height()<=200)
        {
            try
            {
                // open 320x200
                internal_open(Mode(320,200,mode.format()),exact,frequency);

                // success
                return;
            }
            catch (Error&)
            {
                // failure
            }
        }

        // check if 320x240 is a good match
        if (mode.width()<=320 && mode.height()<=240)
        {
            try
            {
                // open 320x240
                internal_open(Mode(320,240,mode.format()),exact,frequency);

                // success
                return;
            }
            catch (Error&)
            {
                // failure
            }
        }

        // check if 512x384 is a good match
        if (mode.width()<=512 && mode.height()<=384)
        {
            try
            {
                // open 512x384
                internal_open(Mode(512,384,mode.format()),exact,frequency);

                // success
                return;
            }
            catch (Error&)
            {
                // failure
            }
        }

        // check if 640x400 is a good match
        if (mode.width()<=640 && mode.height()<=400)
        {
            try
            {
                // open 640x400
                internal_open(Mode(640,400,mode.format()),exact,frequency);

                // success
                return;
            }
            catch (Error&)
            {
                // failure
            }
        }

        // check if 640x400 is a good match
        if (mode.width()<=640 && mode.height()<=480)
        {
            try
            {
                // open 640x480
                internal_open(Mode(640,480,mode.format()),exact,frequency);

                // success
                return;
            }
            catch (Error&)
            {
                // failure
            }
        }

        // check if 800x600 is a good match
        if (mode.width()<=800 && mode.height()<=600)
        {
            try
            {
                // open 800x600
                internal_open(Mode(800,600,mode.format()),exact,frequency);

                // success
                return;
            }
            catch (Error&)
            {
                // failure
            }
        }

        // check if 1024x768 is a good match
        if (mode.width()<=1024 && mode.height()<=768)
        {
            try
            {
                // open 1024x768
                internal_open(Mode(1024,768,mode.format()),exact,frequency);

                // success
                return;
            }
            catch (Error&)
            {
                // failure
            }
        }

        // check if 1280x1024 is a good match
        if (mode.width()<=1280 && mode.height()<=1024)
        {
            try
            {
                // open 1280x1024
                internal_open(Mode(1280,1024,mode.format()),exact,frequency);

                // success
                return;
            }
            catch (Error&)
            {
                // failure
            }
        }

        // check if 1600x1200 is a good match
        if (mode.width()<=1600 && mode.height()<=1200)
        {
            try
            {
                // open 1600x1200
                internal_open(Mode(1600,1200,mode.format()),exact,frequency);

                // success
                return;
            }
            catch (Error&)
            {
                // failure
            }
        }
    }

    // error message
    throw Error("could not set display mode");
}




HRESULT WINAPI DirectXDisplay::callback(LPDDSURFACEDESC descriptor,LPVOID context)
{
    // check pointers are valid (necessary?)
    if (!descriptor || !context) return DDENUMRET_CANCEL; 
    
    // cast context pointer to DirectXDisplay
    DirectXDisplay *display = (DirectXDisplay*)context;

    // check that the descriptor contains the data we want
    if (descriptor->dwFlags&DDSD_WIDTH && descriptor->dwFlags&DDSD_HEIGHT && descriptor->dwFlags&DDSD_PIXELFORMAT)
    {
        // setup display mode
        display->m_modes[display->m_modes_count] = Mode(descriptor->dwWidth,descriptor->dwHeight,DirectXTranslate(descriptor->ddpfPixelFormat));

        // increase modes count
        display->m_modes_count ++;
    }
    else
    {
        // we dont have the information required... hmppft!
        LOG("EnumDisplayModesCallback was not given enough mode information");
    }

    // continue enumeration
    return DDENUMRET_OK;
}
