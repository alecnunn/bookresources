/*********************************************************************NVMH2****
Path:  E:\devrel\NV_SDK_4\DX8\NVEffectsBrowser\Effects\Keyframe
File:  KeyframeCommon.h

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/

#ifndef KEYFRAME_COMMON_H
#define KEYFRAME_COMMON_H

#define PERIOD 400.0f
#define DISTANCE_BETWEEN_FRAMES 7
#define START_FRAME 0
#define FILENAME "mask.md2"
#define TEXTURE_FILENAME "mask.bmp"

struct KeyframeVertex
{
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
	D3DXVECTOR2 Texture;
};

struct Extents
{
	float minX, maxX;
	float minY, maxY;
	float minZ, maxZ;
};

#endif
