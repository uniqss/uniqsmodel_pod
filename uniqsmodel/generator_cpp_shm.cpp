#include "generator_cpp_shm.h"

#include "stl.h"

#include "uniqsproto_constdefines.h"
#include "indent.h"

#include "typeparser.h"
#include "typeparser_cpp_shm.h"

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

Generator_cpp_shm::Generator_cpp_shm() {
    m_pTypeParser = new TypeParser_cpp_shm();
}

bool Generator_cpp_shm::Generate_i(UniqsProto &proto, const string &strTo, const string &strPBTo, std::string &strError) {
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

bool Generator_cpp_shm::_GenerateDefines(UniqsProto &proto, const string &strTo, std::string &strError) {
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

bool Generator_cpp_shm::_GenerateStructs_header(UniqsProto &proto, const string &strTo, std::string &strError) {
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
        ofs << "#include \"smd.h\"" << endl;
        // for (auto it : proto.vecIncludes)
        //{
        //	ofs << "#include \"" << it.strName << ".h\"" << " //" << it.strDescription << endl;
        // }

        std::unordered_set<std::string> strIncludeAvoidSame;
        for (const auto &prop : it.vecProperties) {
            if (!m_pTypeParser->IsRawType(prop.eRawType)) {
                if (strIncludeAvoidSame.find(prop.strType) == strIncludeAvoidSame.end()) {
                    strIncludeAvoidSame.insert(prop.strType);
                    ofs << "#include \"" << prop.strType << ".h\"" << endl;
                }
            }
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
            if (m_pTypeParser->IsComplexType(prop.eComplexType)) {
                ofs << indent << m_pTypeParser->GetComplexTypeSpecific(prop.eComplexType);
                if (prop.eComplexType == EUniqsComplexType_array) {
                    if (m_pTypeParser->IsRawType(prop.eRawType)) {
                        ofs << m_pTypeParser->GetRawTypeSpecific(prop.eRawType) << "> " << prop.strName << ";" << endl;
                    } else {
                        ofs << prop.strType << "> " << prop.strName << ";" << endl;
                    }
                } else if (prop.eComplexType == EUniqsComplexType_map) {
                    // rawtype的value统统int，默认0
                    if (m_pTypeParser->IsRawType(prop.eRawType)) {
                        ofs << m_pTypeParser->GetRawTypeSpecific(prop.eRawType) << ", int> " << prop.strName << ";" << endl;
                    } else {
                        ofs << m_pTypeParser->GetRawTypeSpecific(prop.eRawType_KeyType) << ", " << prop.strType << "> " << prop.strName << ";" << endl;
                    }
                }
            } else if (m_pTypeParser->IsRawType(prop.eRawType)) {
                ofs << indent << m_pTypeParser->GetRawTypeSpecific(prop.eRawType);
                ofs << " " << prop.strName << ";" << endl;
            } else {
                ofs << indent << prop.strType;
                ofs << " " << prop.strName << ";" << endl;
            }
        }
        ofs << endl;

        // functions
        PUBLIC_DECLARATION;

        // ctor
        ofs << indent << it.strName << "(){ Clear(true); }" << endl;

        ofs << endl;

        // clear
        ofs << indent << "void Clear(bool bDestruct);" << endl;

        RIGHT_BRACE_FORCLASS;
        RIGHT_BRACE;
    }

    return true;
}

bool Generator_cpp_shm::_GenerateStructs_source(UniqsProto &proto, const string &strTo, std::string &strError) {
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
            if (m_pTypeParser->IsComplexType(prop.eComplexType)) {
                ofs << indent << "" << GetComplexLenType() << " wLen_" << prop.strName << " = " << prop.strName << ".size() < " << prop.strMax << " ?"
                    << " (" << GetComplexLenType() << ")" << prop.strName << ".size() : (" << GetComplexLenType() << ")" << prop.strMax << "; " << endl;
                ofs << indent << "if (!Serialize_RawType(pszMsgData, Index, Length, wLen_" << prop.strName << "))" << endl;
                LEFT_BRACE;
                ofs << indent << "return false;" << endl;
                RIGHT_BRACE;

                if (prop.eComplexType == EUniqsComplexType_array) {
                    ofs << indent << "for (int nIdx = 0; nIdx < " << prop.strName << ".size() && nIdx < " << prop.strMax << "; ++nIdx)" << endl;
                    LEFT_BRACE;
                    if (m_pTypeParser->IsRawType(prop.eRawType)) {
                        ofs << indent << "if (!Serialize_RawType(pszMsgData, Index, Length, " << prop.strName << "[nIdx]))" << endl;
                    } else {
                        ofs << indent << "if (!" << prop.strName << "[nIdx].Serialize_impl(pszMsgData, Index, Length))" << endl;
                    }

                    LEFT_BRACE;
                    ofs << indent << "return false;" << endl;
                    RIGHT_BRACE;

                    RIGHT_BRACE;
                } else if (prop.eComplexType == EUniqsComplexType_map) {
                    ofs << indent << "int nLen_" << prop.strName << " = 0;" << endl;
                    ofs << indent << "for (const auto& it : " << prop.strName << ")" << endl;
                    LEFT_BRACE;

                    ofs << indent << "if (++nLen_" << prop.strName << " > " << prop.strMax << ")" << endl;
                    LEFT_BRACE;
                    ofs << indent << "return false;" << endl;
                    RIGHT_BRACE;

                    if (m_pTypeParser->IsRawType(prop.eRawType)) {
                        ofs << indent << "if (!Serialize_RawType(pszMsgData, Index, Length, it.first))" << endl;
                    } else {
                        ofs << indent << "if (!it.second.Serialize_impl(pszMsgData, Index, Length))" << endl;
                    }
                    LEFT_BRACE;
                    ofs << indent << "return false;" << endl;
                    RIGHT_BRACE;

                    RIGHT_BRACE;
                }
            } else if (m_pTypeParser->IsRawType(prop.eRawType)) {
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
            if (m_pTypeParser->IsComplexType(prop.eComplexType)) {
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
                    if (m_pTypeParser->IsRawType(prop.eRawType)) {
                        ofs << indent << m_pTypeParser->GetRawTypeSpecific(prop.eRawType) << " oTmp = " << m_pTypeParser->GetDefaultValueSpecific(prop.eRawType)
                            << ";" << endl;
                        ofs << indent << "if (!DeSerialize_RawType(pszMsgData, Index, Length, oTmp))" << endl;
                    } else {
                        ofs << indent << prop.strType << " oTmp;" << endl;
                        ofs << indent << "if (!oTmp.DeSerialize_impl(pszMsgData, Index, Length))" << endl;
                    }

                    LEFT_BRACE;
                    ofs << indent << "return false;" << endl;
                    RIGHT_BRACE;
                    ofs << indent << prop.strName << ".emplace_back(oTmp);" << endl;

                    RIGHT_BRACE;
                } else if (prop.eComplexType == EUniqsComplexType_map) {
                    ofs << indent << "for (int nIdx = 0; nIdx < wLen_" << prop.strName << "; ++nIdx)" << endl;
                    LEFT_BRACE;
                    if (m_pTypeParser->IsRawType(prop.eRawType)) {
                        ofs << indent << m_pTypeParser->GetRawTypeSpecific(prop.eRawType) << " oTmp = " << m_pTypeParser->GetDefaultValueSpecific(prop.eRawType)
                            << ";" << endl;
                        ofs << indent << "if (!DeSerialize_RawType(pszMsgData, Index, Length, oTmp))" << endl;
                    } else {
                        ofs << indent << prop.strType << " oTmp;" << endl;
                        ofs << indent << "if (!oTmp.DeSerialize_impl(pszMsgData, Index, Length))" << endl;
                    }

                    LEFT_BRACE;
                    ofs << indent << "return false;" << endl;
                    RIGHT_BRACE;
                    if (m_pTypeParser->IsRawType(prop.eRawType)) {
                        ofs << indent << prop.strName << "[oTmp] = 0;" << endl;
                    } else {
                        ofs << indent << prop.strName << "[oTmp." << prop.strKey << "] = oTmp;" << endl;
                    }

                    RIGHT_BRACE;
                }
            } else if (m_pTypeParser->IsRawType(prop.eRawType)) {
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
        ofs << indent << "void " << it.strName << "::Clear(bool bDestruct)" << endl;
        LEFT_BRACE;
        for (const auto &prop : it.vecProperties) {
            if (m_pTypeParser->IsComplexType(prop.eComplexType)) {
                ofs << indent << prop.strName << ".clear();" << endl;
            } else if (m_pTypeParser->IsRawType(prop.eRawType)) {
                ofs << indent << prop.strName << " = " << m_pTypeParser->GetDefaultValueSpecific(prop.eRawType) << ";" << endl;
            } else {
                ofs << indent << prop.strName << ".Clear(bDestruct);" << endl;
            }
        }
        RIGHT_BRACE;
#endif

        RIGHT_BRACE;
    }

    return true;
}
