// Additional helper functions for MyRPC with SimpleTest
//
// Copyright 2004 Bae,Hyun-jik.  All rights reserved.
//
// Permission is granted to use this code for any purpose, as long as this
// copyright message remains intact.

#pragma once

#include "../MyRPC/MyRPC.h"

struct Vector3D
{
	float x,y,z;
};

inline void Message_Write(CMessage& m,const Vector3D &val)
{
	Message_Write(m,val.x);
	Message_Write(m,val.y);
	Message_Write(m,val.z);
}

inline void Message_Read(CMessage& m,Vector3D &val)
{
	Message_Read(m,val.x);
	Message_Read(m,val.y);
	Message_Read(m,val.z);
}