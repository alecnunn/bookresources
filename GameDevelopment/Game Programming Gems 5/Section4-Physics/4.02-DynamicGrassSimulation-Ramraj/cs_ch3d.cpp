//----------------------------------------------------------------------------
//
//cs_ch3d.cpp
//
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------

//··········································································//
//    Source :: Inclusions
//··········································································//

#include "mth_vector.h"
#include "mth_quaternion.h"
#include "mth_matrix.h"
#include "cs_ch3d.h"

//··········································································//
//    Source :: Definitions
//··········································································//

#define CH3D_NUM_PREVKEY_DEF 7

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

cs_ch3d_c::cs_ch3d_c(void)
{
	active = false;

	b_loop = true;
	b_anim = false;

	model = NULL;
	skeleton = NULL;
	joints = NULL;

	num_joints = 0;
	d_scale = 1;
}

cs_ch3d_c::~cs_ch3d_c(void)
{
	delete [] joints;
}

void cs_ch3d_c::reset(void)
{
	if(active)
	{
		b_loop = true;
		b_anim = false;
		d_scale = 1;
		num_joints = 0;
		delete [] joints;

		model = NULL;
		skeleton = NULL;
		joints = NULL;

		active = false;
	}
}

bool cs_ch3d_c::set_mdl(cs_am3d_c * mdl)
{
	reset();
	
	model = mdl;
	skeleton = mdl->get_skel();
	if(!skeleton)
		return false;
	
	b_anim = false;

	num_joints = skeleton->get_num_jts();
	joints = new ch3d_joint_t[num_joints];

	if(!joints)
		return false;

	int loop1;
	for(loop1 = 0; loop1 < num_joints; loop1 ++)
	{
		joints[loop1].curTime = 0;
		joints[loop1].curJoint = NULL;
		joints[loop1].nextJoint = NULL;
		joints[loop1].curTranslateKey = 0;
		joints[loop1].curRotateKey = 0;
		joints[loop1].deltaAnim = 0;
		joints[loop1].prevKey = NULL;
	}

	active = true;

	return true;
}

void cs_ch3d_c::set_anim(const kf3d_animation_t * animation, const float deltaAnim)
{
	if(!active || !animation)
		return;
	
	int loop1;

	b_anim = true;

	for(loop1 = 0; loop1 < animation->numJoints; loop1 ++)
	{
		joints[animation->Joints[loop1].index].deltaAnim = deltaAnim;
		joints[animation->Joints[loop1].index].nextJoint = &animation->Joints[loop1];
	}
}

void cs_ch3d_c::rset_anim(void)
{
	if(!active || !b_anim)
		return;
	
	b_anim = false;
	int loop1;

	for(loop1 = 0; loop1 < num_joints; loop1 ++)
	{
		joints[loop1].curJoint = NULL;
		joints[loop1].nextJoint = NULL;
	}
	
}

