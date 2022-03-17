#include "generator_cpp.h"

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


bool Generator_cpp::Generate(UniqsProto &proto, const string &strTo, const string &strPBTo, std::string &strError) {
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

    return true;
}

bool Generator_cpp::_GenerateDefines(UniqsProto &proto, const string &strTo, std::string &strError) {
    Indent indent;

    string strEnumMsgName = "E" + proto.m_strName + "Msg";

    string strToFile = strTo;
    string strDefineName = proto.m_strName + pszDefinePostfix;
    strToFile += strDefineName;
    strToFile += ".h";
    std::ofstream ofs(strToFile.c_str());
    if (!ofs) {
        strError = "Generator_cpp::_GenerateDefines ofs create failed.";
        return false;
    }

    WriteUtf8BOMHead(ofs);

    ofs << indent << "#pragma once" << endl << endl;

    if (!g_strNamespace.empty()) {
        ofs << indent << g_strNamespace << endl;
    }
    LEFT_BRACE;

    for (const auto &it : proto.vecDefines) {
        ofs << indent << "// " << it.strDescription << endl;
        ofs << indent << "#define " << it.strName << " " << it.nValue << endl;
        ofs << endl;
    }

    ofs << endl;

    ofs << indent << "enum " << strEnumMsgName << endl;
    LEFT_BRACE;
    RIGHT_BRACE_FORCLASS;
    ofs << endl;

    RIGHT_BRACE;

    ofs << endl;

    return true;
}

bool Generator_cpp::_GenerateStructs_header(UniqsProto &proto, const string &strTo, std::string &strError) {
    for (const auto &it : proto.vecStructs) {
        string strToFile = strTo;
        strToFile += it.strName;
        strToFile += ".h";

        std::ofstream ofs(strToFile.c_str());
        if (!ofs) {
            strError = "Generator_cpp::Generate_i ofs create failed.";
            return false;
        }

        WriteUtf8BOMHead(ofs);
        WriteFileDeclarations(ofs);

        ofs << "#pragma once\n" << endl;
        // ofs << "#include \"MsgBase.h\"" << endl;
        ofs << "#include <vector>" << endl;
        ofs << "#include <string>" << endl;
        // for (auto it : proto.vecIncludes)
        //{
        //	ofs << "#include \"" << it.strName << ".h\"" << " //" << it.strDescription << endl;
        // }

        bool haveComplexType = false;

        std::unordered_set<std::string> strIncludeAvoidSame;
        std::vector<std::string> vecIncludes;
        for (const auto &prop : it.vecProperties) {
            if (!g_pTypeParser->IsRawValueType(prop)) {
                if (strIncludeAvoidSame.find(prop.strValueType) == strIncludeAvoidSame.end()) {
                    strIncludeAvoidSame.insert(prop.strValueType);
                    vecIncludes.emplace_back(prop.strValueType);
                }
            } else {
                haveComplexType = true;
            }
        }

        if (haveComplexType) {
            vecIncludes.insert(vecIncludes.begin(), proto.m_strName + pszDefinePostfix);
        }

        for (auto it : vecIncludes) {
            ofs << "#include \"" + it + ".h\"" << endl;
        }

        ofs << endl;

        Indent indent;

        if (!g_strNamespace.empty()) {
            ofs << indent << g_strNamespace << endl;
        }

        LEFT_BRACE;

        ofs << indent << "// " << it.strDescription << endl;
        ofs << indent << "class " << it.strName << endl;
        LEFT_BRACE;

        // members
        PUBLIC_DECLARATION;

        for (const auto &prop : it.vecProperties) {
            ofs << indent << "// " << prop.strDescription << endl;
            if (g_pTypeParser->IsComplexType(prop)) {
                ofs << indent << g_pTypeParser->FormComplexStructMember(prop) << ";" << endl;
            } else if (g_pTypeParser->IsRawValueType(prop)) {
                ofs << indent << g_pTypeParser->GetRawTypeSpecific(prop.eValueType);
                ofs << " " << prop.strName << ";" << endl;
            } else {
                ofs << indent << prop.strValueType;
                ofs << " " << prop.strName << ";" << endl;
            }
        }
        ofs << endl;

        // functions
        PUBLIC_DECLARATION;

        // ctor
        ofs << indent << it.strName << "(){ Clear(); }" << endl;

        ofs << endl;

        // clear
        // ofs << indent << "virtual void Clear();" << endl;
        ofs << indent << "void Clear();" << endl;

        RIGHT_BRACE_FORCLASS;
        RIGHT_BRACE;
    }

    return true;
}

