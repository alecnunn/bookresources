/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: invKim.cpp
 *    Desc: Inverse Kinematics Sample application
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#include "stdafx.h"

#include <string>
using std::string;

class cIKApp : public cApplication,
	public iKeyboardReceiver,
	public iMouseReceiver
{

public:

	/**
	 * Lengths of the two links
	 */
	float m_l1, m_l2;

	/**
	 * Angles of the two joings
	 */
	float m_theta1, m_theta2;

	/**
	 * Location in worldspace of the end effector
	 */
	point3 m_endEffector;

	/**
	 * Location in worldspace of the grey diamond
	 * that the end effector trails after
	 */
	point3 m_mouse;

	string m_helpText;

	/**
	 * how long the last frame took (the next one should take about as long)
	 */
	float m_timeDelta;

	/**
	 * If this is true, draw some help on the screen
	 */
	bool m_drawHelp;


	//==========--------------------------  cApplication

	virtual void DoFrame( float timeDelta );
	virtual void SceneInit();

	cIKApp() :
		cApplication()
	{
		m_title = string( "Inverse Kinematics Sample App" );
	}

	virtual void InitInput()
	{
		// force exclusive access to the mouse
		cInputLayer::Create( AppInstance(), MainWindow()->GetHWnd(), true, true, true );
	}


	//==========--------------------------  iKeyboardReceiver

	virtual void KeyUp( int key );
	virtual void KeyDown( int key ){}

	//==========--------------------------  iMouseReceiver

	virtual void MouseMoved( int dx, int dy );
	virtual void MouseButtonUp( int button ){}
	virtual void MouseButtonDown( int button ){}

	//==========--------------------------  cIKApp

	void DrawLinkage();
	void FindJointAngles( float x, float y );
};

cApplication* CreateApplication()
{
	return new cIKApp();
}

void DestroyApplication( cApplication* pApp )
{
	delete pApp;
}

void cIKApp::SceneInit()
{
	/**
	 * We're making the FOV less than 90 degrees.
	 * this is so the object doesn't warp as much
	 * when we're really close to it.
	 */
	Graphics()->SetProjectionData( PI/3, 1.f, 100.f );
	Graphics()->MakeProjectionMatrix();

	/**
	 * Tell the keyboard object to send us state
	 * changes.
	 */
	Input()->GetKeyboard()->SetReceiver( this );
	Input()->GetMouse()->SetReceiver( this );

	/**
	 * initialize our scene
	 */
	LPDIRECT3DDEVICE8 lpDevice = Graphics()->GetDevice();
	lpDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	lpDevice->SetRenderState(D3DRS_DITHERENABLE, TRUE);
	lpDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	lpDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS );

	/**
	 * initialize the camera to stare
	 * at the origin from the z-axis
	 */
	matrix4 camMat;
	camMat.ToTranslation( point3(0,0,25) );
	Graphics()->SetWorldMatrix( matrix4::Identity );
	Graphics()->SetViewMatrix( camMat );

	m_drawHelp = false;

	m_timeDelta = 0.03f; // a 30th of a second is a good first value

	m_l1 = 10.0;
	m_l2 = 8.0;
	m_endEffector = point3(5,5,0);
	m_mouse = point3(5,5,0);
	m_theta1 = 0.f;
	m_theta2 = 0.f;

	/**
	 * Create the help string
	 */
	m_helpText = string("\
Inverse Kinematics Sample Application\n\
Advanced 3D Game Programming using DirectX 8.0\n\
\n\
Controls:\n\
  Mouse: Control end-effector target\n\
  [H]:   Toggle help" );

}


void cIKApp::DoFrame( float timeDelta )
{
	/**
	 * update the time
	 */
	m_timeDelta = timeDelta;
	
	/**
	 * then, draw the frame.
	 */
	LPDIRECT3DDEVICE8 lpDevice = Graphics()->GetDevice();
	if( lpDevice )
	{
		Graphics()->Clear( true, true, -1, 1.f );

		lpDevice->SetVertexShader( 
			D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1 );

		Graphics()->BeginScene();

		/**
		 * Move the joints towards the mouse
		 */
		point3 dir = m_mouse - m_endEffector;
		float mag = dir.Mag();
		if( mag > 0.1f )
		{
			dir /= mag;
			Snap( mag, 0.f, 1.f );
			m_endEffector += 10.f * m_timeDelta * mag * dir;
		}

		/**
		 * Compute the theta angles for the
		 * new position of the end effector
		 */
		FindJointAngles( m_endEffector.x, m_endEffector.y );

		/**
		 * Draw the linkage
		 */
		DrawLinkage();

		

		if( m_drawHelp )
		{
			Graphics()->DrawTextString(
				2,
				2,
				0x40404040,
				(char*)m_helpText.c_str()); 
		}
		else
		{
			Graphics()->DrawTextString(
				2,
				2,
				0x40404040,
				"Press [H] for help" ); 
		}

		Graphics()->EndScene();
		/**
		 * flip the buffer.
		 */
		Graphics()->Flip();
	}
}


