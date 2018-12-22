#pragma once

#include "KDTree.hpp"

class RayTracer
{
	int imageSize;
	int nPixels;
	int maxSteps;
	Vector* image;

	KDTree* kdtree;

	Vector eyePosition;
	Vector lookPosition;
	Intersectable** virtualWorldObjects;
	Vector* virtualWorldVertices;
	Light* virtualWorldLights;
	int nVirtualWorldObjects;
	int nVirtualWorldVertices;
	int nVirtualWorldLights;
public:
	RayTracer(int imageSize, int maxSteps)
	{
		virtualWorldVertices = NULL;
		virtualWorldLights = NULL;
		virtualWorldObjects = NULL;
		nVirtualWorldObjects = 0;
		nVirtualWorldVertices = 0;
		nVirtualWorldLights = 0;
		this->maxSteps = maxSteps;
		this->imageSize = imageSize;
		nPixels = imageSize * imageSize;
		image = new Vector[nPixels];
	}
	~RayTracer(void){delete image;}
	void createScene();
	void render();
	void saveImage(const char* filename);
};
