// File:	BillboardBeam.cpp
// Author:	Brian Hawkins (winterdark@sprynet.com)
// Date:	11/24/01
//
// Copyright 2001, all rights reserved.
//
// Desc:	OpenGL demo application demonstrating billboard beams.
////////////////////////////////////////////////////////////////////////////////

// Standard windows headers.
#include "stdafx.h"

// Standard library.
#include <math.h>

// OpenGL auxilary library.
#include <GL/glaux.h>

// Desc:	Simple vector structure.
struct t_SimpleVector
{
	t_SimpleVector() : m_x(0.0), m_y(0.0), m_z(0.0) {}
	t_SimpleVector(GLdouble i_x, GLdouble i_y, GLdouble i_z)
		: m_x(i_x), m_y(i_y), m_z(i_z) {}
	GLdouble m_x, m_y, m_z;
};

// Desc:    Make unit vector.
void g_Normalize(t_SimpleVector &i_vector)
{
	GLdouble l_length = sqrt(
		i_vector.m_x * i_vector.m_x +
		i_vector.m_y * i_vector.m_y +
		i_vector.m_z * i_vector.m_z);
	GLdouble l_inverseLength = 1.0 / l_length;
	i_vector.m_x *= l_inverseLength;
	i_vector.m_y *= l_inverseLength;
	i_vector.m_z *= l_inverseLength;
}

// Desc:    Return the cross product of two vectors.
t_SimpleVector g_CrossProduct(const t_SimpleVector &i_a,
							  const t_SimpleVector &i_b)
{
	return(t_SimpleVector(
		i_a.m_y * i_b.m_z - i_a.m_z * i_b.m_y,
		i_a.m_z * i_b.m_x - i_a.m_x * i_b.m_z,
		i_a.m_x * i_b.m_y - i_a.m_y * i_b.m_x
		));
}

// Desc:	Simple matrix structure.
struct t_SimpleMatrix
{
	t_SimpleMatrix()
	{
		// Create an identity matrix.
		for(unsigned int l_index = 0; l_index < 16; ++l_index) {
			m_matrix[l_index] = ((l_index / 4) == (l_index % 4)) ?
				1.0 : 0.0;
		}
	}
	GLdouble m_matrix[16];
};

// Desc:	Transform a simple vector using an OpenGL matrix.
const t_SimpleVector g_TransformVector(const t_SimpleMatrix &i_matrix,
									   const t_SimpleVector &i_vector)
{
	return(t_SimpleVector(
		i_matrix.m_matrix[0] * i_vector.m_x +
		i_matrix.m_matrix[4] * i_vector.m_y +
		i_matrix.m_matrix[8] * i_vector.m_z +
		i_matrix.m_matrix[12],
		i_matrix.m_matrix[1] * i_vector.m_x +
		i_matrix.m_matrix[5] * i_vector.m_y +
		i_matrix.m_matrix[9] * i_vector.m_z +
		i_matrix.m_matrix[13],
		i_matrix.m_matrix[2] * i_vector.m_x +
		i_matrix.m_matrix[6] * i_vector.m_y +
		i_matrix.m_matrix[10] * i_vector.m_z +
		i_matrix.m_matrix[14]
		));
}

// Desc:	Simple color structure.
struct t_SimpleColor
{
	t_SimpleColor() : m_r(0.0), m_g(0.0), m_b(0.0), m_a(1.0) {}
	t_SimpleColor(GLdouble i_r, GLdouble i_g, GLdouble i_b,
		GLdouble i_a = 1.0)
		: m_r(i_r), m_g(i_g), m_b(i_b), m_a(i_a) {}
	GLdouble m_r, m_g, m_b, m_a;
};

// Desc:	Global camera matrix.
static bool g_orientBeams = true;
static t_SimpleVector g_cameraPosition;
static t_SimpleVector g_cameraOrientation(180.0, 0.0, 0.0);

// Desc:    Camera movement.
static GLdouble g_cameraSpeed = 1.0;