void cIKApp::KeyUp( int key )
{
	if( key == DIK_H )
	{
		m_drawHelp = !m_drawHelp;
	}
}


void cIKApp::MouseMoved( int dx, int dy )
{
	m_mouse.x += dx * 0.1f;
	m_mouse.y -= dy * 0.1f;
}

void cIKApp::DrawLinkage()
{
	/**
	 * Use the lengths and theta information
	 * to compute the forward dynamics of 
	 * the arm, and draw it.
	 */
	sLitVertex box[4];
	sLitVertex joint[20];
	matrix4 rot1, trans1;
	matrix4 rot2, trans2;

	/**
	 * create a half circle to give our links rounded edges
	 */
	point3 halfCircle[10];
	int i;
	for( i=0; i<10; i++ )
	{
		float theta = (float)i*PI/9.f;
		halfCircle[i] = point3(
			0.85f * sin( theta ),
			0.85f * cos( theta ),
			0.f );
	}

	rot1.ToZRot( m_theta1 );
	trans1.ToTranslation( point3( m_l1,0, 0 ) );

	rot2.ToZRot( m_theta2 );

	LPDIRECT3DDEVICE8 pDevice = Graphics()->GetDevice();

	/**
	 * Make and draw the upper arm
	 */
	matrix4 shoulderMat = rot1;
	for( i=0; i<10; i++ )
	{
		point3 temp = halfCircle[i];
		temp.x = -temp.x;
		joint[i] = sLitVertex( shoulderMat * temp, 0xFF8080 );
		joint[19-i] = sLitVertex( shoulderMat * (halfCircle[i] + point3( m_l1, 0, 0 )), 0x80FF80 );
	}

	
	pDevice->DrawPrimitiveUP( 
		D3DPT_TRIANGLEFAN,
		18,
		joint,
		sizeof( sLitVertex ) );


	/**
	 * Make and draw the lower arm
	 */
	matrix4 elbowMat = rot2 * trans1 * rot1;
	for( i=0; i<10; i++ )
	{
		point3 temp = halfCircle[i];
		temp.x = -temp.x;
		joint[i] = sLitVertex( elbowMat * temp, 0x80FF80 );
		joint[19-i] = sLitVertex( elbowMat * (halfCircle[i] + point3( m_l2, 0, 0.f )), 0x8080FF );
	}


	pDevice->DrawPrimitiveUP( 
		D3DPT_TRIANGLEFAN,
		18,
		joint,
		sizeof( sLitVertex ) );

	/**
	 * Draw a diamond where the mouse is
	 */
	matrix4 mouseTrans;
	mouseTrans.ToTranslation( m_mouse );
	box[0] = sLitVertex( point3(0.5f,0.f,0.f) * mouseTrans, 0x808080 );
	box[1] = sLitVertex( point3(0.f,-0.5f,0.f) * mouseTrans, 0x808080 );
	box[2] = sLitVertex( point3(-0.5f,0.f,0.f) * mouseTrans, 0x808080 );
	box[3] = sLitVertex( point3(0.f,0.5f,0.f) * mouseTrans, 0x808080 );


	pDevice->DrawPrimitiveUP( 
		D3DPT_TRIANGLEFAN,
		2,
		box,
		sizeof( sLitVertex ) );

}

void cIKApp::FindJointAngles( float x, float y )
{
	float minD = (float)fabs(m_l1 - m_l2 );
	float maxD = m_l1 + m_l2;

	float L1 = m_l1;
	float L2 = m_l2;

	/**
	 * Find the standard theta and distance
	 */
	float dist = (float)sqrt(x*x+y*y);
	float theta = (float)atan2(y,x);

	/**
	 * Snap the distance to values we can reach
	 */
	Snap( dist, minD + EPSILON, maxD - EPSILON );

	/**
	 * Adjust the x and y to match the new distance
	 */
	x = (float)cos(theta)*dist;
	y = (float)sin(theta)*dist;

	/**
	 * Find thetaHat using the law of cosines
	 */

	float thetaHat = (float)acos( ( L2*L2 - L1*L1 - dist*dist ) / (-2*dist*L1) );

	/**
	 * theta - thetaHat is theta 1
	 */
	m_theta1 = theta - thetaHat;

	/**
	 * Use the law of cosines to get thetaArm
	 */
	float thetaArm = (float)acos( ( dist*dist - L1*L1 - L2*L2 ) / (-2*L2*L1) );

	/**
	 * With thetaArm we can easily find theta2
	 */
	m_theta2 = PI-thetaArm;

}