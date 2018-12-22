#include "sgx/core/core.hpp"
#include "sgx/filesystem/filesystem.hpp"

CSGXFile& operator<<(CSGXFile&File, sgxVector3 &v)
{
    File << v.x;
    File << v.y;
    File << v.z;
    return File;
}

CSGXFile& operator>>(CSGXFile&File, sgxVector3 &v)
{
    File >> v.x;
    File >> v.y;
    File >> v.z;
    return File;
}
