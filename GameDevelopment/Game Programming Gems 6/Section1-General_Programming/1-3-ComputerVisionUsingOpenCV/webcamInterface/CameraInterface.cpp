#include "stdafx.h"	

// CONSTRUCTOR/DESTRUCTOR
CameraInterface::CameraInterface(bool wOn, int nCam)
{
	numCam=nCam;
	image=NULL;
	HSVimage=NULL;
	backProj=NULL;
	hist=NULL;
	h_plane=NULL;
	v_plane=NULL;
	s_plane=NULL;

	capture=NULL;
	winOn=wOn;
	debug_flag=false;
}
CameraInterface::~CameraInterface()
{
	if(capture) cvReleaseCapture(&capture);
	if(image) cvReleaseImage(&image);
	if(HSVimage) cvReleaseImage(&HSVimage);
	if(backProj) cvReleaseImage(&backProj);
	if(hist) cvReleaseHist(&hist);
	if(h_plane) cvReleaseImage(&h_plane);
	if(v_plane) cvReleaseImage(&v_plane);
	if(s_plane) cvReleaseImage(&s_plane);

	cvDestroyAllWindows();	
}
//VISION
bool CameraInterface::calcHist()
{
	if(!capture)
		return false;

	for(;;) //first a "calibration" frame is required
	{
		IplImage* frame = NULL;
		frame = cvQueryFrame( capture ); //capture a frame
		if( !frame ) return false; //if no frame, error
		
		if(!image) //allocating variables if not still done (we must know the size and origin of the frames)
		{
			image = cvCreateImage( cvGetSize(frame), 8, 3 );
			image->origin = frame->origin;
		}
		if(!HSVimage)
		{
			HSVimage=cvCreateImage(cvGetSize(image),IPL_DEPTH_8U,3);
			HSVimage->origin = frame->origin;
			cvZero(HSVimage);
		}
		if(!backProj)
		{
			backProj=cvCreateImage(cvGetSize(image),IPL_DEPTH_8U,1);
			backProj->origin = frame->origin;
			cvZero(backProj);
		}
		if(!h_plane)
		{
			h_plane = cvCreateImage(cvGetSize(image),IPL_DEPTH_8U, 1 );
			h_plane->origin = frame->origin;
			cvZero(h_plane);
		}
		if(!s_plane)
		{
			s_plane = cvCreateImage(cvGetSize(image),IPL_DEPTH_8U, 1 );
			s_plane->origin = frame->origin;
			cvZero(s_plane);
		}
		if(!v_plane)
		{
			v_plane = cvCreateImage(cvGetSize(image),IPL_DEPTH_8U, 1 );
			v_plane->origin = frame->origin;
			cvZero(v_plane);
		}
		
		cvCopy( frame, image, 0 ); //the returned pointer (frame) shouldn't be modified
	
		cvEllipse(image,cvPoint(image->width/2,image->height/2),cvSize(image->width/6,image->height/3),0,0,360,CV_RGB(255,0,0),2); //a ellipse is drawn to select the user's face

		cvFlip(image,NULL,1); //flip image for easiest visualization
		cvShowImage( "Head Position", image ); //show the image in the window
		
		char c;
		c = cvWaitKey(10); //when the user presses the escape key stop the bucle and process the last captured frame
        if( c == 27 )
            break;
	}

    cvFlip(image,NULL,1); //flip the image back to normal
	IplImage* mask=cvCreateImage(cvGetSize(image),IPL_DEPTH_8U,1); //a mask is created to filter the selected pixels
	cvZero(mask);
	mask->origin = image->origin;
	//a ellipse (slightly smaller) is drawn
	cvEllipse(mask,cvPoint(image->width/2,image->height/2),cvSize(image->width/6-5,image->height/3-5),0,0,360,cvScalar(255),CV_FILLED);


	cvCvtColor( image, HSVimage, CV_RGB2HSV ); //color conversion to HSV
	cvCvtPixToPlane( HSVimage, h_plane, s_plane, v_plane, 0 ); //HSVimage is separated into planes
	IplImage* planes[] = { h_plane }; //the only needed plane is the hue

	int h_bins = 32; //number of bins in the histogram
	int hist_size[] = {h_bins};
	float h_ranges[] = { 0, 180 }; // the hue is an angle, varies from 0 to 180 (360/2 to fit in 8 bits)
    float* ranges[] = { h_ranges };

	if(hist!=0) cvReleaseHist( &hist ); //if already allocated, the histogram is released
	hist = cvCreateHist( 1, hist_size, CV_HIST_ARRAY, ranges, 1 ); //the histogram is created
	
	cvCalcHist( planes, hist, 0, mask ); //the histogram is filled with the pixels inside the ellipse

	cvReleaseImage(&mask); //the mask is deallocated

	return true;
}
int CameraInterface::queryState() //used to see the current position of the player's head
{
	clock_t start,end;

	start=clock();

	if(!capture)
	{
		if(!Start())
			return -2;
	}

	IplImage* frame = NULL;
	frame = cvQueryFrame( capture ); //capture a frame
	if( !frame ) //if no frame, error
	{
		fprintf(stderr,"ERROR: Frame lost.\n");
		return -2;
	}

	cvCopy( frame, image, 0 ); //the returned frame shouldn't be modified

	cvCvtColor( image, HSVimage, CV_RGB2HSV );  //color conversion to HSV

	cvCvtPixToPlane( HSVimage, h_plane, s_plane, v_plane, 0 ); //HSVimage is separated into planes

	IplImage* planes[]={h_plane };
	
	cvZero(backProj); //backProj image is set to 0
	
	//the histogram is used to assign a probability of being the player's skin to each pixel in the captured image
	cvCalcBackProject(planes , backProj, hist );

	CvScalar mean=cvAvg(backProj); //the mean of the backprojected image is calculated
	cvThreshold( backProj, backProj, floor(mean.val[0]), 255 , CV_THRESH_BINARY ); //only the pixels above the mean are truthworthy

	int vol[3]={0,0,0};

	cvSetImageROI(backProj,cvRect(0,image->height/4,image->width/3,image->height*3/4)); //the region of interest is set to the left side of the image
    vol[0]=cvCountNonZero(backProj); //the valid pixels are counted

    cvSetImageROI(backProj,cvRect(image->width/3,image->height/4,image->width/3,image->height*3/4)); //the region of interest is set to the center of the image
    vol[1]=cvCountNonZero(backProj); //the valid pixels are counted

    cvSetImageROI(backProj,cvRect(image->width*2/3,image->height/4,image->width/3,image->height*3/4)); //the region of interest is set to the right side of the image
    vol[2]=cvCountNonZero(backProj); //the valid pixels are counted
    cvResetImageROI(backProj); //the region of interest is reseted

	//the most probable region is returned (-1 left, 0 center, 1 right)
	if(vol[0]>vol[1] && vol[0]>vol[2])
		userPos=1;
	else if(vol[1]>vol[0] && vol[1]>vol[2])
		userPos=0;
	else
		userPos=-1;

	end=clock();

	//std::cout<<end-start<<std::endl; //Time ellapsed

	if(winOn) //if the window is active
	{
		//a bounding box for each region is drawn
		cvRectangle( image, cvPoint(0,image->height/4), cvPoint(image->width/3,image->height), CV_RGB(0,0,255));
		cvRectangle(image,cvPoint(image->width/3,image->height/4), cvPoint(image->width*2/3,image->height), CV_RGB(0,255,0));
		cvRectangle( image, cvPoint(image->width*2/3,image->height/4), cvPoint(image->width,image->height), CV_RGB(255,0,0));

		//a circle is drawn in the user's region
		switch(userPos)
		{
		case 1:
			cvCircle( image, cvPoint(50,190),40, CV_RGB(0,0,255),CV_FILLED);
		break;
		case 0:
			cvCircle( image, cvPoint(159,190),40, CV_RGB(0,255,0),CV_FILLED);
		break;
		case -1:
			cvCircle( image, cvPoint(270,190),40, CV_RGB(255,0,0),CV_FILLED);
		break;
		}

		//which image to show
		if(!debug_flag)
		{
			cvFlip(image,NULL,1); //flip image for easiest visualization
			cvShowImage( "Head Position", image);
		}
		else
		{
			cvFlip(backProj,NULL,1); //flip the image for easiest visualization
			cvShowImage( "Head Position", backProj);
		}
	}
	return userPos;
}

