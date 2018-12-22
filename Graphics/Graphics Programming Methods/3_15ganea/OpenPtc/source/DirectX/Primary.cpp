//
// DirectX Primary class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include "Core/Log.h"
#include "Core/Error.h"
#include "Core/Color.h"
#include "Core/Clipper.h"
#include "DirectX/Check.h"
#include "DirectX/Primary.h"
#include "DirectX/Translate.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif




DirectXPrimary::DirectXPrimary()
{
    // defaults
    m_locked = 0;
    m_window = 0;
    m_width = 0;
    m_height = 0;
    m_back = 0;
    m_front = 0;
    m_pages = 0;
    m_lpDD2 = 0;
    m_lpDDC = 0;
    m_lpDDS_primary = 0;
    m_lpDDS_primary_back = 0;
    m_lpDDS_secondary = 0;
    m_active = true;
    m_blocking = true;
    m_centering = true;
    m_synchronize = true;
    m_fullscreen = false;
    m_primary_width = 0;
    m_primary_height = 0;
    m_secondary_width = 0;
    m_secondary_height = 0;
    memset(&m_lpDDS_primary_page,0,sizeof m_lpDDS_primary_page);
}


DirectXPrimary::~DirectXPrimary()
{
    // close
    close();
}



void DirectXPrimary::initialize(Win32Window &window,LPDIRECTDRAW2 lpDD2)
{
    // log primary surface initialize
    LOG("initializing primary surface");

    // close
    close();

    // setup window
    m_window = &window;

    // setup ddraw
    m_lpDD2 = lpDD2;
}


