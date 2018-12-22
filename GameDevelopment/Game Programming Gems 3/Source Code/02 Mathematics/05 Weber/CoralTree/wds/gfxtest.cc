/******************************************************************************

Coral Tree wds/gfxtest.cc
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


#define WDS_GFX_FAST			TRUE

#define WDS_GFX_BOXSIZE			12
#define WDS_GFX_COUNTER_GRAIN	30

#define WDS_GFX_IMAGE_X			120
#define WDS_GFX_IMAGE_Y			40

#define WDS_GFX_DIAL_X			25
#define WDS_GFX_DIAL_Y0			80
#define WDS_GFX_DIAL_Y1			130
#define WDS_GFX_DIAL_Y2			180
#define WDS_GFX_DIAL_R			20
#define WDS_GFX_DIAL_MARK		3

#define WDS_GFX_BEVEL_MIN		12
#define WDS_GFX_BEVEL_MAX		24

#define WDS_GFX_ELLIPSE_X		80
#define WDS_GFX_ELLIPSE_Y0		60
#define WDS_GFX_ELLIPSE_Y1		120

#define WDS_GFX_ANGLE_1			160	// 60
#define WDS_GFX_ANGLE_2			400	// 200

#define WDS_GFX_POLY_X			60
#define WDS_GFX_POLY_Y0			150
#define WDS_GFX_POLY_Y1			150
#define WDS_GFX_POLY_SIZE		40

#define WDS_GFX_IMAGE			FALSE



/******************************************************************************
	WDS_GfxTest::WDS_GfxTest(void)

******************************************************************************/
WDS_GfxTest::WDS_GfxTest(void)
	{
	SetName("GfxTest");
	SetClipping(EW_CLIPPING_ALL);

	work=0;

	counter[0]=0;
	counter[1]=0;
	counter[2]=0;
	counter[3]=0;

	size[0]=10;
	size[1]=30;
	delta[0]=1;
	delta[1]=2;

	GetLocalLook()->SetElement(WDS_STATE_0,
								WDS_ELEMENT_BORDER,
									WDS_BEVELBOX_IN|
									WDS_BEVELBOX_SMOOTH|
									WDS_BEVELBOX_POINT_UP);
	GetLocalLook()->SetElement(WDS_STATE_1,
								WDS_ELEMENT_BORDER,
									WDS_BEVELBOX_OUT|
									WDS_BEVELBOX_POINT_DOWN);
	GetLocalLook()->SetElement(WDS_STATE_2,
								WDS_ELEMENT_BORDER,
									WDS_BEVELBOX_OUT|
									WDS_BEVELBOX_POINT_LEFT);
	GetLocalLook()->SetElement(WDS_STATE_3,
								WDS_ELEMENT_BORDER,
									WDS_BEVELBOX_OUT|
									WDS_BEVELBOX_RIDGED|
									WDS_BEVELBOX_SMOOTH|
									WDS_BEVELBOX_POINT_RIGHT);

	GetLocalLook()->SetElement(WDS_STATE_ALL,WDS_ELEMENT_DEPTH,2);
	GetLocalLook()->SetElement(WDS_STATE_3,
								WDS_ELEMENT_DEPTH,6);

	SetEventMask(GetLocalEventMask());
	GetLocalEventMask()->SetSource(EW_EVENT_COMMON|EW_EVENT_PERIODIC);

	// pre-mature for valid GetRootWindow()->GetNumberColors()
	loaded_image=ewLoadImageXPM("xv.xpm",16,EW_XPM_FLAG_NONE,NULL,&loadedsize[0],&loadedsize[1]);
	}



/******************************************************************************
	WDS_GfxTest::~WDS_GfxTest(void)

******************************************************************************/
WDS_GfxTest::~WDS_GfxTest(void)
	{
	if(loaded_image)
		osdFree(loaded_image);
	}



