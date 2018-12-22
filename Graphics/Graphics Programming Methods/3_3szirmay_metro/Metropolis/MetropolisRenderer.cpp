// Graphics Programming Methods
// Metropolis sampling in global illumination
// György Antal, Lászlo Szirmay-Kalos
// 2003.

// Implementation of the Metropolis Light Transfer Algorithm (see paper)

#include "MetropolisRenderer.hpp"
#include "KDTree.hpp"
#include <windows.h>
#include "resource.h"
extern HWND			gHwnd;			// the identifier of the window
extern short		gProgress;		// progression percent of the rendering (0..100)

// initialize the Renderer and set up the parameters of the rendering
//----------------------------------------------------------------------------
MetropolisRenderer::MetropolisRenderer(const Scene* pScene, short gSamplePerPixel, Image* pImage) 
{
	samplesPerPixel			= gSamplePerPixel;
	M						= samplesPerPixel * DefaultScreenWidth * DefaultScreenHeight;	//250*200*200 =  10M
	nSeedPaths				= 30000;
	pLarge					= 0.2;
	maxMutationDist			= 0.02;
	maxFollowedVerts		= 5;

	rayId					= 0;
	scene					= pScene;
	image					= pImage;
}

// generate the 0. vertex of the path
//----------------------------------------------------------------------------
void MetropolisRenderer::EyePointSample(void)
{
	eyeNode.m_hit.point		= scene->camera.eyep;
	eyeNode.m_hit.normal	= scene->camera.Z;
	eyeNode.m_pdfFromPrev	= 1.0; // differential eye area cancels out with computing the flux
	eyeNode.m_GfromPrev		= 1.0;
	eyeNode.m_bsdfEval.Set(1.0, 1.0, 1.0);
}

// generate the 1. vertex of the path
//----------------------------------------------------------------------------
bool MetropolisRenderer::PixelSample(void)
{
	PathNode&	pixelNode = newPath.verts[1];	// the vers[0] is the eye

	newPath.nUsedRandCoords = 2;
	double	pix_x = (PrimarySample(0) - 0.5) * scene->camera.pixh * scene->camera.hres;			// rand in [-width/2, +width/2]
	double	pix_y = (PrimarySample(1) - 0.5) * scene->camera.pixv * scene->camera.vres;	
	newPath.nx = (short)((pix_x + scene->camera.pixh * scene->camera.hres / 2.0) / scene->camera.pixh);
	newPath.ny = (short)((pix_y + scene->camera.pixv * scene->camera.vres / 2.0) / scene->camera.pixv);
	if (newPath.nx < 0 || newPath.nx >= scene->camera.hres || newPath.ny < 0 || newPath.ny >= scene->camera.vres)
		return false;	// outside of screen


	Vector rayDir = scene->camera.Z + pix_x * scene->camera.X + pix_y * scene->camera.Y;	// LinearCombination
	double	distPixel2 = rayDir.Norm() * rayDir.Norm();
	rayDir.Normalize ();
	Ray ray (scene->camera.eyep, rayDir); // ray from the eye

	HitRec	hitRec;
	ray.id = rayId++;
	scene->kdtree->traverse(ray, hitRec, 0.00001f, 100000000000.0f);
	if(!hitRec.object)
		return false; // path ends already

	pixelNode.m_hit			= hitRec;
	pixelNode.m_hit.normal	= hitRec.object->GetShadingNormal(pixelNode.m_hit.point);
	pixelNode.m_inDirTo		= ray.dir;

	double	cosPatch = -1.0 * (pixelNode.m_hit.normal * pixelNode.m_inDirTo);
	double	cosPixel = scene->camera.Z * pixelNode.m_inDirTo;
	if ((cosPixel <= 0) || (cosPatch <= 0))
		return false;

	Vector	tmpVec = pixelNode.m_hit.point - scene->camera.eyep;
	double	distPatch2 = tmpVec.Norm() * tmpVec.Norm();

	pixelNode.m_GfromPrev = cosPixel * cosPatch / distPatch2; // scene area measure

	// p1*dxdy = 1/ area_screen
	// p2*dw  = p1 / (cosPixel / distPixel2),  --> area pdf of screen converted to spherical angle pdf
	// p3*dxdy = p2 * (cosPatch / distPatch2)	--> as usual, (pdf of terszog converted to pdf of dx,dy)
	pixelNode.m_pdfFromPrev = (1.0 / (scene->camera.pixh * scene->camera.hres * scene->camera.pixv * scene->camera.vres)) * (distPixel2 / cosPixel) * (cosPatch / distPatch2);

	newPath.nVerts = 2;	// verts[0] is the eye, verts[1] is the pixelNode
	return true;
}

