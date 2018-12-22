/*============================================================================
	Title: MapGenerator.cpp
	Author: Oscar Blasco
	Description: Normal Map Generator For Max4
	Changes:

	Note: The plugin will output a tga image to C:\normal.tga That is not
		  very elegant but it is better than having to choose the output
		  filepath each time the plugin is called.

		  Also the rasterizer code is not very robust, but does the work 
		  correctly.
============================================================================*/

#include "NormalMapGen.h"

#if _VC_
#pragma pack (push, 1)
#endif

/** TGA file header */
typedef struct {	
	unsigned char  id_length, colormap_type, image_type;
	unsigned short colormap_index;
	unsigned char  colormap_size;
	unsigned short x_origin, y_origin, width, height;
	unsigned char  pixel_size, attributes;
} TgaHeader;

#if _VC_
#pragma pack (pop, 1)
#endif


/** If true, the progress will be canceled in the next face loop */
bool CancelProgress = false;


/** 
 * Texture (or tangent) space basis class 
 * This class is used to orthonormalize the basis per texel.
 * Anyways it's disable, if you want to enable it, uncomment
 * the lines in the RasterizeScanlineAndTraceRays method.
 * It's disable because in tangent space the result is better
 * without orthogonality
 */
class Basis{
public:
	Basis(){ F = G = N = Point(); }

	void Orthogonalize(){
		// Gram-Smitch
		// F' = F - (N dot F) N
		// G' = G - (N dot G) N - (F' dot G) F'
		Point u1, u2, u3, tmp;

		u1 = N;
		u1.normalize();

		u2 = F - u1 * (F * u1);
		u2.normalize();

		u3 = G - u1 * (G * u1) - u2 * (G * u2);
		u3.normalize();

		N = u1;
		F = u2;
		G = u3;
	}

	Point F, G, N;
};


/** Computes the basis for the given face */
void ComputeBasis( const Point &v0, const Point &v1, const Point &v2, const Point &tv0, const Point &tv1, const Point &tv2, 
				   const Point &normal, Basis &basis) {

	Point ds, dt;
	Point e0, e1;
	Point cp;

	// Calculate ds, dt (partial derivates of s and t with respecto to x, y, z)

	// X Component
	e0.x = v1.x - v0.x;
	e0.y = (tv1.x*1024 - tv0.x*1024);
	e0.z = (tv1.y*1024 - tv0.y*1024);
	e1.x = v2.x - v0.x;
	e1.y = (tv2.x*1024 - tv0.x*1024);
	e1.z = (tv2.y*1024 - tv0.y*1024);

	//cp.Cross(e0, e1);

	cp = e0 ^ e1;

	if( fabs(cp.x) > EPS ) {
		ds.x = -cp.y / cp.x;
		dt.x = -cp.z / cp.x;
	}

	// Y Component
	e0.x = v1.y - v0.y;
	e1.x = v2.y - v0.y;

	//cp.Cross(e0, e1);
	cp = e0 ^ e1;

	if( fabs(cp.x) > EPS ) {
		ds.y = -cp.y / cp.x;
		dt.y = -cp.z / cp.x;
	}

	// Z Component
	e0.x = v1.z - v0.z;
	e1.x = v2.z - v0.z;

	//cp.Cross(e0, e1);
	cp = e0 ^ e1;

	if( fabs(cp.x) > EPS ) {
		ds.z = -cp.y / cp.x;
		dt.z = -cp.z / cp.x;
	}

	// Normalize the derivates
	ds.normalize();
	dt.normalize();

	// Calculate the normal
	Point bi;
	bi = ds ^ dt;

	float dot = bi * normal;
	if(dot < 0.0f){
		bi.x = -bi.x;
		bi.y = -bi.y;
		bi.z = -bi.z;
	}
	
	bi.normalize();					

	// write output:
	basis.F = ds;		// tangent
	basis.G = dt;		// binormal
	basis.N = bi;		// normal
}


/** 
 * Interpolates all values along the scanline and shoots a ray for each texel
 * to find min distance to the original mesh. Then calculates the normal of 
 * the original mesh at the intersection point
 */
