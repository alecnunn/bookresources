#ifndef __CS_SK3D_SPEC_H__
#define __CS_SK3D_SPEC_H__
//----------------------------------------------------------------------------
//
//cs_sk3d_spec.h
//
//Copyright © Rishi Ramraj, 2004
//sk3d spec
// The data structures are defined in the order as they appear in the .sk3d file.
// The .sk3d file is one of three types of files, which produce a modular system 
// suitable for games and 3d demos. Refer to the Character SDK 3D specifications
// document.
//----------------------------------------------------------------------------

//··········································································//
//    header :: Inclusions
//··········································································//

//··········································································//
//    header :: Definitions
//··········································································//

//··········································································//
//    header :: Structures
//··········································································//

//··········································································//
//    header :: Class Defs
//··········································································//

//··········································································//
//    header :: Function Defs
//··········································································//


//Basic Types·······························································//

#ifndef byte
typedef unsigned char byte;
#endif // byte

#ifndef word
typedef unsigned short word;
#endif // word

//Format····································································//

//
// number of joints
//
// word numJoints;

//
// numJoints * sizeof (sk3d_joint_t)
//
//This definition is commented to allow
//the spec user to define their own matrix implementation.
//Note that the inverse matrices are only needed
//until you apply them once to the animated model.
/*
typedef struct
{
	int			parent;
	float relative[16];
	float absolute[16];
	float absoluteinverse[16];
}sk3d_joint_t;
*/
//----------------------------------------------------------------------------
//cs_sk3d_spec.h
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------
#endif //__CS_SK3D_SPEC_H__