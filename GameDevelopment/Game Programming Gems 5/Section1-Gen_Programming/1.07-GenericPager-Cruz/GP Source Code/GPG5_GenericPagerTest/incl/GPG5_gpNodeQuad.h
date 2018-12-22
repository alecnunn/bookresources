// +-----------------------------------------------------------------------
// |
// |                         File Name: GPG5_gpNodeQuad.h
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPG5_gpNodeQuad Class Header.
// | 
// +-----------------------------------------------------------------------

#ifndef GPG5_GPNODEQUAD_H
#define GPG5_GPNODEQUAD_H
#pragma once

// +---------------------------
// |	Includes
// +---------------------------
#include <GPG5_gpNode.h>

// +---------------------------
// |	Forward Declarations
// +---------------------------
class GPG5_gpNodeTexture;

// +-----------------------------------------------------------------------
// |
// |  Class       : GPG5_gpNodeQuad
// |  Description : 
// |								This class draws and manages a Quad
// |
// +-----------------------------------------------------------------------
class GPG5_gpNodeQuad : public GPG5_gpNode
{
	public:
		// +---------------------------
		// |	Constructor & Destructor
		// +---------------------------
		GPG5_gpNodeQuad();
		virtual ~GPG5_gpNodeQuad();

		// +---------------------------
		// |	Size
		// +---------------------------
		void SetSize( double width, double height );
		void GetSize( double& width, double& height ) const;

		void SetCorners( const GPG5_gpVector& cornerNW, const GPG5_gpVector& cornerNE, const GPG5_gpVector& cornerSE,	const GPG5_gpVector& cornerSW );
		void GetCorners( GPG5_gpVector& cornerNW, GPG5_gpVector& cornerNE, GPG5_gpVector& cornerSE,	GPG5_gpVector& cornerSW ) const;

		// +---------------------------
		// |	Line Width
		// +---------------------------
		void SetLineWidth(float lineWidth);
		float GetLineWidth() const;

		// +---------------------------
		// |	Fill Mode
		// +---------------------------
		void SetFillMode(unsigned int mode);
		unsigned int GetFillMode() const;

		// +---------------------------
		// |	Face Mode
		// |	Front					 : GL_FRONT
		// |	Back					 : GL_BACK 
		// |	Front And Back : GL_FRONT_AND_BACK (default)
		// +---------------------------
		void SetFaceMode(unsigned int faceMode);
		unsigned int GetFaceMode() const;

		// +---------------------------
		// |	Texture
		// +---------------------------
		void SetTexture( GPG5_gpNodeTexture* pTexture );
		GPG5_gpNodeTexture* GetTexture() const;
		void EnableTexture(bool isEnabledTexture);
		bool IsEnabledTexture() const;

		// +--------------------------------------------------
		// |	Virtual Methods of GPG5_gpNode
		// +--------------------------------------------------
		virtual void Process();

	protected:

		// +---------------------------
		// |	Size
		// +---------------------------
		GPG5_gpVector cornerNW_;
		GPG5_gpVector cornerNE_;
		GPG5_gpVector cornerSE_;
		GPG5_gpVector cornerSW_;

		// +---------------------------
		// |	Line Width
		// +---------------------------
		float lineWidth_;

		// +---------------------------
		// |	Fill Mode
		// +---------------------------
		unsigned int fillMode_;

		// +---------------------------
		// |	Face Mode
		// +---------------------------
		unsigned int faceMode_;

		// +---------------------------
		// |	Texture
		// +---------------------------
		GPG5_gpNodeTexture* pTexture_;
		bool isEnabledTexture_;
	
	private:

};

#endif //GPG5_GPNODEQUAD_H