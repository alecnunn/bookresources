/**********************************************************************
 	MODULE: importMRC
	
	DESCRIPTION: This module is useful to put MRC data form file to an 
				 in memory C++ Object.
	
	
	DEVELOPED BY:		MARCO TOMBESI
	CONTACT:		e-mail:		baggior@libero.it		
					web-page:	digilander.iol.it/baggior

 *>	Copyright (c) Marco Tombesi, 2001
 **********************************************************************/

#include "stdafx.h"
#include "MRC.h"

//=========================================================================================
MRC_t::MRC_t()
{
	time_span=0;
	key_cnt=bone_cnt=norm_cnt=vert_cnt=face_cnt=0;
	Bones=NULL;norms=NULL;verts=NULL;faces=NULL;
}

MRC_t::~MRC_t()
{
	if (Bones) delete[] Bones;
	if (norms) delete[] norms;
	if (verts) delete[] verts;
	if (faces) delete[] faces;
}

//=========================================================================================
bone_t::bone_t()
{
	vert_cnt=child_cnt=parent_idx=0;
	memset(inverse_bone_TM,0,sizeof(inverse_bone_TM));
	key=NULL;
	childs=NULL;
	vert_weight=NULL;
}

bone_t::~bone_t()
{
	if (key) delete[] key;
	if (childs) delete[] childs;
	if (vert_weight) delete[] vert_weight;
}
//=========================================================================================