void TiNormalMapGen::RasterizeScanlineAndTraceRays(int PosY){
	int ustart, uend;
	sFace *face = scanlineStart[PosY].face;
	bool flip = false;
	float denom = 1;
	Point dn, dt, db, dp, dtn, n, t, b, p, tn;

	ustart = scanlineStart[PosY].x; // the start position of the scanline
	uend = scanlineEnd[PosY].x;		// the end point

	// This catches the cases when we should draw for right to left
	if(ustart > uend){		
		if(ustart > 0){
			flip = true;
			int tmp = ustart;
			ustart = uend;
			uend = tmp;
		} else return;
	}

	denom = (uend - ustart);
	denom = denom? 1.f/denom : 0;
	
	// Calculate increments for each texel
	if(denom){
		dt = (scanlineEnd[PosY].t - scanlineStart[PosY].t)*denom;
		db = (scanlineEnd[PosY].b - scanlineStart[PosY].b)*denom;
		dn = (scanlineEnd[PosY].n - scanlineStart[PosY].n)*denom;
		dp = (scanlineEnd[PosY].p - scanlineStart[PosY].p)*denom;
		dtn = (scanlineEnd[PosY].tn - scanlineStart[PosY].tn)*denom;
	}else{
		dp = dn = dt = db = dtn = Point();
	}

	// Initialize values
	t = scanlineStart[PosY].t;
	b = scanlineStart[PosY].b;
	n = scanlineStart[PosY].n;
	p = scanlineStart[PosY].p;
	tn = scanlineStart[PosY].tn;
	
	// offset inside the map of the current scanline
	int scan_offset = PosY*SizeX*3; 

	// For each texel in this scanline
	for(int i = ustart; i < uend; i++){
		Point T = t;
		Point B = b;
		Point N = n;
		Point TN = tn;
		T.normalize();
		B.normalize();
		N.normalize();
		TN.normalize();

		Point fn = Point(), output = Point();
		sFace *f = HighFaces.Get();

		double distance = 9999999.f;

		nRaysDone++;

		// Trace a ray and check against all faces in the CachedFaces array, this
		// would improve speed but can cause missing rays
		{
			sFace *f = HighFaces.Get();
			for(int j = 0; j < face->CachedFaces.Size(); j++){

				f = HighFaces.Get(*face->CachedFaces.Get(j));
		
				double dist;
				Point ipoint;

				dist = Point::RayTriIntersectionDistance(p, N, f->normal, f->v[0], f->v[1], f->v[2], f->Max, f->Min, &ipoint);

				if(fabs(dist) < fabs(distance)){
					// Let's compute the barycentric coordinates of the point...
					float areaA, areaB, u, v, w;

					areaA = ((f->v[2] - f->v[1]) ^ (ipoint - f->v[1])).length();
					areaB = ((f->v[1] - f->v[0]) ^ (f->v[2] - f->v[0])).length();

					u = areaA/areaB;

					areaA = ((f->v[0] - f->v[2]) ^ (ipoint - f->v[2])).length();

					v = areaA/areaB;
					w = 1.0 - u - v;

					distance = dist;

					// Finally, interpolate the high mesh normal at the intersection point using the 
					// barycentric coordinates
					fn = f->n[0]*u + f->n[1]*v + f->n[2]*w;		
				}			
				f++;
			}

		}

		// Try to catch the ray if it didn't hit any of the catched faces, this time
		// try with the entire model
		if(distance == 9999999.f){
			nRaysMissed++;
			sFace *f = HighFaces.Get();
			for(int j = 0; j < this->HighMesh->getNumFaces(); j++){
		
				double dist;
				Point ipoint;

				dist = Point::RayTriIntersectionDistance(p, N, f->normal, f->v[0], f->v[1], f->v[2], f->Max, f->Min, &ipoint);

				if(fabs(dist) < fabs(distance)){
					// Let's compute the barycentric coordinates of the point...
					float areaA, areaB, u, v, w;

					areaA = ((f->v[2] - f->v[1]) ^ (ipoint - f->v[1])).length();
					areaB = ((f->v[1] - f->v[0]) ^ (f->v[2] - f->v[0])).inverselength();

					u = areaA*areaB;

					areaA = ((f->v[0] - f->v[2]) ^ (ipoint - f->v[2])).length();

					v = areaA*areaB;
					w = 1.0 - u - v;

					distance = dist;

					// Finally, interpolate the high mesh normal at the intersection point using the 
					// barycentric coordinates
					fn = f->n[0]*u + f->n[1]*v + f->n[2]*w;
				}			
				f++; 
			}
		} 

		// And if still the ray was missed, try using the closest-point scheme
		if(distance == 9999999.f){
			nRaysMissed++;
			sFace *f = HighFaces.Get();
			for(int j = 0; j < this->HighMesh->getNumFaces(); j++){
		
				double dist;
				Point ipoint;

				// Shoot the ray along the face's normal, this is the closest-point scheme
				dist = Point::RayTriIntersectionDistance(p, face->normal, f->normal, f->v[0], f->v[1], f->v[2], f->Max, f->Min, &ipoint);

				if(fabs(dist) < fabs(distance)){
					// Let's compute the barycentric coordinates of the point...
					float areaA, areaB, u, v, w;

					areaA = ((f->v[2] - f->v[1]) ^ (ipoint - f->v[1])).length();
					areaB = ((f->v[1] - f->v[0]) ^ (f->v[2] - f->v[0])).inverselength();

					u = areaA*areaB;

					areaA = ((f->v[0] - f->v[2]) ^ (ipoint - f->v[2])).length();

					v = areaA*areaB;
					w = 1.0 - u - v;

					distance = dist;

					// Finally, interpolate the high mesh normal at the intersection point using the 
					// barycentric coordinates
					fn = f->n[0]*u + f->n[1]*v + f->n[2]*w;
					//fn = f->normal;
				}			
				f++; 
			}
		}
		

		// The nexts lines are for the orthonormalization
		Basis basis;

		basis.F = T;
		basis.G = B;
		if(NonContinousNormals)
			basis.N = TN;
		else basis.N = N;
		
		//basis.Orthogonalize(); // Enable this for orthogonalization

		T = basis.F;
		B = basis.G;
		N = basis.N;

		// Normalize
		fn.normalize();

		if(InTextureSpace){
			// Transform to texture space using the space's basis
			output.x = fn * T;
			output.y = fn * B;
			output.z = fn * N;
		} else output = fn; // Copy it directly

		// This should be be unnecessary if the basis where orotgonal, but
		// that's is not true in our case
		output.normalize();

		// calculate the offset inside the map
		// when flip is true, we are drawing from right to left
		int offset, disp_offset;
		if(flip){
			offset = scan_offset + uend*3 - (i - ustart)*3;
			disp_offset = PosY*SizeX + uend - (i - ustart);
		} else{
			offset = scan_offset + i*3;
			disp_offset = PosY*SizeX + i;
		}

		
		// store the capture normal coordinates into the normal map
		map[offset+0] = (unsigned char) 127.5f + (output.x)*127.5f;
		map[offset+1] = (unsigned char) 127.5f + (output.y)*127.5f;
		map[offset+2] = (unsigned char) 127.5f + (output.z)*127.5f;
		

		// Increase values
		t += dt;
		b += db;
		n += dn;
		p += dp;
		tn += dtn;
	}
}


