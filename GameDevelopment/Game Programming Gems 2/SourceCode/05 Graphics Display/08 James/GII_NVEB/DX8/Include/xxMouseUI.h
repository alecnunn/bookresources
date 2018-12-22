/*********************************************************************NVMH2****
File:  MouseUI.h

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:

This is a modified version of Sim's original MouseUI.h.
This one requires you call LButtonDown() and LButtonUp(), but you no longer
 have to pass in the flags parameter.

******************************************************************************/

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
static const float MOUSEUI_TRANSLATION_SCALE = 0.05f;
static const float MOUSEUI_ROTATION_SCALE = 1.0f;

class MouseUI
{
protected :
	
    bool m_bLButtonDown;
    bool m_bUseCameraControl;
	
	unsigned int mStartX;
	unsigned int mStartY;
	
	unsigned int mWidth;
	unsigned int mHeight;
	
	D3DXMATRIX mRotationMatrix;
	D3DXMATRIX mTranslationMatrix;
	
	float mRotationalSensitivityFactor;
	float mTranslationalSensitivityFactor;
	
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
	
	MouseUI::MouseUI( const RECT& theRect, bool bCameraControl = false ) 
            : mRect( theRect )
            , m_bUseCameraControl( bCameraControl )
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
	
	
	
	bool IsMouseDown() const { return ( m_bLButtonDown ); }
	
	void Reset()
	{
		SetSize();
		mRotationalSensitivityFactor = 1.0f * MOUSEUI_ROTATION_SCALE;
		mTranslationalSensitivityFactor = 1.0f * MOUSEUI_TRANSLATION_SCALE;
		m_bLButtonDown = false;
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
	
	float GetTranslationalSensitivityFactor() const { return mTranslationalSensitivityFactor / MOUSEUI_TRANSLATION_SCALE; }
	
	void  SetTranslationalSensitivityFactor( const float& rhs ) { mTranslationalSensitivityFactor = rhs * MOUSEUI_TRANSLATION_SCALE; }

	float GetRotationalSensitivityFactor() const { return mRotationalSensitivityFactor / MOUSEUI_ROTATION_SCALE; }
	
	void  SetRotationalSensitivityFactor( const float& rhs ) { mRotationalSensitivityFactor = rhs * MOUSEUI_ROTATION_SCALE; }
	
	// accepts window coordinates
	virtual void OnLButtonDown( const unsigned int& x, const unsigned int& y )
	{
		if ( IsInWindow( x, y ) )
		{
			SetLocation( x, y );
			m_bLButtonDown = true;
		}
		else
		{
			OnLButtonUp( x, y );
		}
	}
	
	virtual void OnLButtonUp( const unsigned int& x, const unsigned int& y )
	{
		SetLocation( x, y );
		m_bLButtonDown = false;
	}
	
	virtual void Translate( const float& X, const float& Y, const float& Z )
	{
		D3DXMATRIX mov;
		D3DXMatrixTranslation( &mov, X, Y, Z );
		D3DXMatrixMultiply( &mTranslationMatrix, &mTranslationMatrix, &mov );
	}
	
	virtual void Translate( D3DXVECTOR3 const &t)
	{
		D3DXMATRIX mov;
		D3DXMatrixTranslation( &mov, t.x, t.y, t.z);
		D3DXMatrixMultiply( &mTranslationMatrix, &mTranslationMatrix, &mov );
	}

    virtual void OnMouseMove( const unsigned int& x, const unsigned int& y )
	{
		if ( !IsInWindow( x, y ) )
		{
			OnLButtonUp( x, y );
		}
		else if ( IsMouseDown() )
		{ 
			bool bShift = ( ( GetAsyncKeyState( VK_LSHIFT )   < 0 ) || ( GetAsyncKeyState( VK_SHIFT    ) < 0 ) || ( GetAsyncKeyState( VK_RSHIFT )  < 0 ) );
			bool bAlt =   ( ( GetAsyncKeyState( VK_LMENU )    < 0 ) || ( GetAsyncKeyState( VK_RMENU    ) < 0 ) || ( GetAsyncKeyState( VK_MENU )    < 0 ) );
			bool bCtl =   ( ( GetAsyncKeyState( VK_LCONTROL ) < 0 ) || ( GetAsyncKeyState( VK_RCONTROL ) < 0 ) || ( GetAsyncKeyState( VK_CONTROL ) < 0 ) );

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
				
				D3DXMATRIX temp, matLookRotations;
				//rotate around this vector
				D3DXVECTOR3 axis( perpx, perpy, 0.0f );
				D3DXMatrixRotationAxis( &temp, &axis, D3DXToRadian( -mRotationalSensitivityFactor * mag ) );
				
                if (m_bUseCameraControl)
                {
                    D3DXMatrixTranspose(&matLookRotations, &mRotationMatrix);
                    D3DXVECTOR3 camLookDirection( 0.0f, 0.0f, 1.0f );
                    D3DXVec3TransformNormal(&camLookDirection, &camLookDirection, &matLookRotations);

                    D3DXVECTOR3 const   yVector(0.0f, 1.0f, 0.0f);
                    float       const   h = D3DXVec3Dot(&yVector, &camLookDirection);
                    D3DXVECTOR3         xzProjectedLookDirection(camLookDirection - h*yVector);

                    D3DXVec3Normalize(&xzProjectedLookDirection, &xzProjectedLookDirection);
    
                    D3DXVECTOR3         rightVector;         
                    D3DXVec3Cross(&rightVector, &yVector, &xzProjectedLookDirection);

                    D3DXVECTOR3         transVector;         

                    if ( bShift )
			        {
                        transVector  =  deltax * mTranslationalSensitivityFactor * rightVector;
                        transVector += -deltay * mTranslationalSensitivityFactor * yVector;

				        Translate( transVector );
			        }
			        else if ( bCtl )
			        {
                        transVector  =  deltay * mTranslationalSensitivityFactor * xzProjectedLookDirection;
                        Translate( transVector );
			        }
			        else
			        {
				        D3DXMatrixMultiply( &mRotationMatrix, &mRotationMatrix, &temp );
			        }
                }
                else
                {
                    if ( bShift )
				    {
					    Translate( deltax * mTranslationalSensitivityFactor, -deltay * mTranslationalSensitivityFactor, 0.0f );
				    }
				    else if ( bCtl )
				    {
					    Translate( 0.0f, 0.0f , deltay * mTranslationalSensitivityFactor );
				    }
				    else
				    {
					    D3DXMatrixMultiply( &mRotationMatrix, &mRotationMatrix, &temp );
				    }
                }
			}
			SetLocation( x, y );
		}
	}
};



#endif  _MOUSEUI_H_


