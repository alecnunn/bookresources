/*********************************************************************NVMH2****
Path:  C:\Dev\devrel\Nv_sdk_4\CommonSrc\SceneManagement
File:  nvFrameBase.h

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
	Comments:
	
	  Base class for tree of orientations (via Mat4 transformations)
	  
		
******************************************************************************/


#if !defined(_SCENEMANAGE_NVFRAME_BASE_H)
#define _SCENEMANAGE_NVFRAME_BASE_H

#if _MSC_VER > 1000
#pragma once
#endif

#include "ul.h"
#include <vector>




class nvFrameBase
{
public:
	// STL typedefs...
	typedef std::vector<nvFrameBase*>    FrameList;
	typedef FrameList::iterator FrameIt;
	
	
	nvFrameBase( nvFrameBase * const pParent = NULL );
	virtual ~nvFrameBase();
	

	void			AddFrame(nvFrameBase * pFrame);
	void			RemoveFrame( nvFrameBase * pFrame );	// remove from child list

	nvFrameBase *	GetParent() const;
	void			SetParent( nvFrameBase * pFrame );

	Mat4 &      GetXForm();			// get this frame's transform
	Mat4 &      GetWorldXForm();	// get m_WorldMat

	void		SetXForm( const Mat4 & mat );

	void        SetPosition(const Vec3 & pos);  // Set linear translation
	void        GetPosition(Vec3 & v);			  // Get linear translation
	
	void        UpdateWorldXForm();	// accumulate back up through tree

	
private:
	nvFrameBase *   m_pParent;
	
	// Frame children
	FrameList   m_FrameList;
	
	Mat4        m_Mat;
	Mat4        m_WorldMat;
};

#endif // !defined(_SCENEMANAGE_NVFRAME_BASE_H)
