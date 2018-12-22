
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************
//*
//*				Project:			IGNITION
//*
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************
//*
//*				Subproject:			Graphic-Subsystem	(Ign_graphic_sub)
//*				File:				ign_graphic_sub_shared.h
//*				Description:		Shared file with DLL and OBJ interfaces
//*
//********************************************************************************************************************************************************************
//*
//*				Author:				Kurt Pelzer
//*									Copyright (c) 2002 Kurt Pelzer
//*									All rights reserved.
//*
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************
//*
//*				Status:				In progress
//*
//********************************************************************************************************************************************************************
//*
//*				History:
//*
//*				09/01/02| 0.0.1		## Initial release															(kp) ##
//*				09/15/02| 0.1.1		## Changed architecture: Introducing object interfaces						(kp) ##
//*				09/27/02| 0.1.2		## Added shader support														(kp) ##
//*				10/23/02| 0.1.3		## Added DirectX9 support													(kp) ##
//*				11/11/02| 0.2.1		## Splitted into subsystem and several API-wrappers							(kp) ##
//*				11/18/02| 0.2.2		## Added the 'assistant interface'											(kp) ##
//*				11/20/02| 0.2.3		## Added the documentation texts in 'definitions & declarations sections'	(kp) ##
//*				11/22/02| 0.3.1		## Added the 'gauge interface'												(kp) ##
//*				11/27/02| 0.3.2		## Changed some type names													(kp) ##
//*				12/10/02| 0.4.1		## Changed interface parameter types (-> references)						(kp) ##
//*
//********************************************************************************************************************************************************************
//*
//*				TODOs:
//*						- Support the 'optimization method'
//*
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************




//------------- PREPROCESSOR -----------------------------------------------------------------------------------------------------------------------------------------

#ifndef			IGN_GRAPHIC_SUB_SHARED_H_INCLUDED
#define			IGN_GRAPHIC_SUB_SHARED_H_INCLUDED

#if				_MSC_VER > 1000
#pragma once
#ifdef			IGN_SWITCH_SHOW_HEADER
#pragma message ("---> ign_graphic_sub_shared.h\n")
#endif
#endif 




//------------- DEPENDENCIES -----------------------------------------------------------------------------------------------------------------------------------------

#include		<windows.h>

#ifndef			IGN_TYPES_SHARED_H_INCLUDED
  #include		"../../Types/ign_types_shared.h"
#endif

#ifndef			IGN_POINTERLIST_SHARED_H_INCLUDED
  #include		"../../Types/Lists/ign_pointerlist_shared.h"
#endif

#ifndef			IGN_VALUELIST_SHARED_H_INCLUDED
  #include		"../../Types/Lists/ign_valuelist_shared.h"
#endif

#ifndef			IGN_COLOR_SHARED_H_INCLUDED
  #include		"../../Types/Math/ign_color_shared.h"
#endif

#ifndef			IGN_MATRIX_SHARED_H_INCLUDED
  #include		"../../Types/Math/ign_matrix_shared.h"
#endif

#ifndef			IGN_QUATERNION_SHARED_H_INCLUDED
  #include		"../../Types/Math/ign_quaternion_shared.h"
#endif

#ifndef			IGN_VECTOR_SHARED_H_INCLUDED
  #include		"../../Types/Math/ign_vector_shared.h"
#endif

#ifdef			IGN_EXCEPTIONHANDLING
	#ifndef			IGN_EXCEPTIONS_H_INCLUDED
		#include		"../../Exceptions/ign_exceptions.h"
	#endif
#else
	#ifndef			IGN_EXCEPTIONSDUMMY_H_INCLUDED
		#include		"../../Exceptions/ign_exceptionsdummy.h"
	#endif
#endif




//------------- FORWARD DECLARATIONS ---------------------------------------------------------------------------------------------------------------------------------

struct			IGN_STRUCT_IFSubGraphic;
struct			IGN_STRUCT_GfxAssistMethods;
struct			IGN_STRUCT_GfxVersion;
struct			IGN_STRUCT_GfxModeDesc;
struct			IGN_STRUCT_GfxContextDesc;
struct			IGN_STRUCT_GfxGaugeValues;
struct			IGN_STRUCT_Viewport;
struct			IGN_STRUCT_StencilData;
struct			IGN_STRUCT_PlaneData;
struct			IGN_STRUCT_BoxData;
struct			IGN_STRUCT_CylinderData;
struct			IGN_STRUCT_PolygonData;
struct			IGN_STRUCT_SphereData;
struct			IGN_STRUCT_TeapotData;
struct			IGN_STRUCT_TextData;
struct			IGN_STRUCT_TorusData;
struct			IGN_STRUCT_4x4Matrix;
struct			IGN_STRUCT_KeyFrameItpData;
struct			IGN_STRUCT_3DKeyVector;
struct			IGN_STRUCT_3DKeyQuaternion;
struct			IGN_STRUCT_3DVector;
struct			IGN_STRUCT_3DQuaternion;
struct			IGN_STRUCT_DrawCallDesc;
struct			IGN_STRUCT_ColorValue;
struct			IGN_STRUCT_LightData;
struct			IGN_STRUCT_FogData;
struct			IGN_STRUCT_VertexBufDesc;
struct			IGN_STRUCT_IndexBufDesc;
struct			IGN_STRUCT_AlphaTestData;
struct			IGN_STRUCT_LayersDesc;
struct			IGN_STRUCT_DclElement;
struct			IGN_OBJIF_GfxInstance;
struct			IGN_OBJIF_GfxContext;
struct			IGN_OBJIF_GfxGauge;
struct			IGN_OBJIF_GfxMixer;
struct			IGN_OBJIF_GfxAssistant;
struct			IGN_OBJIF_GfxCamera;
struct			IGN_OBJIF_GfxVisual;
struct			IGN_OBJIF_GfxLight;
struct			IGN_OBJIF_GfxFog;
struct			IGN_OBJIF_GfxVertexData;
struct			IGN_OBJIF_GfxIndexData;
struct			IGN_OBJIF_GfxShader;
struct			IGN_OBJIF_GfxMaterial;
struct			IGN_OBJIF_GfxVertexProgDcl;
struct			IGN_OBJIF_GfxVertexProgram;
struct			IGN_OBJIF_GfxPixelProgram;
struct			IGN_OBJIF_GfxTextureBlock;
struct			IGN_OBJIF_GfxImageData;




//------------- DEFINITIONS ------------------------------------------------------------------------------------------------------------------------------------------

#define			IGNDECL_END()										{ 0xFF,0,IGN_ENUM_DECLTYPE_UNUSED,0,0,0 }

const IGNDWORD	IGN_MAX_ADAPTER_ID_STRING							= 512;							// Length of m_cDriver & m_cDescription in IGN_STRUCT_AdapterDesc
const IGNDWORD	IGN_MAX_VERTEXPROGRAMMREGS							= 256;							// Maximum number of vertex program registers
const IGNDWORD	IGN_MAX_PIXELPROGRAMMREGS							= 256;							// Maximum number of pixel program registers

const IGNDWORD	IGNFVF_RESERVED0									= 0x0001;						// Mask value for reserved bits in the flexible vertex format
const IGNDWORD	IGNFVF_POSITION_MASK								= 0x000E;						// Mask for position bits
const IGNDWORD	IGNFVF_XYZ											= 0x0002;						// Vertex format includes the position of an untransformed vertex
const IGNDWORD	IGNFVF_XYZRHW										= 0x0004;						// Vertex format includes the position of a transformed vertex
const IGNDWORD	IGNFVF_XYZB1										= 0x0006;						// Vertex format contains position data and one weighting value
const IGNDWORD	IGNFVF_XYZB2										= 0x0008;						// Vertex format contains position data and two weighting value
const IGNDWORD	IGNFVF_XYZB3										= 0x000a;						// Vertex format contains position data and three weighting value
const IGNDWORD	IGNFVF_XYZB4										= 0x000c;						// Vertex format contains position data and four weighting value
const IGNDWORD	IGNFVF_XYZB5										= 0x000e;						// Vertex format contains position data and five weighting value
const IGNDWORD	IGNFVF_NORMAL										= 0x0010;						// Vertex format includes a vertex normal vector
const IGNDWORD	IGNFVF_PSIZE										= 0x0020;						// Vertex format specified in point size
const IGNDWORD	IGNFVF_DIFFUSE										= 0x0040;						// Vertex format includes a diffuse color component
const IGNDWORD	IGNFVF_SPECULAR										= 0x0080;						// Vertex format includes a specular color component
const IGNDWORD	IGNFVF_TEXCOUNT_MASK								= 0x0f00;						// Mask value for texture flag bits
const IGNDWORD	IGNFVF_TEXCOUNT_SHIFT								= 0x0008;						// The number of bits by which to shift the number of a tex coords
const IGNDWORD	IGNFVF_TEX0											= 0x0000;						// Vertex format doesn't includes any texture coordinate sets
const IGNDWORD	IGNFVF_TEX1											= 0x0100;						// Vertex format includes one texture coordinate set
const IGNDWORD	IGNFVF_TEX2											= 0x0200;						// Vertex format includes two texture coordinate sets
const IGNDWORD	IGNFVF_TEX3											= 0x0300;						// Vertex format includes three texture coordinate sets
const IGNDWORD	IGNFVF_TEX4											= 0x0400;						// Vertex format includes four texture coordinate sets
const IGNDWORD	IGNFVF_TEX5											= 0x0500;						// Vertex format includes five texture coordinate sets
const IGNDWORD	IGNFVF_TEX6											= 0x0600;						// Vertex format includes six texture coordinate sets
const IGNDWORD	IGNFVF_TEX7											= 0x0700;						// Vertex format includes seven texture coordinate sets
const IGNDWORD	IGNFVF_TEX8											= 0x0800;						// Vertex format includes eight texture coordinate sets
const IGNDWORD	IGNFVF_TEXCOORDSIZE1_0								= 0x00030000;					// The first set of texture coordinates includes one element
const IGNDWORD	IGNFVF_TEXCOORDSIZE2_0								= 0x00000000;					// The first set of texture coordinates includes two elements
const IGNDWORD	IGNFVF_TEXCOORDSIZE3_0								= 0x00010000;					// The first set of texture coordinates includes three elements
const IGNDWORD	IGNFVF_TEXCOORDSIZE4_0								= 0x00020000;					// The first set of texture coordinates includes four elements
const IGNDWORD	IGNFVF_TEXCOORDSIZE1_1								= 0x000C0000;					// The second set of texture coordinates includes one element
const IGNDWORD	IGNFVF_TEXCOORDSIZE2_1								= 0x00000000;					// The second set of texture coordinates includes two elements
const IGNDWORD	IGNFVF_TEXCOORDSIZE3_1								= 0x00040000;					// The second set of texture coordinates includes three elements
const IGNDWORD	IGNFVF_TEXCOORDSIZE4_1								= 0x00080000;					// The second set of texture coordinates includes four elements
const IGNDWORD	IGNFVF_TEXCOORDSIZE1_2								= 0x00300000;					// The third set of texture coordinates includes one element
const IGNDWORD	IGNFVF_TEXCOORDSIZE2_2								= 0x00000000;					// The third set of texture coordinates includes two elements
const IGNDWORD	IGNFVF_TEXCOORDSIZE3_2								= 0x00100000;					// The third set of texture coordinates includes three elements
const IGNDWORD	IGNFVF_TEXCOORDSIZE4_2								= 0x00200000;					// The third set of texture coordinates includes four elements
const IGNDWORD	IGNFVF_TEXCOORDSIZE1_3								= 0x00C00000;					// The fourth set of texture coordinates includes one element
const IGNDWORD	IGNFVF_TEXCOORDSIZE2_3								= 0x00000000;					// The fourth set of texture coordinates includes two elements
const IGNDWORD	IGNFVF_TEXCOORDSIZE3_3								= 0x00400000;					// The fourth set of texture coordinates includes three elements
const IGNDWORD	IGNFVF_TEXCOORDSIZE4_3								= 0x00800000;					// The fourth set of texture coordinates includes four elements
const IGNDWORD	IGNFVF_TEXCOORDSIZE1_4								= 0x03000000;					// The fifth set of texture coordinates includes one element
const IGNDWORD	IGNFVF_TEXCOORDSIZE2_4								= 0x00000000;					// The fifth set of texture coordinates includes two elements
const IGNDWORD	IGNFVF_TEXCOORDSIZE3_4								= 0x01000000;					// The fifth set of texture coordinates includes three elements
const IGNDWORD	IGNFVF_TEXCOORDSIZE4_4								= 0x02000000;					// The fifth set of texture coordinates includes four elements
const IGNDWORD	IGNFVF_TEXCOORDSIZE1_5								= 0x0C000000;					// The sixth set of texture coordinates includes one element
const IGNDWORD	IGNFVF_TEXCOORDSIZE2_5								= 0x00000000;					// The sixth set of texture coordinates includes two elements
const IGNDWORD	IGNFVF_TEXCOORDSIZE3_5								= 0x04000000;					// The sixth set of texture coordinates includes three elements
const IGNDWORD	IGNFVF_TEXCOORDSIZE4_5								= 0x08000000;					// The sixth set of texture coordinates includes four elements
const IGNDWORD	IGNFVF_TEXCOORDSIZE1_6								= 0x30000000;					// The seventh set of texture coordinates includes one element
const IGNDWORD	IGNFVF_TEXCOORDSIZE2_6								= 0x00000000;					// The seventh set of texture coordinates includes two elements
const IGNDWORD	IGNFVF_TEXCOORDSIZE3_6								= 0x10000000;					// The seventh set of texture coordinates includes three elements
const IGNDWORD	IGNFVF_TEXCOORDSIZE4_6								= 0x20000000;					// The seventh set of texture coordinates includes four elements
const IGNDWORD	IGNFVF_TEXCOORDSIZE1_7								= 0xC0000000;					// The eighth set of texture coordinates includes one element
const IGNDWORD	IGNFVF_TEXCOORDSIZE2_7								= 0x00000000;					// The eighth set of texture coordinates includes two elements
const IGNDWORD	IGNFVF_TEXCOORDSIZE3_7								= 0x40000000;					// The eighth set of texture coordinates includes three elements
const IGNDWORD	IGNFVF_TEXCOORDSIZE4_7								= 0x80000000;					// The eighth set of texture coordinates includes four elements
const IGNDWORD	IGNFVF_LASTBETA_UBYTE4								= 0x1000;						// The last beta field in the position data will be of type UBYTE4
const IGNDWORD	IGNFVF_RESERVED2									= 0xE000;						// Mask value for reserved bits in the flexible vertex format

const IGNDWORD	IGNLOCKFLAG_READONLY								= 0x00000010;					// The application will not write to the buffer
const IGNDWORD	IGNLOCKFLAG_DISCARD									= 0x00002000;					// The application overwrites the locked region 
const IGNDWORD	IGNLOCKFLAG_NOOVERWRITE								= 0x00001000;					// The application promises not to overwrite any data
const IGNDWORD	IGNLOCKFLAG_NOSYSLOCK								= 0x00000800;					// Causes the system-wide critical section not to be held
const IGNDWORD	IGNLOCKFLAG_NO_DIRTY_UPDATE							= 0x00008000;					// Prevents any changes to the dirty state of the resource

const IGNDWORD	IGNUSAGE_RENDERTARGET								= 0x00000001;					// The resource will be a render target
const IGNDWORD	IGNUSAGE_DEPTHSTENCIL								= 0x00000002;					// The resource will be a depth stencil buffer
const IGNDWORD	IGNUSAGE_WRITEONLY									= 0x00000008;					// The application writes only to the vertex buffer
const IGNDWORD	IGNUSAGE_SOFTWAREPROCESSING							= 0x00000010;					// Vertex processing is done in software
const IGNDWORD	IGNUSAGE_DONOTCLIP									= 0x00000020;					// The vertex buffer content will never require clipping
const IGNDWORD	IGNUSAGE_POINTS										= 0x00000040;					// The vertex buffer is to be used for drawing points
const IGNDWORD	IGNUSAGE_RTPATCHES									= 0x00000080;					// The vertex buffer is to be used for drawing high-order prims
const IGNDWORD	IGNUSAGE_NPATCHES									= 0x00000100;					// The vertex buffer is to be used for drawing N-patches
const IGNDWORD	IGNUSAGE_DYNAMIC									= 0x00000200;					// The vertex buffer requires dynamic memory use

const IGNDWORD	IGNTEXCOORDTRAFO_DISABLE							= 0;							// Texture coordinates are passed directly to the rasterizer
const IGNDWORD	IGNTEXCOORDTRAFO_COUNT1								= 1;							// The rasterizer should expect 1-D texture coordinates
const IGNDWORD	IGNTEXCOORDTRAFO_COUNT2								= 2;							// The rasterizer should expect 2-D texture coordinates
const IGNDWORD	IGNTEXCOORDTRAFO_COUNT3								= 3;							// The rasterizer should expect 3-D texture coordinates
const IGNDWORD	IGNTEXCOORDTRAFO_COUNT4								= 4;							// The rasterizer should expect 4-D texture coordinates
const IGNDWORD	IGNTEXCOORDTRAFO_PROJECTED							= 256;							// All tex coords are divided by the last element




enum			IGN_ENUM_GFXINSTANCETYPE
{
	IGN_ENUM_GFXINSTANCETYPE_NONE				= 0,												// Not any instance
	IGN_ENUM_GFXINSTANCETYPE_DUMMY				= 1,												// A simple dummy instance (for performance measuring)
	IGN_ENUM_GFXINSTANCETYPE_DIRECTX80			= 2,												// A DirectX 8.0 instance
	IGN_ENUM_GFXINSTANCETYPE_DIRECTX81			= 3,												// A DirectX 8.1 instance
	IGN_ENUM_GFXINSTANCETYPE_DIRECTX90			= 4,												// A DirectX 9.0 instance
	IGN_ENUM_GFXINSTANCETYPE_OPENGL20			= 5,												// A OpenGL 2.0 instance
	IGN_ENUM_GFXINSTANCETYPE_OPENGL14			= 6,												// A OpenGL 1.4 instance
	IGN_ENUM_GFXINSTANCETYPE_FORCEDWORD			= 0x7fffffff
};

enum			IGN_ENUM_PIXELFORMAT
{
	IGN_ENUM_PIXELFORMAT_UNKNOWN				=  0,												// Unknown pixel format
	IGN_ENUM_PIXELFORMAT_R8G8B8					= 20,												// 24-bit RGB pixel format with 8 bits per channel
	IGN_ENUM_PIXELFORMAT_A8R8G8B8				= 21,												// 32-bit ARGB pixel format with alpha, using 8 bits per channel
	IGN_ENUM_PIXELFORMAT_X8R8G8B8				= 22,												// 32-bit RGB pixel format, using 8 bits per channel
	IGN_ENUM_PIXELFORMAT_R5G6B5					= 23,												// 16-bit RGB pixel format using 5-6-5 bits per channel
	IGN_ENUM_PIXELFORMAT_X1R5G5B5				= 24,												// 16-bit pixel format where 5 bits are reserved for each color
	IGN_ENUM_PIXELFORMAT_A1R5G5B5				= 25,												// 16-bit pixel format, using 5 bits per color and 1 bit for alpha
	IGN_ENUM_PIXELFORMAT_A4R4G4B4				= 26,												// 16-bit ARGB pixel format with 4 bits for each channel
	IGN_ENUM_PIXELFORMAT_R3G3B2					= 27,												// 8-bit RGB texture format, using 3-3-2 bits per channel
	IGN_ENUM_PIXELFORMAT_A8						= 28,												// 8-bit alpha only
	IGN_ENUM_PIXELFORMAT_A8R3G3B2				= 29,												// 16-bit ARGB texture format, using 8-3-3-2 bits for A-R-G-B
	IGN_ENUM_PIXELFORMAT_X4R4G4B4				= 30,												// 16-bit RGB pixel format, using 4 bits for each color
	IGN_ENUM_PIXELFORMAT_A2B10G10R10			= 31,												// 32-bit pixel format, using 10 bits per color and two for alpha
	IGN_ENUM_PIXELFORMAT_G16R16					= 34,												// 32-bit pixel format, using 16 bits each for green and red
	IGN_ENUM_PIXELFORMAT_A8P8					= 40,												// 8-bit color indexed with 8 bits of alpha
	IGN_ENUM_PIXELFORMAT_P8						= 41,												// 8-bit color indexed
	IGN_ENUM_PIXELFORMAT_L8						= 50,												// 8-bit luminance only
	IGN_ENUM_PIXELFORMAT_A8L8					= 51,												// 16-bit using 8 bits each for alpha and luminance
	IGN_ENUM_PIXELFORMAT_A4L4					= 52,												// 8-bit using 4 bits each for alpha and luminance
	IGN_ENUM_PIXELFORMAT_V8U8					= 60,												// 16-bit bump-map format using 8 bits each for u and v data
	IGN_ENUM_PIXELFORMAT_L6V5U5					= 61,												// 16-bit bump-map format with luminance
	IGN_ENUM_PIXELFORMAT_X8L8V8U8				= 62,												// 32-bit bump-map format with luminance, using 8 bits per channel
	IGN_ENUM_PIXELFORMAT_Q8W8V8U8				= 63,												// 32-bit bump-map format using 8 bits for each channel
	IGN_ENUM_PIXELFORMAT_V16U16					= 64,												// 32-bit bump-map format using 16 bits for each channel
	IGN_ENUM_PIXELFORMAT_UYVY					= MAKEFOURCC('U', 'Y', 'V', 'Y'),					// UYVY format (PC98 compliance)
	IGN_ENUM_PIXELFORMAT_YUY2					= MAKEFOURCC('Y', 'U', 'Y', '2'),					// YUY2 format (PC98 compliance)
	IGN_ENUM_PIXELFORMAT_DXT1					= MAKEFOURCC('D', 'X', 'T', '1'),					// DXT1 compression texture format (S3TC)
	IGN_ENUM_PIXELFORMAT_DXT2					= MAKEFOURCC('D', 'X', 'T', '2'),					// DXT2 compression texture format (S3TC)
	IGN_ENUM_PIXELFORMAT_DXT3					= MAKEFOURCC('D', 'X', 'T', '3'),					// DXT3 compression texture format (S3TC)
	IGN_ENUM_PIXELFORMAT_DXT4					= MAKEFOURCC('D', 'X', 'T', '4'),					// DXT4 compression texture format (S3TC)
	IGN_ENUM_PIXELFORMAT_DXT5					= MAKEFOURCC('D', 'X', 'T', '5'),					// DXT5 compression texture format (S3TC)
	IGN_ENUM_PIXELFORMAT_FORCEDWORD				= 0x7fffffff
};

