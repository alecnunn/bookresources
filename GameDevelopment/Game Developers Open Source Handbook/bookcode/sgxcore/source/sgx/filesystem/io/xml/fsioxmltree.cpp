#include "sgx/core/core.hpp"
#include "sgx/filesystem/filesystem.hpp"
#include "sgx/filesystem/io/xml/xml.hpp"

CSGXFile& 
operator<<(CSGXFile&File, const sgxXMLTree &tree)
{
	File << tree.GetRootNode();
	return File;
}
