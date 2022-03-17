#include "generator_csharp.h"

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

bool Generator_csharp::Generate_i(UniqsProto &proto, const string &strTo, const string &strPBTo, std::string &strError) {
    if (!_GenerateDefines(proto, strTo, strError)) {
        return false;
    }
    // 类及嵌套类，多个文件
    if (!_GenerateStructs(proto, strTo, strError)) {
        return false;
    }

    return true;
}

bool Generator_csharp::_GenerateDefines(UniqsProto &proto, const string &strTo, std::string &strError) {
    Indent indent;

    string strToFile = strTo;
    string strDefineName = proto.m_strName + pszDefinePostfix;
    strToFile += strDefineName;
    strToFile += ".cs";
    std::ofstream ofs(strToFile.c_str());
    if (!ofs) {
        strError = "Generator_csharp::_GenerateDefines ofs create failed.";
        return false;
    }

    WriteUtf8BOMHead(ofs);

    ofs << endl;

    ofs << indent << "namespace Assets.scripts.Proto" << endl;
    LEFT_BRACE;
    ofs << indent << "public class " << strDefineName << endl;
    LEFT_BRACE;

    for (const auto &it : proto.vecDefines) {
        ofs << indent << "// " << it.strDescription << endl;
        ofs << indent << "public const int " << it.strName << " = " << it.nValue << "; " << endl;
        ofs << endl;
    }
    RIGHT_BRACE;

    RIGHT_BRACE;

    ofs << endl;

    return true;
}

