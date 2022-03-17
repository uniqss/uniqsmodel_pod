#include "typeparser_cpp.h"
#include "uniqs_struct_property.h"

const char* pszRawTypeSpecific_cpp[] = {
    // clang-format off
	"invalid_pszRawTypeSpecific_cpp_1",
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
	"invalid_pszRawTypeSpecific_cpp_2"
    // clang-format on
};
const char* TypeParser_cpp::GetRawTypeSpecific(EUniqsRawType eType) {
    return pszRawTypeSpecific_cpp[eType];
}

std::string TypeParser_cpp::FormComplexStructMember(const Uniqs_StructProperty& prop) {
    if (IsRawValueType(prop)) {
        const char* rawTypeSpecific = GetRawTypeSpecific(prop.eValueType);
        if (prop.eComplexType == EUniqsComplexType_array) {
            return std::string("std::vector<") + rawTypeSpecific + "> " + prop.strName;
        } else if (prop.eComplexType == EUniqsComplexType_rawarray) {
            return std::string(rawTypeSpecific) + " " + prop.strName + "[" + prop.strMax + "]";
        }
    } else {

	}

    return "";
}

std::string TypeParser_cpp::FormComplexStructMemberClear(EUniqsComplexType eComplexType, EUniqsRawType eValueType, const std::string& strPropName,
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
	"invalid_pszComplexTypeSpecific_cpp_1",
	"std::vector<",
	"std::vector<",
	"invalid_pszComplexTypeSpecific_cpp_2"
    // clang-format on
};
const char* TypeParser_cpp::GetComplexTypeSpecific(EUniqsComplexType eType) {
    return pszComplexTypeSpecific_cpp[eType];
}

const char* pszDefaultValueSpecific_cpp[] = {
    // clang-format off
	"invalid_pszDefaultValueSpecific_cpp_1",
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
	"invalid_pszDefaultValueSpecific_cpp_2"
    // clang-format on
};
const char* TypeParser_cpp::GetDefaultValueSpecific(EUniqsRawType eType) {
    return pszDefaultValueSpecific_cpp[eType];
}
