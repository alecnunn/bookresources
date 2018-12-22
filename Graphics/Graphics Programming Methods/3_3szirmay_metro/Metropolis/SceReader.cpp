// Graphics Programming Methods
// Metropolis sampling in global illumination
// György Antal, Lászlo Szirmay-Kalos
// 2003.

// open SCE scene files
// the SCE is an ad-hoc private text file format
// each line of the text contains exactly one data (e.g. Vector, Color, string)

#include <time.h>
#include "Metropolis.hpp"

char*	readBuffer		= NULL;	// the SCE file is readed completely and placed into this buffer
long	readBufferSize	= 0;	// the size of the readBuffer
long	readBufferPos	= 0;	// the current procession position of the readBuffer

// read a Vector value from the readBuffer
//-----------------------------------------------------------------
static Vector ReadVector(void)
{
	char* cp = strchr(readBuffer + readBufferPos, '\n');
	cp[0] = '\0';

	Vector v;
	sscanf(readBuffer + readBufferPos, "%f %f %f", &v.x, &v.y, &v.z);

	readBufferPos += strlen(readBuffer + readBufferPos) + 1;
	return v;
}

// read a Color value from the readBuffer
//-----------------------------------------------------------------
static Color ReadColor(void)
{
	char* cp = strchr(readBuffer + readBufferPos, '\n');
	cp[0] = '\0';

	float r, g, b;
	sscanf(readBuffer + readBufferPos, "%f %f %f", &r, &g, &b);

	readBufferPos += strlen(readBuffer + readBufferPos) + 1;
	return Color(r, g, b);
}

// read a double value from the readBuffer
//-----------------------------------------------------------------
static double ReadDouble(void)
{
	char* cp = strchr(readBuffer + readBufferPos, '\n');
	cp[0] = '\0';

	float f;
	sscanf(readBuffer + readBufferPos, "%f", &f);

	readBufferPos += strlen(readBuffer + readBufferPos) + 1;
	return f;
}

// read an integer value from the readBuffer
//-----------------------------------------------------------------
static int ReadInt(void)
{
	char* cp = strchr(readBuffer + readBufferPos, '\n');
	cp[0] = '\0';

	int d;
	sscanf(readBuffer + readBufferPos, "%d", &d);

	readBufferPos += strlen(readBuffer + readBufferPos) + 1;
	return d;
}

// read complete line (a string) from the readBuffer
//-----------------------------------------------------------------
static void ReadString(char* s)
{
	char* cp = strchr(readBuffer + readBufferPos, '\n');
	cp[0] = '\0';
	strcpy(s, readBuffer + readBufferPos);
	readBufferPos += strlen(readBuffer + readBufferPos) + 1;
}

// fill the readBuffer from file and process the readBuffer
//-----------------------------------------------------------------
void Scene::Read(const char* path)
{
	FILE* file		= fopen(path, "r+t");
	long result		= fseek(file, 0, SEEK_END);	// position to the end of the file
	long fSize		= ftell(file);				// calculate file size
	fseek(file, 0, SEEK_SET);					// position to the begin of the file again	

	readBuffer		= new char[fSize];			// allocate memory for the file data
	readBufferSize	= fread(readBuffer, sizeof(char), fSize, file);
	readBufferPos	= 0;

	char s[512];
	// read the camera section
	ReadString(s);
	camera.eyep = ReadVector();
	camera.updir = ReadVector();
	camera.lookp = ReadVector();
	camera.fov	= ReadDouble();
	camera.CompleteCamera ();

	//  read the materials section
	ReadString(s); //"Materials\n");
	long nMaterials = ReadInt();
	materials.resize(nMaterials);
	for (long i=0; i < materials.size(); i++) {
		materials[i].diffuseAlbedo = ReadColor();
		materials[i].specularAlbedo = ReadColor();
		materials[i].shine = ReadDouble();
	}

	// read the vertices section
	ReadString(s); //"vertices\n");
	long nVertices = ReadInt();
	vertices.resize(nVertices);
	for (long i=0; i < vertices.size(); i++) {
		vertices[i] = ReadVector();
	}

	// read the lights section
	ReadString(s); //"lights\n");
	long nLights = ReadInt();
	lights.resize(nLights);
	for (long i=0; i < lights.size(); i++) {
		lights[i].position = ReadVector();
		lights[i].emission = ReadColor();
	}

	// read the objects section
	ReadString(s); //"objects\n");
	long nObjects = ReadInt();
	objects.resize(nObjects);
	for (long i=0; i < objects.size(); i++) {
		long materialInd = ReadInt();
		long ai = ReadInt();
		long bi = ReadInt();
		long ci = ReadInt();
		ReadString(s); //"---\n");
		objects[i] = new Triangle(ai, bi, ci, materialInd);

		// 2. calculate normal
		Vector va = vertices[bi] - vertices[ai]; 
		Vector vb = vertices[ci] - vertices[bi];
		((Triangle*)objects[i])->normal = va % vb;
		((Triangle*)objects[i])->normal.Normalize();
		assert(!_isnan(((Triangle*)objects[i])->normal.x) && !_isnan(((Triangle*)objects[i])->normal.y) && !_isnan(((Triangle*)objects[i])->normal.z));
	}


	// finish scene
	for (long i = 0; i < vertices.size(); i++)
		vertexNormals.push_back(Vector(0.0, 0.0, 0.0));

	for (long i = 0; i < objects.size(); i++) {
		((Triangle*)objects[i])->a			= &vertices[((Triangle*)objects[i])->ai];
		((Triangle*)objects[i])->b			= &vertices[((Triangle*)objects[i])->bi];
		((Triangle*)objects[i])->c			= &vertices[((Triangle*)objects[i])->ci];
		((Triangle*)objects[i])->aN			= &vertexNormals[((Triangle*)objects[i])->ai];
		((Triangle*)objects[i])->bN			= &vertexNormals[((Triangle*)objects[i])->bi];
		((Triangle*)objects[i])->cN			= &vertexNormals[((Triangle*)objects[i])->ci];
		((Triangle*)objects[i])->material	= &materials[((Triangle*)objects[i])->materialInd];
		objects[i]->FinishObject();
	}

	totalLightEmissionLum = 0.0;
	for (long i = 0; i < lights.size (); i++) {
		totalLightEmissionLum += lights[i].emission.Lum();
	}

	// compute shading normals of vertices
	for (long i = 0; i < objects.size(); i++) {
		*(((Triangle*)objects[i])->aN) += ((Triangle*)objects[i])->normal;
		*(((Triangle*)objects[i])->bN) += ((Triangle*)objects[i])->normal;
		*(((Triangle*)objects[i])->cN) += ((Triangle*)objects[i])->normal;
	}

	for (long i = 0; i < vertices.size(); i++)
		vertexNormals[i].Normalize();

	fclose(file);			// close file
}