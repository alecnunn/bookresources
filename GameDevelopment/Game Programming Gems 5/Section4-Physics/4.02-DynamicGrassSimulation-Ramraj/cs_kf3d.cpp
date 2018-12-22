//----------------------------------------------------------------------------
//
//cs_kf3d.cpp
//
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------

//··········································································//
//    Source :: Inclusions
//··········································································//

#include <stdio.h>
#include <string.h>

#include "cs_kf3d.h"

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

cs_kf3d_c::cs_kf3d_c(void)
{
	active = false;
	num_anims = 0;
	animations = 0;
}

cs_kf3d_c::~cs_kf3d_c(void)
{	
	int loop1, loop2;

	for( loop1 = 0; loop1 < num_anims; loop1++ )
	{
		for(loop2 = 0; loop2 < animations[loop1].numJoints; loop2++ )
		{
			delete [] animations[loop1].Joints[loop2].TranslateKeys;
			delete [] animations[loop1].Joints[loop2].RotationKeys;
		}

		delete [] animations[loop1].Joints;
	}

	delete [] animations;
}

bool cs_kf3d_c::load(char* path)
{
	unload();

	FILE *file;
	int loop1, loop2, loop3;
	kf3d_joint_t * curJoint;
	
	if((file= fopen(path, "rb"))==NULL)
		return false;	
	
	fread( &num_anims,sizeof(int),1,file);
	animations = new kf3d_animation_t[num_anims];
	if(!animations)
		return false;
	
	for( loop1 = 0; loop1 < num_anims; loop1++ )
	{
		fread(animations[loop1].Name, sizeof(char),CS_KF3D_MAX_NAME_LEN,file);
		fread(&animations[loop1].numJoints,sizeof(int),1,file);
		kf3d_joint_t * stuff = new kf3d_joint_t[animations[loop1].numJoints];
		//animations[loop1].Joints = new kf3d_joint_t[animations[loop1].numJoints];
		animations[loop1].Joints = stuff;
		if(!animations[loop1].Joints)
			return false;

		for(loop2 = 0; loop2 < animations[loop1].numJoints; loop2++ )
		{
			curJoint = &animations[loop1].Joints[loop2];
			fread(&curJoint->index,sizeof(int),1,file);
			fread(&curJoint->animLen,sizeof(float),1,file);
			fread(&curJoint->numTranslateKeys,sizeof(int),1,file);
			curJoint->TranslateKeys = new kf3d_Translate_key_t[curJoint->numTranslateKeys];
			if(curJoint->TranslateKeys == NULL)
				return false;

			for( loop3 = 0; loop3 < curJoint->numTranslateKeys; loop3 ++)
			{
				fread(&curJoint->TranslateKeys[loop3].Time,sizeof(float),1,file);
				fread(curJoint->TranslateKeys[loop3].Translation,sizeof(float),3,file);
			}
			
			fread(&curJoint->numRotateKeys,sizeof(int),1,file);
			curJoint->RotationKeys = new kf3d_Rotate_key_t[curJoint->numRotateKeys];
			if(curJoint->RotationKeys == NULL)
				return false;

			for( loop3 = 0; loop3 < curJoint->numRotateKeys; loop3 ++)
			{	
				fread(&curJoint->RotationKeys[loop3].Time,sizeof(float),1,file);
				fread(curJoint->RotationKeys[loop3].Quaternion,sizeof(float),4,file);
			}
		}
	}
	
	if(fclose(file)!=0)
	{
		unload();
		return false;
	}

	active = true;
	
	return true;
}

void cs_kf3d_c::unload(void)
{
	if(active)
	{
		int loop1, loop2;
		for( loop1 = 0; loop1 < num_anims; loop1++ )
		{
			for(loop2 = 0; loop2 < animations[loop1].numJoints; loop2++ )
			{
				delete [] animations[loop1].Joints[loop2].TranslateKeys;
				delete [] animations[loop1].Joints[loop2].RotationKeys;
			}

			delete [] animations[loop1].Joints;
		}

		delete [] animations;	
		num_anims = 0;
		animations = NULL;

		active = false;
	}
}

int cs_kf3d_c::get_anim_int(const char * name) const
{
	if(!active)
		return -1;

	int loop1;	
	for(loop1 = 0; loop1 < num_anims; loop1++ )
	{
		if(strcmp(name,animations[loop1].Name) == 0)
			return loop1;
	}
	return -1;
}

kf3d_animation_t * cs_kf3d_c::get_anim_ptr(const char * name) const
{
	if(!active)
		return NULL;

	int loop1;	
	for(loop1 = 0; loop1 < num_anims; loop1++ )
	{
		if(strcmp(name,animations[loop1].Name) == 0)
			return &animations[loop1];
	}
	return NULL;
}

kf3d_animation_t * cs_kf3d_c::get_anim_ptr(const int index) const
{
	if(active && (index < num_anims) && (index >= 0))
		return &animations[index];
	
	return NULL;
}

//··········································································//
//    Source :: Internal Function Implementation
//··········································································//

//··········································································//
//    Source :: Internal Class Implementation
//··········································································//

//----------------------------------------------------------------------------
//cs_kf3d.cpp
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------
