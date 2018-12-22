// GUIObject.h: interface for the CGUIObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUIOBJECT_H__47DB74B9_1C4F_4C77_BCAB_607DABCCFFE3__INCLUDED_)
#define AFX_GUIOBJECT_H__47DB74B9_1C4F_4C77_BCAB_607DABCCFFE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Sprite;
class Font;

// These are the currently define GUI object run time names
extern const char *kpBaseName;
extern const char *kpSpriteName;
extern const char *kpButtonName;
extern const char *kpTextFieldName;
extern const char *kpListName;

class CTexture;
class CUserInterface;
class CMouse;

class CGUIObject  
{
public:
							CGUIObject();
	virtual					~CGUIObject();

	virtual	const char		*GetObjectTypeName() {return kpBaseName;}

	virtual bool			Update(float fSecsElapsed, CUserInterface *pParent) {return true;}
	virtual bool			Render(void);
	virtual void			ResChangeNotify(void);


	virtual int				ObjectCommand(const char *pCommand) {return 0;}
	virtual bool			SetTexture(char *pTextureName, bool bSetPos = true);
//	virtual void			SetTextureUV(Vector2d *puv);
	virtual void			SetPosition(float fx, float fy, float fw, float fh, float z);
	virtual	bool			SetFont(char *pFontName, float normalSize);

	virtual	bool			KeyHit(int ascii) {return false;}

	void					SetID(unsigned int id) {m_ID = id;}
	unsigned int			GetID(void) {return m_ID;}

	RECT					GetScreenRect() {return m_rScreen;}
	
	virtual bool			StealsMouse(int x, int y);

	void					Enable(bool bDisable, bool bKeepDrawing) {m_isDisabled = bDisable; m_bDrawWhenDisabled = bKeepDrawing;}
	bool					isDisabled() {return m_isDisabled;}
	bool					isDrawing() {return(!m_isDisabled || m_bDrawWhenDisabled);}

	static void				SetTexturePath(const char *sPath)	{m_sTexturePath = sPath;}
	static void				ResetTexturePath(void)				{m_sTexturePath = "Textures";}

protected:
	static std::string		m_sTexturePath;

	bool					m_isDisabled;
	bool					m_bDrawWhenDisabled;

	unsigned int			m_ID;
	RECT					m_rScreen;
//	Vector3d				m_vNormPosition;
	float					m_fNormWidth, m_fNormHeight;

	Sprite					*m_pTexture;
	Font					*m_pFont;
	std::string				m_sFontName;
	float					m_fontNormalSize;
};

#endif // !defined(AFX_GUIOBJECT_H__47DB74B9_1C4F_4C77_BCAB_607DABCCFFE3__INCLUDED_)
