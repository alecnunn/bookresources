//----------------------------------------------------------------------------
//
//cs_sk3d.cpp
//
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------

//··········································································//
//    Source :: Inclusions
//··········································································//

#include <stdio.h>
#include "cs_sk3d.h"

//··········································································//
//    Source :: Definitions
//··········································································//

//··········································································//
//    Source :: Structures
//··········································································//

//··········································································//
//    Source :: Class Defs
//··········································································//

//··········································································//
//    Source :: Private Function Defs
//··········································································//

//··········································································//
//    Source :: Private Vars
//··········································································//

//··········································································//
//    Source :: External Function Implementation
//··········································································//

//··········································································//
//    Source :: External Class Implementation
//··········································································//

cs_sk3d_c::cs_sk3d_c()
{
	active = false;
	b_abs = false;

	num_joints = 0;
	joints = NULL;
	absolutes = NULL;
}

cs_sk3d_c::~cs_sk3d_c()
{
	delete[] joints;
	delete[] absolutes;
}

bool cs_sk3d_c::load(char* path)
{
	unload();
	
	FILE* file;
	int loop;
	
	if(!(file = fopen(path, "rb")))
		return false;	

	//read num_joints
	fread(&num_joints, sizeof(word), 1, file);

	joints = new sk3d_joint_t [num_joints];
	absolutes = new float [num_joints * 16];

	if((joints == NULL) || (absolutes == NULL))
		return false;

	for(loop=0; loop<num_joints; loop++)
	{
		//read the parent
		fread(&joints[loop].parent,sizeof(int),1,file);
		
		//read the Relative Matrix
		fread((void *)joints[loop].relative,sizeof(float),16,file);		

		//read the Absolute Matrix
		fread((void *)&(absolutes[loop * 16]),sizeof(float),16,file);		
	}

	if(fclose(file)!=0)
	{
		unload();
		return false;
	}

	active = true;
	b_abs = true;

	return true;
}

void cs_sk3d_c::unload(void)
{
	if(active)
	{
		active = false;
		b_abs = false;

		num_joints = 0;
		
		delete[] joints;
		delete[] absolutes;
		
		joints = NULL;
		absolutes = NULL;
	}
}

//Note: it is recomended that you delete the inverses once you have
//applied the skeleton to the models you desire. Do not delete the
//skeleton as it is referenced by the animation model class
void cs_sk3d_c::del_abs(void)
{
	if(!active || !b_abs)
		return;

	delete [] absolutes;
	absolutes = NULL;
	b_abs = false;
}

const float * cs_sk3d_c::get_abs(const word bone_id) const
{
	if(!active || !b_abs || bone_id >= num_joints)
		return 0;

	return &absolutes[bone_id * 16];
}
sk3d_joint_t * cs_sk3d_c::get_jnt(const word bone_id) const
{
	if(!active || bone_id >= num_joints)
		return 0;

	return &joints[bone_id];
}

//··········································································//
//    Source :: Internal Function Implementation
//··········································································//

//··········································································//
//    Source :: Internal Class Implementation
//··········································································//

//----------------------------------------------------------------------------
//cs_sk3d.cpp
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------
