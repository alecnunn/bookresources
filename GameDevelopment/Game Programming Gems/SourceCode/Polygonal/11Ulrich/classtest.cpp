/* Copyright (C) Thatcher Ulrich, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Thatcher Ulrich, 2000"
 */
// classtest.cpp	-thatcher 4/6/2000 Copyright Thatcher Ulrich

// Test program to compute some stats on classifying random objects
// in ordinary quadtree vs. loose quadtree.


#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/glut.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


const float	PI = 3.141592654;


const int	OBJECT_COUNT = 500;
const float	WORLD_SIZE = 1000.0;
const float	MAX_RADIUS = 30.0;
const float	MIN_RADIUS = 30.0;
const int	MAX_DEPTH = 5;


struct Object {
	float	x, z, radius;
	Object*	next;	// For linking into lists.
} ObjectList[OBJECT_COUNT];


const int	FRUSTUM_COUNT = 100;
struct Frustum {
	float	x, z;	// viewpoint
	float	nx[3], nz[3];	// Normals of the side lines.  They should point in towards the viewable area.
} FrustumList[FRUSTUM_COUNT];
int	FrustumTotals[FRUSTUM_COUNT];


int	DepthTotals[MAX_DEPTH];
int	NodesChecked = 0;
int	ObjectsActuallyInFrustum = 0;
int	ObjectsChecked = 0;

bool	ShowNodes = true;


struct qnode {
	qnode*	parent;
	qnode*	child[2][2];
	float	cx, cz;
	int	depth;
	Object*	objects;

	qnode(qnode* p, float x, float z, int d) {
		parent = p;
		cx = x;
		cz = z;
		depth = d;
		
		parent = 0;
		for (int j = 0; j < 2; j++) {
			for (int i = 0; i < 2; i++) {
				if (child[j][i]) child[j][i] = 0;
			}
		}
		objects = 0;
	}
	
	~qnode() {
		for (int j = 0; j < 2; j++) {
			for (int i = 0; i < 2; i++) {
				if (child[j][i]) delete child[j][i];
			}
		}
	}
};


bool	FitsInBox(Object* o, float cx, float cz, float HalfSize)
// Tests whether the given object can fit in the box centered at (cx, cz),
// with side dimensions of HalfSize * 2.
{
	if (o->x - o->radius < cx - HalfSize ||
	    o->x + o->radius > cx + HalfSize ||
	    o->z - o->radius < cz - HalfSize ||
	    o->z + o->radius > cz + HalfSize)
	{
		return false;
	} else {
		return true;
	}
}


void	RenderObject(Object* o)
// Draws the given object.
{
	static const int	CT = 50;
	static float	Coords[CT][2];
	static bool	Inited = false;

	if (!Inited) {
		for (int i = 0; i < CT; i++) {
			float	theta = float(i) / CT * 2 * PI;
			Coords[i][0] = cos(theta);
			Coords[i][1] = sin(theta);
		}
		
		Inited = true;
	}
	
	float	x, y, r;

	x = o->x;
	y = o->z;
	r = o->radius;

	int	inc = (int)((50 - o->radius) / 50 * (CT / 4));
	if (inc < 1) inc = 1;
	if (inc > CT/3) inc = CT/3;
	
	glBegin(GL_POLYGON /* TRIANGLE_FAN */);
	for (int i = 0; i < CT; i += inc) {
		glVertex3f(x - Coords[i][0] * r, y - Coords[i][1] * r, 1);
	}
	glEnd();
}


enum Visibility { NO_CLIP = 0, SOME_CLIP, NOT_VISIBLE };