bool Generator_csharp::_GenerateStructs(UniqsProto &proto, const string &strTo, std::string &strError) {
    string strDefineName = proto.m_strName + pszDefinePostfix;
    for (const auto &it : proto.vecStructs) {
        string strToFile = strTo;
        strToFile += it.strName;
        strToFile += ".cs";
        std::ofstream ofs(strToFile.c_str());
        if (!ofs) {
            strError = "Generator_csharp::Generate_i ofs create failed.";
            return false;
        }

        WriteUtf8BOMHead(ofs);
        WriteFileDeclarations(ofs);

        std::string strUsing =
            "using Assets.scripts.UniqsNet;\n"
            "using System;\n"
            "using System.Collections.Generic;\n"
            "using System.Linq;\n"
            "using System.Text;\n";
        ofs << strUsing << endl;
        ofs << "namespace Assets.scripts.Proto" << endl;
        Indent indent;
        LEFT_BRACE;

        ofs << indent << "// " << it.strDescription << endl;
        ofs << indent << "public class " << it.strName << " : MsgBase" << endl;
        LEFT_BRACE;

        // members
#if 1
        for (const auto &prop : it.vecProperties) {
            ofs << indent << "// " << prop.strDescription << endl;
            ofs << indent << "public ";
            if (g_pTypeParser->IsComplexType(prop.eComplexType)) {
                ofs << g_pTypeParser->GetComplexTypeSpecific(prop.eComplexType);
                if (prop.eComplexType == EUniqsComplexType_array) {
                    if (g_pTypeParser->IsRawType(prop.eRawType)) {
                        ofs << g_pTypeParser->GetRawTypeSpecific(prop.eRawType) << "> " << prop.strName << " = new ";
                        ofs << g_pTypeParser->GetComplexTypeSpecific(prop.eComplexType) << g_pTypeParser->GetRawTypeSpecific(prop.eRawType) << ">();" << endl;
                    } else {
                        ofs << prop.strType << "> " << prop.strName << " = new ";
                        ofs << g_pTypeParser->GetComplexTypeSpecific(prop.eComplexType) << prop.strType << ">();" << endl;
                    }
                } else if (prop.eComplexType == EUniqsComplexType_map) {
                    // rawtype的value统统int，默认0
                    if (g_pTypeParser->IsRawType(prop.eRawType)) {
                        ofs << g_pTypeParser->GetRawTypeSpecific(prop.eRawType) << ", int> " << prop.strName << " = new ";
                        ofs << g_pTypeParser->GetComplexTypeSpecific(prop.eComplexType) << g_pTypeParser->GetRawTypeSpecific(prop.eRawType) << ", int>();" << endl;
                    } else {
                        ofs << g_pTypeParser->GetRawTypeSpecific(prop.eRawType_KeyType) << ", " << prop.strType << "> " << prop.strName << " = new ";
                        ofs << g_pTypeParser->GetComplexTypeSpecific(prop.eComplexType) << g_pTypeParser->GetRawTypeSpecific(prop.eRawType_KeyType) << ", "
                            << prop.strType << ">(); " << endl;
                    }
                }
            } else if (g_pTypeParser->IsRawType(prop.eRawType)) {
                ofs << g_pTypeParser->GetRawTypeSpecific(prop.eRawType) << " " << prop.strName << " = " << g_pTypeParser->GetDefaultValueSpecific(prop.eRawType)
                    << ";" << endl;
            } else {
                ofs << prop.strType << " " << prop.strName << " = new " << prop.strType << "();" << endl;
            }
        }
        ofs << endl;
#endif

        // serialize
#if 1
        ofs << indent << "public override bool Serialize_impl(byte[] pszMsgData, ref int Index, int Length)" << endl;
        LEFT_BRACE;
        for (const auto &prop : it.vecProperties) {
            ofs << indent << "// serialize " << prop.strDescription << endl;

            if (g_pTypeParser->IsComplexType(prop.eComplexType)) {
                ofs << indent << "UInt16 wLen_" << prop.strName << " = (UInt16)(" << prop.strName << ".Count < " << strDefineName << "." << prop.strMax << " ? "
                    << prop.strName << ".Count : " << strDefineName << "." << prop.strMax << "); " << endl;
                ofs << indent << "if (!Serialize_RawType(pszMsgData, ref Index, Length, wLen_" << prop.strName << "))" << endl;
                LEFT_BRACE;
                ofs << indent << "return false;" << endl;
                RIGHT_BRACE;

                if (prop.eComplexType == EUniqsComplexType_array) {
                    ofs << indent << "for (int nIdx = 0; nIdx < " << prop.strName << ".Count() && nIdx < " << strDefineName << "." << prop.strMax << "; ++nIdx)"
                        << endl;
                    LEFT_BRACE;
                    if (g_pTypeParser->IsRawType(prop.eRawType)) {
                        ofs << indent << "if (!Serialize_RawType(pszMsgData, ref Index, Length, " << prop.strName << "[nIdx]))" << endl;
                        LEFT_BRACE;
                        ofs << indent << "return false;" << endl;
                        RIGHT_BRACE;
                    } else {
                        ofs << indent << "if (!" << prop.strName << "[nIdx].Serialize_impl(pszMsgData, ref Index, Length))" << endl;
                        LEFT_BRACE;
                        ofs << indent << "return false;" << endl;
                        RIGHT_BRACE;
                    }
                    RIGHT_BRACE;
                } else if (prop.eComplexType == EUniqsComplexType_map) {
                    ofs << indent << "int nLen_" << prop.strName << " = 0;" << endl;
                    ofs << indent << "foreach (var it in " << prop.strName << ")" << endl;
                    LEFT_BRACE;

                    ofs << indent << "if (++nLen_" << prop.strName << " > " << strDefineName << "." << prop.strMax << ")" << endl;
                    LEFT_BRACE;
                    ofs << indent << "return false;" << endl;
                    RIGHT_BRACE;

                    if (g_pTypeParser->IsRawType(prop.eRawType)) {
                        ofs << indent << "if (!Serialize_RawType(pszMsgData, ref Index, Length, it.Key))" << endl;
                    } else {
                        ofs << indent << "if (!it.Value.Serialize_impl(pszMsgData, ref Index, Length))" << endl;
                    }

                    LEFT_BRACE;
                    ofs << indent << "return false;" << endl;
                    RIGHT_BRACE;

                    RIGHT_BRACE;
                }
            } else if (g_pTypeParser->IsRawType(prop.eRawType)) {
                ofs << indent << "if (!Serialize_RawType(pszMsgData, ref Index, Length, " << prop.strName << "))" << endl;
                LEFT_BRACE;
                ofs << indent << "return false;" << endl;
                RIGHT_BRACE;
            } else {
                ofs << indent << "if (!" << prop.strName << ".Serialize_impl(pszMsgData, ref Index, Length))" << endl;
                LEFT_BRACE;
                ofs << indent << "return false;" << endl;
                RIGHT_BRACE;
            }
        }
        ofs << indent << "return true;" << endl;
        RIGHT_BRACE;

        ofs << endl;
#endif

        // deserialize
#if 1
        ofs << indent << "public override bool DeSerialize_impl(byte[] pszMsgData, ref int Index, int Length)" << endl;
        LEFT_BRACE;
        for (const auto &prop : it.vecProperties) {
            ofs << indent << "// deserialize " << prop.strDescription << endl;
            if (g_pTypeParser->IsComplexType(prop.eComplexType)) {
                ofs << indent << GetComplexLenType() << " wLen_" << prop.strName << " = 0;" << endl;
                ofs << indent << "if (!DeSerialize_RawType(pszMsgData, ref Index, Length, ref wLen_" << prop.strName << "))" << endl;
                LEFT_BRACE;
                ofs << indent << "return false;" << endl;
                RIGHT_BRACE;
                ofs << indent << "if (wLen_" << prop.strName << " > (" << GetComplexLenType() << ")" << strDefineName << "." << prop.strMax << ")" << endl;
                LEFT_BRACE;
                ofs << indent << "wLen_" << prop.strName << " = (" << GetComplexLenType() << ")" << strDefineName << "." << prop.strMax << ";" << endl;
                RIGHT_BRACE;

                if (prop.eComplexType == EUniqsComplexType_array) {
                    ofs << indent << "for (int nIdx = 0; nIdx < wLen_" << prop.strName << "; ++nIdx)" << endl;
                    LEFT_BRACE;
                    if (g_pTypeParser->IsRawType(prop.eRawType)) {
                        ofs << indent << g_pTypeParser->GetRawTypeSpecific(prop.eRawType) << " oTmp = " << g_pTypeParser->GetDefaultValueSpecific(prop.eRawType)
                            << ";" << endl;
                        ofs << indent << "if (!DeSerialize_RawType(pszMsgData, ref Index, Length, ref oTmp))" << endl;
                    } else {
                        ofs << indent << prop.strType << " oTmp = new " << prop.strType << "();" << endl;
                        ofs << indent << "if (!oTmp.DeSerialize_impl(pszMsgData, ref Index, Length))" << endl;
                    }

                    LEFT_BRACE;
                    ofs << indent << "return false;" << endl;
                    RIGHT_BRACE;
                    ofs << indent << prop.strName << ".Add(oTmp);" << endl;

                    RIGHT_BRACE;
                } else if (prop.eComplexType == EUniqsComplexType_map) {
                    ofs << indent << "for (int nIdx = 0; nIdx < wLen_" << prop.strName << "; ++nIdx)" << endl;
                    LEFT_BRACE;
                    if (g_pTypeParser->IsRawType(prop.eRawType)) {
                        ofs << indent << g_pTypeParser->GetRawTypeSpecific(prop.eRawType) << " oTmp = " << g_pTypeParser->GetDefaultValueSpecific(prop.eRawType)
                            << ";" << endl;
                        ofs << indent << "if (!DeSerialize_RawType(pszMsgData, ref Index, Length, ref oTmp))" << endl;
                    } else {
                        ofs << indent << prop.strType << " oTmp = new " << prop.strType << "();" << endl;
                        ofs << indent << "if (!oTmp.DeSerialize_impl(pszMsgData, ref Index, Length))" << endl;
                    }

                    LEFT_BRACE;
                    ofs << indent << "return false;" << endl;
                    RIGHT_BRACE;
                    if (g_pTypeParser->IsRawType(prop.eRawType)) {
                        ofs << indent << prop.strName << ".Add(oTmp, 0);" << endl;
                    } else {
                        ofs << indent << prop.strName << ".Add(oTmp." << prop.strKey << ", oTmp);" << endl;
                    }

                    RIGHT_BRACE;
                }
            } else if (g_pTypeParser->IsRawType(prop.eRawType)) {
                ofs << indent << "if (!DeSerialize_RawType(pszMsgData, ref Index, Length, ref " << prop.strName << "))" << endl;
                LEFT_BRACE;
                ofs << indent << "return false;" << endl;
                RIGHT_BRACE;
            } else {
                ofs << indent << "if (!" << prop.strName << ".DeSerialize_impl(pszMsgData, ref Index, Length))" << endl;
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
        ofs << indent << "public override void Clear()" << endl;
        LEFT_BRACE;
        for (const auto &prop : it.vecProperties) {
            if (g_pTypeParser->IsComplexType(prop.eComplexType)) {
                ofs << indent << prop.strName << ".Clear(bDestruct);" << endl;
            } else if (g_pTypeParser->IsRawType(prop.eRawType)) {
                ofs << indent << prop.strName << " = " << g_pTypeParser->GetDefaultValueSpecific(prop.eRawType) << ";" << endl;
            } else {
                ofs << indent << prop.strName << ".Clear(bDestruct);" << endl;
            }
        }
        RIGHT_BRACE;
#endif

        RIGHT_BRACE;
        RIGHT_BRACE;
    }

    return true;
}

bool Generator_csharp::_GenerateMsgMapper(UniqsProto &proto, const string &strTo, std::string &strError) {
    Indent indent;

    string strToFile = strTo;
    string strMsgMapperName = proto.m_strName + "MsgMapper";

    string strEnumMsgName = "E" + proto.m_strName + "Msg";
    strToFile += strMsgMapperName;
    strToFile += ".cs";
    std::ofstream ofs(strToFile.c_str());
    if (!ofs) {
        strError = "Generator_csharp::Generate_i ofs create failed.";
        return false;
    }

    WriteUtf8BOMHead(ofs);
    WriteFileDeclarations(ofs);

    ofs << indent
        << "using Assets.scripts.MsgHandlers;\n"
           "using Assets.scripts.UniqsNet;\n"
           "using System;\n"
           "using System.Collections.Generic;\n"
           "using System.Linq;\n"
           "using System.Text;\n"
           "using UnityEngine; \n"
           "namespace Assets.scripts.Proto"
        << endl;

    LEFT_BRACE;

    RIGHT_BRACE;

    return true;
}