/** 
 * Fills some info into the scanlineStart or scanlineEnd arrays
 * Such info is the scanlines that are going to be drawn for the 
 * current face. 
 */
void TiNormalMapGen::ComputeScanlines(sFace &Face, int start, int end, int type)
{
	Point dp, dn, dt, db, dtn, p, n, t, b, tn; // Increments and vars
	float denom, du, u;
	ScanLineInfo *scanline = NULL;
	int xs, xe;

	// Face is inverted, we should draw it from top to down 
	if(Face.texelsV[end] < Face.texelsV[start]){
		int tmp = start;
		start = end;
		end = tmp;
	}

	// denominator
	denom = (Face.texelsV[end] - Face.texelsV[start]);

	// Compute increments
	if(denom){
		denom = 1.f/denom; // inverse denom
		du = (Face.texelsU[end] - Face.texelsU[start])*(denom); // U increment
	}
	else du = 0;

	u = Face.texelsU[start];

	// If denom != 0 compute increments
	if(denom){
		dp = (Face.v[end] - Face.v[start])*(denom);
		dt = (Face.t[end] - Face.t[start])*(denom);
		db = (Face.b[end] - Face.b[start])*(denom);
		dn = (Face.n[end] - Face.n[start])*(denom);
		dtn = (Face.tn[end] - Face.tn[start])*(denom);
	} else{
		dp = dn = dt = db = dtn = Point();
	}

	// Start point info
	p = Face.v[start];
	t = Face.t[start];
	b = Face.b[start];
	n = Face.n[start];
	tn = Face.tn[start];

	xe = Face.texelsV[end];
	xs = Face.texelsV[start];

	// The type tells us if the line is the end point or the start one
	if(type == LINE_START) 
		scanline = &scanlineStart[Face.texelsV[start]];
	else 
		scanline = &scanlineEnd[Face.texelsV[start]];

	for(int i = xs; i < xe; i++, scanline++){
		scanline->p = p;
		scanline->t = t;
		scanline->b = b;
		scanline->n = n;
		scanline->tn = tn;
		scanline->x = (int)u;
		scanline->face = &Face;

		p += dp;	
		t += dt;
		b += db;
		n += dn;
		tn += dtn;
		u += du;
	}
}


