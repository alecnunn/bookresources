#include "StdAfx.h"
#include "raytracer.h"


void RayTracer::createScene()
{
	eyePosition = Vector(-1.0f, -1.0f, 1.0f);
	lookPosition = Vector(0.5f, 0.5f, 0.0f);

	nVirtualWorldLights = 2;
	virtualWorldLights = new Light[2];
	nVirtualWorldVertices = 81;
	virtualWorldVertices = new Vector[nVirtualWorldVertices];
	nVirtualWorldObjects = 128 + 1;
	virtualWorldObjects = new Intersectable*[nVirtualWorldObjects];

	virtualWorldLights[0] = Light(Vector(0.0f, 1.0f, 1.0f), Vector(0.9f, 0.6f, 0.3f));
	virtualWorldLights[1] = Light(Vector(1.0f, 0.0f, 1.0f), Vector(0.3f, 0.6f, 0.9f));

	int iVertex = 0;
	for(int x=0; x<9; x++)
		for(int y=0;y<9;y++)
			virtualWorldVertices[iVertex++] = Vector(x / 8.0f, y / 8.0f, 0.0f);

	int iObject = 0;
	for(int a=0; a<8; a++)
		for(int b=0; b<8; b++)
		{
			virtualWorldObjects[iObject++] = new Triangle(
						virtualWorldVertices + a + 1 + b * 9,
						virtualWorldVertices + a + b * 9,
						virtualWorldVertices + a + (b + 1) * 9,
						((a+b)%2)?&Material::DIFFUSEWHITE:&Material::SHINYBLACK);
			virtualWorldObjects[iObject++] = new Triangle(
						virtualWorldVertices + a + 1 + b * 9,
						virtualWorldVertices + a + (b + 1) * 9,
						virtualWorldVertices + a + 10 + b * 9,
						((a+b)%2)?&Material::DIFFUSEWHITE:&Material::SHINYBLACK);
		}

	virtualWorldObjects[iObject++] = new Sphere(Vector(0.5f, 0.5f, 0.31f), 0.2f, &Material::STEEL);

	kdtree = new KDTree(virtualWorldObjects, nVirtualWorldObjects);

}

void RayTracer::render()
{
	float pixelEdgeLength = 2.0f * tanf(3.14159265358979323846f / 8.0f) / imageSize;
	Vector upwards(0.0f, 0.0f, 1.0f);
	Vector eyeCoordsZ = lookPosition - eyePosition;
	eyeCoordsZ.Normalize();
	Vector eyeCoordsX = eyeCoordsZ && upwards;
	Vector eyeCoordsY = eyeCoordsZ && eyeCoordsX;

	int rayId = 1;
	Ray ray;
	HitRec hitRec;
	int halfImageSize = imageSize / 2;
	for(int j=-halfImageSize; j < halfImageSize; j++)
		for(int k=-halfImageSize; k < halfImageSize; k++)
		{
			Vector radianceInPixel((float)(halfImageSize + j) / (float)imageSize, (float)(halfImageSize + k) /(float)imageSize, 0.5f);
			Vector idealBrdf(1.0f, 1.0f, 0.1f);
			ray.origin = eyePosition;
			ray.dir =   eyeCoordsX * (pixelEdgeLength * k) +
						eyeCoordsY * (pixelEdgeLength * j) +
						eyeCoordsZ;
			ray.dir.Normalize();
			ray.id = rayId++;

			for(int depth=0; depth < maxSteps; depth++)
			{
				kdtree->traverse(ray, hitRec, 0.00001f, 100000000000.0f);
				if(!hitRec.object)
					break;
				Vector normal = hitRec.object->getShadingNormal(hitRec.point);
				radianceInPixel = Vector(0.0f, 0.0f, 0.0f);
				Ray shadowRay;
				shadowRay.origin = hitRec.point;
				for(int iLight=0; iLight < nVirtualWorldLights; iLight++)
				{
					shadowRay.id = rayId++;
					shadowRay.dir = virtualWorldLights[iLight].position - shadowRay.origin;
					float distance = shadowRay.dir.Normalize();
					HitRec shadowHit;
					kdtree->traverse(shadowRay, shadowHit, 0.00001f, distance);
					if(!shadowHit.isIntersect)
					{
						Vector lightBrdfValue;
						hitRec.object->material->EvalBrdfCosTheta(ray.dir, shadowRay.dir, normal, lightBrdfValue);
						radianceInPixel += (idealBrdf % lightBrdfValue % virtualWorldLights[iLight].radiance) * (1.0 / (distance * distance));
					}
				}
				if((depth < maxSteps - 1) && hitRec.object->material->isMirror())
				{
					ray.origin = hitRec.point;
					Vector reflectionDir;
					reflectionDir.SetIdealReflectedDirection(ray.dir, normal);
					Vector idealBrdfValue;
					hitRec.object->material->EvalReflectivity(idealBrdfValue);
					idealBrdf %= idealBrdfValue;
					ray.dir = reflectionDir;
				}
			}
			image[(halfImageSize + j) * imageSize + (halfImageSize + k)] = radianceInPixel;
		}

}

void RayTracer::saveImage(const char* filename)
{
	FILE* file = fopen(filename, "w+b");

	fputc(0,file);
	fputc(0,file);
	fputc(2,file);
	for(int i=3;i<12;i++) 
		fputc(0,file);
	fputc(imageSize&0xff,file);
	fputc(imageSize/256,file);
	fputc(imageSize&0xff,file);
	fputc(imageSize/256,file);
	fputc(24,file);
	fputc(32,file);

	for(int y = 0; y < nPixels; y++ )
	{
		float tone = 200.0;
		unsigned int red = image[y].r * tone;
		unsigned int green = image[y].g * tone;
		unsigned int blue = image[y].b * tone;

		if (blue > 255) blue = 255;
		if (green > 255) green = 255;
		if (red > 255) red = 255;

		fputc(blue,file);
		fputc(green,file);
		fputc(red,file);
	}
	fclose(file);
}