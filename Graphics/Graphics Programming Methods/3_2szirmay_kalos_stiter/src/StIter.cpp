#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <stdio.h>

#include "stiter.hpp"
#include "SelectionTree.hpp"

StIter::StIter(Scene* scene, long iteration, float power, float importance)
: m_pScene(scene), m_maxIteration(iteration)
{
	printf("Start rendering...\n"
		"# of iteration=%d\n"
		"samples per light=%d\n"
		"power=%d%% importance=%d%% mixed=%d%%\n\n", 
		m_maxIteration, m_pScene->m_samplesPerLight, 
		(long)(power*100), (long)(importance*100), (long)((100.5f-(power+importance)*100)) );

	time_start = clock ();	

	m_screenBuffer.Initialize(&m_pScene->m_camera);

	m_probPatchSelection_Power	= power;
	m_probPatchSelection_Importance = importance;
	m_probPatchSelection_Mix = (1.0f - m_probPatchSelection_Importance - m_probPatchSelection_Power);

	m_VisibilityInfo=NULL;
	m_vertexColors = new Color[m_pScene->nSceneVertices];

	for (long i=0; i<m_pScene->nScenePatches; i++) {
		Patch* p=&m_pScene->scenePatches[i];
		p->Reset();

		p->m_VisDir = m_pScene->m_camera.eyep - p->midPoint;
		p->m_VisDir.Normalize();
		p->dirW = p->m_EyePatchDir = -p->m_VisDir;
	}
}

StIter::~StIter()
{
	delete m_pImportanceTree;
	delete m_pPowerTree;
	delete[] m_vertexColors;
	delete[] m_VisibilityInfo;
}


bool StIter::SetWindowPixelFormat(HDC hDC, DWORD dwFlags, BYTE colorBits) {

	PIXELFORMATDESCRIPTOR pixelDesc;

	pixelDesc.nSize		= sizeof(PIXELFORMATDESCRIPTOR);
	pixelDesc.nVersion	= 1;
	pixelDesc.dwFlags			= dwFlags;
	pixelDesc.iPixelType		= PFD_TYPE_RGBA;
	pixelDesc.cColorBits		= colorBits;
	pixelDesc.cRedBits			= 8;
	pixelDesc.cRedShift			= 16;
	pixelDesc.cGreenBits		= 8;
	pixelDesc.cGreenShift		= 8;
	pixelDesc.cBlueBits			= 8;
	pixelDesc.cBlueShift		= 0;
	pixelDesc.cAlphaBits		= 0;
	pixelDesc.cAlphaShift		= 0;
	pixelDesc.cAccumBits		= 64;	
	pixelDesc.cAccumRedBits		= 16;
	pixelDesc.cAccumGreenBits	= 16;
	pixelDesc.cAccumBlueBits	= 16;
	pixelDesc.cAccumAlphaBits	= 0;
	pixelDesc.cDepthBits		= 32;
	pixelDesc.cStencilBits		= 8;
	pixelDesc.cAuxBuffers		= 0;
	pixelDesc.iLayerType		= PFD_MAIN_PLANE;
	pixelDesc.bReserved			= 0;
	pixelDesc.dwLayerMask		= 0;
	pixelDesc.dwVisibleMask		= 0;
	pixelDesc.dwDamageMask		= 0;

	int openGLPixelIndex = ::ChoosePixelFormat (hDC, &pixelDesc);
	if (openGLPixelIndex==0) { // Let's choose a default index. 
		openGLPixelIndex = 1;    
		if (DescribePixelFormat (hDC, openGLPixelIndex, sizeof(PIXELFORMATDESCRIPTOR), &pixelDesc) == 0)
			return false;
	}
	
	if (SetPixelFormat (hDC, openGLPixelIndex, &pixelDesc) == false)
		return false;
	
	return true;
}

