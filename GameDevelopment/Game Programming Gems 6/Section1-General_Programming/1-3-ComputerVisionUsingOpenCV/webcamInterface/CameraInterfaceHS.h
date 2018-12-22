#ifndef ____CAMERA_INTERFACE_HS____
#define ____CAMERA_INTERFACE_HS____

#include "highgui.h"
#include "cv.h"
#include "cxcore.h"

class CameraInterfaceHS
{
	IplImage* image;
	IplImage* HSVimage;
	IplImage* backProj;

	IplImage* h_plane;
	IplImage* s_plane;
	IplImage* v_plane;

	CvHistogram *hist;
	CvCapture* capture;
	
	int numCam;
	bool winOn;
	bool debug_flag;
	int userPos;
	
	bool calcHist();
public:
	CameraInterfaceHS(bool wOn,int nCam=0);
	~CameraInterfaceHS();

	int Start();
	int Stop();
	void setWin(int state);
	void setDebug(bool state);

	int queryState(); 

};


#endif