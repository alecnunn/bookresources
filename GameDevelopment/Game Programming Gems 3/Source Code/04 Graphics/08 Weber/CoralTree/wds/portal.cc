/******************************************************************************

Coral Tree wds/button.cc
Copyright (C) 1998 Infinite Monkeys Incorporated

This program is free software; you can redistribute it and/or modify
it under the terms of the Artistic License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
Artistic License for more details.

You should have received a copy of the Artistic License along with
this program (see meta/ART_LICE); if not, write to coral@imonk.com.

******************************************************************************/



#include "wds.h"

#define	WDS_USEGLU	TRUE
#if WDS_USEGLU
#include "GL/glu.h"
#endif


/*****************************************************************************
		WDS_Portal::WDS_Portal()

	initialize button upon construction

Keywords:
******************************************************************************/
WDS_Portal::WDS_Portal()
	{
	DB("WDS_Portal::WDS_Portal()");

	SetName("Portal");
	SetClipping(EW_CLIPPING_ALL);

	SetBounds(EW_BOUNDS_IGNORE_PARENT);
	SetAnnotatable(TRUE);

	// accept bypassed events
	SetEventMask(GetLocalEventMask());
	GetLocalEventMask()->SetSource(EW_EVENT_PERIODIC);

	angle=0.0f;
	}



/******************************************************************************
long WDS_Portal::Operate(EW_Event *event)

******************************************************************************/
long WDS_Portal::Operate(EW_Event *event)
	{
#if FALSE
	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_Portal::Operate()");
	event->Print((long)this);
#endif

//	if(event->IsWork())
	if(event->IsTimer())
		{
		angle+=10.0f;
		if(angle>360.0f)
			angle-=360.0f;
		SetDirty(EW_DIRTY_COMPLETE);
		}

	return TRUE;
	}



/******************************************************************************
void	WDS_Portal::Draw(EW_Redraw *redraw)

	draw the button

Keywords:
******************************************************************************/
void WDS_Portal::Draw(EW_Redraw *redraw)
	{
	DB("WDS_Portal::Draw()");

	BevelBox(TRUE,TRUE,NULL);

	if(!GetRootWindow()->GetRGBMode())
		return;

#if EW_GL==EW_OPENGL
	long rx,ry,rsx,rsy;
	GetRegion(FALSE,&rx,&ry,&rsx,&rsy);
	GetRootWindow()->RectFill(rx,ry,rx+rsx-1,ry+rsy-1,EW_BLACK);

	glViewport(rx,ry,rsx,rsy);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
#if WDS_USEGLU
	gluPerspective(30.0f,rsx/(float)rsy,1.0f,100.0f);
#endif
	glMatrixMode(GL_MODELVIEW);

	glClearIndex(EW_BLUE);
	glClear(GL_COLOR_BUFFER_BIT);

	glClearDepth(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glEnable(GL_DEPTH_TEST);

		// TODO drawing callback
		glTranslatef(0.0f,0.0f,-5.0f);

		glRotatef(angle,0.577f,0.577f,0.577f);
		DrawFigure();

	glDisable(GL_DEPTH_TEST);
	glPopMatrix();

	GetRootWindow()->UseFullScreenOrthoViewport();
#endif
	}



/******************************************************************************
void WDS_Portal::DrawFigure(void)

******************************************************************************/
void WDS_Portal::DrawFigure(void)
	{
	static float vert[5][3]={
		-0.5f,-0.5f,0.0f,
		0.5f,-0.5f,0.0f,
		0.5f,0.5f,0.0f,
		-0.5f,0.5f,0.0f,
		0.0f,0.0f,1.0f
		};

	const float red[3]=		{ 1.0f,0.0f,0.0f };
	const float green[3]=	{ 0.0f,1.0f,0.0f };
	const float blue[3]=	{ 0.0f,0.0f,1.0f };
	const float yellow[3]=	{ 1.0f,1.0f,0.0f };
	const float magenta[3]=	{ 1.0f,0.0f,1.0f };

#if EW_GL==EW_OPENGL
	glColor3fv(red);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3fv(vert[0]);
	glVertex3fv(vert[1]);
	glVertex3fv(vert[2]);
	glVertex3fv(vert[3]);
	glEnd();

	glBegin(GL_TRIANGLES);

	glColor3fv(green);
	glVertex3fv(vert[0]);
	glVertex3fv(vert[1]);
	glVertex3fv(vert[4]);

	glColor3fv(blue);
	glVertex3fv(vert[1]);
	glVertex3fv(vert[2]);
	glVertex3fv(vert[4]);

	glColor3fv(yellow);
	glVertex3fv(vert[2]);
	glVertex3fv(vert[3]);
	glVertex3fv(vert[4]);

	glColor3fv(magenta);
	glVertex3fv(vert[3]);
	glVertex3fv(vert[0]);
	glVertex3fv(vert[4]);

	glEnd();
#endif
	}