/** Rasterize the face to build the displacement map */
void TiNormalMapGen::RasterizeFace(sFace &Face)
{
	int top = 0, low = 0;
	int left = 0, right = 0;
	int progress = 1; 

	// Find the position of each vertex of the face in the texture.

	// Look for the top vertex
	{for(int i = 0; i < 3; i++){
		if(Face.texelsV[i] < Face.texelsV[top]) top = i;
	}}

	// Look for the low vertex
	{for(int i = 0; i < 3; i++){
		if(Face.texelsV[i] > Face.texelsV[low]) low = i;
	}}

	// Look for the left vertex
	{for(int i = 0; i < 3; i++){
		//if(i == top) continue;		
		if(Face.texelsU[i] < Face.texelsU[left] && i != top) left = i;
		else if(i != top && left == top) left = i;
	}}

	// Look for the right vertex
	{for(int i = 0; i < 3; i++){
		//if(i == top) continue;		
		if(Face.texelsU[i] > Face.texelsU[right] && i != top) right = i;
		else if(i != top && right == top) right = i;
	}}

	if(right == left){
		int other = (0 != top && 0 != left? 0 : 1 != top && 1 != left? 1 : 2 != top && 2 != left? 2 : 0);

		if(Face.texelsU[left] < Face.texelsU[top]){ // Left side
			if(Face.texelsV[left] <= Face.texelsV[other]){
				right = other;
			} else left = other;
		}else{ // Right side
			if(Face.texelsV[left] <= Face.texelsV[other]){
				left = other;
			} else right = other;
		}
	}

	// Next lines: to determine which scanlines we should draw for the current face being drawn
	// we 'paint' three lines by calling ComputeScanlines. That function Initialices the 
	// two arrays called scanlineStart and scanlineEnd. The first one is the start point
	// for each scanline while scanlineEnd has the info for the end point, so the pair
	// scanlineStart[a] and scanlineEnd[a] determine an scanline of the face.

	// Draw top to left vertices line
	ComputeScanlines(Face, top, left, LINE_START);
	// Draw top to right line
	ComputeScanlines(Face, top, right, LINE_END);

	// Now the last line, it could be and end line or an start line...
	if(Face.texelsU[left] == Face.texelsU[right]){
		if(Face.texelsU[left] < Face.texelsU[top])
			ComputeScanlines(Face, left, right, LINE_START);
		else ComputeScanlines(Face, right, left, LINE_END);
	} else{
		if(Face.texelsV[left] < Face.texelsV[right])
			ComputeScanlines(Face, left, right, LINE_START);
		else //if(Face.texelsV[left] > Face.texelsV[right])
			ComputeScanlines(Face, left, right, LINE_END);
	}


	SendMessage(hwnd_face_progress, PBM_SETRANGE, 0, MAKELPARAM(0, Face.texelsV[low] - Face.texelsV[top]));

	{for(int i = (int)Face.texelsV[top]; i < (int)Face.texelsV[low]; i++){
		RasterizeScanlineAndTraceRays(i);
		SendMessage(hwnd_face_progress, PBM_SETPOS, (WPARAM) progress++, 0);
		SendMessage(hwnd_progress, WM_PAINT, 0, 0);
	}}
}


