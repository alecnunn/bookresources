#include "sgx/core/core.hpp"
#include "sgx/filesystem/filesystem.hpp"

sgxXMLTree null_tree;

sgxXMLReader::sgxXMLReader() : m_Tree(null_tree)
{
}

sgxXMLReader::sgxXMLReader(sgxXMLTree &tree) : m_Tree(tree)
{
}

sgxXMLReader::~sgxXMLReader()
{
}

tBOOL sgxXMLReader::Read(const sgxString &filename)
{
	CSGXFile xmlfile(filename);
	
	if (xmlfile.IsValid()) {
		return Read(xmlfile);
	}

	return FALSE;
}

tBOOL sgxXMLReader::Read(CSGXFile &file)
{
	return sgxXMLParser::Get()->Parse(m_Tree, file);
}