enum			IGN_ENUM_DEPTHFORMAT
{
	IGN_ENUM_DEPTHFORMAT_D16_LOCKABLE			= 70,												// 16-bit z-buffer bit depth (application-lockable)
	IGN_ENUM_DEPTHFORMAT_D32					= 71,												// 32-bit z-buffer bit depth
	IGN_ENUM_DEPTHFORMAT_D15S1					= 73,												// 16-bit z-buffer bit depth with 1 bit for stencil
	IGN_ENUM_DEPTHFORMAT_D24S8					= 75,												// 32-bit z-buffer bit depth with 8 bits for the stencil
	IGN_ENUM_DEPTHFORMAT_D24X8					= 77,												// 32-bit z-buffer bit depth using 24 bits for the depth channel
	IGN_ENUM_DEPTHFORMAT_D24X4S4				= 79,												// 32-bit z-buffer bit depth with 4 bits for the stencil
	IGN_ENUM_DEPTHFORMAT_D16					= 80,												// 16-bit z-buffer bit depth
	IGN_ENUM_DEPTHFORMAT_FORCEDWORD				= 0x7fffffff
};

enum			IGN_ENUM_INDEXFORMAT
{
	IGN_ENUM_INDEXFORMAT_INDEX16				= 101,												// 16-bit index buffer bit depth
	IGN_ENUM_INDEXFORMAT_INDEX32				= 102,												// 32-bit index buffer bit depth
	IGN_ENUM_INDEXFORMAT_FORCEDWORD				= 0x7fffffff
};

enum			IGN_ENUM_MULTISAMPLE
{
	IGN_ENUM_MULTISAMPLE_NONE					= 0,												// No level of full-scene multisampling is available
	IGN_ENUM_MULTISAMPLE_2_SAMPLES				= 2,												// 2 levels of full-scene multisampling available
	IGN_ENUM_MULTISAMPLE_3_SAMPLES				= 3,												// 3 levels of full-scene multisampling available
	IGN_ENUM_MULTISAMPLE_4_SAMPLES				= 4,												// 4 levels of full-scene multisampling available
	IGN_ENUM_MULTISAMPLE_5_SAMPLES				= 5,												// 5 levels of full-scene multisampling available
	IGN_ENUM_MULTISAMPLE_6_SAMPLES				= 6,												// 6 levels of full-scene multisampling available
	IGN_ENUM_MULTISAMPLE_7_SAMPLES				= 7,												// 7 levels of full-scene multisampling available
	IGN_ENUM_MULTISAMPLE_8_SAMPLES				= 8,												// 8 levels of full-scene multisampling available
	IGN_ENUM_MULTISAMPLE_9_SAMPLES				= 9,												// 9 levels of full-scene multisampling available
	IGN_ENUM_MULTISAMPLE_10_SAMPLES				= 10,												// 10 levels of full-scene multisampling available
	IGN_ENUM_MULTISAMPLE_11_SAMPLES				= 11,												// 11 levels of full-scene multisampling available
	IGN_ENUM_MULTISAMPLE_12_SAMPLES				= 12,												// 12 levels of full-scene multisampling available
	IGN_ENUM_MULTISAMPLE_13_SAMPLES				= 13,												// 13 levels of full-scene multisampling available
	IGN_ENUM_MULTISAMPLE_14_SAMPLES				= 14,												// 14 levels of full-scene multisampling available
	IGN_ENUM_MULTISAMPLE_15_SAMPLES				= 15,												// 15 levels of full-scene multisampling available
	IGN_ENUM_MULTISAMPLE_16_SAMPLES				= 16,												// 16 levels of full-scene multisampling available
	IGN_ENUM_MULTISAMPLE_FORCEDWORD				= 0x7fffffff
};

enum			IGN_ENUM_GAUGEMODUS
{
	IGN_ENUM_GAUGEMODUS_DISABLED				= 0,												// Disables statistical measuring
	IGN_ENUM_GAUGEMODUS_SIMPLE					= 1,												// Enables simple statistical measuring
	IGN_ENUM_GAUGEMODUS_STANDARD				= 2,												// Enables standard statistical measuring
	IGN_ENUM_GAUGEMODUS_ADVANCED				= 3,												// Enables advanced statistical measuring
	IGN_ENUM_GAUGEMODUS_FORCEDWORD				= 0x7fffffff
};

enum			IGN_ENUM_GFXCONTEXTCREATE
{
	IGN_ENUM_GFXCONTEXTCREATE_FPUPRESERVE			= 0x00000002L,									// The app needs double-precision FPU or FPU exceptions enabled
	IGN_ENUM_GFXCONTEXTCREATE_MULTITHREADED			= 0x00000004L,									// The app requests context to be multithread safe
	IGN_ENUM_GFXCONTEXTCREATE_PUREDEVICE			= 0x00000010L,									// Does not provide any emulation services for vertex processing
	IGN_ENUM_GFXCONTEXTCREATE_SWVERTEXPROCESSING	= 0x00000020L,									// Specifies software vertex processing
	IGN_ENUM_GFXCONTEXTCREATE_HWVERTEXPROCESSING	= 0x00000040L,									// Specifies hardware vertex processing
	IGN_ENUM_GFXCONTEXTCREATE_MIXVERTEXPROCESSING	= 0x00000080L,									// Specifies mixed (both software and hardware) vertex processing
	IGN_ENUM_GFXCONTEXTCREATE_NODRIVERMANAGEMENT	= 0x00000100L,									// Context should manage resources instead of the driver
	IGN_ENUM_GFXCONTEXTCREATE_FORCEDWORD			= 0x7fffffff
};

enum			IGN_ENUM_ITLOPTIMIZATION
{
	IGN_ENUM_ITLOPTIMIZATION_NONE				= 0,												// Not any optimization
	IGN_ENUM_ITLOPTIMIZATION_SUB				= 1,												// Optimizes the subsystem level
	IGN_ENUM_ITLOPTIMIZATION_API				= 2,												// Optimizes the API-wrapper level
	IGN_ENUM_ITLOPTIMIZATION_SUBANDAPI			= 3,												// Optimizes both,the subsystem and the API-wrapper levels
	IGN_ENUM_ITLOPTIMIZATION_FORCEDWORD			= 0x7fffffff
};

enum			IGN_ENUM_VIEWPORTCLEAR
{
	IGN_ENUM_VIEWPORTCLEAR_COLOR				= 0x00000001L,										// Clears the color buffer
	IGN_ENUM_VIEWPORTCLEAR_DEPTH				= 0x00000002L,										// Clears the depth buffer
	IGN_ENUM_VIEWPORTCLEAR_STENCIL				= 0x00000004L,										// Clears the stencil buffer
	IGN_ENUM_VIEWPORTCLEAR_FORCEDWORD			= 0x7fffffff
};

enum			IGN_ENUM_MASTERRENDERMODE
{
	IGN_ENUM_MASTERRENDERMODE_VISUAL			= 0,												// Visual is responsible for render mode
	IGN_ENUM_MASTERRENDERMODE_POINT				= 1,												// Fills points (mm) [mm == 'master mode']
	IGN_ENUM_MASTERRENDERMODE_WIREFRAME			= 2,												// Fills wireframes (mm)
	IGN_ENUM_MASTERRENDERMODE_SOLID				= 3,												// Fills solids (mm)
	IGN_ENUM_MASTERRENDERMODE_FORCEDWORD		= 0x7fffffff
};

enum			IGN_ENUM_MASTERSHADEMODE
{
	IGN_ENUM_MASTERSHADEMODE_VISUAL				= 0,												// Visual is responsible for shade mode
	IGN_ENUM_MASTERSHADEMODE_FLAT				= 1,												// Flat shading mode (mm)
	IGN_ENUM_MASTERSHADEMODE_GOURAUD			= 2,												// Gouraud shading mode (mm)
	IGN_ENUM_MASTERSHADEMODE_FORCEDWORD			= 0x7fffffff
};

enum			IGN_ENUM_MASTERCULLINGMODE
{
	IGN_ENUM_MASTERCULLINGMODE_VISUAL			= 0,												// Visual is responsible for culling mode
	IGN_ENUM_MASTERCULLINGMODE_NONE				= 1,												// Does not cull back faces (mm)
	IGN_ENUM_MASTERCULLINGMODE_CW				= 2,												// Culls back faces with clockwise vertices (mm)
	IGN_ENUM_MASTERCULLINGMODE_CCW				= 3,												// Culls back faces with counterclockwise vertices (mm)
	IGN_ENUM_MASTERCULLINGMODE_FORCEDWORD		= 0x7fffffff
};

enum			IGN_ENUM_MASTERCMPFUNC
{
	IGN_ENUM_MASTERCMPFUNC_VISUAL				= 0,												// Visual is responsible for depth testing compare function
	IGN_ENUM_MASTERCMPFUNC_NEVER				= 1,												// Always fails the test (mm)
	IGN_ENUM_MASTERCMPFUNC_LESS					= 2,												// Succeeds if the new value is less than the old (mm)
	IGN_ENUM_MASTERCMPFUNC_EQUAL				= 3,												// Succeeds if the new value equals the old (mm)
	IGN_ENUM_MASTERCMPFUNC_LESSEQUAL			= 4,												// Succeeds if the new value is less than or equal the old (mm)
	IGN_ENUM_MASTERCMPFUNC_GREATER				= 5,												// Succeeds if the new value is greater than the old (mm)
	IGN_ENUM_MASTERCMPFUNC_NOTEQUAL				= 6,												// Succeeds if the new value does not equal the old (mm)
	IGN_ENUM_MASTERCMPFUNC_GREATEREQUAL			= 7,												// Succeeds if the new value is greater than or equal the old (mm)
	IGN_ENUM_MASTERCMPFUNC_ALWAYS				= 8,												// Succeeds always (mm)
	IGN_ENUM_MASTERCMPFUNC_FORCEDWORD			= 0x7fffffff
};

enum			IGN_ENUM_MASTERWRITE
{
	IGN_ENUM_MASTERWRITE_VISUAL					= 0,												// Visual is responsible for write mode
	IGN_ENUM_MASTERWRITE_DISABLE				= 1,												// Disables depth writing (mm)
	IGN_ENUM_MASTERWRITE_ENABLE					= 2,												// Enables depth writing (mm)
	IGN_ENUM_MASTERWRITE_FORCEDWORD				= 0x7fffffff
};

enum			IGN_ENUM_MASTERSTENCIL
{
	IGN_ENUM_MASTERSTENCIL_VISUAL				= 0,												// Visual is responsible for stencil mode
	IGN_ENUM_MASTERSTENCIL_DISABLE				= 1,												// Disables stencil testing (mm)
	IGN_ENUM_MASTERSTENCIL_ENABLE				= 2,												// Enables stencil testing (mm)
	IGN_ENUM_MASTERSTENCIL_FORCEDWORD			= 0x7fffffff
};

enum			IGN_ENUM_MASTERBLENDING
{
	IGN_ENUM_MASTERBLENDING_MATERIAL			= 0,												// Material is responsible for blending mode
	IGN_ENUM_MASTERBLENDING_ZERO				= 1,												// Blend factor is (0, 0, 0, 0) (mm)
	IGN_ENUM_MASTERBLENDING_ONE					= 2,												// Blend factor is (1, 1, 1, 1) (mm)
	IGN_ENUM_MASTERBLENDING_SRCCOLOR			= 3,												// Blend factor is (Rs,Gs,Bs,As) (mm)
	IGN_ENUM_MASTERBLENDING_INVSRCCOLOR			= 4,												// Blend factor is (1 - Rs, 1 - Gs, 1 - Bs, 1 - As) (mm)
	IGN_ENUM_MASTERBLENDING_SRCALPHA			= 5,												// Blend factor is (As, As, As, As) (mm)
	IGN_ENUM_MASTERBLENDING_INVSRCALPHA			= 6,												// Blend factor is ( 1 - As, 1 - As, 1 - As, 1 - As) (mm)
	IGN_ENUM_MASTERBLENDING_DESTALPHA			= 7,												// Blend factor is (Ad, Ad, Ad, Ad) (mm)
	IGN_ENUM_MASTERBLENDING_INVDESTALPHA		= 8,												// Blend factor is (1 - Ad, 1 - Ad, 1 - Ad, 1 - Ad) (mm)
	IGN_ENUM_MASTERBLENDING_DESTCOLOR			= 9,												// Blend factor is (Rd, Gd, Bd, Ad) (mm)
	IGN_ENUM_MASTERBLENDING_INVDESTCOLOR		= 10,												// Blend factor is (1 - Rd, 1 - Gd, 1 - Bd, 1 - Ad) (mm)
	IGN_ENUM_MASTERBLENDING_SRCALPHASAT			= 11,												// Blend factor is (f, f, f, 1); f = min(A, 1 - Ad) (mm)
	IGN_ENUM_MASTERBLENDING_FORCEDWORD			= 0x7fffffff
};

enum			IGN_ENUM_MASTERALPHATEST
{
	IGN_ENUM_MASTERALPHATEST_MATERIAL			= 0,												// Material is responsible for alpha testing mode
	IGN_ENUM_MASTERALPHATEST_DISABLE			= 1,												// Disables alpha testing (mm)
	IGN_ENUM_MASTERALPHATEST_ENABLE				= 2,												// Enables alpha testing (mm)
	IGN_ENUM_MASTERALPHATEST_FORCEDWORD			= 0x7fffffff
};

enum			IGN_ENUM_STENCILOP
{
	IGN_ENUM_STENCILOP_KEEP						= 1,												// Does not update the entry in the stencil buffer
	IGN_ENUM_STENCILOP_ZERO						= 2,												// Sets the stencil-buffer entry to 0
	IGN_ENUM_STENCILOP_REPLACE					= 3,												// Replaces the stencil-buffer entry with reference value
	IGN_ENUM_STENCILOP_INCRSAT					= 4,												// Increments the stencil-buffer entry with clamping to max
	IGN_ENUM_STENCILOP_DECRSAT					= 5,												// Decrements the stencil-buffer entry, clamping to zero
	IGN_ENUM_STENCILOP_INVERT					= 6,												// Inverts the bits in the stencil-buffer entry
	IGN_ENUM_STENCILOP_INCR						= 7,												// Increments the stencil-buffer entry, wrapping to zero
	IGN_ENUM_STENCILOP_DECR						= 8,												// Decrements the stencil-buffer entry, wrapping to the max
	IGN_ENUM_STENCILOP_FORCEDWORD				= 0x7fffffff
};

enum			IGN_ENUM_NORMALMAPGEN
{
	IGN_ENUM_NORMALMAPGEN_DEFAULT				= 0,												// Default behaviour
	IGN_ENUM_NORMALMAPGEN_MIRRORU				= (1 << 16),										// Off-Edge pixels of the texture on the u-axis should be mirrored
	IGN_ENUM_NORMALMAPGEN_MIRRORV				= (2 << 16),										// Off-Edge pixels of the texture on the v-axis should be mirrored
	IGN_ENUM_NORMALMAPGEN_MIRROR				= (3 << 16),										// IGN_ENUM_NORMALMAPGEN_MIRRORU | IGN_ENUM_NORMALMAPGEN_MIRRORV
	IGN_ENUM_NORMALMAPGEN_INVERTSIGN			= (8 << 16),										// Inverts the direction of each normal
	IGN_ENUM_NORMALMAPGEN_COMPUTEOCCLUSION		= (16 << 16),										// Computes the per-pixel occlusion and encodes it into the alpha
	IGN_ENUM_NORMALMAPGEN_FORCEDWORD			= 0x7fffffff
};

enum			IGN_ENUM_CHANNEL
{
	IGN_ENUM_CHANNEL_RED						= (1 << 0),											// Indicates the red channel should be used
	IGN_ENUM_CHANNEL_BLUE						= (1 << 1),											// Indicates the blue channel should be used
	IGN_ENUM_CHANNEL_GREEN						= (1 << 2),											// Indicates the green channel should be used
	IGN_ENUM_CHANNEL_ALPHA						= (1 << 3),											// Indicates the alpha channel should be used
	IGN_ENUM_CHANNEL_LUMINANCE					= (1 << 4),											// Indicates the overall luminance should be used
	IGN_ENUM_CHANNEL_FORCEDWORD					= 0x7fffffff
};

enum			IGN_ENUM_RENDERMODE
{
	IGN_ENUM_RENDERMODE_POINT					= 1,												// Fills points
	IGN_ENUM_RENDERMODE_WIREFRAME				= 2,												// Fills wireframes
	IGN_ENUM_RENDERMODE_SOLID					= 3,												// Fills solids
	IGN_ENUM_RENDERMODE_FORCEDWORD				= 0x7fffffff
};

enum			IGN_ENUM_SHADEMODE
{
	IGN_ENUM_SHADEMODE_FLAT						= 1,												// Flat shading mode
	IGN_ENUM_SHADEMODE_GOURAUD					= 2,												// Gouraud shading mode
	IGN_ENUM_SHADEMODE_FORCEDWORD				= 0x7fffffff
};

enum			IGN_ENUM_CULLINGMODE
{
	IGN_ENUM_CULLINGMODE_NONE					= 1,												// Does not cull back faces
	IGN_ENUM_CULLINGMODE_CW						= 2,												// Culls back faces with clockwise vertices
	IGN_ENUM_CULLINGMODE_CCW					= 3,												// Culls back faces with counterclockwise vertices
	IGN_ENUM_CULLINGMODE_FORCEDWORD				= 0x7fffffff
};

enum			IGN_ENUM_CMPFUNC
{
	IGN_ENUM_CMPFUNC_NEVER						= 1,												// Always fails the test
	IGN_ENUM_CMPFUNC_LESS						= 2,												// Succeeds if the new value is less than the old
	IGN_ENUM_CMPFUNC_EQUAL						= 3,												// Succeeds if the new value equals the old
	IGN_ENUM_CMPFUNC_LESSEQUAL					= 4,												// Succeeds if the new value is less than or equal the old
	IGN_ENUM_CMPFUNC_GREATER					= 5,												// Succeeds if the new value is greater than the old
	IGN_ENUM_CMPFUNC_NOTEQUAL					= 6,												// Succeeds if the new value does not equal the old
	IGN_ENUM_CMPFUNC_GREATEREQUAL				= 7,												// Succeeds if the new value is greater than or equal the old
	IGN_ENUM_CMPFUNC_ALWAYS						= 8,												// Succeeds always
	IGN_ENUM_CMPFUNC_FORCEDWORD					= 0x7fffffff
};

