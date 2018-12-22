/*********************************************************************NVMH1****
File:
dtMachineState.h

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/

#ifndef DTMACHINESTATE_H	   
#define DTMACHINESTATE_H	   

typedef struct dtTextureStage
{
    //dtpTexture pTexture; 
    D3DTEXTUREMIPFILTER mipfilter;
    D3DTEXTUREMAGFILTER magfilter;
    D3DTEXTUREMINFILTER minfilter;
    
    D3DTEXTUREOP colorop;
    int colorarg1, color1comp, color1arep;
    int colorarg2, color2comp, color2arep;
    
    D3DTEXTUREOP alphaop;
    int alphaarg1, alpha1comp;
    int alphaarg2, alpha2comp;
    
    
    D3DTEXTUREADDRESS address;
    
    D3DTEXTUREADDRESS textureaddress;
    int texcoordindex;
    int miplevels;
    DWORD maxanisotropy;
    D3DCOLOR bordercolor;
    Float32 mipmaplodbias;
    DWORD maxmiplevel;
    D3DTEXTURETRANSFORMFLAGS texturetransformflags;

} dtTextureStage;


typedef struct _dtMachineStateData
{
    LPDIRECT3DDEVICE_TYPE    pd3d_device;       // The D3D device
    
    dtTextureStage ts[8];  //  8 stage pipeline
    
    D3DANTIALIASMODE antialiasmode;
    D3DZBUFFERTYPE zbuffertype;
    D3DFILLMODE fillmode;
    D3DSHADEMODE shademode;      
    DWORD linepattern;     
    bool zwriteenable;
    
    bool alphatestenable;
    bool lastpixel;
    D3DBLEND srcblend;
    D3DBLEND destblend;
    D3DCULL cullmode;
    D3DCMPFUNC zfunc;
    BYTE alpharef;
    D3DCMPFUNC alphafunc;
    bool ditherenable;
    bool alphablendenable;
    bool fogenable;
    bool specularenable;
    bool zvisible;
    bool stippledalpha;
    D3DCOLOR fogcolor;

    D3DFOGMODE fog_vertex_mode;
    Float32 fog_vertex_start;
    Float32 fog_vertex_end;
    Float32 fog_vertex_density;

    D3DFOGMODE fog_table_mode;
    Float32 fog_table_density;
    Float32 fog_table_start;
    Float32 fog_table_end;

    bool edgeantialias;
    bool colorkeyenable;
    bool rangefogenable;

    bool stencil_enable;
    D3DSTENCILOP stencil_fail;
    D3DSTENCILOP stencil_zfail;
    D3DSTENCILOP stencil_pass;
    D3DCMPFUNC stencil_func;
    BYTE stencil_ref;
    BYTE stencil_mask;
    BYTE stencil_write_mask;
    bool clipping;
    bool lighting;
    bool extents;
    D3DCOLOR ambient;      
    bool colorvertex;
    bool localviewer;
    bool normalizenormals;  
    bool colorkeyblendenable;
    D3DMATERIALCOLORSOURCE diffusematerialsource;
    D3DMATERIALCOLORSOURCE ambientmaterialsource;
    D3DMATERIALCOLORSOURCE specularmaterialsource;
    D3DMATERIALCOLORSOURCE emissivematerialsource;
    D3DVERTEXBLENDFLAGS vertexblendflags;
    DWORD clipplaneenable;
    LONG zbias;
    
    DWORD texturefactor;

    LPD3DMATERIAL7 lpMaterial;
    
} _dtMachineStateData;

class dtMachineState
{
public:
    static void SetDefaultRenderStates();
//    static void RestoreRenderState();
//    static HRESULT Create3DDevice(LPDIRECT3D_TYPE pD3D, GUID & guid, LPDIRECTDRAWSURFACE_TYPE pddsBackBuffer);

    _dtMachineStateData m;

    static dtMachineState * m_pInstance;
    static dtMachineState * GetpInstance() { return m_pInstance;}
	
	static LPDIRECT3DDEVICE_TYPE & GetpD3DDevice() {	return msData().pd3d_device; }
	// LPDIRECT3DDEVICE_TYPE & dtMachineState::GetpD3DDevice()


    static _dtMachineStateData * pdtData() { return &m_pInstance->m;}
    static _dtMachineStateData & msData() { return m_pInstance->m;}

	static HRESULT SetTexture(const int &stage, dtpTexture pt) 
    { 
        if (pt)  
            return pt->SetTextureInStage(stage);
        else
            return GetpD3DDevice()->SetTexture(stage, 0);
    
    }
    static HRESULT SetTexture(const int &stage, dtpCubeMap cm) 
    { 
        if (cm)  
            return GetpD3DDevice()->SetTexture(stage, cm->GetSurface(0) );
        else
            return GetpD3DDevice()->SetTexture(stage, 0 );    
    }
    static HRESULT SetTexture(const int &stage, LPDIRECTDRAWSURFACE_TYPE pt) 
    { 
        return GetpD3DDevice()->SetTexture(stage, pt);
    }
    static HRESULT GetTexture(const int &stage, LPDIRECTDRAWSURFACE_TYPE * lplpTexture) 
    { 
        return GetpD3DDevice()->GetTexture(stage, lplpTexture) ;
    }
//    */

    dtMachineState();
    

    // matrices are not saved in state
