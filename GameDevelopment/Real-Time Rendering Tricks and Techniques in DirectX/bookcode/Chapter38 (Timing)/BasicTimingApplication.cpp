/***************************************************************
* BasicTimingApplication.cpp                                   *
*                                                              *
* This file contains the implementation of the                 *
* CBasicTimingApplication class.	        				   *
* To compile correctly, this file must be linked with:         *
* kernel32.lib                                                 *
* user32.lib                                                   *
* d3dx8dt.lib                                                  *
* d3d8.lib                                                     *
*                                                              *
***************************************************************/

#include "BasicTimingApplication.h"

//For sprintf
#include "stdio.h"

CBasicTimingApplication::CBasicTimingApplication()
{
}

CBasicTimingApplication::~CBasicTimingApplication()
{
}

BOOL CBasicTimingApplication::PostInitialize()
{	
	if (FAILED(EasyCreateWindowed(m_hWnd, D3DDEVTYPE_HAL, 
					  D3DCREATE_HARDWARE_VERTEXPROCESSING)))
		return FALSE;

	//Create a font
	m_Font.CreateFont(m_pD3DDevice, "Arial", 20);


	//Get the start time in ticks (milliseconds)
	m_LowResStartTime = GetTickCount();
	m_LowResCurrentTime = m_LowResStartTime;

	//Get the high res time. First get the frequency, then get the current
	//counter value. 
	QueryPerformanceFrequency((LARGE_INTEGER *)&m_HighResFrequency);
	QueryPerformanceCounter((LARGE_INTEGER *)&m_HighResStartTime);
	m_HighResCurrentTime = m_HighResStartTime;

	return TRUE;
}

void CBasicTimingApplication::Render()
{
	//This will give you the time it took to render the last frame, 
	//plus present that frame to the screen. This value may be 
	//constrained by the monitor's refresh rate. For example, if you
	//display the framerate now, you will get the real onscreen framerate.
	m_LowResLastFrameTime = (float)(GetTickCount() - m_LowResCurrentTime) / 1000.0f;
	m_LowResCurrentTime = GetTickCount();

	//Compute the framerate
	m_LowResFPS = 1.0f / (float)m_LowResLastFrameTime;

	//Display the full low resframerate
	sprintf(m_Output, "Low Res Frame Rate (everything): %3.2f fps", m_LowResFPS);
	m_Font.DrawText(0.0f, 0.0f, m_Output, 0xffffffff);

	//Now High Res... You have to divide by the frequency to convert
	//the counter values to seconds.
	QueryPerformanceCounter((LARGE_INTEGER *)&m_HighResTemp);
	m_HighResLastFrameTime = (double)(m_HighResTemp - m_HighResCurrentTime) /
		                     (double)m_HighResFrequency * 1000000.0;
	QueryPerformanceCounter((LARGE_INTEGER *)&m_HighResCurrentTime);

	//Compute the high res frame rate
	m_HighResFPS = 1000000.0 / m_HighResLastFrameTime;

	//Display the full high resframerate
	sprintf(m_Output, "High Res Frame Rate (everything): %3.3lf fps", m_HighResFPS);
	m_Font.DrawText(0.0f, 60.0f, m_Output, 0xffffffff);


	//Do some processing
	//The velocity for the test render is 320 + 320 * sin(t / 1000). This means
	//that the object will always be at a given place at a given time. The speed
	//is frame rate independent. 
	float XPosition = 320.0f + 320.0f * (float)sin(m_LowResCurrentTime / 1000.0f);
	m_Font.DrawDebug(XPosition, 150.0f);

	//Throw in some random slow downs
	//long Slowdown = (long)((float)rand() / (float)RAND_MAX * 50.0f);
	//Sleep(Slowdown);


	//We now reuse the last frame time variable to compute the time it
	//took to render this frame. If we displayed the framerate now,
	//we'd show the time it takes to execute the rendering calls.
	m_LowResLastFrameTime = (float)(GetTickCount() - m_LowResCurrentTime) / 1000.0f;

	//Compute the framerate
	m_LowResFPS = 1.0f / (float)m_LowResLastFrameTime;

	//Display the single frame framerate
	sprintf(m_Output, "Low Res Frame Rate (single frame): %3.2f fps", m_LowResFPS);
	m_Font.DrawText(0.0f, 30.0f, m_Output, 0xffffffff);

	//Reuse the high res variables for the single frame time
	QueryPerformanceCounter((LARGE_INTEGER *)&m_HighResTemp);
	m_HighResLastFrameTime = (double)(m_HighResTemp - m_HighResCurrentTime) /
		                     (double)m_HighResFrequency * 1000000.0;

	//Compute the high res frame rate
	m_HighResFPS = 1000000.0 / m_HighResLastFrameTime;

	//Display the full high resframerate
	sprintf(m_Output, "High Res Frame Rate (single frame): %3.3lf fps", m_HighResFPS);
	m_Font.DrawText(0.0f, 90.0f, m_Output, 0xffffffff);

}

BOOL CBasicTimingApplication::PreReset()
{
	m_Font.DestroyFont();
	return TRUE;
}

BOOL CBasicTimingApplication::PostReset()
{
	m_Font.CreateFont(m_pD3DDevice, "Arial", 40);
	return TRUE;
}

HRESULT CBasicTimingApplication::PostRender()
{
	//Simulate some sort of slowdown if you want to 
	//see the difference between frame processing times and
	//"everything" times.
	//Sleep(5);

	return CHostApplication::PostRender();
}

BOOL CBasicTimingApplication::PreTerminate()
{
	m_Font.DestroyFont();

	return TRUE;
}
