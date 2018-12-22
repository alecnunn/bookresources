#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include <iostream>

#define ESC_KEY 27

CvCapture* capture = NULL;
IplImage* image,*frame,*mask,*HSVimage,*h_plane,*s_plane,*v_plane,*backProj;
CvHistogram* hist = NULL;
int userPos;
bool flag=true;

void paintEllipse();
void calcHistogram();
bool detectionLoop();
void showImage();

int main()
{
    int numCam=-1;
    if( !(capture = cvCaptureFromCAM( numCam )) )
    {
		std::cout<<"ERROR: Camera can't be initialized."<<std::endl;
        return -1; //error
    }

    //DECLARATIONS BLOCK
    cvNamedWindow("WEBCAM",1);

    //FIRST CAPTURE BLOCK (INITIALIZATION OF VARIABLES)
    frame = cvQueryFrame( capture );
    if( !frame ) return false;
    image = cvCreateImage( cvGetSize(frame), 8, 3 );
    image->origin = frame->origin;

    //NEVER-ENDING LOOP BLOCK
    for(;;)
    {
    
        frame = cvQueryFrame( capture );
        if( !frame ) return false;
    
        cvCopy( frame, image, 0 );

		paintEllipse(); // [REF01]

        cvShowImage( "WEBCAM", image );

        char c;
        c = cvWaitKey(10);
        if( c == ESC_KEY )
            break;

    }

	calcHistogram(); // [REF02]

	
	detectionLoop(); // [REF04]

    //CLEANUP BLOCK
    cvReleaseCapture(&capture);
    cvDestroyWindow("WEBCAM");
    cvReleaseImage(&image);
    cvReleaseHist(&hist);
    cvReleaseImage(&HSVimage);
    cvReleaseImage(&h_plane);
    cvReleaseImage(&v_plane);
    cvReleaseImage(&s_plane);
    cvReleaseImage(&backProj);

	return 0;
}



// [REF01]
void paintEllipse()
{
    cvEllipse(image,cvPoint(image->width/2,image->height/2),
    cvSize(image->width/6,image->height/3),0,0,
        360,CV_RGB(255,0,0),2);
    cvFlip(image,NULL,1); //flip the image for easiest visualization 
}

void calcHistogram()
{
	//HISTOGRAM CALCULATION BLOCK
    

	cvFlip(image,NULL,1); //flip the image back to normal
    mask=cvCreateImage(cvGetSize(image),IPL_DEPTH_8U,1);
    cvZero(mask);
    mask->origin = image->origin;
    cvEllipse(mask,cvPoint(image->width/2,image->height/2),
              cvSize((image->width/6)-5,(image->height/3)-5),0,0,
              360,cvScalar(255),CV_FILLED);

    HSVimage=cvCreateImage(cvGetSize(image),IPL_DEPTH_8U,3);
    HSVimage->origin=image->origin;

    cvCvtColor( image, HSVimage, CV_RGB2HSV );

    h_plane=cvCreateImage(cvGetSize(image),IPL_DEPTH_8U,1 );
    s_plane=cvCreateImage(cvGetSize(image),IPL_DEPTH_8U,1 );
    v_plane=cvCreateImage(cvGetSize(image),IPL_DEPTH_8U,1 );
    h_plane->origin = image->origin;
    s_plane->origin = image->origin;
    v_plane->origin = image->origin;

    cvCvtPixToPlane( HSVimage, h_plane, s_plane, v_plane, 0 );
    IplImage* planes[] = { h_plane };

    int h_bins = 32;
    int hist_size[] = {h_bins};
    float h_ranges[] = {0, 180};
    float* ranges[] = {h_ranges};

    hist=cvCreateHist(1,hist_size,CV_HIST_ARRAY,ranges,1);	
    cvCalcHist( planes, hist, 0, mask );
}



bool detectionLoop()
{
	//DETECTION LOOP BLOCK
    backProj=cvCreateImage(cvGetSize(image),IPL_DEPTH_8U,1);
    backProj->origin = frame->origin;
    
    char c;

    for(;;)
    {
    
        frame = cvQueryFrame( capture );
        if( !frame ) return false;
    
        cvCopy( frame, image, 0 );

        cvCvtColor( image, HSVimage, CV_RGB2HSV ); 

        cvCvtPixToPlane( HSVimage, h_plane, s_plane, v_plane, 0 );
        IplImage* planes[]={h_plane };
	
        cvZero(backProj);	
        cvCalcBackProject(planes , backProj, hist );

        CvScalar mean=cvAvg( backProj);
        cvThreshold(backProj,backProj,floor(mean.val[0]),
                    255, CV_THRESH_BINARY );

        int vol[3]={0,0,0};

        cvSetImageROI(backProj,cvRect(0,image->height/4,
                      image->width/3,image->height*3/4));
        vol[0]=cvCountNonZero(backProj);

        cvSetImageROI(backProj,cvRect(image->width/3,
                  image->height/4,image->width/3,image->height*3/4));
        vol[1]=cvCountNonZero(backProj);

        cvSetImageROI(backProj,cvRect(image->width*2/3,
                  image->height/4,image->width/3,image->height*3/4));
        vol[2]=cvCountNonZero(backProj);
        cvResetImageROI(backProj);


        if(vol[0]>vol[1] && vol[0]>vol[2]) userPos=-1;
        else if(vol[1]>vol[0] && vol[1]>vol[2]) userPos=0;
        else userPos=1;

        //VISUALIZATION BLOCK
    
		showImage(); // [REF06]
 
        c = cvWaitKey(10);
        if( c == 27 ) break;
        if( c == 'b') flag=flag?0:1;
    }
    // [REF04]
}


void showImage()
{
    cvRectangle( image, cvPoint(0,image->height/4),
            cvPoint(image->width/3,image->height), CV_RGB(0,0,255));
    cvRectangle(image,cvPoint(image->width/3,image->height/4),
            cvPoint(image->width*2/3,image->height), CV_RGB(0,255,0));
    cvRectangle( image, cvPoint(image->width*2/3,image->height/4),
             cvPoint(image->width,image->height), CV_RGB(255,0,0));

    switch(userPos)
    {
    case -1:	
        cvCircle( image, cvPoint(50,190),40, 
                CV_RGB(0,0,255),CV_FILLED);
    break;
    case 0:
        cvCircle( image, cvPoint(159,190),40, 
                CV_RGB(0,255,0),CV_FILLED);
    break;
    case 1:
        cvCircle( image, cvPoint(270,190),40, 
                CV_RGB(255,0,0),CV_FILLED);
    break;
    }

    if(flag)
    {
        cvFlip(image,NULL,1); //flip image for easiest visualization
        cvShowImage( "WEBCAM", image);
    }
    else
    {
        cvFlip(backProj,NULL,1); //flip the image 
        cvShowImage( "WEBCAM", backProj);
    }
}
