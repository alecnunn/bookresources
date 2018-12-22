#ifndef SGX_IMAGELOADER_HPP
#define SGX_IMAGELOADER_HPP

class CSGXSurface;

class CImageParser {
public:
	static	void	Initialize();

	virtual tBOOL	IsRecognised(CSGXFile &file) const;
	virtual tBOOL	Open(CSGXFile &file, CImageData &properties) const;
	virtual tBOOL	Close(CImageData &properties) const;

protected:
	static CImageParser	*m_pSingleton;
};

class CImageLoader {
public:
	static CImageLoader *Create();
	static CImageLoader *Get();

	virtual void	Initialize();
	tBOOL			Register(const CImageParser *pCls);
	tBOOL			LoadImageData(CImageData &data, CSGXFile &file);

protected:
	CImageLoader();

	static CImageLoader	*m_pSingleton;

private:
	sgxVector<const CImageParser *>	m_Parsers;
};



#endif /* SGX_IMAGELOADER_HPP */

