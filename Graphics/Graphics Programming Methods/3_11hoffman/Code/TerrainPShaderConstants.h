#ifndef _TERRAIN_P_SHADER_CONSTANTS_H
#define _TERRAIN_P_SHADER_CONSTANTS_H

// constants


// pixel shader macros
#define VP_EXTINCTION v0
#define VP_INSCATTERING v1

// constants
#define CP_SUN_COLOR		0
#define CP_SUN_DIRECTION	1
#define CP_TERRAIN_REFLECTANCE 2



// texture stages
#define TEX_NORMAL_HORIZON_MAP		t0
#define TEX_TERRAIN_COLOR			t1
#define TEX_CLOUD_LAYER				t2

#define REG_T_NORMAL_HORIZON_MAP r0
#define REG_T_TERRAIN_COLOR		 r1
#define REG_T_CLOUD_LAYER		 r2


// registers

#define OUTPUT_REG			r0

#define REG_SUNLIGHT			r0
#define REG_SHADOW			r1
#define REG_SKYLIGHT			r1
#define REG_TERRAIN_COLOR	r1



#endif