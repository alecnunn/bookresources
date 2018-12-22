/*
 * Copyright (c) 2005 Erin Catto http://www.gphysics.com
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies.
 * Erin Catto makes no representations about the suitability 
 * of this software for any purpose.  
 * It is provided "as is" without express or implied warranty.
 */

#include <windows.h>
#include <gl\gl.h>
#include <vector>
#include <algorithm>

#include "glut.h"
#include "Buoyancy.h"

using std::vector;

Polyhedron poly;
RigidBody body;
WaterVolume water;
const float gravity = 9.8f;
float rotate = 0.0f;

void SetTransform(const Vec3& x, const Quaternion& q)
{
	Mat33 R(q);

	float H[16];
	H[0]  = R(0,0);	H[4]  = R(0,1); H[8]  = R(0,2); H[12] = x.x;
	H[1]  = R(1,0);	H[5]  = R(1,1);	H[9]  = R(1,2);	H[13] = x.y;
	H[2]  = R(2,0);	H[6]  = R(2,1);	H[10] = R(2,2);	H[14] = x.z;
	H[3]  = 0;		H[7]  = 0;		H[11] = 0;		H[15] = 1;
	
	glMultMatrixf(H);
}

void LoadPolyhedron(const char* fileName, float scale)
{
	vector<Vec3> verts;
	vector<Face> faces;

	FILE* file = fopen(fileName, "r");
	if(!file)
		return;

	char ch = 0;
	Vec3 v;
	Face f;

	while(!feof(file))
	{
		ch = fgetc(file);

		switch(ch)
		{
		case 'v':
			fscanf(file, "%f %f %f", &v.x, &v.y, &v.z);
			v *= scale;
			verts.push_back(v);
			break;

		case 'f':
			fscanf(file, "%d %d %d", &f.i1, &f.i2, &f.i3);
			--f.i1; --f.i2; --f.i3;
			faces.push_back(f);
			break;

		case '\n':
			break;

		default:
			break;
		}
	}

	poly.numVerts = (int)verts.size();
	poly.verts = new Vec3 [poly.numVerts];
	std::copy(verts.begin(), verts.end(), poly.verts);

	poly.numFaces = (int)faces.size();
	poly.faces = new Face [poly.numFaces];
	std::copy(faces.begin(), faces.end(), poly.faces);

	fclose(file);
}

void DrawPolyhedron(const Polyhedron& p, const Vec3& x, const Quaternion& q)
{
	glPushMatrix();
	SetTransform(x, q);

	glColor3f(0.7f, 0.7f, 0.9f);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < p.numFaces; ++i)
	{
		int i1 = p.faces[i].i1, i2 = p.faces[i].i2, i3 = p.faces[i].i3;
		Vec3 v1 = p.verts[i1];
		Vec3 v2 = p.verts[i2];
		Vec3 v3 = p.verts[i3];

		Vec3 n = (v2 - v1) % (v3 - v1);
		n.Normalize();
		glNormal3f(n.x, n.y, n.z);
		glVertex3f(v1.x, v1.y, v1.z);
		glVertex3f(v2.x, v2.y, v2.z);
		glVertex3f(v3.x, v3.y, v3.z);
	}
	glEnd();

	glPopMatrix();
}

void DrawWaterPlane()
{
	glEnable(GL_BLEND);
	glColor4f(0.0f, 0.85f, 0.95f, 1.0f);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glVertex3f(-100, 0, -100);
	glVertex3f(-100, 0, 100);
	glVertex3f(100, 0, 100);
	glVertex3f(100, 0, -100);
	glEnd();
	glDisable(GL_BLEND);
}

void BuildRaft()
{
	if (poly.verts)
		delete [] poly.verts;
	if (poly.faces)
		delete [] poly.faces;

	poly.numVerts = 8;
	poly.numFaces = 12;

	poly.verts = new Vec3 [8];
	poly.faces = new Face [12];

	float hx = 1.0f, hy = 0.1f, hz = 1.0f;

	poly.verts[0] = Vec3(-hx, -hy, -hz); // 000
	poly.verts[1] = Vec3(-hx, -hy,  hz); // 001
	poly.verts[2] = Vec3(-hx,  hy, -hz); // 010
	poly.verts[3] = Vec3(-hx,  hy,  hz); // 011
	poly.verts[4] = Vec3( hx, -hy, -hz); // 100
	poly.verts[5] = Vec3( hx, -hy,  hz); // 101
	poly.verts[6] = Vec3( hx,  hy, -hz); // 110
	poly.verts[7] = Vec3( hx,  hy,  hz); // 111

	poly.faces[0]  = Face(0, 1, 3);
	poly.faces[1]  = Face(0, 3, 2);
	poly.faces[2]  = Face(6, 3, 7);
	poly.faces[3]  = Face(6, 2, 3);
	poly.faces[4]  = Face(4, 6, 5);
	poly.faces[5]  = Face(6, 7, 5);
	poly.faces[6]  = Face(4, 5, 0);
	poly.faces[7]  = Face(0, 5, 1);
	poly.faces[8]  = Face(5, 7, 1);
	poly.faces[9]  = Face(7, 3, 1);
	poly.faces[10] = Face(0, 6, 4);
	poly.faces[11] = Face(0, 2, 6);

	// Approximate length
	poly.length = 1.0f;

	poly.volume = ComputeVolume(poly);

	// Use half the density of water for the body.
	// Thus, the box should be half way submerged at equilibrium.
	body.mass = 500.0f*poly.volume;	// half the density of water
	body.I = (4.0f*body.mass/12.0f)*Vec3(hy*hz, hx*hz, hx*hy);

	// Setup initial conditions
	body.x = Vec3(0.0f, 2.0f, 0.0f);
	body.q = Quaternion(1.0f, 2.0f, 3.0f, 4.0f);
	body.q.Normalize();
	body.v.SetZero();
	body.omega.SetZero();
	body.F.SetZero();
	body.T.SetZero();
}

