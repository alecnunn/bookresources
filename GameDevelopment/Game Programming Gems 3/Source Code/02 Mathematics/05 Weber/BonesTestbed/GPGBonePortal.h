
/******************************************************************************
IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.

By downloading, copying, installing or using the software you agree to this
license. If you do not agree to this license, do not download, install,
copy or use the software.

Intel Open Source License for "Improved Bones Deformation" and
"Constrained Inverse Kinematics" sample code

Copyright (c) 2002, Intel Corporation.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
	list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.
* Neither the name of Intel Corporation nor the names of its contributors may
	be used to endorse or promote products derived from this software without
	specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ''AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
EXPORT LAWS: THIS LICENSE ADDS NO RESTRICTIONS TO THE EXPORT LAWS OF YOUR
JURISDICTION. It is each licensee's responsibility to comply with any export
regulations applicable in each licensee's jurisdiction.
******************************************************************************/

#ifndef __GPGBonePortal_h__
#define __GPGBonePortal_h__


/*
#include "stdio.h"
#include "windows.h"
#include "winuser.h"
#include "mmsystem.h"
#include "gl/gl.h"
#include "gl/glu.h"
*/


#include"wds/wds.h"
#include"GPGBase.h"


#define GPG_COMMANDLINE_LENGTH	1024


//* \todo use PlatformAbstraction/GPGSystemTicker

//* no return value and we know it (asm sets the return value)
#if GPG_OSTYPE==GPG_WIN32
#include"mmsystem.h"
#pragma warning( push )
#pragma warning( disable : 4035 )
inline unsigned long ifxGetSystemTick(void) { __asm { rdtsc } };
#pragma warning( pop )
inline unsigned long ifxGetTickFrequency(void)
{
	unsigned long ms,tick,ticksPerMS=0;

	long pass=0;
	BOOL done=false;
	while(!done && pass<5)
	{
		Sleep(10);	// less likely to be interrupted if we start a new timestep

		ms=timeGetTime();
		tick=ifxGetSystemTick();

		Sleep(100);

		ms=timeGetTime()-ms;
		tick=ifxGetSystemTick()-tick;
		if(tick<1 || ms<1)
		{
			pass++;
			continue;
		}

		ticksPerMS=tick/ms;
		done=true;
	}
	return ticksPerMS;
}
#else
/*
inline unsigned long ifxGetSystemTick(void)
{
	asm("rdtsc");
}
*/
inline unsigned long ifxGetSystemTick(void)
{
	unsigned long low;
	unsigned long high;
	__asm__ __volatile__ ( "rdtsc" : "=a" (low) , "=d" (high) );
	return low;
};

inline unsigned long ifxGetTickFrequency(void)
{
	unsigned long ms,tick1=0,tick2=0,ticksPerMS=0;

	struct timeval tv1,tv2;
	struct timezone tz;
	long pass=0;
	BOOL done=false;
	while(!done && pass<5)
	{
		usleep(10000);

		gettimeofday(&tv1,&tz);
		tick1=ifxGetSystemTick();

		usleep(10000);

		gettimeofday(&tv2,&tz);
		tick2=ifxGetSystemTick();

		ms=(tv2.tv_usec-tv1.tv_usec)/1000;

//		printf("%d ticks in %d ms\n",tick2-tick1,ms);
		if(tick2<=tick1 || tv2.tv_usec<tv1.tv_usec)
		{
			pass++;
			continue;
		}

		ticksPerMS=(tick2-tick1)/ms;
		done=true;
	}
	if(!ticksPerMS)
		ticksPerMS=1000000;
	return ticksPerMS;
}
#endif



/******************************************************************************
class GPGBonePortal

	NOTE Camera transforms are negated and in reverse order

******************************************************************************/
class GPGBonePortal: public WDS_Portal
	{
	public:
				GPGBonePortal(void)
					{
					fovy=60.0f;
					nearplane=0.1f;
					farplane=10000.0f;
					mousebuttons=0;

					SetEventMask(GetLocalEventMask());
					GetLocalEventMask()->SetSource(
							EW_EVENT_COMMON|EW_EVENT_FOCUS|EW_EVENT_PERIODIC);
					};

				//	`as WDS_Portal'
virtual	long	Operate(EW_Event *event);
virtual	void	Draw(EW_Redraw *redraw);

virtual	void	IdleFunction(void)											=0;
virtual	void	Draw3D(void)												=0;
virtual	void	Draw2D(void)												=0;
virtual	void	React(EW_Event *event)										=0;
virtual	void	PlaceCamera(void)
					{
					glRotatef(-90.0f,1.0f,0.0f,0.0f);
					glTranslatef(0.0f,150.0f,-50.0f);
					};

		void	DrawBanner(const char *text);

		void	SetFOVY(float set)					{ fovy=set; };
		void	SetNearFar(float n,float f)
					{
					nearplane=n;
					farplane=f;
					};

static	long	GetSystemTimer(void)
				{
#if GPG_OSTYPE==GPG_WIN32
					return timeGetTime();
#else
					struct timeval tv;
					struct timezone tz;
					gettimeofday(&tv,&tz);
					return tv.tv_usec/1000;
#endif
				}
		void	SetTimer(long set)		{ m_timer=set; };
		long	GetTimer(void)			{ return m_timer; };
		void	AccumulateTime(long exclusive,long inclusive);
		void	GetFPS(float *exclusive,float *inclusive)
				{
					*exclusive=m_fps_excl;
					*inclusive=m_fps_incl;
				}

		void	SetMouseButtons(long set)	{ mousebuttons=set; };
		long	GetMouseButtons(void) const	{ return mousebuttons; };

		void	SetCommandLine(char *line)
				{
					strncpy(commandline,line,GPG_COMMANDLINE_LENGTH);
					commandline[GPG_COMMANDLINE_LENGTH-1]=0;
				}
const	char	*GetCommandLine(void) const	{ return commandline; };

/*
		HGLRC	GetHGLRC(void)		{ return hglrc; };
		void	SetHGLRC(HGLRC set)	{ hglrc=set; };

		HDC		GetHDC(void)		{ return hdc; };
		void	SetHDC(HDC set)		{ hdc=set; };
*/

	// not inline:

//		long	Open(HINSTANCE hInstance,char *title);
//		void	SetDCPixelFormat(HDC hdc);
//		void	Loop(bool dropout=false);
		void	Resize(GLsizei w,GLsizei h);
		void	DrawViewer(void);
		void	DrawImageU8(long width,long height,unsigned char *data);
		void	Prepare2D(void);
		void	Prepare3D(void);

//		bool	LoadFont(void);
//		long	GetFontBase(void)	{ return fontbase; };

		void	UpdateTime(void);
//		void	DrawText(const char *text);
		void	ReportErrors(char *text);
//		void	GeneratePalette(void);

	private:
		char			commandline[GPG_COMMANDLINE_LENGTH];
		float			fovy;
		float			nearplane;
		float			farplane;

//		HWND			hWnd;
//		WNDCLASS		wndClass;
//		MSG				msg;

		long			m_timer;
		long			m_time_incl;
		long			m_time_excl;
		long			m_frames;
		float			m_fps_incl;
		float			m_fps_excl;

//		HGLRC			hglrc;
//		HDC				hdc;

//		HFONT			fonthandle;
//		long			fontbase;

//		HPALETTE		hpalette;

		long			mousebuttons;
	};



#endif /* __GPGBonePortal_h__ */
