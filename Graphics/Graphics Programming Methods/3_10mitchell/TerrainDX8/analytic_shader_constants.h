// GPM - Pixel Shader Optimizations for Terrain Rendering Demo
// Kenny Mitchell
// Copyright Electronic Arts 2002, 2003

#ifndef ANALYTIC_SHADER_CONSTANTS_H
#define ANALYTIC_SHADER_CONSTANTS_H

// vertex shader macros

// constants
#define CV_VIEW_PROJECTION					0

#define CV_VIEW_PROJECTION_0				0
#define CV_VIEW_PROJECTION_1				1
#define CV_VIEW_PROJECTION_2				2
#define CV_VIEW_PROJECTION_3				3

#define CV_CONST								8


#define CV_BASE_TEX_PROJECTION			9

#define CV_DETAIL_TEX_PROJECTION			10

#define CV_CLOUD_TEX_PROJECTION			11


#define CV_FOG_PARAMETERS					13

// inputs
#define VPOSITION								v0

#define REG_POSITION							r0


// pixel shader macros

// constants
#define CP_SKYLIGHT			c0
#define CP_SKYLIGHT_SLOT	0

#define CP_SUNLIGHT			c1
#define CP_SUNLIGHT_SLOT	1

#define CP_SUNCOLOR			c2
#define CP_SUNCOLOR_SLOT	2

// texture stages
#define TEX_TERRAIN_COLOR			t0
#define TEX_DETAIL_COLOR			t1
#define TEX_NORMAL_HORIZON_MAP	t2
#define TEX_CLOUD_LAYER				t3

// registers

#define OUTPUT_REG			r0

#define REG_SUNLIGHT			r0
#define REG_SHADOW			r1
#define REG_SKYLIGHT			r1
#define REG_TERRAIN_COLOR	r1

#endif