enum			IGN_ENUM_WRITE
{
	IGN_ENUM_WRITE_DISABLE						= 1,												// Disables depth writing
	IGN_ENUM_WRITE_ENABLE						= 2,												// Enables depth writing
	IGN_ENUM_WRITE_FORCEDWORD					= 0x7fffffff
};

enum			IGN_ENUM_STENCIL
{
	IGN_ENUM_STENCIL_DISABLE					= 1,												// Disables stencil testing
	IGN_ENUM_STENCIL_ENABLE						= 2,												// Enables stencil testing
	IGN_ENUM_STENCIL_FORCEDWORD					= 0x7fffffff
};

enum			IGN_ENUM_PRIMITIVETYPE 
{
	IGN_ENUM_PRIMITIVETYPE_POINTLIST			= 1,												// Renders the vertices as a collection of isolated points
	IGN_ENUM_PRIMITIVETYPE_LINELIST				= 2,												// Renders the vertices as a list of isolated straight line segs
	IGN_ENUM_PRIMITIVETYPE_LINESTRIP			= 3,												// Renders the vertices as a single polyline
	IGN_ENUM_PRIMITIVETYPE_TRIANGLELIST			= 4,												// Renders the vertices as a sequence of isolated triangles
	IGN_ENUM_PRIMITIVETYPE_TRIANGLESTRIP		= 5,												// Renders the vertices as a triangle strip
	IGN_ENUM_PRIMITIVETYPE_TRIANGLEFAN			= 6,												// Renders the vertices as a triangle fan
	IGN_ENUM_PRIMITIVETYPE_FORCEDWORD			= 0x7fffffff
};

enum			IGN_ENUM_LIGHTTYPE
{
	IGN_ENUM_LIGHTTYPE_POINT					= 1,												// Light is a point source
	IGN_ENUM_LIGHTTYPE_SPOT						= 2,												// Light is a spotlight source
	IGN_ENUM_LIGHTTYPE_DIRECTIONAL				= 3,												// Light is a directional source
	IGN_ENUM_LIGHTTYPE_FORCEDWORD				= 0x7fffffff
};

enum			IGN_ENUM_FOGTYPE
{
	IGN_ENUM_FOGTYPE_VERTEX						= 1,												// Applies fog calculations at each vertex in a polygon
	IGN_ENUM_FOGTYPE_PIXEL						= 2,												// Applies fog calculations on a per-pixel basis
	IGN_ENUM_FOGTYPE_FORCEDWORD					= 0x7fffffff
};

enum			IGN_ENUM_FOGMODE
{
	IGN_ENUM_FOGMODE_NONE						= 0,												// No fog effect
	IGN_ENUM_FOGMODE_EXP						= 1,												// Fog effect intensifies exponentially
	IGN_ENUM_FOGMODE_EXP2						= 2,												// Fog effect intensifies exponentially with the distance^2
	IGN_ENUM_FOGMODE_LINEAR						= 3,												// Fog effect intensifies linearly between the start and end
	IGN_ENUM_FOGMODE_FORCEDWORD					= 0x7fffffff
};

enum			IGN_ENUM_BLENDING
{
	IGN_ENUM_BLENDING_ZERO						= 1,												// Blend factor is (0, 0, 0, 0)
	IGN_ENUM_BLENDING_ONE						= 2,												// Blend factor is (1, 1, 1, 1)
	IGN_ENUM_BLENDING_SRCCOLOR					= 3,												// Blend factor is (Rs,Gs,Bs,As)
	IGN_ENUM_BLENDING_INVSRCCOLOR				= 4,												// Blend factor is (1 - Rs, 1 - Gs, 1 - Bs, 1 - As) 
	IGN_ENUM_BLENDING_SRCALPHA					= 5,												// Blend factor is (As, As, As, As)
	IGN_ENUM_BLENDING_INVSRCALPHA				= 6,												// Blend factor is ( 1 - As, 1 - As, 1 - As, 1 - As)
	IGN_ENUM_BLENDING_DESTALPHA					= 7,												// Blend factor is (Ad, Ad, Ad, Ad)
	IGN_ENUM_BLENDING_INVDESTALPHA				= 8,												// Blend factor is (1 - Ad, 1 - Ad, 1 - Ad, 1 - Ad) 
	IGN_ENUM_BLENDING_DESTCOLOR					= 9,												// Blend factor is (Rd, Gd, Bd, Ad)
	IGN_ENUM_BLENDING_INVDESTCOLOR				= 10,												// Blend factor is (1 - Rd, 1 - Gd, 1 - Bd, 1 - Ad) 
	IGN_ENUM_BLENDING_SRCALPHASAT				= 11,												// Blend factor is (f, f, f, 1); f = min(A, 1 - Ad) 
	IGN_ENUM_BLENDING_FORCEDWORD				= 0x7fffffff
};

enum			IGN_ENUM_ALPHATEST
{
	IGN_ENUM_ALPHATEST_DISABLE					= 1,												// Disables alpha testing
	IGN_ENUM_ALPHATEST_ENABLE					= 2,												// Enables alpha testing
	IGN_ENUM_ALPHATEST_FORCEDWORD				= 0x7fffffff
};

enum			IGN_ENUM_AUXBUFFERTYPE
{
	IGN_ENUM_AUXBUFFERTYPE_ERRORS				= 2,												// Auxiliary buffer for errors
	IGN_ENUM_AUXBUFFERTYPE_BINARY				= 3,												// Auxiliary buffer for binary
	IGN_ENUM_AUXBUFFERTYPE_FORCEDWORD			= 0x7fffffff
};

enum			IGN_ENUM_MATERIALCOLORSRC
{
	IGN_ENUM_MATERIALCOLORSRC_MATERIAL			= 0,												// Uses the color from the current material
	IGN_ENUM_MATERIALCOLORSRC_COLOR1			= 1,												// Uses the diffuse vertex color
	IGN_ENUM_MATERIALCOLORSRC_COLOR2			= 2,												// Uses the specular vertex color
	IGN_ENUM_MATERIALCOLORSRC_FORCEDWORD		= 0x7fffffff
};

enum			IGN_ENUM_TEXADDRESS
{
	IGN_ENUM_TEXADDRESS_WRAP					= 1,												// Tiles the texture at every integer junction
	IGN_ENUM_TEXADDRESS_MIRROR					= 2,												// Flips the texture at every integer junction
	IGN_ENUM_TEXADDRESS_CLAMP					= 3,												// Tex coords outside the base are set to nearest tex edge color
	IGN_ENUM_TEXADDRESS_BORDER					= 4,												// Tex coords outside the base are set to the border color
	IGN_ENUM_TEXADDRESS_MIRRORONCE				= 5,												// Clamps the absolute value of the tex coord to the maximum value
	IGN_ENUM_TEXADDRESS_FORCEDWORD				= 0x7fffffff
};

enum			IGN_ENUM_TEXFILTER
{
	IGN_ENUM_TEXFILTER_NONE						= 0,												// Mipmapping disabled (uses the magnification filter instead)
	IGN_ENUM_TEXFILTER_POINT					= 1,												// Point filtering used as mag or min filter
	IGN_ENUM_TEXFILTER_LINEAR					= 2,												// Bilinear interpolation filtering used as mag or min filter
	IGN_ENUM_TEXFILTER_ANISOTROPIC				= 3,												// Anisotropic texture filtering used as mag or min filter
	IGN_ENUM_TEXFILTER_FLATCUBIC				= 4,												// Flat-cubic filtering used as a texture mag filter
	IGN_ENUM_TEXFILTER_GAUSSIANCUBIC			= 5,												// Gaussian-cubic filtering used as a texture mag filter
	IGN_ENUM_TEXFILTER_PYRAMIDALQUAD			= 6,												// A 4-sample tent filter used as a texture mag or min filter
	IGN_ENUM_TEXFILTER_GAUSSIANQUAD				= 7,												// A 4-sample Gaussian filter used as a texture mag or min filter
	IGN_ENUM_TEXFILTER_FORCEDWORD				= 0x7fffffff
};

enum			IGN_ENUM_TEXOPERATION
{
	IGN_ENUM_TEXOPERATION_DISABLE				=  1,												// Disables output from this texture stage
	IGN_ENUM_TEXOPERATION_SELECTARG1			=  2,												// Uses this texture stage's first argument, unmodified
	IGN_ENUM_TEXOPERATION_SELECTARG2			=  3,												// Uses this texture stage's second argument, unmodified
	IGN_ENUM_TEXOPERATION_MODULATE				=  4,												// Multiplies the components of the arguments
	IGN_ENUM_TEXOPERATION_MODULATE2X			=  5,												// (see above) and shifts the products to the left 1 bit 
	IGN_ENUM_TEXOPERATION_MODULATE4X			=  6,												// (see above) and shifts the products to the left 2 bits
	IGN_ENUM_TEXOPERATION_ADD					=  7,												// Adds the components of the arguments
	IGN_ENUM_TEXOPERATION_ADDSIGNED				=  8,												// Adds the components of the arguments with a –0.5 bias
	IGN_ENUM_TEXOPERATION_ADDSIGNED2X			=  9,												// (see above) and shifts the products to the left 1 bit
	IGN_ENUM_TEXOPERATION_SUBTRACT				= 10,												// Subtracts the second argument from the first argument
	IGN_ENUM_TEXOPERATION_ADDSMOOTH				= 11,												// Add the arguments; then subtract their product from the sum
	IGN_ENUM_TEXOPERATION_BLENDDIFFUSEALPHA		= 12,												// Linearly blending, using the interpolated vertex alpha
	IGN_ENUM_TEXOPERATION_BLENDTEXTUREALPHA		= 13,												// Linearly blending, using the texture alpha
	IGN_ENUM_TEXOPERATION_BLENDFACTORALPHA		= 14,												// Linearly blending, using scalar alpha set with color factor
	IGN_ENUM_TEXOPERATION_BLENDTEXTUREALPHAPM	= 15,												// Linearly blend a texture stage that uses a premultiplied alpha
	IGN_ENUM_TEXOPERATION_BLENDCURRENTALPHA		= 16,												// Linearly blend this texture stage, using the previous alpha
	IGN_ENUM_TEXOPERATION_PREMODULATE			= 17,												// (see DirectX documentation)
	IGN_ENUM_TEXOPERATION_MODALPHA_ADDCOLOR		= 18,												// (see DirectX documentation)
	IGN_ENUM_TEXOPERATION_MODCOLOR_ADDALPHA		= 19,												// (see DirectX documentation)
	IGN_ENUM_TEXOPERATION_MODINVALPHA_ADDCOLOR	= 20,												// (see DirectX documentation)
	IGN_ENUM_TEXOPERATION_MODINVCOLOR_ADDALPHA	= 21,												// (see DirectX documentation)
	IGN_ENUM_TEXOPERATION_BUMPENVMAP			= 22,												// Bump mapping, using the environment map in the next stage
	IGN_ENUM_TEXOPERATION_BUMPENVMAPLUMINANCE	= 23,												// (see above) with luminance
	IGN_ENUM_TEXOPERATION_DOTPRODUCT3			= 24,												// (see DirectX documentation)
	IGN_ENUM_TEXOPERATION_MULTIPLYADD			= 25,												// (see DirectX documentation)
	IGN_ENUM_TEXOPERATION_LERP					= 26,												// (see DirectX documentation)
	IGN_ENUM_TEXOPERATION_FORCE_DWORD			= 0x7fffffff
};

enum			IGN_ENUM_TEXARGUMENT
{
	IGN_ENUM_TEXARGUMENT_DIFFUSE				= 0x00000000,										// The diffuse color interpolated from vertex components
	IGN_ENUM_TEXARGUMENT_CURRENT				= 0x00000001,										// The result of the previous blending stage
	IGN_ENUM_TEXARGUMENT_TEXTURE				= 0x00000002,										// The texture color for this texture stage
	IGN_ENUM_TEXARGUMENT_COLORFACTOR			= 0x00000003,										// The color factor set in a previous call
	IGN_ENUM_TEXARGUMENT_SPECULAR				= 0x00000004,										// The specular color interpolated from vertex components
	IGN_ENUM_TEXARGUMENT_TEMP					= 0x00000005,										// The temporary register color
    IGN_ENUM_TEXARGUMENT_COMPLEMENT				= 0x00000010,										// Inverts the argument (modifier flag)
    IGN_ENUM_TEXARGUMENT_ALPHAREPLICATE			= 0x00000020,										// Replicates the alpha to all color channels (modifier flag)
	IGN_ENUM_TEXARGUMENT_FORCEDWORD				= 0x7fffffff
};

enum			IGN_ENUM_TEXCOORDCALC
{
	IGN_ENUM_TEXCOORDCLAC_PASSTHRU				= 0x00000000,										// Uses the texture coordinates contained within the vertex
	IGN_ENUM_TEXCOORDCALC_CAMERASPACENORMAL		= 0x00010000,										// Uses the vertex normal, transformed to camera space
	IGN_ENUM_TEXCOORDCALC_CAMERASPACEPOSITION	= 0x00020000,										// Uses the vertex position, transformed to camera space
	IGN_ENUM_TEXCOORDCALC_CAMERASPACEREFLVECTOR	= 0x00030000,										// Uses the reflection vector, transformed to camera space
	IGN_ENUM_TEXCOORDCALC_FORCEDWORD			= 0x7fffffff
};

enum			IGN_ENUM_POOLTYPE
{
	IGN_ENUM_POOLTYPE_DEFAULT					= 0,												// Resources are placed in the most appropriate memory pool
	IGN_ENUM_POOLTYPE_MANAGED					= 1,												// Resources are copied automatically to device-accessible memory
	IGN_ENUM_POOLTYPE_SYSTEMMEM					= 2,												// Memory that is not typically accessible by the 3-D device
	IGN_ENUM_POOLTYPE_SCRATCH					= 3,												// Placed in system RAM (not accessible by the 3-D device)
	IGN_ENUM_POOLTYPE_FORCE_DWORD				= 0x7fffffff
};

enum			IGN_ENUM_CUBEMAPFACE
{
	IGN_ENUM_CUBEMAPFACE_POSITIVE_X				= 0,												// Positive x-face of the cubemap
	IGN_ENUM_CUBEMAPFACE_NEGATIVE_X				= 1,												// Negative x-face of the cubemap
	IGN_ENUM_CUBEMAPFACE_POSITIVE_Y				= 2,												// Positive y-face of the cubemap
	IGN_ENUM_CUBEMAPFACE_NEGATIVE_Y				= 3,												// Negative y-face of the cubemap
	IGN_ENUM_CUBEMAPFACE_POSITIVE_Z				= 4,												// Positive z-face of the cubemap
	IGN_ENUM_CUBEMAPFACE_NEGATIVE_Z				= 5,												// Negative z-face of the cubemap
	IGN_ENUM_CUBEMAPFACE_NONE					= 6,												// Not any cube map face
	IGN_ENUM_CUBEMAPFACE_FORCE_DWORD			= 0xffffffff
};

enum			IGN_ENUM_DECLTYPE
{
	IGN_ENUM_DECLTYPE_FLOAT1					= 0,												// 1-D float expanded to (float, 0, 0, 1)
	IGN_ENUM_DECLTYPE_FLOAT2					,													// 2-D float expanded to (float, float, 0, 1)
	IGN_ENUM_DECLTYPE_FLOAT3					,													// 3-D float expanded to (float, float, float, 1)
	IGN_ENUM_DECLTYPE_FLOAT4					,													// 4-D float expanded to (float, float, float, float)
	IGN_ENUM_DECLTYPE_COLORVALUE				,													// 4-D packed unsigned bytes mapped to 0 to 1 range
	IGN_ENUM_DECLTYPE_UBYTE4					,													// 4-D unsigned byte
	IGN_ENUM_DECLTYPE_SHORT2					,													// 2-D signed short expanded to (value, value, 0, 1)
	IGN_ENUM_DECLTYPE_SHORT4					,													// 4-D signed short expanded to (value, value, value, value)

	//
	// The following types are valid only with vertex shaders >= 2.0
	//

	IGN_ENUM_DECLTYPE_UBYTE4N					,													// Each of 4 bytes is normalized by dividing to 255.0
	IGN_ENUM_DECLTYPE_SHORT2N					,													// Normalized 2-D, exp. to (1.byte/32767.0, 2.byte/32767.0, 0, 1)
	IGN_ENUM_DECLTYPE_SHORT4N					,													// Normalized 4-D, exp. to vector/32767.0
	IGN_ENUM_DECLTYPE_USHORT2N					,													// (see above) using unsigned short
	IGN_ENUM_DECLTYPE_USHORT4N					,													// (see above) using unsigned short
	IGN_ENUM_DECLTYPE_UDEC3						,													// 3-D unsigned 10-10-10 exp. to (value, value, value, 1)
	IGN_ENUM_DECLTYPE_DEC3N						,													// 3-D 10-10-10 norm. & exp. to ((v[0], v[1], v[2])/511.0, 1)
	IGN_ENUM_DECLTYPE_FLOAT16_2					,													// Two 16-bit floating point values exp. to (value, value, 0, 1)
	IGN_ENUM_DECLTYPE_FLOAT16_4					,													// Four 16-bit floating point values exp. to (val, val, val, val)
	IGN_ENUM_DECLTYPE_UNUSED																		// Type field in the declaration is unused
};

enum			IGN_ENUM_DECLMETHOD
{
	IGN_ENUM_DECLMETHOD_DEFAULT					= 0,												// The tessellator copies the vertex data as is
	IGN_ENUM_DECLMETHOD_PARTIALU				,													// Computes the tangent in the U direction
	IGN_ENUM_DECLMETHOD_PARTIALV				,													// Computes the tangent in the V direction
	IGN_ENUM_DECLMETHOD_CROSSUV					,													// Computes the cross product of the two tangents
	IGN_ENUM_DECLMETHOD_UV						,													// Copies out the U, V values
	IGN_ENUM_DECLMETHOD_LOOKUP					,													// Looks up a displacement map
	IGN_ENUM_DECLMETHOD_LOOKUPPRESAMPLED															// Looks up a presampled displacement map
};