void cs_ch3d_c::update(const int milliseconds)
{
	word loop1;	

	if(b_anim)
	{
		float deltat = (float)milliseconds/(float)1000;
		deltat *= d_scale;

		float i_val = 0.0f;		
		mth_quat t_quat;
		mth_vector t_vec;

		kf3d_Translate_key_t * cur_t_key, * next_t_key;
		kf3d_Rotate_key_t * cur_r_key, * next_r_key;

		sk3d_joint_t * cur_sk_joint;
		kf3d_joint_t * cur_kf_joint;
		ch3d_joint_t * cur_ch_joint;

		for(loop1 = 0; loop1 < num_joints; loop1++)
		{
			cur_sk_joint = skeleton->get_jnt(loop1);
			if(!cur_sk_joint)
				return;
			cur_kf_joint = joints[loop1].curJoint;
			cur_ch_joint = &joints[loop1];

			cur_ch_joint->curTime += deltat;

			if(cur_ch_joint->nextJoint)
			{
				//no animation interpolation
				if( (!cur_ch_joint->curJoint) || ((cur_kf_joint->numTranslateKeys == 0) && (cur_kf_joint->numRotateKeys == 0)) )
				{
					cur_ch_joint->curJoint = cur_ch_joint->nextJoint;
					cur_ch_joint->nextJoint = 0;
					cur_ch_joint->curTime = 0;

					//append the parent's transformation
					if(cur_sk_joint->parent != -1)
					{
						mth_matrix_wrp mtx(cur_ch_joint->Final);
						mtx.set_matrix(joints[cur_sk_joint->parent].Final);
						mtx.set_multiply(cur_sk_joint->relative);
					}
					else
					{
						mth_matrix_wrp mtx(cur_ch_joint->Final);
						mtx.set_matrix(cur_sk_joint->relative);
					}

					continue;
				}				

				if(cur_ch_joint->prevKey)
				{
					if( cur_ch_joint->curTime >= cur_ch_joint->deltaAnim )
					{
						cur_ch_joint->curTime = 0;
						cur_ch_joint->curRotateKey = 0;
						cur_ch_joint->curTranslateKey = 0;
						
						delete [] cur_ch_joint->prevKey;
						cur_ch_joint->prevKey = 0;

						cur_ch_joint->curJoint = cur_ch_joint->nextJoint;
						cur_ch_joint->nextJoint = 0;					

						//dnf 2 process this joint
						continue;
					}

					//interpolate between the previous key and the next

					if( (cur_ch_joint->nextJoint->numRotateKeys == 0) && (cur_ch_joint->nextJoint->numTranslateKeys == 0) )
					{
						t_vec.set_vector(cur_ch_joint->prevKey);
						t_quat.set_quat(&cur_ch_joint->prevKey[3]);
					}
					else
					{
						//translation
	
						//calculate time delta
						if(cur_ch_joint->deltaAnim)
							i_val = cur_ch_joint->curTime / cur_ch_joint->deltaAnim;
						else
							i_val = 1.0f;

						t_vec.set_vector(
							cur_ch_joint->prevKey[0] + (cur_ch_joint->nextJoint->TranslateKeys[0].Translation[0] - cur_ch_joint->prevKey[0]) * i_val,
							cur_ch_joint->prevKey[1] + (cur_ch_joint->nextJoint->TranslateKeys[0].Translation[1] - cur_ch_joint->prevKey[1]) * i_val,
							cur_ch_joint->prevKey[2] + (cur_ch_joint->nextJoint->TranslateKeys[0].Translation[2] - cur_ch_joint->prevKey[2]) * i_val);

						//rotation
						t_quat.set_slerp(&cur_ch_joint->prevKey[3], cur_ch_joint->nextJoint->RotationKeys[0].Quaternion, i_val);
					}

					mth_matrix trans;
					t_quat.set_matrix(trans.get_mtx());
					trans.set_trans(t_vec.get_vec());

					//append the parent's transformation
					mth_matrix_wrp fin(cur_ch_joint->Final);					
					if(cur_sk_joint->parent != -1)
					{
						mth_matrix rel_fin(cur_sk_joint->relative);
						rel_fin.set_multiply(trans);

						fin.set_matrix(joints[cur_sk_joint->parent].Final);
						fin.set_multiply(rel_fin);						
					}
					else
					{
						fin.set_matrix(cur_sk_joint->relative);
						fin.set_multiply(trans);
					}
				}
				else
				{
					cur_ch_joint->prevKey = new float [CH3D_NUM_PREVKEY_DEF];

					if( cur_ch_joint->curTime > cur_kf_joint->animLen )
					{
						if(b_loop)
						{
							cur_ch_joint->curTime = 0;
							cur_ch_joint->curRotateKey = 0;
							cur_ch_joint->curTranslateKey = 0;
						}
						else
							cur_ch_joint->curTime = cur_kf_joint->animLen;						
					}

					//translation
					if( cur_ch_joint->curTime <= cur_kf_joint->TranslateKeys[0].Time )
						t_vec.set_vector(cur_kf_joint->TranslateKeys[0].Translation);					
					else if ( cur_ch_joint->curTime >= cur_kf_joint->TranslateKeys[cur_kf_joint->numTranslateKeys - 1].Time)
						t_vec.set_vector(cur_kf_joint->TranslateKeys[cur_kf_joint->numTranslateKeys - 1].Translation);
					else
					{
						while( (cur_ch_joint->curTranslateKey < cur_kf_joint->numTranslateKeys) &&
								(cur_kf_joint->TranslateKeys[cur_ch_joint->curTranslateKey].Time <
								cur_ch_joint->curTime) )
							cur_ch_joint->curTranslateKey++;				
	
						cur_t_key = & cur_kf_joint->TranslateKeys[cur_ch_joint->curTranslateKey - 1];
						next_t_key = & cur_kf_joint->TranslateKeys[cur_ch_joint->curTranslateKey];

						//calculate time delta
						i_val = next_t_key->Time - cur_t_key->Time;
						if(i_val)
							i_val = (float)( (cur_ch_joint->curTime - cur_t_key->Time)/i_val );

						t_vec.set_vector(
							cur_t_key->Translation[0] + (next_t_key->Translation[0] - cur_t_key->Translation[0]) * i_val,
							cur_t_key->Translation[1] + (next_t_key->Translation[1] - cur_t_key->Translation[1]) * i_val,
							cur_t_key->Translation[2] + (next_t_key->Translation[2] - cur_t_key->Translation[2]) * i_val);
					}
				
					//rotation
					if( cur_ch_joint->curTime <= cur_kf_joint->RotationKeys[0].Time )
						t_quat.set_quat(cur_kf_joint->RotationKeys[0].Quaternion);
					else if ( cur_ch_joint->curTime >= cur_kf_joint->RotationKeys[cur_kf_joint->numRotateKeys - 1].Time)
						t_quat.set_quat(cur_kf_joint->RotationKeys[cur_kf_joint->numRotateKeys - 1].Quaternion);					
					else
					{
						while( (cur_ch_joint->curRotateKey < cur_kf_joint->numRotateKeys) &&
							   (cur_kf_joint->RotationKeys[cur_ch_joint->curRotateKey].Time <
								cur_ch_joint->curTime) )
							cur_ch_joint->curRotateKey++;						
	
						cur_r_key  = & cur_kf_joint->RotationKeys[cur_ch_joint->curRotateKey - 1];
						next_r_key = & cur_kf_joint->RotationKeys[cur_ch_joint->curRotateKey];
	
						//calculate time delta
						i_val = next_r_key->Time - cur_r_key->Time;	
						if(i_val)
							i_val = (float)( (cur_ch_joint->curTime - cur_r_key->Time)/i_val );						
	
						t_quat.set_slerp(cur_r_key->Quaternion, next_r_key->Quaternion, i_val);
					}

					mth_matrix trans;
					t_quat.set_matrix(trans.get_mtx());
					trans.set_trans(t_vec.get_vec());

					//append the parent's transformation
					mth_matrix_wrp fin(cur_ch_joint->Final);
					if(cur_sk_joint->parent != -1)
					{
						mth_matrix rel_fin(cur_sk_joint->relative);
						rel_fin.set_multiply(trans);

						fin.set_matrix(joints[cur_sk_joint->parent].Final);
						fin.set_multiply(rel_fin);
					}
					else
					{
						fin.set_matrix(cur_sk_joint->relative);
						fin.set_multiply(trans);
					}

					//copy the previous keyframe data into the prevkey array
					memcpy(cur_ch_joint->prevKey,t_vec.get_vec(),sizeof(float) * 3);
					memcpy(&cur_ch_joint->prevKey[3],t_quat.get_quat(),sizeof(float) * 4);
					cur_ch_joint->curTime = 0;
				}//if(cur_ch_Joint->prevKey)
			}
			else
			{
				if( (!cur_ch_joint->curJoint) || ((cur_kf_joint->numTranslateKeys == 0) && (cur_kf_joint->numRotateKeys == 0)) )
				{
					cur_ch_joint->curTime = 0;

					//append the parent's transformation
					if(cur_sk_joint->parent != -1)
					{
						mth_matrix_wrp mtx(cur_ch_joint->Final);
						mtx.set_matrix(joints[cur_sk_joint->parent].Final);
						mtx.set_multiply(cur_sk_joint->relative);
					}
					else
					{
						mth_matrix_wrp mtx(cur_ch_joint->Final);
						mtx.set_matrix(cur_sk_joint->relative);
					}

					continue;
				}

				if( cur_ch_joint->curTime > cur_kf_joint->animLen )
				{
					if(b_loop)
					{
						cur_ch_joint->curTime = 0;
						cur_ch_joint->curRotateKey = 0;
						cur_ch_joint->curTranslateKey = 0;
					}
					else
						cur_ch_joint->curTime = cur_kf_joint->animLen;					
				}

				//translation

				if( cur_ch_joint->curTime <= cur_kf_joint->TranslateKeys[0].Time )
					t_vec.set_vector(cur_kf_joint->TranslateKeys[0].Translation);				
				else if ( cur_ch_joint->curTime >= cur_kf_joint->TranslateKeys[cur_kf_joint->numTranslateKeys - 1].Time)
					t_vec.set_vector(cur_kf_joint->TranslateKeys[cur_kf_joint->numTranslateKeys - 1].Translation);
				else
				{
					while( (cur_ch_joint->curTranslateKey < cur_kf_joint->numTranslateKeys) &&
						   (cur_kf_joint->TranslateKeys[cur_ch_joint->curTranslateKey].Time <
							cur_ch_joint->curTime) )
						cur_ch_joint->curTranslateKey++;					
	
					cur_t_key = & cur_kf_joint->TranslateKeys[cur_ch_joint->curTranslateKey - 1];
					next_t_key = & cur_kf_joint->TranslateKeys[cur_ch_joint->curTranslateKey];

					//calculate time delta
					i_val = next_t_key->Time - cur_t_key->Time;
					if(i_val)
						i_val = (float)( (cur_ch_joint->curTime - cur_t_key->Time)/i_val );

					t_vec.set_vector(
						cur_t_key->Translation[0] + (next_t_key->Translation[0] - cur_t_key->Translation[0]) * i_val,
						cur_t_key->Translation[1] + (next_t_key->Translation[1] - cur_t_key->Translation[1]) * i_val,
						cur_t_key->Translation[2] + (next_t_key->Translation[2] - cur_t_key->Translation[2]) * i_val);
				}
				
				//rotation

				if( cur_ch_joint->curTime <= cur_kf_joint->RotationKeys[0].Time )
					t_quat.set_quat(cur_kf_joint->RotationKeys[0].Quaternion);
				else if ( cur_ch_joint->curTime >= cur_kf_joint->RotationKeys[cur_kf_joint->numRotateKeys - 1].Time)				
					t_quat.set_quat(cur_kf_joint->RotationKeys[cur_kf_joint->numRotateKeys - 1].Quaternion);				
				else
				{
					while( (cur_ch_joint->curRotateKey < cur_kf_joint->numRotateKeys) &&
						   (cur_kf_joint->RotationKeys[cur_ch_joint->curRotateKey].Time <
							cur_ch_joint->curTime) )
						cur_ch_joint->curRotateKey++;					

					cur_r_key =  &cur_kf_joint->RotationKeys[cur_ch_joint->curRotateKey - 1];
					next_r_key = &cur_kf_joint->RotationKeys[cur_ch_joint->curRotateKey];

					//calculate time delta
					i_val = next_r_key->Time - cur_r_key->Time;

					if(i_val)
						i_val = (float)( (cur_ch_joint->curTime - cur_r_key->Time)/i_val );

					t_quat.set_slerp(cur_r_key->Quaternion, next_r_key->Quaternion, i_val);
				}				

				mth_matrix trans;
				t_quat.set_matrix(trans.get_mtx());
				trans.set_trans(t_vec.get_vec());

				//append the parent's transformation
				mth_matrix_wrp fin(cur_ch_joint->Final);
				if(cur_sk_joint->parent != -1)
				{
					mth_matrix rel_fin(cur_sk_joint->relative);
					rel_fin.set_multiply(trans);

					fin.set_matrix(joints[cur_sk_joint->parent].Final);
					fin.set_multiply(rel_fin);
				}
				else
				{
					fin.set_matrix(cur_sk_joint->relative);
					fin.set_multiply(trans);
				}
			}//if(cur_ch_joint->nextJoint)
		}//for(loop1 = 0; loop1 < num_joints; loop1++)
	}//if(b_anim)
	else
	{
		sk3d_joint_t * cur_sk_joint;
		ch3d_joint_t * cur_ch_joint;

		for(loop1 = 0; loop1 < num_joints; loop1++)
		{
			cur_sk_joint = skeleton->get_jnt(loop1);
			if(!cur_sk_joint)
				return;
			cur_ch_joint = &joints[loop1];

			cur_ch_joint->curTime = 0;

			//append the parent's transformation
			if(cur_sk_joint->parent != -1)
			{
				mth_matrix_wrp mtx(cur_ch_joint->Final);
				mtx.set_matrix(joints[cur_sk_joint->parent].Final);
				mtx.set_multiply(cur_sk_joint->relative);
			}
			else
			{
				mth_matrix_wrp mtx(cur_ch_joint->Final);
				mtx.set_matrix(cur_sk_joint->relative);
			}			
		}
	}
}