void DirectXPrimary::primary(int pages,bool video,bool fullscreen,bool palette,bool complex)
{   
    try
    {
        // log primary surface creation data
        LOG("creating primary surface");
        LOG("pages",pages);
        LOG("video",video);
        LOG("fullscreen",fullscreen);
        LOG("palette",palette);
        LOG("complex",complex);

        // check that the number of pages requested is valid
        if (pages<=0) throw Error("invalid number of pages");

        // setup fullscreen flag
        m_fullscreen = fullscreen;

        // setup clear attach pages flag
        bool attach_primary_pages = false;

        // check if we should create a complex surface
        if (complex || !palette || pages==1)
        {
            // log primary surface creation
            LOG("creating complex primary flipping surface");

            // create complex primary flipping surface
            DDSURFACEDESC descriptor;
            memset(&descriptor,0,sizeof descriptor);
            descriptor.dwSize  = sizeof(descriptor);
            descriptor.dwFlags = DDSD_CAPS;
            if (pages>1) descriptor.dwFlags |= DDSD_BACKBUFFERCOUNT;
            descriptor.dwBackBufferCount = pages - 1;
            descriptor.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
            if (video) descriptor.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
            if (pages>1) descriptor.ddsCaps.dwCaps |= DDSCAPS_COMPLEX | DDSCAPS_FLIP;
            DirectXCheck(m_lpDD2->CreateSurface(&descriptor,&m_lpDDS_primary,0),"m_lpDD2->CreateSurface failed in DirectXPrimary::primary");
        }
        else
        {
            // log simple primary surface creation
            LOG("creating simple primary surface");

            // create primary flipping surface
            DDSURFACEDESC descriptor;
            memset(&descriptor,0,sizeof descriptor);
            descriptor.dwSize  = sizeof(descriptor);
            descriptor.dwFlags = DDSD_CAPS;
            descriptor.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
            if (video) descriptor.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
            DirectXCheck(m_lpDD2->CreateSurface(&descriptor,&m_lpDDS_primary,0),"m_lpDD2->CreateSurface failed in DirectXPrimary::primary (palette)");

            // attach primary pages
            attach_primary_pages = true;
        }

        // get surface descriptor
        DDSURFACEDESC descriptor;
        memset(&descriptor,0,sizeof descriptor);
        descriptor.dwSize = sizeof(descriptor);
        DirectXCheck(m_lpDDS_primary->GetSurfaceDesc(&descriptor),"m_lpDDS_primary->GetSurfaceDesc failed in DirectXPrimary::primary");
        
        // log primary surface memory location
        if (descriptor.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) LOG("primary surface is in video memory");
        else LOG("primary surface is in system memory");

        // get pixel format
        DDPIXELFORMAT ddpf;
        memset(&ddpf,0,sizeof ddpf);
        ddpf.dwSize = sizeof(ddpf);
        DirectXCheck(m_lpDDS_primary->GetPixelFormat(&ddpf),"m_lpDDS_primary->GetPixelFormat failed in DirectXPrimary::primary");

        // setup data
        m_front  = m_lpDDS_primary;
        m_pages  = pages;
        m_width  = descriptor.dwWidth;
        m_height = descriptor.dwHeight;
        m_format = DirectXTranslate(ddpf);

        // log primary surface data
        LOG("primary width",m_width);
        LOG("primary height",m_height);
        LOG("primary pages",m_pages);
        LOG("primary format",m_format);
        
        // check palette
        if (palette)
        {
            // log primary palette clear
            LOG("clearing primary palette");

            // set black palette
            this->palette(Palette());
        }

        // check primary page attach
        if (attach_primary_pages)
        {
            // check that the number of pages will fit into our primary page surface array
            if (pages-1>sizeof m_lpDDS_primary_page) throw Error("too many primary pages");

            // iterate requested pages
            for (int i=0; i<pages-1; i++)
            {
                // log primary page surface creation
                LOG("creating primary page surface");

                // create a single page surface
                DDSURFACEDESC descriptor;
                memset(&descriptor,0,sizeof descriptor);
                descriptor.dwSize  = sizeof(descriptor);
                descriptor.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
                descriptor.dwWidth = m_width;
                descriptor.dwHeight = m_height;
                descriptor.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
                if (video) descriptor.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
                DirectXCheck(m_lpDD2->CreateSurface(&descriptor,&m_lpDDS_primary_page[i],0),"m_lpDD2->CreateSurface failed in DirectXPrimary::primary (primary page)");

                // get surface descriptor
                memset(&descriptor,0,sizeof descriptor);
                descriptor.dwSize = sizeof(descriptor);
                DirectXCheck(m_lpDDS_primary_page[i]->GetSurfaceDesc(&descriptor),"m_lpDDS_primary_page->GetSurfaceDesc failed in DirectXPrimary::primary");
        
                // log primary surface page memory location
                if (descriptor.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) LOG("primary surface page is in video memory");
                else LOG("primary surface page is in system memory");

                // log primary page surface creation
                LOG("attaching page to primary surface");

                // attach page surface to primary
                DirectXCheck(m_lpDDS_primary->AddAttachedSurface(m_lpDDS_primary_page[i]),"m_lpDDS_primary->AddAttachedSurface failed in DirectXPrimary::primary");
            }
        }

        // setup primary data
        m_primary_width = m_width;
        m_primary_height = m_height;

        // check if fullscreen
        if (!fullscreen)
        {
            // get window client area
            RECT rectangle;
            GetClientRect(m_window->handle(),&rectangle);

            // set data to match window
            m_width  = rectangle.right;
            m_height = rectangle.bottom;
        }
    
        // setup primary surface area
        m_area = Area(0,0,m_width,m_height);

        // setup clip area
        m_clip = m_area;

        // check pages
        if (pages>1)
        {
            // get back surface
            DDSCAPS capabilities;
            capabilities.dwCaps = DDSCAPS_BACKBUFFER;
            DirectXCheck(m_front->GetAttachedSurface(&capabilities,&m_lpDDS_primary_back),"m_lpDDS_primary->GetAttachedSurface failed in DirectXPrimary::primary");

            // get surface descriptor
            DDSURFACEDESC descriptor;
            memset(&descriptor,0,sizeof descriptor);
            descriptor.dwSize = sizeof(descriptor);
            DirectXCheck(m_lpDDS_primary_back->GetSurfaceDesc(&descriptor),"m_lpDDS_primary_back->GetSurfaceDesc failed in DirectXPrimary::primary");
    
            // log primary back surface memory location
            if (descriptor.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) LOG("primary back surface is in video memory");
            else LOG("primary back surface is in system memory");
        
        }
        else
        {
            // only one surface
            m_lpDDS_primary_back = m_front;
        }

        // setup back surface
        m_back = m_lpDDS_primary_back;

        // check fullscreen
        if (fullscreen)
        {
            // clear all pages
            while (pages--)
            {
                // log primary page clearing
                LOG("clearing primary page");

                // clear
                clear();
                update();
            }
        }
    }
    catch (Error &error)
    {
        // check m_lpDDS_primary
        if (m_lpDDS_primary)
        {
            // release interface
            m_lpDDS_primary->Release();

            // null interface
            m_lpDDS_primary = 0;
        }

        // error composite message
        throw Error("could not create primary surface",error);
    }
}


