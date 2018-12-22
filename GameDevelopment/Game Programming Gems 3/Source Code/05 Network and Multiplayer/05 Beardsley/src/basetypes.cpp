// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// basetypes.cpp

#include "basetypes.h"
#include "misc.h"

// Check sizes of the base types

COMPILE_ASSERT( Size_int8, sizeof(int8) == 1 );
COMPILE_ASSERT( Size_uint8, sizeof(uint8) == 1 );
COMPILE_ASSERT( Size_int16, sizeof(int16) == 2 );
COMPILE_ASSERT( Size_uint16, sizeof(uint16) == 2 );
COMPILE_ASSERT( Size_int32, sizeof(int32) == 4 );
COMPILE_ASSERT( Size_uint32, sizeof(uint32) == 4 );
COMPILE_ASSERT( Size_int64, sizeof(int64) == 8 );
COMPILE_ASSERT( Size_uint64, sizeof(uint64) == 8 );

COMPILE_ASSERT( Size_float32, sizeof(float32) == 4 );
COMPILE_ASSERT( Size_float64, sizeof(float64) == 8 );