/*    static HRESULT SetTextureStageState(const DWORD &dwStage, const D3DTEXTURESTAGESTATETYPE &dwState,  const DWORD dwValue) 
    {
        return GetpD3DDevice()->SetTextureStageState(dwStage, dwState, dwValue);
    }

/*
    static HRESULT SetAntiAlias(const D3DANTIALIASMODE &mode) 
    { 
        msData().antialiasmode = mode;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_ANTIALIAS          , mode);
    }    /*  */
/*    static HRESULT EnableZ(const D3DZBUFFERTYPE &type) 
    { 
        msData().zbuffertype = type;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_ZENABLE    , type);
    }    
    /* D3DZBUFFERTYPE (or true/false for legacy) */
/*    static HRESULT SetFillMode(const D3DFILLMODE &fillmode) 
    {
        msData().fillmode = fillmode;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_FILLMODE           , fillmode);
    }    
    /*         */
/*    static HRESULT SetShadeMode(const D3DSHADEMODE &shademode) 
    {
        msData().shademode = shademode;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_SHADEMODE          , shademode); 
    }    
    /*  */
/*    static HRESULT SetLinePattern(const DWORD &pat) 
    {
        msData().linepattern = pat;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_LINEPATTERN        , pat);
    }   
    /*  */
/*    static HRESULT EnableZWrite(const bool &b) 
    {
        msData().zwriteenable = b;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE       , b); 
    }    /* true to enable z writes */
/*    static HRESULT EnableAlphaTest(const bool &b) 
    {
        msData().alphatestenable = b;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE    , b); 
    }   /* true to enable alpha tests */
/*    static HRESULT EnableLastPixel(const bool &b) 
    {
        msData().lastpixel = b;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_LASTPIXEL          , b); 
    }   /* true for last-pixel on lines */
/*    static HRESULT SetAlphaSrcBlend(const D3DBLEND &blend) 
    { 
        msData().srcblend = blend;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_SRCBLEND           , blend) ;
    }   /*  */
/*    static HRESULT SetAlphaDestBlend(const D3DBLEND &blend) 
    { 
        msData().destblend = blend;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_DESTBLEND          , blend) ;
    }   /* D3DBLEND */
/*    static HRESULT SetCullMode(const D3DCULL &cullmode) 
    { 
        msData().cullmode = cullmode;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_CULLMODE           , cullmode) ;
    }   /*  */
/*    static HRESULT SetZFunc(const D3DCMPFUNC &func) 
    { 
        msData().zfunc = func;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_ZFUNC              , func) ;
    }  /*  */
/*    static HRESULT SetAlphaRef(const BYTE &ref) 
    { 
        msData().alpharef = ref;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_ALPHAREF, ref) ;
    }   /*  */
/*    static HRESULT SetAlphaFunc(const D3DCMPFUNC &func) 
    { 
        msData().alphafunc = func;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_ALPHAFUNC          , func) ;
    }  /*  */
/*    static HRESULT EnableDither(const bool &b) 
    { 
        msData().ditherenable = b;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_DITHERENABLE       , b); 
    }   /* true to enable dithering */
/*    static HRESULT EnableAlphaBlend(const bool &b) 
    { 
        msData().alphablendenable = b;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE        , b); 
    }   /* true to enable alpha blending */
