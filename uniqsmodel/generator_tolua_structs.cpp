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

bool Generator_tolua::_GenerateStructs_header(UniqsProto &proto, const string &strTo, std::string &strError) {
    for (const auto &it : proto.vecStructs) {
        string strToFile = strTo;
        strToFile += it.strName;
        strToFile += ".h";

        std::ofstream ofs(strToFile.c_str());
        if (!ofs) {
            strError = "Generator_tolua::Generate_i ofs create failed.";
            return false;
        }

        WriteUtf8BOMHead(ofs);
        WriteFileDeclarations(ofs);

        std::string strPragmaAndInclude = "#pragma once";
        strPragmaAndInclude += "\n#include <stdint.h>";
        strPragmaAndInclude += "\n#include <string>";
        strPragmaAndInclude += "\n#include <vector>";
        strPragmaAndInclude += "\n#include <unordered_map>";
        ofs << strPragmaAndInclude << endl;
        // for (auto it : proto.vecIncludes)
        //{
        //	ofs << "#include \"" << it.strName << ".h\"" << " //" << it.strDescription << endl;
        // }

        std::unordered_set<std::string> strIncludeAvoidSame;
        for (const auto &prop : it.vecProperties) {
            if (!g_pTypeParser->IsRawType(prop.eRawType)) {
                if (strIncludeAvoidSame.find(prop.strType) == strIncludeAvoidSame.end()) {
                    strIncludeAvoidSame.insert(prop.strType);
                    ofs << "#include \"" << prop.strType << ".h\"" << endl;
                }
            }
        }
        if (proto.m_bGenProtobuf) {
            ofs << "#include \"pb/uniqsmodel_" << proto.m_strName << ".pb.h\"" << endl;
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
        ofs << indent << "// members" << endl;
        PUBLIC_DECLARATION;

        for (const auto &prop : it.vecProperties) {
            ofs << indent << "// " << prop.strDescription << endl;
            if (g_pTypeParser->IsComplexType(prop.eComplexType)) {
                ofs << indent << g_pTypeParser->GetComplexTypeSpecific(prop.eComplexType);
                if (prop.eComplexType == EUniqsComplexType_array) {
                    if (g_pTypeParser->IsRawType(prop.eRawType)) {
                        ofs << g_pTypeParser->GetRawTypeSpecific(prop.eRawType) << "> " << prop.strName << ";" << endl;
                    } else {
                        ofs << prop.strType << "*> " << prop.strName << ";" << endl;
                    }
                } else if (prop.eComplexType == EUniqsComplexType_map) {
                    // rawtype的value统统int，默认0
                    if (g_pTypeParser->IsRawType(prop.eRawType)) {
                        ofs << g_pTypeParser->GetRawTypeSpecific(prop.eRawType) << ", " << g_pTypeParser->GetRawTypeSpecific(prop.eRawType_KeyType) << "> "
                            << prop.strName << ";" << endl;
                    } else {
                        ofs << g_pTypeParser->GetRawTypeSpecific(prop.eRawType_KeyType) << ", " << prop.strType << "*> " << prop.strName << ";" << endl;
                    }
                }
            } else if (g_pTypeParser->IsRawType(prop.eRawType)) {
                ofs << indent << g_pTypeParser->GetRawTypeSpecific(prop.eRawType);
                ofs << " " << prop.strName << ";" << endl;
            } else {
                ofs << indent << prop.strType;
                ofs << " " << prop.strName << ";" << endl;
            }
        }
        ofs << endl;

        const std::string &classname = it.strName;
        // singleton
        if (it.bSingleton) {
            ofs << indent << "// singleton simple implemention by uniqs" << endl;
            PRIVATE_DECLARATION;
            ofs << indent << "static " << classname << "* m_pInstance;" << endl;
            PUBLIC_DECLARATION;
            ofs << indent << "static " << classname << "* Instance()" << endl;
            LEFT_BRACE;
            ofs << indent << "return m_pInstance;" << endl;
            RIGHT_BRACE;
            ofs << indent << "static void CreateInstance()" << endl;
            LEFT_BRACE;
            ofs << indent << "m_pInstance = new " << classname << "();" << endl;
            RIGHT_BRACE;
            ofs << indent << "static void ReleaseInstance()" << endl;
            LEFT_BRACE;
            ofs << indent << "delete m_pInstance;" << endl;
            ofs << indent << "m_pInstance = nullptr;" << endl;
            RIGHT_BRACE;
        }

        ofs << endl;

        // functions
        ofs << indent << "// functions" << endl;
        PUBLIC_DECLARATION;

        // ctor
        ofs << indent << it.strName << "(){ Clear(false); }" << endl;

        // dtor 所有vector/map里存的指针全部要在这里统一释放
        ofs << indent << "virtual ~" << it.strName << "(){ Clear(true); }" << endl;

        ofs << endl;

        // UAlloc
        if (!it.bSingleton) {
            ofs << indent << "static " << it.strName << "* UAlloc();" << endl;
            ofs << indent << "static void UFree(" << it.strName << "* p);" << endl;
        }

        ofs << endl;

#if 0
		// serialize
		ofs << indent << "virtual bool Serialize_impl(char* pszMsgData, int& Index, int Length) const;" << endl;

		ofs << endl;

		// deserialize
		ofs << indent << "virtual bool DeSerialize_impl(const char* pszMsgData, int& Index, int Length);" << endl;

		ofs << endl;
#endif

        // clear
        ofs << indent << "virtual void Clear(bool bDestruct);" << endl;

        ofs << endl;

        if (proto.m_bGenProtobuf) {
            // FromPB
            ofs << indent << "virtual void FromPB(const google::protobuf::" << it.strName << "& pb, bool bSetDefault);" << endl;
            // ToPB
            ofs << indent << "virtual void ToPB(google::protobuf::" << it.strName << "& pb) const;" << endl;
        }

        RIGHT_BRACE_FORCLASS;
        RIGHT_BRACE;
    }

    return true;
}

bool Generator_tolua::_GenerateStructs_source(UniqsProto &proto, const string &strTo, std::string &strError) {
    string strDefineName = proto.m_strName + pszDefinePostfix;
    for (const auto &its : proto.vecStructs) {
        string strToFile = strTo;
        strToFile += its.strName;
        strToFile += ".cpp";

        std::ofstream ofs(strToFile.c_str());
        if (!ofs) {
            strError = "Generator_tolua::Generate_i ofs create failed.";
            return false;
        }

        WriteUtf8BOMHead(ofs);
        WriteFileDeclarations(ofs);

        ofs << "#include \"" << its.strName << ".h\"" << endl;
        ofs << "#include \"" << strDefineName << ".h\"" << endl;
        ofs << "#include \"uniqsallocator_" << proto.m_strName << ".h\"" << endl;
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
            if (g_pTypeParser->IsComplexType(prop.eComplexType)) {
                ofs << indent << "" << GetComplexLenType() << " wLen_" << prop.strName << " = " << prop.strName << ".size() < " << prop.strMax << " ?"
                    << " (" << GetComplexLenType() << ")" << prop.strName << ".size() : (" << GetComplexLenType() << ")" << prop.strMax << "; " << endl;
                ofs << indent << "if (!Serialize_RawType(pszMsgData, Index, Length, wLen_" << prop.strName << "))" << endl;
                LEFT_BRACE;
                ofs << indent << "return false;" << endl;
                RIGHT_BRACE;

                if (prop.eComplexType == EUniqsComplexType_array) {
                    ofs << indent << "for (int nIdx = 0; nIdx < " << prop.strName << ".size() && nIdx < " << prop.strMax << "; ++nIdx)" << endl;
                    LEFT_BRACE;
                    if (g_pTypeParser->IsRawType(prop.eRawType)) {
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

                    if (g_pTypeParser->IsRawType(prop.eRawType)) {
                        ofs << indent << "if (!Serialize_RawType(pszMsgData, Index, Length, it.first))" << endl;
                    } else {
                        ofs << indent << "if (!it.second.Serialize_impl(pszMsgData, Index, Length))" << endl;
                    }
                    LEFT_BRACE;
                    ofs << indent << "return false;" << endl;
                    RIGHT_BRACE;

                    RIGHT_BRACE;
                }
            } else if (g_pTypeParser->IsRawType(prop.eRawType)) {
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
            if (g_pTypeParser->IsComplexType(prop.eComplexType)) {
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
                    if (g_pTypeParser->IsRawType(prop.eRawType)) {
                        ofs << indent << g_pTypeParser->GetRawTypeSpecific(prop.eRawType) << " oTmp = " << g_pTypeParser->GetDefaultValueSpecific(prop.eRawType)
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
                    if (g_pTypeParser->IsRawType(prop.eRawType)) {
                        ofs << indent << g_pTypeParser->GetRawTypeSpecific(prop.eRawType) << " oTmp = " << g_pTypeParser->GetDefaultValueSpecific(prop.eRawType)
                            << ";" << endl;
                        ofs << indent << "if (!DeSerialize_RawType(pszMsgData, Index, Length, oTmp))" << endl;
                    } else {
                        ofs << indent << prop.strType << " oTmp;" << endl;
                        ofs << indent << "if (!oTmp.DeSerialize_impl(pszMsgData, Index, Length))" << endl;
                    }

                    LEFT_BRACE;
                    ofs << indent << "return false;" << endl;
                    RIGHT_BRACE;
                    if (g_pTypeParser->IsRawType(prop.eRawType)) {
                        ofs << indent << prop.strName << "[oTmp] = 0;" << endl;
                    } else {
                        ofs << indent << prop.strName << "[oTmp." << prop.strKey << "] = oTmp;" << endl;
                    }

                    RIGHT_BRACE;
                }
            } else if (g_pTypeParser->IsRawType(prop.eRawType)) {
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

        // UAlloc UFree
        if (!its.bSingleton) {
            ofs << indent << "UniqsModel::" << its.strName << "* " << its.strName << "::UAlloc()" << endl;
            LEFT_BRACE;
            ofs << indent << "return g_pUniqsAllocator_" << proto.m_strName << "->UAlloc_" << its.strName << "();" << endl;
            RIGHT_BRACE;
            ofs << indent << "void " << its.strName << "::UFree(" << its.strName << "* p)" << endl;
            LEFT_BRACE;
            ofs << indent << "g_pUniqsAllocator_" << proto.m_strName << "->UFree_" << its.strName << "(p);" << endl;
            RIGHT_BRACE;
        }

        ofs << endl;

        // clear
#if 1
        ofs << indent << "void " << its.strName << "::Clear(bool bDestruct)" << endl;
        LEFT_BRACE;
        ofs << indent << "if (!bDestruct)" << endl;
        LEFT_BRACE;
        for (const auto &prop : its.vecProperties) {
            if (g_pTypeParser->IsComplexType(prop.eComplexType)) {
            } else if (g_pTypeParser->IsRawType(prop.eRawType)) {
                ofs << indent << prop.strName << " = " << g_pTypeParser->GetDefaultValueSpecific(prop.eRawType) << ";" << endl;
            }
        }
        RIGHT_BRACE;
        for (const auto &prop : its.vecProperties) {
            if (g_pTypeParser->IsComplexType(prop.eComplexType)) {
                if (prop.eComplexType == EUniqsComplexType_map) {
                    if (g_pTypeParser->IsRawType(prop.eRawType)) {
                        ofs << indent << prop.strName << ".clear();" << endl;
                    } else {
                        ofs << indent << "for (auto& it : " << prop.strName << ")" << endl;
                        LEFT_BRACE;
                        ofs << indent << "if (it.second) it.second->UFree(it.second);" << endl;
                        RIGHT_BRACE;
                        ofs << indent << prop.strName << ".clear();" << endl;
                    }
                } else {
                    ofs << indent << prop.strName << ".clear();" << endl;
                }
            } else if (g_pTypeParser->IsRawType(prop.eRawType)) {
            } else {
                ofs << indent << prop.strName << ".Clear(bDestruct);" << endl;
            }
        }
        RIGHT_BRACE;
#endif
        ofs << endl;

        if (proto.m_bGenProtobuf) {
            // FromPB
            ofs << indent << "void " << its.strName << "::FromPB(const google::protobuf::" << its.strName << "& pb, bool bSetDefault)" << endl;
            LEFT_BRACE;
            for (const auto &it : its.vecProperties) {
                if (!g_pTypeParser->IsComplexType(it.eComplexType)) {
                    if (g_pTypeParser->IsRawType(it.eRawType)) {
                        ofs << indent << "if (bSetDefault || pb.has_" << it.strName << "()) " << it.strName << " = pb." << it.strName << "(); " << endl;
                    } else {
                        ofs << indent << "if (bSetDefault || pb.has_" << it.strName << "()) " << it.strName << ".FromPB(pb." << it.strName << "(), bSetDefault);"
                            << endl;
                    }
                } else {
                    if (g_pTypeParser->IsRawType(it.eRawType)) {
                        ofs << indent << "for (const auto& it : pb." << it.strName << "()){ " << it.strName << "[it] = it; }" << endl;
                    } else {
                        ofs << indent << "for (const auto& it : pb." << it.strName << "())" << endl;
                        LEFT_BRACE;
                        ofs << indent << "auto p = " << it.strType << "::UAlloc();" << endl;
                        ofs << indent << "p->FromPB(it, bSetDefault);" << endl;
                        ofs << indent << "" << it.strName << "[p->" << it.strKey << "] = p;" << endl;
                        RIGHT_BRACE;
                    }
                }
            }
            RIGHT_BRACE;
            // ToPB
            ofs << indent << "void " << its.strName << "::ToPB(google::protobuf::" << its.strName << "& pb) const" << endl;
            LEFT_BRACE;
            for (const auto &it : its.vecProperties) {
                if (!g_pTypeParser->IsComplexType(it.eComplexType)) {
                    if (g_pTypeParser->IsRawType(it.eRawType)) {
                        ofs << indent << "pb.set_" << it.strName << "(" << it.strName << ");" << endl;
                    } else {
                        ofs << indent << "item.ToPB(*pb.mutable_" << it.strName << "());" << endl;
                    }
                } else {
                    if (g_pTypeParser->IsRawType(it.eRawType)) {
                        ofs << indent << "for (const auto& it : " << it.strName << "){ pb.mutable_" << it.strName << "()->Add(it.second); }" << endl;
                    } else {
                        ofs << indent << "for (const auto& it : " << it.strName << ") { it.second->ToPB(*pb.mutable_" << it.strName << "()->Add()); }" << endl;
                    }
                }
            }
            RIGHT_BRACE;
        }

        if (its.bSingleton) {
            ofs << indent << "" << its.strName << "* " << its.strName << "::m_pInstance = nullptr;" << endl;
        }

        RIGHT_BRACE;
    }

    return true;
}
