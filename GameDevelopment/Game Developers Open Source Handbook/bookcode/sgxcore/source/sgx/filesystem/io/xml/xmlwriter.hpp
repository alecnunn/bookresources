#ifndef SGX_FILESYSTEM_IO_XMLWRITER_HPP
#define SGX_FILESYSTEM_IO_XMLWRITER_HPP	1

class sgxXMLWriter {
public:

	sgxXMLWriter(const sgxXMLTree &tree) : m_Tree(tree) { }

	tBOOL	Write(const sgxString &filename);
	tBOOL	Write(CSGXFile &file);
private:
	const sgxXMLTree	&m_Tree;
};

#endif /* SGX_FILESYSTEM_IO_XMLWRITER_HPP */
