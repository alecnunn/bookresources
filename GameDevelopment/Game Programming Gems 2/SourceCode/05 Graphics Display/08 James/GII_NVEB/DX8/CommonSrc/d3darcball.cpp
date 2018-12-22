/*//////////////////////////////////////////////////////////////////////////////
//
// File: d3darcball.cpp
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
//
//////////////////////////////////////////////////////////////////////////////*/

/*
 * Simple arcball roration controll
 * uses luciforms quaternion stuff
 *
 * Peter-Pike Sloan
 */

#include <Windows.h>
#include <d3d8.h>
#include <d3dx8.h>
#include "d3dArcball.h"

// constructors - default makes most
// sense, SetWindow is what's important...

GXArcBall::GXArcBall()
{
	Reset();
}

GXArcBall::GXArcBall(const int& x, const int& y)
{
	m_vCenter = D3DXVECTOR3((float)x,(float)y,0.0f);
	Reset();
}

GXArcBall::GXArcBall(D3DXVECTOR3 &center)
{
	m_vCenter = center;
	Reset();
}

void GXArcBall::Reset()
{
    D3DXQuaternionIdentity(&m_qDown);
    D3DXQuaternionIdentity(&m_qNow);
	m_bDrag = false;
  m_fRadius = oldTestRadius;
}

void GXArcBall::SetWindow(int iWidth, int iHeight, float fRadius)
{
  // set win info
	m_iWidth = iWidth; 
  m_iHeight = iHeight; 
  m_fRadius = fRadius; 

  oldTestRadius = m_fRadius;

  m_fRadius = m_fRadius;


	// set center after this
	m_vCenter = D3DXVECTOR3(iWidth/2.0f,iHeight/2.0f,0.0f);
}

void GXArcBall::SetWindow(HWND hwnd, float fRadius)
{
    RECT r;

    GetClientRect( hwnd, &r );

    // setup the client rect size, used for frame rate info
    m_sizeClient.cx = r.right - r.left;
    m_sizeClient.cy = r.bottom - r.top;

    SetWindow(r.right, r.bottom, fRadius);
}

void GXArcBall::BeginDrag(const int& x, const int& y)
{
	m_bDrag = true;
	m_vDown = ScreenToVector(x,y);
}

void GXArcBall::EndDrag()
{
	m_bDrag = false;
	m_qDown = m_qNow;
}

void GXArcBall::Mouse(const int& x, const int& y)
{
	if (m_bDrag) { // ignore it if you aren't dragging
		m_vCur = ScreenToVector(x,y);
		Update(); // always build qNow_...
	}
}

void GXArcBall::GetMat(D3DXMATRIX *pmat)
{
    D3DXQUATERNION qConj;
    D3DXMatrixRotationQuaternion(pmat, D3DXQuaternionConjugate(&qConj, &m_qNow));
}

D3DXVECTOR3 GXArcBall::ScreenToVector(const int& px, const int& py)
{
	float x,y,z;

	x = (px - m_vCenter.x) / (m_iWidth * m_fRadius * 0.5f); // scales to screen...
#if 1
	y = (m_vCenter.y - py) / (m_iHeight * m_fRadius * 0.5f); // invert Y coordinate...
#else
	y = (py - m_vCenter.y) / (m_iHeight * m_fRadius * 0.5f); // invert Y coordinate...
#endif

	float mag = x*x + y*y;

	if (mag > 1.0) {
		float scale = 1.0f/sqrtf(mag);
		x *= scale;
		y *= scale;
		z = 0.0;
	} else {
		z = sqrtf((1.0f-mag));
	}

	return D3DXVECTOR3(x,y,z);
}

void GXArcBall::Update()
{
	if (m_bDrag) { // recompute m_qNow
		m_qNow = m_qDown * QuatFromBallPoints(m_vDown, m_vCur);
	}
}




D3DXQUATERNION QuatFromBallPoints(const D3DXVECTOR3 &vFrom, const D3DXVECTOR3 &vTo)
{
	D3DXVECTOR3 vPart;
	float     dotPart = D3DXVec3Dot(&vFrom, &vTo);

    D3DXVec3Cross(&vPart, &vFrom, &vTo);

	return D3DXQUATERNION(vPart.x, vPart.y, vPart.z, dotPart);
}
