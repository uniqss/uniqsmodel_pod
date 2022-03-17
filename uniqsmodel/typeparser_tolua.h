#pragma once

#include "typeparser.h"

class TypeParser_tolua : public TypeParser
{
public:
	virtual const char* GetRawTypeSpecific(EUniqsRawType eType);
	virtual const char* GetComplexTypeSpecific(EUniqsComplexType eType);
	virtual const char* GetDefaultValueSpecific(EUniqsRawType eType);
};