Visibility	CheckBoxAgainstFrustum(float cx, float cz, float HalfSize, Frustum* f)
// Given the box centered at (cx,cz) and extending +/- HalfSize units
// along both axes, this function checks to see if the box overlaps the
// given frustum.  If the box is totally outside the frustum, then
// returns NOT_VISIBLE; if the box is totally inside the frustum, then
// returns NO_CLIP; otherwise, returns SOME_CLIP.
{
	int	OrCodes = 0, AndCodes = ~0;

	int	i, j, k;
	// Check each box corner against each edge of the frustum.
	for (j = 0; j < 2; j++) {
		for (i = 0; i < 2; i++) {
			int	mask = 1 << (j * 2 + i);
			
			float	x = cx + (i == 0 ? -HalfSize : HalfSize);
			float	z = cz + (j == 0 ? -HalfSize : HalfSize);

			// Check the corner against the two sides of the frustum.
			int	Code = 0;
			int	Bit = 1;
			for (k = 0; k < 3; k++, Bit <<= 1) {
				float	dot = f->nx[k] * (x - f->x) + f->nz[k] * (z - f->z);

				if (dot < 0) {
					// The point is outside this edge.
					Code |= Bit;
				}
			}

			OrCodes |= Code;
			AndCodes &= Code;
		}
	}

	// Based on bit-codes, return culling results.
	if (OrCodes == 0) {
		// The box is completely within the frustum (no box vert is outside any frustum edge).
		return NO_CLIP;
	} else if (AndCodes != 0) {
		// All the box verts are outside one of the frustum edges.
		return NOT_VISIBLE;
	} else {
		return SOME_CLIP;
	}
}


Visibility	CheckObjectAgainstFrustum(Object* o, Frustum* f)
// Returns the visibility of the object w/r/t the frustum.
{
	bool	In = true;
	bool	On = false;

	int	i;
	for (i = 0; i < 3; i++) {
		float	dist = f->nx[i] * (o->x - f->x) + f->nz[i] * (o->z - f->z);
		if (dist < - o->radius) {
			// Object is completely on wrong side of this frustum plane.
			In = false;
		} else if (dist > o->radius) {
			// Object is completely on right side of this plane.
		} else {
			// Object straddles the plane.
			On = true;
		}
	}

	if (In) {
		if (On) return SOME_CLIP;
		else return NO_CLIP;
	} else {
		return NOT_VISIBLE;
	}
}


bool	CheckObjectAgainstObject(Object* o, Object* p)
// Returns true if the objects overlap.
{
	float	dx = o->x - p->x;
	float	dz = o->z - p->z;
	float	r2 = dx * dx + dz * dz;

	if (r2 > (o->radius + p->radius) * (o->radius + p->radius)) {
		return false;
	} else {
		return true;
	}
}


//
// Ordinary quadtree functions.
//


int	QuadTreeInsert(qnode* q, Object* o)
// Insert the given object into the tree given by qnode.
// Returns the depth of the node the object was placed in.
{
	int	i, j;
	
	// Check child nodes to see if object fits in one of them.
	if (q->depth + 1 < MAX_DEPTH) {
		float	HalfSize = WORLD_SIZE / (2 << q->depth);
		float	QuarterSize = HalfSize / 2;
		float	offset = QuarterSize;
		
		for (j = 0; j < 2; j++) {
			for (i = 0; i < 2; i++) {
				float	cx = q->cx + ((i == 0) ? -offset : offset);
				float	cz = q->cz + ((j == 0) ? -offset : offset);

				if (FitsInBox(o, cx, cz, QuarterSize)) {
					// Recurse into this node.
					if (q->child[j][i] == 0) {
						q->child[j][i] = new qnode(q, cx, cz, q->depth + 1);
					}
					return QuadTreeInsert(q->child[j][i], /* cx, cz, depth + 1, */ o);
				}
			}
		}
	}

	// Keep object in this node.
	o->next = q->objects;
	q->objects = o;
	
	DepthTotals[q->depth]++;
	
	return q->depth;
}


int	CountNodes(qnode* q)
// Returns the number of nodes in the given subtree.
{
	int	count = 1;	// Count ourself.

	// Count descendants.
	int	i, j;
	for (j = 0; j < 2; j++) {
		for (i = 0; i < 2; i++) {
			if (q->child[j][i]) count += CountNodes(q->child[j][i]);
		}
	}

	return count;
}


