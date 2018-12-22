//==============================================================
//= Camera.cpp
//= Original coders: Trent Polack (trent@codershq.com)
//==============================================================
//= This file contains the code routines for a simple camera.
//==============================================================
#include <SDL_opengl.h>
#include <cmath>

#include "Camera.h"
#include "..\\MathLib\\Matrix.h"
using namespace DemoFramework;

//-
//compute the view matrix based on the camera's information
void Camera::ComputeViewMatrix( float timeDelta ) {
	//clamp the pitch value
	CLAMP( m_pitch, -89.0f, 89.0f );

	//calculate some variables used in the vector calculations below
	float cosYaw  = cosf( DEG_TO_RAD( m_yaw ) );
	float sinYaw  = sinf( DEG_TO_RAD( m_yaw ) );
	float sinPitch= sinf( DEG_TO_RAD( m_pitch ) );
	float cosPitch= cosf( DEG_TO_RAD( m_pitch ) );

	m_forward[0]= sinYaw*cosPitch;
	m_forward[1]= sinPitch;
	m_forward[2]= cosPitch*( -cosYaw );

	m_lookAt= m_eyePos + m_forward;

	m_side= m_forward.CrossProduct( m_up );
}

//-
//set the view matrix
void Camera::SetViewMatrix( void )
{
	//have OpenGL calculate the viewing matrix
	gluLookAt( m_eyePos[0], m_eyePos[1], m_eyePos[2],
			   m_lookAt[0], m_lookAt[1], m_lookAt[2],
			   m_up[0],	    m_up[1],	 m_up[2] );
}

//-
//compute the view frustum based on information from the projection/modelview matricies
void Camera::ComputeViewFrustum( void ) {
	MathLib::Matrix projMtrx;	//projection matrix
	MathLib::Matrix mmMtrx;	//modelview matrix
	MathLib::Matrix clip;
	float norm;

	//the projection matrix
	glGetFloatv( GL_PROJECTION_MATRIX, projMtrx.Get( ) );

	//the modelview matrix
	glGetFloatv( GL_MODELVIEW_MATRIX, mmMtrx.Get( ) );

	//multiply the two matrices
	clip= projMtrx.MultiplyMatrix( mmMtrx );

	//extract the numbers for the RIGHT plane
	m_viewFrustum[0][0]= clip.m_matrix[ 3] - clip.m_matrix[ 0];
	m_viewFrustum[0][1]= clip.m_matrix[ 7] - clip.m_matrix[ 4];
	m_viewFrustum[0][2]= clip.m_matrix[11] - clip.m_matrix[ 8];
	m_viewFrustum[0][3]= clip.m_matrix[15] - clip.m_matrix[12];

	//normalize the plane
	norm= ( float )sqrt( SQR( m_viewFrustum[0][0] )+
						 SQR( m_viewFrustum[0][1] )+
						 SQR( m_viewFrustum[0][2] ) );
	m_viewFrustum[0][0]/= norm;
	m_viewFrustum[0][1]/= norm;
	m_viewFrustum[0][2]/= norm;
	m_viewFrustum[0][3]/= norm;

	//extract the numbers for the LEFT plane
	m_viewFrustum[1][0]= clip.m_matrix[ 3] + clip.m_matrix[ 0];
	m_viewFrustum[1][1]= clip.m_matrix[ 7] + clip.m_matrix[ 4];
	m_viewFrustum[1][2]= clip.m_matrix[11] + clip.m_matrix[ 8];
	m_viewFrustum[1][3]= clip.m_matrix[15] + clip.m_matrix[12];

	//normalize the plane
	norm= ( float )sqrt( SQR( m_viewFrustum[1][0] )+
						 SQR( m_viewFrustum[1][1] )+
						 SQR( m_viewFrustum[1][2] ) );
	m_viewFrustum[1][0]/= norm;
	m_viewFrustum[1][1]/= norm;
	m_viewFrustum[1][2]/= norm;
	m_viewFrustum[1][3]/= norm;

	//extract the BOTTOM plane
	m_viewFrustum[2][0]= clip.m_matrix[ 3] + clip.m_matrix[ 1];
	m_viewFrustum[2][1]= clip.m_matrix[ 7] + clip.m_matrix[ 5];
	m_viewFrustum[2][2]= clip.m_matrix[11] + clip.m_matrix[ 9];
	m_viewFrustum[2][3]= clip.m_matrix[15] + clip.m_matrix[13];

	//normalize the plane
	norm= ( float )sqrt( SQR( m_viewFrustum[2][0] )+
						 SQR( m_viewFrustum[2][1] )+
						 SQR( m_viewFrustum[2][2] ) );
	m_viewFrustum[2][0]/= norm;
	m_viewFrustum[2][1]/= norm;
	m_viewFrustum[2][2]/= norm;
	m_viewFrustum[2][3]/= norm;

	//extract the TOP plane
	m_viewFrustum[3][0]= clip.m_matrix[ 3] - clip.m_matrix[ 1];
	m_viewFrustum[3][1]= clip.m_matrix[ 7] - clip.m_matrix[ 5];
	m_viewFrustum[3][2]= clip.m_matrix[11] - clip.m_matrix[ 9];
	m_viewFrustum[3][3]= clip.m_matrix[15] - clip.m_matrix[13];

	//normalize the plane
	norm= ( float )sqrt( SQR( m_viewFrustum[3][0] )+
						 SQR( m_viewFrustum[3][1] )+
						 SQR( m_viewFrustum[3][2] ) );
	m_viewFrustum[3][0]/= norm;
	m_viewFrustum[3][1]/= norm;
	m_viewFrustum[3][2]/= norm;
	m_viewFrustum[3][3]/= norm;

	//extract the FAR plane
	m_viewFrustum[4][0]= clip.m_matrix[ 3] - clip.m_matrix[ 2];
	m_viewFrustum[4][1]= clip.m_matrix[ 7] - clip.m_matrix[ 6];
	m_viewFrustum[4][2]= clip.m_matrix[11] - clip.m_matrix[10];
	m_viewFrustum[4][3]= clip.m_matrix[15] - clip.m_matrix[14];

	//normalize the plane
	norm= ( float )sqrt( SQR( m_viewFrustum[4][0] )+
						 SQR( m_viewFrustum[4][1] )+
						 SQR( m_viewFrustum[4][2] ) );
	m_viewFrustum[4][0]/= norm;
	m_viewFrustum[4][1]/= norm;
	m_viewFrustum[4][2]/= norm;
	m_viewFrustum[4][3]/= norm;

	//extract the NEAR plane
	m_viewFrustum[5][0]= clip.m_matrix[ 3] + clip.m_matrix[ 2];
	m_viewFrustum[5][1]= clip.m_matrix[ 7] + clip.m_matrix[ 6];
	m_viewFrustum[5][2]= clip.m_matrix[11] + clip.m_matrix[10];
	m_viewFrustum[5][3]= clip.m_matrix[15] + clip.m_matrix[14];

	//normalize the plane
	norm= ( float )sqrt( SQR( m_viewFrustum[5][0] )+
						 SQR( m_viewFrustum[5][1] )+
						 SQR( m_viewFrustum[5][2] ) );
	m_viewFrustum[5][0]/= norm;
	m_viewFrustum[5][1]/= norm;
	m_viewFrustum[5][2]/= norm;
	m_viewFrustum[5][3]/= norm;
}

