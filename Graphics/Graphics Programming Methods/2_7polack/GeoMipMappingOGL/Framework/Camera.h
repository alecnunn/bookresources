//==============================================================
//= Camera.h
//= Original coders: Trent Polack (trent@codershq.com)
//==============================================================
//= This file contains the code routines for a simple camera.
//==============================================================
#ifndef __DemoFramework_Camera_H__
#define __DemoFramework_Camera_H__

#include "..\\MathLib\\Vector.h"

//simple 3D camera class
namespace DemoFramework {
	class Camera {
		public:
			MathLib::Vector m_eyePos;			//position of camera
			MathLib::Vector m_lookAt;			//lookat vector

			//up, forward, right vectors
			MathLib::Vector m_up;
			MathLib::Vector m_forward;
			MathLib::Vector m_side;

			//the camera's yaw and pitch
			float m_yaw;
			float m_pitch;

			float m_viewFrustum[6][4];

		//-
		//set the camera's eye position
		inline void SetPosition( float x, float y, float z ) {
			m_eyePos.Set( x, y, z );
		}

		void ComputeViewMatrix( float timeDelta= 1.0f );
		void SetViewMatrix( void );
		void ComputeViewFrustum( void );

		bool VertexInFrustum( float x, float y, float z );
		bool CubeInFrustum( float x, float y, float z, float size );
		bool SphereInFrustum( float x, float y, float z, float radius );

	Camera( void ) {
		m_up.Set( 0.0f, 1.0f, 0.0f );
		m_side.Set( 1.0f, 0.0f, 0.0f );
	}

	~Camera( void ) { }
	};
}

#endif	//__DemoFramework_Camera_H__
