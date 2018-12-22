/***************************************************************
* BasicTimingApplication.h                                     *
*                                                              *
* This file contains the header information for the class      *
* derived from CHostApplication.  This new class will          *
* demonstrate the basics of drawing timing.  For this file to  *
* compile correctly, the path for the DX include files must be *
* added to your path.                                          *
*                                                              *
* Changes:                                                     *
*	*  None                                                    *
***************************************************************/
#include "Application.h"
#include "DirectXFont.h"

class CBasicTimingApplication : public CHostApplication  
{
public:
	CBasicTimingApplication();
	virtual ~CBasicTimingApplication();

	virtual BOOL PostInitialize();
	virtual BOOL PreTerminate();
	virtual BOOL PreReset();
	virtual BOOL PostReset();
	virtual HRESULT PostRender();
	virtual void Render();

	//These low res times are in milliseconds
	long    m_LowResStartTime;
	long    m_LowResCurrentTime;

	//These low res times are in seconds
	float   m_LowResLastFrameTime;
	float   m_LowResFPS;

	//This is the high res frequency to convert 
	//clocks to microseconds
	__int64 m_HighResFrequency;

	//These high res times are in counter values
	//when needed, use the frequency to convert
	//(saves precision)
	__int64 m_HighResStartTime;
	__int64 m_HighResCurrentTime;

	//Used to retrieve values since the functions
	//require pointers as input parameters
	__int64 m_HighResTemp;

	//These high res times are in seconds
	double m_HighResLastFrameTime;
	double m_HighResFPS;

	//The font used to display the timing info... and
	//the generic output buffer for the framerate.
	CDirectXFont m_Font;
	char m_Output[256];
};