void DirectXPrimary::secondary(int width,int height)
{
    // log secondary surface creation data
    LOG("creating secondary surface");
    LOG("width",width);
    LOG("height",height);

    // create the secondary surface
    DDSURFACEDESC descriptor;
    descriptor.dwSize  = sizeof(descriptor);
    descriptor.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    descriptor.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    descriptor.dwHeight = height;
    descriptor.dwWidth = width;
    DirectXCheck(m_lpDD2->CreateSurface(&descriptor,&m_lpDDS_secondary,0),"m_lpDD2->CreateSurface failed in DirectXPrimary::secondary");

    // get surface descriptor
    memset(&descriptor,0,sizeof descriptor);
    descriptor.dwSize = sizeof(descriptor);
    DirectXCheck(m_lpDDS_secondary->GetSurfaceDesc(&descriptor),"m_lpDDS_secondary->GetSurfaceDesc failed in DirectXPrimary::secondary");

    // log primary surface page memory location
    if (descriptor.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) LOG("secondary surface is in video memory");
    else LOG("secondary surface is in system memory");
    
    // check fullscreen
    if (!m_fullscreen)
    {
        // log clipper attach to primary surface
        LOG("attaching clipper to primary surface");
        
        // create a clipper object
        DirectXCheck(m_lpDD2->CreateClipper(0,&m_lpDDC,0),"m_lpDD2->CreateClipper failed in DirectXPrimary::secondary");

        // set clipper to hWnd
        DirectXCheck(m_lpDDC->SetHWnd(0,m_window->handle()),"m_lpDDC->SetHWnd failed in DirectXPrimary::secondary");

        // attach clipper object to primary surface
        DirectXCheck(m_lpDDS_primary->SetClipper(m_lpDDC),"m_lpDDS_primary->SetClipper failed in DirectXPrimary::secondary");
    }

    // setup data
    m_width = width;
    m_height = height;

    // setup primary surface area
    m_area = Area(0,0,m_width,m_height);

    // setup clip area
    m_clip = m_area;

    // setup secondary data
    m_secondary_width = m_width;
    m_secondary_height = m_height;

    // set back surface to secondary
    m_back = m_lpDDS_secondary;

    // caps data
    DDCAPS hel;
    DDCAPS driver;

    // setup sizes
    hel.dwSize = sizeof hel;
    driver.dwSize = sizeof driver;

    // get ddraw capabilities
    DirectXCheck(m_lpDD2->GetCaps(&driver,&hel));

    //
    // auto stretching support is disabled below because in almost 100% of cases
    // centering is faster and we must choose the fastest option by default!
    //

    // check if we have hardware stretching of arbitrary values
    if (driver.dwCaps&DDCAPS_BLTSTRETCH && driver.dwFXCaps&DDFXCAPS_BLTSTRETCHX && driver.dwFXCaps&DDFXCAPS_BLTSTRETCHY)
    {
        // we found hardware stretching support
        LOG("found hardware stretching support");
    }
    else
    {
        // no hardware stretching support
        LOG("no hardware stretching support");
    }

    // surface capabilities
    DDSCAPS capabilities;

    // get secondary surface capabilities
    m_lpDDS_secondary->GetCaps(&capabilities);

    // check if the secondary is in system memory
    if (capabilities.dwCaps & DDSCAPS_SYSTEMMEMORY)
    {
        // the secondary surface is in system memory
        LOG("secondary surface is is system memory");
    }

    // use centering
    centering(true);

    // log secondary clear page
    LOG("clearing secondary page");

    // clear
    clear();

    // update
    update();
}


void DirectXPrimary::synchronize(bool update)
{
    // set synchronize flag
    m_synchronize = update;

    // synchronization is only required for single page primary
    if (m_pages>1) m_synchronize = false;

    // log synchronize flag data
    LOG("primary synchronize",m_synchronize);
}


void DirectXPrimary::centering(bool center)
{
    // set centering flag
    m_centering = center;

    // log centering flag data
    LOG("primary centering",m_centering);
}


void DirectXPrimary::blocking(bool blocking)
{
    // set blocking
    m_blocking = blocking;

    // log blocking flag data
    LOG("primary blocking",m_blocking);
}


void DirectXPrimary::close()
{
    try
    {
        // log primary surface close
        LOG("closing primary surface");

        // clear lost flag
        bool lost = false;

        // check if primary surface has been lost
        if (m_lpDDS_primary && m_lpDDS_primary->IsLost()!=DD_OK) lost = true;

        // check if secondary surface has benn lost
        if (m_lpDDS_secondary && m_lpDDS_secondary->IsLost()!=DD_OK) lost = true;

        // check surfaces are valid and we are fullscreen and not lost
        if (m_back && m_lpDDS_primary && m_fullscreen && !lost)
        {
            // log palette clearing
            LOG("clearing primary palette");

            // set black palette
            palette(Palette());

            // log primary page clearing
            LOG("clearing primary pages");
        
            // iterate all primary pages
            for (int i=0; i<m_pages; i++)
            {
                // clear
                clear();

                // update
                update();
            }
        }
    }
    catch (Error&)
    {
        // log primary close clear failure
        LOG("primary close clearing failed");
    }

    // check m_lpDDC
    if (m_lpDDC)
    {
        // log clipper release
        LOG("releasing clipper");

        // release interface
        m_lpDDC->Release();

        // null interface
        m_lpDDC = 0;
    }

    // check m_lpDDS_secondary
    if (m_lpDDS_secondary)
    {
        // log secondary surface release
        LOG("releasing secondary surface");

        // release interface
        m_lpDDS_secondary->Release();

        // null interface
        m_lpDDS_secondary = 0;
    }

    // release all attached primary pages
    for (int i=0; m_lpDDS_primary_page[i]!=0; i++)
    {
        // log primary surface attached page release
        LOG("releasing attached primary surface page");

        // release interface
        m_lpDDS_primary_page[i]->Release();

        // null interface
        m_lpDDS_primary_page[i] = 0;
    }

    // check m_lpDDS_primary
    if (m_lpDDS_primary)
    {
        // log primary surface release
        LOG("releasing primary surface");

        // release interface
        m_lpDDS_primary->Release();

        // null interface
        m_lpDDS_primary = 0;
    }

    // clear
    m_back = 0;
    m_front = 0;
    m_lpDDS_primary_back = 0;
}




void DirectXPrimary::update()
{
    // block
    block();

    // paint
    paint();

    // check pages
    if (m_pages>1)
    {
        // flip surface
        DirectXCheck(m_front->Flip(0,DDFLIP_WAIT),"m_front->Flip failed in DirectXPrimary::update");
    }
}




