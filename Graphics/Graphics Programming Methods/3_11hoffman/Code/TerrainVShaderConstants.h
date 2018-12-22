#ifndef _TERRAIN_V_SHADER_CONSTANTS_H
#define _TERRAIN_V_SHADER_CONSTANTS_H

// constants

#define CV_WORLD_VIEW_PROJECTION		0
#define CV_WORLD_VIEW_PROJECTION_0		0
#define CV_WORLD_VIEW_PROJECTION_1		1
#define CV_WORLD_VIEW_PROJECTION_2		2
#define CV_WORLD_VIEW_PROJECTION_3		3

#define CV_WORLD_VIEW			4
#define CV_WORLD_VIEW_0			4
#define CV_WORLD_VIEW_1			5
#define CV_WORLD_VIEW_2			6
#define CV_WORLD_VIEW_3			7

#define CV_WORLD					8
#define CV_WORLD_0					8
#define CV_WORLD_1					9
#define CV_WORLD_2					10
#define CV_WORLD_3					11

#define CV_SUN_DIRECTION			12			//(.w = intensity)		
#define CV_SUN_COLOR				15
#define CV_TERRAIN_REFLECTANCE		13

#define CV_BETA_1					30
#define CV_BETA_2					29
#define CV_BETA_DASH_1				27
#define CV_BETA_DASH_2				26
#define CV_BETA_1_PLUS_2			24
#define CV_ONE_OVER_BETA_1_PLUS_2	23

#define CV_HG						28		// = 1-g^2, 1+g, 2g, 0
#define CV_CONSTANTS				31		// = 1.0, log_2 e, 0, 0
#define CV_TERM_MULTIPLIERS			25		// = frac_ext, frac_ins.

#define CV_ZERO						20
#define CV_ONE						21


#define CV_BASE_TEX_PROJECTION			16
#define CV_DETAIL_TEX_PROJECTION		17
#define CV_CLOUD_TEX_PROJECTION_0		18
#define CV_CLOUD_TEX_PROJECTION_1		19

#define CV_EYE_POSITION				22


// inputs
#define VPOSITION								v0




#endif