enum			IGN_ENUM_DECLUSAGE
{
	IGN_ENUM_DECLUSAGE_POSITION					= 0,												// Position data
	IGN_ENUM_DECLUSAGE_BLENDWEIGHT				,													// Blending weight data
	IGN_ENUM_DECLUSAGE_BLENDINDICES				,													// Blending indices data
	IGN_ENUM_DECLUSAGE_NORMAL					,													// Vertex normal data
	IGN_ENUM_DECLUSAGE_PSIZE					,													// Point size data
	IGN_ENUM_DECLUSAGE_TEXCOORD					,													// Texture coordinate data
	IGN_ENUM_DECLUSAGE_TANGENT					,													// Vertex tangent data
	IGN_ENUM_DECLUSAGE_BINORMAL					,													// Vertex binormal data
	IGN_ENUM_DECLUSAGE_TESSFACTOR				,													// Single positive floating point value that controls tessellation
    IGN_ENUM_DECLUSAGE_POSITIONT				,													// Vertex data contains transformed and lit position data
	IGN_ENUM_DECLUSAGE_COLOR					,													// Vertex data contains diffuse or specular color
	IGN_ENUM_DECLUSAGE_FOG						,													// Vertex data contains fog data
	IGN_ENUM_DECLUSAGE_DEPTH					,													// Vertex data contains depth data
	IGN_ENUM_DECLUSAGE_SAMPLE																		// Vertex data contains sampler data
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Vertex Shaders declaration macros for DirectX 8.0 / 8.1
//
#define IGN_SHADER_ASM_DEBUG					1<<0
#define IGN_SHADER_ASM_SKIPVALIDATION			1<<1

typedef enum _IGN_ENUM_VSD_TOKENTYPE
{
    IGN_ENUM_VSD_TOKEN_NOP						= 0,												// NOP or extension
    IGN_ENUM_VSD_TOKEN_STREAM					,													// Stream selector
    IGN_ENUM_VSD_TOKEN_STREAMDATA				,													// Stream data definition (map to vertex input memory)
    IGN_ENUM_VSD_TOKEN_TESSELLATOR				,													// Vertex input memory from tessellator
    IGN_ENUM_VSD_TOKEN_CONSTMEM					,													// Constant memory from shader
    IGN_ENUM_VSD_TOKEN_EXT						,													// Extension
    IGN_ENUM_VSD_TOKEN_END						= 7,												// End-of-array (requires all DWORD bits to be 1)
    IGN_ENUM_VSD_FORCE_DWORD					= 0xffffffff
} IGN_ENUM_VSD_TOKENTYPE;

#define IGN_VSD_TOKENTYPESHIFT					29
#define IGN_VSD_TOKENTYPEMASK					(7 << IGN_VSD_TOKENTYPESHIFT)

#define IGN_VSD_STREAMNUMBERSHIFT				0
#define IGN_VSD_STREAMNUMBERMASK				(0xF << IGN_VSD_STREAMNUMBERSHIFT)

#define IGN_VSD_DATALOADTYPESHIFT				28
#define IGN_VSD_DATALOADTYPEMASK				(0x1 << IGN_VSD_DATALOADTYPESHIFT)
	
#define IGN_VSD_DATATYPESHIFT					16
#define IGN_VSD_DATATYPEMASK					(0xF << IGN_VSD_DATATYPESHIFT)

#define IGN_VSD_SKIPCOUNTSHIFT					16
#define IGN_VSD_SKIPCOUNTMASK					(0xF << IGN_VSD_SKIPCOUNTSHIFT)

#define IGN_VSD_VERTEXREGSHIFT					0
#define IGN_VSD_VERTEXREGMASK					(0x1F << IGN_VSD_VERTEXREGSHIFT)

#define IGN_VSD_VERTEXREGINSHIFT				20
#define IGN_VSD_VERTEXREGINMASK					(0xF << IGN_VSD_VERTEXREGINSHIFT)

#define IGN_VSD_CONSTCOUNTSHIFT					25
#define IGN_VSD_CONSTCOUNTMASK					(0xF << IGN_VSD_CONSTCOUNTSHIFT)

#define IGN_VSD_CONSTADDRESSSHIFT				0
#define IGN_VSD_CONSTADDRESSMASK				(0x7F << IGN_VSD_CONSTADDRESSSHIFT)

#define IGN_VSD_CONSTRSSHIFT					16
#define IGN_VSD_CONSTRSMASK						(0x1FFF << IGN_VSD_CONSTRSSHIFT)

#define IGN_VSD_EXTCOUNTSHIFT					24
#define IGN_VSD_EXTCOUNTMASK					(0x1F << IGN_VSD_EXTCOUNTSHIFT)

#define IGN_VSD_EXTINFOSHIFT					0
#define IGN_VSD_EXTINFOMASK						(0xFFFFFF << IGN_VSD_EXTINFOSHIFT)

#define IGN_VSD_MAKETOKENTYPE(tokenType)		((tokenType << IGN_VSD_TOKENTYPESHIFT) & IGN_VSD_TOKENTYPEMASK)

//
// macros for generation of CreateVertexShader Declaration token array
//

//
// Set current stream
// _StreamNumber [0..(MaxStreams-1)] stream to get data from
//
#define IGN_VSD_STREAM( _StreamNumber ) \
    (IGN_VSD_MAKETOKENTYPE(IGN_ENUM_VSD_TOKEN_STREAM) | (_StreamNumber))

//
// Set tessellator stream
//
#define IGN_VSD_STREAMTESSSHIFT					28
#define IGN_VSD_STREAMTESSMASK					(1 << IGN_VSD_STREAMTESSSHIFT)
#define IGN_VSD_STREAM_TESS( ) \
    (IGN_VSD_MAKETOKENTYPE(IGN_ENUM_VSD_TOKEN_STREAM) | (IGN_VSD_STREAMTESSMASK))

//
// bind single vertex register to vertex element from vertex stream
//
// _VertexRegister [0..15] address of the vertex register
// _Type [D3DVSDT_*] dimensionality and arithmetic data type

#define IGN_VSD_REG( _VertexRegister, _Type ) \
    (IGN_VSD_MAKETOKENTYPE(IGN_ENUM_VSD_TOKEN_STREAMDATA) |            \
     ((_Type) << IGN_VSD_DATATYPESHIFT) | (_VertexRegister))

//
// Skip _DWORDCount DWORDs in vertex
//
#define IGN_VSD_SKIP( _DWORDCount ) \
    (IGN_VSD_MAKETOKENTYPE(IGN_ENUM_VSD_TOKEN_STREAMDATA) | 0x10000000 | \
     ((_DWORDCount) << IGN_VSD_SKIPCOUNTSHIFT))

//
// load data into vertex shader constant memory
//
// _ConstantAddress [0..95] - address of constant array to begin filling data
// _Count [0..15] - number of constant vectors to load (4 DWORDs each)
// followed by 4*_Count DWORDS of data
//
#define IGN_VSD_CONST( _ConstantAddress, _Count ) \
    (IGN_VSD_MAKETOKENTYPE(IGN_ENUM_VSD_TOKEN_CONSTMEM) | \
     ((_Count) << IGN_VSD_CONSTCOUNTSHIFT) | (_ConstantAddress))

//
// enable tessellator generated normals
//
// _VertexRegisterIn  [0..15] address of vertex register whose input stream
//                            will be used in normal computation
// _VertexRegisterOut [0..15] address of vertex register to output the normal to
//
#define GE_VSD_TESSNORMAL( _VertexRegisterIn, _VertexRegisterOut ) \
    (IGN_VSD_MAKETOKENTYPE(IGN_ENUM_VSD_TOKEN_TESSELLATOR) | \
     ((_VertexRegisterIn) << IGN_VSD_VERTEXREGINSHIFT) | \
     ((0x02) << IGN_VSD_DATATYPESHIFT) | (_VertexRegisterOut))

//
// enable tessellator generated surface parameters
//
// _VertexRegister [0..15] address of vertex register to output parameters
//
#define IGN_VSD_TESSUV( _VertexRegister ) \
    (IGN_VSD_MAKETOKENTYPE(IGN_ENUM_VSD_TOKEN_TESSELLATOR) | 0x10000000 | \
     ((0x01) << IGN_VSD_DATATYPESHIFT) | (_VertexRegister))

//
// Generates END token
//
#define IGN_VSD_END()							0xFFFFFFFF

//
// Generates NOP token
//
#define IGN_VSD_NOP()							0x00000000

//
// bit declarations for _Type fields
//
#define IGN_VSDT_FLOAT1							0x00												// 1D float expanded to (value, 0., 0., 1.)
#define IGN_VSDT_FLOAT2							0x01												// 2D float expanded to (value, value, 0., 1.)
#define IGN_VSDT_FLOAT3							0x02												// 3D float expanded to (value, value, value, 1.)
#define IGN_VSDT_FLOAT4							0x03												// 4D float
#define IGN_VSDT_D3DCOLOR						0x04												// 4D packed unsigned bytes mapped to 0. to 1. range
																									// Input is in D3DCOLOR format (ARGB) expanded to (R, G, B, A)
#define IGN_VSDT_UBYTE4							0x05												// 4D unsigned byte
#define IGN_VSDT_SHORT2							0x06												// 2D signed short expanded to (value, value, 0., 1.)
#define IGN_VSDT_SHORT4							0x07												// 4D signed short

//
// assignments of vertex input registers for fixed function vertex shader
//
#define	IGN_VSDE_POSITION						0
#define IGN_VSDE_BLENDWEIGHT					1
#define IGN_VSDE_BLENDINDICES					2
#define IGN_VSDE_NORMAL							3
#define IGN_VSDE_PSIZE							4
#define IGN_VSDE_DIFFUSE						5
#define IGN_VSDE_SPECULAR						6
#define IGN_VSDE_TEXCOORD0						7
#define IGN_VSDE_TEXCOORD1						8
#define IGN_VSDE_TEXCOORD2						9
#define IGN_VSDE_TEXCOORD3						10
#define IGN_VSDE_TEXCOORD4						11
#define IGN_VSDE_TEXCOORD5						12
#define IGN_VSDE_TEXCOORD6						13
#define IGN_VSDE_TEXCOORD7						14
#define IGN_VSDE_POSITION2						15
#define IGN_VSDE_NORMAL2						16
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////



typedef		IGN_OBJIF_GfxInstance*					IGN_LP_OBJIF_GfxInstance;
typedef		IGN_OBJIF_GfxContext*					IGN_LP_OBJIF_GfxContext;
typedef		IGN_OBJIF_GfxGauge*						IGN_LP_OBJIF_GfxGauge;
typedef		IGN_OBJIF_GfxMixer*						IGN_LP_OBJIF_GfxMixer;
typedef		IGN_OBJIF_GfxAssistant*					IGN_LP_OBJIF_GfxAssistant;
typedef		IGN_OBJIF_GfxCamera*					IGN_LP_OBJIF_GfxCamera;
typedef		IGN_OBJIF_GfxVisual*					IGN_LP_OBJIF_GfxVisual;
typedef		IGN_OBJIF_GfxLight*						IGN_LP_OBJIF_GfxLight;
typedef		IGN_OBJIF_GfxFog*						IGN_LP_OBJIF_GfxFog;
typedef		IGN_OBJIF_GfxVertexData*				IGN_LP_OBJIF_GfxVertexData;
typedef		IGN_OBJIF_GfxIndexData*					IGN_LP_OBJIF_GfxIndexData;
typedef		IGN_OBJIF_GfxShader*					IGN_LP_OBJIF_GfxShader;
typedef		IGN_OBJIF_GfxMaterial*					IGN_LP_OBJIF_GfxMaterial;
typedef		IGN_OBJIF_GfxVertexProgDcl*				IGN_LP_OBJIF_GfxVertexProgDcl;
typedef		IGN_OBJIF_GfxVertexProgram*				IGN_LP_OBJIF_GfxVertexProgram;
typedef		IGN_OBJIF_GfxPixelProgram*				IGN_LP_OBJIF_GfxPixelProgram;
typedef		IGN_OBJIF_GfxTextureBlock*				IGN_LP_OBJIF_GfxTextureBlock;
typedef		IGN_OBJIF_GfxImageData*					IGN_LP_OBJIF_GfxImageData;




//********************************************************************************************************************************************************************
//				Typedefs for all interface methods
//********************************************************************************************************************************************************************

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// DLL export methods
//
typedef			IGNRESULT	IGN_DLLCALL	IGN_METHOD_GetSubHookGraphic						(			IGN_STRUCT_IFSubGraphic**		a_ppInterface				);

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// DLL interface methods
//
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxGetVersion							(			IGN_STRUCT_GfxVersion&			a_Version					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxCreateInstance						(			IGN_ENUM_GFXINSTANCETYPE		a_InstanceType,
																										IGN_LP_OBJIF_GfxInstance&		a_pInstanceInterface		);

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Instance methods	
//
typedef			IGN_ENUM_GFXINSTANCETYPE
							IGN_OBJCALL	IGN_METHOD_GfxInstance_GetInstanceType				(			void														);														
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxInstance_EnumContext					(			IGNU32							a_u32ContextNumber,
																										IGN_STRUCT_GfxContextDesc&		a_ContextDescription		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxInstance_GetContextCount				(			IGNU32&							a_u32ContextCount			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxInstance_CreateContext				(			IGNU32							a_u32ContextNumber,
																										HWND							a_hWindow,
																										IGN_ENUM_GFXCONTEXTCREATE		a_Flags,
																										IGNBOOL							a_bWindowed,
																										IGN_ENUM_MULTISAMPLE			a_MultisampleType,
																										IGN_LP_OBJIF_GfxContext&		a_pContextInterface			);	

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Context methods																				
//
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxContext_CreateGauge					(			IGN_LP_OBJIF_GfxGauge&			a_pGaugeInterface			);																				
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxContext_CreateMixer					(			IGN_LP_OBJIF_GfxMixer&			a_pMixerInterface			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxContext_CreateAssistant				(			IGN_LP_OBJIF_GfxAssistant&		a_pProgCompiler				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxContext_CreateCamera					(			IGN_LP_OBJIF_GfxCamera&			a_pCameraInterface			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxContext_CreateVisual					(			IGN_LP_OBJIF_GfxVisual&			a_pVisualInterface			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxContext_CreateLight					(			IGN_LP_OBJIF_GfxLight&			a_pLightInterface			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxContext_CreateFog						(			IGN_LP_OBJIF_GfxFog&			a_pFogInterface				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxContext_CreateVertexData				(			IGN_LP_OBJIF_GfxVertexData&		a_pVertexDataInterface		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxContext_CreateIndexData				(			IGN_LP_OBJIF_GfxIndexData&		a_pIndexDataInterface		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxContext_CreateShader					(			IGN_LP_OBJIF_GfxShader&			a_pShaderInterface			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxContext_CreateMaterial				(			IGN_LP_OBJIF_GfxMaterial&		a_pMaterialInterface		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxContext_CreateVertexProgDcl			(			IGN_LP_OBJIF_GfxVertexProgDcl&	a_pVertexProgDclInterface	);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxContext_CreateVertexProgram			(			IGN_LP_OBJIF_GfxVertexProgram&	a_pVertexProgramInterface	);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxContext_CreatePixelProgram			(			IGN_LP_OBJIF_GfxPixelProgram&	a_pPixelProgramInterface	);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxContext_CreateTextureBlock			(			IGN_LP_OBJIF_GfxTextureBlock&	a_pTextureBlockInterface	);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxContext_CreateImageData				(			IGN_LP_OBJIF_GfxImageData&		a_pImageDataInterface		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxContext_ToggleNewMode					(			IGNBOOL							a_bWindowed,
																										IGN_ENUM_MULTISAMPLE			a_MultisampleType,
																								const	IGN_STRUCT_GfxModeDesc&			a_ModeDesc					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxContext_IsResetNecessary				(			IGNBOOL&						a_bIsNecessary				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxContext_Reset							(			void														);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxContext_SetInternalOptimization		(			IGN_ENUM_ITLOPTIMIZATION		a_InternalOptimization		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxContext_GetInternalOptimization		(			IGN_ENUM_ITLOPTIMIZATION&		a_InternalOptimization		);

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Gauge methods
//
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxGauge_Reset							(			void														);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxGauge_SetModus						(			IGN_ENUM_GAUGEMODUS				a_Modus						);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxGauge_GetValues						(			IGN_STRUCT_GfxGaugeValues&		a_GaugeValues				);

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Mixer methods																				
//
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_SetViewport						(	const	IGN_STRUCT_Viewport&			a_Viewport					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_GetViewport						(			IGN_STRUCT_Viewport&			a_Viewport					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_ClearViewport					(			IGN_ENUM_VIEWPORTCLEAR			a_Flags,
																										IGNU32							a_u32Color,
																										IGNFLOAT						a_fZ,
																										IGNU32							a_u32Stencil				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_SwapScreen						(			HWND							a_hDestWindowOverride,
																								const	RECT*							a_pSourceRectangle,
																								const	RECT*							a_pDestinationRectangle		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_BeginScene						(			void														);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_EndScene						(			void														);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_ExecuteCamera					(			IGN_LP_OBJIF_GfxCamera			a_pCameraInterface			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_ExecuteVisual					(			IGN_LP_OBJIF_GfxVisual			a_pVisualInterface			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_ExecuteLight					(			IGN_LP_OBJIF_GfxLight			a_pLightInterface			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_ExecuteFog						(			IGN_LP_OBJIF_GfxFog				a_pFogInterface				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_GetActiveCamera					(			IGN_LP_OBJIF_GfxCamera&			a_pCameraInterface			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_GetActiveVisual					(			IGN_LP_OBJIF_GfxVisual&			a_pVisualInterface			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_GetActiveLight					(			IGN_LP_OBJIF_GfxLight&			a_pLightInterface			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_GetActiveFog					(			IGN_LP_OBJIF_GfxFog&			a_pFogInterface				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_SetMasterRenderMode				(			IGN_ENUM_MASTERRENDERMODE		a_RenderMode				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_GetMasterRenderMode				(			IGN_ENUM_MASTERRENDERMODE&		a_RenderMode				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_SetMasterShadeMode				(			IGN_ENUM_MASTERSHADEMODE		a_ShadeMode					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_GetMasterShadeMode				(			IGN_ENUM_MASTERSHADEMODE&		a_ShadeMode					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_SetMasterCullingMode			(			IGN_ENUM_MASTERCULLINGMODE		a_CullingMode				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_GetMasterCullingMode			(			IGN_ENUM_MASTERCULLINGMODE&		a_CullingMode				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_SetMasterDepthTesting			(			IGN_ENUM_MASTERCMPFUNC			a_CmpFunc					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_GetMasterDepthTesting			(			IGN_ENUM_MASTERCMPFUNC&			a_CmpFunc					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_SetMasterDepthWrite				(			IGN_ENUM_MASTERWRITE			a_Write						);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_GetMasterDepthWrite				(			IGN_ENUM_MASTERWRITE&			a_Write						);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_SetMasterStencilTesting			(			IGN_ENUM_MASTERSTENCIL			a_Stencil,
																								const	IGN_STRUCT_StencilData&			a_StencilData				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_GetMasterStencilTesting			(			IGN_ENUM_MASTERSTENCIL&			a_Stencil,
																										IGN_STRUCT_StencilData&			a_StencilData				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_SetMasterBlending				(			IGN_ENUM_MASTERBLENDING			a_SourceBlend,
																										IGN_ENUM_MASTERBLENDING			a_DestinationBlend			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_GetMasterBlending				(			IGN_ENUM_MASTERBLENDING&		a_SourceBlend,
																										IGN_ENUM_MASTERBLENDING&		a_DestinationBlend			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_SetMasterAlphaTesting			(			IGN_ENUM_MASTERALPHATEST		a_AlphaTest,
																								const	IGN_STRUCT_AlphaTestData&		a_AlphaTestData				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_GetMasterAlphaTesting			(			IGN_ENUM_MASTERALPHATEST&		a_AlphaTest,
																										IGN_STRUCT_AlphaTestData&		a_AlphaTestData				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_SetMasterLighting				(			IGNBOOL							a_bEnabled					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_GetMasterLighting				(			IGNBOOL&						a_bEnabled					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_SetMasterAlphaBlending			(			IGNBOOL							a_bEnabled					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_GetMasterAlphaBlending			(			IGNBOOL&						a_bEnabled					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_SetMasterCameraExec				(			IGN_LP_OBJIF_GfxCamera			a_pCameraInterface			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_GetMasterCameraExec				(			IGN_LP_OBJIF_GfxCamera&			a_pCameraInterface			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_SetMasterVisualExec				(			IGN_LP_OBJIF_GfxVisual			a_pVisualInterface			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_GetMasterVisualExec				(			IGN_LP_OBJIF_GfxVisual&			a_pVisualInterface			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_SetMasterLightExec				(			IGN_LP_OBJIF_GfxLight			a_pLightInterface			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_GetMasterLightExec				(			IGN_LP_OBJIF_GfxLight&			a_pLightInterface			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_SetMasterFogExec				(			IGN_LP_OBJIF_GfxFog				a_pFogInterface				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_GetMasterFogExec				(			IGN_LP_OBJIF_GfxFog&			a_pFogInterface				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_SetMasterVertexDataExec			(			IGN_LP_OBJIF_GfxVertexData		a_pVertexDataInterface		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_GetMasterVertexDataExec			(			IGN_LP_OBJIF_GfxVertexData&		a_pVertexDataInterface		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_SetMasterIndexDataExec			(			IGN_LP_OBJIF_GfxIndexData		a_pIndexDataInterface		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_GetMasterIndexDataExec			(			IGN_LP_OBJIF_GfxIndexData&		a_pIndexDataInterface		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_SetMasterShaderExec				(			IGN_LP_OBJIF_GfxShader			a_pShaderInterface			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_GetMasterShaderExec				(			IGN_LP_OBJIF_GfxShader&			a_pShaderInterface			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_SetMasterMaterialExec			(			IGN_LP_OBJIF_GfxMaterial		a_pMaterialInterface		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_GetMasterMaterialExec			(			IGN_LP_OBJIF_GfxMaterial&		a_pMaterialInterface		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_SetMasterVertexProgDclExec		(			IGN_LP_OBJIF_GfxVertexProgDcl	a_pVertexProgDclInterface	);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_GetMasterVertexProgDclExec		(			IGN_LP_OBJIF_GfxVertexProgDcl&	a_pVertexProgDclInterface	);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_SetMasterVertexProgramExec		(			IGN_LP_OBJIF_GfxVertexProgram	a_pVertexProgramInterface	);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_GetMasterVertexProgramExec		(			IGN_LP_OBJIF_GfxVertexProgram&	a_pVertexProgramInterface	);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_SetMasterPixelProgramExec		(			IGN_LP_OBJIF_GfxPixelProgram	a_pPixelProgramInterface	);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_GetMasterPixelProgramExec		(			IGN_LP_OBJIF_GfxPixelProgram&	a_pPixelProgramInterface	);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_SetMasterTextureBlockExec		(			IGN_LP_OBJIF_GfxTextureBlock	a_pTextureBlockInterface	);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_GetMasterTextureBlockExec		(			IGN_LP_OBJIF_GfxTextureBlock&	a_pTextureBlockInterface	);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_SetMasterImageDataExec			(			IGN_LP_OBJIF_GfxImageData		a_pImageDataInterface		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_GetMasterImageDataExec			(			IGN_LP_OBJIF_GfxImageData&		a_pImageDataInterface		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_CreateSpecialDepthBuffer		(			IGNU32							a_u32Heigth,
																										IGNU32							a_u32Width,
																										IGN_ENUM_DEPTHFORMAT			a_Format,
																										IGNHANDLE&						a_hSpecialDepthBuffer		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_DestroySpecialDepthBuffer		(			IGNHANDLE						a_hSpecialDepthBuffer		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_SetRenderTarget					(			IGNHANDLE						a_hSpecialImageRenderTarget,
																										IGNHANDLE						a_hSpecialDepthRenderTarget	);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_GetRenderTarget					(			IGNHANDLE&						a_hSpecialImageRenderTarget,
																										IGNHANDLE&						a_hSpecialDepthRenderTarget	);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_GetFVFSize						(			IGNU32							a_u32FVF,
																										IGNU32&							a_VertexSize				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_SetClipPlane					(			IGNU32							a_u32Index,
																										IGNBOOL							a_bEnabled					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_GetClipPlane					(			IGNU32							a_u32Index,
																										IGNBOOL&						a_bEnabled					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_SetClipPlaneData				(			IGNU32							a_u32Index,
																								const	IGN_STRUCT_PlaneData&			a_ClipPlane					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMixer_GetClipPlaneData				(			IGNU32							a_u32Index,
																										IGN_STRUCT_PlaneData&			a_ClipPlane					);

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Assistant methods
//
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxAssistant_GetMethodSupport			(			IGN_STRUCT_GfxAssistMethods&	a_AssistMethods				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxAssistant_GetAuxBuffer				(			IGN_ENUM_AUXBUFFERTYPE			a_AuxBufferType,
																										void**							a_ppData,
																										IGNU32&							a_u32NumberOfBytes			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxAssistant_AssembleProgram				(	const	IGNCHAR*						a_pSource,
																										IGNU32							a_u32SourceSize				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxAssistant_AssembleProgramFromFile		(			LPCSTR							a_pString					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxAssistant_ComputeNormalMap			(			IGN_LP_OBJIF_GfxImageData		a_pImageDataInterface,
																										IGN_LP_OBJIF_GfxImageData		a_pSourceImageDataInterface,
																										IGN_ENUM_NORMALMAPGEN			a_GenerationControlFlags,
																										IGN_ENUM_CHANNEL				a_Channel,
																										IGNFLOAT						a_fAmplitude				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxAssistant_FillImageFromFile			(			IGN_LP_OBJIF_GfxImageData		a_pImageDataInterface,
																								const	IGN_STRUCT_LayersDesc&			a_LayersDescription,
																								const	IGNCHAR*						a_pSource					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxAssistant_GenerateMipMaps				(			IGN_LP_OBJIF_GfxImageData		a_pImageDataInterface		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxAssistant_CreateBox					(	const	IGN_STRUCT_BoxData&				a_BoxData,
																										IGN_LP_OBJIF_GfxVertexData		a_pVertexDataInterface,
																										IGN_LP_OBJIF_GfxIndexData		a_pIndexDataInterface		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxAssistant_CreateCylinder				(	const	IGN_STRUCT_CylinderData&		a_CylinderData,
																										IGN_LP_OBJIF_GfxVertexData		a_pVertexDataInterface,
																										IGN_LP_OBJIF_GfxIndexData		a_pIndexDataInterface		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxAssistant_CreatePolygon				(	const	IGN_STRUCT_PolygonData&			a_PolygonData,
																										IGN_LP_OBJIF_GfxVertexData		a_pVertexDataInterface,
																										IGN_LP_OBJIF_GfxIndexData		a_pIndexDataInterface		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxAssistant_CreateSphere				(	const	IGN_STRUCT_SphereData&			a_SphereData,
																										IGN_LP_OBJIF_GfxVertexData		a_pVertexDataInterface,
																										IGN_LP_OBJIF_GfxIndexData		a_pIndexDataInterface		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxAssistant_CreateTeapot				(			IGN_LP_OBJIF_GfxVertexData		a_pVertexDataInterface,
																										IGN_LP_OBJIF_GfxIndexData		a_pIndexDataInterface		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxAssistant_CreateText					(	const	IGN_STRUCT_TextData&			a_TextData,
																										IGN_LP_OBJIF_GfxVertexData		a_pVertexDataInterface,
																										IGN_LP_OBJIF_GfxIndexData		a_pIndexDataInterface		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxAssistant_CreateTorus					(	const	IGN_STRUCT_TorusData&			a_TorusData,
																										IGN_LP_OBJIF_GfxVertexData		a_pVertexDataInterface,
																										IGN_LP_OBJIF_GfxIndexData		a_pIndexDataInterface		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxAssistant_CreateSRTKeyFrameInterpolator(	const	IGN_STRUCT_KeyFrameItpData&		a_KeyFrameItpData,
																										IGNHANDLE&						a_hKeyFrameInterpolator		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxAssistant_DestroySRTKeyFrameInterpolator(			IGNHANDLE						a_hKeyFrameInterpolator		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxAssistant_GetInterpolatorName			(			IGNHANDLE						a_hKeyFrameInterpolator,
																										LPCSTR&							a_pString					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxAssistant_GetInterpolatorPeriod		(			IGNHANDLE						a_hKeyFrameInterpolator,
																										IGNDOUBLE&						a_dPeriod					);	
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxAssistant_GetInterpolatedSRT			(			IGNHANDLE						a_hKeyFrameInterpolator,
																										IGNDOUBLE						a_dTime,
																										IGN_STRUCT_3DVector&			a_Scale,
																										IGN_STRUCT_3DQuaternion&		a_Rotate,
																										IGN_STRUCT_3DVector&			a_Translate					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxAssistant_GetLastInterpolatedSRT		(			IGNHANDLE						a_hKeyFrameInterpolator,
																										IGN_STRUCT_3DVector&			a_Scale,
																										IGN_STRUCT_3DQuaternion&		a_Rotate,
																										IGN_STRUCT_3DVector&			a_Translate					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxAssistant_SaveImageToFile				(			IGN_LP_OBJIF_GfxImageData		a_pImageDataInterface,
																										IGNCHAR*						a_pSource					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxAssistant_DrawText					(			LPCSTR							a_pString,
																										IGNU32							a_u32Color,
																										RECT&							a_DestinationRectangle		);

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Camera methods																				
//
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxCamera_SetViewMatrix					(	const	IGN_STRUCT_4x4Matrix&			a_Matrix					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxCamera_GetViewMatrix					(			IGN_STRUCT_4x4Matrix&			a_Matrix					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxCamera_SetProjectionMatrix			(	const	IGN_STRUCT_4x4Matrix&			a_Matrix					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxCamera_GetProjectionMatrix			(			IGN_STRUCT_4x4Matrix&			a_Matrix					);

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Visual methods																				
//
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVisual_AttachVertexData				(			IGN_LP_OBJIF_GfxVertexData		a_pVertexDataInterface		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVisual_GetAttachedVertexData			(			IGN_LP_OBJIF_GfxVertexData&		a_pVertexDataInterface		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVisual_AttachIndexData				(			IGN_LP_OBJIF_GfxIndexData		a_pIndexDataInterface		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVisual_GetAttachedIndexData			(			IGN_LP_OBJIF_GfxIndexData&		a_pIndexDataInterface		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVisual_AttachShader					(			IGN_LP_OBJIF_GfxShader			a_pShaderInterface			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVisual_GetAttachedShader				(			IGN_LP_OBJIF_GfxShader&			a_pShaderInterface			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVisual_SetRenderMode					(			IGN_ENUM_RENDERMODE				a_RenderMode				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVisual_GetRenderMode					(			IGN_ENUM_RENDERMODE&			a_RenderMode				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVisual_SetShadeMode					(			IGN_ENUM_SHADEMODE				a_ShadeMode					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVisual_GetShadeMode					(			IGN_ENUM_SHADEMODE&				a_ShadeMode					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVisual_SetCullingMode					(			IGN_ENUM_CULLINGMODE			a_CullingMode				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVisual_GetCullingMode					(			IGN_ENUM_CULLINGMODE&			a_CullingMode				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVisual_SetDepthTesting				(			IGN_ENUM_CMPFUNC				a_CmpFunc					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVisual_GetDepthTesting				(			IGN_ENUM_CMPFUNC&				a_CmpFunc					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVisual_SetDepthWrite					(			IGN_ENUM_WRITE					a_Write						);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVisual_GetDepthWrite					(			IGN_ENUM_WRITE&					a_Write						);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVisual_SetStencilTesting				(			IGN_ENUM_STENCIL				a_Stencil,
																								const	IGN_STRUCT_StencilData&			a_StencilData				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVisual_GetStencilTesting				(			IGN_ENUM_STENCIL&				a_Stencil,
																										IGN_STRUCT_StencilData&			a_StencilData				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVisual_SetWorldMatrix					(	const	IGN_STRUCT_4x4Matrix&			a_Matrix					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVisual_GetWorldMatrix					(			IGN_STRUCT_4x4Matrix&			a_Matrix					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVisual_SetDrawCallDescription			(	const	IGN_STRUCT_DrawCallDesc&		a_DrawCallDesc				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVisual_GetDrawCallDescription			(			IGN_STRUCT_DrawCallDesc&		a_DrawCallDesc				);

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Light methods																				
//
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxLight_SetAmbientLighting				(			IGNU32							a_u32Color					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxLight_GetAmbientLighting				(			IGNU32&							a_u32Color					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxLight_SetVertexLighting				(			IGNU32							a_u32Index,
																										IGNBOOL							a_bEnabled					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxLight_GetVertexLighting				(			IGNU32							a_u32Index,
																										IGNBOOL&						a_bEnabled					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxLight_SetVertexLightingData			(			IGNU32							a_u32Index,
																								const	IGN_STRUCT_LightData&			a_LightData					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxLight_GetVertexLightingData			(			IGNU32							a_u32Index,
																										IGN_STRUCT_LightData&			a_LightData					);

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Fog methods																				
//
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxFog_SetFog							(	const	IGN_STRUCT_FogData&				a_FogData					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxFog_GetFog							(			IGN_STRUCT_FogData&				a_FogData					);

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// VertexData methods																				
//
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVertexData_CreateBuffer				(	const	IGN_STRUCT_VertexBufDesc&		a_VertexBufferDescription	);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVertexData_DestroyBuffer				(			void														);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVertexData_GetBufferDescription		(			IGN_STRUCT_VertexBufDesc&		a_VertexBufferDescription	);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVertexData_LockBuffer					(			IGNUINT							a_uiOffset,
																										IGNUINT							a_uiSize,
																										IGNDWORD						a_dwLockFlags,
																										IGNUCHAR**						a_ppucharBytes				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVertexData_UnlockBuffer				(			void														);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVertexData_SetStreamNumber			(			IGNU32							a_u32StreamNumber			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVertexData_GetStreamNumber			(			IGNU32&							a_u32StreamNumber			);

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IndexData methods																				
//
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxIndexData_CreateBuffer				(	const	IGN_STRUCT_IndexBufDesc&		a_IndexBufferDescription	);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxIndexData_DestroyBuffer				(			void														);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxIndexData_GetBufferDescription		(			IGN_STRUCT_IndexBufDesc&		a_IndexBufferDescription	);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxIndexData_LockBuffer					(			IGNUINT							a_uiOffset,
																										IGNUINT							a_uiSize,
																										IGNDWORD						a_dwLockFlags,
																										IGNUCHAR**						a_ppucharBytes				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxIndexData_UnlockBuffer				(			void														);

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Shader methods																				
//
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxShader_AttachMaterial					(			IGN_LP_OBJIF_GfxMaterial		a_pMaterialInterface		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxShader_GetAttachedMaterial			(			IGN_LP_OBJIF_GfxMaterial&		a_pMaterialInterface		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxShader_AttachVertexProgDcl			(			IGN_LP_OBJIF_GfxVertexProgDcl	a_pVertexProgDclInterface	);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxShader_GetAttachedVertexProgDcl		(			IGN_LP_OBJIF_GfxVertexProgDcl&	a_pVertexProgDclInterface	);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxShader_AttachVertexProgram			(			IGN_LP_OBJIF_GfxVertexProgram	a_pVertexProgramInterface	);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxShader_GetAttachedVertexProgram		(			IGN_LP_OBJIF_GfxVertexProgram&	a_pVertexProgramInterface	);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxShader_AttachPixelProgram				(			IGN_LP_OBJIF_GfxPixelProgram	a_pPixelProgramInterface	);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxShader_GetAttachedPixelProgram		(			IGN_LP_OBJIF_GfxPixelProgram&	a_pPixelProgramInterface	);

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Material methods																				
//
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMaterial_AttachTextureBlock			(			IGNU32							a_u32UnitNumber,
																										IGN_LP_OBJIF_GfxTextureBlock	a_pTextureBlockInterface	);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMaterial_GetAttachedTextureBlock		(			IGNU32							a_u32UnitNumber,
																										IGN_LP_OBJIF_GfxTextureBlock&	a_pTextureBlockInterface	);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMaterial_SetDiffuseReflection			(	const	IGN_STRUCT_ColorValue&			a_Color						);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMaterial_GetDiffuseReflection			(			IGN_STRUCT_ColorValue&			a_Color						);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMaterial_SetAmbientReflection			(	const	IGN_STRUCT_ColorValue&			a_Color						);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMaterial_GetAmbientReflection			(			IGN_STRUCT_ColorValue&			a_Color						);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMaterial_SetSpecularReflection		(	const	IGN_STRUCT_ColorValue&			a_Color,
																										IGNFLOAT						a_fPower					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMaterial_GetSpecularReflection		(			IGN_STRUCT_ColorValue&			a_Color,
																										IGNFLOAT&						a_fPower					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMaterial_SetEmission					(	const	IGN_STRUCT_ColorValue&			a_Color						);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMaterial_GetEmission					(			IGN_STRUCT_ColorValue&			a_Color						);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMaterial_SetColorFactor				(			IGNU32							a_u32Color					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMaterial_GetColorFactor				(			IGNU32&							a_u32Color					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMaterial_SetBlending					(			IGN_ENUM_BLENDING				a_SourceBlend,
																										IGN_ENUM_BLENDING				a_DestinationBlend			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMaterial_GetBlending					(			IGN_ENUM_BLENDING&				a_SourceBlend,
																										IGN_ENUM_BLENDING&				a_DestinationBlend			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMaterial_SetAlphaTesting				(			IGN_ENUM_ALPHATEST				a_AlphaTest,
																								const	IGN_STRUCT_AlphaTestData&		a_AlphaTestData				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMaterial_GetAlphaTesting				(			IGN_ENUM_ALPHATEST&				a_AlphaTest,
																										IGN_STRUCT_AlphaTestData&		a_AlphaTestData				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMaterial_SetAmbientMaterialSource		(	const	IGN_ENUM_MATERIALCOLORSRC		a_MaterialColorSrc			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMaterial_GetAmbientMaterialSource		(			IGN_ENUM_MATERIALCOLORSRC&		a_MaterialColorSrc			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMaterial_SetDiffuseMaterialSource		(	const	IGN_ENUM_MATERIALCOLORSRC		a_MaterialColorSrc			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMaterial_GetDiffuseMaterialSource		(			IGN_ENUM_MATERIALCOLORSRC&		a_MaterialColorSrc			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMaterial_SetSpecularMaterialSource	(	const	IGN_ENUM_MATERIALCOLORSRC		a_MaterialColorSrc			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMaterial_GetSpecularMaterialSource	(			IGN_ENUM_MATERIALCOLORSRC&		a_MaterialColorSrc			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMaterial_SetEmissiveMaterialSource	(	const	IGN_ENUM_MATERIALCOLORSRC		a_MaterialColorSrc			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxMaterial_GetEmissiveMaterialSource	(			IGN_ENUM_MATERIALCOLORSRC&		a_MaterialColorSrc			);

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// VertexProgDecl methods																		
//
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVertexProgDcl_CreateDeclaration		(	const	IGN_STRUCT_DclElement*			a_pDeclElement				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVertexProgDcl_DestroyDeclaration		(			void														);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVertexProgDcl_GetDeclaration			(			IGN_STRUCT_DclElement*			a_pDeclElement,
																										IGNUINT&						a_uiNumberOfElements		);

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// VertexProgram methods																		
//
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVertexProgram_CreateProgram			(			IGNHANDLE						a_hBinary,
																										IGNBOOL							a_bSoftwareProcessing		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVertexProgram_DestroyProgram			(			void														);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVertexProgram_SetProgramRegister		(			IGNU32							a_u32Register,
																								const	IGNFLOAT*						a_pfData,
																										IGNU32							a_u32DataCount				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVertexProgram_GetProgramRegister		(			IGNU32							a_u32Register,
																										IGNFLOAT*						a_pfData,
																										IGNU32							a_u32DataCount				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVertexProgram_ClearProgramRegister	(			IGNU32							a_u32Register,
																										IGNU32							a_u32DataCount				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVertexProgram_CreateDeclarationBuffer	(			IGNU32							a_u32NumberOfBytes			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVertexProgram_DestroyDeclarationBuffer(			void														);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxVertexProgram_GetDeclarationBuffer	(			void**							a_ppData,
																										IGNU32&							a_u32NumberOfBytes			);

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// PixelProgram methods											
//
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxPixelProgram_CreateProgram			(			IGNHANDLE						a_hBinary					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxPixelProgram_DestroyProgram			(			void														);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxPixelProgram_SetProgramRegister		(			IGNU32							a_u32Register,
																										IGNFLOAT*						a_pfData,
																										IGNU32							a_u32DataCount				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxPixelProgram_GetProgramRegister		(			IGNU32							a_u32Register,
																										IGNFLOAT*						a_pfData,
																										IGNU32							a_u32DataCount				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxPixelProgram_ClearProgramRegister		(			IGNU32							a_u32Register,
																										IGNU32							a_u32DataCount				);

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TextureBlock methods																				
//
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxTextureBlock_AttachImageData			(			IGN_LP_OBJIF_GfxImageData		a_pImageDataInterface		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxTextureBlock_GetAttachedImageData		(			IGN_LP_OBJIF_GfxImageData&		a_pImageDataInterface		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxTextureBlock_SetTextureAdressing		(			IGN_ENUM_TEXADDRESS				a_TexAddressU, 
																										IGN_ENUM_TEXADDRESS				a_TexAddressV, 
																										IGN_ENUM_TEXADDRESS				a_TexAddressW				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxTextureBlock_GetTextureAdressing		(			IGN_ENUM_TEXADDRESS&			a_TexAddressU, 
																										IGN_ENUM_TEXADDRESS&			a_TexAddressV, 
																										IGN_ENUM_TEXADDRESS&			a_TexAddressW				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxTextureBlock_SetTextureFiltering		(			IGN_ENUM_TEXFILTER				a_TexFilterMin,
																										IGN_ENUM_TEXFILTER				a_TexFilterMax,
																										IGN_ENUM_TEXFILTER				a_TexFilterMipMap			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxTextureBlock_GetTextureFiltering		(			IGN_ENUM_TEXFILTER&				a_TexFilterMin,
																										IGN_ENUM_TEXFILTER&				a_TexFilterMax,
																										IGN_ENUM_TEXFILTER&				a_TexFilterMipMap			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxTextureBlock_SetTextureCoordinateIndex(			IGNU32							a_u32TexCoordIndex			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxTextureBlock_GetTextureCoordinateIndex(			IGNU32&							a_u32TexCoordIndex			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxTextureBlock_SetColorCalc				(			IGN_ENUM_TEXOPERATION			a_TexOp,
																										IGN_ENUM_TEXARGUMENT			a_TexArg1,
																										IGN_ENUM_TEXARGUMENT			a_TexArg2					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxTextureBlock_GetColorCalc				(			IGN_ENUM_TEXOPERATION&			a_TexOp,
																										IGN_ENUM_TEXARGUMENT&			a_TexArg1,
																										IGN_ENUM_TEXARGUMENT&			a_TexArg2					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxTextureBlock_SetAlphaCalc				(			IGN_ENUM_TEXOPERATION			a_TexOp,
																										IGN_ENUM_TEXARGUMENT			a_TexArg1,
																										IGN_ENUM_TEXARGUMENT			a_TexArg2					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxTextureBlock_GetAlphaCalc				(			IGN_ENUM_TEXOPERATION&			a_TexOp,
																										IGN_ENUM_TEXARGUMENT&			a_TexArg1,
																										IGN_ENUM_TEXARGUMENT&			a_TexArg2					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxTextureBlock_SetTextureCoordinateCalc	(			IGN_ENUM_TEXCOORDCALC			a_TexCoordCalc				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxTextureBlock_GetTextureCoordinateCalc	(			IGN_ENUM_TEXCOORDCALC&			a_TexCoordCalc				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxTextureBlock_SetTextureCoordinateTrafo(			IGNU32							a_TexCoordTrafoFlags		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxTextureBlock_GetTextureCoordinateTrafo(			IGNU32&							a_TexCoordTrafoFlags		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxTextureBlock_SetTextureResultArgument	(			IGN_ENUM_TEXARGUMENT			a_ResultArg					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxTextureBlock_GetTextureResultArgument	(			IGN_ENUM_TEXARGUMENT&			a_ResultArg					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxTextureBlock_SetTextureMatrix			(	const	IGN_STRUCT_4x4Matrix&			a_Matrix					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxTextureBlock_GetTextureMatrix			(			IGN_STRUCT_4x4Matrix&			a_Matrix					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxTextureBlock_SetUnitNumber			(			IGNU32							a_u32Unit					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxTextureBlock_GetUnitNumber			(			IGNU32&							a_u32Unit					);

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ImageData methods																				
//
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxImageData_CreateLayers				(	const	IGN_STRUCT_LayersDesc&			a_LayersDescription			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxImageData_DestroyLayers				(			void														);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxImageData_GetLayersDescription		(			IGN_STRUCT_LayersDesc&			a_LayersDescription			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxImageData_LockLayer					(			IGNUINT							a_uiLayer,
																										IGN_ENUM_CUBEMAPFACE			a_CubeMapFace,
																										IGNDWORD						a_dwLockFlags,
																										IGNINT&							a_iRowPitch,
																										IGNINT&							a_iSlicePitch,
																										void**							a_ppBits					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxImageData_UnlockLayer					(			IGN_ENUM_CUBEMAPFACE			a_CubeMapFace,
																										IGNUINT							a_uiLayer					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxImageData_SetUnitNumber				(			IGNU32							a_u32Unit					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxImageData_GetUnitNumber				(			IGNU32&							a_u32Unit					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GfxImageData_GetRenderTargetHandle		(			IGN_ENUM_CUBEMAPFACE			a_CubeMapFace,
																										IGNHANDLE&						a_hSpecialImageRenderTarget	);




//------------- DECLARATIONS -----------------------------------------------------------------------------------------------------------------------------------------

struct	IGN_STRUCT_IFSubGraphic
{
	IGN_METHOD_GfxGetVersion*							m_pGetVersion;								// Pointer to GetVersion(..) method
	IGN_METHOD_GfxCreateInstance*						m_pCreateInstance;							// Pointer to CreateInstance(..) method
};

struct	IGN_STRUCT_GfxAssistMethods
{
	IGNBOOL												m_bGetAuxBuffer;							// Support status for GetAuxBuffer(..) method
	IGNBOOL												m_bAssembleProgram;							// Support status for AssembleProgram(..) method
	IGNBOOL												m_bAssembleProgramFromFile;					// Support status for AssembleProgramFromFile(..) method
	IGNBOOL												m_bComputeNormalMap;						// Support status for ComputeNormalMap(..) method
	IGNBOOL												m_bFillImageFromFile;						// Support status for FillImageFromFile(..) method
	IGNBOOL												m_bGenerateMipMaps;							// Support status for GenerateMipMaps(..) method
	IGNBOOL												m_bCreateBox;								// Support status for CreateBox(..) method
	IGNBOOL												m_bCreateCylinder;							// Support status for CreateCylinder(..) method
	IGNBOOL												m_bCreatePolygon;							// Support status for CreatePolygon(..) method
	IGNBOOL												m_bCreateSphere;							// Support status for CreateSphere(..) method
	IGNBOOL												m_bCreateTeapot;							// Support status for CreateTeapot(..) method
	IGNBOOL												m_bCreateText;								// Support status for CreateText(..) method
	IGNBOOL												m_bCreateTorus;								// Support status for CreateTorus(..) method
	IGNBOOL												m_bCreateSRTKexFrameInterpolator;			// Support status for CreateSRTKexFrameInterpolator(..) method
	IGNBOOL												m_bDestroySRTKexFrameInterpolator;			// Support status for DestroySRTKexFrameInterpolator(..) method
	IGNBOOL												m_bGetInterpolatorName;						// Support status for GetInterpolatorName(..) method
	IGNBOOL												m_bGetInterpolatorPeriod;					// Support status for GetInterpolatorPeriod(..) method
	IGNBOOL												m_bGetInterpolatedSRT;						// Support status for GetInterpolatedSRT(..) method
	IGNBOOL												m_bGetLastInterpolatedSRT;					// Support status for GetLastInterpolatedSRT(..) method
	IGNBOOL												m_bSaveImageToFile;							// Support status for SaveImageToFile(..) method
	IGNBOOL												m_bDrawText;								// Support status for DrawText(..) method
};

struct	IGN_STRUCT_GfxVersion
{
	IGNCHAR												m_pcharIdentifier[256];						// Module identifier string
	IGNCHAR												m_pcharVendor[256];							// Vendor string
	IGNU32												m_u32MainVersionNumber;						// Main version number of module
	IGNU32												m_u32SubVersionNumber;						// Sub version number of module
	IGNCHAR												m_pcharReserved[256];						// Reserved (not in use)
};

struct	IGN_STRUCT_VSHADERCAPS2_0
{
	IGNDWORD											m_dCaps;									// Instruction predication is supported if this value is nonzero
	IGNINT												m_iDynamicFlowControlDepth;					// The depth of the dynamic flow control instruction nesting
	IGNINT												m_iNumTemps;								// The number of temporary registers supported
	IGNINT												m_iStaticFlowControlDepth;					// The depth of nesting of the loop/rep and call/callnz instructs
};

struct	IGN_STRUCT_PSHADERCAPS2_0
{
	IGNDWORD											m_dCaps;									// Instruction predication is supported if this value is nonzero
	IGNINT												m_iDynamicFlowControlDepth;					// The depth of the dynamic flow control instruction nesting
	IGNINT												m_iNumTemps;								// The number of temporary registers supported
	IGNINT												m_iStaticFlowControlDepth;					// The depth of nesting of the loop/rep and call/callnz instructs
	IGNINT												m_iNumInstructionSlots;						// The number of instruction slots supported
};

struct	IGN_STRUCT_GfxAdapterCaps
{
	IGNDWORD											m_dCaps;									// The following driver-specific capability: READ_SCANLINE
	IGNDWORD											m_dCaps2;									// Driver-specific capabilities (see DirectX documentation)
	IGNDWORD											m_dCaps3;									// Driver-specific capabilities (see DirectX documentation)
	IGNDWORD											m_dPresentationIntervals;					// Representing what presentation swap intervals are available
	IGNDWORD											m_dCursorCaps;								// Indicating what hardware support is available for cursors
	IGNDWORD											m_dDevCaps;									// Flags identifying the capabilities of the device
	IGNDWORD											m_dPrimitiveMiscCaps;						// Miscellaneous driver primitive capabilities
	IGNDWORD											m_dRasterCaps;								// Information on raster-drawing capabilities
	IGNDWORD											m_dZCmpCaps;								// Z-buffer comparison capabilities
	IGNDWORD											m_dSrcBlendCaps;							// Source-blending capabilities
	IGNDWORD											m_dDestBlendCaps;							// Destination-blending capabilities
	IGNDWORD											m_dAlphaCmpCaps;							// Alpha-test comparison capabilities
	IGNDWORD											m_dShadeCaps;								// Shading operations capabilities
	IGNDWORD											m_dTextureCaps;								// Miscellaneous texture-mapping capabilities
	IGNDWORD											m_dTextureFilterCaps;						// Texture-filtering capabilities for a texture object
	IGNDWORD											m_dCubeTextureFilterCaps;					// Texture-filtering capabilities for a cube texture object
	IGNDWORD											m_dVolumeTextureFilterCaps;					// Texture-filtering capabilities for a volume texture object
	IGNDWORD											m_dTextureAddressCaps;						// Texture-addressing capabilities for texture objects
	IGNDWORD											m_dVolumeTextureAddressCaps;				// Texture-addressing capabilities for volume texture objects
	IGNDWORD											m_dLineCaps;								// Defines the capabilities for line-drawing primitives
	IGNDWORD											m_dMaxTextureWidth;							// Maximum texture width for this device
	IGNDWORD											m_dMaxTextureHeight;						// Maximum texture height for this device
	IGNDWORD											m_dMaxVolumeExtent;							// Maximum value for any of the three dimensions of a volume tex
	IGNDWORD											m_dMaxTextureRepeat;						// Maximum range of the integer bits of the texture coordinates
	IGNDWORD											m_dMaxTextureAspectRatio;					// Maximum texture aspect ratio supported by the hardware
	IGNDWORD											m_dMaxAnisotropy;							// Maximum valid value
	IGNFLOAT											m_fMaxVertexW;								// Maximum W-based depth value that the device supports
	IGNFLOAT											m_fGuardBandLeft;							// Screen-space coordinate of the guard-band clipping region
	IGNFLOAT											m_fGuardBandTop;							// Screen-space coordinate of the guard-band clipping region
	IGNFLOAT											m_fGuardBandRight;							// Screen-space coordinate of the guard-band clipping region
	IGNFLOAT											m_fGuardBandBottom;							// Screen-space coordinate of the guard-band clipping region
	IGNFLOAT											m_fExtentsAdjust;							// Number of pixels to adjust the extents rectangle outward 
	IGNDWORD											m_dStencilCaps;								// Flags specifying supported stencil-buffer operations
	IGNDWORD											m_dFVFCaps;									// Flexible vertex format capabilities
	IGNDWORD											m_dTextureOpCaps;							// Combination of supported flags describing the texture ops
	IGNDWORD											m_dMaxTextureBlendStages;					// Maximum number of texture-blending stages supported
	IGNDWORD											m_dMaxSimultaneousTextures;					// Maximum number of textures that can be simultaneously bound
	IGNDWORD											m_dVertexProcessingCaps;					// Vertex processing capabilities
	IGNDWORD											m_dMaxActiveLights;							// Maximum number of lights that can be active simultaneously
	IGNDWORD											m_dMaxUserClipPlanes;						// Maximum number of user-defined clipping planes supported
	IGNDWORD											m_dMaxVertexBlendMatrices;					// Maximum number of vertex blending matrices
	IGNDWORD											m_dMaxVertexBlendMatrixIndex;				// The maximum matrix index that can be indexed into
	IGNFLOAT											m_fMaxPointSize;							// Maximum size of a point primitive
	IGNDWORD											m_dMaxPrimitiveCount;						// Maximum number of primitives for each draw call
	IGNDWORD											m_dMaxVertexIndex;							// Maximum index supported for hardware vertex processing
	IGNDWORD											m_dMaxStreams;								// Maximum number of concurrent data streams
	IGNDWORD											m_dMaxStreamStride;							// Maximum stride of streams
	IGNDWORD											m_dVertexShaderVersion;						// Two numbers representing vertex shader main and sub versions
	IGNDWORD											m_dMaxVertexShaderConst;					// The number of vertex shader registers reserved for constants
	IGNDWORD											m_dPixelShaderVersion;						// Two numbers representing pixel shader main and sub versions
	IGNFLOAT											m_fMaxPixelShader1xValue;					// The number of pixel shader 1.x registers reserved for constants

	//
	// The following caps are valid only with DirectX 9.0 and higher
	//

	IGNDWORD											m_dDevCaps2;								// Device driver capabilities for adaptive tessellation
	IGNFLOAT											m_fMaxNPatchTesselationLevel;				// Maximum number of N-patch subdivision levels
	IGNFLOAT											m_fMinAntialiasedLineWidth;					// The minimum antialiased line width
	IGNFLOAT											m_fMaxAntialiasedLineWidth;					// The maximum antialiased line width
	IGNUINT												m_uiMasterAdapterOrdinal;					// Indicates which device is the master for this subordinate
	IGNUINT												m_uiAdapterOrdinalInGroup;					// Indicates the order in which heads are referenced
	IGNUINT												m_uiNumberOfAdaptersInGroup;				// Number of adapters in this adapter group (only if master)
	IGNDWORD											m_dDeclTypes;								// Data types contained in a vertex declaration
	IGNDWORD											m_dNumSimultaneousRTs;						// Number of simultaneous render targets
	IGNDWORD											m_dStretchRectFilterCaps;					// Combination of constants that describe the operations
	IGN_STRUCT_VSHADERCAPS2_0							m_VS20Caps;									// Device supports vertex shader version 2_0 extended capability
	IGN_STRUCT_PSHADERCAPS2_0							m_PS20Caps;									// Device supports pixel shader version 2_0 extended capability
	IGNDWORD											m_dVertexTextureFilterCaps;					// Device supports vertex shader texture filter capability
	IGNDWORD											m_dMaxVShaderInstructionsExecuted;			// Maximum number of vertex shader instructions that can be run
	IGNDWORD											m_dMaxPShaderInstructionsExecuted;			// Maximum number of vertex shader instructions that can be run
	IGNDWORD											m_dMaxVShader30InstructionSlots;			// Maximum number of vertex shader instruction slots supported
	IGNDWORD											m_dMaxPShader30InstructionSlots;			// Maximum number of pixel shader instruction slots supported
	IGNDWORD											m_dReserved2;								// Reserved (not in use)
	IGNDWORD											m_dReserved3;								// Reserved (not in use)
};

struct	IGN_STRUCT_GfxAdapterDesc
{
	IGNCHAR												m_cDriver[IGN_MAX_ADAPTER_ID_STRING];		// Used for presentation to the user
	IGNCHAR												m_cDescription[IGN_MAX_ADAPTER_ID_STRING];	// Used for presentation to the user
	IGNU64												m_u64DriverVersion;							// Identify the version of the Microsoft Direct3D® driver
	IGNU32												m_u32VendorId;								// Can be used to help identify a particular chip set
	IGNU32												m_u32DeviceId;								// Can be used to help identify a particular chip set
	IGNU32												m_u32SubSysId;								// Can be used to help identify a particular chip set
	IGNU32												m_u32Revision;								// Can be used to help identify a particular chip set
	IGN_STRUCT_GfxAdapterCaps							m_AdapterCaps;								// (see above)
};

struct	IGN_STRUCT_GfxModeDesc
{
	IGNU32												m_u32Width;									// Screen width, in pixels
	IGNU32												m_u32Height;								// Screen height, in pixels
	IGNU32												m_u32RefreshRate;							// Refresh rate. The value of 0 indicates an adapter default
	IGN_ENUM_PIXELFORMAT								m_Format;									// The surface format of the display mode
};

struct	IGN_STRUCT_GfxGaugeValues
{
	IGN_ENUM_GAUGEMODUS									m_GaugeModus;								// Current modus of the gauge
	struct
	{
		IGNU32											m_u32ScreenSwapCalls;						// Number of screen swap calls since last reset
		IGNU32											m_u32ViewportClearCalls;					// Number of viewport clear calls since last reset
	}													m_Simple;									// Simple values
	struct
	{
		IGNU32											m_u32CameraExecuteCalls;					// Number of camera execute calls
		IGNU32											m_u32VisualExecuteCalls;					// Number of visual execute calls
		IGNU32											m_u32PolyCount;								// Number of drawn polys
		IGNU32											m_u32LightExecuteCalls;						// Number of light execute calls
		IGNU32											m_u32FogExecuteCalls;						// Number of fog execute calls
		IGNU32											m_u32VertexDataExecuteCalls;				// Number of vertex data execute calls
		IGNU32											m_u32IndexDataExecuteCalls;					// Number of index data execute calls
		IGNU32											m_u32ShaderExecuteCalls;					// Number of shader execute calls
		IGNU32											m_u32MaterialExecuteCalls;					// Number of material execute calls
		IGNU32											m_u32VertexProgDclExecuteCalls;				// Number of vertex prog decl execute calls
		IGNU32											m_u32VertexProgramExecuteCalls;				// Number of vertex program execute calls
		IGNU32											m_u32PixelProgramExecuteCalls;				// Number of index program execute calls
		IGNU32											m_u32TextureBlockExecuteCalls;				// Number of texture block execute calls
		IGNU32											m_u32ImageDataExecuteCalls;					// Number of image data execute calls
	}													m_Standard;									// Standard values
	struct
	{
		IGNU32											m_u32TextureCreates;						// Number of texture creates
		IGNU32											m_u32CubemapCreates;						// Number of cubemap creates
		IGNU32											m_u32VolumeCreates;							// Number of volume creates
		IGNU32											m_u32VertexBufferCreates;					// Number of vertex buffer creates
		IGNU32											m_u32IndexBufferCreates;					// Number of index buffer creates
		IGNU32											m_u32VertexShaderCreates;					// Number of vertex shader creates
		IGNU32											m_u32PixelShaderCreates;					// Number of pixel shader creates
		IGNU32											m_u32TextureLocks;							// Number of texture locks
		IGNU32											m_u32CubemapLocks;							// Number of cubemap locks
		IGNU32											m_u32VolumeLocks;							// Number of volume locks
		IGNU32											m_u32VertexBufferLocks;						// Number of vertex buffer locks
		IGNU32											m_u32IndexBufferLocks;						// Number of index buffer locks
		IGNU32											m_u32RenderStateSwitches;					// Number of render state switches
		IGNU32											m_u32TextureStageStateSwitches;				// Number of texture stage state switches
		IGNU32											m_u32SamplerStateSwitches;					// Number of sampler state switches
		IGNU32											m_u32TextureSwitches;						// Number of texture switches
		IGNU32											m_u32CubemapSwitches;						// Number of cubemap switches
		IGNU32											m_u32VolumeSwitches;						// Number of volume switches
		IGNU32											m_u32VertexBufferSwitches;					// Number of vertex buffer switches
		IGNU32											m_u32IndexBufferSwitches;					// Number of index buffer switches
		IGNU32											m_u32VertexShaderSwitches;					// Number of vertex shader switches
		IGNU32											m_u32PixelShaderSwitches;					// Number of pixel shader switches
		IGNU32											m_u32RenderTargetSwitches;					// Number of render target switches
		IGNU32											m_u32DrawCalls;								// Number of draw calls
	}													m_Advanced;									// Advanced values
};

struct	IGN_STRUCT_GfxContextDesc
{
	IGNU32												m_u32AdapterNumber;							// Index of the adapter
	IGN_STRUCT_GfxAdapterDesc							m_AdapterDesc;								// (see above)
	IGNU32												m_u32ModeNumber;							// Index of the mode
	IGN_STRUCT_GfxModeDesc								m_ModeDesc;									// (see above)
};

struct	IGN_STRUCT_Viewport
{
	IGNU32												m_u32X;										// Pixel coordinate of the upper-left corner of the viewport
	IGNU32												m_u32Y;										// Pixel coordinate of the upper-left corner of the viewport
	IGNU32												m_u32Width;									// Width dimension of the clip volume, in pixels
	IGNU32												m_u32Height;								// Height dimension of the clip volume, in pixels
	IGNFLOAT											m_fMinZ;									// Describes the depth range into which a scene will be rendered
	IGNFLOAT											m_fMaxZ;									// Describes the depth range into which a scene will be rendered
};

struct	IGN_STRUCT_StencilData
{
	IGN_ENUM_STENCILOP									m_StencilFail;								// Stencil operation if the stencil test fails
	IGN_ENUM_STENCILOP									m_StencilZFail;								// Stencil operation if the stencil test passes and z-test fails
	IGN_ENUM_STENCILOP									m_StencilPass;								// Stencil operation if the stencil test and z-test passes
	IGN_ENUM_CMPFUNC									m_StencilFunc;								// Comparison function for the stencil test
	IGNU32												m_u32StencilRef;							// Reference value for the stencil test
	IGNU32												m_u32StencilMask;							// Determines the significant bits for the stencil test
	IGNU32												m_u32StencilWriteMask;						// Write mask applied to values written into the stencil buffer
};

struct	IGN_STRUCT_PlaneData
{
	IGNFLOAT											m_fA;										// Parameter A in A*x + B*y + C*z + D*w = 0
	IGNFLOAT											m_fB;										// Parameter B in A*x + B*y + C*z + D*w = 0
	IGNFLOAT											m_fC;										// Parameter C in A*x + B*y + C*z + D*w = 0
	IGNFLOAT											m_fD;										// Parameter D in A*x + B*y + C*z + D*w = 0
};

struct	IGN_STRUCT_BoxData
{
	IGNFLOAT											m_fWidth;									// Width of the box, along the x-axis
	IGNFLOAT											m_fHeight;									// Height of the box, along the y-axis
	IGNFLOAT											m_fDepth;									// Depth of the box, along the z-axis
};

struct	IGN_STRUCT_CylinderData
{
	IGNFLOAT											m_fRadius1;									// Radius at the negative Z end
	IGNFLOAT											m_fRadius2;									// Radius at the positive Z end
	IGNFLOAT											m_fLength;									// Length of the cylinder along the z-axis
	IGNUINT												m_uiSlices;									// Number of slices about the main axis
	IGNUINT												m_uiStacks;									// Number of stacks along the main axis
};

struct	IGN_STRUCT_PolygonData
{
	IGNFLOAT											m_fLength;									// Length of each side
	IGNUINT												m_uiSides;									// Number of sides for the polygon
};

struct	IGN_STRUCT_SphereData
{
 	IGNFLOAT											m_fRadius;									// Radius of the sphere
	IGNUINT												m_uiSlices;									// Number of slices about the main axis
	IGNUINT												m_uiStacks;									// Number of stacks along the main axis
};

struct	IGN_STRUCT_TextData
{
	HDC													m_hDC;										// Device context, containing the font for output
	LPCTSTR												m_pText;									// Pointer to a string that specifies the text to generate
	IGNFLOAT											m_fDeviation;								// Maximum chordal deviation from TrueType font outlines
	IGNFLOAT											m_fExtrusion;								// Amount to extrude text in the negative z-direction
};

struct	IGN_STRUCT_TorusData
{
	IGNFLOAT											m_fInnerRadius;								// Inner-radius of the torus
    IGNFLOAT											m_fOuterRadius;								// Outer-radius of the torus
    IGNUINT												m_uiSides;									// Number of sides in a cross-section
    IGNUINT												m_uiRings;									// Number of rings making up the torus
};

struct	IGN_STRUCT_DrawCallDesc
{
	IGNBOOL												m_bIndexedDrawCall;							// Indicates if primitive is based on indexing 
	IGN_ENUM_PRIMITIVETYPE								m_PrimitiveType;							// The type of primitive to render
	IGNU32												m_u32PrimitiveCount;						// Number of primitives to render
	IGNU32												m_u32StartVertex;							// Index of the first vertex to load
	IGNU32												m_u32MinIndex;								// Minimum vertex index for vertices used during this call
	IGNU32												m_u32NumberOfVertices;						// Number of vertices used during this call
	IGNU32												m_u32StartIndex;							// Location in the index array to start reading vertices
	IGNU32												m_u32BaseVertexIndex;						// Index buffer Offset to the first vertex index
};

struct	IGN_STRUCT_LightData
{
    IGN_ENUM_LIGHTTYPE									m_Type;										// Type of the light source
    IGN_STRUCT_ColorValue								m_Diffuse;									// Diffuse color emitted by the light
    IGN_STRUCT_ColorValue								m_Specular;									// Specular color emitted by the light
    IGN_STRUCT_ColorValue								m_Ambient;									// Ambient color emitted by the light
    IGN_STRUCT_3DVector									m_Position;									// Position of the light in world space
    IGN_STRUCT_3DVector									m_Direction;								// Direction that the light is pointing in world space
    IGNFLOAT											m_fRange;									// Distance beyond which the light has no effect
    IGNFLOAT											m_fFalloff;									// Decrease in illumination for spotlights 
    IGNFLOAT											m_fAttenuation0;							// Value specifying how the light intensity changes over distance
    IGNFLOAT											m_fAttenuation1;							// Value specifying how the light intensity changes over distance
    IGNFLOAT											m_fAttenuation2;							// Value specifying how the light intensity changes over distance
    IGNFLOAT											m_fTheta;									// Angle, in radians, of a spotlight's inner cone
    IGNFLOAT											m_fPhi;										// Angle, in radians, of a spotlight's outer cone
};

struct	IGN_STRUCT_FogData
{
	IGNBOOL												m_Enable;									// TRUE to enable fog blending
	IGNBOOL												m_RangeFogEnable;							// TRUE to enable range-based vertex fog
	IGNU32												m_Color;									// Fog color for both pixel and vertex fog
	IGN_ENUM_FOGTYPE									m_Type;										// Vertex or pixel fog
	IGN_ENUM_FOGMODE									m_Mode;										// Fog formula to be used
	IGNFLOAT											m_Start;									// Depth at which fog effects begin for linear fog mode
	IGNFLOAT											m_End;										// Depth at which fog effects end for linear fog mode
	IGNFLOAT											m_Density;									// Fog density for fog used in the exponential fog modes 
};

struct	IGN_STRUCT_VertexBufDesc
{
	IGNUINT												m_uiLength;									// Size of the vertex buffer, in bytes
	IGNDWORD											m_dwUsage;									// The usage for this resource
	IGNDWORD											m_dwFVF;									// The vertex format of the vertices in this buffer
	IGN_ENUM_POOLTYPE									m_Pool;										// The class of memory allocated for this vertex buffer
};

struct	IGN_STRUCT_IndexBufDesc
{
	IGNUINT												m_uiLength;									// Size of the index buffer, in bytes
	IGNDWORD											m_dwUsage;									// The usage for this resource
	IGN_ENUM_INDEXFORMAT								m_Format;									// The surface format of the index buffer data
	IGN_ENUM_POOLTYPE									m_Pool;										// The class of memory allocated for this index buffer
};

struct	IGN_STRUCT_AlphaTestData
{
	IGNU32												m_AlphaTestRef;								// Reference alpha value against which pixels are tested 
	IGN_ENUM_CMPFUNC									m_AlphaTestFunc;							// Accept or reject a pixel, based on its alpha value
};

struct	IGN_STRUCT_KeyFrameItpData
{
	LPCSTR												m_pName;									// A pointer to the name that identifies the interpolator
	IGN_STRUCT_3DKeyVector*								m_pScaleKeys;								// Each vector contains the scale at a single key frame
	IGNUINT												m_uiNumScaleKeys;							// The number of scale keys
	IGN_STRUCT_3DKeyQuaternion*							m_pRotationKeys;							// Each quaternion contains the rotation at a single key frame
	IGNUINT												m_uiNumRotationKeys;						// The number of rotation keys
	IGN_STRUCT_3DKeyVector*								m_pTranslateKeys;							// Each vector contains a translation vector at a single key frame
	IGNUINT												m_uiNumTranslationKeys;						// The number of translation keys
	IGNDOUBLE											m_dScaleInputTimeBy;						// The time scaling factor
};

struct	IGN_STRUCT_LayersDesc
{
	IGNUINT												m_uiWidth;									// Layer width, in texels
	IGNUINT												m_uiHeight;									// Layer height, in texels
	IGNUINT												m_uiDepth;									// Layer depth, in texels (for volumes)
	IGNBOOL												m_bCubeMap;									// Indicates a cube map
	IGNUINT												m_uiLevels;									// Number of mip map levels
	IGNDWORD											m_dwUsage;									// Usage flag
	IGN_ENUM_PIXELFORMAT								m_Format;									// Texel format
	IGN_ENUM_POOLTYPE									m_Pool;										// Memory pool type
};

struct	IGN_STRUCT_DclElement
{
	IGNWORD												m_wStream;									// Stream number
	IGNWORD												m_wOffset;									// Offset from the beginning of the stream to data start
	IGNU8												m_u8Type;									// Data type
	IGNU8												m_u8Method;									// Tessellator processing method
	IGNU8												m_u8Usage;									// Defines the intended use of the data
	IGNU8												m_u8UsageIndex;								// Allows the user to specify multiple usage types
};




//********************************************************************************************************************************************************************
//				Concrete object interfaces
//				See separate manual for detailed documentation
//********************************************************************************************************************************************************************
															
//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Instance interface - SINGLETON	
//
IGN_OBJ_INTERFACE_DECL_BaseObject( IGN_OBJIF_GfxInstance )
{
	//
	// Version 1.0.0
	//

	//
	// Retrieves a copy of the instance type
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxInstance_GetInstanceType,					GetInstanceType					);

	//
	// Retrieves a copy of the adapter and mode description of specified context
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxInstance_EnumContext,						EnumContext						);

	//
	// Retrieves a copy of the count of possible contexts
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxInstance_GetContextCount,					GetContextCount					);

	//
	// Creates a new IGN_OBJIF_GfxContext object
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxInstance_CreateContext,					CreateContext					);
};											
																
//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Context interface - SINGLETON	
//
IGN_OBJ_INTERFACE_DECL_BaseObject( IGN_OBJIF_GfxContext )
{
	//
	// Version 1.0.0
	//

	//
	// Creates a new IGN_OBJIF_GfxGauge object
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxContext_CreateGauge,						CreateGauge						);

	//
	// Creates a new IGN_OBJIF_GfxMixer object
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxContext_CreateMixer,						CreateMixer						);

	//
	// Creates a new IGN_OBJIF_GfxAssistant object
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxContext_CreateAssistant,					CreateAssistant					);

	//
	// Creates a new IGN_OBJIF_GfxCamera object
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxContext_CreateCamera,						CreateCamera					);

	//
	// Creates a new IGN_OBJIF_GfxVisual object
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxContext_CreateVisual,						CreateVisual					);

	//
	// Creates a new IGN_OBJIF_GfxLight object
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxContext_CreateLight,						CreateLight						);

	//
	// Creates a new IGN_OBJIF_GfxFog object
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxContext_CreateFog,						CreateFog						);

	//
	// Creates a new IGN_OBJIF_GfxVertexData object
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxContext_CreateVertexData,					CreateVertexData				);

	//
	// Creates a new IGN_OBJIF_GfxIndexData object
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxContext_CreateIndexData,					CreateIndexData					);

	//
	// Creates a new IGN_OBJIF_GfxShader object
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxContext_CreateShader,						CreateShader					);

	//
	// Creates a new IGN_OBJIF_GfxMaterial object
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxContext_CreateMaterial,					CreateMaterial					);

	//
	// Creates a new IGN_OBJIF_GfxVertexProgDcl object
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxContext_CreateVertexProgDcl,				CreateVertexProgDcl				);

	//
	// Creates a new IGN_OBJIF_GfxVertexProgram object
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxContext_CreateVertexProgram,				CreateVertexProgram				);

	//
	// Creates a new IGN_OBJIF_GfxPixelProgram object
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxContext_CreatePixelProgram,				CreatePixelProgram				);

	//
	// Creates a new IGN_OBJIF_GfxTextureBlock object
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxContext_CreateTextureBlock,				CreateTextureBlock				);

	//
	// Creates a new IGN_OBJIF_GfxImageData object
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxContext_CreateImageData,					CreateImageData					);

	//
	// Resets the device to new type, size, and format
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxContext_ToggleNewMode,					ToggleNewMode					);

	//
	// Detects if a device reset is necessary
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxContext_IsResetNecessary,					IsResetNecessary				);

	//
	// Resets the type, size, and format of the swap chain
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxContext_Reset,							Reset							);

	//
	// Sets the internal optimization type
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxContext_SetInternalOptimization,			SetInternalOptimization			);

	//
	// Gets a copy of the internal optimization type
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxContext_GetInternalOptimization,			GetInternalOptimization			);
};															
																
//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Gauge interface - SINGLETON
//
IGN_OBJ_INTERFACE_DECL_BaseObject( IGN_OBJIF_GfxGauge )
{
	//
	// Version 1.0.0
	//

	//
	// Resets all gauge measuring values
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxGauge_Reset,								Reset							);

	//
	// Sets the operation modus of the gauge
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxGauge_SetModus,							SetModus						);

	//
	// Retrieves all gauge measuring values
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxGauge_GetValues,							GetValues						);
};																
																
//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Mixer interface - SINGLETON
//
IGN_OBJ_INTERFACE_DECL_BaseObject( IGN_OBJIF_GfxMixer )
{
	//
	// Version 1.0.0
	//

	//
	// Sets the viewport parameters for the device
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_SetViewport,						SetViewport						);

	//
	// Retrieves a copy of the viewport parameters of the device
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_GetViewport,						GetViewport						);

	//
	// Clears the viewport to a specified RGBA color, clears the depth buffer, and erases the stencil buffer
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_ClearViewport,						ClearViewport					);

	//
	// Presents the contents of the next in the sequence of back buffers owned by the device
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_SwapScreen,							SwapScreen						);

	//
	// Begins a scene
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_BeginScene,							BeginScene						);

	//
	// Ends a scene that was begun by calling the IGN_OBJIF_Mixer::BeginScene method
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_EndScene,							EndScene						);

	//
	// Executes the specified IGN_OBJIF_Camera object
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_ExecuteCamera,						ExecuteCamera					);

	//
	// Executes the specified IGN_OBJIF_Visual object
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_ExecuteVisual,						ExecuteVisual					);

	//
	// Executes the specified IGN_OBJIF_Light object
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_ExecuteLight,						ExecuteLight					);

	//
	// Executes the specified IGN_OBJIF_Fog object
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_ExecuteFog,							ExecuteFog						);

	//
	// Retrieves a copy of the latest executed IGN_OBJIF_Camera object pointer
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_GetActiveCamera,					GetActiveCamera					);

	//
	// Retrieves a copy of the latest executed IGN_OBJIF_Visual object pointer
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_GetActiveVisual,					GetActiveVisual					);

	//
	// Retrieves a copy of the latest executed IGN_OBJIF_Light object pointer
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_GetActiveLight,						GetActiveLight					);

	//
	// Retrieves a copy of the latest executed IGN_OBJIF_Fog object pointer
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_GetActiveFog,						GetActiveFog					);

	//
	// Activates the specified master fill mode
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_SetMasterRenderMode,				SetMasterRenderMode				);

	//
	// Retrieves a copy of the active master fill mode
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_GetMasterRenderMode,				GetMasterRenderMode				);

	//
	// Activates the specified master shading mode
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_SetMasterShadeMode,					SetMasterShadeMode				);

	//
	// Retrieves a copy of the active master shading mode
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_GetMasterShadeMode,					GetMasterShadeMode				);

	//
	// Activates the specified master culling mode
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_SetMasterCullingMode,				SetMasterCullingMode			);

	//
	// Retrieves a copy of the active master culling mode
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_GetMasterCullingMode,				GetMasterCullingMode			);

	//
	// Activates the specified master depth testing
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_SetMasterDepthTesting,				SetMasterDepthTesting			);

	//
	// Retrieves a copy of the active master depth testing mode
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_GetMasterDepthTesting,				GetMasterDepthTesting			);

	//
	// Enables/disables master depth value writing to buffer
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_SetMasterDepthWrite,				SetMasterDepthWrite				);

	//
	// Retrieves a copy of the current master depth value writing modus
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_GetMasterDepthWrite,				GetMasterDepthWrite				);

	//
	// Enables/disables drawing to the rendering target surface on a pixel-by-pixel basis per stencil buffer
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_SetMasterStencilTesting,			SetMasterStencilTesting			);

	//
	// Retrieves a copy of the current stencil testing mode
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_GetMasterStencilTesting,			GetMasterStencilTesting			);

	//
	// Activates the specified master blending mode
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_SetMasterBlending,					SetMasterBlending				);

	//
	// Retrieves a copy of the active master blending mode
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_GetMasterBlending,					GetMasterBlending				);

	//
	// Activates the specified master alpha testing mode
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxMixer_SetMasterAlphaTesting,				SetMasterAlphaTesting			);

	//
	// Retrieves a copy of the active master alpha testing mode
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxMixer_GetMasterAlphaTesting,				GetMasterAlphaTesting			);

	//
	// Enables/disables the lighting
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxMixer_SetMasterLighting,					SetMasterLighting				);

	//
	// Retrieves a copy of the lighting status
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxMixer_GetMasterLighting,					GetMasterLighting				);

	//
	// Enables/disables the alpha blending
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxMixer_SetMasterAlphaBlending,				SetMasterAlphaBlending			);

	//
	// Retrieves a copy of the alpha blending status
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxMixer_GetMasterAlphaBlending,				GetMasterAlphaBlending			);

	//
	// Sets the master camera executable
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxMixer_SetMasterCameraExec,				SetMasterCameraExec				);

	//
	// Retrieves a copy of the master camera executable object pointer
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxMixer_GetMasterCameraExec,				GetMasterCameraExec				);

	//
	// Sets the master visual executable
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxMixer_SetMasterVisualExec,				SetMasterVisualExec				);

	//
	// Retrieves a copy of the master visual executable object pointer
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxMixer_GetMasterVisualExec,				GetMasterVisualExec				);

	//
	// Sets the master light executable
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxMixer_SetMasterLightExec,					SetMasterLightExec				);

	//
	// Retrieves a copy of the master light executable object pointer
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxMixer_GetMasterLightExec,					GetMasterLightExec				);

	//
	// Sets the master fog executable
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxMixer_SetMasterFogExec,					SetMasterFogExec				);

	//
	// Retrieves a copy of the master fog executable object pointer
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxMixer_GetMasterFogExec,					GetMasterFogExec				);

	//
	// Sets the master vertex data executable
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxMixer_SetMasterVertexDataExec,			SetMasterVertexDataExec			);

	//
	// Retrieves a copy of the master vertex data executable object pointer
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxMixer_GetMasterVertexDataExec,			GetMasterVertexDataExec			);

	//
	// Sets the master index data executable
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxMixer_SetMasterIndexDataExec,				SetMasterIndexDataExec			);

	//
	// Retrieves a copy of the master index data executable object pointer
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxMixer_GetMasterIndexDataExec,				GetMasterIndexDataExec			);

	//
	// Sets the master shader executable
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxMixer_SetMasterShaderExec,				SetMasterShaderExec				);

	//
	// Retrieves a copy of the master shader executable object pointer
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxMixer_GetMasterShaderExec,				GetMasterShaderExec				);

	//
	// Sets the master material executable
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxMixer_SetMasterMaterialExec,				SetMasterMaterialExec			);

	//
	// Retrieves a copy of the master material executable object pointer
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxMixer_GetMasterMaterialExec,				GetMasterMaterialExec			);

	//
	// Sets the master vertex program declaration executable
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxMixer_SetMasterVertexProgDclExec,			SetMasterVertexProgDclExec		);

	//
	// Retrieves a copy of the master vertex program declaration executable object pointer
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxMixer_GetMasterVertexProgDclExec,			GetMasterVertexProgDclExec		);

	//
	// Sets the master vertex program executable
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxMixer_SetMasterVertexProgramExec,			SetMasterVertexProgramExec		);

	//
	// Retrieves a copy of the master vertex program executable object pointer
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxMixer_GetMasterVertexProgramExec,			GetMasterVertexProgramExec		);

	//
	// Sets the master pixel program executable
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxMixer_SetMasterPixelProgramExec,			SetMasterPixelProgramExec		);

	//
	// Retrieves a copy of the master pixel program executable object pointer
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxMixer_GetMasterPixelProgramExec,			GetMasterPixelProgramExec		);

	//
	// Sets the master texture block executable
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxMixer_SetMasterTextureBlockExec,			SetMasterTextureBlockExec		);

	//
	// Retrieves a copy of the master texture block executable object pointer
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxMixer_GetMasterTextureBlockExec,			GetMasterTextureBlockExec		);

	//
	// Sets the master image data executable
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxMixer_SetMasterImageDataExec,				SetMasterImageDataExec			);

	//
	// Retrieves a copy of the master image data executable object pointer
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxMixer_GetMasterImageDataExec,				GetMasterImageDataExec			);

	//
	// Creates a new depth stencil buffer
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_CreateSpecialDepthBuffer,			CreateSpecialDepthBuffer		);

	//
	// Destroys the specified depth stencil buffer
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_DestroySpecialDepthBuffer,			DestroySpecialDepthBuffer		);

	//
	// Sets the specified render targets
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_SetRenderTarget,					SetRenderTarget					);

	//
	// Retrieves a copy of the special render target handles
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_GetRenderTarget,					GetRenderTarget					);

	//
	// Retrieves the vertex size of specified FVF
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_GetFVFSize,							GetFVFSize						);

	//
	// Enables/disables the clip plane with specified index (0 - 5)
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_SetClipPlane,						SetClipPlane					);

	//
	// Gets the status of the clip plane with specified index (0 - 5)
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_GetClipPlane,						GetClipPlane					);

	//
	// Sets the clip plane with specified index (0 - 5)
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_SetClipPlaneData,					SetClipPlaneData				);

	//
	// Gets the clip plane with specified index (0 - 5)
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMixer_GetClipPlaneData,					GetClipPlaneData				);
};
																
//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Assistant interface - SINGLETON
//
IGN_OBJ_INTERFACE_DECL_BaseObject( IGN_OBJIF_GfxAssistant )
{
	//
	// Version 1.0.0
	//

	//
	// Retrieves the support stati of all assistant methods
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxAssistant_GetMethodSupport,				GetMethodSupport				);

	//
	// Retrieves a pointer to the data in the buffer and the total size of the data in the buffer
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxAssistant_GetAuxBuffer,					GetAuxBuffer					);

	//
	// Assembles an ASCII description of a shader into binary form
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxAssistant_AssembleProgram,				AssembleProgram					);

	//
	// Assembles an ASCII description of a shader from file into binary form
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxAssistant_AssembleProgramFromFile,		AssembleProgramFromFile			);

	//
	// Converts a height map into a normal map 
	// The (x,y,z) components of each normal are mapped to the (r,g,b) channels of the output texture
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxAssistant_ComputeNormalMap,				ComputeNormalMap				);

	//
	// Loads a surface from a file
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxAssistant_FillImageFromFile,				FillImageFromFile				);

	//
	// Generates all mipmap levels
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxAssistant_GenerateMipMaps,				GenerateMipMaps					);

	//
	// Uses a left-handed coordinate system to create an axis-aligned box
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxAssistant_CreateBox,						CreateBox						);

	//
	// Uses a left-handed coordinate system to create a cylinder
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxAssistant_CreateCylinder,					CreateCylinder					);

	//
	// Uses a left-handed coordinate system to create an n-sided polygon
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxAssistant_CreatePolygon,					CreatePolygon					);

	//
	// Uses a left-handed coordinate system to create a sphere
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxAssistant_CreateSphere,					CreateSphere					);

	//
	// Uses a left-handed coordinate system to create a teapot
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxAssistant_CreateTeapot,					CreateTeapot					);

	//
	// Creates a mesh containing the specified text, using the font associated with the device context
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxAssistant_CreateText,						CreateText						);

	//
	// Uses a left-handed coordinate system to create a torus
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxAssistant_CreateTorus,					CreateTorus						);

	//
	// Creates a new interpolator
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxAssistant_CreateSRTKeyFrameInterpolator,	CreateSRTKeyFrameInterpolator	);

	//
	// Destroys the specified interpolator
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxAssistant_DestroySRTKeyFrameInterpolator,	DestroySRTKeyFrameInterpolator	);

	//
	// Get the name of the interpolator
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxAssistant_GetInterpolatorName,			GetInterpolatorName				);

	//
	// Get the interpolator period
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxAssistant_GetInterpolatorPeriod,			GetInterpolatorPeriod			);

	//
	// Returns the scale, rotation, and translation at a given time
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxAssistant_GetInterpolatedSRT,				GetInterpolatedSRT				);

	//
	// Returns the previous scale, rotation, and translation values
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxAssistant_GetLastInterpolatedSRT,			GetLastInterpolatedSRT			);

	//
	// Saves the specified image or the back buffer to file
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxAssistant_SaveImageToFile,				SaveImageToFile					);

	//
	// Draws the specified string as text
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxAssistant_DrawText,						DrawText						);
};														
									
//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Camera interface	
//
IGN_OBJ_INTERFACE_DECL_ExecutableObject( IGN_OBJIF_GfxCamera )
{
	//
	// Version 1.0.0
	//

	//
	// Saves the view transformation matrix
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxCamera_SetViewMatrix,						SetViewMatrix					);

	//
	// Retrieves a copy of the saved view transformation matrix
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxCamera_GetViewMatrix,						GetViewMatrix					);

	//
	// Saves the projection transformation matrix
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxCamera_SetProjectionMatrix,				SetProjectionMatrix				);

	//
	// Retrieves a copy of the saved projection transformation matrix
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxCamera_GetProjectionMatrix,				GetProjectionMatrix				);
};															
																
//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Visual interface	
//
IGN_OBJ_INTERFACE_DECL_ExecutableObject( IGN_OBJIF_GfxVisual )
{
	//
	// Version 1.0.0
	//

	//
	// Saves the attached vertex data interface
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVisual_AttachVertexData,					AttachVertexData				);

	//
	// Retrieves a copy of the saved attached vertex data interface
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVisual_GetAttachedVertexData,				GetAttachedVertexData			);

	//
	// Saves the attached index data interface
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVisual_AttachIndexData,					AttachIndexData					);

	//
	// Retrieves a copy of the saved attached index data interface
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVisual_GetAttachedIndexData,				GetAttachedIndexData			);

	//
	// Saves the attached shader interface
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVisual_AttachShader,						AttachShader					);

	//
	// Retrieves a copy of the saved attached shader interface
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVisual_GetAttachedShader,					GetAttachedShader				);

	//
	// Saves the render mode
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVisual_SetRenderMode,						SetRenderMode					);

	//
	// Retrieves the saved render mode
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVisual_GetRenderMode,						GetRenderMode					);

	//
	// Saves the shading mode
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVisual_SetShadeMode,						SetShadeMode					);

	//
	// Retrieves the saved shading mode
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVisual_GetShadeMode,						GetShadeMode					);

	//
	// Saves the culling mode
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVisual_SetCullingMode,					SetCullingMode					);

	//
	// Retrieves the saved culling mode
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVisual_GetCullingMode,					GetCullingMode					);

	//
	// Saves the depth testing mode
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVisual_SetDepthTesting,					SetDepthTesting					);

	//
	// Retrieves the saved depth testing mode
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVisual_GetDepthTesting,					GetDepthTesting					);

	//
	// Saves the depth writing mode
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVisual_SetDepthWrite,						SetDepthWrite					);

	//
	// Retrieves the saved depth writing mode
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVisual_GetDepthWrite,						GetDepthWrite					);

	//
	// Saves drawing mode to the rendering target surface on a pixel-by-pixel basis per stencil buffer
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVisual_SetStencilTesting,					SetStencilTesting				);

	//
	// Retrieves the saved drawing mode to the rendering target surface on a pixel-by-pixel basis per
	// stencil buffer
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVisual_GetStencilTesting,					GetStencilTesting				);

	//
	// Saves the world transformation matrix
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVisual_SetWorldMatrix,					SetWorldMatrix					);

	//
	// Retrieves a copy of the saved world transformation matrix
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVisual_GetWorldMatrix,					GetWorldMatrix					);

	//
	// Saves the draw call description
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVisual_SetDrawCallDescription,			SetDrawCallDescription			);

	//
	// Retrieves the saved draw call description
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVisual_GetDrawCallDescription,			GetDrawCallDescription			);
};																
																
//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Light interface	
//
IGN_OBJ_INTERFACE_DECL_ExecutableObject( IGN_OBJIF_GfxLight )
{
	//
	// Version 1.0.0
	//

	//
	// Saves the ambient lighting color
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxLight_SetAmbientLighting,					SetAmbientLighting				);

	//
	// Retrieves the saved ambient lighting color
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxLight_GetAmbientLighting,					GetAmbientLighting				);

	//
	// Saves the vertex lighting
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxLight_SetVertexLighting,					SetVertexLighting				);

	//
	// Retrieves the saved vertex lighting
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxLight_GetVertexLighting,					GetVertexLighting				);

	//
	// Saves the vertex lighting data
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxLight_SetVertexLightingData,				SetVertexLightingData			);

	//
	// Retrieves a copy of the saved vertex lighting data
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxLight_GetVertexLightingData,				GetVertexLightingData			);
};															
																
//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Fog interface	
//
IGN_OBJ_INTERFACE_DECL_ExecutableObject( IGN_OBJIF_GfxFog )
{
	//
	// Version 1.0.0
	//

	//
	// Saves the fog data
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxFog_SetFog,								SetFog							);

	//
	// Retrieves a copy of the saved fog data
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxFog_GetFog,								GetFog							);
};																
																
//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// VertexData interface	
//
IGN_OBJ_INTERFACE_DECL_ExecutableObject( IGN_OBJIF_GfxVertexData )
{
	//
	// Version 1.0.0
	//

	//
	// Creates a new vertex buffer
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVertexData_CreateBuffer,					CreateBuffer					);

	//
	// Destroys the vertex buffer
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVertexData_DestroyBuffer,					DestroyBuffer					);

	//
	// Retrieves the actual vertex buffer description
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVertexData_GetBufferDescription,			GetBufferDescription			);

	//
	// Locks the specified area in this vertex buffer
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVertexData_LockBuffer,					LockBuffer						);

	//
	// Unlocks the vertex buffer
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVertexData_UnlockBuffer,					UnlockBuffer					);

	//
	// Saves the specified stream number
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVertexData_SetStreamNumber,				SetStreamNumber					);

	//
	// Retrieves the saved stream number
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVertexData_GetStreamNumber,				GetStreamNumber					);
};																
																
//////////////////////////////////////////////////////////////////////////////////////////////////////
// IndexData interface	
IGN_OBJ_INTERFACE_DECL_ExecutableObject( IGN_OBJIF_GfxIndexData )
{
	//
	// Version 1.0.0
	//

	//
	// Creates a new index buffer
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxIndexData_CreateBuffer,					CreateBuffer					);

	//
	// Destroys the index buffer
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxIndexData_DestroyBuffer,					DestroyBuffer					);

	//
	// Retrieves the actual index buffer description
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxIndexData_GetBufferDescription,			GetBufferDescription			);

	//
	// Locks the specified area in this index buffer
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxIndexData_LockBuffer,						LockBuffer						);

	//
	// Unlocks the index buffer
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxIndexData_UnlockBuffer,					UnlockBuffer					);
};																
																
//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Shader interface	
//
IGN_OBJ_INTERFACE_DECL_ExecutableObject( IGN_OBJIF_GfxShader )
{
	//
	// Version 1.0.0
	//

	//
	// Saves the attached material interface
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxShader_AttachMaterial,					AttachMaterial					);

	//
	// Retrieves the saved attached material interface
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxShader_GetAttachedMaterial,				GetAttachedMaterial				);

	//
	// Saves the attached vertex program declaration interface
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxShader_AttachVertexProgDcl,				AttachVertexProgDcl				);

	//
	// Retrieves the saved attached vertex program declaration interface
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxShader_GetAttachedVertexProgDcl,			GetAttachedVertexProgDcl		);

	//
	// Saves the attached vertex program interface
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxShader_AttachVertexProgram,				AttachVertexProgram				);

	//
	// Retrieves the saved attached vertex program interface
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxShader_GetAttachedVertexProgram,			GetAttachedVertexProgram		);

	//
	// Saves the attached pixel program interface
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxShader_AttachPixelProgram,				AttachPixelProgram				);

	//
	// Retrieves the saved attached pixel program interface
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxShader_GetAttachedPixelProgram,			GetAttachedPixelProgram			);
};															
																
//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Material interface	
//
IGN_OBJ_INTERFACE_DECL_ExecutableObject( IGN_OBJIF_GfxMaterial )
{
	//
	// Version 1.0.0
	//

	//
	// Saves the attached texture block interface for specified unit number
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMaterial_AttachTextureBlock,				AttachTextureBlock				);

	//
	// Retrieves the saved texture block interface for specified unit number
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMaterial_GetAttachedTextureBlock,			GetAttachedTextureBlock			);

	//
	// Saves the diffuse reflection color
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMaterial_SetDiffuseReflection,			SetDiffuseReflection			);