// generate the last vertex of the path
//----------------------------------------------------------------------------
bool MetropolisRenderer::LightSample(void)	// append a new light node at the end of the path
{
	PathNode&	lightNode = newPath.verts[newPath.nVerts];
	PathNode&	lastNode = newPath.verts[newPath.nVerts - 1];

	// use 1 random number for sampling a point light source
	float	rnd = PrimarySample(newPath.nUsedRandCoords++) * scene->totalLightEmissionLum;
	float	currentSum = scene->lights[0].emission.Lum();
	for (short i = 1; i < scene->lights.size(); i++) {
		if (rnd <= currentSum)
			break;
		currentSum += scene->lights[i].emission.Lum();
	}
	const Light&		light = scene->lights[--i];

	lightNode.m_hit.object		= NULL;	// point light source is used
	lightNode.m_hit.point		= light.position;
	lightNode.m_pdfFromPrev		= light.emission.Lum() / scene->totalLightEmissionLum; //

	// check visibility --> shoot a ray from the light source
	Ray ray (light.position, lastNode.m_hit.point - light.position);	// points from the lightSource
	double dist	= ray.dir.Norm();
	ray.dir.Normalize();

	double cosRay1 = -ray.dir * lastNode.m_hit.normal;
	double cosRay2 = 1;		// ray.dir.DotProduct (lightNode->m_normal) coincide
	if (cosRay1 <= 0)		//light source sees the back face
		return false;
	
	HitRec	hitRec;
	ray.id = rayId++;
	scene->kdtree->traverse(ray, hitRec, 0.00001f, 100000000000.0f);
	if (hitRec.object == NULL || hitRec.object != lastNode.m_hit.object)
		return false; // light cannot see the last node

	lightNode.m_hit.normal	= ray.dir;		// do we need it???
	lightNode.m_GfromPrev	= cosRay1; //  cosRay2 / dist / dist;
	lightNode.m_bsdfEval	= light.emission;
	lastNode.m_bsdfEval		= lastNode.m_hit.object->material->Brdf(lastNode.m_inDirTo, -ray.dir, lastNode.m_hit.normal);
	return true;
}

// generate one in-between vertex of the path
//----------------------------------------------------------------------------
bool MetropolisRenderer::SurfaceSample(void)
{
	PathNode& prevNode	= newPath.verts[newPath.nVerts - 2]; 
	PathNode& thisNode	= newPath.verts[newPath.nVerts - 1];
	PathNode& newNode	= newPath.verts[newPath.nVerts];

	double rnd1 = PrimarySample(newPath.nUsedRandCoords++); 
	double rnd2 = PrimarySample(newPath.nUsedRandCoords++); 
	double rnd3 = PrimarySample(newPath.nUsedRandCoords++);

	Material& thisMaterial = *thisNode.m_hit.object->material;

	double	pdfDir;
	thisMaterial.SampleOutdir(thisNode.m_inDirTo, thisNode.m_hit.normal, rnd1, rnd2, rnd3, &newNode.m_inDirTo, &pdfDir);
	if (pdfDir < 1e-6F)	// it is possible to sample with 0 probability, in which case, it is impossible to
		return false;		// get this sample, anyway: m_pdfFromPrev would be 0 also, and if we divide  by 0 it _isnan (x) == true

	Ray		ray(thisNode.m_hit.point, newNode.m_inDirTo);
	HitRec	hitRec;
	ray.id = rayId++;
	scene->kdtree->traverse(ray, hitRec, 0.00001f, 100000000000.0f);
	if (hitRec.object == NULL)
		return false; // there is not patch in this direction --> path cannot be continued
		
	newNode.m_hit			= hitRec;
	//newNode.m_hit.normal	= ((Triangle*)newNode.m_hit.object)->normal;
	newNode.m_hit.normal	= hitRec.object->GetShadingNormal(newNode.m_hit.point);

	thisNode.m_bsdfEval	= thisMaterial.Brdf(thisNode.m_inDirTo, newNode.m_inDirTo, thisNode.m_hit.normal);

	double	dist = (newNode.m_hit.point - thisNode.m_hit.point).Norm();
	double	cosa = thisNode.m_hit.normal * newNode.m_inDirTo;
	double	cosb = -1.0 * (newNode.m_hit.normal * newNode.m_inDirTo);

	newNode.m_pdfFromPrev	= pdfDir * cosb / (dist * dist);		// probability converted from solid angle to area
	newNode.m_GfromPrev		= cosa * cosb / (dist * dist);
	return true;
}

