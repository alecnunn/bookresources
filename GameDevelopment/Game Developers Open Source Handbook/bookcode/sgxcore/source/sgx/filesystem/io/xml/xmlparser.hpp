#ifndef SGX_FILESYSTEM_IO_XMLPARSER_HPP
#define SGX_FILESYSTEM_IO_XMLPARSER_HPP	1


class sgxXMLParser {
public:
    static  sgxXMLParser    *Get();
    static  sgxXMLParser    *Create();
    virtual tBOOL			Parse(sgxXMLTree &tree, CSGXFile &file);

protected:
    static  sgxXMLParser    *ms_pSingleton;
    sgxXMLParser();
    virtual ~sgxXMLParser();
};


#endif /* SGX_FILESYSTEM_IO_XMLPARSER_HPP */
