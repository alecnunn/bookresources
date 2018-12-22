#ifndef _BASE_INTERFACE_H_
#define _BASE_INTERFACE_H_

#include "core_defs.h"

class BaseInterface
{
public:
	BaseInterface(){}
	virtual ~BaseInterface(){}

	virtual GEN_RESULT Init() = 0;
	virtual GEN_RESULT Term() = 0;

protected:
};

#endif
