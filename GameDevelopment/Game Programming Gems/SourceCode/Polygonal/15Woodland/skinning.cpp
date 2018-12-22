/* Copyright (C) Ryan Woodland, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Ryan Woodland, 2000"
 */
struct Bone
{
	Mtx orientation;
	Mtx animation;
	Mtx inverseOrientation;

	Mtx final;

	Bone *child;
	Bone *sibling;
};

struct Vertex
{
	float s, t;
	float x, y, z;
	unsigned long color;

	unsigned long boneIndex1;
	unsigned long boneIndex2;

	float weight;
};


Vector3D TransformVertex ( Vertex *vert, Bone *boneArray )
{
	Vector3D	temp;
	Vector3D	final;

	temp = XFormVec(vert->position, bone[vert->boneIndex1]->final)
	final.x = temp.x * vert->weight;
	final.y = temp.y * vert->weight;
	final.z = temp.z * vert->weight;

	temp = XFormVec(vert->position, bone[vert->boneIndex2]->final)
	final.x += temp.x * (1.0F - vert->weight);
	final.y += temp.y * (1.0F - vert->weight);
	final.z += temp.z * (1.0F - vert->weight);

	return final;
}
