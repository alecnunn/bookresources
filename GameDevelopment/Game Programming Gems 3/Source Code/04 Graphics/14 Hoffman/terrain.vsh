// Cloud mapping vertex shader
// Kenny Mitchell - Westwood Studios EA 2002

// vertex shader version 1.1
vs.1.1

#include "terrain_shader_constants.h"

// In:
//   v0 - world space vertex position

//  c[CV_FOG_PARAMETERS].x = fog start
//  c[CV_FOG_PARAMETERS].y = fog end
//  c[CV_FOG_PARAMETERS].z = range

// Out:
//   oPos - screen postion
//   oT0  - terrain color texture coordinates
//	  oT1  - sun light texture coordinates
//   oT2  - cloud map texture coordinates
//   oT3  - cloud map texture coordinates
//   oFog - fog

// world space vertex position -> screen (early as possible for view clipping)
dp4 oPos.x, VPOSITION, c[CV_VIEW_PROJECTION_0]
dp4 oPos.y, VPOSITION, c[CV_VIEW_PROJECTION_1]
dp4 oPos.z, VPOSITION, c[CV_VIEW_PROJECTION_2]
dp4 oPos.w, VPOSITION, c[CV_VIEW_PROJECTION_3]

// apply terrain color texture projection
mad oT0.xy, VPOSITION.xz, c[CV_BASE_TEX_PROJECTION].xy, c[CV_BASE_TEX_PROJECTION].zw

// apply sunlight texture projection (quicker to re-calculate than store and copy result)
mad oT1.xy, VPOSITION.xz, c[CV_BASE_TEX_PROJECTION].xy, c[CV_BASE_TEX_PROJECTION].zw

// apply cloud texture projection for layer 0
mad oT2.xy, VPOSITION.xz, c[CV_CLOUD_TEX_PROJECTION_0].xy, c[CV_CLOUD_TEX_PROJECTION_0].zw

// apply cloud texture projection for layer 1
mad oT3.xy, VPOSITION.xz, c[CV_CLOUD_TEX_PROJECTION_1].xy, c[CV_CLOUD_TEX_PROJECTION_1].zw



// calculate fog (fog is based on method by Greg James, NVIDIA)
dp4 r0, VPOSITION, c[CV_VIEW_PROJECTION_2] // recalculate cameraspace depth (z)

// scale by fog parameters:
add r0, r0, -c[CV_FOG_PARAMETERS].x // cameraspace depth (z) - fog start
rcp r0.y, c[CV_FOG_PARAMETERS].z // 1/range

// 1.0 - (z - fog start) * 1/range
// Because Fog = 1.0 means no fog, and fog = 0.0 means full fog
mad oFog.x, -r0.x, r0.y, c[CV_ONE].x