#define RED(x) 			((x) & 0xff);
#define GREEN(x)		(((x) >> 8) & 0xff) 
#define BLUE(x)			(((x) >> 16) & 0xff)
#define INT(r, g, b)	(((r) + ((g) << 8) + ((b) << 16)))
/*! Determine visibility by rendering the scene using unique color for each patch.
This clearly shows in which pixel which patch can be seen.*/
void StIter::DetermineVisibility()
{
	printf("Determining visibility...\n");
	unsigned char* Image= new unsigned char[3 * (m_pScene->m_camera.hres + 1) * (m_pScene->m_camera.vres + 1)];

	BYTE* m_pBitmapBits;


	//! memory device context
	HDC	DC = ::CreateCompatibleDC(NULL);

	BITMAPINFO				bmi;
	memset (&bmi, 0, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth			= m_pScene->m_camera.hres;
	bmi.bmiHeader.biHeight			= m_pScene->m_camera.vres;
	bmi.bmiHeader.biPlanes			= 1;
	bmi.bmiHeader.biBitCount		= 24;
	bmi.bmiHeader.biCompression		= BI_RGB;
	bmi.bmiHeader.biSizeImage		= m_pScene->m_camera.hres * m_pScene->m_camera.vres * 3;

	//! the DIB format bitmap
	HBITMAP	hDib = ::CreateDIBSection (DC, &bmi, DIB_RGB_COLORS, (void**)&m_pBitmapBits, NULL, (DWORD)0);
	SelectObject (DC, hDib);
	
	bool res=SetWindowPixelFormat (DC, PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL | PFD_STEREO_DONTCARE, 24);
	//! memory rendering context
	HGLRC RC = wglCreateContext (DC);

	if (RC==NULL) 
		return;

	if (wglMakeCurrent (DC, RC)==FALSE) return;

	// Default mode
	glEnable (GL_NORMALIZE);
	
	// Lights, material properties
	glClearColor (0, 0, 0, 0);
	glClearDepth (1.0);
	
	glEnable (GL_CULL_FACE);
	glPolygonMode (GL_FRONT, GL_FILL);		

	glShadeModel (GL_FLAT);
    glEnable (GL_DEPTH_TEST);	// z-buffer
    glDisable (GL_DITHER);		// no dither
	
	glDisable (GL_LIGHTING);
	glDrawBuffer (GL_FRONT);

	GLsizei width = m_pScene->m_camera.hres;
	GLsizei height = m_pScene->m_camera.vres;
	GLfloat aspect;
	if (height == 0)
		aspect = (GLfloat) width;
	else
		aspect = (GLfloat) width / (GLfloat) height;

	glViewport (0, 0, width, height);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective (m_pScene->m_camera.vfov * 2.0, aspect, m_pScene->m_camera.nearClip, m_pScene->m_camera.farClip);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gluLookAt (m_pScene->m_camera.eyep.x, m_pScene->m_camera.eyep.y ,m_pScene->m_camera.eyep.z,
		m_pScene->m_camera.lookp.x, m_pScene->m_camera.lookp.y, m_pScene->m_camera.lookp.z,
		m_pScene->m_camera.updir.x, m_pScene->m_camera.updir.y, m_pScene->m_camera.updir.z);

	// set up scene
	glPushMatrix ();

	glBegin (GL_TRIANGLES);
	for (long k = 0; k < m_pScene->nScenePatches; k++) {
		// glClearColor is set to black (0.0f, 0.0f, 0.0f)
		// therefore the first availabe color index will have an offset of 1 
		GLubyte	r = (GLubyte)RED (k+1);
		GLubyte	g = (GLubyte)GREEN (k+1); 
		GLubyte	b = (GLubyte)BLUE (k+1); 

		glColor3ub (r, g, b);

		glVertex3d (m_pScene->scenePatches[k].a->x, m_pScene->scenePatches[k].a->y, m_pScene->scenePatches[k].a->z);
		glVertex3d (m_pScene->scenePatches[k].b->x, m_pScene->scenePatches[k].b->y, m_pScene->scenePatches[k].b->z);
		glVertex3d (m_pScene->scenePatches[k].c->x, m_pScene->scenePatches[k].c->y, m_pScene->scenePatches[k].c->z);
	}
	glEnd ();


	glPopMatrix ();
	::glFlush();

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels (0, 0, m_pScene->m_camera.hres, m_pScene->m_camera.vres, GL_RGB, GL_UNSIGNED_BYTE, &Image[0]);

	// make the rendering context not current
	wglMakeCurrent (NULL, NULL);

	// delete the rendering context 
	wglDeleteContext (RC);
	DeleteDC (DC);
	DeleteObject (hDib);


	if (m_VisibilityInfo) delete m_VisibilityInfo;
	m_VisibilityInfo = new HitRec[m_pScene->m_camera.vres*m_pScene->m_camera.hres];

	Ray ray;
	ray.origin = m_pScene->m_camera.eyep;	
	float&	h = m_pScene->m_camera.pixh;
	float&	v = m_pScene->m_camera.pixv;

	// calculate the middle of the pixel
	float	pix_x = (float)(-h * width  / 2.0f + h/2.0f);
	float	pix_y = (float)(-v * height / 2.0f + v/2.0f);

	for (long y = 0; y < m_pScene->m_camera.vres; y++ ) {
		for (long x = 0; x < m_pScene->m_camera.hres; x++) {
			//OpenGL image is upside-down
			long i=3*((m_pScene->m_camera.vres-y-1)*m_pScene->m_camera.hres+x);

			// don't forget about the offset in patch-color indexing!
			long j=INT(Image[i], Image[i+1], Image[i+2])-1;

			if (j>=0 && j<m_pScene->nScenePatches ) {
				long pixelID = y*m_pScene->m_camera.hres+x;

				m_pScene->scenePatches[j].visible=true;
				m_VisibilityInfo[pixelID].object = &m_pScene->scenePatches[j];

				//Calculate m_VisibilityInfo[pixelID].depth, m_VisibilityInfo[pixelID].point
				ray.dir.SetLinearCombination (m_pScene->m_camera.Z, pix_x+x*h, m_pScene->m_camera.X, pix_y+y*v, m_pScene->m_camera.Y);
				ray.dir.Normalize();
				ray.id = ++ray.maxRayID;

				m_pScene->scenePatches[j].CalculateIntersection(ray, m_VisibilityInfo[pixelID]);

				//Assign screen pixels to their patch
				m_pScene->scenePatches[j].nScreenPixel++;
			}
			else {
				m_VisibilityInfo[y*m_pScene->m_camera.hres+x].object = NULL;
			}
		}
	}

	delete Image;
}

void StIter::CalculateDirectLight()
{
	printf("Calculating direct illumination...\n");
	Ray ray;
	ray.origin = m_pScene->m_camera.eyep;	

	for (int y = 0; y < m_pScene->m_camera.vres; y++) {
		for (int x = 0; x < m_pScene->m_camera.hres; x++) {
			Color thisRgb = Color::BLACK;
			long pixelID=y*m_pScene->m_camera.hres+x;
			Patch* patch = (Patch*)m_VisibilityInfo[pixelID].object;

			if (patch) {
				const Material* pMaterial = patch->m_pMaterial;

				// emission color of the visible surface point
				if (pMaterial->m_edf != NULL)
					thisRgb = pMaterial->m_edf->GetEmissionRad();

				ray.dir=(m_VisibilityInfo[pixelID].point-ray.origin)/m_VisibilityInfo[pixelID].depth;
				thisRgb+=SampleLightSources (m_VisibilityInfo[pixelID], ray);
			} else {
				// NOP - No patch is visible in the pixel
			}
			m_screenBuffer.SetRadiance (x, y, thisRgb);
		}
	}
}

Color StIter::SampleLightSources(const HitRec& hitRec, const Ray& ray)
{
	Color result=Color::BLACK;
	if (!hitRec.object) return result;

	long visiblePatchID = hitRec.object->m_index;

	//diffuse brdf
	Color kd=m_pScene->scenePatches[visiblePatchID].m_pMaterial->m_brdf->GetDiffuseBrdf();

	Patch* patch=(Patch*)hitRec.object;

	Ray		rayToLight;
	Color		L=Color::BLACK;
	Color		diffuse=Color::BLACK;

	for (long i = 0; i < m_pScene->nSceneLights;  i++) {
		L = Color::BLACK;
		diffuse = Color::BLACK;

		long nSample = m_pScene->sceneLights[i].m_pSamples->m_nSamples;

		Patch* lightPatch=m_pScene->sceneLights[i].m_pPatch;
		const Material* pMaterialLight = lightPatch->m_pMaterial;
		const Material* pMaterialPatch = patch->m_pMaterial;
		const Color emission=pMaterialLight->m_edf->GetEmissionRad();

		for (long j=0; j< nSample; j++) {
			// a ray from the surface point to the middle point of the lightsource
			rayToLight.id = ++Ray::maxRayID;
			rayToLight.origin	= hitRec.point;
			rayToLight.dir = m_pScene->sceneLights[i].m_pSamples->samples[j] - hitRec.point;

			// the distance of the surface point and the point of the lightsource
			float	lightDist = rayToLight.dir.Norm ();
			rayToLight.dir *= (1.0f/lightDist);

			// the shading normal at the surface point
			Vector	shadingNormal=patch->getShadingNormal(hitRec.point);

			float	cost = rayToLight.dir * shadingNormal;	//dot product

			// whether or not the front face of the surface is visible from the lightsource
			if (cost > 0 && (rayToLight.dir * patch->normal)>0)  {
				HitRec	hitRecToLight;
				hitRecToLight.object = NULL;

				m_pScene->m_pKDTree->traverse(rayToLight, hitRecToLight, 0.0f, lightDist+2*EPSILON);
				if (hitRecToLight.object==NULL) continue;

				if (hitRecToLight.object->m_index == visiblePatchID) {
					printf("I've hit myself!\n");
				}

				if (hitRecToLight.object==lightPatch) {
					// we should * -1.0 the ray.dir, but do it after make dotProduct
					float	cosLightFace = - (rayToLight.dir * lightPatch->normal);

					// whether or not the front face of the lightsource is visible from the surface
					if (cosLightFace > 0) {
						Color	brdf=pMaterialPatch->m_brdf->EvalBrdf (ray.dir, rayToLight.dir, shadingNormal);

						// the color from the lightsource reflected to the eye
						Color I=emission * (cost * cosLightFace * lightPatch->area / (lightDist * lightDist));
						L+=(I % brdf);	// blend the values

						//Initialize diffuse value
						diffuse += I;
					};
				};
			}	// if cost
		} // samples per light
		result += L / (float)nSample;

		//Initialize diffuse value
		m_pScene->scenePatches[visiblePatchID].Ld  += (diffuse % kd) / (float)(nSample * m_pScene->scenePatches[visiblePatchID].nScreenPixel);
	}	// for

	return result;
}

void StIter::render()
{
	DetermineVisibility();

	m_pPowerTree = new PowerTree;
	m_pPowerTree->Build(m_pScene->scenePatches, m_pScene->nScenePatches);

	//To build the importance tree, a preceding call to DetermineVisibility() is a must!
	m_pImportanceTree = new ImportanceTree;
	m_pImportanceTree->Build(m_pScene->scenePatches, m_pScene->nScenePatches);

	//Calculate direct illumination
	CalculateDirectLight();

//enable this if you want to save the direct illumination image
//into a separate image file
#ifdef SAVE_DIRECT_ILLUMINATION
	m_screenBuffer.Syncronize();
	m_screenBuffer.saveImage("direct.tga");
#endif

	printf("Calculating indirect illumination...\n");
	for (m_nIteration=1; m_nIteration<m_maxIteration; m_nIteration++) {
		DoIteration();
	}

	m_nIteration--; //set properly the number of completed iterations

	FinalizeLtoEye ();
	CalculateVertexColors ();	
	MergeIndirectOpenGLImage();

	time_end = clock ();
	duration = (float)(time_end - time_start) / CLOCKS_PER_SEC;
	printf("Finished rendering.\n\nElapsed time=%2.4f seconds\n",  duration);
}

void StIter::saveImage()
{
	char fname[_MAX_PATH];
	_snprintf(fname, _MAX_PATH, "%s_%d_%d_%d_%2.0fsec_%d_%d.tga", 
		m_pScene->getSceneFileName(),
		m_pScene->nScenePatches,
		m_maxIteration, m_pScene->m_samplesPerLight, duration,
		(long)(m_probPatchSelection_Power*100), (long)(m_probPatchSelection_Importance*100));

	printf("Saving image %s\n", fname);
	m_screenBuffer.Syncronize();
	m_screenBuffer.saveImage(fname);
}

bool StIter::DoIteration()
{
	long i; // receiver patch ID
	long j;	// source patch ID

	Vector x;	// point on the receiver patch
	Vector y;	// point on the source patch

	Vector w;	// normalized direction vector pointing (y->x)

	HitRec hitRec;
	hitRec.object = NULL;
	Ray ray;

	float G;	// geometric factor
	float costhetax, costhetay;

	float p1, p2;

	//w always points y->x (j->i)
	//_w points x->y (i->j)
	Vector _w; // = -w;

	float r=RAND;
	if (r<m_probPatchSelection_Power) {
		//Strategy #1
		j = m_pPowerTree->FindDistrib(RAND, m_nIteration);

		//select a point on the source patch
		y=m_pScene->scenePatches[j].PointInTriangle();

		float pdf_j;
		w=GenerateRadianceDirection(j, pdf_j);

		ray.id = ++ray.maxRayID;
		ray.origin=y;
		ray.dir=w;

		m_pScene->m_pKDTree->traverse(ray, hitRec, 0.0f, FLT_MAX);

		if(hitRec.object == NULL) return false;

		i=hitRec.object->m_index;
		x=hitRec.point;

		//Calculate DotProducts
		costhetax = -( w * m_pScene->scenePatches[i].normal );	
		costhetay =  ( w * m_pScene->scenePatches[j].normal );

		// are the patches facing towards each other?
		if (costhetay<0.0f || costhetax<0.0f) return false;

		Vector yx = x - y;
		G=costhetax*costhetay/yx.Norm2();
		_w = -w;

		float pPatchSelectionRj=m_pPowerTree->getValue2(j, m_nIteration)/m_pPowerTree->getRootValue(m_nIteration);
		float pPatchSelectionRi=m_pPowerTree->getValue2(i, m_nIteration)/m_pPowerTree->getRootValue(m_nIteration);
		float pdf_i = RadianceDirectionProbability(&m_pScene->scenePatches[i], _w);
		p1 = pPatchSelectionRj * pdf_j / (costhetay*m_pScene->scenePatches[j].area) +
			pPatchSelectionRi * pdf_i / (costhetax*m_pScene->scenePatches[i].area);

		pPatchSelectionRj=m_pImportanceTree->getValue2(j, m_nIteration)/m_pImportanceTree->getRootValue(m_nIteration);
		pPatchSelectionRi=m_pImportanceTree->getValue2(i, m_nIteration)/m_pImportanceTree->getRootValue(m_nIteration);
		pdf_j = ImportanceDirectionProbability(&m_pScene->scenePatches[j], w);
		pdf_i = ImportanceDirectionProbability(&m_pScene->scenePatches[i], _w);
		p2 = pPatchSelectionRj * pdf_j / (costhetay*m_pScene->scenePatches[j].area) +
			pPatchSelectionRi * pdf_i / (costhetax*m_pScene->scenePatches[i].area);
	}
	else if (r<m_probPatchSelection_Power+m_probPatchSelection_Importance) {
		//Strategy #2
		i = m_pImportanceTree->FindDistrib(RAND, m_nIteration);

		//select a point on the receiver patch
		x = m_pScene->scenePatches[i].PointInTriangle();

		ray.id = ++ray.maxRayID;
		ray.origin=x;

		float pdf_i;
		ray.dir=GenerateImportanceDirection(i, pdf_i);	//in this case, ray.dir points x->y;
		w=-ray.dir;								//w always points y->x

		m_pScene->m_pKDTree->traverse(ray, hitRec, 0.0f, FLT_MAX);
		if(hitRec.object == NULL) return false;

		j=hitRec.object->m_index;
		// check if by chance I hit myself due to KD-Tree numerical error
		if (i==j) return false;	
		y=hitRec.point;

		//calculate DotProducts
		costhetax = -( w * m_pScene->scenePatches[i].normal );	
		costhetay =  ( w * m_pScene->scenePatches[j].normal );

		// are the patches facing towards each other?
		if (costhetay<0.0f || costhetax<0.0f) 	return false;

		Vector yx = x - y;
		G=costhetax*costhetay/yx.Norm2();


		_w = -w;

		float pPatchSelectionRj=m_pImportanceTree->getValue2(j, m_nIteration)/m_pImportanceTree->getRootValue(m_nIteration);
		float pPatchSelectionRi=m_pImportanceTree->getValue2(i, m_nIteration)/m_pImportanceTree->getRootValue(m_nIteration);
		float pdf_j = ImportanceDirectionProbability(&m_pScene->scenePatches[j], w);
		p2 = pPatchSelectionRj * pdf_j / (costhetay*m_pScene->scenePatches[j].area) +
			pPatchSelectionRi * pdf_i / (costhetax*m_pScene->scenePatches[i].area);

		pPatchSelectionRj=m_pPowerTree->getValue2(j, m_nIteration)/m_pPowerTree->getRootValue(m_nIteration);
		pPatchSelectionRi=m_pPowerTree->getValue2(i, m_nIteration)/m_pPowerTree->getRootValue(m_nIteration);
		pdf_i = RadianceDirectionProbability(&m_pScene->scenePatches[i], _w);
		pdf_j = RadianceDirectionProbability(&m_pScene->scenePatches[j], w);
		p1 = pPatchSelectionRj * pdf_j / (costhetay*m_pScene->scenePatches[j].area) +
			pPatchSelectionRi * pdf_i / (costhetax*m_pScene->scenePatches[i].area);
	} else {
		// Strategy #3
		j = m_pPowerTree->FindDistrib(RAND, m_nIteration);
		//select a point on the source patch
		y=m_pScene->scenePatches[j].PointInTriangle();

		i = m_pImportanceTree->FindDistrib(RAND, m_nIteration);
		//select a point on the receiver patch
		x = m_pScene->scenePatches[i].PointInTriangle();

		w = x - y;		//w always points y->x
		float yxNorm2 = w.Norm2();
		w /= sqrtf(yxNorm2);

		//calculate DotProducts
		costhetax = -( w * m_pScene->scenePatches[i].normal );
		costhetay =  ( w * m_pScene->scenePatches[j].normal );

		// are the patches facing towards each other?
		if (costhetay< 0 || costhetax< 0 ) return false;
				
		ray.id = ++ray.maxRayID;
		ray.origin=y;
		ray.dir=w;		//in this case, ray.dir points y->x

		m_pScene->m_pKDTree->traverse(ray, hitRec, 0.0f, FLT_MAX);

		if (hitRec.object == NULL) return false; 
		if (i != hitRec.object->m_index) {
			//we found a different patch, path i and patch j are not visible
			return false;
		}
			
		G=costhetax*costhetay/yxNorm2;

		_w = -w;

		float pPatchSelectionRj=m_pPowerTree->getValue2(j, m_nIteration)/m_pPowerTree->getRootValue(m_nIteration);
		float pPatchSelectionRi=m_pPowerTree->getValue2(i, m_nIteration)/m_pPowerTree->getRootValue(m_nIteration);
		float pdf_i = RadianceDirectionProbability(&m_pScene->scenePatches[i], _w);
		float pdf_j = RadianceDirectionProbability(&m_pScene->scenePatches[j], w);
		p1 = pPatchSelectionRj * pdf_j / m_pScene->scenePatches[j].area +
			pPatchSelectionRi * pdf_i / m_pScene->scenePatches[i].area;

		pPatchSelectionRj=m_pImportanceTree->getValue2(j, m_nIteration)/m_pImportanceTree->getRootValue(m_nIteration);
		pPatchSelectionRi=m_pImportanceTree->getValue2(i, m_nIteration)/m_pImportanceTree->getRootValue(m_nIteration);
		pdf_j = ImportanceDirectionProbability(&m_pScene->scenePatches[j], w);
		pdf_i = ImportanceDirectionProbability(&m_pScene->scenePatches[i], _w);
		p2 = pPatchSelectionRj * pdf_j / m_pScene->scenePatches[j].area +
			pPatchSelectionRi * pdf_i / m_pScene->scenePatches[i].area;

	}

	if (!_finite(G) || G<EPSILON) return false;


	// Areas
	float Ai=m_pScene->scenePatches[i].area;
	float Aj=m_pScene->scenePatches[j].area;

	float fi=m_pPowerTree->getRootValue(m_nIteration);
	float lambda=m_pImportanceTree->getRootValue(m_nIteration);

	//specular albedo
	Color da_i=m_pScene->scenePatches[i].m_pMaterial->m_brdf->ComputedSpecularMaxPhongAlbedo(w,m_pScene->scenePatches[i].normal);
	float Lum_da_i = da_i.Average();

	//specular albedo
	Color da_j=m_pScene->scenePatches[j].m_pMaterial->m_brdf->ComputedSpecularMaxPhongAlbedo(_w,m_pScene->scenePatches[j].normal);
	float Lum_da_j = da_j.Average();

	//diffuse BRDF
	Color fd_i=m_pScene->scenePatches[i].m_pMaterial->m_brdf->GetDiffuseBrdf();
	Color fd_j=m_pScene->scenePatches[j].m_pMaterial->m_brdf->GetDiffuseBrdf();

	Color Le_i, Le_j;	// radiance emittance
	Color L_i=getRadiance(i, _w, Le_i); Color L_j=getRadiance(j, w, Le_j);
	float R_i=getImportance(i, _w); float R_j=getImportance(j, w);

	p1*=G;
	p2*=G;

	float p3=(m_pImportanceTree->getValue2(i, m_nIteration)*m_pPowerTree->getValue2(j, m_nIteration)+
		m_pImportanceTree->getValue2(j, m_nIteration)*m_pPowerTree->getValue2(i, m_nIteration))
		/(Ai*Aj*lambda*fi);

	//d = P1*p1+P2*p2+P3*p3
	float d = m_probPatchSelection_Power*p1+m_probPatchSelection_Importance*p2+m_probPatchSelection_Mix*p3;
	if (!_finite(d) || d<1e-10f) return false;

	Color	newI_i = L_j*G/(Ai*d);		Color	newI_j = L_i*G/(Aj*d);
	float	newW_i = R_j*G/(Ai*d);		float	newW_j = R_i*G/(Aj*d);


	Color f=m_pScene->scenePatches[i].m_pMaterial->m_brdf->EvalBrdf(w, m_pScene->scenePatches[i].m_VisDir, m_pScene->scenePatches[i].normal, costhetax);
	m_pScene->scenePatches[i].Leye += ((L_j-Le_j)*G/(Ai*d)) % f;

	f=m_pScene->scenePatches[j].m_pMaterial->m_brdf->EvalBrdf(_w, m_pScene->scenePatches[j].m_VisDir, m_pScene->scenePatches[j].normal, costhetay);
	m_pScene->scenePatches[j].Leye += ((L_i-Le_i)*G/(Aj*d)) % f;

	m_pScene->scenePatches[i].C_I += (newI_i % da_i).Average();	
	m_pScene->scenePatches[j].C_I += (newI_j % da_j).Average();

	m_pScene->scenePatches[i].C_W += newW_i * Lum_da_i;				
	m_pScene->scenePatches[j].C_W += newW_j * Lum_da_j;

	m_pScene->scenePatches[i].Ld  += newI_i % fd_i;			
	m_pScene->scenePatches[j].Ld  += newI_j % fd_j;

	m_pScene->scenePatches[i].Rd+= newW_i * fd_i.Average();	
	m_pScene->scenePatches[j].Rd+= newW_j * fd_j.Average();

	//Accept or reject new newI_i, newI_j, newW_i, newW_j values
	Accept(m_pScene->scenePatches[i].I, newI_i, m_pScene->scenePatches[i].dirI, w, da_i);
	Accept(m_pScene->scenePatches[j].I, newI_j, m_pScene->scenePatches[j].dirI, _w, da_j);

	Accept(m_pScene->scenePatches[i].W, newW_i, m_pScene->scenePatches[i].dirW, w);
	Accept(m_pScene->scenePatches[j].W, newW_j, m_pScene->scenePatches[j].dirW, _w);

	m_pPowerTree->Update(i);
	m_pPowerTree->Update(j);

	m_pImportanceTree->Update(i);
	m_pImportanceTree->Update(j);

	return true;
}

float StIter::CalculateSpecularDirectionProbability(Patch* p, float value, const Vector& indir, const Vector& outdir)
{
	if (value<EPSILON) return 0.0f;

	Vector idealReflDir;
	idealReflDir.SetIdealReflectedDirection (indir, p->normal);
	return CalculateSpecularDirectionProbability(p, value, indir, outdir, idealReflDir);
}

float StIter::CalculateSpecularDirectionProbability(Patch* p, float value, const Vector& indir, const Vector& outdir, const Vector& idealReflDir)
{
	if (value<EPSILON) return 0.0f;

	float specPdf;

	float	cos_ideal = idealReflDir * outdir;
	if (cos_ideal > 0)
		specPdf = powf (cos_ideal, p->m_pMaterial->m_brdf->Ns) * p->m_pMaterial->m_brdf->specularPdfScaling;
	else
		specPdf = 0.0f;

	return specPdf;
}

Vector StIter::GenerateSpecularDirection(Patch* p, float value, const Vector& indir, float& specPdf)
{
	Vector outdir;

	//specular direction selection strategy
	CPhongBrdf*	brdf = p->m_pMaterial->m_brdf;
	Vector idealReflDir;
	idealReflDir.SetIdealReflectedDirection (indir, p->normal);

	for (;;) {
		idealReflDir.SampleHemisphereCosNTheta(brdf->Ns, brdf->specularPdfScaling, outdir, specPdf);
		float cosx=outdir * p->normal;	//DotProduct
		if (cosx>0) break;
	}

	outdir.Normalize();
	specPdf = CalculateSpecularDirectionProbability(p, value, indir, outdir, idealReflDir);
	return outdir;
}

float StIter::RadianceDirectionProbability(Patch* p, const Vector& outdir)
{
	float sePower = m_nIteration*M_PI*(p->m_pMaterial->m_edf ? p->m_pMaterial->m_edf->GetEmissionLum() : 0.0f);
	float sdPower = M_PI*p->Ld.Average();

	Color Ks=p->m_pMaterial->m_brdf->ComputedSpecularMaxPhongAlbedo(outdir, p->normal);
	float ssPower = (Ks % p->I).Average()*p->C_I;
	float ss=sePower+sdPower+ssPower;

	if (ss<EPSILON) return 0.0f;

	float cosTheta = p->normal * outdir;
	float diffPdf = cosTheta / M_PI;
	float specPdf = CalculateSpecularDirectionProbability(p, p->C_I, p->dirI, outdir);
	float pdf = specPdf * ssPower/ss + diffPdf* (1.0f-ssPower/ss);
	
	return pdf;
}

float StIter::ImportanceDirectionProbability(Patch* p, const Vector& outdir)
{
	Color Ks=p->m_pMaterial->m_brdf->ComputedSpecularMaxPhongAlbedo(outdir, p->normal);

	float srdImportance,srsImportance;
	float sedImportance=0.0f;	// 'emissive' diffuse importance
	float sesImportance=0.0f;	// 'emissive' specular importance
	if (p->visible) {
		Color Kd=p->m_pMaterial->m_brdf->GetDiffuseAlbedo();

		sedImportance = M_PI*Kd.Average()*m_nIteration;
		sesImportance = Ks.Average()*m_nIteration;
	};
	srdImportance = M_PI*p->Rd;
	srsImportance = p->C_W*(Ks * p->W).Average();

	float diffPdf, specPdf1, specPdf2;

	float sum_sImportance=sedImportance+sesImportance+srdImportance+srsImportance;
	if (sum_sImportance<EPSILON) return 0.0f;

	float pDiffuse=(sedImportance+srdImportance)/sum_sImportance;
	float p1=srsImportance/sum_sImportance;
	float p2=sesImportance/sum_sImportance;

	specPdf1 = CalculateSpecularDirectionProbability(p, p->C_W, p->dirW, outdir);
	specPdf2 = CalculateSpecularDirectionProbability(p, 1.0f, p->m_EyePatchDir, outdir);
	float cosTheta = p->normal * outdir;
	diffPdf = cosTheta / M_PI;

	float pdf= pDiffuse * diffPdf + p1 * specPdf1 + p2* specPdf2;

	return pdf;
}

Vector StIter::GenerateRadianceDirection(long patchIndex, float& pdf)
{
	Patch* p=&m_pScene->scenePatches[patchIndex];
	float sePower = m_nIteration*M_PI*(p->m_pMaterial->m_edf ? p->m_pMaterial->m_edf->GetEmissionLum() : 0.0f);
	float sdPower = M_PI*p->Ld.Average();

	Color Ks=p->m_pMaterial->m_brdf->ComputedSpecularMaxPhongAlbedo(p->dirI, p->normal);
	float ssPower = (Ks % p->I).Average()*p->C_I;
	float ss=sePower+sdPower+ssPower;

	Vector outdir;
	float specPdf;
	float diffPdf;

	if (RAND<ssPower/ss) {
		//specular direction selection strategy
		outdir=GenerateSpecularDirection(p, p->C_I, p->dirI, specPdf);
		float cosTheta = p->normal * outdir;
		diffPdf = cosTheta / M_PI;
	} else 
	{
		//diffuse direction selection strategy
		p->normal.SampleHemisphereCosTheta(outdir, diffPdf);
		outdir.Normalize();

		specPdf = CalculateSpecularDirectionProbability(p, p->C_I, p->dirI, outdir);
	}

	pdf = specPdf * ssPower/ss + diffPdf* (1.0f-(ssPower)/ss);
	return outdir;
}

Vector StIter::GenerateImportanceDirection(long patchIndex, float& pdf)
{
	Patch* p=&m_pScene->scenePatches[patchIndex];

	float srdImportance,srsImportance;
	float sedImportance=0.0f;	// 'emissive' diffuse importance
	float sesImportance=0.0f;	// 'emissive' specular importance
	if (p->visible) {
		Color Kd=p->m_pMaterial->m_brdf->GetDiffuseAlbedo();
		Color Ks=p->m_pMaterial->m_brdf->ComputedSpecularMaxPhongAlbedo(p->m_EyePatchDir, p->normal);

		sedImportance = M_PI*Kd.Average()*m_nIteration;
		sesImportance = Ks.Average()*m_nIteration;
	};

	Color Ks=p->m_pMaterial->m_brdf->ComputedSpecularMaxPhongAlbedo(p->dirW, p->normal);
	srdImportance = M_PI*p->Rd;
	srsImportance = p->C_W*(Ks * p->W).Average();

	Vector outdir;

	float diffPdf, specPdf1, specPdf2;
	float sum_sImportance = sedImportance+srdImportance+srsImportance+sesImportance;

	float p1=(sedImportance+srdImportance)/sum_sImportance;
	float p2=srsImportance/sum_sImportance;
	float p3=sesImportance/sum_sImportance;

	float r=RAND;
	if (r< p1) {
		//diffuse direction selection strategy
		p->normal.SampleHemisphereCosTheta (outdir, diffPdf);
		outdir.Normalize();

		specPdf1 = CalculateSpecularDirectionProbability(p, p->C_W, p->dirW, outdir);
		specPdf2 = CalculateSpecularDirectionProbability(p, 1.0f, p->m_EyePatchDir, outdir);
	}
	else if (r<(p1+p2)) {
		outdir = GenerateSpecularDirection(p, p->C_W, p->dirW, specPdf1);
		float cosTheta = p->normal * outdir;
		diffPdf = cosTheta / M_PI;
		specPdf2 = CalculateSpecularDirectionProbability(p, 1.0f, p->m_EyePatchDir, outdir);
	} else {
		outdir = GenerateSpecularDirection(p, 1.0f, p->m_EyePatchDir, specPdf2);
		float cosTheta = p->normal * outdir;
		diffPdf = cosTheta / M_PI;
		specPdf1 = CalculateSpecularDirectionProbability(p, p->C_W, p->dirW, outdir);
	}

	pdf = diffPdf * p1 + specPdf1 * p2 + specPdf2 * p3;
	return outdir;
}

Color StIter::getRadiance(long patchIndex, const Vector& outdir, Color& emission)
{
	Patch* p=&m_pScene->scenePatches[patchIndex];
	Color L=p->Ld/(float)m_nIteration;

	if (p->C_I>EPSILON) {
		CPhongBrdf* Brdf = p->m_pMaterial->m_brdf;
		
		Color f=Brdf->EvalSpecularBrdf (p->dirI, outdir, p->normal);
		Color Ks=Brdf->ComputedSpecularMaxPhongAlbedo(p->dirI, p->normal);

		float q=(p->I % Ks).Average()*m_nIteration/p->C_I;
		if (q>EPSILON) {
			L+=(f % p->I) / q;
		}
	}

	// if it is a lightSource, add emittance power to reflected power
	if (p->m_pMaterial->m_edf != NULL) {	
		emission=p->m_pMaterial->m_edf->GetEmissionRad();
		L += emission;
	} else {
		emission=Color::BLACK;
	}

	return L;
}


float StIter::getImportance(long patchIndex, const Vector& outdir)
{
	float R=0.0f;

	Patch* p=&m_pScene->scenePatches[patchIndex];
	CPhongBrdf* Brdf = p->m_pMaterial->m_brdf;

	// if the patch is directly visible from the eye,
	// we consider it's incoming importance 1.0f
	if (p->visible) {
		Color f = Brdf->EvalBrdf (p->m_EyePatchDir, outdir, p->normal);
		R = f.Average();
	}

	R+=(p->Rd/m_nIteration);

	if (p->W>EPSILON) {
		Color Ks=Brdf->ComputedSpecularMaxPhongAlbedo(p->dirW, p->normal);
		float KsAvg=Ks.Average();

		if (KsAvg>EPSILON) {
			Color f=Brdf->EvalSpecularBrdf(p->dirW, outdir, p->normal);
			float _f = f.Average();
			R += p->W/(p->W*KsAvg)*p->C_W*_f/m_nIteration;
		}
	}

	return R;
}

void	StIter::Accept(Color& _old, Color& _new, Vector& vOld, Vector& vNew, Color& Ks ) 
{
	float o=(_old % Ks).Average();
	if (o<EPSILON || RAND < (_new % Ks).Average()/o) {
		_old = _new; 
		vOld = vNew;
	}
}

void	StIter::Accept(float& _old, float& _new, Vector& vOld, Vector& vNew) 
{
	float o=_old;
	if (o<EPSILON || RAND < _new/o) {
		_old = _new; 
		vOld = vNew;
	}
}

//--------------------------------------

void	StIter::FinalizeLtoEye (void)
{	
	float scale = 1.0f/m_nIteration;
	for (long i = 0; i < m_pScene->nScenePatches; i++) {
		Patch* p = &m_pScene->scenePatches[i];
		p->Leye *= scale;
		if (p->m_pMaterial->m_edf != NULL) {
			p->Leye+=p->m_pMaterial->m_edf->GetEmissionRad();
		}
	}	// for
}

void	StIter::CalculateVertexColors (void)
{
	long nSceneVertices = m_pScene->nSceneVertices;
	long* vertexColorsInc = new long[nSceneVertices];

	// compute vertex colors with Goaroud shading
	for (long i=0; i<nSceneVertices; i++) {
		m_vertexColors[i] = Color::BLACK;
		vertexColorsInc[i] = 0;
	}
	
	for (i=0; i<m_pScene->nScenePatches; i++) {
		Patch* p=&m_pScene->scenePatches[i];
		if (p->area>EPSILON) {
			long vertIndex = (long)(p->a - m_pScene->sceneVertices);
			m_vertexColors[vertIndex]+=p->Leye;
			vertexColorsInc[vertIndex]++;
			
			vertIndex = (long)(p->b - m_pScene->sceneVertices);
			m_vertexColors[vertIndex]+= p->Leye;
			vertexColorsInc[vertIndex]++;
			
			vertIndex = (long)(p->c - m_pScene->sceneVertices);
			m_vertexColors[vertIndex]+= p->Leye;
			vertexColorsInc[vertIndex]++;
		}
	}
	
	for (i=0; i<m_pScene->nSceneVertices; i++) {
		if (vertexColorsInc[i] >0) {
			m_vertexColors[i]*=1.0f/vertexColorsInc[i];
		}
	}

	delete[] vertexColorsInc;
}

void StIter::MergeIndirectOpenGLImage()
{
	const Camera& camera = m_pScene->m_camera;

	unsigned char* Image= new unsigned char[3 * (camera.hres + 1) * (camera.vres + 1)];

	BYTE* m_pBitmapBits;

	//! memory device context
	HDC	DC = ::CreateCompatibleDC(NULL);

	BITMAPINFO				bmi;
	memset (&bmi, 0, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth			= camera.hres;
	bmi.bmiHeader.biHeight			= camera.vres;
	bmi.bmiHeader.biPlanes			= 1;
	bmi.bmiHeader.biBitCount		= 24;
	bmi.bmiHeader.biCompression		= BI_RGB;
	bmi.bmiHeader.biSizeImage		= camera.hres * camera.vres * 3;

	//! the DIB format bitmap
	HBITMAP	hDib = ::CreateDIBSection (DC, &bmi, DIB_RGB_COLORS, (void**)&m_pBitmapBits, NULL, (DWORD)0);
	SelectObject (DC, hDib);
	
	bool res=SetWindowPixelFormat (DC, PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL | PFD_STEREO_DONTCARE, 24);
	//! memory rendering context
	HGLRC RC = wglCreateContext (DC);

	if (RC==NULL) 
		return;

	if (wglMakeCurrent (DC, RC)==FALSE) return;

	// Default mode
	glEnable (GL_NORMALIZE);
	
	// Lights, material properties
	glClearColor (0, 0, 0, 0);
	glClearDepth (1.0);
	
	glEnable (GL_CULL_FACE);
	glPolygonMode (GL_FRONT, GL_FILL);		


    glEnable (GL_DEPTH_TEST);	// z-buffer
    glDisable (GL_DITHER);		// no dither
	
	glDisable (GL_LIGHTING);
	glDrawBuffer (GL_FRONT);

	glShadeModel (GL_SMOOTH);		// Gouraud shading


	RBOpenGLRenderScene ();


	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels (0, 0, camera.hres, camera.vres, GL_RGB, GL_UNSIGNED_BYTE, &Image[0]);

	// make the rendering context not current
	wglMakeCurrent (NULL, NULL);

	// delete the rendering context 
	wglDeleteContext (RC);
	DeleteDC (DC);
	DeleteObject (hDib);


	Color L;
	float scale = 1.0f/255.0f;
	for (int y=0; y<camera.vres; y++) {
		for (int x=0; x<camera.hres; x++) {
			// The OpenGL image is upside-down
			long id=3*((camera.vres-y-1)*camera.hres+x);
			L.r=Image[id]*scale;
			L.g=Image[id+1]*scale;
			L.b=Image[id+2]*scale;
			m_screenBuffer.AccumulateRadiance(x,y,L);
		}
	}
}

void	StIter::RBOpenGLRenderScene ()
{
	const Camera& camera = m_pScene->m_camera;

	GLsizei width = camera.hres;
	GLsizei height = camera.vres;
	
	GLfloat aspect = (GLfloat) width / (GLfloat) height;
	
	glViewport (0, 0, width, height);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective (camera.vfov * 2.0, aspect, camera.nearClip, camera.farClip);
	
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gluLookAt (camera.eyep.x, camera.eyep.y ,camera.eyep.z,
		camera.lookp.x, camera.lookp.y, camera.lookp.z,
		camera.updir.x, camera.updir.y, camera.updir.z);
	
	// set up scene
	glPushMatrix ();
	
	glBegin (GL_TRIANGLES);
	for (long i=0; i<m_pScene->nScenePatches; i++) {
		
		const Vector* vec = m_pScene->scenePatches[i].a;
		Color* pVertexColor = m_vertexColors + (vec - m_pScene->sceneVertices);
		glColor3d (pVertexColor->r, pVertexColor->g, pVertexColor->b);
		glVertex3d (vec->x, vec->y, vec->z); 

		vec = m_pScene->scenePatches[i].b;
		pVertexColor = m_vertexColors + (vec - m_pScene->sceneVertices);
		glColor3d (pVertexColor->r, pVertexColor->g, pVertexColor->b );
		glVertex3d (vec->x, vec->y, vec->z); 

		vec = m_pScene->scenePatches[i].c;
		pVertexColor = m_vertexColors + (vec - m_pScene->sceneVertices);
		glColor3d (pVertexColor->r, pVertexColor->g, pVertexColor->b );
		glVertex3d (vec->x, vec->y, vec->z); 	
	}
	glEnd ();

	glPopMatrix ();
	::glFlush();
}
