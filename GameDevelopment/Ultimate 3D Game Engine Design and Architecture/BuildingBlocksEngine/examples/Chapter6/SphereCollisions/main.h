/*
   Sphere Collisions with Point Masses Demo - Chapter 6
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#ifndef _MAIN_H_
#define _MAIN_H_

// Includes...
#include<OpenGL.h>
#include<Input.h>

// Defines...
#define WINDOW_NAME        "Sphere Collisions with Point Masses Test"
#define WINDOW_CLASS       "UPGCLASS"
#define WINDOW_WIDTH       800
#define WINDOW_HEIGHT      600
#define WINDOW_FULLSCREEN  0

// Function prototypes...
void DemoResize(int width, int height);
bool DemoInitialize(bbe::RenderParams &params);
void DemoUpdate();
void DemoRender();
void DemoShutdown();

// Quit flag.
extern bool g_quitDemo;

#endif