/*********************************************************************NVMH2****
Path:  C:\Dev\devrel\Nv_sdk_4\CommonSrc\SceneManagement
File:  nvFrameBase.cpp

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/

#include "stdafx.h"

#include "nvFrameBase.h"

// nvFrameBase.cpp: implementation of the nvFrame class.
//
//////////////////////////////////////////////////////////////////////
/*
#ifndef _NVGLOBAL_H
#include "nvGlobal.h"
#endif // _NVGLOBAL_H

#ifndef _NVRENDERUTIL_H
#include "nvRenderUtil.h"
#endif // _NVRENDERUTIL_H

#ifndef _NVMESH_H
#include "nvMesh.h"
#endif // _NVMESH_H

*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

nvFrameBase::nvFrameBase( nvFrameBase * const pParent)
   : m_pParent(pParent)
{
	// remember pParent & add to parent's
	//  collection of children

  m_Mat.SetIdentity();
  if (pParent)
    pParent->AddFrame(this);
}

nvFrameBase::~nvFrameBase()
{

	// Remove self from parent's list
	if( m_pParent )
	{
		m_pParent->RemoveFrame( this );
	}

	// Remove the child frames
	FrameIt frameit = m_FrameList.begin();
	while (frameit != m_FrameList.end())
	{
		delete (*frameit);
		++frameit;
	}
}


void nvFrameBase::AddFrame(nvFrameBase * pFrame)
{
	// set pFrame's parent too!
	_ASSERT( pFrame );

	pFrame->SetParent( this );
	m_FrameList.push_back(pFrame);
}

void nvFrameBase::RemoveFrame( nvFrameBase * pFrame )
{
	// remove frame from child list

	FrameIt frameit = m_FrameList.begin();
	while (frameit != m_FrameList.end())
	{
		if( *frameit == pFrame )
		{
			m_FrameList.erase( frameit );
		}
	}
}

void	nvFrameBase::SetXForm( const Mat4 & mat )
{
	m_Mat = mat;
}

void nvFrameBase::SetPosition(const Vec3 & pos)
{
	m_Mat.SetTranslation(pos);
}

void nvFrameBase::GetPosition(Vec3 & v)
{
	m_Mat.GetTranslation(v);
}

nvFrameBase * nvFrameBase::GetParent() const
{
	return m_pParent;
}

void nvFrameBase::SetParent( nvFrameBase * pParent )
{
	_ASSERT( pParent );
	m_pParent = pParent;
}

Mat4 & nvFrameBase::GetXForm()
{
	return m_Mat;
}

Mat4 & nvFrameBase::GetWorldXForm()
{
	return m_WorldMat;
}

void nvFrameBase::UpdateWorldXForm()
{
	// update from root node down through the tree.

  if (m_pParent)
    m_WorldMat = m_pParent->GetWorldXForm() * m_Mat;
  else
    m_WorldMat = m_Mat;

  FrameIt frameit = m_FrameList.begin();
  while (frameit != m_FrameList.end())
  {
    (*frameit)->UpdateWorldXForm();
    ++frameit;
  }
}


