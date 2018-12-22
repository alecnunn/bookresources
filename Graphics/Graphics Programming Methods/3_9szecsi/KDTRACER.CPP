// kdtracer.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include "RayTracer.h"

void main(int argc, char* argv[])
{
	RayTracer rt(300, 3);
	rt.createScene();
	rt.render();
	rt.saveImage("chess.tga");
}

