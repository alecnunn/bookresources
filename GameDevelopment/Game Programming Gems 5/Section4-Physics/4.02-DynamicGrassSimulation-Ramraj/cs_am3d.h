#ifndef __CS_AM3D_H__
#define __CS_AM3D_H__
//----------------------------------------------------------------------------
//
//cs_am3d.h
//
//Copyright © Rishi Ramraj, 2004
//Animated model
//----------------------------------------------------------------------------

//··········································································//
//    header :: Inclusions
//··········································································//

#include <windows.h>
#include <gl/gl.h>
#pragma comment( lib, "opengl32.lib" )

#include "cs_sk3d.h"
#include "cs_am3d_spec.h"

//··········································································//
//    header :: Definitions
//··········································································//

#define AM3D_VERTEX_T_DEF			0
#define AM3D_VERTEX_MASK_T_DEF		1
#define AM3D_TRIANGLE_T_DEF			2
#define AM3D_TRIANGLE_MASK_T_DEF	3
#define AM3D_GROUP_T_DEF			4

//··········································································//
//    header :: Structures
//··········································································//

typedef struct
{
	float * final;
}am3d_joint_t;

typedef struct
{
	GLuint alpha;
    GLuint texture;
} am3d_material_t;

typedef struct
{
	float   vertex[3];
} am3d_sel_vertex_t;

//··········································································//
//    header :: Class Defs
//··········································································//

//··········································································//
//    header :: Function Defs
//··········································································//

class cs_am3d_c
{
public:				
	cs_am3d_c(void);
	~cs_am3d_c(void);

	bool is_active(void) const {return active;}
	bool load(const char * path);
	void unload(void);
	void render(void) const;
	void render(const void (*gl_cmd)(word selected_t, word t_index)) const;
	void render_no_anim(void) const;
	void render_no_anim(const void (*gl_cmd)(word selected_t, word t_index)) const;
	bool apply_skel(const cs_sk3d_c * newSkel);
	void set_joint(float * jnt, int index);
	cs_sk3d_c * get_skel(void) const;
	//delete returned memory once finished
	//creates an array of vertex indices that are selected
	word * gel_sel_vertices(int & num_vert) const;
	am3d_sel_vertex_t get_vertex(const word index, const bool anim = false) const;	

private:

	bool active;

	word num_vertices,
		 num_triangles,
		 num_groups,		 
		 num_textures,
		 num_joints,
		 grp_solid,
		 grp_textured,
		 grp_masked;

	am3d_vertex_t		* vertices;	
	am3d_triangle_t		* triangles;
	am3d_group_t		* groups;	
	am3d_material_t		* textures;	
	am3d_joint_t		* joints;
	cs_sk3d_c			* skeleton;
};

//----------------------------------------------------------------------------
//cs_am3d.h
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------
#endif //__CS_AM3D_H__