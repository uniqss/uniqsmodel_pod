#include "typeparser_cpp_shm.h"

const char* pszRawTypeSpecific_cpp_shm[] = {
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
	"smd::shm_string",
	"invalid"
    // clang-format on
};
const char* TypeParser_cpp_shm::GetRawTypeSpecific(EUniqsRawType eType) {
    return pszRawTypeSpecific_cpp_shm[eType];
}

const char* pszComplexTypeSpecific_cpp_shm[] = {
    // clang-format off
	"invalid",
	"smd::shm_vector<",
	"smd::shm_map<",
	"invalid"
    // clang-format on
};
const char* TypeParser_cpp_shm::GetComplexTypeSpecific(EUniqsComplexType eType) {
    return pszComplexTypeSpecific_cpp_shm[eType];
}

const char* pszDefaultValueSpecific_cpp_shm[] = {
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
const char* TypeParser_cpp_shm::GetDefaultValueSpecific(EUniqsRawType eType) {
    return pszDefaultValueSpecific_cpp_shm[eType];
}
