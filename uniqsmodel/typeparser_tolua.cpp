#include "typeparser_tolua.h"

namespace uniqs_tp_tolua {
const char* pszRawTypeSpecific[] = {
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

const char* pszComplexTypeSpecific[] = {
    // clang-format off
		"invalid",
		"std::vector<",
		"std::unordered_map<",
		"invalid"
    // clang-format on
};

const char* pszDefaultValueSpecific[] = {
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
}  // namespace uniqs_tp_tolua
const char* TypeParser_tolua::GetRawTypeSpecific(EUniqsRawType eType) {
    return uniqs_tp_tolua::pszRawTypeSpecific[eType];
}

const char* TypeParser_tolua::GetComplexTypeSpecific(EUniqsComplexType eType) {
    return uniqs_tp_tolua::pszComplexTypeSpecific[eType];
}

const char* TypeParser_tolua::GetDefaultValueSpecific(EUniqsRawType eType) {
    return uniqs_tp_tolua::pszDefaultValueSpecific[eType];
}
