#ifndef __CS_CH3D_H__
#define __CS_CH3D_H__
//----------------------------------------------------------------------------
//
//cs_ch3d.h
//
//Copyright © Rishi Ramraj, 2004
//Character wrapper
//----------------------------------------------------------------------------

//··········································································//
//    header :: Inclusions
//··········································································//

//INCLUDES
#include "cs_am3d.h"
#include "cs_sk3d.h"
#include "cs_kf3d.h"

//··········································································//
//    header :: Definitions
//··········································································//

//··········································································//
//    header :: Structures
//··········································································//

typedef struct
{
	float curTime;
	kf3d_joint_t * curJoint;
	kf3d_joint_t * nextJoint;
	int curTranslateKey, curRotateKey;

	float Final[16];

	//temporary storage for previous animations
	float deltaAnim;
	float * prevKey;
}ch3d_joint_t;

//··········································································//
//    header :: Class Defs
//··········································································//

class cs_ch3d_c
{
public:

	cs_ch3d_c(void);
	~cs_ch3d_c(void);
	
	bool is_active(void) const {return active;}
	void reset(void);
	bool set_mdl(cs_am3d_c * mdl);
	void set_anim(const kf3d_animation_t * animation, const float deltaAnim = 0);
	void rset_anim(void);
	void update(const int milliseconds);	
	void render(const void (*gl_cmd)(word selected_t, word t_index) = NULL) const;
	am3d_sel_vertex_t get_vertex_anim(const word index);

	cs_am3d_c * get_mdl(void) const;
	cs_sk3d_c * get_skel(void) const;

	void set_anim(bool anim);
	void set_loop(bool loop);
	void set_scale(float scale);

private:

	bool active;

	void load_mdl_anim(void) const;

	bool	b_loop, b_anim;
	float	d_scale;
	word	num_joints;

	cs_am3d_c		* model;
	cs_sk3d_c		* skeleton;	
	ch3d_joint_t	* joints;
};

//··········································································//
//    header :: Function Defs
//··········································································//

//----------------------------------------------------------------------------
//cs_ch3d.h
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------
#endif //__CS_CH3D_H__