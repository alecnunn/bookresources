#ifndef SGX_FILESYSTEM_IO_MATH_HPP
#define SGX_FILESYSTEM_IO_MATH_HPP

CSGXFile& operator<<(CSGXFile&File, sgxVector3 &v);
CSGXFile& operator>>(CSGXFile&File, sgxVector3 &v);

#endif /* SGX_FILESYSTEM_IO_MATH_HPP */