/*    static HRESULT EnableFog(const bool &b) 
    { 
        msData().fogenable = b;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_FOGENABLE          , b); 
    }  /* true to enable fog */
/*    static HRESULT EnableSpecular(const bool &b) 
    { 
        msData().specularenable = b;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_SPECULARENABLE     , b); 
    }   /* true to enable specular */
/*    static HRESULT EnableZVisible(const bool &b) 
    { 
        msData().zvisible = b;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_ZVISIBLE           , b) ;
    }   /* true to enable z checking */
/*    static HRESULT EnableStippledAlpha(const bool &b) 
    { 
        msData().stippledalpha = b;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_STIPPLEDALPHA      , b); 
    }   /* true to enable stippled alpha */
/*    static HRESULT SetFogColor(const D3DCOLOR &color) 
    { 
        msData().fogcolor = color;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_FOGCOLOR           , color) ;
    }   /*  */
/*    static HRESULT SetFogTableMode(const D3DFOGMODE &fogmode) 
    { 
        msData().fog_table_mode = fogmode;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_FOGTABLEMODE       , fogmode) ;
    }   /*  */
/*    static HRESULT SetFogTableStart(const Float32 & fogstart) 
    {
        msData().fog_table_start = fogstart;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_FOGTABLESTART , *(DWORD *)&fogstart) ;
    }
	*/
/*    static HRESULT SetFogTableEnd(const Float32 & fogend) 
    { 
        msData().fog_table_end = fogend;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_FOGTABLEEND , *(DWORD *)&fogend) ; 
    }   */
/*    static HRESULT SetFogTableDensity(const Float32 & fogdensity)  
    { 
        msData().fog_table_density = fogdensity;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_FOGTABLEDENSITY , *(DWORD *)&fogdensity) ; 
    } 
*/	
/*    static HRESULT EnableEdgeAntiAlias(const bool &b) 
    { 
        msData().edgeantialias = b;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_EDGEANTIALIAS      , b); 
    }   /* true to enable edge antialiasing */
/*    static HRESULT EnableColorkey(const bool &b) 
    { 
        msData().colorkeyenable = b;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE     , b); 
    }   /* true to enable source colorkeyed textures */
/*    static HRESULT SetZBias(const LONG &zbias) 
    { 
        msData().zbias = zbias;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_ZBIAS              , zbias) ;
    }   /* LONG Z bias */
/*    static HRESULT EnableRangeFog(const bool &b) 
    { 
        msData().rangefogenable = b;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_RANGEFOGENABLE     , b); 
    }   /* Enables range-based fog */
/*    static HRESULT EnableStencil(const bool &b) 
    { 
        msData().stencil_enable = b;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_STENCILENABLE      , b); 
    }   /* bool enable/disable stenciling */
/*    static HRESULT SetStencilFail(const D3DSTENCILOP &op) 
    { 
        msData().stencil_fail = op;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_STENCILFAIL        , op) ;
    }   /* D3DSTENCILOP to do if stencil test fails */
/*    static HRESULT SetStencilZFail(const D3DSTENCILOP &op) 
    { 
        msData().stencil_zfail = op;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_STENCILZFAIL       , op) ;
    }   /* D3DSTENCILOP to do if stencil test passes and Z test fails */
/*    static HRESULT SetStencilPass(const D3DSTENCILOP &op) 
    { 
        msData().stencil_pass = op;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_STENCILPASS        , op) ;
    }   /* D3DSTENCILOP to do if both stencil and Z tests pass */
/*    static HRESULT SetStencilFunc(const D3DCMPFUNC &func) 
    { 
        msData().stencil_func = func;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_STENCILFUNC        , func) ;
    }   /* D3DCMPFUNC fn.  Stencil Test passes if ((ref & mask) stencilfn (stencil & mask)) is true */
/*    static HRESULT SetStencilRef(const BYTE &ref) 
    { 
        msData().stencil_ref = ref;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_STENCILREF         , ref) ;
    }   /* Reference value used in stencil test */
/*    static HRESULT SetStencilMask(const BYTE &mask) 
    { 
        msData().stencil_mask = mask;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_STENCILMASK        , mask) ;
    }   /* Mask value used in stencil test */
