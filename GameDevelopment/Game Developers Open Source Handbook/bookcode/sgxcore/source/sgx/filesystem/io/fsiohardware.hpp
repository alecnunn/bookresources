#ifndef SGX_FILESYSTEM_IO_HARDWARE_HPP
#define SGX_FILESYSTEM_IO_HARDWARE_HPP

CSGXFile& operator>>(CSGXFile&File, CHardwarePlatform::tPlatform &v);
CSGXFile& operator<<(CSGXFile&File, CHardwarePlatform::tPlatform &v);

#endif /* SGX_FILESYSTEM_IO_HARDWARE_HPP */
