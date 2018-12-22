#ifndef __DXFONT__
#define __DXFONT__



class Font
{
public:
					Font();
	virtual			~Font();

	virtual bool	Create(int size, const char *typeface);
	virtual bool	DrawText(const char *text, DWORD color, int x, int y, int x2=640, int y2=480);
	virtual void    GetTextExtent(const char *text, unsigned int *pWidth, unsigned int *pHeight=NULL) {*pWidth = 0;}
protected:
	HFONT			m_hFont;
	LPD3DXFONT		m_pd3dxFont;

};

#endif