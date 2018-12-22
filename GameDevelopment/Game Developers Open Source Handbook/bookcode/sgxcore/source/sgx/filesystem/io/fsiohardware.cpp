#include "sgx/core/core.hpp"
#include "sgx/filesystem/filesystem.hpp"

//
// Overload the type so we can save the enum as integers
//
CSGXFile& 
operator>>(CSGXFile&File, CHardwarePlatform::tPlatform &v)
{
    tUINT32 as_int;
    File >> as_int;
    v = (CHardwarePlatform::tPlatform)as_int;

    return File;
}

CSGXFile& 
operator<<(CSGXFile&File, CHardwarePlatform::tPlatform &v)
{
    tUINT32 as_int = (tUINT32)v;
    File << as_int;

    return File;
}