/*    static HRESULT SetStencilWriteMask(const BYTE &mask) 
    { 
        msData().stencil_write_mask = mask;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_STENCILWRITEMASK   , mask) ;
    }   /* Write mask applied to values written to stencil buffer */
/*    static HRESULT SetTextureFactor(const DWORD &factor) 
    { 
        msData().texturefactor = factor;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_TEXTUREFACTOR      , factor) ;
    }  /* D3DCOLOR used for multi-texture blend */

/*    static HRESULT EnableClipping(const bool & b)                
    { 
        msData().clipping = b;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_CLIPPING, b);               
    }
/*    static HRESULT EnableLighting(const bool & b)                
    { 
        msData().lighting = b;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_LIGHTING               , b);
    }
*/
/*    static HRESULT EnableExtents(const bool & b)                 
    { 
        msData().extents = b;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_EXTENTS                , b);
    }
/*    static HRESULT SetAmbientLight(const DWORD & color)                 
    { 
        msData().ambient = color;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_AMBIENT                , color);
    }
*/
/*    static HRESULT SetFogVertexMode(const D3DFOGMODE & mode)           
    { 
        msData().fog_vertex_mode = mode;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_FOGVERTEXMODE          , mode);
    }
/*    static HRESULT EnableColorVertex(const bool & b)             
    { 
        msData().colorvertex = b;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_COLORVERTEX            , b);
    }
/*    static HRESULT EnableLocalViewer(const bool & b)             
    { 
        msData().localviewer = b;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_LOCALVIEWER            , b);
    }
/*    static HRESULT EnableNormalizeNormals(const bool & b)        
    { 
        msData().normalizenormals = b;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_NORMALIZENORMALS       , b);
    }
/*    static HRESULT EnableColorkeyBlend(const bool & b)     
    { 
        msData().colorkeyblendenable =b;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_COLORKEYBLENDENABLE    , b);
    }
*/
/*    static HRESULT SetDiffuseMaterialSource(const D3DMATERIALCOLORSOURCE & src)   
    { 
        msData().diffusematerialsource  = src;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_DIFFUSEMATERIALSOURCE  , src);
    }
/*    static HRESULT SetSpecularMaterialSource(const D3DMATERIALCOLORSOURCE & src)  
    { 
        msData().specularmaterialsource  = src;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_SPECULARMATERIALSOURCE  , src);
    }
/*    static HRESULT SetAmbientMaterialSource(const D3DMATERIALCOLORSOURCE & src)   
    { 
        msData().ambientmaterialsource  = src;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_AMBIENTMATERIALSOURCE   , src);
    }
/*    static HRESULT SetEmissiveMaterialSource(const D3DMATERIALCOLORSOURCE & src)  
    { 
        msData().emissivematerialsource  = src;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_EMISSIVEMATERIALSOURCE  , src);
    }
/*    static HRESULT SetVertexBlend(const D3DVERTEXBLENDFLAGS & flags)             
    { 
        msData().vertexblendflags = flags;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_VERTEXBLEND             , flags);
    }
/*    static HRESULT EnableClipPlane(const DWORD & planes)         
    { 
        msData().clipplaneenable = planes;
        return GetpD3DDevice()->SetRenderState(D3DRENDERSTATE_CLIPPLANEENABLE         , planes);
    }
	*/
/*    static HRESULT SetColorOp(const int &stage,   const D3DTEXTUREOP &op) 
    { 
        msData().ts[stage].colorop = op;
        return GetpD3DDevice()->SetTextureStageState(stage, D3DTSS_COLOROP        , op);
    } /* D3DTEXTUREOP - per-stage blending controls for color channels */
/*    static HRESULT SetColorArg1(const int &stage,   const DWORD &arg) 
    { 
        msData().ts[stage].colorarg1 = arg;
        return GetpD3DDevice()->SetTextureStageState(stage, D3DTSS_COLORARG1      ,  arg);
    } /* D3DTA_* (texture arg) */
/*    static HRESULT SetColorArg2(const int &stage,   const DWORD &arg) 
    { 
        msData().ts[stage].colorarg2 = arg;
        return GetpD3DDevice()->SetTextureStageState(stage, D3DTSS_COLORARG2      ,  arg);
    } /* D3DTA_* (texture arg) */