void* DirectXPrimary::lock()
{
    // block
    block();

    // setup surface descriptor
    DDSURFACEDESC descriptor;
    descriptor.dwSize = sizeof descriptor;

    // check fullscreen and secondary
    if (m_fullscreen || m_back==m_lpDDS_secondary)
    {
        // lock surface
        DirectXCheck(m_back->Lock(0,&descriptor,DDLOCK_WAIT,0),"m_back->Lock failed in DirectXPrimary::lock");

        // update locked pointer
        m_locked = descriptor.lpSurface;
    }
    else
    {
        // get origin of client area
        POINT point = { 0,0 };
        ClientToScreen(m_window->handle(),&point);

        // setup client area rectangle
        RECT rect = { point.x, point.y, point.x+m_width, point.y+m_height };

        // lock surface
        DirectXCheck(m_back->Lock(&rect,&descriptor,DDLOCK_WAIT,0),"m_back->Lock(rect) failed in DirectXPrimary::lock");

        // update locked pointer
        m_locked = descriptor.lpSurface;
    }

    // locked pointer
    return m_locked;
}


void DirectXPrimary::unlock()
{
    // block
    block();

    // unlock surface
    DirectXCheck(m_back->Unlock(m_locked),"m_back->Unlock failed in DirectXPrimary::unlock");
}




void DirectXPrimary::clear()
{
    // block
    block();

    // check fullscreen and secondary
    if (m_fullscreen || m_back==m_lpDDS_secondary)
    {
        // clear surface
        DDBLTFX fx;
        fx.dwSize = sizeof(fx);
        fx.dwFillColor = 0;
        DirectXCheck(m_back->Blt(0,0,0,DDBLT_COLORFILL|DDBLT_WAIT,&fx),"m_back->Blt failed in DirectXPrimary::clear");
    }
    else
    {
        // todo: replace with hardware clear!

        // check console format
        if (format().direct())
        {
            // direct color
            clear(Color(0,0,0,0),m_area);
        }
        else
        {
            // indexed color
            clear(Color(0),m_area);
        }
    }
}


void DirectXPrimary::clear(const Color &color,const Area &area)
{
    // block
    block();

    // check fullscreen and secondary
    if (m_fullscreen || m_back==m_lpDDS_secondary)
    {
        // clip clear area
        Area clipped = Clipper::clip(area,m_clip);

        // pack clear color value from color object
        DWORD clear_color = (DWORD) pack(color,m_format);
    
        // setup blt rect
        RECT rect;
        rect.left = clipped.left();
        rect.top = clipped.top();
        rect.right = clipped.right();
        rect.bottom = clipped.bottom();

        // clear surface
        DDBLTFX fx;
        fx.dwSize = sizeof(fx);
        fx.dwFillColor = clear_color;
        DirectXCheck(m_back->Blt(&rect,0,0,DDBLT_COLORFILL|DDBLT_WAIT,&fx),"m_back->Blt(rect) failed in DirectXPrimary::clear");
    }
    else
    {
        // todo: replace with accelerated clearing code!

        // lock console
        void *pixels = lock();

        try
        {
            // clip area
            Area clipped_area = Clipper::clip(area,this->clip());

            // request clear
            m_clear.request(format());

            // clear console area
            m_clear.clear(pixels,clipped_area.left(),clipped_area.top(),clipped_area.width(),clipped_area.height(),pitch(),color);

            // unlock
            unlock();
        }
        catch (Error &error)
        {
            // unlock
            unlock();

            // error message
            throw Error("failed to clear console area",error);
        }
    }
}