int	QuadTreeCountObjectsInFrustum(qnode* q, Frustum* f, Visibility vis)
// Counts the objects in q which might touch the given frustum, based on
// the information in the quadtree partitioning.
{
	NodesChecked++;
	
	float	HalfSize = WORLD_SIZE / (2 << q->depth);
	
	if (vis == SOME_CLIP) {
		vis = CheckBoxAgainstFrustum(q->cx, q->cz, HalfSize, f);
		if (vis == NOT_VISIBLE) return 0;
	}

	int	count = 0;
	
	// Check children.
	int	j, i;
	for (j = 0; j < 2; j++) {
		for (i = 0; i < 2; i++) {
			if (q->child[j][i]) {
				count += QuadTreeCountObjectsInFrustum(q->child[j][i], f, vis);
			}
		}
	}

	// Count objects in this node and check frustum visibility.
	Object*	o = q->objects;
	while (o) {
		if (CheckObjectAgainstFrustum(o, f) != NOT_VISIBLE) ObjectsActuallyInFrustum++;
		
		count++;
		o = o->next;
	}

	return count;
}


void	RenderOrdinary(qnode* q, Frustum* f, Visibility vis)
// Render the objects in the frustum.
{
	float	HalfSize = WORLD_SIZE / (2 << q->depth);
	
	if (vis == SOME_CLIP) {
		vis = CheckBoxAgainstFrustum(q->cx, q->cz, HalfSize, f);
		if (vis == NOT_VISIBLE) return;
	}

	float	d = (q->depth / 5.0) + 0.2;
	if (d > 1) d = 1;

	if (ShowNodes) {
		// Draw this node.
		glColor3f(0, d, 0);
		glBegin(GL_POLYGON);
		glVertex2f(q->cx - HalfSize, q->cz - HalfSize);
		glVertex2f(q->cx + HalfSize, q->cz - HalfSize);
		glVertex2f(q->cx + HalfSize, q->cz + HalfSize);
		glVertex2f(q->cx - HalfSize, q->cz + HalfSize);
		glEnd();
	}
	
	// Render objects in this node.
	
	Object*	o = q->objects;
	while (o) {
		if (CheckObjectAgainstFrustum(o, f) != NOT_VISIBLE) {
			glColor3f(1, 1, 1);
			RenderObject(o);
		} else {
			glColor3f(0.5, 0.5, 0.5);
			RenderObject(o);
		}
		
		o = o->next;
	}

	// Render children.
	int	j, i;
	for (j = 0; j < 2; j++) {
		for (i = 0; i < 2; i++) {
			if (q->child[j][i]) {
				RenderOrdinary(q->child[j][i], f, vis);
			}
		}
	}
}


int	OrdinaryCountContactingObjects(qnode* q, Object* o)
// Returns the number of objects within the subtree q which contact
// the given object.  Increments ObjectsChecked for each object which
// is tested for contact.
{
	NodesChecked++;
	
	float	HalfSize = WORLD_SIZE / (2 << q->depth);
	
	// First check to see if the object is completely outside the boundary
	// of this node.
	float	dx = fabs(q->cx - o->x);
	float	dz = fabs(q->cz - o->z);
	if (dx - o->radius > HalfSize || dz - o->radius > HalfSize) {
		// Object is completely outside the boundary of this
		// node; don't bother checking contents.
		return 0;
	}

	int	count = 0;
	
	// Check children.
	int	j, i;
	for (j = 0; j < 2; j++) {
		for (i = 0; i < 2; i++) {
			if (q->child[j][i]) count += OrdinaryCountContactingObjects(q->child[j][i], o);
		}
	}

	// Check objects in this node.
	Object*	p = q->objects;
	while (p) {
		if (o != p && CheckObjectAgainstObject(o, p)) {
			count++;
		}
		
		ObjectsChecked++;
		p = p->next;
	}

	return count;
}


//
// Loose quadtree functions.
//


float	LooseK = 2;