void cs_ch3d_c::render(const void (*gl_cmd)(word selected_t, word t_index)) const
{
	load_mdl_anim();
	if(gl_cmd)
		model->render(gl_cmd);
	else
		model->render();
}

am3d_sel_vertex_t cs_ch3d_c::get_vertex_anim(const word index)
{
	if(!active)
	{
		am3d_sel_vertex_t ret_val;
		ret_val.vertex[0] = 0.0f;
		ret_val.vertex[1] = 0.0f;
		ret_val.vertex[2] = 0.0f;
		return ret_val;
	}

	load_mdl_anim();
	return model->get_vertex(index,true);
}

cs_am3d_c * cs_ch3d_c::get_mdl(void) const
{
	return model;
}
cs_sk3d_c * cs_ch3d_c::get_skel(void) const
{
	return skeleton;
}

void cs_ch3d_c::set_anim(bool anim)
{
	b_anim = anim;
}
void cs_ch3d_c::set_loop(bool loop)
{
	b_loop = loop;
}
void cs_ch3d_c::set_scale(float scale)
{
	d_scale = scale;
}

void cs_ch3d_c::load_mdl_anim(void) const
{
	int loop1;

	for(loop1 = 0; loop1 < num_joints; loop1++)
	{
		model->set_joint(joints[loop1].Final,loop1);
	}
}

//··········································································//
//    Source :: Internal Function Implementation
//··········································································//

//··········································································//
//    Source :: Internal Class Implementation
//··········································································//

//----------------------------------------------------------------------------
//cs_ch3d.cpp
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------
