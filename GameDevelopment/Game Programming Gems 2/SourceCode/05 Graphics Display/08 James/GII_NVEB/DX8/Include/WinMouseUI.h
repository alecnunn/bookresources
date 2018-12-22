/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
#ifndef _MOUSEUI_H_
#define _MOUSEUI_H_

#include <d3dx8.h>

/////////////
// MouseUI.h
//
// By Sim Dietrich & Cem Cebenoyan
//
// What it does :
//
// This UI object supports simple rotation through dragging with the left mouse button held down.
// It also has virtual methods for left button down & up, which could be overriden by a derived
// GUI object that handled buttons or something.
// 
// Usage:
//
// Create an instance of the MouseUI object, and pass in the Rect of the window you want it to refer to.
// I originally had it use the HWND of the window, but it occurred to me that we really want to have
// the ability to do sub-areas, for instance if you had a 3D gui, and you only wanted the smaller viewport
// to respond to mouse messages.
//
// If the window resizes, you should call SetRECT() with the new rect size.
//
// One other nice thing about this class is that you don't have to check if the mouse x & y is in the rect.
// The class will always check for you on all button/mouse move messages.
//
// Just call MouseMove when appropriate and the MouseUI will update its internal RotationMatrix.  It doesn't
// orthonormalize the matrix, so it may go wacky eventually.
//
// Then call GetRotationMatrix().  You can use the matrix to rotate around the object, the view or the world as appropriate.
// Then call GetTranslationMatrix().  You can use the matrix to translate the object, the view or the world as appropriate.
//
// To get the rotation & translation matrices back to identity, call Reset()
//
// You can also get/set the sensitivity of the rotations through the Get/SetSensitivityFactor() calls.
// The SensitivityFactor defaults to 1.0f
//

class MouseUI
{
	protected :

		DWORD mFlags;

		unsigned int mStartX;
		unsigned int mStartY;

		unsigned int mWidth;
		unsigned int mHeight;

		D3DXMATRIX mRotationMatrix;
		D3DXMATRIX mTranslationMatrix;

		float mSensitivityFactor;

		RECT mRect;

		void SetLocation( const unsigned int& x, const unsigned int& y )
		{
			mStartX = max( mRect.left, x );
			mStartX = min( mRect.right, mStartX );

			mStartY = max( mRect.top, y );
			mStartY = min( mRect.bottom, mStartY );
		}

		void SetSize()
		{
			mWidth =  ( mRect.right - mRect.left );
			mHeight = ( mRect.bottom - mRect.top );
		}

	public :

		MouseUI::MouseUI( const RECT& theRect ) : mRect( theRect )
		{
			Reset();
		}

		RECT GetRECT() const { return mRect; }

		void SetRECT( const RECT& rhs ) 
		{
			mRect = rhs; 
			SetSize();
		}

		void GetRotationMatrix( D3DXMATRIX* pMatrix ) const { (*pMatrix) = mRotationMatrix; }

		D3DXMATRIX GetRotationMatrix() const { return mRotationMatrix; }

		void GetTranslationMatrix( D3DXMATRIX* pMatrix ) const { (*pMatrix) = mTranslationMatrix; }

		D3DXMATRIX GetTranslationMatrix() const { return mTranslationMatrix; }



		bool IsMouseDown() const { return ( ( mFlags & MK_LBUTTON ) != 0 ); }

		void Reset()
		{
			SetSize();
			mSensitivityFactor = 1.0f;
			mFlags = 0;
			SetLocation( 0, 0 );
			D3DXMatrixIdentity( &mRotationMatrix );
			D3DXMatrixIdentity( &mTranslationMatrix );
		}

		bool IsInWindow( const unsigned int& x, const unsigned int& y )
		{
			SetSize();

			return ( ( x >= mRect.left ) && ( x < mRect.right ) &&
				     ( y >= mRect.top  ) && ( y < mRect.bottom ) );
		}

		float GetSensitivityFactor() const { return mSensitivityFactor; }

		void  SetSensitivityFactor( const float& rhs ) { mSensitivityFactor = rhs; }

		// accepts window coordinates
		virtual void OnLButtonDown( const unsigned int& x, const unsigned int& y, const DWORD& theFlags )
		{
			if ( IsInWindow( x, y ) )
			{
				SetLocation( x, y );
				mFlags = theFlags;
			}
			else
			{
				OnLButtonUp( x, y, theFlags );
			}
		}

		virtual void OnLButtonUp( const unsigned int& x, const unsigned int& y, const DWORD& theFlags )
		{
			SetLocation( x, y );
			mFlags = theFlags;
		}

		virtual void Translate( const float& X, const float& Y, const float& Z )
		{
			D3DXMATRIX mov;
			D3DXMatrixTranslation( &mov, X, Y, Z );
			D3DXMatrixMultiply( &mTranslationMatrix, &mTranslationMatrix, &mov );
		}

		virtual void OnMouseMove( const unsigned int& x, const unsigned int& y, const DWORD& theFlags )
		{
			mFlags = theFlags;

			if ( !IsInWindow( x, y ) )
			{
				OnLButtonUp( x, y, theFlags );
			}
			else
			if ( IsMouseDown() )
			{
				float deltax = ( (float)x - (float)mStartX );
				float deltay = ( (float)y - (float)mStartY );

				if ( !( ( deltax == 0.0f ) && ( deltay == 0.0f ) ) )
				{
					float mag = sqrt( deltax * deltax + deltay * deltay );

					float dx = ( deltax / mag );
					float dy = ( deltay / mag );

					//find a vector in the plane perpendicular to delta vector
					float perpx = dy;
					float perpy = dx;

					D3DXMATRIX temp;
					//rotate around this vector
					D3DXVECTOR3 axis( perpx, perpy, 0.0f );
					D3DXMatrixRotationAxis( &temp, &axis, D3DXToRadian( -mSensitivityFactor * mag ) );

					D3DXMatrixMultiply( &mRotationMatrix, &mRotationMatrix, &temp );
				}
				SetLocation( x, y );
			}
		}

};



#endif  _MOUSEUI_H_


