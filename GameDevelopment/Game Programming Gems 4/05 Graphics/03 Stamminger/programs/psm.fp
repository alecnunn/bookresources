//
// Perspective Shadow Maps
// accompanying example code for Game Programming Gems 4
// (w)(c) 2003, Carsten Dachsbacher (dachsbacher@cs.fau.de), Marc Stamminger (stamminger@cs.fau.de)
//
!!ARBfp1.0

##############
# TEMPORARIES

TEMP depthFromMap, 
     depthFragment, 
	 shadowFlag;

##############
# INPUTS

ATTRIB  shadowMapCoord = fragment.texcoord[ 0 ];

##############
# PROGRAM

# get depth value from shadowmap
TXP		depthFromMap, shadowMapCoord, texture[ 0 ], 2D;

# de-homogenize depth of fragment
RCP		depthFragment, shadowMapCoord.w;
MUL		depthFragment, depthFragment, shadowMapCoord;

# small offset
#ADD		depthFromMap, depthFromMap, 0.0025;
SGE		shadowFlag, depthFromMap.z, depthFragment.z;

MAD		result.color, shadowFlag, fragment.color, state.material.ambient;

END 
