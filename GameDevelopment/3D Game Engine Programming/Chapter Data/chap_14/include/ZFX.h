/*******************************************************************
 * ZFXEngine!                                                      *
 * (c)2003 by Stefan Zerbst | www.zfx.info                         *
 *-----------------------------------------------------------------*
 * File: ZFX.h                                                     *
 * defining everything that needs to be globally known             *
 *******************************************************************/

#ifndef ZFX_H
#define ZFX_H

#include <windows.h>
#include "zfx3d.h"

// everything went smooth
#define ZFX_OK                        S_OK

// just reports no errors
#define ZFX_CANCELED                  0x82000000

// generell error message
#define ZFX_FAIL                      0x82000001

// specific error messages
#define ZFX_CREATEAPI                 0x82000002
#define ZFX_CREATEDEVICE              0x82000003
#define ZFX_CREATEBUFFER              0x82000004
#define ZFX_INVALIDPARAM              0x82000005
#define ZFX_INVALIDID                 0x82000006
#define ZFX_BUFFERSIZE                0x82000007
#define ZFX_BUFFERLOCK                0x82000008
#define ZFX_NOTCOMPATIBLE             0x82000009
#define ZFX_OUTOFMEMORY               0x8200000a
#define ZFX_FILENOTFOUND              0x8200000b
#define ZFX_INVALIDFILE               0x8200000c
#define ZFX_NOSHADERSUPPORT           0x8200000d


#define DWORD_ARGB(a,r,g,b) ((DWORD)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)));


// VERTEX TYPES:

typedef struct PVERTEX_TYPE {
   float	 x, y, z;
   } PVERTEX;

typedef struct VERTEX_TYPE {
   float	 x, y, z;
	float  vcN[3];
   float  tu, tv;
   } VERTEX;

typedef struct LVERTEX_TYPE {
   float	 x, y, z;
	DWORD  Color;
   float  tu, tv;
   } LVERTEX;

typedef struct CVERTEX_TYPE {
   float	 x, y, z;
	float  vcN[3];
   float  tu, tv;
   float  fBone1, fWeight1; 
   float  fBone2, fWeight2;
   } CVERTEX;

typedef struct VERTEX3T_TYPE {
   float	 x, y, z;
   float  vcN[3];
   float  tu0, tv0;
   float  tu1, tv1;
   float  tu2, tv2;
   } VERTEX3T;

/*----------------------------------------------------------------*/


// ENUMERATED TYPES:

typedef enum ZFXVERTEXID_TYPE {
   VID_PS,       // untransformed position only
   VID_UU,       // untransformed and unlit vertex
   VID_UL,       // untransformed and lit vertex vertex
   VID_CA,       // used for character animation
   VID_3T,       // three texture coord pairs
   } ZFXVERTEXID;


typedef enum ZFXENGINEMODE_TYPE {
   EMD_PERSPECTIVE,      // perspective projection
   EMD_TWOD,             // world coords equal to screen coords
   EMD_ORTHOGONAL        // orthogonal projection
   } ZFXENGINEMODE;

typedef enum ZFXLIGHTID_TYPE {
   LGT_DIRECTIONAL,  // directional light source
   LGT_POINT,        // point light source
   LGT_SPOT          // spot light source
   } ZFXLIGHTID;

typedef enum ZFXINPUTDEV_TYPE {
   IDV_KEYBOARD,
   IDV_JOYSTICK,
   IDV_MOUSE
   } ZFXINPUTDEV;

typedef enum ZFXNETMODE_TYPE {
   NMD_SERVER=0,
   NMD_CLIENT=1,
   } ZFXNETMODE;

typedef enum ZFXSIDE_TYPE {
   SDE_RIGHT,
   SDE_LEFT,
   SDE_UP,
   SDE_DOWN,
   SDE_NONE
   } ZFXSIDE;

