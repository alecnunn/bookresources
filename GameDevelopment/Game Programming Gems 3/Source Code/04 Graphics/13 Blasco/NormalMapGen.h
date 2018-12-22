/*============================================================================
	Title: MaxInterface.h
	Author: Oscar Blasco
	Description: Normal Map Generator For Max4
	Changes:
============================================================================*/

#include "max.h"
#include "utilapi.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include "Resource.h"

#include "Maths.h"
#include "Array.h"


#define SIMPLIFIEROBJ_CLASS_ID 998877

/**
 * sFace class, contains all the necessary info
 * to rasterize the face.
 */
class sFace{
public:
	sFace(){}
	sFace(double v1x, double v1y, double v1z,
		  double v2x, double v2y, double v2z,
		  double v3x, double v3y, double v3z,
		  double n1x, double n1y, double n1z,
		  double n2x, double n2y, double n2z,
		  double n3x, double n3y, double n3z
		  ){

		v[0] = Point(v1x, v1y, v1z);
		v[1] = Point(v2x, v2y, v2z);
		v[2] = Point(v3x, v3y, v3z);
		n[0] = Point(n1x, n1y, n1z);
		n[1] = Point(n2x, n2y, n2z);
		n[2] = Point(n3x, n3y, n3z);
		
	}

	void SetTexCoords(int u1, int v1,
					  int u2, int v2,
					  int u3, int v3){
		texelsU[0] = u1; texelsU[1] = u2; texelsU[2] = u3;
		texelsV[0] = v1; texelsV[1] = v2; texelsV[2] = v3;
	}
		  
	Point v[3], normal;							/**< Face's vertices and normal		   */
	Point n[3], t[3], b[3], tn[3];				/**< Basis for each vertex             */
	int texelsU[3], texelsV[3];					/**< Texel coords for each vertex      */

	Point Max, Min;								/**< Face's bounding Box			   */			

	TiIDlist CachedFaces;						/**< Chached faces for fast raytracing */
};


/** Scanline Rasterization helping structure */
class ScanLineInfo{
public:
	Point p;
	Point n, t, b, tn;
	int x;
	sFace *face;								/**< Current face being rastetized      */								
};


class TiNormalMapGen : public UtilityObj
{
public:

	TiNormalMapGen(){ 
		HighMesh = LowMesh = NULL; hwnd_progress = NULL; 
		OnSelectedFaces = false;
	}

	CreateMouseCallBack* GetCreateMouseCallBack();
	void BeginEditParams(Interface *ip,IUtil *iu);
	void EndEditParams(Interface *ip,IUtil *iu);

	void OnInitDlg(HWND hWnd);
	void OnInitViewDlg(HWND hWnd);


	// Animatable methods		
	void DeleteThis() {/*delete this;*/}
	Class_ID ClassID() { return Class_ID(SIMPLIFIEROBJ_CLASS_ID, 32670); } 

	void ComputeDisplacementMap();

	static TriObject *GetTriObjectFromNode(INode *node, int &deleteIt) {
		deleteIt = FALSE;
		Object *obj = node->EvalWorldState(ip->GetTime()).obj;
		if (obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0))) { 
			TriObject *tri = (TriObject *) obj;//->ConvertToType(ip->GetTime(), 
			if (obj != tri) deleteIt = TRUE;
			return tri;
		}
		else {
			return NULL;
		}
	}

private:
	void RasterizeFace(sFace &Face);
	void RasterizeScanlineAndTraceRays(int posY);
	enum{ LINE_START, LINE_END };
	void ComputeScanlines(sFace &Face, int start, int end, int type);


	
public:
	HWND hParams;
	
	/** Max interface */
	static Interface *ip;
	static IParamMap *pmapCreate;
	

	/// Custom Controls ///

	ICustButton *ccButtonPickLow;
	ICustButton *ccButtonPickHigh;
	ICustButton *ccButtonOnSelectedFaces;
	ICustButton *ccButtonNonContinousNormals;
	ICustButton *ccButtonTextureSpace;

	ICustEdit *ccEditMapSize;

	HWND hwnd_progress;							/**< Progress Window handle    */
	HWND hwnd_face_progress;					/**< Progress bar handle       */
	HWND hwnd_info;								/**< Progress Info line handle */

	Mesh *HighMesh, *LowMesh;					/**< the two models...         */

	ScanLineInfo *scanlineStart, *scanlineEnd;  /**< Helping arrays for scanline drawing */

	BitArray FaceSelection;                     /**< Max selected faces        */

	unsigned char *map;

	int SizeX, SizeY;

	int nRaysDone, nRaysMissed;					/**< Statistics					*/

	bool NonContinousNormals, OnSelectedFaces;  /**< Some flags                 */
	bool LowPriorityOn, InTextureSpace;         /**< More flags...              */
 
	HANDLE hThread;								/**< Progress Thread            */

	nmArray<sFace> HighFaces, LowFaces;         /**< Face arrays                */
};