
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


#include "GPGBonePortal.h"



#define GPGBONEPORTAL_FOLLOWTIMER	FALSE
#define GPGBONEPORTAL_EVENT_DEBUG	FALSE

#define GPGBONEPORTAL_COLORINDEX	FALSE
#define GPGBONEPORTAL_COLOROFFSET	10
#define GPGBONEPORTAL_COLORINDICES	150


void GPGBonePortal::DrawImageU8(long width,long height,unsigned char *data)
{
	glRasterPos3f(0.0f,0.0f,0.0f);
	glDrawPixels(width,height,GL_LUMINANCE,GL_UNSIGNED_BYTE,data);
}


long GPGBonePortal::Operate(EW_Event *event)
{
//	WDS_Portal::Operate(event);
	React(event);
	return FALSE;
}


void GPGBonePortal::Draw(EW_Redraw *redraw)
{
//	WDS_Portal::Draw(redraw);
	DrawViewer();
}


void GPGBonePortal::DrawViewer(void)
{
#if GPGBONEPORTAL_EVENT_DEBUG
	GPGLOG("GPGBonePortal::Draw()\n");
#endif

	ReportErrors("GPGBonePortal::Draw() pre");

	ReportErrors("GPGBonePortal::Draw() 1");
	Prepare3D();
	ReportErrors("GPGBonePortal::Draw() 2");

	long lasttime=GetTimer();
	long starttime=GetSystemTimer();
	SetTimer(starttime);

	Draw3D();
	long endtime=GetSystemTimer();
	ReportErrors("GPGBonePortal::Draw() 3");

	AccumulateTime(endtime-starttime,starttime-lasttime);

	Prepare2D();
	Draw2D();

	glFlush();
//	glFinish();

//	SwapBuffers(hdc);
//	ReportErrors("GPGBonePortal::Draw() 4");
//	ValidateRect(hWnd,NULL);

	ReportErrors("GPGBonePortal::Draw() post");
}


void GPGBonePortal::AccumulateTime(long exclusive,long inclusive)
{
	if(exclusive<0 || inclusive<0)
		return;

	const long update_ms=300;

	m_time_excl+=exclusive;
	m_time_incl+=inclusive;
	m_frames++;

//	printf("%d %d\n",exclusive,inclusive);

	if(inclusive==0 || m_time_incl>update_ms)
	{
		m_fps_excl=m_time_excl? m_frames/(m_time_excl*0.001f): 0.0f;
		m_fps_incl=m_time_incl? m_frames/(m_time_incl*0.001f): 0.0f;

//		printf("%.6G %.6G\n",m_fps_excl,m_fps_incl);

		m_time_incl=0;
		m_time_excl=0;
		m_frames=0;
	}
}


void GPGBonePortal::Prepare2D(void)
{
	long w,h;
	GetSize(&w,&h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-0.375,(float)w-0.375,
			-0.375,(float)h-0.375, -1.0,1.0);

	glViewport(0,0,w,h);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void GPGBonePortal::Prepare3D(void)
{
	long w,h;
	GetSize(&w,&h);

#if TRUE
	GLfloat aspect=(GLfloat)w/(GLfloat)h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy,aspect,nearplane,farplane);
#else
	static const float scalar=200.0f;

	GLfloat width,height;
	if(w<h)
	{
		height=scalar*h/w;
		width=scalar;
	}
	else
	{
		height=scalar;
		width=scalar*w/h;
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-width/2.0,width/2.0,
			-height/2.0,height/2.0,
			10.0f*scalar,-10.0f*scalar);
#endif

	glViewport(0,0,w,h);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	PlaceCamera();
}


void GPGBonePortal::DrawBanner(const char *text)
{
//	Loop(true);

	Prepare2D();

	glClearColor(0.3f,0.0f,0.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	long sx,sy;
	GetSize(&sx,&sy);

	float x=5.0f;
	float y=sy-20.0f;

	glColor3f(1.0f,1.0f,1.0f);
	glPushMatrix();
	glTranslatef(x,y,0.0f);

//	DrawText(text);
	glPopMatrix();

	glFlush();
//	SwapBuffers(hdc);
//	ValidateRect(hWnd,NULL);
}


void GPGBonePortal::ReportErrors(char *text)
{
	GLenum glError;

	bool first=false;
	while( (glError=glGetError()) != GL_NO_ERROR )
	{
		if(first)
		{
			first=false;
			char buffer[1024];
			sprintf(buffer,"GPGBonePortal::ReportErrors(%s)\n",text);
			GPGLOG(buffer);
		}

		GPGLOG((char *)gluErrorString(glError));
		GPGLOG("\n");
	}
}