void __stdcall g_ToggleBeamOrientation()
{
	g_orientBeams = !g_orientBeams;
}
void __stdcall g_CameraFasterCallback()
{
	g_cameraSpeed *= 2.0;
}
void __stdcall g_CameraSlowerCallback()
{
	g_cameraSpeed *= 0.5;
}
void __stdcall g_CameraForwardCallback()
{
	g_cameraPosition.m_z += g_cameraSpeed;
}
void __stdcall g_CameraBackwardCallback()
{
	g_cameraPosition.m_z -= g_cameraSpeed;
}
void __stdcall g_CameraRightCallback()
{
	g_cameraPosition.m_x += g_cameraSpeed;
}
void __stdcall g_CameraLeftCallback()
{
	g_cameraPosition.m_x -= g_cameraSpeed;
}
void __stdcall g_CameraUpCallback()
{
	g_cameraPosition.m_y += g_cameraSpeed;
}
void __stdcall g_CameraDownCallback()
{
	g_cameraPosition.m_y -= g_cameraSpeed;
}

void __stdcall g_CameraRotateLeftCallback()
{
	g_cameraOrientation.m_y += g_cameraSpeed;
}
void __stdcall g_CameraRotateRightCallback()
{
	g_cameraOrientation.m_y -= g_cameraSpeed;
}
void __stdcall g_CameraRotateUpCallback()
{
	g_cameraOrientation.m_x -= g_cameraSpeed;
}
void __stdcall g_CameraRotateDownCallback()
{
	g_cameraOrientation.m_x += g_cameraSpeed;
}

// Desc:	Update the projection matrix.
// Input:	i_width - Width of current window.
//			i_height - Height of current window.
void g_UpdateProjectionMatrix(int i_width, int i_height)
{
	// Default settings.
	static const GLdouble k_FIELDOFVIEWY	= 45.0;
	static const GLdouble k_NEARPLANEZ		= 1.0;
	static const GLdouble k_FARPLANEZ		= 1000.0;

	// Update projection matrix with correct aspect ratio.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(k_FIELDOFVIEWY, static_cast<GLdouble>(i_width / i_height),
		k_NEARPLANEZ, k_FARPLANEZ);
	glMatrixMode(GL_MODELVIEW);
}

// Desc:	Global camera.
static t_SimpleMatrix g_cameraToWorldMatrix;

