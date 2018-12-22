#pragma once
#include <windows.h>
#include <time.h>

#include "scene.hpp"
#include "ScreenBuffer.hpp"
#include "SelectionTree.hpp"

//! Stochastic Iteration for Non-diffuse Global Illumination
class StIter
{
	//! pointer to the scene describing patches (triangles),
	//! materials, lights
	Scene*				m_pScene;


	//! ScreenBuffer is responsible for holding the
	//! calculated result for each pixel of the screen
	ScreenBuffer		m_screenBuffer;

	//! ImportanceTree allows fast (log n) selection
	//! and update for importance-based patch selection
	ImportanceTree*		m_pImportanceTree;

	//! PowerTree allows fast (log n) selection
	//! and update for power-based patch selection
	PowerTree*	m_pPowerTree;

	//! maximum number of iteration
	long				m_maxIteration;		

	//! current number of iteration
	long				m_nIteration;

	//! strategy selection probabilities
	//! m_probPatchSelection_Power+m_probPatchSelection_Importance+m_probPatchSelection_Mix=1.0f
	float m_probPatchSelection_Power, 
		m_probPatchSelection_Importance, 
		m_probPatchSelection_Mix;

	//! misc variables for time measuring
	clock_t time_start, time_end;
	float duration;
public:
	StIter(Scene* scene, long iteration, float power=0.8f, float importance=0.1f);
	~StIter();

	//! rendere image
	void render();

	//! save final result
	void saveImage();


private:
	//! temporary variable, calculating vertex colors based on patch colors
	Color* m_vertexColors;

	//! for each screen pixel, store the result of an
	//! eye-visible patch intersection test
	HitRec* m_VisibilityInfo;

	//! Determine visibility using special, OpenGL-generated image
	void DetermineVisibility();
	bool SetWindowPixelFormat(HDC hDC, DWORD dwFlags, BYTE colorBits);

	void CalculateDirectLight();
	Color SampleLightSources(const HitRec& hitRec, const Ray& ray);

	//! Performs the main iteration step
	bool DoIteration();

	//! Does the final scaling of the indirect illumination
	void FinalizeLtoEye();

	//! Calculate the average vertex color based on the neighbouring patches' color
	void CalculateVertexColors();

	//! OpenGL-Render patches using indirect illumination and add the
	//! resulting image to the screen buffer
	void MergeIndirectOpenGLImage();
	
	//! OpenGL-Render patches using indirect illumination
	void RBOpenGLRenderScene();

	//! Generate a random outgoing direction from the patch based on its radiance
	Vector GenerateRadianceDirection(long patchIndex, float& pdf);

	//! Generate a random outgoing direction from the patch based on its importance
	Vector GenerateImportanceDirection(long patchIndex, float& pdf);

	//! Generate a valid, normalized specular outgoing direction
	Vector GenerateSpecularDirection(Patch* p, float value, const Vector& indir, float& specPdf);

	//! Calculates the radiance of a patch into a specific outgoing direction
	Color getRadiance(long patchIndex, const Vector& outdir, Color& emission);

	//! Calculates the importance of a patch into a specific outgoing direction
	float getImportance(long patchIndex, const Vector& outdir);

	//! Randomized acceptance of the incoming radiance
	void Accept(Color& _old, Color& _new, Vector& vOld, Vector& vNew, Color& Ks );

	//! Randomized acceptance of the incoming importance
	void Accept(float& _old, float& _new, Vector& vOld, Vector& vNew);

	//! Calculates the probability of a given outdir, considering specular surface
	inline float CalculateSpecularDirectionProbability(Patch* p, float value, const Vector& indir, const Vector& outdir);
	inline float CalculateSpecularDirectionProbability(Patch* p, float value, const Vector& indir, const Vector& outdir, const Vector& idealReflDir);

	//! Conservative radiance direction probability calculation
	float RadianceDirectionProbability(Patch* p, const Vector& outdir);
	float ImportanceDirectionProbability(Patch* p, const Vector& outdir);

};
