#ifndef SGX_FILESYSTEM_BASEIO_HPP
#define SGX_FILESYSTEM_BASEIO_HPP	1

// Read data
CSGXFile& operator>>(CSGXFile&, tUINT32 &val);
CSGXFile& operator>>(CSGXFile&, tINT32 &val);
CSGXFile& operator>>(CSGXFile&, tUINT16 &val);
CSGXFile& operator>>(CSGXFile&, tINT16 &val);
CSGXFile& operator>>(CSGXFile&, tUINT8 &val);
CSGXFile& operator>>(CSGXFile&, tINT8 &val);
CSGXFile& operator>>(CSGXFile&, float &val);
CSGXFile& operator>>(CSGXFile&, sgxString &str);


// Write data
CSGXFile& operator<<(CSGXFile&, tUINT32 &val);
CSGXFile& operator<<(CSGXFile&, tINT32 &val);
CSGXFile& operator<<(CSGXFile&, tUINT16 &val);
CSGXFile& operator<<(CSGXFile&, tINT16 &val);
CSGXFile& operator<<(CSGXFile&, tUINT8 &val);
CSGXFile& operator<<(CSGXFile&, tINT8 &val);
CSGXFile& operator<<(CSGXFile&, float &val);
CSGXFile& operator<<(CSGXFile&, const sgxString &str);

#endif /* SGX_FILESYSTEM_BASEIO_HPP */
