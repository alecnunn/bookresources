/**********************************************************************
 	MODULE: SkelDemo
	
	DESCRIPTION: This module is a demo of how to pratically use MRC data
				 to finally render the animation in an OpenGl window.				 	
	
	DEVELOPED BY:		MARCO TOMBESI
	CONTACT:		e-mail:		baggior@libero.it		
					web-page:	digilander.iol.it/baggior

 *>	Copyright (c) Marco Tombesi, 2001
 **********************************************************************/

#pragma once
#include "std.h"
#include "Bones.h"

class _Skin
{
public:
	float Color[3];

	_RootBone RootBone;		//Object skeleton

	// mesh data
	int VertCnt;
	int NormCnt;
	int FaceCnt;
	vect* Vert, * TempVert;
	vect* Norm;
	faceIdx* Face;

	_Skin();
	_Skin(const MRC_t* MRCdata);
	~_Skin();

	Update(ulong mSec);
private:
	UpdateTempData( _Bone &Bone);
};