/******************************************************************************
long	WDS_GfxTest::Operate(EW_Event *event)

Keywords:
******************************************************************************/
long WDS_GfxTest::Operate(EW_Event *event)
	{
#if FALSE
	EW_PRINT(EW_WIDGET,EW_LOG,"WDS_GfxTest::Operate()");
	event->Print((long)this);
#endif

	if(event->IsTimer())
		{
		long index=(event->GetState()-1);

		counter[index]=(counter[index]+1)%WDS_GFX_COUNTER_GRAIN;

		Increment();

		if(index==2)
			{
			work=(work+1)%7;
			GetRootWindow()->SetDoWork(work==1);

#if FALSE
			if(work==1)
				GetRootWindow()->Raise(TRUE);
			else
				GetRootWindow()->Lower(TRUE);
#endif
			}
		}

	if(event->IsWork())
		{
		counter[0]=(counter[0]+1)%WDS_GFX_COUNTER_GRAIN;

		Increment();

//		osdMilliSleep(10);
		}

	return TRUE;
	}



/******************************************************************************
void	WDS_GfxTest::Increment(void)

	per-frame value changes

Keywords:
******************************************************************************/
void WDS_GfxTest::Increment(void)
	{
	long m;

	for(m=0;m<2;m++)
		{
		size[m]+=delta[m];
		if( (size[m]>WDS_GFX_BEVEL_MAX && delta[m]>0) || (size[m]<WDS_GFX_BEVEL_MIN && delta[m]<0) )
			delta[m]= -delta[m];
		}

	counter[3]++;
	long x,y,yx;
	float rad;

	for(y=0;y<WDS_GFXTEST_IMAGESIZE;y++)
		{
		yx=WDS_GFXTEST_IMAGESIZE*y;
		for(x=0;x<WDS_GFXTEST_IMAGESIZE;x++)
			{
			rad=sqrt(x*x+y*y);

			made_image[yx+x]=(long)(rad+counter[3])/3 %16;
			}
		}

	SetDirty(EW_DIRTY_COMPLETE);
	}



