#include "sgx/core/core.hpp"
#include "sgx/filesystem/filesystem.hpp"
#include "sgx/filesystem/io/xml/xml.hpp"

tBOOL	
sgxXMLWriter::Write(const sgxString &filename)
{
	CSGXFile file(filename, "rw");

	if (file.IsValid()) {
		return Write(file);
	}
	return FALSE;
}

tBOOL	
sgxXMLWriter::Write(CSGXFile &file)
{
	if (!file.IsValid()) {
		return FALSE;
	}

	file << m_Tree;

	return TRUE;
}