void DirectXPrimary::palette(const Palette &palette)
{
    // block
    block();
    
    // update palette
    m_palette = palette;

    // check format is indexed
    if (!m_format.indexed())
    {
        // log palette set in direct color
        LOG("palette set in direct color");
    }
    else
    {
        // get palette data
        const int32 *data = palette.data();

        // convert palette data
        PALETTEENTRY temp[256];
        for (int i=0; i<256; i++)
        {
            temp[i].peRed   = (BYTE) ( (data[i] & 0x00FF0000) >> 16 );
            temp[i].peGreen = (BYTE) ( (data[i] & 0x0000FF00) >> 8  );
            temp[i].peBlue  = (BYTE) ( (data[i] & 0x000000FF) >> 0  );
            temp[i].peFlags = 0;
        }

        // directdraw palette interface
        LPDIRECTDRAWPALETTE lpDDP = 0;

        // get attached surface palette
        if (m_lpDDS_primary->GetPalette(&lpDDP)!=DD_OK)
        {
            // create palette object
            DirectXCheck(m_lpDD2->CreatePalette(DDPCAPS_8BIT|DDPCAPS_ALLOW256|DDPCAPS_INITIALIZE,temp,&lpDDP,0),"m_lpDD2->CreatePalette failed in DirectXPrimary::palette");

            // attach palette to surface
            DirectXCheck(m_lpDDS_primary->SetPalette(lpDDP),"m_lpDDS_primary->SetPalette failed in DirectXPrimary::palette");
        }
        else
        {
            // set palette entries
            DirectXCheck(lpDDP->SetEntries(0,0,256,temp),"lpDDP->SetEntries failed in DirectXPrimary::palette");
        }
    }
}


const Palette& DirectXPrimary::palette() const
{
    // get palette
    return m_palette;
}




void DirectXPrimary::clip(const Area &area)
{
    // set new clip area
    m_clip = Clipper::clip(area,m_area);
}




int DirectXPrimary::width() const
{
    // get width
    return m_width;
}


int DirectXPrimary::height() const
{
    // get height
    return m_height;
}


int DirectXPrimary::pages() const
{
    // get pages
    return m_pages;
}


int DirectXPrimary::pitch() const
{
    // block
    block();

    // setup surface descriptor
    DDSURFACEDESC descriptor;
    descriptor.dwSize = sizeof(descriptor);

    // get surface descriptor
    DirectXCheck(m_back->GetSurfaceDesc(&descriptor),"m_back->GetSurfaceDesc failed in DirectXPrimary::pitch");

    // return surface pitch
    return descriptor.lPitch;
}


const Area& DirectXPrimary::area() const
{
    // get area
    return m_area;
}


const Area& DirectXPrimary::clip() const
{
    // get clip
    return m_clip;
}


const Format& DirectXPrimary::format() const
{
    // get format
    return m_format;
}




LPDIRECTDRAWSURFACE DirectXPrimary::lpDDS() const
{
    // return secondary if exists, otherwise primary
    if (m_lpDDS_secondary) return m_lpDDS_secondary;
    else return m_lpDDS_primary_back;
}                      


LPDIRECTDRAWSURFACE DirectXPrimary::lpDDS_primary() const
{
    // get primary surface interface
    return m_lpDDS_primary;
}                      


LPDIRECTDRAWSURFACE DirectXPrimary::lpDDS_secondary() const
{
    // get secondary surface interface
    return m_lpDDS_secondary;
}                      




void DirectXPrimary::activate()
{   
    // log primary activate
    LOG("primary activated");

    // activate
    m_active = true;
}


void DirectXPrimary::deactivate()
{
    // log primary deactivate
    LOG("primary deactivated");

    // check blocking
    if (m_blocking)
    {
        // deactivate
        m_active = false;
    }
    else
    {
        // no deactivation when not blocking...
    }
}


bool DirectXPrimary::active() const
{
    // is active?
    return m_active;
}


