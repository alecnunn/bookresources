/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
       
        
******************************************************************************/

// c_constants contains useful digits, in this case zero
// r_texture0 may be used.
// Note that the linker will usually automatically remove these lines 
// as redundant code if it can, by inserting the initialisation the first time 
// it is referenced
#define ZERO c_constants.x
#define ONE c_constants.y
#define BUMP_SCALE c_constants.z

// We always want the position in v0 and any texture0 coordinate in v1.
// This is to ensure the gloss pass can map to the vertex stream.  It's also a nice
// way to show how you can mix and match fixed vs variable types.
#define FIXED_POSITION v0
#define FIXED_POSITION_LOCATION 0

#define FIXED_TEXTURE0 v1
#define FIXED_TEXTURE0_LOCATION 1
