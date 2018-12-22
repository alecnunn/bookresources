// **********************************************************************
// * FILE  : glutFunc.c
// *---------------------------------------------------------------------
// * PURPOSE : glut and GL stuff
// *
// *---------------------------------------------------------------------
// * AUTHOR   : Thomas Di Giacomo . MIRALab (giacomo@miralab.unige.ch)
// **********************************************************************
/*
 * This copyright notice must be included in all copies or derivative works.
 * (c) Copyright 2003, MIRALab, C.U.I. University of Geneva 
 * ALL RIGHTS RESERVED
 * Permission to use, copy, modify, and distribute this software for
 * any purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright notice
 * and this permission notice appear in supporting documentation, and that
 * the name of MIRALab not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL MIRALAB
 * BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
 * KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
 * LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
 * THIRD PARTIES, WHETHER OR NOT MIRALAB  HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
 * POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include "glutFunc.h"
#include "glut.h"

static tVect	mouseForce;
static bool		mouseControl;
static bool		mouseEnv;
static float	mouseForceOffset = 0.00001f;
static float	firstMouseX, firstMouseY;
static bool		scaling;
static bool		moving;
static bool		objControl;
static bool		newModel;
static float	scalefactor = 1.f;

// Text display
static int		font=(int)GLUT_BITMAP_8_BY_13;
static int		w, h;
static int		frame, time, timebase=0;
static char		s[30];

// Trackball quaternions
float curquat[4];
float lastquat[4];

GLfloat position [] = { 0.f, 10.f, 10.f, 1.f };


// ****************************************
// * Main draw
// ****************************************

void drawEnvNode(CEnvNod* node, const bool& drawMass, const bool& drawPipe, const float& drawSize)
{
	tVect thisPos = node->Compute3DRelativePosition();
	if (node->GetIndex()==0)
		node->ComputeNurbsCrossSection(thisPos);

	// Drawing
	glTranslatef(node->GetBrosGlob()->GetPosition().x, node->GetBrosGlob()->GetPosition().y, node->GetBrosGlob()->GetPosition().z);
	if (!drawPipe)
	{
		// Draw enveloppe spring
		glLineWidth(1.f);
		glBegin(GL_LINE_STRIP);
			glColor3f(1.f, 1.f, 0.f);
			glVertex3f(0.f, 0.f, 0.f);
			glVertex3f(thisPos.x, thisPos.y, thisPos.z);
		glEnd();
		// Draw nurbs cross-section
		glLineWidth(3.f);
		for(int e = 1; e <= wire->GetNURBSAcc(); e++)
		{
			glBegin(GL_LINES);
				glColor3f(0.6f, 0.6f, 0.6f);
				glVertex3f(node->GetNurbsPts(e-1).x, node->GetNurbsPts(e-1).y, node->GetNurbsPts(e-1).z);
				glVertex3f(node->GetNurbsPts(e).x, node->GetNurbsPts(e).y, node->GetNurbsPts(e).z);
			glEnd();
		}
	}
	else
	{
		if (node->GetBrosGlob()->GetSon())
		{
			tVect tmp;
			SubVector(&node->GetBrosGlob()->GetSon()->GetPosition(), &node->GetBrosGlob()->GetPosition(), &tmp);
			for (int k =0; k < wire->GetNURBSAcc(); k++)
			{
				tVect n0, n1, ns0, ns1, norm;
				CopyVector(&node->GetNurbsPts(k), &n0);
				CopyVector(&node->GetNurbsPts(k+1), &n1);
				CopyVector(&node->GetBrosGlob()->GetSon()->GetBros(node->GetEnvIndex())->GetNurbsPts(k), &ns0);
				CopyVector(&node->GetBrosGlob()->GetSon()->GetBros(node->GetEnvIndex())->GetNurbsPts(k+1), &ns1);
				SumVector(&ns0, &tmp, &ns0);
				SumVector(&ns1, &tmp, &ns1);
				SetVector(&norm, 2*(n0.x + n1.x), 2*(n0.y + n1.y), 2*(n0.z + n1.z));
				NormalVector(&norm);
				// Draw enveloppe nurbs surface
				glDisable(GL_CULL_FACE);
				glBegin(GL_QUAD_STRIP);
					glColor3f(0.7f, 0.6f, 0.3f);
					glNormal3f(norm.x, norm.y, norm.z);
					glVertex3f(n0.x, n0.y, n0.z);
					glVertex3f(n1.x, n1.y, n1.z);
					glVertex3f(ns0.x, ns0.y, ns0.z);
					glVertex3f(ns1.x, ns1.y, ns1.z);
				glEnd();
				glEnable(GL_CULL_FACE);
			}
		}
	}
	// Draw enveloppe mass
	glColor3f(1.f, 1.f, 0.f);

	if (node->GetIndex() == 0) 
	{
		glTranslatef(thisPos.x, thisPos.y, thisPos.z);
		glColor3f(1.f, 0.f, 0.f);
		glutSolidCube(drawSize);
		glTranslatef(-thisPos.x, -thisPos.y, -thisPos.z);
	}	
	// draw mass
	if (drawMass)
	{
		glTranslatef(thisPos.x, thisPos.y, thisPos.z);
		glColor3f(0.f, 0.f, 0.f);
		glutSolidCube(drawSize);
		glTranslatef(-thisPos.x, -thisPos.y, -thisPos.z);
	}

	glTranslatef(-node->GetBrosGlob()->GetPosition().x, -node->GetBrosGlob()->GetPosition().y, -node->GetBrosGlob()->GetPosition().z);
}

void drawNode(CAxNod* node, bool mode[5], float size)
{
	if (!mode[4])
	{
		if (node->GetIndex() == 0) 
		{
			glTranslatef(node->GetPosition().x, node->GetPosition().y, node->GetPosition().z);
			glColor3f(1.f, 0.f, 0.f);
			glutSolidCube(size);
			glTranslatef(-node->GetPosition().x, -node->GetPosition().y, -node->GetPosition().z);
		}	
		// draw mass
		if (mode[0])
		{
			glTranslatef(node->GetPosition().x, node->GetPosition().y, node->GetPosition().z);
			glColor3f(0.f, 0.f, 0.f);
			glutSolidCube(size);
			glTranslatef(-node->GetPosition().x, -node->GetPosition().y, -node->GetPosition().z);
		}		
		
		// Draw velocity
		if (mode[3])
		{
			int vDrawOff = 10;
			glBegin(GL_LINES);
				glColor3f(0.f, 0.f, 1.f);
				glVertex3f(node->GetPosition().x, node->GetPosition().y, node->GetPosition().z);
				glVertex3f(node->GetPosition().x + node->GetVelocity().x * vDrawOff, node->GetPosition().y + node->GetVelocity().y * vDrawOff, node->GetPosition().z + node->GetVelocity().z * vDrawOff);
			glEnd();
		}
		// Draw force
 		if (mode[2])
		{
			int fDrawOff = 1000;
			glBegin(GL_LINES);
				glColor3f(0.f, 1.f, 0.f);
				glVertex3f(node->GetPosition().x, node->GetPosition().y, node->GetPosition().z);
				glVertex3f(node->GetPosition().x + node->GetForce().x * fDrawOff, node->GetPosition().y + node->GetForce().y * fDrawOff, node->GetPosition().z + node->GetForce().z * fDrawOff);
			glEnd();		
		}
		
		// --------------------------------
		// Draw enveloppe bros
		if (mode[1])
		{
			for (int i = 0; i < node->GetWire()->GetNbEnv(); i++)
				drawEnvNode(node->GetBros(i), mode[0], mode[4], size);
		}
		
		// Draw axial sons
		if (node->GetSon()) 
		{
			glLineWidth(2.f);
			glBegin(GL_LINES);
				glColor3f(0.f, 0.f, 0.f);
				glVertex3f(node->GetPosition().x, node->GetPosition().y, node->GetPosition().z);
				glVertex3f(node->GetSon()->GetPosition().x, node->GetSon()->GetPosition().y, node->GetSon()->GetPosition().z);
			glEnd();
			drawNode(node->GetSon(), mode, size);
		}
	}
	else
	{
		for (int i = 0; i < node->GetWire()->GetNbEnv(); i++)
			drawEnvNode(node->GetBros(i), mode[0], mode[4], size);
		if (node->GetSon()) 
			drawNode(node->GetSon(), mode, size);
	}
}

void drawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// World container 
	glColor3f(1.f,1.f,1.f);

  glBegin(GL_LINE_STRIP);
      glVertex3f(-worldSizeX/2.f, worldSizeY/2.f,-worldSizeZ/2.f);
      glVertex3f( worldSizeX/2.f, worldSizeY/2.f,-worldSizeZ/2.f);
      glVertex3f( worldSizeX/2.f, worldSizeY/2.f, worldSizeZ/2.f);
      glVertex3f(-worldSizeX/2.f, worldSizeY/2.f, worldSizeZ/2.f);
      glVertex3f(-worldSizeX/2.f, worldSizeY/2.f,-worldSizeZ/2.f);
      glVertex3f(-worldSizeX/2.f,-worldSizeY/2.f,-worldSizeZ/2.f);
  glEnd();
  glBegin(GL_LINES);
      glVertex3f( worldSizeX/2.f, worldSizeY/2.f,-worldSizeZ/2.f);
      glVertex3f( worldSizeX/2.f,-worldSizeY/2.f,-worldSizeZ/2.f);

      glVertex3f( worldSizeX/2.f, worldSizeY/2.f, worldSizeZ/2.f);
      glVertex3f( worldSizeX/2.f,-worldSizeY/2.f, worldSizeZ/2.f);

      glVertex3f(-worldSizeX/2.f, worldSizeY/2.f, worldSizeZ/2.f);
      glVertex3f(-worldSizeX/2.f,-worldSizeY/2.f, worldSizeZ/2.f);
  glEnd();
  // floor
  glDisable(GL_CULL_FACE);
  glBegin(GL_QUADS);
      glColor3f(0.5f, 0.5f, 0.5f);
      glVertex3f(-worldSizeX/2.f,-worldSizeY/2.f,-worldSizeZ/2.f);
      glVertex3f( worldSizeX/2.f,-worldSizeY/2.f,-worldSizeZ/2.f);
      glVertex3f( worldSizeX/2.f,-worldSizeY/2.f, worldSizeZ/2.f);
      glVertex3f(-worldSizeX/2.f,-worldSizeY/2.f, worldSizeZ/2.f);
  glEnd();
  glEnable(GL_CULL_FACE);
		
	// Object
	glPushMatrix();
		glTranslatef(object->GetPosition().x, object->GetPosition().y, object->GetPosition().z);
		glColor3f(0.5f, 0.3f, 0.2f);
		glutSolidCube(object->GetSize());
	glPopMatrix();

	// Wire
	drawNode(wire->GetRoot(), wire->GetDrawMode(), wire->GetDrawSize());

}

// ****************************************
// * Trackball
// ****************************************

void recalcModelView(void)
{
	GLfloat m[4][4];

	glPopMatrix();
	glPushMatrix();
	build_rotmatrix(m, curquat);
	glMultMatrixf(&m[0][0]);
	glScalef(scalefactor, scalefactor, scalefactor);
	newModel = false;
}

void animate(void)
{
	add_quats(lastquat, curquat, curquat);
	newModel = true;
	glutPostRedisplay();
}

// ****************************************
// * Main display
// ****************************************

void display()
{
	if (newModel)
		recalcModelView();

	// Update
	wire->CheckWorldCollisions(object);
	wire->ComputeForce();
	wire->Integrate();

	object->CheckWorldCollisions();	
	object->Integrate();

	// Draw
	drawScene();

	// FPS
	frame++;
	time=glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000) 
	{
		sprintf(s, "FPS:%4.2f", frame * 1000.f / (time-timebase));
		timebase = time;		
		frame = 0;
	}
	glColor3f(0.f, 1.f, 1.f);
	setOrthographicProjection();
	glPushMatrix();
		glLoadIdentity();
		renderBitmapString(10, 15, (void *)font, s);
		renderBitmapString(10, 65, (void *)font, "(M)ass");
		renderBitmapString(10, 80, (void *)font, "(F)orce");
		renderBitmapString(10, 95, (void *)font, "(V)elocity");
		renderBitmapString(10, 110, (void *)font, "(E)nvelope");
	glPopMatrix();
	resetPerspectiveProjection();

	// Swap buffers
	glutSwapBuffers();
}


// ****************************************
// * FPS functions
// ****************************************

void setOrthographicProjection() 
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
	glScalef(1.f, -1.f, 1.f);
	glTranslatef(0, -h, 0);
	glMatrixMode(GL_MODELVIEW);
}

void resetPerspectiveProjection() 
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void renderBitmapString(float x, float y, void *font,char *string)
{
	char *c;
	glRasterPos2f(x, y);
	for (c=string; *c != '\0'; c++) 
	{
		glutBitmapCharacter(font, *c);
	}
}

// ****************************************
// * Callback functions
// ****************************************

void reshape(int x, int y)
{
	w = x;
	h = y;

	if (y == 0 || x == 0) return;

	glMatrixMode(GL_PROJECTION);  
	glLoadIdentity();

	gluPerspective(40.0f, (GLdouble)x/(GLdouble)y, 0.5f, 20.f);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, x, y);
}

void keyboard(unsigned char key, int x, int y)
{
 	switch (key) {
		case 27:
			delete wire;
			delete object;
			exit(0);
		break;
		case 'r':
			wire->Reset();
			object->Reset();
		break;
		case 'm':						
			wire->SetDrawMode(0);
		break;
		case 'e':
			wire->SetDrawMode(1);
		break;
		case 'f':
			wire->SetDrawMode(2);
		break;
		case 'v':
			wire->SetDrawMode(3);
		break;
		case 'a':
			wire->SetDrawMode(4);
		break;
	}
}

void processSpecialKeys(int key, int x, int y) 
{
	switch(key) {
		case GLUT_KEY_RIGHT : 
		break;
		case GLUT_KEY_LEFT : 
		break;
		case GLUT_KEY_UP : 
		break;
		case GLUT_KEY_DOWN : 			
		break;
	}
}

void processMouse(int button, int state, int x, int y) 
{
	if (glutGetModifiers() & GLUT_ACTIVE_SHIFT) 
	{
		scaling = true;
		mouseEnv = true;
    } 
	else 
	{
		scaling = false;
		mouseEnv = false;
    }

	if (state == GLUT_DOWN)
	{
		if (button == GLUT_LEFT_BUTTON) 
		{
			mouseControl = true;
			firstMouseX = x;
			firstMouseY = y;
			if (glutGetModifiers() & GLUT_ACTIVE_CTRL) 
			{
  			objControl = true;
				mouseControl = false;
			}
			else objControl = false;
		}
		else if (button == GLUT_MIDDLE_BUTTON) 
		{
		}
		else if (button == GLUT_RIGHT_BUTTON)
		{
			moving = 1;
			firstMouseX = x;
			firstMouseY = y;
		}
	}
	else if (state == GLUT_UP)
	{
		if (button == GLUT_LEFT_BUTTON) 
		{
			firstMouseX = 0.f;
			firstMouseY = 0.f;
			mouseControl = false;
			objControl = false;
		}
		else if (button == GLUT_MIDDLE_BUTTON) 
		{
		}
		else if (button == GLUT_RIGHT_BUTTON) 
		{
			moving = 0;
		}
	}
}

void processMouseActiveMotion(int x, int y) 
{
	if (mouseControl)
	{
		mouseForce.x = (x - firstMouseX) * mouseForceOffset;
		mouseForce.y = -(y - firstMouseY) * mouseForceOffset;

		if (mouseEnv)
			wire->GetRoot()->GetBros(0)->SetAxForce(mouseForce.x);
		else
			wire->GetRoot()->SetForce(mouseForce);
	}
	else if (objControl)
	{
		mouseForce.x = (x - firstMouseX) * mouseForceOffset;
		mouseForce.y = -(y - firstMouseY) * mouseForceOffset;
		object->SetForce(mouseForce);
	}
	else
	{
		if (scaling) 
		{
			scalefactor = scalefactor * (1.f + (((float) (firstMouseY - y)) / h));
			firstMouseX = x;
			firstMouseY = y;
			newModel = 1;
			glutPostRedisplay();
			return;
		}
		if (moving) 
		{
			trackball(lastquat, (2.f * firstMouseX - w) / w, (h - 2.f * firstMouseY) / h, (2.f * x - w) / w, (h - 2.f * y) / h);
			firstMouseX = x;
			firstMouseY = y;
			animate();
		}
	}
}

// ****************************************
// * Init glut
// ****************************************

void glutMain(int argc, char **argv) 
{	

	GLfloat specular [] = { 1.f, 1.f, 1.f, 1.f };
	GLfloat shininess [] = { 1000.f };
	GLfloat ambient [] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat diffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };

	// Window and glut
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	trackball(curquat, 0.0, 0.0, 0.0, 0.0);

	
	glutInitWindowSize(1024, 800);
	glutInitWindowPosition (10, 10);
	int Window3D = glutCreateWindow("Wire. (c)MIRALab.");

	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(processSpecialKeys);
	glutMouseFunc(processMouse);
	glutMotionFunc(processMouseActiveMotion);


	// GL
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glClearColor(.6f,.7f,.8f,0.f);
	glTranslatef(0.f,0.f,-5.f);

	// Lightning
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	
	glutMainLoop();
}
