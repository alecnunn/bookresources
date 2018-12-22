// +-----------------------------------------------------------------------
// |
// |                         File Name: GPG5_gpNodeAxis.h
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPG5_gpNodeAxis Class Header.
// | 
// +-----------------------------------------------------------------------

#ifndef GPG5_GPNODEAXIS_H
#define GPG5_GPNODEAXIS_H
#pragma once

// +---------------------------
// |	Includes
// +---------------------------
#include<GPG5_gpNode.h>

// +-----------------------------------------------------------------------
// |
// |  Class       : GPG5_gpNodeAxis
// |  Description : 
// |								This class draws and manages a Cartesian 3D Axis
// |
// +-----------------------------------------------------------------------
class GPG5_gpNodeAxis : public GPG5_gpNode
{
	public:
		// +---------------------------
		// |	Constructor & Destructor
		// +---------------------------
		GPG5_gpNodeAxis();
		virtual ~GPG5_gpNodeAxis();

		// +---------------------------
		// |	Axis Lengths
		// +---------------------------
		void SetLengths(const GPG5_gpVector& lengths);
		const GPG5_gpVector& GetLenghts() const;

		// +---------------------------
		// |	Axis Colors
		// +---------------------------
		void SetAxisColor(const GPG5_gpVector& xColor, const GPG5_gpVector& yColor, const GPG5_gpVector& zColor);
		void GetAxisColor(GPG5_gpVector& xColor, GPG5_gpVector& yColor, GPG5_gpVector& zColor) const;

		// +--------------------------------------------------
		// |	Virtual Methods of GPG5_gpNode
		// +--------------------------------------------------
		virtual void Process();

	protected:
		// +---------------------------
		// |	Axis Lengths
		// +---------------------------
		GPG5_gpVector lengths_;

		// +---------------------------
		// |	Axis Colors
		// +---------------------------
		GPG5_gpVector xColor_;
		GPG5_gpVector yColor_;
		GPG5_gpVector zColor_;

	private:
};

#endif //GPG5_GPNODEAXIS_H