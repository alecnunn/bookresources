//
// DirectX Check function for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//                    

// include files
#include "Core/Error.h"
#include "DirectX/Check.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




void DirectXCheck(HRESULT result)
{
    // check for success
    if (result==DD_OK) return;

#ifdef __PTC_DIRECTX_ERROR_STRINGS__

    // error message                                    
    switch (result)
    {
        case DDERR_ALREADYINITIALIZED:                  throw Error("DDERR_ALREADYINITIALIZED");
        case DDERR_CANNOTATTACHSURFACE:                 throw Error("DDERR_CANNOTATTACHSURFACE");
        case DDERR_CANNOTDETACHSURFACE:                 throw Error("DDERR_CANNOTDETACHSURFACE");
        case DDERR_CURRENTLYNOTAVAIL:                   throw Error("DDERR_CURRENTLYNOTAVAIL");
        case DDERR_EXCEPTION:                           throw Error("DDERR_EXCEPTION");
        case DDERR_GENERIC:                             throw Error("DDERR_GENERIC");
        case DDERR_HEIGHTALIGN:                         throw Error("DDERR_HEIGHTALIGN");
        case DDERR_INCOMPATIBLEPRIMARY:                 throw Error("DDERR_INCOMPATIBLEPRIMARY");
        case DDERR_INVALIDCAPS:                         throw Error("DDERR_INVALIDCAPS");
        case DDERR_INVALIDCLIPLIST:                     throw Error("DDERR_INVALIDCLIPLIST");
        case DDERR_INVALIDMODE:                         throw Error("DDERR_INVALIDMODE");
        case DDERR_INVALIDOBJECT:                       throw Error("DDERR_INVALIDOBJECT");
        case DDERR_INVALIDPARAMS:                       throw Error("DDERR_INVALIDPARAMS");
        case DDERR_INVALIDPIXELFORMAT:                  throw Error("DDERR_INVALIDPIXELFORMAT");
        case DDERR_INVALIDRECT:                         throw Error("DDERR_INVALIDRECT");
        case DDERR_LOCKEDSURFACES:                      throw Error("DDERR_LOCKEDSURFACES");
        case DDERR_NO3D:                                throw Error("DDERR_NO3D");
        case DDERR_NOALPHAHW:                           throw Error("DDERR_NOALPHAHW");
        case DDERR_NOCLIPLIST:                          throw Error("DDERR_CLIPLIST");
        case DDERR_NOCOLORCONVHW:                       throw Error("DDERR_NOCOLORCONVHW");
        case DDERR_NOCOOPERATIVELEVELSET:               throw Error("DDERR_NOCOOPERATIVELEVELSET");
        case DDERR_NOCOLORKEY:                          throw Error("DDERR_NOCOLORKEY");
        case DDERR_NOCOLORKEYHW:                        throw Error("DDERR_NOCOLORKEYHW");
        case DDERR_NODIRECTDRAWSUPPORT:                 throw Error("DDERR_NODIRECTDRAWSUPPORT");
        case DDERR_NOEXCLUSIVEMODE:                     throw Error("DDERR_NOEXCLUSIVEMODE");
        case DDERR_NOFLIPHW:                            throw Error("DDERR_NOFLIPHW");
        case DDERR_NOGDI:                               throw Error("DDERR_NOGDI");
        case DDERR_NOMIRRORHW:                          throw Error("DDERR_NOMIRRORHW");
        case DDERR_NOTFOUND:                            throw Error("DDERR_NOTFOUND");
        case DDERR_NOOVERLAYHW:                         throw Error("DDERR_NOOVERLAYHW");
        case DDERR_NORASTEROPHW:                        throw Error("DDERR_NORASTEROPHW");
        case DDERR_NOROTATIONHW:                        throw Error("DDERR_NOROTATIONHW");
        case DDERR_NOSTRETCHHW:                         throw Error("DDERR_NOSTRETCHHW");
        case DDERR_NOT4BITCOLOR:                        throw Error("DDERR_NOT4BITCOLOR");
        case DDERR_NOT4BITCOLORINDEX:                   throw Error("DDERR_NOT4BITCOLORINDEX");
        case DDERR_NOT8BITCOLOR:                        throw Error("DDERR_NOT8BITCOLOR");
        case DDERR_NOTEXTUREHW:                         throw Error("DDERR_NOTEXTUREHW");
        case DDERR_NOVSYNCHW:                           throw Error("DDERR_NOVSYNCHW");
        case DDERR_NOZBUFFERHW:                         throw Error("DDERR_NOZBUFFERHW");
        case DDERR_NOZOVERLAYHW:                        throw Error("DDERR_NOZOVERLAYHW");
        case DDERR_OUTOFCAPS:                           throw Error("DDERR_OUTOFCAPS");
        case DDERR_OUTOFMEMORY:                         throw Error("DDERR_OUTOFMEMORY");
        case DDERR_OUTOFVIDEOMEMORY:                    throw Error("DDERR_OUTOFVIDEOMEMORY");
        case DDERR_OVERLAYCANTCLIP:                     throw Error("DDERR_OVERLAYCANTCLIP");
        case DDERR_OVERLAYCOLORKEYONLYONEACTIVE:        throw Error("DDERR_OVERLAYCOLORKEYONLYONEACTIVE");
        case DDERR_PALETTEBUSY:                         throw Error("DDERR_PALETTEBUSY");
        case DDERR_COLORKEYNOTSET:                      throw Error("DDERR_COLORKEYNOTSET");
        case DDERR_SURFACEALREADYATTACHED:              throw Error("DDERR_SURFACEALREADYATTACHED");
        case DDERR_SURFACEALREADYDEPENDENT:             throw Error("DDERR_SURFACEALREADYDEPENDENT");
        case DDERR_SURFACEBUSY:                         throw Error("DDERR_SURFACEBUSY");
        case DDERR_CANTLOCKSURFACE:                     throw Error("DDERR_CANTLOCKSURFACE");
        case DDERR_SURFACEISOBSCURED:                   throw Error("DDERR_SURFACEISOBSCURED");
        case DDERR_SURFACELOST:                         throw Error("DDERR_SURFACELOST");
        case DDERR_SURFACENOTATTACHED:                  throw Error("DDERR_SURFACENOTATTACHED");
        case DDERR_TOOBIGHEIGHT:                        throw Error("DDERR_TOOBIGHEIGHT");
        case DDERR_TOOBIGSIZE:                          throw Error("DDERR_TOOBIGSIZE");
        case DDERR_TOOBIGWIDTH:                         throw Error("DDERR_TOOBIGWIDTH");
        case DDERR_UNSUPPORTED:                         throw Error("DDERR_UNSUPPORTED");
        case DDERR_UNSUPPORTEDFORMAT:                   throw Error("DDERR_UNSUPPORTEDFORMAT");
        case DDERR_UNSUPPORTEDMASK:                     throw Error("DDERR_UNSUPPORTEDMASK");
        case DDERR_VERTICALBLANKINPROGRESS:             throw Error("DDERR_VERTICALBLANKINPROGRESS");
        case DDERR_WASSTILLDRAWING:                     throw Error("DDERR_WASSTILLDRAWING");
        case DDERR_XALIGN:                              throw Error("DDERR_XALIGN");
        case DDERR_INVALIDDIRECTDRAWGUID:               throw Error("DDERR_INVALIDDIRECTDRAWGUID");
        case DDERR_DIRECTDRAWALREADYCREATED:            throw Error("DDERR_DIRECTDRAWALREADYCREATED");
        case DDERR_NODIRECTDRAWHW:                      throw Error("DDERR_NODIRECTDRAWHW");
        case DDERR_PRIMARYSURFACEALREADYEXISTS:         throw Error("DDERR_PRIMARYSURFACEALREADYEXISTS");
        case DDERR_NOEMULATION:                         throw Error("DDERR_NOEMULATION");
        case DDERR_REGIONTOOSMALL:                      throw Error("DDERR_REGIONTOOSMALL");
        case DDERR_CLIPPERISUSINGHWND:                  throw Error("DDERR_CLIPPERISUSINGHWND");
        case DDERR_NOCLIPPERATTACHED:                   throw Error("DDERR_NOCLIPPERATTACHED");
        case DDERR_NOHWND:                              throw Error("DDERR_NOHWND");
        case DDERR_HWNDSUBCLASSED:                      throw Error("DDERR_HWNDSUBCLASSED");
        case DDERR_HWNDALREADYSET:                      throw Error("DDERR_HWNDALREADYSET");
        case DDERR_NOPALETTEATTACHED:                   throw Error("DDERR_NOPALETTEATTACHED");
        case DDERR_NOPALETTEHW:                         throw Error("DDERR_NOPALETTEHW");
        case DDERR_BLTFASTCANTCLIP:                     throw Error("DDERR_BLTFASTCANTCLIP");
        case DDERR_NOBLTHW:                             throw Error("DDERR_NOBLTHW");
        case DDERR_NODDROPSHW:                          throw Error("DDERR_NODDROPSHW");
        case DDERR_OVERLAYNOTVISIBLE:                   throw Error("DDERR_OVERLAYNOTVISIBLE");
        case DDERR_NOOVERLAYDEST:                       throw Error("DDERR_NOOVERLAYDEST");
        case DDERR_INVALIDPOSITION:                     throw Error("DDERR_INVALIDPOSITION");
        case DDERR_NOTAOVERLAYSURFACE:                  throw Error("DDERR_NOTAOVERLAYSURFACE");
        case DDERR_EXCLUSIVEMODEALREADYSET:             throw Error("DDERR_EXCLUSIVEMODEALREADYSET");
        case DDERR_NOTFLIPPABLE:                        throw Error("DDERR_EXCLUSIVEMODEALREADYSET");
        case DDERR_CANTDUPLICATE:                       throw Error("DDERR_CANTDUPLICATE");
        case DDERR_NOTLOCKED:                           throw Error("DDERR_NOTLOCKED");
        case DDERR_CANTCREATEDC:                        throw Error("DDERR_CANTCREATEDC");
        case DDERR_NODC:                                throw Error("DDERR_NODC");
        case DDERR_WRONGMODE:                           throw Error("DDERR_WRONGMODE");
        case DDERR_IMPLICITLYCREATED:                   throw Error("DDERR_IMPLICITLYCREATED");
        case DDERR_NOTPALETTIZED:                       throw Error("DDERR_NOPALETTIZED");
        case DDERR_UNSUPPORTEDMODE:                     throw Error("DDERR_UNSUPPORTEDMODE");
        case DDERR_NOMIPMAPHW:                          throw Error("DDERR_NOMIPMAPHW");
        case DDERR_INVALIDSURFACETYPE:                  throw Error("DDERR_INVALIDSURFACETYPE");
        case DDERR_DCALREADYCREATED:                    throw Error("DDERR_DCALREADYCREATED");
        case DDERR_CANTPAGELOCK:                        throw Error("DDERR_CANTPAGELOCK");
        case DDERR_CANTPAGEUNLOCK:                      throw Error("DDERR_CANTPAGEUNLOCK");
        case DDERR_NOTPAGELOCKED:                       throw Error("DDERR_NOTPAGELOCKED");
        case DDERR_NOTINITIALIZED:                      throw Error("DDERR_NOTINITIALIZED");
    }

#endif

    // default error message
    char message[256];
    wsprintf(message,"DDERR 0x%X",result);
    throw Error(message);
}


void DirectXCheck(HRESULT result,const char message[])
{
    try
    {
        // check result
        DirectXCheck(result);
    }
    catch (Error &error)
    {
        // composite error message
        throw Error(message,error);
    }
}




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif
