#ifndef __UI_TYPES_
#define __UI_TYPES_

#include "XML Test.h"


/* 
 * a (too) simple rectangle class 
 */
class UIRectangle
{
public:
	UIRectangle(void) :	mTop(0.0F), mBottom(0.0F), 
						mLeft(0.0F), mRight(0.0F) 
						{}

	float mTop;
	float mBottom;
	float mLeft;
	float mRight;
};


/* 
 * specifies a ui event 
 */
class UIEvent
{
public:
						 UIEvent(void) {};
	virtual				~UIEvent(void) {};

	virtual bool		Fire(void) { return false; };

	virtual void		SetParam1(const string& inParam)
							{ this->mParam1 = inParam; }
	virtual void		SetParam2(const string& inParam)
							{ this->mParam2 = inParam; }
	virtual void		SetParam3(const string& inParam)
							{ this->mParam3 = inParam; }
	virtual void		SetParam4(const string& inParam)
							{ this->mParam4 = inParam; }

	virtual void		SetCodeMap(const string& inCode)
							{ this->mCodeMap = inCode; }

private:
	string				mCodeMap;
	
	string				mParam1;
	string				mParam2;
	string				mParam3;
	string				mParam4;
};


/* 
 * an abstract base class for user interface entities 
 */
class UIEntity
{
public:
								 UIEntity(void) 
									{ this->mEventMap.clear(); }
								 
	virtual						~UIEntity(void);
								
	virtual void				AppendText(const string& inString) {}
	
	virtual void				AddEvent(IXMLDOMElementPtr inElement, const string& inEventType);

	virtual UIRectangle&		GetRectangle(void)
									{ return this->mRectangle; }
	
	virtual bool				Draw(void) = 0;

	virtual void				SetBackgroundTexture(const string& inBgTex)
									{ this->mBackgroundTexture = inBgTex; }
	
	virtual void				SetName(const string& inName)
									{ this->mID = inName; }
						
private:				
	static map<string, uint32>	mIDMap;
	map<string, UIEvent*>		mEventMap;
	UIRectangle					mRectangle;
	string						mBackgroundTexture;
	string						mID;
};


/* 
 * specifies a ui screen 
 */
class UIScreen : public UIEntity
{
public:
						UIScreen(void) 
						{ this->mControls.clear(); }

	virtual				~UIScreen(void) 
						{
							for (int theLoop = 0; theLoop < this->mControls.size(); theLoop++)
								delete this->mControls[theLoop];
						}

	virtual void		AddControl(UIEntity* inControl)
							{ this->mControls.push_back(inControl); }
						
	virtual bool		Draw(void) { return false; }

private:
	vector<UIEntity*>	mControls;
};


/* 
 * specifies a ui button 
 */
class UIButton : public UIEntity
{
public:
						 UIButton(void) {};
	virtual				~UIButton(void) {};

	virtual void		AppendText(const string& inString) 
							{ this->mText = inString; }
	
	virtual bool		Draw(void) { return false; }

private:
	string				mText;
};


/* 
 * specifies a ui listbox 
 */
class UIListBox : public UIEntity
{
public:
						 UIListBox(void) 
							{ this->mTextArray.clear(); }

	virtual				~UIListBox(void) {};

	virtual void		AppendText(const string& inString) 
							{ this->mTextArray.push_back(inString); }
	
	virtual bool		Draw(void) { return false; }

private:
	vector<string>		mTextArray;
};


/* 
 * specifies a ui textbox 
 */
class UITextBox : public UIEntity
{
public:
						 UITextBox(void) {}
	virtual				~UITextBox(void) {};

	virtual void		AppendText(const string& inString) 
							{ this->mText = inString; }
	
	virtual bool		Draw(void) { return false; }

private:
	string				mText;
};

#endif