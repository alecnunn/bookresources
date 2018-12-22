#ifndef SGX_FILESYSTEM_IO_FSIOXMLNODE_HPP
#define SGX_FILESYSTEM_IO_FSIOXMLNODE_HPP	1

CSGXFile& operator<<(CSGXFile&File, const sgxXMLNode *pNode);
// NOTE: There's no input XML node, since that's currently
// all covered by the parser code. However, there's no reason
// it can not be provided.

#endif /* SGX_FILESYSTEM_IO_FSIOXMLNODE_HPP */
