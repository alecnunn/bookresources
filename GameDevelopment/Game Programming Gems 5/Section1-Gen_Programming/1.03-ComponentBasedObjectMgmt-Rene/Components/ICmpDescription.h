#ifndef __ICMPDESCRIPTION_H
#define __ICMPDESCRIPTION_H

#include "IComponent.h"

class ICmpDescription : public IComponent
{
public:
	virtual void		SetShortDescription(const char *) = 0;
	virtual void		SetLongDescription(const char *) = 0;
	virtual const char	*GetShortDescription() const = 0;
	virtual const char	*GetLongDescription() const = 0;
protected:
	static void		RegisterInterface(EComponentTypeId);
};

#endif //__ICMPDESCRIPTION_H