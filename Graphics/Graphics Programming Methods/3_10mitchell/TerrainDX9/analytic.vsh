// GPM - Pixel Shader Optimizations for Terrain Rendering Demo
// Kenny Mitchell
// Copyright Electronic Arts 2002, 2003

#include "analytic_shader_constants.h"

// vertex shader version 1.1
vs_1_1

// In:
//   v0 - world space vertex position

//  CV_FOG_PARAMETERS.x = fog start
//  CV_FOG_PARAMETERS.y = fog end
//  CV_FOG_PARAMETERS.z = range

// Out:
//	oPos - screen postion
//	oT0  - terrain color texture coordinates
//	oT1  - sun light texture coordinates
//	oT2  - cloud map texture coordinates
//	oT3  - cloud map texture coordinates
//	oFog - fog

dcl_position VPOSITION

// world space vertex position -> screen (early as possible for view clipping)
m4x4 oPos, VPOSITION, CV_VIEW_PROJECTION_0


// apply terrain color texture projection
mad oT0.xy, VPOSITION.xz, CV_BASE_TEX_PROJECTION.xy, CV_BASE_TEX_PROJECTION.zw

// apply terrain detail color texture projection
mad oT1.xy, VPOSITION.xz, CV_DETAIL_TEX_PROJECTION.xy, CV_DETAIL_TEX_PROJECTION.zw

// apply horizon & normal texture projection (quicker to re-calculate than store and copy result)
mad oT2.xy, VPOSITION.xz, CV_BASE_TEX_PROJECTION.xy, CV_BASE_TEX_PROJECTION.zw

// apply cloud texture projection for layer 0
mad oT3.xy, VPOSITION.xz, CV_CLOUD_TEX_PROJECTION.xy, CV_CLOUD_TEX_PROJECTION.zw




// calculate fog (fog is based on method by Phil Taylor, Microsoft)
dp4 r0, VPOSITION, CV_VIEW_PROJECTION_2 // recalculate cameraspace depth (z)

mad oFog, -r0.z, CV_FOG_PARAMETERS.x, CV_FOG_PARAMETERS.y