	//
	// Retrieves the saved diffuse reflection color
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMaterial_GetDiffuseReflection,			GetDiffuseReflection			);

	//
	// Saves the ambient reflection color
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMaterial_SetAmbientReflection,			SetAmbientReflection			);

	//
	// Retrieves the saved ambient reflection color
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMaterial_GetAmbientReflection,			GetAmbientReflection			);

	//
	// Saves the specular reflection color and power
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMaterial_SetSpecularReflection,			SetSpecularReflection			);

	//
	// Retrieves the saved specular reflection color and power
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMaterial_GetSpecularReflection,			GetSpecularReflection			);

	//
	// Saves the emission color
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMaterial_SetEmission,						SetEmission						);

	//
	// Retrieves the saved emission color
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMaterial_GetEmission,						GetEmission						);

	//
	// Saves the color factor
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMaterial_SetColorFactor,					SetColorFactor					);

	//
	// Retrieves the saved color factor
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMaterial_GetColorFactor,					GetColorFactor					);

	//
	// Saves the blending mode
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMaterial_SetBlending,						SetBlending						);

	//
	// Retrieves the saved blending mode
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMaterial_GetBlending,						GetBlending						);

	//
	// Saves the alpha testing mode
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMaterial_SetAlphaTesting,					SetAlphaTesting					);

	//
	// Retrieves the saved alpha testing mode
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMaterial_GetAlphaTesting,					GetAlphaTesting					);

	//
	// Sets the ambient color source for lighting calculations
	//
	IGN_OBJ_METHOD( IGN_METHOD_GfxMaterial_SetAmbientMaterialSource,		SetAmbientMaterialSource		);

	//
	// Retrieves the ambient color source for lighting calculations
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMaterial_GetAmbientMaterialSource,		GetAmbientMaterialSource		);

	//
	// Sets the diffuse color source for lighting calculations
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMaterial_SetDiffuseMaterialSource,		SetDiffuseMaterialSource		);

	//
	// Retrieves the diffuse color source for lighting calculations
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMaterial_GetDiffuseMaterialSource,		GetDiffuseMaterialSource		);

	//
	// Sets the specular color source for lighting calculations
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMaterial_SetSpecularMaterialSource,		SetSpecularMaterialSource		);

	//
	// Retrieves the specular color source for lighting calculations
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMaterial_GetSpecularMaterialSource,		GetSpecularMaterialSource		);

	//
	// Sets the emissive color source for lighting calculations
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMaterial_SetEmissiveMaterialSource,		SetEmissiveMaterialSource		);

	//
	// Retrieves the emissive color source for lighting calculations
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxMaterial_GetEmissiveMaterialSource,		GetEmissiveMaterialSource		);
};																		
																
