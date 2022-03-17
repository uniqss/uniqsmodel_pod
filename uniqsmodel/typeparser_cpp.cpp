#include "typeparser_cpp.h"

const char* pszRawTypeSpecific_cpp[] = {
    // clang-format off
	"invalid",
	"bool",
	"char",
	"unsigned char",
	"short",
	"unsigned short",
	"int",
	"unsigned",
	"int64_t",
	"uint64_t",
	"std::string",
	"invalid"
    // clang-format on
};
const char* TypeParser_cpp::GetRawTypeSpecific(EUniqsRawType eType) {
    return pszRawTypeSpecific_cpp[eType];
}

const char* pszComplexTypeSpecific_cpp[] = {
    // clang-format off
	"invalid",
	"std::vector<",
	"std::unordered_map<",
	"invalid"
    // clang-format on
};
const char* TypeParser_cpp::GetComplexTypeSpecific(EUniqsComplexType eType) {
    return pszComplexTypeSpecific_cpp[eType];
}

const char* pszDefaultValueSpecific_cpp[] = {
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
	"\"\"",
	"invalid"
    // clang-format on
};
const char* TypeParser_cpp::GetDefaultValueSpecific(EUniqsRawType eType) {
    return pszDefaultValueSpecific_cpp[eType];
}