/*    static HRESULT SetAlphaOp(const int &stage,   const D3DTEXTUREOP &op ) 
    { 
        msData().ts[stage].alphaop = op;
        return GetpD3DDevice()->SetTextureStageState(stage, D3DTSS_ALPHAOP        , op );
    } /* D3DTEXTUREOP - per-stage blending controls for alpha channel */
/*    static HRESULT SetAlphaArg1(const int &stage,   const DWORD &arg) 
    { 
        msData().ts[stage].alphaarg1 = arg;
        return GetpD3DDevice()->SetTextureStageState(stage, D3DTSS_ALPHAARG1      ,  arg);
    } /* D3DTA_* (texture arg) */
/*    static HRESULT SetAlphaArg2(const int &stage,   const DWORD &arg) 
    { 
        msData().ts[stage].alphaarg2 = arg;
        return GetpD3DDevice()->SetTextureStageState(stage, D3DTSS_ALPHAARG2      ,  arg);
    } /* D3DTA_* (texture arg) */

/*
    static HRESULT SetBumpEnvMat00(const int &stage,    const Float32 &v) 
    { 
        return GetpD3DDevice()->SetTextureStageState(stage, D3DTSS_BUMPENVMAT00   , *(DWORD *)&v);
    } /* D3DVALUE (bump mapping matrix) */
/*    static HRESULT SetBumpEnvMat01(const int &stage,    const Float32 &v) 
    { 
        return GetpD3DDevice()->SetTextureStageState(stage, D3DTSS_BUMPENVMAT01   , *(DWORD *)&v);
    } /* D3DVALUE (bump mapping matrix) */
/*    static HRESULT SetBumpEnvMat10(const int &stage,    const Float32 &v) 
    { 
        return GetpD3DDevice()->SetTextureStageState(stage, D3DTSS_BUMPENVMAT10   , *(DWORD *)&v);
    } /* D3DVALUE (bump mapping matrix) */
/*    static HRESULT SetBumpEnvMat11(const int &stage,    const Float32 &v) 
    { 
        return GetpD3DDevice()->SetTextureStageState(stage, D3DTSS_BUMPENVMAT11   , *(DWORD *)&v);
    } /* D3DVALUE (bump mapping matrix) */


/*    static HRESULT SetTexCoordIndex(const int &stage,   const DWORD &index) 
    { 
        msData().ts[stage].texcoordindex = index;
        return GetpD3DDevice()->SetTextureStageState(stage, D3DTSS_TEXCOORDINDEX  , index );
    } /* identifies which set of texture coordinates index this texture */


/*    static HRESULT SetTextureAddress(const int &stage,   const D3DTEXTUREADDRESS &addr) 
    { 
        msData().ts[stage].address = addr;
        return GetpD3DDevice()->SetTextureStageState(stage, D3DTSS_ADDRESS        , addr);
    } /* D3DTEXTUREADDRESS for both coordinates */

/*
    static HRESULT SetTextureAddressU(const int &stage,   const D3DTEXTUREADDRESS &addr) 
    { 
        //msData().ts[stage].addressu = addr;
        return GetpD3DDevice()->SetTextureStageState(stage, D3DTSS_ADDRESSU       , addr);
    } /* D3DTEXTUREADDRESS for U coordinate */
/*    static HRESULT SetTextureAddressV(const int &stage,   const D3DTEXTUREADDRESS &addr) 
    { 
        //msData().ts[stage].addressv = addr;
        return GetpD3DDevice()->SetTextureStageState(stage, D3DTSS_ADDRESSV       , addr);
    } /* D3DTEXTUREADDRESS for V coordinate */
/*    static HRESULT SetBorderColor(const int &stage,   const D3DCOLOR &color) 
    { 
        msData().ts[stage].bordercolor = color;
        return GetpD3DDevice()->SetTextureStageState(stage, D3DTSS_BORDERCOLOR    , color);
    } /* D3DCOLOR */
/*    static HRESULT SetMagFilter(const int &stage,   const D3DTEXTUREMAGFILTER &filter) 
    { 
        msData().ts[stage].magfilter = filter;
        return GetpD3DDevice()->SetTextureStageState(stage, D3DTSS_MAGFILTER      , filter);
    } /* D3DTEXTUREMAGFILTER filter to use for magnification */
