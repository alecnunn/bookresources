// +-----------------------------------------------------------------------
// |
// |                         File Name: GPG5_gpCamera2D.h
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPG5_gpCamera2D Class Header.
// | 
// +-----------------------------------------------------------------------

#ifndef GPG5_GPCAMERA2D_H
#define GPG5_GPCAMERA2D_H
#pragma once

// +---------------------------
// |	Includes
// +---------------------------
#include<GPG5_gpNode.h>

// +-----------------------------------------------------------------------
// |
// |  Class       : GPG5_gpCamera2D
// |  Description : 
// |								This class is a mouse and keyboard 2D Camera
// |
// +-----------------------------------------------------------------------
class GPG5_gpCamera2D: public GPG5_gpNode
{
	public:
		// +---------------------------
		// |	Constructor & Destructor
		// +---------------------------
		GPG5_gpCamera2D();
		virtual ~GPG5_gpCamera2D();

		// +---------------------------
		// |	Position
		// +---------------------------
		void Situate(const GPG5_gpVector& position);
		virtual void SetPosition(const GPG5_gpVector& position);

		void SetEye(const GPG5_gpVector& eye);
		const GPG5_gpVector& GetEye() const;
		
		void SetUp(const GPG5_gpVector& up);
		const GPG5_gpVector& GetUp() const;

		// +---------------------------
		// |	Control
		// +---------------------------
		virtual void Move(float speed);
		virtual void Strafe(float speed);
		virtual void Zoom(float speed);
		virtual void Move(const GPG5_gpVector& dir, float speed);

		// +--------------------------------------------------
		// |	Virtual Methods of GPG5_gpNode
		// +--------------------------------------------------
		virtual void ProcessNode();
		virtual void Process();
		virtual bool ProcessEvent(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	protected:
		// +---------------------------
		// |	Position
		// +---------------------------
		virtual void Update();

		GPG5_gpVector eye_;
		GPG5_gpVector up_;
		GPG5_gpVector view_;
		GPG5_gpVector right_;

		// +---------------------------
		// |	Control
		// +---------------------------
		float translationSpeed_;
		float zoomSpeed_;
		double distance_;

		int deltaX_;
		int deltaY_;
		
		int oldXMouse_;
		int oldYMouse_;

		bool leftPressed_;
		bool middlePressed_;
		bool rightPressed_;

	private:
		
};

#endif //GPG5_GPCAMERA2D_H