// generate the subpath of the path by iteratively calling SurfaceSample()
//----------------------------------------------------------------------------
void MetropolisRenderer::TraceEyePath(void)
{
	if (newPath.nVerts + 1 >= maxFollowedVerts)
		return;	// end of the recursion
	if (!SurfaceSample())
		return;	// Russion Roulette has ended the path

	newPath.nVerts++;
	TraceEyePath(); // return value not needed
}

// generate the 'newPath' by generating vertices
//-----------------------------------------------------------------
bool MetropolisRenderer::GenerateNewPath(void)
{
	newPath.radiance.Set(0, 0, 0);	// if it will be an invalid path
	newPath.I = 0.0;

	// 1. Eye point is sampled
	memcpy (&newPath.verts[0], &eyeNode, sizeof (PathNode)); 	// eye is always included
	newPath.nVerts = 1;

	// 2. pixelNode is sampled
	if (!PixelSample())
		return false;
	newPath.nVerts = 2;		// pixel is sampled

	// 3. other surfaceNode is sampled
	TraceEyePath();

	// 4. a light node is placed at the end of the path
	if (!LightSample())
		return false;
	newPath.nVerts++;

	// 5. evaluate radiance, pdf and from them the importance function
	newPath.radiance	= Color (1.0, 1.0, 1.0);
	newPath.pdfGen		= 1.0;
	for (short i = 1; i < newPath.nVerts; i++) {
		newPath.radiance *= newPath.verts[i].m_bsdfEval * newPath.verts[i].m_GfromPrev;
		newPath.pdfGen	 *= newPath.verts[i].m_pdfFromPrev;
	}
	newPath.I = newPath.radiance.Lum() / newPath.pdfGen;
	return true;
}

// update the pixel of the image affected by the path
//-----------------------------------------------------------------
void MetropolisRenderer::ContributePath(const Path& path, double weight)
{
	Color Fstar = path.radiance / path.pdfGen;
	if (Fstar.Lum() < 1e-9F)	// to avoid dividing by zero
		return;
	Color color = weight * (Fstar / Fstar.Lum()) * path.I;
	image->AddPixelRad (path.nx, path.ny, color);	// spike check??
}