// Desc:	Render a single beam.
void g_RenderBeam(const t_SimpleVector &i_endpoint1,
				  const t_SimpleVector &i_endpoint2,
				  const t_SimpleColor &i_color,
				  GLdouble i_size)
{
	// Eye vector to beam.
	// E = Cp - B1
	t_SimpleVector l_eye(
		g_cameraToWorldMatrix.m_matrix[12] - i_endpoint1.m_x,
		g_cameraToWorldMatrix.m_matrix[13] - i_endpoint1.m_y,
		g_cameraToWorldMatrix.m_matrix[14] - i_endpoint1.m_z);

	// Beam vector defined by endpoints.
	// B = B2 - B1
	t_SimpleVector l_beam(
		i_endpoint1.m_x - i_endpoint2.m_x,
		i_endpoint1.m_y - i_endpoint2.m_y,
		i_endpoint1.m_z - i_endpoint2.m_z);

	// Cross product of beam and eye.
	// P = B x E
	t_SimpleVector l_perpendicularBeam =
		g_CrossProduct(l_beam, l_eye);

	// Extract camera front vector from camera to world matrix.
	// Bf = Cf
	t_SimpleVector l_frontVector;
	l_frontVector.m_x = g_cameraToWorldMatrix.m_matrix[8];
	l_frontVector.m_y = g_cameraToWorldMatrix.m_matrix[9];
	l_frontVector.m_z = g_cameraToWorldMatrix.m_matrix[10];

	// Determine up and right vectors.
	// Bu = (Bf x P) * (1 / ||(Bf x P)||)
	// Br = Bf x Bu
	t_SimpleVector l_upVector =
		g_CrossProduct(l_frontVector, l_perpendicularBeam);
	g_Normalize(l_upVector);
	t_SimpleVector l_rightVector =
		g_CrossProduct(l_frontVector, l_upVector);

	// Form matrix from the vectors.
	t_SimpleMatrix l_billboardMatrix;
	l_billboardMatrix.m_matrix[8]	= l_frontVector.m_x;
	l_billboardMatrix.m_matrix[9]	= l_frontVector.m_y;
	l_billboardMatrix.m_matrix[10]	= l_frontVector.m_z;
	l_billboardMatrix.m_matrix[4]	= l_upVector.m_x;
	l_billboardMatrix.m_matrix[5]	= l_upVector.m_y;
	l_billboardMatrix.m_matrix[6]	= l_upVector.m_z;
	l_billboardMatrix.m_matrix[0]	= l_rightVector.m_x;
	l_billboardMatrix.m_matrix[1]	= l_rightVector.m_y;
	l_billboardMatrix.m_matrix[2]	= l_rightVector.m_z;

	// Create a matrix for each endpoint.
	t_SimpleMatrix l_billboardMatrix1 = l_billboardMatrix;
	l_billboardMatrix1.m_matrix[12]	= i_endpoint1.m_x;
	l_billboardMatrix1.m_matrix[13]	= i_endpoint1.m_y;
	l_billboardMatrix1.m_matrix[14]	= i_endpoint1.m_z;
	t_SimpleMatrix l_billboardMatrix2 = l_billboardMatrix;
	l_billboardMatrix2.m_matrix[12]	= i_endpoint2.m_x;
	l_billboardMatrix2.m_matrix[13]	= i_endpoint2.m_y;
	l_billboardMatrix2.m_matrix[14]	= i_endpoint2.m_z;

	// Transform vertices.
	t_SimpleVector l_vertex00 =
		g_TransformVector(l_billboardMatrix1,
		t_SimpleVector(0.0, i_size, 0.0));
	t_SimpleVector l_vertex0h =
		g_TransformVector(l_billboardMatrix1,
		t_SimpleVector(-i_size, 0.0, 0.0));
	t_SimpleVector l_vertexh0 =
		g_TransformVector(l_billboardMatrix1,
		t_SimpleVector(i_size, 0.0, 0.0));
	t_SimpleVector l_vertex1h =
		g_TransformVector(l_billboardMatrix2,
		t_SimpleVector(i_size, 0.0, 0.0));
	t_SimpleVector l_vertexh1 =
		g_TransformVector(l_billboardMatrix2,
		t_SimpleVector(-i_size, 0.0, 0.0));
	t_SimpleVector l_vertex11 =
		g_TransformVector(l_billboardMatrix2,
		t_SimpleVector(0.0, -i_size, 0.0));

	// Turn vertices in to triangle strip.
	glBegin(GL_TRIANGLE_STRIP);

	// Set beam color.
	glColor4d(i_color.m_r, i_color.m_g, i_color.m_b, i_color.m_a);

	// Set beam vertices.
	glTexCoord2d(0.0, 0.0);
	glVertex3d(l_vertex00.m_x, l_vertex00.m_y, l_vertex00.m_z);

	glTexCoord2d(0.0, 0.5);
	glVertex3d(l_vertex0h.m_x, l_vertex0h.m_y, l_vertex0h.m_z);

	glTexCoord2d(0.5, 0.0);
	glVertex3d(l_vertexh0.m_x, l_vertexh0.m_y, l_vertexh0.m_z);

	glTexCoord2d(0.0, 1.0);
	glVertex3d(l_vertexh1.m_x, l_vertexh1.m_y, l_vertexh1.m_z);

	glTexCoord2d(0.5, 0.5);
	glVertex3d(l_vertex1h.m_x, l_vertex1h.m_y, l_vertex1h.m_z);

	glTexCoord2d(0.5, 1.0);
	glVertex3d(l_vertex11.m_x, l_vertex11.m_y, l_vertex11.m_z);

	glEnd();
}