int	LooseQuadTreeInsert(qnode* q, Object* o)
// Insert the given object into the tree given by qnode.
// Returns the depth of the node the object was placed in.
{
	int	i, j;
	
	// Check child nodes to see if object fits in one of them.
//	if (o->radius < WORLD_SIZE / (4 << q->depth)) {
	if (q->depth + 1 < MAX_DEPTH) {
		float	HalfSize = LooseK * WORLD_SIZE / (2 << q->depth);
		float	QuarterSize = HalfSize / 2;
		float	offset = (WORLD_SIZE / (2 << q->depth)) / 2;

		// Pick child based on classification of object's center point.
		int	i = (o->x <= q->cx) ? 0 : 1;
		int	j = (o->z <= q->cz) ? 0 : 1;

		float	cx = q->cx + ((i == 0) ? -offset : offset);
		float	cz = q->cz + ((j == 0) ? -offset : offset);

		if (FitsInBox(o, cx, cz, QuarterSize)) {
			// Recurse into this node.
			if (q->child[j][i] == 0) {
				q->child[j][i] = new qnode(q, cx, cz, q->depth + 1);
			}
			return LooseQuadTreeInsert(q->child[j][i], o);
		}
	}

	// Keep object in this node.
	o->next = q->objects;
	q->objects = o;
	
	DepthTotals[q->depth]++;
	
	return q->depth;
}


int	LooseQuadTreeCountObjectsInFrustum(qnode* q, Frustum* f, Visibility vis)
// Counts the objects in q which might touch the given frustum, based on
// the information in the loose quadtree partitioning.
{
	float	HalfSize = LooseK * WORLD_SIZE / (2 << q->depth);
	
	if (vis == SOME_CLIP) {
		vis = CheckBoxAgainstFrustum(q->cx, q->cz, HalfSize, f);
		if (vis == NOT_VISIBLE) return 0;
	}

	NodesChecked++;
	
	int	count = 0;
	
	// Check children.
	int	j, i;
	for (j = 0; j < 2; j++) {
		for (i = 0; i < 2; i++) {
			if (q->child[j][i]) {
				count += LooseQuadTreeCountObjectsInFrustum(q->child[j][i], f, vis);
			}
		}
	}

	// Count objects in this node.
	Object*	o = q->objects;
	while (o) {
		if (CheckObjectAgainstFrustum(o, f) != NOT_VISIBLE) ObjectsActuallyInFrustum++;
		
		count++;
		o = o->next;
	}

	return count;
}


void	RenderLoose(qnode* q, Frustum* f, Visibility vis)
// Render the objects in the frustum.
{
	float	HalfSize = LooseK * WORLD_SIZE / (2 << q->depth);
	
	if (vis == SOME_CLIP) {
		vis = CheckBoxAgainstFrustum(q->cx, q->cz, HalfSize, f);
		if (vis == NOT_VISIBLE) return;
	}

	float	d = (q->depth / 5.0) + 0.2;
	if (d > 1) d = 1;

	if (ShowNodes) {
		// Draw this node.
		glColor3f(0, d, 0);
		glBegin(GL_POLYGON);
		glVertex2f(q->cx - HalfSize, q->cz - HalfSize);
		glVertex2f(q->cx + HalfSize, q->cz - HalfSize);
		glVertex2f(q->cx + HalfSize, q->cz + HalfSize);
		glVertex2f(q->cx - HalfSize, q->cz + HalfSize);
		glEnd();
	}
	
	// Render objects in this node.
	
	Object*	o = q->objects;
	while (o) {
		if (CheckObjectAgainstFrustum(o, f) != NOT_VISIBLE) {
			glColor3f(1, 1, 1);
			RenderObject(o);
		} else {
			glColor3f(0.5, 0.5, 0.5);
			RenderObject(o);
		}
		
		o = o->next;
	}

	// Render children.
	int	j, i;
	for (j = 0; j < 2; j++) {
		for (i = 0; i < 2; i++) {
			if (q->child[j][i]) {
				RenderLoose(q->child[j][i], f, vis);
			}
		}
	}
}


