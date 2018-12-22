#include <string.h>
#include <direct.h>
#include <stdio.h>

#include "scene.hpp"

Scene::Scene(const char* lpszFileName, long samplesPerLight)
: m_samplesPerLight(samplesPerLight)
{
	m_pKDTree = NULL;

	sceneVertices=NULL;
	sceneVerticesNormals=NULL;
	nSceneVertices=0;

	scenePatches=NULL;
	nScenePatches=0;

	sceneMaterials=NULL;
	nSceneMaterials=0;

	sceneLights=NULL;
	nSceneLights=0;

	sceneFileName[0]=0;

	Brdfs=NULL;
	nBrdf=0;

	Edfs=NULL;
	nEdf=0;

	printf("Loading %s ... ", lpszFileName);
	Read(lpszFileName);
	printf("Done.\n");

	FinishInitializeScene();

//	sceneFileName
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath( lpszFileName, drive, dir, fname, ext );
	_makepath( sceneFileName, drive, dir, fname, "");
};


//!	put area light sources to light source list
inline void Scene::FinishLights (long numSceneLights)
{
	nSceneLights = numSceneLights;
	sceneLights = new Light[nSceneLights];
	
	long j=0;
	for (long i=0; i<nScenePatches; i++) {
		Patch* pPatch = &(scenePatches [i]);
		if (pPatch->m_pMaterial->m_edf != NULL) {
			sceneLights [j++].Init(pPatch, m_samplesPerLight);
		}
	}
}

//! Perform the final steps of scene initialization
void Scene::FinishInitializeScene()
{
	nSceneLights = 0;
	Intersectable** objects = new Intersectable*[nScenePatches];
	Patch::gMaxIndex = 0;
	for (long j=0; j<nScenePatches; j++) {
		scenePatches[j].m_index = Patch::gMaxIndex++;
		objects[j]=&(scenePatches[j]);
		if (scenePatches[j].m_pMaterial->m_edf != NULL) 
			nSceneLights++;
	}

	m_pKDTree = new KDTree(objects, nScenePatches);
	delete objects;

	FinishLights (nSceneLights);
}

Scene::~Scene()
{
	if (sceneVertices != NULL)
		delete[] sceneVertices;

	if (sceneVerticesNormals != NULL)
		delete[] sceneVerticesNormals;

	if (scenePatches != NULL)
		delete[] scenePatches;

	if (sceneMaterials)
		delete[] sceneMaterials;

	if (Brdfs) 
		delete[] Brdfs;

	if (Edfs) 
		delete[] Edfs;

	if (sceneLights != NULL)
		delete[] sceneLights;

	if (m_pKDTree) delete m_pKDTree;
}

char*	readBuffer		= NULL;
size_t	readBufferSize	= 0;
size_t	readBufferPos	= 0;

static void WriteVector(FILE* file, const Vector& v)
{
	char s[512];
	sprintf(s, "%f %f %f\n", v.x, v.y, v.z);
	fwrite(s, sizeof(char), strlen(s), file);
}

static Vector ReadVector(void)
{
	char* cp = strchr(readBuffer + readBufferPos, '\n');
	cp[0] = '\0';

	Vector v;
	sscanf(readBuffer + readBufferPos, "%f %f %f", &v.x, &v.y, &v.z);

	readBufferPos += strlen(readBuffer + readBufferPos) + 1;
	return v;
}

static void WriteColor(FILE* file, const Color& c)
{
	char s[512];
	sprintf(s, "%f %f %f\n", c.r, c.g, c.b);
	fwrite(s, sizeof(char), strlen(s), file);
}

static Color ReadColor(void)
{
	char* cp = strchr(readBuffer + readBufferPos, '\n');
	cp[0] = '\0';

	float r, g, b;
	sscanf(readBuffer + readBufferPos, "%f %f %f", &r, &g, &b);

	readBufferPos += strlen(readBuffer + readBufferPos) + 1;
	return Color(r, g, b);
}

static void WriteDouble(FILE* file, const double& f)
{
	char s[512];
	sprintf(s, "%f\n", f);
	fwrite(s, sizeof(char), strlen(s), file);
}

static double ReadDouble(void)
{
	char* cp = strchr(readBuffer + readBufferPos, '\n');
	cp[0] = '\0';

	float f;
	sscanf(readBuffer + readBufferPos, "%f", &f);

	readBufferPos += strlen(readBuffer + readBufferPos) + 1;
	return f;
}

static void WriteInt(FILE* file, int d)
{
	char s[512];
	sprintf(s, "%d\n", d);
	fwrite(s, sizeof(char), strlen(s), file);
}

