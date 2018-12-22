#ifndef __CCMPDescription_H
#define __CCMPDescription_H

#include "ICmpDescription.h"

class CCmpDescription : public ICmpDescription
{
public:
	CCmpDescription();
	virtual ~CCmpDescription();

	// Static methods
	static void			RegisterComponentType(void);
	static IComponent	*CreateMe();
	static bool			DestroyMe(IComponent *);

	// Virtual IComponent methods
	virtual bool		Init(CObjectId, CParameterNode &);
	virtual void		Deinit(void);
	virtual EMessageResult		HandleMessage(const CMessage &);
	virtual EComponentTypeId	GetComponentTypeId(void);

	// ICmpDescription methods
	virtual void		SetShortDescription(const char *);
	virtual void		SetLongDescription(const char *);
	virtual const char	*GetShortDescription() const;
	virtual const char	*GetLongDescription() const;
private:
	char		mShortDesc[MAX_STR_LEN];
	char		mLongDesc[MAX_STR_LEN];
};

#endif //__CCMPDescription_H