int	LooseCountContactingObjects(qnode* q, Object* o)
// Returns the number of objects within the subtree q which contact
// the given object.  Increments ObjectsChecked for each object which
// is tested for contact.
{
	NodesChecked++;
	
	float	HalfSize = LooseK * WORLD_SIZE / (2 << q->depth);
	
	// First check to see if the object is completely outside the boundary
	// of this node.
	float	dx = fabs(q->cx - o->x);
	float	dz = fabs(q->cz - o->z);
	if (dx - o->radius > HalfSize || dz - o->radius > HalfSize) {
		// Object is completely outside the boundary of this
		// node; don't bother checking contents.
		return 0;
	}

	int	count = 0;
	
	// Check children.
	int	j, i;
	for (j = 0; j < 2; j++) {
		for (i = 0; i < 2; i++) {
			if (q->child[j][i]) count += LooseCountContactingObjects(q->child[j][i], o);
		}
	}

	// Check objects in this node.
	Object*	p = q->objects;
	while (p) {
		if (o != p && CheckObjectAgainstObject(o, p)) {
			count++;
		}
		
		ObjectsChecked++;
		p = p->next;
	}

	return count;
}


//
//
//


void	SetupFrustumNormals(Frustum* f, float ViewAngle, float FOV)
// Sets the normals of the frustum boundaries using the given parameters.
{
	f->nx[0] = cos(ViewAngle - FOV/2 + PI/2);
	f->nz[0] = -sin(ViewAngle - FOV/2 + PI/2);
	
	f->nx[1] = cos(ViewAngle + FOV/2 - PI/2);
	f->nz[1] = -sin(ViewAngle + FOV/2 - PI/2);

	f->nx[2] = cos(ViewAngle);
	f->nz[2] = -sin(ViewAngle);
}


void	PrintDepthTotals()
{
	printf("\nDepth Totals:\n");
	
	int	i;
	for (i = 0; i < MAX_DEPTH; i++) {
		printf("ct[%d] = %d\n", i, DepthTotals[i]);
	}
}


void	OpenOGL();
void	RunInteractive();


const float	DefaultFOV = PI / 4;


qnode*	qordinary = NULL;
qnode*	qloose = NULL;
Frustum	TestFrustum;
float	TestAngle = 0;
float	TestFOV = DefaultFOV;
bool	ShowLoose = true;


