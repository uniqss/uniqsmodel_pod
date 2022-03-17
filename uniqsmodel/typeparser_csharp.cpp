#include "typeparser_csharp.h"

const char* pszRawTypeSpecific_csharp[] = {
    // clang-format off
	"invalid",
	"bool",
	"sbyte",
	"byte",
	"Int16",
	"UInt16",
	"int",
	"UInt32",
	"long",
	"ulong",
	"string",
	"invalid"
    // clang-format on
};
const char* TypeParser_csharp::GetRawTypeSpecific(EUniqsRawType eType) {
    return pszRawTypeSpecific_csharp[eType];
}

const char* pszComplexTypeSpecific_csharp[] = {
    // clang-format off
	"invalid",
	"List<",
	"Dictionary<",
	"invalid"
    // clang-format on
};
const char* TypeParser_csharp::GetComplexTypeSpecific(EUniqsComplexType eType) {
    return pszComplexTypeSpecific_csharp[eType];
}

const char* pszDefaultValueSpecific_csharp[] = {
    // clang-format off
	"invalid",
	"false",
	"0",
	"0",
	"0",
	"0",
	"0",
	"0",
	"0",
	"0",
	"string.Empty",
	"invalid"
    // clang-format on
};
const char* TypeParser_csharp::GetDefaultValueSpecific(EUniqsRawType eType) {
    return pszDefaultValueSpecific_csharp[eType];
}