void DirectXPrimary::block() const
{
    // check blocking
    if (!m_blocking) return;

    // check if active
    if (!active())
    {
        // restored flag
        bool restored = false;

        // loop until restored
        while (!restored)
        {
            // log blocking progress
            LOG("blocking until activated");

            // block until active
            while (!active())
            {
                // force window update
                m_window->update(true);

                // sleep
                Sleep(10);
            }

            // log blocking progress
            LOG("primary is active");

            // update window (forced)
            m_window->update(true);

            try
            {
                // restore
                restore();

                // success
                restored = true;

                // successful restore
                LOG("successful restore");
            }
            catch (Error&)
            {
                // log a failed restore iteration
                LOG("application is active but cannot restore");
            }

            // sleep
            Sleep(10);
        }
    }

    // check primary surface is not lost
    if (m_lpDDS_primary->IsLost()!=DD_OK)
    {
        // this should not occur!
        throw Error("primary surface lost unexpectedly! report this bug to ptc@gaffer.org immediately!");
    }

    // check secondary surface is not lost
    if (m_lpDDS_secondary && m_lpDDS_secondary->IsLost()!=DD_OK)
    {
        // this should not occur!
        throw Error("secondary surface lost unexpectedly! report this bug to ptc@gaffer.org immediately!");
    }
}


void DirectXPrimary::save() const
{
    // check primary surface is not lost
    if (m_lpDDS_primary->IsLost()==DD_OK)
    {
        // log primary surface contents save
        LOG("saving contents of primary surface");

        // todo: save contents of primary surface
    }
    else
    {
        // the primary surface has already been lost
        LOG("could not save primary surface");
    }

    // check secondary surface exists and is not lost
    if (m_lpDDS_secondary && m_lpDDS_secondary->IsLost()==DD_OK)
    {
        // log secondary surface contents save
        LOG("saving contents of secondary surface");

        // todo: save contents of secondary surface
    }
    else if (m_lpDDS_secondary)
    {
        // the secondary surface has already been lost
        LOG("could not save secondary surface");
    }
}


void DirectXPrimary::restore() const
{
    // restore primary surface
    DirectXCheck(m_lpDDS_primary->Restore(),"m_lpDDS_primary->Restore failed in DirectXPrimary::restore");

    // restore secondary surface
    if (m_lpDDS_secondary) DirectXCheck(m_lpDDS_secondary->Restore(),"m_lpDDS_secondary->Restore failed in DirectXPrimary::restore");

    // log primary surface restore
    LOG("restoring contents of primary surface");

    // todo: restore palette object on primary surface ?

    // todo: restore contents of primary surface

    // check for attached primary pages
    if (m_lpDDS_primary_page[0]!=0)
    {
        // log attached pages restore
        LOG ("restoring attached pages");

        // iterate all attached surfaces
        for (int i=0; i<m_pages-1; i++)
        {
            // restore manually attached primary page surface
            DirectXCheck(m_lpDDS_primary_page[i]->Restore(),"m_lpDDS_primary_page->Restore failed in DirectXPrimary::restore");
        }
    }

    // check secondary surface
    if (m_lpDDS_secondary)
    {
        // check fullscreen
        if (m_fullscreen)
        {
            // log temporary primary surface clear
            LOG("temporary primary surface clear");

            // temporary: clear primary surface
            RECT rect = {0,0,m_primary_width,m_primary_height};
            DDBLTFX fx;
            fx.dwSize = sizeof(fx);
            fx.dwFillColor = 0;
            DirectXCheck(m_lpDDS_primary->Blt(&rect,0,0,DDBLT_COLORFILL|DDBLT_WAIT,&fx),"m_lpDDS_primary->Blt failed in DirectXPrimary::restore");
        }

        // log secondary surface restore
        LOG("restoring contents of secondary surface");
    
        // todo: restore contents of secondary surface
    }
}




