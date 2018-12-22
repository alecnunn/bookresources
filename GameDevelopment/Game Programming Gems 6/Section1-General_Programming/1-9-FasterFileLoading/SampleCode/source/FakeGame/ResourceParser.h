#ifndef _RESOURCE_PARSER_H_
#define _RESOURCE_PARSER_H_

#include "defs.h"

class ResourceParser
{
public:
	ResourceParser(){}
	virtual ~ResourceParser(){}
	virtual void Parse(const char* szResourceList, FileList& fileList, const char* sLevelName);
protected:
};

#endif