// perform the Metropolis Light Transport rendering algorithm
//-----------------------------------------------------------------
bool MetropolisRenderer::RenderMLT(void)	// metropolis renderer
{
	// inizialize
	nAcceptedMutations = nRejectedMutations = 0;
	EyePointSample();	// for efficiency calculate just once, then copy to the generated paths
	time = large_step_time = 0;
	for (short j = 0; j < 3 * MaxPathVerts; j++) {
		u[j].value		= RAND;
		u[j].modifyTime = 0;	
	}

	// 1. First phase: a number of independent samples are generated. These samples are used to estimate 
	// the overall brightness of the image and to select seeds for the Metropolis phase.
	seedPaths = new Path[nSeedPaths];
	double sumI	= 0.0; 
	newPath.large_step = true;
	for (ULONG i = 0; i < nSeedPaths; i++) {
		GenerateNewPath();
		time++;
		while (!uStack.empty())
			uStack.pop();
		assert (newPath.I >= 0.0);
		sumI += newPath.I;
		seedPaths[i] = newPath;		// it is a little bit slow, but OK
	}
	double b = sumI / nSeedPaths;  // b is the average luminance of the image (see Veach thesis)

	// Select a seed path according to the importance of the path
	double rnd				= RAND * sumI;
	long selectedPathInd	= 0;
	double accumImportance	= 0.0;
	for (i = 0; i < nSeedPaths; i++) {
		accumImportance += seedPaths[i].I;
		if (accumImportance >= rnd) {
			selectedPathInd = i;
			break;
		}
	}

	// 2. Metropolis phase
	oldPath = seedPaths[selectedPathInd];
	for (ULONG i = 0; i < M; i++) {
		newPath.large_step = (RAND < pLarge);
		GenerateNewPath();
		double a = newPath.I / oldPath.I;
		if (a > 1.0) a = 1.0;
		double large_step		= (newPath.large_step) ? 1 : 0;
		double wNew				= (a+large_step) / (newPath.I/b + pLarge)/M;
		double wOld				= (1.0-a) / (oldPath.I/b+pLarge)/M;

		ContributePath(newPath, wNew);
		ContributePath(oldPath, wOld);

		if (RAND < a) {			// accept
			nAcceptedMutations++;
			if (newPath.large_step) large_step_time = time;
			time++;
			while (!uStack.empty()) uStack.pop();	//uStack.clear();
			oldPath = newPath;
		} else {		// reject: restore primary sample u
			nRejectedMutations++;
			short uIndex = newPath.nUsedRandCoords - 1;
			while (!uStack.empty()) {
				u[uIndex--] = uStack.top();
				uStack.pop();
			}
		}
		if (gProgress < i*100/M) {		// progressBar needs to be updated -> resraw the client area
			gProgress++;
			MSG msg;	// before redrowing the window, perform all messages in the message buffer
			while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
				if (msg.message == WM_QUIT) {
					PostQuitMessage(0);
					return true;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			RedrawWindow(gHwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
		}	
	}

	char s[256];
	long percent = (100.0 * (double)nAcceptedMutations)/((double)nAcceptedMutations + (double)nRejectedMutations);
	sprintf (s, "Accepted mutations: %d%s (%d, %d)", percent, "%", nAcceptedMutations, nRejectedMutations);
	::MessageBox(NULL, s, "Rendering has finished", MB_OK);
	return true;
}

// mutate a coordinate randomly in [0..1]
//-----------------------------------------------------------------
float MetropolisRenderer::MutateValue(float value) { // maxMutationDist = 0.03
	value += 2 * maxMutationDist * (RAND - 0.5);	 // [-0.03, + 0.03]	
	if (value > 1.0) value -= 1.0;
	if (value < 0.0) value += 1.0;
	return value;
}

// lazy evaluation of a coordinate in the primary sample space
//-----------------------------------------------------------------
float MetropolisRenderer::PrimarySample(short i) { // gets the ith coordinate of the primary sample
	if (u[i].modifyTime < time) {
		if (newPath.large_step) {				// large step
			uStack.push(u[i]);    				// save state
			u[i].value		= RAND;				// large steps are independent 
			u[i].modifyTime	= time;				// modified now
		} else {            					// small step
			if (u[i].modifyTime < large_step_time) {
				u[i].value		= RAND;
				u[i].modifyTime	= large_step_time;
			}
			while (u[i].modifyTime < time-1) { // lazy evaluation
				u[i].value = MutateValue(u[i].value);
				u[i].modifyTime++;
			}
			uStack.push(u[i]);    				// save state
			u[i].value		= MutateValue(u[i].value);
			u[i].modifyTime	= time;				// modified now
		}
	}
	return u[i].value;
}