int	main()
{
	OpenOGL();
	
	int	i;

	//
	// Generate a list of random objects.
	//
	for (i = 0; i < OBJECT_COUNT; i++) {
		// Generate a random object.
		Object&	o = ObjectList[i];
		float	rn = rand() / float(RAND_MAX);

		// We want the area covered by our range of objects to be roughly constant w/r/t object radius.
		if (rn < 0.001) rn = 0.001;
		o.radius = (1 / sqrt(1000 * rn)) * (MAX_RADIUS - MIN_RADIUS) + MIN_RADIUS;
		
		o.x = (rand() / float(RAND_MAX) - 0.5) * 2 * (WORLD_SIZE/2 - o.radius);
		o.z = (rand() / float(RAND_MAX) - 0.5) * 2 * (WORLD_SIZE/2 - o.radius);
	}

	//
	// Generate a list of random frustums.
	//
	for (i = 0; i < FRUSTUM_COUNT; i++) {
		Frustum&	f = FrustumList[i];
		f.x = (rand() / float(RAND_MAX) - 0.5) * WORLD_SIZE;
		f.z = (rand() / float(RAND_MAX) - 0.5) * WORLD_SIZE;
		float	theta = (rand() / float(RAND_MAX)) * 2 * PI;

		SetupFrustumNormals(&f, theta, DefaultFOV);
	}

	TestFrustum.x = 0;
	TestFrustum.z = 0;
	SetupFrustumNormals(&TestFrustum, TestAngle, TestFOV);
	
	//
	// Classify objects into ordinary quadtree.
	//
	
	// Clear the depth totals.
	for (i = 0; i < MAX_DEPTH; i++) {
		DepthTotals[i] = 0;
	}

	qordinary = new qnode(0, 0, 0, 0);
	for (i = 0; i < OBJECT_COUNT; i++) {
		Object&	o = ObjectList[i];
		int	d = QuadTreeInsert(qordinary, /* 0, 0, 0, */ &o);
	}

	// print some stats.
	PrintDepthTotals();
	printf("Node count = %d\n", CountNodes(qordinary));

	// Do some frustum-like queries, and count the number of potential hits.
	NodesChecked = 0;
	ObjectsActuallyInFrustum = 0;
	int	ct = 0;
	for (i = 0; i < FRUSTUM_COUNT; i++) {
		ct += QuadTreeCountObjectsInFrustum(qordinary, &FrustumList[i], SOME_CLIP);
	}
	printf("%d objects possibly in frustum, %d objects actually in frustum, %d nodes checked.\n", ct, ObjectsActuallyInFrustum, NodesChecked);

	// Do contact checks, and count the number of objects tested.
	ObjectsChecked = 0;
	NodesChecked = 0;
	int	Contacts = 0;
	for (i = 0; i < OBJECT_COUNT; i++) {
		Contacts += OrdinaryCountContactingObjects(qordinary, &ObjectList[i]);
	}
	printf("%d contacts, %d objects checked, %d nodes checked\n", Contacts, ObjectsChecked, NodesChecked);
	
	//
	// Classify objects into loose quadtree.
	//

	// Clear the depth totals.
	for (i = 0; i < MAX_DEPTH; i++) {
		DepthTotals[i] = 0;
	}

	qloose = new qnode(0, 0, 0, 0);
	for (i = 0; i < OBJECT_COUNT; i++) {
		Object&	o = ObjectList[i];
		int	d = LooseQuadTreeInsert(qloose, /* 0, 0, 0, */ &o);
	}

	// print some stats.
	PrintDepthTotals();
	printf("Node count = %d\n", CountNodes(qloose));
	
	// Do some frustum-like queries, and count the number of potential hits.
	ct = 0;
	NodesChecked = 0;
	ObjectsActuallyInFrustum = 0;
	for (i = 0; i < FRUSTUM_COUNT; i++) {
		ct += LooseQuadTreeCountObjectsInFrustum(qloose, &FrustumList[i], SOME_CLIP);
	}
	printf("%d objects possibly in frustum, %d objects actually in frustum, %d nodes checked.\n", ct, ObjectsActuallyInFrustum, NodesChecked);

	// Do contact checks, and count some stats.
	ObjectsChecked = 0;
	NodesChecked = 0;
	Contacts = 0;
	for (i = 0; i < OBJECT_COUNT; i++) {
		Contacts += LooseCountContactingObjects(qloose, &ObjectList[i]);
	}
	printf("%d contacts, %d objects checked, %d nodes checked\n", Contacts, ObjectsChecked, NodesChecked);
	

	// Go into interactive mode so the user can explore the data.
	RunInteractive();

	
	return 0;
}


//
//
//


void	IdleFunc()
{
	// Call the display routine next time through the main loop  
	glutPostRedisplay();
}


void	DisplayFunc()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glScalef(0.001, -0.001, 1);
	
	// Draw the objects, tree, whatever.
	Frustum*	f = &TestFrustum;
	if (ShowLoose) {
		RenderLoose(qloose, f, SOME_CLIP);
	} else {
		RenderOrdinary(qordinary, f, SOME_CLIP);
	}

	// Draw frustum.
	glColor3f(1, 0, 0);
	glBegin(GL_LINES);
	
	glVertex2f(f->x, f->z);
	glVertex2f(f->x + -f->nz[0] * 2000, f->z + f->nx[0] * 2000);

	glVertex2f(f->x, f->z);
	glVertex2f(f->x + f->nz[1] * 2000, f->z + -f->nx[1] * 2000);
	
	glEnd();

	glutSwapBuffers();
}