/** Called by the plugin, computes the displacement map */
void TiNormalMapGen::ComputeDisplacementMap()
{
	int valid = 0;
	nmArray<Point> Normals, Tangents, Binormals, texNormals; // Arrays to compute the basis
	float boundsize = 0.1f;  // Bounding size to choose which faces put in the cache
	bool lastmissed = false; // This tells us if the last ray was missed when using cached faces 

	CancelProgress = false;  // If true the proccess is canceled in the next loop
	LowPriorityOn = false;   // Low priority thread

	FILE * fp; // The file pointer

	ICustButton *priorityBut = GetICustButton(GetDlgItem(hwnd_progress,IDC_LOWPRIORITYBUTTON));
	priorityBut->SetType(CBT_CHECK);

	if(!LowMesh->tVerts) {
		SetWindowText(GetDlgItem(hParams, IDC_PICKLOWBUTTON), "Needs Mapping!!");
		ShowWindow(hwnd_progress, 0);
		SendMessage(hwnd_progress, WM_DESTROY, NULL, NULL);
		hwnd_progress = NULL;
		HighMesh = NULL;
		LowMesh = NULL;
		return;
	}

	// Get map size 
	int size = ccEditMapSize->GetInt(&valid);

	if(size) 
		SizeX = SizeY = size;
	else SizeX = SizeY = 128;

	scanlineStart = new ScanLineInfo[SizeY];
	scanlineEnd = new ScanLineInfo[SizeY];

	HWND bar;

	// Make and show a progress dialog
	hwnd_info = GetDlgItem(hwnd_progress, IDC_INFO);
	SetDlgItemText(hwnd_progress, IDC_INFO, "Creating structures...");

	hwnd_face_progress = GetDlgItem(hwnd_progress, IDC_FACEPROGRESSBAR);

	bar = GetDlgItem(hwnd_progress, IDC_PROGRESSBAR);
	SendMessage(bar, PBM_SETRANGE, 0, MAKELPARAM(0, this->LowMesh->getNumFaces()));

	// This is the place to put the generated map
	map = new unsigned char[SizeX*SizeY*3];
	memset(map, 0, SizeX*SizeY*sizeof(unsigned char)*3);

	FaceSelection = LowMesh->FaceSel(); // Obtain selected faces (if any) from max

	Face *f = HighMesh->faces; 

	nmArray<Point> hNormals; // Detailed mesh normals 
	hNormals.Grow(HighMesh->numVerts);
	memset(hNormals.Get(), 0, sizeof(Point)*hNormals.nAllocated());
	
	// Calculate the normals for the detailed mesh
	{for(int i = 0; i < HighMesh->getNumFaces(); i++){
		sFace sf;

		sf.v[0].x = HighMesh->verts[f->v[0]].x;
		sf.v[0].y = HighMesh->verts[f->v[0]].y;
		sf.v[0].z = HighMesh->verts[f->v[0]].z;
		sf.v[1].x = HighMesh->verts[f->v[1]].x;
		sf.v[1].y = HighMesh->verts[f->v[1]].y;
		sf.v[1].z = HighMesh->verts[f->v[1]].z;
		sf.v[2].x = HighMesh->verts[f->v[2]].x;
		sf.v[2].y = HighMesh->verts[f->v[2]].y;
		sf.v[2].z = HighMesh->verts[f->v[2]].z;

		Point normal = (sf.v[1]-sf.v[0])^(sf.v[2]-sf.v[1]);
		normal.normalize();

		sf.normal = normal;

		Point *pn = hNormals.Get(f->v[0]);
		*pn += normal;
		pn = hNormals.Get(f->v[1]);
		*pn += normal;
		pn = hNormals.Get(f->v[2]);
		*pn += normal;

		f++;
	}}

	{for(int n = 0; n < hNormals.nAllocated(); n++){
		Point *pn = hNormals.Get(n);
		pn->normalize();
	}}

	f = HighMesh->faces;

	// Generate the array of faces of the detailed mesh

	HighFaces.Clear();
	if(HighFaces.nAllocated() < HighMesh->getNumFaces()){
		HighFaces.Free();
		HighFaces.Grow(HighMesh->getNumFaces());
	}
	if(!HighFaces.nAllocated()) HighFaces.Grow(HighMesh->getNumFaces());

	{for(int i = 0; i < HighMesh->getNumFaces(); i++){
		sFace sf;

		sf.v[0].x = HighMesh->verts[f->v[2]].x;
		sf.v[0].y = HighMesh->verts[f->v[2]].y;
		sf.v[0].z = HighMesh->verts[f->v[2]].z;
		sf.v[1].x = HighMesh->verts[f->v[1]].x;
		sf.v[1].y = HighMesh->verts[f->v[1]].y;
		sf.v[1].z = HighMesh->verts[f->v[1]].z;
		sf.v[2].x = HighMesh->verts[f->v[0]].x;
		sf.v[2].y = HighMesh->verts[f->v[0]].y;
		sf.v[2].z = HighMesh->verts[f->v[0]].z;

		Point normal = (sf.v[1]-sf.v[0])^(sf.v[2]-sf.v[1]);
		normal.normalize();

		sf.normal = normal;

		sf.n[2] = *hNormals.Get(f->v[0]);
		sf.n[1] = *hNormals.Get(f->v[1]);
		sf.n[0] = *hNormals.Get(f->v[2]);

		sf.Max = Point(-999999.f, -999999.f, -999999.f);
		sf.Min = Point(999999.f, 999999.f, 999999.f);

		{for(int i = 0; i < 3; i++){
			if(sf.v[i].x > sf.Max.x) sf.Max.x = sf.v[i].x;
			if(sf.v[i].x < sf.Min.x) sf.Min.x = sf.v[i].x;

			if(sf.v[i].y > sf.Max.y) sf.Max.y = sf.v[i].y;
			if(sf.v[i].y < sf.Min.y) sf.Min.y = sf.v[i].y;

			if(sf.v[i].z > sf.Max.z) sf.Max.z = sf.v[i].z;
			if(sf.v[i].z < sf.Min.z) sf.Min.z = sf.v[i].z;
		}}

		HighFaces << (&sf);
		f++;
	}}

	hNormals.Free(); // Free used memory

	// Now time for the low mesh...

	Normals.Grow(LowMesh->numTVerts);
	memset(Normals.Get(), 0, sizeof(Point)*Normals.nAllocated());
	texNormals.Grow(LowMesh->numTVerts);
	memset(texNormals.Get(), 0, sizeof(Point)*texNormals.nAllocated());
	Tangents.Grow(LowMesh->numTVerts);
	memset(Tangents.Get(), 0, sizeof(Point)*Tangents.nAllocated());
	Binormals.Grow(LowMesh->numTVerts);
	memset(Binormals.Get(), 0, sizeof(Point)*Binormals.nAllocated());

	f = LowMesh->faces; 
	UVVert *uvs = LowMesh->mapVerts(1);
	TVFace *tvFace = LowMesh->mapFaces(1);

	// Compute tangent space basis for each face 
	// (and average then for each vertex)
	{for(int n = 0; n < LowMesh->getNumFaces(); n++){
		sFace sf;
		Point t[3];
		Basis basis;

		sf.v[0].x = LowMesh->verts[f->v[0]].x;
		sf.v[0].y = LowMesh->verts[f->v[0]].y;
		sf.v[0].z = LowMesh->verts[f->v[0]].z;
		sf.v[1].x = LowMesh->verts[f->v[1]].x;
		sf.v[1].y = LowMesh->verts[f->v[1]].y;
		sf.v[1].z = LowMesh->verts[f->v[1]].z;
		sf.v[2].x = LowMesh->verts[f->v[2]].x;
		sf.v[2].y = LowMesh->verts[f->v[2]].y;
		sf.v[2].z = LowMesh->verts[f->v[2]].z;

		t[0].x = uvs[tvFace[n].getTVert(0)].x;
		t[1].x = uvs[tvFace[n].getTVert(1)].x;
		t[2].x = uvs[tvFace[n].getTVert(2)].x;
		t[0].y = 1 - uvs[tvFace[n].getTVert(0)].y;
		t[1].y = 1 - uvs[tvFace[n].getTVert(1)].y;
		t[2].y = 1 - uvs[tvFace[n].getTVert(2)].y;

		

		Point normal = (sf.v[1]-sf.v[0])^(sf.v[2]-sf.v[1]);
		normal.normalize();	

		ComputeBasis(sf.v[0], sf.v[1], sf.v[2], t[0], t[1], t[2], normal, basis);

		Point *pn = Normals.Get(f->v[0]);
		*pn += normal;
		pn = Normals.Get(f->v[1]);	
		*pn += normal;
		pn = Normals.Get(f->v[2]);	
		*pn += normal;

		pn = texNormals.Get(tvFace[n].t[0]);
		*pn += normal;
		pn = texNormals.Get(tvFace[n].t[1]);	
		*pn += normal;
		pn = texNormals.Get(tvFace[n].t[2]);	
		*pn += normal;

		pn = Tangents.Get(tvFace[n].t[0]);
		*pn += basis.F;
		pn = Tangents.Get(tvFace[n].t[1]);
		*pn += basis.F;
		pn = Tangents.Get(tvFace[n].t[2]);
		*pn += basis.F;

		pn = Binormals.Get(tvFace[n].t[0]);
		*pn += basis.G;
		pn = Binormals.Get(tvFace[n].t[1]);
		*pn += basis.G;
		pn = Binormals.Get(tvFace[n].t[2]);
		*pn += basis.G;

		f++;
	}}
	
	// Normalize basis

	{for(int n = 0; n < Normals.nAllocated(); n++){
		Normals.Get(n)->normalize();
	}}

	{for(int n = 0; n < Tangents.nAllocated(); n++){
		texNormals.Get(n)->normalize();
		Binormals.Get(n)->normalize();
		Tangents.Get(n)->normalize();
	}}
	
	
	f = LowMesh->faces; 
	uvs = LowMesh->mapVerts(1);
	tvFace = LowMesh->mapFaces(1);

	// Draw each face of the low mesh
	{for(int i = 0; i < LowMesh->getNumFaces(); i++){
		sFace sf;

		// Only use selected faces if the option is enabled
		if(OnSelectedFaces && !FaceSelection[i]){
			f++; tvFace++;
			continue;
		}

		nRaysDone = nRaysMissed = 0;

		// Copy data to the face structure
		sf.v[0].x = LowMesh->verts[f->v[0]].x;
		sf.v[0].y = LowMesh->verts[f->v[0]].y;
		sf.v[0].z = LowMesh->verts[f->v[0]].z;
		sf.v[1].x = LowMesh->verts[f->v[1]].x;
		sf.v[1].y = LowMesh->verts[f->v[1]].y;
		sf.v[1].z = LowMesh->verts[f->v[1]].z;
		sf.v[2].x = LowMesh->verts[f->v[2]].x;
		sf.v[2].y = LowMesh->verts[f->v[2]].y;
		sf.v[2].z = LowMesh->verts[f->v[2]].z;		

		sf.n[0] = *Normals.Get(f->v[0]);
		sf.n[1] = *Normals.Get(f->v[1]);
		sf.n[2] = *Normals.Get(f->v[2]);

		sf.tn[0] = *texNormals.Get(tvFace->t[0]);
		sf.tn[1] = *texNormals.Get(tvFace->t[1]);
		sf.tn[2] = *texNormals.Get(tvFace->t[2]);

		sf.t[0] = *Tangents.Get(tvFace->t[0]);
		sf.t[1] = *Tangents.Get(tvFace->t[1]);
		sf.t[2] = *Tangents.Get(tvFace->t[2]);

		sf.b[0] = *Binormals.Get(tvFace->t[0]);
		sf.b[1] = *Binormals.Get(tvFace->t[1]);
		sf.b[2] = *Binormals.Get(tvFace->t[2]);	

		sf.texelsU[0] = uvs[tvFace->getTVert(0)].x*(SizeX);
		sf.texelsU[1] = uvs[tvFace->getTVert(1)].x*(SizeX);
		sf.texelsU[2] = uvs[tvFace->getTVert(2)].x*(SizeX);
		sf.texelsV[0] = (1 - uvs[tvFace->getTVert(0)].y)*(SizeY);
		sf.texelsV[1] = (1 - uvs[tvFace->getTVert(1)].y)*(SizeY);
		sf.texelsV[2] = (1 - uvs[tvFace->getTVert(2)].y)*(SizeY);

		// Try to put in a cache the faces that are near to the current face
		Point Center = sf.v[0]*0.5 + sf.v[1]*0.5 + sf.v[2]*0.5;
		float len = (sf.v[0] - Center).length();

		sFace *face = HighFaces.Get();
		{for(int j = 0; j < HighMesh->getNumFaces(); j++){
			if(face->normal * sf.normal < 0.f){
				face++; continue;
			}
			
			Point fcenter = face->v[0]*0.5 + face->v[1]*0.5 + face->v[2]*0.5;

			if((fcenter - Center).length() > len*2.5f){ face++; continue; }
			if(fabs((fcenter - Center)*sf.normal) > len*boundsize){ face++; continue; }

			sf.CachedFaces << j; // Store the face in the cache
			
			face++;
			
		}}
	

		SendMessage(bar, PBM_SETPOS, (WPARAM) i, 0);
		SendMessage(hwnd_progress, WM_PAINT, 0, 0);

		char text[64];
		sprintf(text, "Tracing face %i of %i", i+1, this->LowMesh->getNumFaces());

		SetDlgItemText(hwnd_progress, IDC_INFO, text);

		if(CancelProgress) break;

		RasterizeFace(sf);

		// If we missed too many faces using the cache, increase the bounding size
		// of the cache for the next face, otherwise, try to minimize it.
		if((float(nRaysDone-nRaysMissed)/float(nRaysDone)) <= 0.85f){
			boundsize += 0.1f;
			lastmissed = true;
		} else if(!lastmissed) boundsize -= 0.1f;
		else lastmissed = false;

		f++; tvFace++; 
	}}

	// Save the generated normal map
	
	{	bool loop = false;
		
		unsigned char *tmpmap = new unsigned char[SizeX*SizeY*3];
		int count1 = 0, count2 = 0;
	
		// Apply an slow expansion edge filter...
		do{
			memset(tmpmap, 0, SizeX*SizeY*3*sizeof(unsigned char));
			count1 = count2 = 0;
			loop = false;
			{for(int i = 0; i < SizeY; i++){
				{for(int j = 0; j < SizeX; j++){
					if(!map[i*SizeX*3+j*3+0] && !map[i*SizeX*3+j*3+1] && !map[i*SizeX*3+j*3+2]){

						unsigned char p1[3], p2[3], p3[3], p4[3]; 
						unsigned char c = 0;

						count1++;						

						if(j < SizeX-1){
							p1[0] = map[(i)*SizeX*3+(j+1)*3+0];
							p1[1] = map[(i)*SizeX*3+(j+1)*3+1];
							p1[2] = map[(i)*SizeX*3+(j+1)*3+2];			
						} else p1[0] = p1[1] = p1[2] = 0;

						if(j > 0){
							p4[0] = map[(i)*SizeX*3+(j-1)*3+0];
							p4[1] = map[(i)*SizeX*3+(j-1)*3+1];
							p4[2] = map[(i)*SizeX*3+(j-1)*3+2];	
						} else p4[0] = p4[1] = p4[2] = 0;
						

						if(i+1 < SizeY){
							p2[0] = map[(i+1)*SizeX*3+(j)*3+0];
							p2[1] = map[(i+1)*SizeX*3+(j)*3+1];
							p2[2] = map[(i+1)*SizeX*3+(j)*3+2];					
						} else p2[0] = p2[1] = p2[2] = 0;

						if(i > 0){
							p3[0] = map[(i-1)*SizeX*3+(j)*3+0];
							p3[1] = map[(i-1)*SizeX*3+(j)*3+1];
							p3[2] = map[(i-1)*SizeX*3+(j)*3+2];		
						} else p3[0] = p3[1] = p3[2] = 0;

						if(p1[0] || p1[1] || p1[2]) c++;
						if(p2[0] || p2[1] || p2[2]) c++;
						if(p3[0] || p3[1] || p3[2]) c++;
						if(p4[0] || p4[1] || p4[2]) c++;

						if(c){
							tmpmap[(i)*SizeX*3+(j)*3+0] = (p1[0] + p2[0] + p3[0] + p4[0])/c;
							tmpmap[(i)*SizeX*3+(j)*3+1] = (p1[1] + p2[1] + p3[1] + p4[1])/c;
							tmpmap[(i)*SizeX*3+(j)*3+2] = (p1[2] + p2[2] + p3[2] + p4[2])/c;
						} else{ loop = true; count2++; }
					}
				}}
		
			}}
			
			{for(int i = 0; i < SizeY; i++){
				{for(int j = 0; j < SizeX; j++){
					if(tmpmap[i*SizeX*3+j*3+0] || tmpmap[i*SizeX*3+j*3+1] || tmpmap[i*SizeX*3+j*3+2]){
						map[i*SizeX*3+j*3+0] = tmpmap[i*SizeX*3+j*3+0];
						map[i*SizeX*3+j*3+1] = tmpmap[i*SizeX*3+j*3+1];
						map[i*SizeX*3+j*3+2] = tmpmap[i*SizeX*3+j*3+2];
					}
				}}
			}}
		}while(loop && count2 != count1);

		delete [] tmpmap;

		if((fp=fopen("c:/normal.tga","wb"))){
			TgaHeader hd;
			memset(&hd, 0, sizeof(hd));
			hd.image_type = 2;
			hd.height = SizeY;
			hd.width = SizeX;
			hd.pixel_size = 24;

			fwrite(&hd, 1, sizeof(hd), fp);

			unsigned char *tmp_map = new unsigned char[SizeX*SizeY*3];
	
			// Flip verticaly the image
			{for(int i = 0; i < SizeY; i++){
				for(int j = 0; j < SizeX; j++){
					tmp_map[i*SizeX*3+j*3  ] = map[(SizeY-i-1)*SizeX*3+j*3+2];
					tmp_map[i*SizeX*3+j*3+1] = map[(SizeY-i-1)*SizeX*3+j*3+1];
					tmp_map[i*SizeX*3+j*3+2] = map[(SizeY-i-1)*SizeX*3+j*3  ];
				}	
			}}

			fwrite(tmp_map, SizeX*SizeY*3, sizeof(unsigned char), fp);

			delete [] tmp_map;

			fclose(fp);
		}
	
	}

	EndDialog(hwnd_progress, 1);

	// Destroy the allocated memory
	delete [] scanlineStart;
	delete [] scanlineEnd;
	delete [] map;

	LowMesh = HighMesh = NULL;
	SetWindowText(GetDlgItem(hParams, IDC_PICKLOWBUTTON), "Low Object");
	SetWindowText(GetDlgItem(hParams, IDC_PICKHIGHBUTTON), "High Object");

	hwnd_progress = NULL;
	ReleaseICustButton(priorityBut);
}