void BuildNonConvex()
{
	if (poly.verts)
		delete [] poly.verts;
	if (poly.faces)
		delete [] poly.faces;

	poly.numVerts = 6;
	poly.numFaces = 8;

	poly.verts = new Vec3 [6];
	poly.faces = new Face [8];

	poly.verts[0] = Vec3( 0.5f,  0.0f, -0.5f);
	poly.verts[1] = Vec3(-0.5f,  0.0f, -0.5f);
	poly.verts[2] = Vec3(-0.5f,  0.0f,  0.5f);
	poly.verts[3] = Vec3( 0.5f,  0.0f,  0.5f);
	poly.verts[4] = Vec3( 1.5f,  2.0f,  0.0f);
	poly.verts[5] = Vec3( 1.5f, -2.0f,  0.0f);

	poly.faces[0]  = Face(0, 1, 4);
	poly.faces[1]  = Face(0, 5, 1);
	poly.faces[2]  = Face(1, 2, 4);
	poly.faces[3]  = Face(1, 5, 2);
	poly.faces[4]  = Face(2, 3, 4);
	poly.faces[5]  = Face(2, 5, 3);
	poly.faces[6]  = Face(3, 0, 4);
	poly.faces[7]  = Face(3, 5, 0);

	// Approximate length
	poly.length = 1.0f;

	poly.volume = ComputeVolume(poly);

	body.mass = 500.0f*poly.volume;	// half the density of water
	body.I = (1.0f*body.mass/12.0f)*Vec3(1.0f, 1.0f, 1.0f); // approximate

	// Setup initial conditions
	body.x = Vec3(0.0f, 2.0f, 0.0f);
	body.q = Quaternion(1.0f, -2.0f, 3.0f, -4.0f);
	body.q.Normalize();
	body.v.SetZero();
	body.omega.SetZero();
	body.F.SetZero();
	body.T.SetZero();
}

void BuildBoat()
{
	if (poly.verts)
		delete [] poly.verts;
	if (poly.faces)
		delete [] poly.faces;

	LoadPolyhedron("boat.dat", 0.02f);

	// Approximate length
	poly.length = 2.0f;

	poly.volume = ComputeVolume(poly);

	body.mass = 500.0f*poly.volume;	// half the density of water
	body.I = (1.0f*body.mass/12.0f)*Vec3(1.0f, 1.0f, 1.0f); // approximate

	// Setup initial conditions
	body.x = Vec3(0.0f, 2.0f, 0.0f);
	body.q = Quaternion(1.0f, -2.0f, 3.0f, -4.0f);
	body.q.Normalize();
	body.v.SetZero();
	body.omega.SetZero();
	body.F.SetZero();
	body.T.SetZero();
}

void Step(RigidBody& body, float dt)
{
	// Clear force and torque accumulators.
	body.F.SetZero();
	body.T.SetZero();

	// Set gravity force.
	body.F.y = -body.mass*gravity;

	// Add buoyancy force.
	ComputeBuoyancy(body, poly, water, gravity);

	// Update velocities first.
	body.v += (dt/body.mass)*body.F;
	Vec3 tmp(body.T.x/body.I.x, body.T.y/body.I.y, body.T.z/body.I.z);
	body.omega += dt*tmp;

	// Update positions using new velocities.
	Quaternion omegaQ(body.omega.x, body.omega.y, body.omega.z, 0);
	body.x += dt*body.v;
	body.q += (0.5f*dt) * omegaQ * body.q;
	body.q.Normalize();
}

void SimulationLoop()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(0, 1, 0, 0);
	glTranslatef(-0.1f, -1.5f, -10.0f);
	glRotatef(rotate, 0, 1, 0);

	// Step rigid body.
	Step(body, 0.015f);

	// Draw
	DrawPolyhedron(poly, body.x, body.q);
	DrawWaterPlane();

	glutSwapBuffers();
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;

	case 'b': case 'B':
		BuildBoat();
		break;

	case 'r': case 'R':
		BuildRaft();
		break;

	case 'n': case 'N':
		BuildNonConvex();
		break;
	}
}

void ArrowKeys(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		rotate += 5;
		break;
	case GLUT_KEY_RIGHT:
		rotate -= 5;
		break;
	}
}

void Reshape(int width, int height)
{
	if (height == 0)
		height = 1;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (float)width/(float)height, 0.1, 100.0);
}

void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Buoyancy Test - (R)aft, (N)onconvex, (B)oat");

	float lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float lightDiffuse[] = { 0.6f, 0.6f, 0.6f, 1.0f };
	float lightSpecular[] = { 0.9f, 0.9f, 0.9f };
	float lightpos[] = { 1.0f, 10.0f, 2.0f, 0.0f };

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.25f);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);

	glutReshapeFunc(Reshape);
	glutDisplayFunc(SimulationLoop);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(ArrowKeys);
	glutIdleFunc(SimulationLoop);

	memset(&poly, 0, sizeof Polyhedron);
	BuildRaft();

	water.density = 1000.0f;
	water.angularDrag = 0.5f;
	water.linearDrag = 5.0f;
	water.velocity.SetZero();
	water.plane.normal = Vec3(0, 1, 0);
	water.plane.offset = 0.0f;

	glutMainLoop();
}
