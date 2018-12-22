#include "sgx/core/core.hpp"
#include "sgx/filesystem/filesystem.hpp"
#include "drivers/wintel32/wintel32core.hpp"
#include "drivers/wintel32/filesystem/win32disc.hpp"

//
// Setup
//
CSGXDeviceBlockWin32Disc::CSGXDeviceBlockWin32Disc(const sgxString &filename) : CSGXDeviceBlock()
{
    m_Filename = filename;
}