//-
//test a vertex for inclusion in the view frustum
bool Camera::VertexInFrustum( float x, float y, float z ) {
	int i;

	//loop through the frustum planes
	for( i=0; i < 6; i++ ) {
		if( m_viewFrustum[i][0]*x + m_viewFrustum[i][1]*y + m_viewFrustum[i][2]*z + m_viewFrustum[i][3] <= 0 )
			return false;
		
		if( m_viewFrustum[i][0]*x + m_viewFrustum[i][1]*y + m_viewFrustum[i][2]*z + m_viewFrustum[i][3] <= 0 )
			return false;

		if( m_viewFrustum[i][0]*x + m_viewFrustum[i][1]*y + m_viewFrustum[i][2]*z + m_viewFrustum[i][3] <= 0 )
			return false;
	}

	return true;
}

//-
//test a cube (AABB) for inclusion in the view frustum
bool Camera::CubeInFrustum( float x, float y, float z, float size ) {
	int i;

	//loop through and test the six vertices of the bounding box against the viewing frustum
	for( i=0; i < 6; i++ ) {
		if( m_viewFrustum[i][0] * ( x-size )+m_viewFrustum[i][1] * 
								  ( y-size )+m_viewFrustum[i][2] * 
								  ( z-size )+m_viewFrustum[i][3] > 0 )
			continue;
		if( m_viewFrustum[i][0] * ( x+size )+m_viewFrustum[i][1] * 
								  ( y-size )+m_viewFrustum[i][2] * 
								  ( z-size )+m_viewFrustum[i][3] > 0 )
			continue;
		if( m_viewFrustum[i][0] * ( x-size )+m_viewFrustum[i][1] * 
								  ( y+size )+m_viewFrustum[i][2] * 
								  ( z-size )+m_viewFrustum[i][3] > 0 )
			continue;
		if( m_viewFrustum[i][0] * ( x+size )+m_viewFrustum[i][1] * 
								  ( y+size )+m_viewFrustum[i][2] * 
								  ( z-size )+m_viewFrustum[i][3] > 0 )
			continue;
		if( m_viewFrustum[i][0] * ( x-size )+m_viewFrustum[i][1] *
								  ( y-size )+m_viewFrustum[i][2] *
								  ( z+size )+m_viewFrustum[i][3] > 0 )
			continue;
		if( m_viewFrustum[i][0] * ( x+size )+m_viewFrustum[i][1] *
								  ( y-size )+m_viewFrustum[i][2] * 
								  ( z+size )+m_viewFrustum[i][3] > 0 )
			continue;
		if( m_viewFrustum[i][0] * ( x-size )+m_viewFrustum[i][1] *
								  ( y+size )+m_viewFrustum[i][2] * 
								  ( z+size )+m_viewFrustum[i][3] > 0 )
			continue;
		if( m_viewFrustum[i][0] * ( x+size )+m_viewFrustum[i][1] *
								  ( y+size )+m_viewFrustum[i][2] * 
								  ( z+size )+m_viewFrustum[i][3] > 0 )
			continue;

		return false;
	}
	
	return true;
}

//-
//test a sphere for inclusion in the frustum
bool Camera::SphereInFrustum( float x, float y, float z, float radius ) {
	int i;

	for( i=0; i < 6; i++ ) {
		if( m_viewFrustum[i][0]*x + 
			m_viewFrustum[i][1]*y + 
			m_viewFrustum[i][2]*z + 
			m_viewFrustum[i][3] < -radius )
			return false;
	}

	return true;
}

