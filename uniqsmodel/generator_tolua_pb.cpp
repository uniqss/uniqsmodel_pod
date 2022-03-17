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

bool Generator_tolua::_Generate_tolua_pb(UniqsProto &proto, const string &strPBTo, std::string &strError) {
    string strToFile = strPBTo;
    strToFile += "uniqsmodel_" + proto.m_strName + ".proto";

    std::ofstream ofs(strToFile.c_str());
    if (!ofs) {
        strError = string(__FUNCTION__) + " ofs create failed.";
        return false;
    }

    // WriteUtf8BOMHead(ofs);
    WriteFileDeclarations(ofs);

    Indent indent;

    ofs << indent << "syntax = \"proto2\";" << endl;
    if (!proto.m_strProtobufNamespace.empty()) {
        ofs << indent << "package google.protobuf." << proto.m_strProtobufNamespace << ";" << endl;
    } else {
        ofs << indent << "package google.protobuf;" << endl;
    }

    for (const auto &its : proto.vecStructs) {
        ofs << indent << "message " << its.strName << endl;
        LEFT_BRACE;
        int nPropCount = 0;
        for (const auto &it : its.vecProperties) {
            ++nPropCount;

            if (g_pTypeParser->IsComplexType(it.eComplexType)) {
                // map暂时只支持存value
                if (it.eComplexType == EUniqsComplexType_map) {
                    ofs << indent << "repeated " << it.strType << " " << it.strName << " = " << std::to_string(nPropCount) << ";" << endl;
                } else if (it.eComplexType == EUniqsComplexType_array) {
                    ofs << indent << "repeated " << it.strType << " " << it.strName << " = " << std::to_string(nPropCount) << ";" << endl;
                } else {
                    ofs << indent << "// not supported type:" << it.strName << endl;
                }
            } else {
                ofs << indent << "optional " << it.strType << " " << it.strName << " = " << std::to_string(nPropCount) << ";" << endl;
            }
        }
        RIGHT_BRACE;
        ofs << indent << endl;
    }

    return true;
}
