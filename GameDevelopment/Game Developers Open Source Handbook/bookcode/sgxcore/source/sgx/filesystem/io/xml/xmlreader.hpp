#ifndef SGX_FILESYSTEM_IO_XMLREADER_HPP
#define SGX_FILESYSTEM_IO_XMLREADER_HPP	1

class sgxXMLReader {
public:

	sgxXMLReader(sgxXMLTree &tree);

	virtual tBOOL	Read(const sgxString &filename);
	virtual tBOOL	Read(CSGXFile &file);

	virtual ~sgxXMLReader();

protected:
	sgxXMLTree		&m_Tree;

private:
	sgxXMLReader();
};

#endif /* SGX_FILESYSTEM_IO_XMLREADER_HPP */
