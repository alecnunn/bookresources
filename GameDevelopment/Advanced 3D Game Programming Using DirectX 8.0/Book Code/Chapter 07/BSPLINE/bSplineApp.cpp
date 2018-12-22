/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: bSplineApp.cpp
 *    Desc: Simple B-Spline application.  The vectors grow forever,
 *          This app isn't supposed to run forever, it's just 
 *          supposed to show how to use B-Splines.
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#include "stdafx.h"

#include "bspline.h"

// the thing that dances around
class cBSplineTrail
{
	vector< sLitVertex >	m_vertList;
	cBSpline m_spline;

	/**
	 * Generate a point somewhere between (-10,-10,-10), (10,10,10)
	 */
	point3 GenRandomPoint()
	{
		float scale = 10.f;
		return point3(
			scale * (float)(rand()-(RAND_MAX/2)) / (RAND_MAX/2),
			scale * (float)(rand()-(RAND_MAX/2)) / (RAND_MAX/2),
			scale * (float)(rand()-(RAND_MAX/2)) / (RAND_MAX/2));
	}

public:

	cBSplineTrail()
	{
		m_spline.AddCtrlPoint( GenRandomPoint() );
		m_spline.AddCtrlPoint( GenRandomPoint() );
		m_spline.AddCtrlPoint( GenRandomPoint() );
		m_spline.AddCtrlPoint( GenRandomPoint() );

		m_currStep = 0;
		m_nSteps = 30;
		m_startPoint = 0;
		m_trailLength = 200;
	}

	color3 m_tipColor;
	color3 m_mainColor;
	int m_currStep;
	int m_nSteps;
	int m_startPoint;
	int m_trailLength;

	void Process();
	void Draw();
};


class cBSplineApp : public cApplication  
{
	cBSplineTrail	m_redTrail;
	cBSplineTrail	m_greenTrail;
	cBSplineTrail	m_blueTrail;
	cBSplineTrail	m_magentaTrail;
	cBSplineTrail	m_cyanTrail;
	cBSplineTrail	m_yellowTrail;

public:

	cBSplineApp() :
		cApplication()
	{
		m_title = string( "BSpline Sample" );
	}
	~cBSplineApp()
	{
	}

	virtual void SceneInit();
	virtual void DoFrame( float timeDelta );
};

cApplication* CreateApplication()
{
	return new cBSplineApp();
}


/**
 * EachFrame: Perform processing on our trails and draw them
 */
void cBSplineApp::DoFrame( float timeDelta )
{
	LPDIRECT3DDEVICE8 pDevice = Graphics()->GetDevice();

	static float yaw = 0.f;
	static float pitch = 0.f;
	static float roll = 0.f;
	static float dist = 11.0f;

	// process the trails
	m_blueTrail.Process();
	m_redTrail.Process();
	m_greenTrail.Process();
	m_magentaTrail.Process();
	m_cyanTrail.Process();
	m_yellowTrail.Process();

	// rotate things around a little
	yaw += timeDelta * 0.8f;
	pitch += timeDelta* 0.35f;

	matrix4 objMat;
	objMat.MakeIdent();
	objMat.Rotate( yaw, pitch, roll );
	objMat.Place( point3(0.0f, 0.f, dist) );

	Graphics()->GetDevice()->SetVertexShader( 
		D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1 );

	// then, draw the frame.
	Graphics()->Clear( true, true, 0x00000000, 1.f );

	Graphics()->BeginScene();

	Graphics()->SetWorldMatrix( objMat );

	m_blueTrail.Draw();
	m_redTrail.Draw();
	m_greenTrail.Draw();
	m_magentaTrail.Draw();
	m_cyanTrail.Draw();
	m_yellowTrail.Draw();

	Graphics()->EndScene();

	// flip the buffer.
	Graphics()->Flip();
}

void cBSplineApp::SceneInit()
{

	// initialize the camera
	Graphics()->SetProjectionData( PI/3, 1.f, 100.f );
	Graphics()->MakeProjectionMatrix();

	Graphics()->SetViewMatrix( matrix4::Identity );
	Graphics()->GetDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );



	m_blueTrail.m_mainColor = color3::Blue;
	m_blueTrail.m_tipColor = color3::Yellow * 1.0f + color3::Blue*0.8f;

	m_redTrail.m_mainColor = color3::Red;
	m_redTrail.m_tipColor = color3::Yellow * 1.0f + color3::Blue*0.8f;

	m_greenTrail.m_mainColor = color3::Green;
	m_greenTrail.m_tipColor = color3::Yellow * 1.0f + color3::Blue*0.8f;

	m_magentaTrail.m_mainColor = color3::Magenta;
	m_magentaTrail.m_tipColor = color3::Yellow * 1.0f + color3::Blue*0.8f;

	m_cyanTrail.m_mainColor = color3::Cyan;
	m_cyanTrail.m_tipColor = color3::Yellow * 1.0f + color3::Blue*0.8f;

	m_yellowTrail.m_mainColor = color3::Yellow;
	m_yellowTrail.m_tipColor = color3::Yellow * 1.0f + color3::Blue*0.8f;

}


void cBSplineTrail::Process()
{
	// calculate the next step
	float t = (float)m_currStep/m_nSteps;
	m_vertList.push_back( sLitVertex( m_spline.Calc(t, m_spline.NumPoints()-4), m_tipColor.MakeDWord(), 0, 0, 0 ) );

	if( m_currStep++ == m_nSteps )
	{
		m_currStep = 0;
		m_spline.AddCtrlPoint( GenRandomPoint() );
	}

	int i;
	t = 0.f;
	for( i=m_vertList.size()-(m_trailLength-40); i>=0 && i>m_vertList.size()-m_trailLength; i-- )
	{
		// color the trailing 40 vertices black
		m_vertList[i].diffuse = (color3::Black * t + m_mainColor * (1-t)).MakeDWord();
		t += 1.f/41.f;
	}

	t = 0.f;
	for( i=m_vertList.size()-1; i>=0 && i>m_vertList.size()-20; i-- )
	{
		// color the trailing 20 vertices black
		m_vertList[i].diffuse = (m_mainColor * t + m_tipColor* (1-t)).MakeDWord();
		t += 1.f/21.f;
	}

	m_startPoint = m_vertList.size()-m_trailLength;
	if( m_startPoint < 0 ) 
		m_startPoint = 0;


}

void cBSplineTrail::Draw()
{

	Graphics()->GetDevice()->DrawPrimitiveUP(
		D3DPT_LINESTRIP,
		m_vertList.size() - m_startPoint - 1,
		&m_vertList[m_startPoint],
		sizeof( sLitVertex ) );

}

