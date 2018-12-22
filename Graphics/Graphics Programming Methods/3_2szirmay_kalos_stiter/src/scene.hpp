#pragma once

#include "world.hpp"
#include "phong.hpp"
#include "camera.hpp"
#include "kdtree.hpp"

//! Defines the scene to be rendered (patches, materials, lights).
//! Also allows to load a scene and save the resulting image
class Scene {
public:
	Scene(const char* lpszFileName, long samplesPerLight);
	~Scene();


protected:
	//! Set up the scene from file
	void Read(const char* path);
	void FinishInitializeScene();
	void FinishLights(long numSceneLights);

public:
	/*!@name Scene vertices */
	//@{
	Vector*		sceneVertices;
	Vector*		sceneVerticesNormals;
	long		nSceneVertices;
	//@}

	/*!@name  Scene patches */
	//@{
	Patch*		scenePatches;
	long		nScenePatches;
	//@}

	/*!@name Scene materials */
	//@{
	Material*	sceneMaterials;
	long		nSceneMaterials;
	//@}

	/*!@name Scene Brdf functions */
	//@{
	CPhongBrdf*	Brdfs;
	long		nBrdf;
	//@}

	/*!@name Scene Edf functions */
	//@{
	CPhongEdf*	Edfs;
	long		nEdf;
	//@}

	/*!@name Scene Lights and light samples */
	//@{
	Light*		sceneLights;
	long		nSceneLights;
	long		m_samplesPerLight;
	//@}

	//! contains the file name including the full path,
	//! excluding the .extension
	char		sceneFileName [_MAX_PATH];
	const char* getSceneFileName() { return sceneFileName; }

	//! Scene camera
	Camera		m_camera;

	//! Space Partitioning data structure allowing fast ray-patch intersection lookup
	KDTree*		m_pKDTree;
};
