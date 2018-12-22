#include "sgx/core/core.hpp"
#include "sgx/filesystem/filesystem.hpp"

#define SGX_BUFFER_SIZE 256

namespace {
	tUCHAR8 ms_Buffer[SGX_BUFFER_SIZE];
};
    

//
// Overloaded File.Read routines
//

CSGXFile& operator>>(CSGXFile&File, tREAL32 &val)
{
    sgxAssert(File.IsLoading()); 

    File.Read((tBYTE *)&val, sizeof(val));

    if (File.SwapEndian()) {
        register tUINT32 t = *(tUINT32 *)&val;

        *(tUINT32 *)&val = sgxEndianSwap32(t);
    }

    return File;
}

CSGXFile& operator>>(CSGXFile&File, tUINT32 &val)
{
    sgxAssert(File.IsLoading()); 

    File.Read((tBYTE *)&val, sizeof(val));

    if (File.SwapEndian()) {
        val = sgxEndianSwap32(val);
    }

    return File;
}

CSGXFile& operator>>(CSGXFile&File, tINT32 &val)
{
    sgxAssert(File.IsLoading()); 

    File.Read((tBYTE *)&val, sizeof(val));

    if (File.SwapEndian()) {
        val = sgxEndianSwap32(val);
    }

    return File;
}

CSGXFile& operator>>(CSGXFile&File, tUINT16 &val)
{
    sgxAssert(File.IsLoading()); 

    File.Read((tBYTE *)&val, sizeof(val));

    if (File.SwapEndian()) {
        val = sgxEndianSwap16(val);
    }

    return File;
}

CSGXFile& operator>>(CSGXFile&File, tINT16 &val)
{
    sgxAssert(File.IsLoading()); 

    File.Read((tBYTE *)&val, sizeof(val));

    if (File.SwapEndian()) {
        val = sgxEndianSwap16(val);
    }

    return File;
}

CSGXFile& operator>>(CSGXFile&File, tUINT8 &val)
{
    sgxAssert(File.IsLoading()); 

    File.Read((tBYTE *)&val, sizeof(val));

    return File;
}

CSGXFile& operator>>(CSGXFile&File, tINT8 &val)
{
    sgxAssert(File.IsLoading()); 

    File.Read((tBYTE *)&val, sizeof(val));

    return File;
}

CSGXFile& operator>>(CSGXFile&File, sgxString &str)
{
    sgxAssert(File.IsLoading()); 
    tUINT32 bytesRead;
    tUINT32 length;

    File >> length;

	// NOTE: This temporary buffer is not thread-safe.
	// It relies on the file operations occuring in only
	// one thread.
    str = "";
    do
    {
        bytesRead = File.Read((tBYTE *)ms_Buffer, sgxMin(length, sizeof(ms_Buffer)));

        if (bytesRead) {
            str += sgxString((char *)ms_Buffer);
        }
    }
    while(sizeof(ms_Buffer) == bytesRead);

    return File;
}


//
// Overloaded File.Write routines
//
CSGXFile& operator<<(CSGXFile&File, tREAL32 &val)
{
    sgxAssert(File.IsSaving()); 

    tREAL32 swp = val;

    if (File.SwapEndian()) {
        register tUINT32 t = *(tUINT32 *)&swp;

        *(tUINT32 *)&swp = sgxEndianSwap32(t);
    }

    File.Write((tBYTE *)&swp, sizeof(swp));

    return File;
}

CSGXFile& operator<<(CSGXFile&File, tUINT32 &val)
{
    sgxAssert(File.IsSaving()); 

    register tUINT32 swp = val;

    if (File.SwapEndian()) {
        swp = sgxEndianSwap32(swp);
    }

    File.Write((tBYTE *)&swp, sizeof(swp));

    return File;
}

CSGXFile& operator<<(CSGXFile&File, tINT32 &val)
{
    sgxAssert(File.IsSaving()); 

    register tINT32 swp = val;

    if (File.SwapEndian()) {
        swp = sgxEndianSwap32(swp);
    }

    File.Write((tBYTE *)&swp, sizeof(swp));

    return File;
}

CSGXFile& operator<<(CSGXFile&File, tUINT16 &val)
{
    sgxAssert(File.IsSaving()); 

    register tUINT16 swp = val;

    if (File.SwapEndian()) {
        swp = sgxEndianSwap16(swp);
    }

    File.Write((tBYTE *)&swp, sizeof(swp));

    return File;
}

CSGXFile& operator<<(CSGXFile&File, tINT16 &val)
{
    sgxAssert(File.IsSaving()); 

    register tINT16 swp = val;

    if (File.SwapEndian()) {
        swp = sgxEndianSwap16(swp);
    }

    File.Write((tBYTE *)&swp, sizeof(swp));

    return File;
}

CSGXFile& operator<<(CSGXFile&File, tUINT8 &val)
{
    sgxAssert(File.IsSaving()); 

    File.Write((tBYTE *)&val, sizeof(val));

    return File;
}

CSGXFile& operator<<(CSGXFile&File, tINT8 &val)
{
    sgxAssert(File.IsSaving()); 

    File.Write((tBYTE *)&val, sizeof(val));

    return File;
}

CSGXFile& operator<<(CSGXFile&File, const sgxString &str)
{
    sgxAssert(File.IsSaving()); 
    tUINT32 length = str.length()+1;

    File << length;
    File.Write((tBYTE *)str.c_str(), length);

    return File;
}