//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// VertexProgDcl interface	
//
IGN_OBJ_INTERFACE_DECL_ExecutableObject( IGN_OBJIF_GfxVertexProgDcl )
{
	//
	// Version 1.0.0
	//

	//
	// Creates a vertex shader declaration
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVertexProgDcl_CreateDeclaration,			CreateDeclaration				);

	//
	// Deletes the vertex shader declaration and frees up the associated resources
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVertexProgDcl_DestroyDeclaration,			DestroyDeclaration				);

	//
	// Retrieves the vertex shader declaration content
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVertexProgDcl_GetDeclaration,				GetDeclaration					);
};																
																
//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// VertexProgram interface	
//
IGN_OBJ_INTERFACE_DECL_ExecutableObject( IGN_OBJIF_GfxVertexProgram )
{
	//
	// Version 1.0.0
	//

	//
	// Creates a vertex shader
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVertexProgram_CreateProgram,				CreateProgram					);

	//
	// Deletes the vertex shader and frees up the associated resources
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVertexProgram_DestroyProgram,				DestroyProgram					);

	//
	// Saves the specified registers
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVertexProgram_SetProgramRegister,			SetProgramRegister				);

	//
	// Retrieves a copy of the specified saved registers
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVertexProgram_GetProgramRegister,			GetProgramRegister				);

	//
	// Clears the specified registers
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVertexProgram_ClearProgramRegister,		ClearProgramRegister			);

	//
	// Creates a buffer object
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVertexProgram_CreateDeclarationBuffer,	CreateDeclarationBuffer			);

	//
	// Destroys a buffer object
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVertexProgram_DestroyDeclarationBuffer,	DestroyDeclarationBuffer		);

	//
	// Retrieves a pointer to the data in the buffer and the total size of the data in the buffer
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxVertexProgram_GetDeclarationBuffer,		GetDeclarationBuffer			);
};															
																
