/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/
#ifndef __NVFILE_H
#define __NVFILE_H

#include "nvinc.h"
#include "nvmesh.h"
#include "nvframe.h"

#pragma comment(lib, "d3dxof.lib")
#pragma comment(lib, "dxguid.lib")

namespace nv_objects
{


class NVFile : public NVFrame
{
	HRESULT LoadMesh( NVDevice* pDevice, LPDIRECTXFILEDATA pFileData, 
					  NVFrame* pParentFrame );
	HRESULT LoadFrame( NVDevice* pDevice, LPDIRECTXFILEDATA pFileData, 
		               NVFrame* pParentFrame );
public:

	void GetBoundingInfo(NVBounds* pBounds);
	HRESULT Create( NVDevice* pDevice, const std::string& strFilename );
	HRESULT Render( NVDevice* pDevice );

	NVFile() : NVFrame( _T("NVFile_Root") ) {}

	std::string m_strFilePath;
};




}; // nv_objects

#endif



