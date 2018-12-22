#ifndef SGX_BMP_PARSER_HPP
#define SGX_BMP_PARSER_HPP

class CBMPParser : public CImageParser {
public:
	static	void	Initialize();

	virtual tBOOL	IsRecognised(CSGXFile &file) const;
	virtual tBOOL	Open(CSGXFile &file, CImageData &properties) const;
	virtual tBOOL	Close(CImageData &properties) const;

protected:
	static CBMPParser	*m_pSingleton;
};

#endif /* SGX_BMP_PARSER_HPP */