//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// PixelProgram interface	
//
IGN_OBJ_INTERFACE_DECL_ExecutableObject( IGN_OBJIF_GfxPixelProgram )
{
	//
	// Version 1.0.0
	//

	//
	// Creates a pixel shader
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxPixelProgram_CreateProgram,				CreateProgram					);

	//
	// Deletes the pixel shader and frees up the associated resources
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxPixelProgram_DestroyProgram,				DestroyProgram					);

	//
	// Saves the specified registers
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxPixelProgram_SetProgramRegister,			SetProgramRegister				);

	//
	// Retrieves a copy of the specified saved registers
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxPixelProgram_GetProgramRegister,			GetProgramRegister				);

	//
	// Clears the specified registers
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxPixelProgram_ClearProgramRegister,		ClearProgramRegister			);
};																
																
//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TextureBlock interface	
//
IGN_OBJ_INTERFACE_DECL_ExecutableObject( IGN_OBJIF_GfxTextureBlock )
{
	//
	// Version 1.0.0
	//

	//
	// Saves the attached image data interface
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxTextureBlock_AttachImageData,				AttachImageData					);

	//
	// Retrieves the saved attached image data interface
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxTextureBlock_GetAttachedImageData,		GetAttachedImageData			);

	//
	// Saves the attached texture addressings
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxTextureBlock_SetTextureAdressing,			SetTextureAdressing				);

	//
	// Retrieves the saved attached texture addressings
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxTextureBlock_GetTextureAdressing,			GetTextureAdressing				);

	//
	// Saves the attached texture filters
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxTextureBlock_SetTextureFiltering,			SetTextureFiltering				);

	//
	// Retrieves the saved attached texture addressings
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxTextureBlock_GetTextureFiltering,			GetTextureFiltering				);

	//
	// Saves the attached texture coordinate index
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxTextureBlock_SetTextureCoordinateIndex,	SetTextureCoordinateIndex		);

	//
	// Retrieves the saved attached texture coordinate index
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxTextureBlock_GetTextureCoordinateIndex,	GetTextureCoordinateIndex		);

	//
	// Saves the attached color calculation operation and arguments
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxTextureBlock_SetColorCalc,				SetColorCalc					);

	//
	// Retrieves the saved attached color calculation operation and arguments
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxTextureBlock_GetColorCalc,				GetColorCalc					);

	//
	// Saves the attached alpha calculation operation and arguments
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxTextureBlock_SetAlphaCalc,				SetAlphaCalc					);

	//
	// Retrieves the saved attached alpha calculation operation and arguments
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxTextureBlock_GetAlphaCalc,				GetAlphaCalc					);

	//
	// Saves the attached texture coordinate calculation
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxTextureBlock_SetTextureCoordinateCalc,	SetTextureCoordinateCalc		);

	//
	// Retrieves the saved attached texture coordinate calculation
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxTextureBlock_GetTextureCoordinateCalc,	GetTextureCoordinateCalc		);

	//
	// Saves the attached texture coordinate trafo flags
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxTextureBlock_SetTextureCoordinateTrafo,	SetTextureCoordinateTrafo		);

	//
	// Retrieves the saved attached texture coordinate trafo flags
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxTextureBlock_GetTextureCoordinateTrafo,	GetTextureCoordinateTrafo		);

	//
	// Saves the attached texture trafo matrix
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxTextureBlock_SetTextureMatrix,			SetTextureMatrix				);

	//
	// Retrieves the saved attached texture trafo matrix
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxTextureBlock_GetTextureMatrix,			GetTextureMatrix				);

	//
	// Saves the attached select destination register
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxTextureBlock_SetTextureResultArgument,	SetTextureResultArgument		);

	//
	// Retrieves the saved attached select destination register
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxTextureBlock_GetTextureResultArgument,	GetTextureResultArgument		);

	//
	// Saves the unit number
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxTextureBlock_SetUnitNumber,				SetUnitNumber					);

	//
	// Retrieves the saved unit number
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxTextureBlock_GetUnitNumber,				GetUnitNumber					);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ImageData interface	
//
IGN_OBJ_INTERFACE_DECL_ExecutableObject( IGN_OBJIF_GfxImageData )
{
	//
	// Version 1.0.0
	//

	//
	// Creates a new texture
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxImageData_CreateLayers,					CreateLayers					);

	//
	// Destroys the texture
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxImageData_DestroyLayers,					DestroyLayers					);

	//
	// Retrieves the actual texture description
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxImageData_GetLayersDescription,			GetLayersDescription			);

	//
	// Locks the specified texture layer
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxImageData_LockLayer,						LockLayer						);

	//
	// Unlocks the specified texture layer
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxImageData_UnlockLayer,					UnlockLayer						);
	
	//
	// Saves the unit number
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxImageData_SetUnitNumber,					SetUnitNumber					);

	//
	// Retrieves the saved unit number
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxImageData_GetUnitNumber,					GetUnitNumber					);

	//
	// Retrieves the handle used for IGN_METHOD_Mixer_SetRenderTarget
	//
	IGN_OBJ_METHOD(	IGN_METHOD_GfxImageData_GetRenderTargetHandle,			GetRenderTargetHandle			);
};




//------------- PREPROCESSOR END -------------------------------------------------------------------------------------------------------------------------------------

#endif // IGN_GRAPHIC_SUB_SHARED_H_INCLUDED