bool Generator_cpp::_GenerateStructs_source(UniqsProto &proto, const string &strTo, std::string &strError) {
    string strDefineName = proto.m_strName + pszDefinePostfix;
    for (const auto &it : proto.vecStructs) {
        string strToFile = strTo;
        strToFile += it.strName;
        strToFile += ".cpp";

        std::ofstream ofs(strToFile.c_str());
        if (!ofs) {
            strError = "Generator_cpp::Generate_i ofs create failed.";
            return false;
        }

        WriteUtf8BOMHead(ofs);
        WriteFileDeclarations(ofs);

        ofs << "#include \"" << it.strName << ".h\"" << endl;
        ofs << "#include \"" << strDefineName << ".h\"" << endl;
        ofs << "#include <cstring>" << endl;
        ofs << endl;

        Indent indent;
        if (!g_strNamespace.empty()) {
            ofs << indent << g_strNamespace << endl;
        }
        LEFT_BRACE;

        // serialize
#if 0
        ofs << indent << "bool " << it.strName << "::Serialize_impl(char* pszMsgData, int& Index, int Length) const" << endl;
        LEFT_BRACE;
        for (const auto &prop : it.vecProperties) {
            ofs << indent << "// serialize " << prop.strDescription << endl;
            if (g_pTypeParser->IsComplexType(prop)) {
                ofs << indent << "" << GetComplexLenType() << " wLen_" << prop.strName << " = " << prop.strName << ".size() < " << prop.strMax << " ?"
                    << " (" << GetComplexLenType() << ")" << prop.strName << ".size() : (" << GetComplexLenType() << ")" << prop.strMax << "; " << endl;
                ofs << indent << "if (!Serialize_RawType(pszMsgData, Index, Length, wLen_" << prop.strName << "))" << endl;
                LEFT_BRACE;
                ofs << indent << "return false;" << endl;
                RIGHT_BRACE;

                if (prop.eComplexType == EUniqsComplexType_array) {
                    ofs << indent << "for (int nIdx = 0; nIdx < " << prop.strName << ".size() && nIdx < " << prop.strMax << "; ++nIdx)" << endl;
                    LEFT_BRACE;
                    if (g_pTypeParser->IsRawValueType(prop)) {
                        ofs << indent << "if (!Serialize_RawType(pszMsgData, Index, Length, " << prop.strName << "[nIdx]))" << endl;
                    } else {
                        ofs << indent << "if (!" << prop.strName << "[nIdx].Serialize_impl(pszMsgData, Index, Length))" << endl;
                    }

                    LEFT_BRACE;
                    ofs << indent << "return false;" << endl;
                    RIGHT_BRACE;

                    RIGHT_BRACE;
                }
            } else if (g_pTypeParser->IsRawValueType(prop)) {
                ofs << indent << "if (!Serialize_RawType(pszMsgData, Index, Length, " << prop.strName << "))" << endl;
                LEFT_BRACE;
                ofs << indent << "return false;" << endl;
                RIGHT_BRACE;
            } else {
                ofs << indent << "if (!" << prop.strName << ".Serialize_impl(pszMsgData, Index, Length))" << endl;
                LEFT_BRACE;
                ofs << indent << "return false;" << endl;
                RIGHT_BRACE;
            }

            ofs << endl;
        }
        ofs << indent << "return true;" << endl;
        RIGHT_BRACE;

        ofs << endl;
#endif

        // deserialize
#if 0
        ofs << indent << "bool " << it.strName << "::DeSerialize_impl(const char* pszMsgData, int& Index, int Length)" << endl;
        LEFT_BRACE;
        for (const auto &prop : it.vecProperties) {
            ofs << indent << "// deserialize " << prop.strDescription << endl;
            if (g_pTypeParser->IsComplexType(prop)) {
                ofs << indent << GetComplexLenType() << " wLen_" << prop.strName << " = 0;" << endl;
                ofs << indent << "if (!DeSerialize_RawType(pszMsgData, Index, Length, wLen_" << prop.strName << "))" << endl;
                LEFT_BRACE;
                ofs << indent << "return false;" << endl;
                RIGHT_BRACE;
                ofs << indent << "if (wLen_" << prop.strName << " > (" << GetComplexLenType() << ")" << prop.strMax << ")" << endl;
                LEFT_BRACE;
                ofs << indent << "wLen_" << prop.strName << " = (" << GetComplexLenType() << ")" << prop.strMax << ";" << endl;
                RIGHT_BRACE;

                if (prop.eComplexType == EUniqsComplexType_array) {
                    ofs << indent << "for (int nIdx = 0; nIdx < wLen_" << prop.strName << "; ++nIdx)" << endl;
                    LEFT_BRACE;
                    if (g_pTypeParser->IsRawValueType(prop)) {
                        ofs << indent << g_pTypeParser->GetRawTypeSpecific(prop.eValueType) << " oTmp = " << g_pTypeParser->GetDefaultValueSpecific(prop.eValueType)
                            << ";" << endl;
                        ofs << indent << "if (!DeSerialize_RawType(pszMsgData, Index, Length, oTmp))" << endl;
                    } else {
                        ofs << indent << prop.strValueType << " oTmp;" << endl;
                        ofs << indent << "if (!oTmp.DeSerialize_impl(pszMsgData, Index, Length))" << endl;
                    }

                    LEFT_BRACE;
                    ofs << indent << "return false;" << endl;
                    RIGHT_BRACE;
                    ofs << indent << prop.strName << ".emplace_back(oTmp);" << endl;

                    RIGHT_BRACE;
                }
            } else if (g_pTypeParser->IsRawValueType(prop)) {
                ofs << indent << "if (!DeSerialize_RawType(pszMsgData, Index, Length, " << prop.strName << "))" << endl;
                LEFT_BRACE;
                ofs << indent << "return false;" << endl;
                RIGHT_BRACE;
            } else {
                ofs << indent << "if (!" << prop.strName << ".DeSerialize_impl(pszMsgData, Index, Length))" << endl;
                LEFT_BRACE;
                ofs << indent << "return false;" << endl;
                RIGHT_BRACE;
            }

            ofs << endl;
        }
        ofs << indent << "return true;" << endl;
        RIGHT_BRACE;

        ofs << endl;
#endif

        // clear
#if 1
        // POD，直接memset
        ofs << indent << "void " << it.strName << "::Clear()" << endl;
        LEFT_BRACE;
        ofs << indent << "memset((void*)this, 0, sizeof(*this));" << endl;
        RIGHT_BRACE;
#endif

        RIGHT_BRACE;
    }

    return true;
}