void	KeyboardFunc(unsigned char key, int x, int y)
{
	if (key == 'z') {
		TestFOV -= (5.0 / 360) * 2 * PI;
		if (TestFOV < (5.0 / 360) * 2 * PI) TestFOV = (5.0 / 360) * 2 * PI;
		SetupFrustumNormals(&TestFrustum, TestAngle, TestFOV);
	} else if (key == 'x') {
		TestFOV += (5.0 / 360) * 2 * PI;
		if (TestFOV > PI) TestFOV = PI;
		SetupFrustumNormals(&TestFrustum, TestAngle, TestFOV);
	} else if (key == 'l') {
		ShowLoose = !ShowLoose;

		if (ShowLoose) {
			delete qloose;
			qloose = new qnode(0, 0, 0, 0);
			for (int i = 0; i < OBJECT_COUNT; i++) {
				LooseQuadTreeInsert(qloose, &ObjectList[i]);
			}
		} else {
			delete qordinary;
			qordinary = new qnode(0, 0, 0, 0);
			for (int i = 0; i < OBJECT_COUNT; i++) {
				QuadTreeInsert(qordinary, &ObjectList[i]);
			}
		}
	} else if (key == 'n') {
		ShowNodes = !ShowNodes;
	} else if (key == '+' || key == '-') {
		// Adjust looseness.
		if (key == '-') {
			LooseK -= 0.1;
			if (LooseK < 1) LooseK = 1;
		} else if (key == '+') {
			LooseK += 0.1;
		}

		printf("K = %g\n", LooseK);
		
		if (ShowLoose) {
			// Reclassify all objects.
			delete qloose;
			qloose = new qnode(0, 0, 0, 0);
			for (int i = 0; i < OBJECT_COUNT; i++) {
				LooseQuadTreeInsert(qloose, &ObjectList[i]);
			}
		}
	}
}


int	LastMouseX = 0, LastMouseY = 0;
bool	LeftButton = false, RightButton = false;


void	MouseHandler(int button, int state, int x, int y)
{
	if (button == 0) {
		if (state == 0) {
			LeftButton = true;
		} else {
			LeftButton = false;
		}
	}
	if (button == 2) {
		if (state == 0) {
			RightButton = true;
		} else {
			RightButton = false;
		}
	}
	LastMouseX = x;
	LastMouseY = y;
}


void	MotionHandler(int x, int y)
{
	float	dx = x - LastMouseX;
	float	dy = y - LastMouseY;
	LastMouseX = x;
	LastMouseY = y;
	
	if (LeftButton && RightButton) {
		
	} else if (LeftButton) {
		// Point at cursor.
//		float	cx = cos(TestAngle + PI / 2);
//		float	cz = sin(TestAngle + PI / 2);
//		float	cross = dx * cz - dy * cx;
		float	rx = (x - 250) - TestFrustum.x / 2;
		float	rz = (y - 250) - TestFrustum.z / 2;

//		float	d = sqrt(rx * rx + rz * rz);
//		if (d > 1) d = 1;
//		rx /= d;
//		rz /= d;
//		float	dot = (dx * rx + dy * rz) / d;
//		
//		TestAngle += dot / 10;
//		while (TestAngle < 0) TestAngle += 2 * PI;
//		while (TestAngle > 2 * PI) TestAngle -= 2 * PI;
		TestAngle = -atan2(rz, rx);

		SetupFrustumNormals(&TestFrustum, TestAngle, TestFOV);
		
	} else if (RightButton) {
		// Translate.
		TestFrustum.x += dx * 2;
		TestFrustum.z += dy * 2;
	}
}


void	OpenOGL()
{
	glutInitDisplayMode(GLUT_RGBA | /* GLUT_DEPTH | */ GLUT_DOUBLE);
	glutInitWindowPosition(200, 0);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Classification test");
	
	glutDisplayFunc(DisplayFunc);
	glutKeyboardFunc(KeyboardFunc);
	glutMouseFunc(MouseHandler);
	glutMotionFunc(MotionHandler);
//  glutSpecialFunc(KeyboardSpecialFunc);
//  glutReshapeFunc(ReshapeFunc);
	glutIdleFunc(IdleFunc);
//  
//  InitializeProgram();
	
	glutSwapBuffers();
//  glutMainLoop();
	
}



void	RunInteractive()
{
	glutMainLoop();
}

