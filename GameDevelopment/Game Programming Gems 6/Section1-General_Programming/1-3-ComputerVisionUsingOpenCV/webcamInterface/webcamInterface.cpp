#include "stdafx.h"

#define ESC_KEY 27

int _tmain(int argc, _TCHAR* argv[])
{
	
//	CameraInterface *a=new CameraInterface(true);
	CameraInterfaceHS *a=new CameraInterfaceHS(true); //Uncomment this line and comment the previous to test the HS version


	bool flag_debug=false;
	bool flag_window=true;

	a->Start();

	a->setDebug(flag_debug);

	cvNamedWindow("MAIN",1); //a window to keep capturing keyboard events through cvWaitKey

	for(;;)	//Never-ending loop
	{
		std::cout<<a->queryState()<<std::endl; 

		char c;
		c = cvWaitKey(10); //Wait a key for 10ms
        if( c == ESC_KEY )
            break;
		if( c == 'b' )
		{
			flag_debug=flag_debug?0:1;
			a->setDebug(flag_debug);
		}
		if( c == 'w' )	//Enable / Disable debugging window
		{
			flag_window=flag_window?0:1;
			a->setWin(flag_window);
		}
	}

	a->Stop();
	
	delete a;
	cvDestroyWindow("MAIN");
	
	return 0;
}

