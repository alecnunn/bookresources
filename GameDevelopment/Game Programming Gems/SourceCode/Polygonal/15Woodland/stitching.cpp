/* Copyright (C) Ryan Woodland, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Ryan Woodland, 2000"
 */
struct Vertex
{
	float s, t;
	float x, y, z;
	unsigned long color;

	unsigned long boneIndex;
};

struct Bone
{
	Mtx orientation;
	Mtx animation;
	Mtx inverseOrientation;

	Mtx final;

	Bone *child;
	Bone *sibling;
};

void BuildMatrices ( Bone *bone, Mtx forward, Mtx orientation )
{
	Mtx localForward;
	Mtx localOrientation;

	// concatenate the hierarchy's orientation matrices so that we can 
	//   generate the inverse
	concatenate(bone->orientation, orientation, localOrientation);

	// take the inverse of the orientation matrix for this bone
	inverse(localOrientation, bone->inverseOrientation);

	// concatenate this bone's orientation onto the forward matrix
	concatenate(bone->orientation, forward, localForward);

	// concatenate this bone's animation onto the forward matrix
	concatenate(bone->animation, localForward, localForward);

	// build the bone's final matrix
	concatenate(bone->inverseOrientation, localForward, bone->final);

	if(bone->child)
		BuildMatrices(bone->child, localForward, localOrientation);

	if(bone->sibling)
		BuildMatrices(bone->sibling, forward, orientation);
}
