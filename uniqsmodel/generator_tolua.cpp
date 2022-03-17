#include "generator_tolua.h"

#include "stl.h"

#include "uniqsproto_constdefines.h"
#include "indent.h"

#include "typeparser.h"

#define LEFT_BRACE                \
    ofs << indent << "{" << endl; \
    ++indent;

#define RIGHT_BRACE \
    --indent;       \
    ofs << indent << "}" << endl;
#define RIGHT_BRACE_FORCLASS \
    --indent;                \
    ofs << indent << "};" << endl;

#define PUBLIC_DECLARATION              \
    --indent;                           \
    ofs << indent << "public:" << endl; \
    ++indent;
#define PRIVATE_DECLARATION              \
    --indent;                            \
    ofs << indent << "private:" << endl; \
    ++indent;

#define IFDEFCPLUSPLUS_BEGIN ofs << "#ifdef __cplusplus" << endl;
#define EIF ofs << "#endif" << endl;

#define IFNDEFTOLUARELEASE ofs << "#ifndef TOLUA_RELEASE" << endl;

bool Generator_tolua::Generate_i(UniqsProto& proto, const string& strTo, const string& strPBTo, std::string& strError) {
    if (!_GenerateDefines(proto, strTo, strError)) {
        return false;
    }
    // 生成两类文件，第一类：类及嵌套类的头文件和源文件，多个
    if (!_GenerateStructs_header(proto, strTo, strError)) {
        return false;
    }
    if (!_GenerateStructs_source(proto, strTo, strError)) {
        return false;
    }
    // 生成tolua的文件
    if (!_Generate_tolua_header(proto, strTo, strError)) {
        return false;
    }
    if (!_Generate_tolua_source(proto, strTo, strError)) {
        return false;
    }
    // 生成allocator
    if (!_GenerateAllocator_header(proto, strTo, strError)) {
        return false;
    }
    if (!_GenerateAllocator_source(proto, strTo, strError)) {
        return false;
    }
    // pb
    if (!_Generate_tolua_pb(proto, strPBTo, strError)) {
        return false;
    }

    return true;
}

bool Generator_tolua::_GenerateDefines(UniqsProto& proto, const string& strTo, std::string& strError) {
    Indent indent;

    // string strEnumMsgName = "E" + proto.m_strName + "Msg";

    string strToFile = strTo;
    string strDefineName = proto.m_strName + pszDefinePostfix;
    strToFile += strDefineName;
    strToFile += ".h";
    std::ofstream ofs(strToFile.c_str());
    if (!ofs) {
        strError = "Generator_tolua::_GenerateDefines ofs create failed.";
        return false;
    }

    WriteUtf8BOMHead(ofs);

    ofs << indent << "#pragma once" << endl << endl;

    if (!g_strNamespace.empty()) {
        ofs << indent << g_strNamespace << endl;
    }
    LEFT_BRACE;

    for (const auto& it : proto.vecDefines) {
        ofs << indent << "// " << it.strDescription << endl;
        ofs << indent << "#define " << it.strName << " " << it.nValue << endl;
        ofs << endl;
    }

    ofs << endl;

    RIGHT_BRACE;

    ofs << endl;

    return true;
}

std::string Generator_tolua::_GetToLuaComplexTypeExpand(const UniqsProto_StructProperty& rProp) {
    if (rProp.eComplexType == EUniqsComplexType_map) {
        if (g_pTypeParser->IsRawType(rProp.eRawType)) {
            return std::string("unordered_map_") + g_pTypeParser->GetRawTypeSpecific(rProp.eRawType_KeyType) + "_" +
                   g_pTypeParser->GetRawTypeSpecific(rProp.eRawType) + "__";
        } else {
            return std::string("unordered_map_") + g_pTypeParser->GetRawTypeSpecific(rProp.eRawType_KeyType) + "_" + rProp.strType + "_";
        }
    }
    return "ComplexTypeExpandNotSupoorted";
}

std::string Generator_tolua::_GetToLuaComplexTypeTemplate(const UniqsProto_StructProperty& rProp) {
    if (rProp.eComplexType == EUniqsComplexType_map) {
        if (g_pTypeParser->IsRawType(rProp.eRawType)) {
            return std::string("unordered_map<") + g_pTypeParser->GetRawTypeSpecific(rProp.eRawType_KeyType) + ", " +
                   g_pTypeParser->GetRawTypeSpecific(rProp.eRawType) + ">";
        } else {
            return std::string("unordered_map<") + g_pTypeParser->GetRawTypeSpecific(rProp.eRawType_KeyType) + ", " + rProp.strType + "*>";
        }
    }
    if (rProp.eComplexType == EUniqsComplexType_array) {
        if (g_pTypeParser->IsRawType(rProp.eRawType)) {
            return std::string("vector<") + g_pTypeParser->GetRawTypeSpecific(rProp.eRawType) + ">";
        } else {
            return std::string("vector<") + rProp.strType + "*>";
        }
    }
    return "ComplexTypeTemplateNotSupoorted";
}

const char* Generator_tolua::_tolua_typename(EUniqsRawType eType) {
    static const char* pszTolua[] = {
        // clang-format off
        "usertype",
        "boolean", 
        "number",  
        "number",
        "number",
        "number",
        "number",
        "number",
        "int64_t",
        "uint64_t",
        "cppstring",
        "usertype"
        // clang-format on
    };
    return pszTolua[eType];
}
