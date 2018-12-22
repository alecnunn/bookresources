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
#include "math.h"
#include "skin.h"

typedef float color[4];

const color white = {1.0, 1.0, 1.0, 1.0};
const color black = {0.0, 0.0, 0.0, 1.0};
const color red = {1.0, 0.0, 0.0, 1.0};
const color green = {0.0, 1.0, 0.0, 1.0};
const color blue = {0.0, 0.0, 1.0, 1.0};
const color cyan = {0.0, 1.0, 1.0, 1.0};
const color yellow = {1.0, 1.0, 0, 1.0};
const color magenta = {1.0, 0, 1.0, 1.0};
const color grey = {0.7f, 0.7f, 0.7f, 1.0};
const color darkgrey = {0.5, 0.5, 0.5, 1.0};

//=========================================================================
// display a bidimensional grid with 20 divisions on XZ plane
void DisplayGrid(float dim, const color col);
// display each vertex normal with f as magnitude
void DisplayNormals(_Skin *pSkin, float ampl=6);
// Display the mesh
void DisplayMesh(_Skin *pSkin);
// display skeletal animated structure
void DisplaySkel(const _Bone &Bone);
//=========================================================================

//=========================================================================
GLvoid ReSizeGLScene(GLsizei width, GLsizei height)	;						// Resize And Initialize The GL Window
int InitGL(GLvoid);															// All Setup For OpenGL Goes Here
int DrawGLScene(GLvoid);													// Here's Where We Do All The Drawing
//=========================================================================