typedef enum ZFXMCAPART_TYPE {
   APT_ELEVATOR,
   APT_RUDDER,
   APT_FLAP,
   APT_AILERON,
   APT_OTHER
   } ZFXMCAPART;

typedef enum ZFXENTITY_TYPE {
   ENT_UNKNOWN,
   ENT_DOOR,
   ENT_LIFT,
   ENT_PLAYER,
   ENT_BOT
   } ZFXENTITY;
 
typedef enum ZFXRENDERSTATE_TYPE {
   RS_NONE,             // just nothing
   RS_CULL_CW,          // cull clockwise ordered triangles
   RS_CULL_CCW,         // cull counter cw ordered triangles
   RS_CULL_NONE,        // render front- and backsides
   RS_DEPTH_READWRITE,  // read and write depth buffer
   RS_DEPTH_READONLY,   // read but don't write depth buffer
   RS_DEPTH_NONE,       // no read or write with depth buffer
   RS_SHADE_POINTS,     // render just vertices
   RS_SHADE_LINES,      // render two verts as one line
   RS_SHADE_TRIWIRE,    // render triangulated wire
   RS_SHADE_HULLWIRE,   // render polyline
   RS_SHADE_SOLID,      // render solid polygons
   RS_TEX_ADDSIGNED,    // texture stage operation
   RS_TEX_MODULATE      // texture stage operation
   } ZFXRENDERSTATE;
/*----------------------------------------------------------------*/


// STRUCTURES:
// plain viewport type
typedef struct ZFXVIEWPORT_TYPE {
   DWORD X;
   DWORD Y;
   DWORD Width;
   DWORD Height;
   } ZFXVIEWPORT;

// generic skin type
typedef struct ZFXSKIN_TYPE {
   bool bAlpha;         // needs alphablending enabled
   UINT nMaterial;      // ID to material in SkinManager
   UINT nTexture[8];    // ID to up to 8 textures in SkinManager
   } ZFXSKIN;

// generic color type
typedef struct ZFXCOLOR_TYPE {
   union {
      struct {
         float fR;
         float fG;
         float fB;
         float fA;
         };
      float c[4];
      };
   } ZFXCOLOR;

// material type
typedef struct ZFXMATERIAL_TYPE {
   ZFXCOLOR cDiffuse;   // RGBA diffuse light value
   ZFXCOLOR cAmbient;   // RGBA ambient light value
   ZFXCOLOR cSpecular;  // RGBA specular light value
   ZFXCOLOR cEmissive;  // RGBA emissive light value
   float    fPower;     // specular power
   } ZFXMATERIAL;


// texture type
typedef struct ZFXTEXTURE_TYPE {
   float     fAlpha;    // overall transparency value
   char     *chName;    // texture filename
   void     *pData;     // texture data
   ZFXCOLOR *pClrKeys;  // color key array
   DWORD     dwNum;     // number of color keys
   } ZFXTEXTURE;


// light type
typedef struct ZFXLIGHT_TYPE {
   ZFXLIGHTID Type;           // type of light
   ZFXCOLOR   cDiffuse;       // RGBA diffuse light value
   ZFXCOLOR   cSpecular;      // RGBA specular light value
   ZFXCOLOR   cAmbient;       // RGBA ambient light value
   ZFXVector  vcPosition;     // light position
   ZFXVector  vcDirection;    // light direction
   float      fRange;         // range of light
   float      fTheta;         // angle of spot light inner cone
   float      fPhi;           // angle of spot light outer cone
   float      fAttenuation0;  // change of intensity over distance
   float      fAttenuation1;  // change of intensity over distance
   } ZFXLIGHT;

typedef enum ZFXDATATYPE_TYPE {
   DAT_BOOL,          // boolean
   DAT_INT,           // integer
   DAT_FLOAT          // floating point
   } ZFXDATATYPE;

typedef enum ZFXSHADER_TYPE {
   SHT_VERTEX,
   SHT_PIXEL
   } ZFXSHADERTYPE;
/*----------------------------------------------------------------*/

#endif