// Desc:    Render scene.
void g_RenderScene()
{
	// Clear the buffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Obtain camera to world matrix.
	if(g_orientBeams) {
		glPushMatrix();
		glLoadIdentity();
		glTranslated(g_cameraPosition.m_x,
			g_cameraPosition.m_y, g_cameraPosition.m_z);
		glRotated(g_cameraOrientation.m_z, 0.0, 0.0, 1.0);
		glRotated(g_cameraOrientation.m_y, 0.0, 1.0, 0.0);
		glRotated(g_cameraOrientation.m_x, 1.0, 0.0, 0.0);
		glGetDoublev(GL_MODELVIEW_MATRIX, g_cameraToWorldMatrix.m_matrix);
		glPopMatrix();
	}

	// Activate camera.
	glPushMatrix();
	glRotated(-g_cameraOrientation.m_x, 1.0, 0.0, 0.0);
	glRotated(-g_cameraOrientation.m_y, 0.0, 1.0, 0.0);
	glRotated(-g_cameraOrientation.m_z, 0.0, 0.0, 1.0);
	glTranslated(-g_cameraPosition.m_x,
		-g_cameraPosition.m_y, -g_cameraPosition.m_z);

	// Solid beams.
	glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);

	// Render cube of beams.
	g_RenderBeam(
		t_SimpleVector(10.0, 10.0, 200.0),
		t_SimpleVector(10.0, -10.0, 200.0),
		t_SimpleColor(1.0, 1.0, 1.0), 1.0);
	g_RenderBeam(
		t_SimpleVector(10.0, -10.0, 200.0),
		t_SimpleVector(-10.0, -10.0, 200.0),
		t_SimpleColor(1.0, 1.0, 1.0), 1.0);
	g_RenderBeam(
		t_SimpleVector(-10.0, -10.0, 200.0),
		t_SimpleVector(-10.0, 10.0, 200.0),
		t_SimpleColor(1.0, 1.0, 1.0), 1.0);
	g_RenderBeam(
		t_SimpleVector(-10.0, 10.0, 200.0),
		t_SimpleVector(10.0, 10.0, 200.0),
		t_SimpleColor(1.0, 1.0, 1.0), 1.0);
	g_RenderBeam(
		t_SimpleVector(10.0, 10.0, 250.0),
		t_SimpleVector(10.0, -10.0, 250.0),
		t_SimpleColor(1.0, 1.0, 1.0), 1.0);
	g_RenderBeam(
		t_SimpleVector(10.0, -10.0, 250.0),
		t_SimpleVector(-10.0, -10.0, 250.0),
		t_SimpleColor(1.0, 1.0, 1.0), 1.0);
	g_RenderBeam(
		t_SimpleVector(-10.0, -10.0, 250.0),
		t_SimpleVector(-10.0, 10.0, 250.0),
		t_SimpleColor(1.0, 1.0, 1.0), 1.0);
	g_RenderBeam(
		t_SimpleVector(-10.0, 10.0, 250.0),
		t_SimpleVector(10.0, 10.0, 250.0),
		t_SimpleColor(1.0, 1.0, 1.0), 1.0);
	g_RenderBeam(
		t_SimpleVector(10.0, 10.0, 200.0),
		t_SimpleVector(10.0, 10.0, 250.0),
		t_SimpleColor(1.0, 1.0, 1.0), 1.0);
	g_RenderBeam(
		t_SimpleVector(10.0, -10.0, 200.0),
		t_SimpleVector(10.0, -10.0, 250.0),
		t_SimpleColor(1.0, 1.0, 1.0), 1.0);
	g_RenderBeam(
		t_SimpleVector(-10.0, -10.0, 200.0),
		t_SimpleVector(-10.0, -10.0, 250.0),
		t_SimpleColor(1.0, 1.0, 1.0), 1.0);
	g_RenderBeam(
		t_SimpleVector(-10.0, 10.0, 200.0),
		t_SimpleVector(-10.0, 10.0, 250.0),
		t_SimpleColor(1.0, 1.0, 1.0), 1.0);

	// Additive beams.
	glBlendFunc(GL_ONE, GL_ONE);

	// Render red beams.
	g_RenderBeam(
		t_SimpleVector(-20.0, -30.0, 100.0),
		t_SimpleVector(-20.0, -30.0, 120.0),
		t_SimpleColor(1.0, 0.0, 0.0), 1.0);
	g_RenderBeam(
		t_SimpleVector(-20.0, -30.0, 140.0),
		t_SimpleVector(-20.0, -30.0, 160.0),
		t_SimpleColor(1.0, 0.0, 0.0), 1.0);
	g_RenderBeam(
		t_SimpleVector(-20.0, -30.0, 180.0),
		t_SimpleVector(-20.0, -30.0, 200.0),
		t_SimpleColor(1.0, 0.0, 0.0), 1.0);
	g_RenderBeam(
		t_SimpleVector(-20.0, -30.0, 220.0),
		t_SimpleVector(-20.0, -30.0, 240.0),
		t_SimpleColor(1.0, 0.0, 0.0), 1.0);
	g_RenderBeam(
		t_SimpleVector(20.0, -30.0, 100.0),
		t_SimpleVector(20.0, -30.0, 120.0),
		t_SimpleColor(1.0, 0.0, 0.0), 1.0);
	g_RenderBeam(
		t_SimpleVector(20.0, -30.0, 140.0),
		t_SimpleVector(20.0, -30.0, 160.0),
		t_SimpleColor(1.0, 0.0, 0.0), 1.0);
	g_RenderBeam(
		t_SimpleVector(20.0, -30.0, 180.0),
		t_SimpleVector(20.0, -30.0, 200.0),
		t_SimpleColor(1.0, 0.0, 0.0), 1.0);
	g_RenderBeam(
		t_SimpleVector(20.0, -30.0, 220.0),
		t_SimpleVector(20.0, -30.0, 240.0),
		t_SimpleColor(1.0, 0.0, 0.0), 1.0);

	// Render blue beams.
	g_RenderBeam(
		t_SimpleVector(-20.0, 30.0, 100.0),
		t_SimpleVector(-20.0, 30.0, 110.0),
		t_SimpleColor(0.0, 0.0, 1.0), 0.4);
	g_RenderBeam(
		t_SimpleVector(-20.0, 30.0, 120.0),
		t_SimpleVector(-20.0, 30.0, 130.0),
		t_SimpleColor(0.0, 0.0, 1.0), 0.4);
	g_RenderBeam(
		t_SimpleVector(-20.0, 30.0, 140.0),
		t_SimpleVector(-20.0, 30.0, 150.0),
		t_SimpleColor(0.0, 0.0, 1.0), 0.4);
	g_RenderBeam(
		t_SimpleVector(-20.0, 30.0, 160.0),
		t_SimpleVector(-20.0, 30.0, 170.0),
		t_SimpleColor(0.0, 0.0, 1.0), 0.4);
	g_RenderBeam(
		t_SimpleVector(20.0, 30.0, 100.0),
		t_SimpleVector(20.0, 30.0, 110.0),
		t_SimpleColor(0.0, 0.0, 1.0), 0.4);
	g_RenderBeam(
		t_SimpleVector(20.0, 30.0, 120.0),
		t_SimpleVector(20.0, 30.0, 130.0),
		t_SimpleColor(0.0, 0.0, 1.0), 0.4);
	g_RenderBeam(
		t_SimpleVector(20.0, 30.0, 140.0),
		t_SimpleVector(20.0, 30.0, 150.0),
		t_SimpleColor(0.0, 0.0, 1.0), 0.4);
	g_RenderBeam(
		t_SimpleVector(20.0, 30.0, 160.0),
		t_SimpleVector(20.0, 30.0, 170.0),
		t_SimpleColor(0.0, 0.0, 1.0), 0.4);

	// Render crossover beams.
	g_RenderBeam(
		t_SimpleVector(-40.0, 15.0, 150.0),
		t_SimpleVector(-40.0, -15.0, 250.0),
		t_SimpleColor(1.0, 0.0, 0.0), 2.0);
	g_RenderBeam(
		t_SimpleVector(-40.0, -15.0, 150.0),
		t_SimpleVector(-40.0, 15.0, 250.0),
		t_SimpleColor(0.0, 1.0, 0.0), 2.0);
	g_RenderBeam(
		t_SimpleVector(40.0, 15.0, 150.0),
		t_SimpleVector(40.0, -15.0, 250.0),
		t_SimpleColor(0.0, 1.0, 0.0), 2.0);
	g_RenderBeam(
		t_SimpleVector(40.0, -15.0, 150.0),
		t_SimpleVector(40.0, 15.0, 250.0),
		t_SimpleColor(1.0, 0.0, 0.0), 2.0);

	// End camera.
	glPopMatrix();

	// Perform all operations.
	glFlush();

	// Swap back buffer to front.
	auxSwapBuffers();
}

