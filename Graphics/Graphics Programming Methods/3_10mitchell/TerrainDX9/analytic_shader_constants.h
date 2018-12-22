// GPM - Pixel Shader Optimizations for Terrain Rendering Demo
// Kenny Mitchell
// Copyright Electronic Arts 2002, 2003

#ifndef ANALYTIC_SHADER_CONSTANTS_H
#define ANALYTIC_SHADER_CONSTANTS_H

// vertex shader macros

// constants
#define CV_VIEW_PROJECTION_SLOT				0

#define CV_VIEW_PROJECTION_0				c0
#define CV_VIEW_PROJECTION_0_SLOT			0
#define CV_VIEW_PROJECTION_1				c1
#define CV_VIEW_PROJECTION_1_SLOT			1
#define CV_VIEW_PROJECTION_2				c2
#define CV_VIEW_PROJECTION_2_SLOT			2
#define CV_VIEW_PROJECTION_3				c3
#define CV_VIEW_PROJECTION_3_SLOT			3

#define CV_CONST							c8
#define CV_CONST_SLOT						8


#define CV_BASE_TEX_PROJECTION				c9
#define CV_BASE_TEX_PROJECTION_SLOT			9

#define CV_DETAIL_TEX_PROJECTION			c10
#define CV_DETAIL_TEX_PROJECTION_SLOT		10

#define CV_CLOUD_TEX_PROJECTION				c11
#define CV_CLOUD_TEX_PROJECTION_SLOT		11

#define CV_FOG_PARAMETERS					c13
#define CV_FOG_PARAMETERS_SLOT				13







// inputs
#define VPOSITION							v0

#define REG_POSITION						r0


// pixel shader macros

// constants
#define CP_CONST			c0
#define CP_CONST_SLOT		0

#define CP_SKYLIGHT			c1
#define CP_SKYLIGHT_SLOT	1

#define CP_SUNLIGHT			c2
#define CP_SUNLIGHT_SLOT	2

#define CP_SUNCOLOR			c3
#define CP_SUNCOLOR_SLOT	3

// texture registers
#define TEX_TERRAIN_COLOR			t0
#define TEX_DETAIL_COLOR			t1
#define TEX_NORMAL_HORIZON_MAP		t2
#define TEX_CLOUD_LAYER				t3

// texture samplers
#define SAMP_TERRAIN_COLOR			s0
#define SAMP_DETAIL_COLOR			s1
#define SAMP_NORMAL_HORIZON_MAP		s2
#define SAMP_CLOUD_LAYER			s3

// texture registers
#define REG_TERRAIN_COLOR			r0
#define REG_DETAIL_COLOR			r1
#define REG_NORMAL_HORIZON_MAP		r2
#define REG_CLOUD_LAYER				r3
#define REG_SKY_RADIANCE			r7
#define REG_SKY_TEMP				r8
#define REG_1_MINUS_TERRAIN			r9

// registers

#define OUTPUT_REG			r6

#define REG_SUNLIGHT		r4
#define REG_SHADOW			r5
#define REG_SKYLIGHT		r5

#endif
