// +-----------------------------------------------------------------------
// |
// |                         File Name: GPG5_gpNode.h
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPG5_gpNode Class Header.
// | 
// +-----------------------------------------------------------------------

#ifndef GPG5_GPNODE_H
#define GPG5_GPNODE_H
#pragma once

// +---------------------------
// |	Includes
// +---------------------------
#include <GPG5_gpNode.types.h>

// +-----------------------------------------------------------------------
// |
// |  Class       : GPG5_gpNode
// |  Description : 
// |								This is the base class used in all the GPG5 Test Engine
// |								that has been designed and implemented for testing the
// |								Generic Pager.
// |	
// |								This class provides some methods to create and control
// |								any kind of OpenGL object in avery easy way.
// |								A derived class only has to implement the method:
// |											virtual void Process() = 0;
// |								and put here all the draw code ( or any type of code).
// |								That derived does not have to take care about push and pop
// |								the view matrix or the attributs. This class do it in the 
// |								ProcessNode method.
// |
// +-----------------------------------------------------------------------
class GPG5_gpNode
{
	public:
		// +---------------------------
		// |	Constructor & Destructor
		// +---------------------------
		GPG5_gpNode();
		virtual ~GPG5_gpNode();

		// +---------------------------
		// |	Name
		// +---------------------------
		virtual void SetName(const std::string& name);
		const std::string& GetName() const;

		// +---------------------------
		// |	Position
		// +---------------------------
		virtual void SetPosition(const GPG5_gpVector& position);
		const GPG5_gpVector& GetPosition() const;

		// +---------------------------
		// |	Orientation
		// +---------------------------
		virtual void SetOrientation(const GPG5_gpVector& orientation);
		const GPG5_gpVector& GetOrientation() const;

		// +---------------------------
		// |	Color
		// +---------------------------
		virtual void SetColor(const GPG5_gpVector& color);
		const GPG5_gpVector& GetColor() const;
		
		// +--------------------------------------------------
		// |	Virtual Methods to Implement by a derived class
		// +--------------------------------------------------
		virtual void ProcessNode();
		virtual void Process() = 0;
		virtual bool ProcessEvent(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		
	protected:
		// +---------------------------
		// |	Name
		// +---------------------------
		std::string name_;

		// +---------------------------
		// |	Position
		// +---------------------------
		GPG5_gpVector position_;

		// +---------------------------
		// |	Orientation
		// +---------------------------
		GPG5_gpVector orientation_;

		// +---------------------------
		// |	Color
		// +---------------------------
		GPG5_gpVector color_;
};

#endif //GPG5_GPNODE_H