#ifndef __CS_SK3D_H__
#define __CS_SK3D_H__
//----------------------------------------------------------------------------
//
//cs_sk3d.h
//
//Copyright © Rishi Ramraj, 2004
//Skeleton wrapper
//----------------------------------------------------------------------------

//··········································································//
//    header :: Inclusions
//··········································································//

#include "cs_sk3d_spec.h"

//··········································································//
//    header :: Definitions
//··········································································//

//··········································································//
//    header :: Structures
//··········································································//

typedef struct
{
	int			parent;
	float		relative[16];
}sk3d_joint_t;

//··········································································//
//    header :: Class Defs
//··········································································//

class cs_sk3d_c
{
public:

	cs_sk3d_c();
	~cs_sk3d_c();
	
	bool load(char* path);
	void unload(void);
	//delete the absolute matrices
	void del_abs(void);
	
	bool is_active(void) const {return active;}
	//absolutes have not been deleted
	bool has_abs(void) const {return b_abs;}	
	word get_num_jts(void) const {return num_joints;}

	const float * get_abs(const word bone_id) const;
	sk3d_joint_t * get_jnt(const word bone_id) const;
	
private:

	bool active;
	bool b_abs;

	word num_joints;

	sk3d_joint_t	* joints;
	float			* absolutes;
};

//··········································································//
//    header :: Function Defs
//··········································································//

//----------------------------------------------------------------------------
//cs_sk3d.h
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------
#endif //__CS_SK3D_H__