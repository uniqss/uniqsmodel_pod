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

std::string TypeParser_cpp::FormComplexStructMember(EUniqsComplexType eComplexType, EUniqsRawType eRawType, const std::string& strPropName,
                                                    const std::string& strMax) {
    const char* rawTypeSpecific = GetRawTypeSpecific(eRawType);
    if (eComplexType == EUniqsComplexType_array) {
        return std::string("std::vector<") + rawTypeSpecific + "> " + strPropName;
    } else if (eComplexType == EUniqsComplexType_rawarray) {
        return std::string(rawTypeSpecific) + " " + strPropName + "[" + strMax + "]";
    }

    return "";
}

std::string TypeParser_cpp::FormComplexStructMemberClear(EUniqsComplexType eComplexType, EUniqsRawType eRawType, const std::string& strPropName,
                                                    const std::string& strMax) {
    if (eComplexType == EUniqsComplexType_array) {
		return strPropName + ".clear();";
    } else if (eComplexType == EUniqsComplexType_rawarray) {
		return std::string("memset(") + strPropName + ", 0, sizeof(" + strPropName + "));";
    }

    return "";
}


const char* pszComplexTypeSpecific_cpp[] = {
    // clang-format off
	"invalid",
	"std::vector<",
	"std::vector<",
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
