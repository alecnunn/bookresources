#ifndef __CS_KF3D_SPEC_H__
#define __CS_KF3D_SPEC_H__
//----------------------------------------------------------------------------
//
//cs_kf3d_spec.h
//
//Copyright © Rishi Ramraj, 2004
//kf3d spec
// The data structures are defined in the order as they appear in the .kf3d file.
// The .kf3d file is one of three types of files, which produce a modular system 
// suitable for games and 3d demos. Refer to the Character SDK 3D specifications
// document. Note that multiple animations from different skeletions can be stored
// in the same file.
//----------------------------------------------------------------------------

//··········································································//
//    header :: Inclusions
//··········································································//

//··········································································//
//    header :: Definitions
//··········································································//

#define CS_KF3D_MAX_NAME_LEN 32

//··········································································//
//    header :: Structures
//··········································································//

//Basic Types·······························································//

typedef struct
{
	float Time;
	float Translation[3];
}kf3d_Translate_key_t;

typedef struct
{
	float Time;
	//w, x, y, z
	float Quaternion[4];
}kf3d_Rotate_key_t;

typedef struct
{
	int index;
	float animLen;	
	int numTranslateKeys;
	kf3d_Translate_key_t * TranslateKeys;
	int numRotateKeys;
	kf3d_Rotate_key_t * RotationKeys;	
}kf3d_joint_t;

//Format····································································//

//
// number of Animations
//
// int numAnimations;

//
// size of the file in bytes starting after the number of animations
// this value is used for appending and should be ignored if loading this data
//
// long fsize;


typedef struct
{
	char Name[CS_KF3D_MAX_NAME_LEN];
	int numJoints;
	kf3d_joint_t * Joints;

}kf3d_animation_t;

//··········································································//
//    header :: Class Defs
//··········································································//

//··········································································//
//    header :: Function Defs
//··········································································//

//----------------------------------------------------------------------------
//cs_kf3d_spec.h
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------
#endif //__CS_KF3D_SPEC_H__