static int ReadInt(void)
{
	char* cp = strchr(readBuffer + readBufferPos, '\n');
	cp[0] = '\0';

	int d;
	sscanf(readBuffer + readBufferPos, "%d", &d);

	readBufferPos += strlen(readBuffer + readBufferPos) + 1;
	return d;
}


static void WriteString(FILE* file, const char* s)
{
	fwrite(s, sizeof(char), strlen(s), file);
}

static void ReadString(char* s)
{
	char* cp = strchr(readBuffer + readBufferPos, '\n');
	cp[0] = '\0';
	strcpy(s, readBuffer + readBufferPos);
	readBufferPos += strlen(readBuffer + readBufferPos) + 1;
}


void Scene::Read(const char* path)
{
	FILE* file = fopen(path, "r+t");
	if (file==NULL) {
		printf("ERROR: unable to open file '%s'\n", path);
		exit(-1);
	}

	long result = fseek(file, 0, SEEK_END);
	long fSize = ftell(file);
	fseek (file, 0, SEEK_SET); 

	readBuffer = new char[fSize];
	readBufferSize = fread(readBuffer, sizeof(char), fSize, file);

	char s[512];
	// camera
	ReadString(s);
	m_camera.eyep	= ReadVector();
	m_camera.updir	= ReadVector();
	m_camera.lookp	= ReadVector();
	m_camera.fov	= (float)ReadDouble();
	m_camera.CompleteCamera ();

	ReadString(s); //"Materials\n");
	nEdf = nBrdf = nSceneMaterials = ReadInt();
	Brdfs = new CPhongBrdf[nBrdf];
	Edfs = new CPhongEdf[nEdf];
	sceneMaterials = new Material[nSceneMaterials];

	long i;
	for (i=0; i < nBrdf; i++) {
		Brdfs[i].Kd = ReadColor();	// diffuseAlbedo
		Brdfs[i].Ks = ReadColor();	// specularAlbedo
		Brdfs[i].Ns = (float)ReadDouble();	// shine
		Edfs[i].kd =  ReadColor();	// diffuse radiosity

		sceneMaterials[i].m_brdf = &Brdfs[i];
		if (Edfs[i].kd.Average()<EPSILON) sceneMaterials[i].m_edf = NULL;
		else sceneMaterials[i].m_edf = &Edfs[i];

		Brdfs[i].FinishPhong();
		Edfs[i].FinishPhong();
	}

	// vertices
	ReadString(s); //"vertices\n");
	nSceneVertices = ReadInt();
	sceneVertices = new Vector[nSceneVertices];
	sceneVerticesNormals = new Vector[nSceneVertices];
	for (i=0; i < nSceneVertices; i++) {
		sceneVertices[i] = ReadVector();
		sceneVerticesNormals[i].Set(0.0f, 0.0f, 0.0f);
	}


	// objects
	ReadString(s); //"objects\n");
	nScenePatches = ReadInt();
	scenePatches = new Patch[nScenePatches];
	for (i=0; i < nScenePatches; i++) {
		long materialInd = ReadInt();

		long ai = ReadInt();
		long bi = ReadInt();
		long ci = ReadInt();
		ReadString(s); //"---\n");

		scenePatches[i].a = &sceneVertices[ai];
		scenePatches[i].Na = &sceneVerticesNormals[ai];

		scenePatches[i].b = &sceneVertices[bi];
		scenePatches[i].Nb = &sceneVerticesNormals[bi];

		scenePatches[i].c = &sceneVertices[ci];
		scenePatches[i].Nc = &sceneVerticesNormals[ci];

		scenePatches[i].m_pMaterial = &sceneMaterials[materialInd];

		// 2. calculate normal
		Vector va = sceneVertices[bi] - sceneVertices[ai]; 
		Vector vb = sceneVertices[ci] - sceneVertices[bi];
		scenePatches[i].normal = va && vb;	//cross product
		scenePatches[i].normal.Normalize();
	}

	fclose(file);

	// finishScene
	for (i = 0; i < nScenePatches; i++) {
		scenePatches[i].Finish();
		scenePatches[i].CalculateArea ();
	}

	// compute shading normals of vertices
	for (i = 0; i < nScenePatches; i++) {
		*scenePatches[i].Na += scenePatches[i].normal;
		*scenePatches[i].Nb += scenePatches[i].normal;
		*scenePatches[i].Nc += scenePatches[i].normal;
	}

	for (i = 0; i < nSceneVertices; i++)
		sceneVerticesNormals[i].Normalize();
}
