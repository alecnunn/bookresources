/* Copyright (C) Mason McCuskey, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Mason McCuskey, 2000"
 */
// 3DSprite.h: interface for the C3DSprite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_3DSPRITE_H__A99E5461_FDDA_11D3_BC41_444553546170__INCLUDED_)
#define AFX_3DSPRITE_H__A99E5461_FDDA_11D3_BC41_444553546170__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class C3DSprite  
{
public:
	C3DSprite();
	virtual ~C3DSprite();

  void Init();
	void Display();
  void SetPos(int x, int y) { m_iX = x; m_iY = y; }
  void SetSize(int width, int height) { m_iWidth = width; m_iHeight = height; }

  int GetX(void) { return(m_iX); }
  int GetY(void) { return(m_iY); }
  void SetX(int x) { m_iX = x; }
  void SetY(int y) { m_iY = y; }

  int GetWidth(void) { return(m_iWidth); }
  int GetHeight(void) { return(m_iHeight); }
  void SetWidth(int Width) { m_iWidth = Width; }
  void SetHeight(int Height) { m_iHeight = Height; }
  void SetRotation(float angle) { m_fRotation = angle; }

  float GetRotation(void) { return(m_fRotation); }

  int GetAlpha(void) { return(m_iAlpha); }
  void SetAlpha(int a) { m_iAlpha = a; }

private:
	float	m_fRotation;
	unsigned int mTextureID;
  
  int m_iX;
  int m_iY;
  
  int m_iWidth;
  int m_iHeight;
  int m_iAlpha;
};

#endif // !defined(AFX_3DSPRITE_H__A99E5461_FDDA_11D3_BC41_444553546170__INCLUDED_)