void DirectXPrimary::paint()
{
    // no painting if not active
    if (!active())
    {
        // log painting when not active
        LOG("paint when not active");

        // done
        return;
    }

    // check secondary
    if (m_lpDDS_secondary)
    {
        // no painting if any surfaces are lost
        if (m_lpDDS_primary->IsLost()!=DD_OK || m_lpDDS_secondary->IsLost()!=DD_OK)
        {
            // log painting when surfaces are lost
            LOG("paint when surfaces are lost");

            // done
            return;
        }

        // setup source rectangle
        RECT source = { 0,0,m_secondary_width,m_secondary_height };

        // setup destination rectangle
        RECT destination = { 0,0,m_primary_width,m_primary_height };

        // note: code below assumes secondary is smaller than primary

        // check if centering
        if (m_centering && m_fullscreen)
        {
            // calculate centering position
            const int x = ( destination.right - source.right ) / 2;
            const int y = ( destination.right - source.right ) / 2;

            // center source on destination
            destination.left = x;
            destination.top = y;
            destination.right = x + source.right;
            destination.bottom = y + source.bottom;
        }

        // check fullscreen
        if (!m_fullscreen)
        {
            // get origin of client area
            POINT point = { 0,0 };
            ClientToScreen(m_window->handle(),&point);

            // get window client area
            GetClientRect(m_window->handle(),&destination);

            // offset destination rectangle
            destination.left += point.x;
            destination.top += point.y;
            destination.right += point.x;
            destination.bottom += point.y;
        }

        // check for zero area source or destination rects
        if (source.right-source.left==0 || source.bottom-source.top==0 || destination.right-destination.left==0 || destination.bottom-destination.top==0)
        {
            // log zero area early exit
            LOG("zero area in primary paint");

            // zero area
            return;
        }

        // synchronized paint
        if (m_synchronize)
        {
            // setup fx
            DDBLTFX fx;
            fx.dwSize = sizeof fx;
            fx.dwDDFX = DDBLTFX_NOTEARING;

            try
            {
                // blt from secondary to primary
                DirectXCheck(m_lpDDS_primary_back->Blt(&destination,m_lpDDS_secondary,&source,DDBLT_WAIT|DDBLT_DDFX,&fx),"m_lpDDS_primary->Blt (synchronized) failed in DirectXPrimary::paint");
            }
            catch (Error&)
            {
                // synchronized blt is not supported
                LOG("falling back to unsynchronized blt");

                // synchronize failure
                m_synchronize = false;
            }
        }

        // unsynchonized paint
        if (!m_synchronize)
        {
            // blt from secondary to primary
            DirectXCheck(m_lpDDS_primary_back->Blt(&destination,m_lpDDS_secondary,&source,DDBLT_WAIT,0),"m_lpDDS_primary->Blt (unsynchronized) failed in DirectXPrimary::paint");
        }
    }
}


int32 DirectXPrimary::pack(const Color &color,const Format &format)
{
    // check color and format types
    if (color.direct() && format.direct())
    {
        // color bases
        int r_base = 0;
        int g_base = 0;
        int b_base = 0;
        int a_base = 0;

        // color sizes
        int r_size = 0;
        int g_size = 0;
        int b_size = 0;
        int a_size = 0;

        // analyse pixel format masks
        analyse(format.r(),r_base,r_size);
        analyse(format.g(),g_base,g_size);
        analyse(format.b(),b_base,b_size);
        analyse(format.a(),a_base,a_size);

        // calculate color scale values
        float r_scale = (float) (1<<r_size);
        float g_scale = (float) (1<<g_size);
        float b_scale = (float) (1<<b_size);
        float a_scale = (float) (1<<a_size);

        // pack color integer
        int32 data = ( int32(color.r() * r_scale) << r_base ) | 
                     ( int32(color.g() * g_scale) << g_base ) | 
                     ( int32(color.b() * b_scale) << b_base ) | 
                     ( int32(color.a() * a_scale) << a_base );

        // finished
        return data;
    }
    else if (color.indexed() && format.indexed())
    {
        // indexed color
        return color.index();
    }
    else
    {
        // error message
        throw Error("color format type mismatch");
    }
}


void DirectXPrimary::analyse(int32 mask,int &base,int &size)
{
    // clear
    base = 0;
    size = 0;

    // check zero mask
    if (!mask) return;

    // find mask base
    while (!(mask&1))
    {
        mask>>=1;
        base++;
    }

    // find mask size
    while (mask&1)
    {
        mask>>=1;
        size++;
    }
}