// Desc:	Callback function used during idle cycles.
void __stdcall g_IdleCallback()
{
	g_RenderScene();
}

// Desc:	Callback called when window is resized.
// Input:	i_width - New width.
//			i_height - New height.
void __stdcall g_ResizeCallback(int i_width, int i_height)
{
	// Resize viewport.
	glViewport(0, 0, i_width, i_height);

	// Update projection matrix.
	g_UpdateProjectionMatrix(i_width, i_height);
}

// Desc:	Initialize window and scene.
// Input:	i_width - Desired width.
//			i_height - Desired height.
// Output:	true if initialization is successful, false otherwise.
bool g_Initialize(int i_width, int i_height)
{
	// Default in the upper left corner.
	auxInitPosition(0, 0, i_width, i_height);

	// Full color double buffered display with Z buffer.
	auxInitDisplayMode(AUX_RGB | AUX_DEPTH | AUX_DOUBLE);

	// Initialize window and window title.
	auxInitWindow("Billboard Beam");

	// Set the function to call when application is idle.
	auxIdleFunc(g_IdleCallback);

	// Set the function to call when window is resized.
	auxReshapeFunc(g_ResizeCallback);

	// Set keyboard controls.
	auxKeyFunc(AUX_o, g_ToggleBeamOrientation);
	auxKeyFunc(AUX_q, g_CameraFasterCallback);
	auxKeyFunc(AUX_e, g_CameraSlowerCallback);
	auxKeyFunc(AUX_w, g_CameraForwardCallback);
	auxKeyFunc(AUX_s, g_CameraBackwardCallback);
	auxKeyFunc(AUX_d, g_CameraRightCallback);
	auxKeyFunc(AUX_a, g_CameraLeftCallback);
	auxKeyFunc(AUX_z, g_CameraUpCallback);
	auxKeyFunc(AUX_x, g_CameraDownCallback);
	auxKeyFunc(AUX_UP, g_CameraRotateUpCallback);
	auxKeyFunc(AUX_DOWN, g_CameraRotateDownCallback);
	auxKeyFunc(AUX_LEFT, g_CameraRotateLeftCallback);
	auxKeyFunc(AUX_RIGHT, g_CameraRotateRightCallback);

	// Clear to black furthest away.
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClearDepth(1.0);

	// Disable back-face culling.
	glDisable(GL_CULL_FACE);

	// Additive blending.
	glEnable(GL_BLEND);

	// Initialize projection matrix.
	g_UpdateProjectionMatrix(i_width, i_height);

	// Initialize texture.
	AUX_RGBImageRec *l_image = auxDIBImageLoad("beam.bmp");
	if(l_image) {
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, l_image->sizeX, l_image->sizeY,
			GL_RGB, GL_UNSIGNED_BYTE, l_image->data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
			GL_LINEAR);
		glEnable(GL_TEXTURE_2D);
	}

	// Initialization successful.
	return(true);
}

// Desc:	Main application loop.
void __stdcall g_MainLoop()
{
	g_RenderScene();
}

// Desc:	Application entry point.
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	// Initialize the window and scene settings.
	g_Initialize(640, 480);

	// Enter main loop.
	auxMainLoop(g_MainLoop);

	// Return success.
	return(0);
}