//PLAYBACK/GUI

int CameraInterface::Start()
{
	if( !(capture = cvCaptureFromCAM( numCam )) )
	{
		fprintf(stderr,"ERROR: Camera can't be initialized.\n");//Tractar amb excepcions?
        return -1;
    }
	if(winOn) cvNamedWindow( "Head Position", 1 );

	if(!calcHist())
		return -1;

	return 0;
}
int CameraInterface::Stop()
{
	if(capture)
	{
		cvReleaseCapture( &capture );
		if(winOn) cvDestroyWindow("Head Position");
		if(HSVimage) cvReleaseImage(&HSVimage);
		if(image) cvReleaseImage(&image);
		if(backProj) cvReleaseImage(&backProj);
		if(hist) cvReleaseHist(&hist);

		if(h_plane) cvReleaseImage(&h_plane);
		if(v_plane) cvReleaseImage(&v_plane);
		if(s_plane) cvReleaseImage(&s_plane);
		
		return 0;
	}
	else
		return -1;
}

void CameraInterface::setWin(int state) //0: no win / 1: win / Other: switch
{
	switch(state)
	{
	case 0:
		if(winOn)
		{
			winOn=false;
			cvDestroyWindow("Head Position");
		}
		break;
	case 1:
		if(!winOn)
		{
			winOn=true;
			cvNamedWindow("Head Position",1);
		}
		break;
	default:
		if(!winOn)
		{
			winOn=true;
			cvNamedWindow("Head Position",1);
		}
		else
		{
			winOn=false;
			cvDestroyWindow("Head Position");
		}
	}
}

void CameraInterface::setDebug(bool state)
{
	debug_flag=state;
}