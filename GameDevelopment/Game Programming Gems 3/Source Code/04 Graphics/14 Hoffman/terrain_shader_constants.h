// terrain shader constants
// Kenny Mitchell - Westwood Studios EA 2002

#ifndef TERRAIN_SHADER_CONSTANTS_H
#define TERRAIN_SHADER_CONSTANTS_H

// vertex shader macros

// constants
#define CV_ZERO								0
#define CV_ONE									1

#define CV_VIEW_PROJECTION					2

#define CV_VIEW_PROJECTION_0				2
#define CV_VIEW_PROJECTION_1				3
#define CV_VIEW_PROJECTION_2				4
#define CV_VIEW_PROJECTION_3				5

#define CV_BASE_TEX_PROJECTION			6

#define CV_CLOUD_TEX_PROJECTION_0		7
#define CV_CLOUD_TEX_PROJECTION_1		8

#define CV_FOG_PARAMETERS					9

// inputs
#define VPOSITION								v0


// pixel shader macros

// constants
#define CP_SKYLIGHT_FACTOR	c0
#define CP_SKYLIGHT_SLOT	0

// texture stages
#define TEX_TERRAIN_COLOR	t0
#define TEX_SUNLIGHT			t1
#define TEX_CLOUD_LAYER_0	t2
#define TEX_CLOUD_LAYER_1	t3

// registers
#define SKYLIGHT_REG			r0
#define OUTPUT_REG			r0

#define AVG_CLOUD_DENSITY	r1
#define CLOUD_SHADOW_LUM	r1

#endif