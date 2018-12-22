// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// serdefs.cpp

#include "serdefs.h"
#include "misc.h"

// check that tags do not overflow a uint8

COMPILE_ASSERT( SerializeTagCheck, eTagLast_ <= 256 );
