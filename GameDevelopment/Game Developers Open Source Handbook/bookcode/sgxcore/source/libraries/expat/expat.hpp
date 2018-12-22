#ifndef SGX_LIBRARIES_EXPAT_HPP
#define SGX_LIBRARIES_EXPAT_HPP	1


class sgxXMLExpatParser : public sgxXMLParser {
public:
    static  sgxXMLParser    *Create();
    virtual tBOOL			Parse(sgxXMLTree &tree, CSGXFile &file);

protected:
    sgxXMLExpatParser();

private:
    virtual ~sgxXMLExpatParser();
};


#endif /* SGX_LIBRARIES_EXPAT_HPP */