/*    static HRESULT SetMinFilter(const int &stage,   const D3DTEXTUREMINFILTER &filter) 
    { 
        msData().ts[stage].minfilter = filter;
        return GetpD3DDevice()->SetTextureStageState(stage, D3DTSS_MINFILTER      , filter);
    } /* D3DTEXTUREMINFILTER filter to use for minification */
/*    static HRESULT SetMipFilter(const int &stage,   const D3DTEXTUREMIPFILTER &filter) 
    { 
        msData().ts[stage].mipfilter = filter;
        return GetpD3DDevice()->SetTextureStageState(stage, D3DTSS_MIPFILTER      , filter);
    } /* D3DTEXTUREMIPFILTER filter to use between mipmaps during minification */
/*    static HRESULT SetMipMapLODBias(const int &stage,    const Float32 & zbias) 
    { 
        msData().ts[stage].mipmaplodbias = zbias;
        return GetpD3DDevice()->SetTextureStageState(stage, D3DTSS_MIPMAPLODBIAS  , *(DWORD *)&zbias);
    }/* D3DVALUE Mipmap LOD bias */
/*    static HRESULT SetMaxMipLevel(const int &stage,   const DWORD &maxlevel) 
    { 
        msData().ts[stage].maxmiplevel = maxlevel;
        return GetpD3DDevice()->SetTextureStageState(stage, D3DTSS_MAXMIPLEVEL    , maxlevel);
    } /* DWORD 0..(n-1) LOD index of largest map to use (0 ,, largest) */
/*    static HRESULT SetMaxAnisotropy(const int &stage,   const DWORD &maxani) 
    { 
        msData().ts[stage].maxanisotropy = maxani;
        return GetpD3DDevice()->SetTextureStageState(stage, D3DTSS_MAXANISOTROPY  , maxani);
    } /* DWORD maximum anisotropy */
/*    static HRESULT SetBumpEnvlScale(const int &stage,   const DWORD &bs) 
    { 
        //msData().ts[stage]. = ;
        return GetpD3DDevice()->SetTextureStageState(stage, D3DTSS_BUMPENVLSCALE  , bs);
    } /* DWORD scale for bump map luminance */
/*    static HRESULT SetBumpEnvlOffset(const int &stage,    const Float32 & be) 
    { 
        return GetpD3DDevice()->SetTextureStageState(stage, D3DTSS_BUMPENVLOFFSET , *(DWORD *)&be);
    } /* D3DVALUE offset for bump map luminance */


/*    static HRESULT SetTextureTransformFlags(const int &stage, const D3DTEXTURETRANSFORMFLAGS & flags) 
    { 
        msData().ts[stage].texturetransformflags = flags;
        
        return GetpD3DDevice()->SetTextureStageState(stage, D3DTSS_TEXTURETRANSFORMFLAGS , flags);
    }
*/


/*   static HRESULT SetMaterial( LPD3DMATERIAL7 lpMaterial) 
   {
       msData().lpMaterial = lpMaterial;
       return GetpD3DDevice()->SetMaterial(lpMaterial);
   }
    */
    
   
//    static HRESULT SetVertexFogMode(const D3DFOGMODE &vertexfogmode) ;
//    static HRESULT SetVertexFogStart(const Float32 & vertexfogstart) ;
//    static HRESULT SetVertexFogEnd(const Float32 & vertexfogend);
//    static HRESULT SetVertexFogDensity(const Float32 & vertexfogdensity);
    
    
    


/*
 
    static HRESULT GetRenderState(const D3DRENDERSTATETYPE &op, const LPDWORD data) 
    {
        return GetpD3DDevice()->GetRenderState(op, data);
    }
    static HRESULT GetCullMode(D3DCULL * cullmode) 
    { 
        return GetpD3DDevice()->GetRenderState(D3DRENDERSTATE_CULLMODE, (LPDWORD)cullmode) ;
    }   /*  */
/*    static HRESULT GetMaterial( LPD3DMATERIAL7 lpMaterial ) 
    { 
        return GetpD3DDevice()->GetMaterial(lpMaterial);
    }*/

/*
    static HRESULT SetViewport(D3DVIEWPORT_TYPE *lpViewport) 
    { 
        return GetpD3DDevice()->SetViewport(lpViewport);
    }
*/


};
    
#endif