/******************************************************************************
void	WDS_GfxTest::Draw(EW_Redraw *redraw)

Keywords:
******************************************************************************/
void WDS_GfxTest::Draw(EW_Redraw *redraw)
	{
//	printf("WDS_GfxTest::Draw()\n");

	EW_Window *root=GetRootWindow();

	long truex,truey,sx,sy;
	long m;
	//long n;
	long x,y,r1,r2;

	float angle;


	GetTrueGeometry(&truex,&truey,&sx,&sy);

	root->RectFill(truex,truey,truex+sx-1,truey+sy-1,InterpretElement(WDS_STATE_ALL,EW_COLOR_BACKGROUND));
//	root->RectWire(truex,truey,truex+sx-1,truey+sy-1,(long)EW_BLUE);

	long dial;
	for(dial=0;dial<3;dial++)
		{
		x=truex+WDS_GFX_DIAL_X;
		y=truey;
		r1=WDS_GFX_DIAL_R;
		r2=r1+WDS_GFX_DIAL_MARK;

		switch(dial)
			{
			case 0:
				y+=WDS_GFX_DIAL_Y0;
				break;
			case 1:
				y+=WDS_GFX_DIAL_Y1;
				break;
			case 2:
				y+=WDS_GFX_DIAL_Y2;
				break;
			};

		angle= 2.0*M_PI * counter[dial]/(float)WDS_GFX_COUNTER_GRAIN;

		root->Line(x,y,x+r1*sin(angle),y+r1*cos(angle),EW_YELLOW);

		for(m=0;m<WDS_GFX_COUNTER_GRAIN;m++)
			{
			angle=2.0*M_PI * m/(float)WDS_GFX_COUNTER_GRAIN;

			root->Line(x+r1*sin(angle),y+r1*cos(angle),x+r2*sin(angle),y+r2*cos(angle),EW_RED);
			}
		}

	long ix,iy;

	for(m=0;m<16;m++)
		{
		ix=m%4;
		iy=m/4;

		x=truex+(WDS_GFX_BOXSIZE+1)*ix;
		y=truey+(WDS_GFX_BOXSIZE+1)*iy;

		long color= (m==16)? EW_DARKGREY: m;

		root->RectWire(x,y,x+WDS_GFX_BOXSIZE-1,y+WDS_GFX_BOXSIZE-1,EW_BLACK);
		root->RectFill(x+2,y+2,x+WDS_GFX_BOXSIZE-3,y+WDS_GFX_BOXSIZE-3,color);
		}

#if WDS_GFX_FAST
	if(TRUE)
		return;
#endif

	ix=truex + WDS_GFX_BEVEL_MAX/2 + (WDS_GFX_BOXSIZE+1)*5;
//	iy=truey + WDS_GFX_BEVEL_MAX/2 + WDS_GFX_BOXSIZE;
	iy=truey + WDS_GFX_BEVEL_MAX/2 + 5;

	long ix2,iy2;
	for(m=0;m<4;m++)
		{
		ix2=ix-size[0]/2;
		iy2=iy-size[1]/2;

		SetWidgetState(m);
		BevelBox(FALSE,TRUE,NULL,ix2,iy2,size[0],size[1]);
		root->RectWire(ix2-1,iy2-1,
					ix2+size[0],iy2+size[1],EW_GREEN);

		ix+=WDS_GFX_BEVEL_MAX*5/4;
		}

	// for all the angles and arcangles
	long arcangle[3];
	for(m=0;m<3;m++)
		arcangle[m]= (long)(360.0*counter[m]/(float)WDS_GFX_COUNTER_GRAIN);

/*
	arcangle[0]=45.0;
	arcangle[1]=340.0;
	arcangle[2]=200.0;
*/

	root->ArcFill(truex+WDS_GFX_ELLIPSE_X,truey+WDS_GFX_ELLIPSE_Y0,size[1],size[0],arcangle[0],arcangle[1]);
	root->ArcWire(truex+WDS_GFX_ELLIPSE_X,truey+WDS_GFX_ELLIPSE_Y1,size[0],size[1],arcangle[2],arcangle[1]);

	root->PolygonalArc(TRUE,truex+WDS_GFX_ELLIPSE_X,truey+WDS_GFX_ELLIPSE_Y0,size[1]-4,size[0]-4,arcangle[0],arcangle[1],EW_BLUE);
	root->PolygonalArc(FALSE,truex+WDS_GFX_ELLIPSE_X,truey+WDS_GFX_ELLIPSE_Y1,size[0]-4,size[1]-4,arcangle[2],arcangle[1],EW_BLUE);

	static float s_pentagon[5][2]=
		{
		{0.2, 0.0},
		{0.8, 0.2},
		{1.0, 0.6},
		{0.4, 1.0},
		{0.0, 0.6},
		};

	long pentagon[5][2];

	for(m=0;m<5;m++)
		{
		pentagon[m][0]=(long)(truex+WDS_GFX_POLY_X+
		  s_pentagon[m][0]*WDS_GFX_POLY_SIZE*size[1]/(float)WDS_GFX_BEVEL_MAX);
		pentagon[m][1]=(long)(truey+WDS_GFX_POLY_Y0+
		  s_pentagon[m][1]*WDS_GFX_POLY_SIZE*size[0]/(float)WDS_GFX_BEVEL_MAX);
		}

	root->PolyFill(pentagon,5);

	for(m=0;m<5;m++)
		{
		pentagon[m][0]=(long)(truex+WDS_GFX_POLY_X+
		  s_pentagon[m][0]*WDS_GFX_POLY_SIZE*size[0]/(float)WDS_GFX_BEVEL_MAX);
		pentagon[m][1]=(long)(truey+WDS_GFX_POLY_Y1+
		  s_pentagon[m][1]*WDS_GFX_POLY_SIZE*size[1]/(float)WDS_GFX_BEVEL_MAX);
		}

	root->PolyWire(TRUE,pentagon,5,EW_CYAN);

	root->ImageDraw(WDS_GFX_IMAGE_X,WDS_GFX_IMAGE_Y,WDS_GFXTEST_IMAGESIZE,WDS_GFXTEST_IMAGESIZE,made_image);

	if(WDS_GFX_IMAGE)
		root->ImageDraw(WDS_GFX_IMAGE_X,WDS_GFX_IMAGE_Y+2*WDS_GFXTEST_IMAGESIZE,loadedsize[0],loadedsize[1],loaded_image);

	root->ImageCopy(400,200,WDS_GFX_IMAGE_X,WDS_GFX_IMAGE_Y+3*WDS_GFXTEST_IMAGESIZE,75,100);
	}
