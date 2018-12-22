/* Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001"
 */


#include "pofLL.h"

const unsigned int POF_HEADER    = pofChunkType(POF_HEADER_TXT);
const unsigned int POF_END       = pofChunkType(POF_END_TXT);
const unsigned int POF_MATERIAL  = pofChunkType(POF_MATERIAL_TXT);
const unsigned int POF_VERTEX    = pofChunkType(POF_VERTEX_TXT);
const unsigned int POF_NORMAL    = pofChunkType(POF_NORMAL_TXT);
const unsigned int POF_TEX_COORD = pofChunkType(POF_TEX_COORD_TXT);
const unsigned int POF_COLOR     = pofChunkType(POF_COLOR_TXT);
const unsigned int POF_TEXT      = pofChunkType(POF_TEXT_TXT);
const unsigned int POF_FACE_SET  = pofChunkType(POF_FACE_SET_TXT);