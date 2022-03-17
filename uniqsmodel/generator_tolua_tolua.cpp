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

bool Generator_tolua::_Generate_tolua_header(UniqsProto &proto, const string &strTo, std::string &strError) {
    string strToFile = strTo;
    strToFile += "tolua_" + proto.m_strName + ".h";

    std::ofstream ofs(strToFile.c_str());
    if (!ofs) {
        strError = "Generator_tolua::_Generate_tolua_header ofs create failed.";
        return false;
    }

    WriteUtf8BOMHead(ofs);
    WriteFileDeclarations(ofs);

    std::string strPragmaAndInclude = "#pragma once";
    ofs << strPragmaAndInclude << endl;

    ofs << "TOLUA_API int tolua_";
    ofs << proto.m_strName;

    ofs << "_open (lua_State* tolua_S);" << endl;

    return true;
}

bool Generator_tolua::_Generate_tolua_source(UniqsProto &proto, const string &strTo, std::string &strError) {
    string strToFile = strTo;
    strToFile += "tolua_" + proto.m_strName + ".cpp";

    std::ofstream ofs(strToFile.c_str());
    if (!ofs) {
        strError = "Generator_tolua::_Generate_tolua_source ofs create failed.";
        return false;
    }

    WriteUtf8BOMHead(ofs);

    // 第一大块，杂事、include及函数声明（函数声明可以直接换成包含头文件）
    WriteFileDeclarations(ofs);

    Indent indent;

    IFDEFCPLUSPLUS_BEGIN;
    ofs << "#include \"stdlib.h\"" << endl;
    EIF;
    ofs << "#include \"string.h\"" << endl;
    ofs << endl;
    ofs << "#include \"tolua++.h\"" << endl;
    ofs << endl;
    ofs << "#include \"tolua_" + proto.m_strName + ".h\"" << endl;
    ofs << endl;

    for (const auto &it : proto.vecIncludes) {
        ofs << "// " << it.strDescription << endl;
        ofs << "#include \"" << it.strName << "\"" << endl;
    }

    ofs << "#include<unordered_map>" << endl;
    ofs << "using std::unordered_map;" << endl;

    for (const auto &its : proto.vecStructs) {
        ofs << indent << "#include \"" << its.strName << ".h\"" << endl;
    }
    ofs << indent << "using " << g_strNamespace << ";" << endl;

    ofs << endl;
    ofs << endl;

    // 第二大块，tolua_collect_ ***，lua的回收函数
    ofs << indent << "// function to release collected object via destructor" << endl;
    IFDEFCPLUSPLUS_BEGIN;
    // 第二大块.1 所有STL容器的展开（暂时只支持VECTOR/MAP，下同）
    {
        unordered_set<string> ussConflict;
        for (const auto &its : proto.vecStructs) {
            for (const auto &it : its.vecProperties) {
                // 防止搞多个
                string strTypeExpand = _GetToLuaComplexTypeExpand(it);
                if (ussConflict.find(strTypeExpand) != ussConflict.end()) {
                    continue;
                }
                ussConflict.insert(strTypeExpand);
                // 第一版先只支持map，下同
                if (it.eComplexType == EUniqsComplexType_map) {
                    ofs << indent << "static int tolua_collect_" << _GetToLuaComplexTypeExpand(it) << "(lua_State* tolua_S)" << endl;
                    LEFT_BRACE;
                    ofs << indent << _GetToLuaComplexTypeTemplate(it) << "* self = (" << _GetToLuaComplexTypeTemplate(it) << "*) tolua_tousertype(tolua_S, 1, 0); "
                        << endl;
                    ofs << indent << "Mtolua_delete(self);" << endl;
                    ofs << indent << "return 0;" << endl;
                    RIGHT_BRACE;
                    ofs << indent << endl;
                }
            }
        }
    }
    // 第二大块.2 tolua_collect_自定义类
    for (const auto &its : proto.vecStructs) {
        ofs << indent << "static int tolua_collect_" << its.strName << "(lua_State* tolua_S)" << endl;
        LEFT_BRACE;
        ofs << indent << its.strName << "* self = (" << its.strName << "*) tolua_tousertype(tolua_S, 1, 0); " << endl;
        ofs << indent << "Mtolua_delete(self);" << endl;
        ofs << indent << "return 0;" << endl;
        RIGHT_BRACE;
        ofs << indent << endl;
    }

    EIF;
    ofs << endl;
    ofs << endl;

    // 第三大块，tolua_usertype 类型注册
    ofs << indent << "// function to register type" << endl;
    ofs << indent << "static void tolua_reg_types (lua_State* tolua_S)" << endl;
    LEFT_BRACE;
    for (const auto &its : proto.vecStructs) {
        ofs << indent << "tolua_usertype(tolua_S,\"" << its.strName << "\");" << endl;
    }
    for (const auto &its : proto.vecStructs) {
        for (const auto &it : its.vecProperties) {
            // 第一版先只支持map，下同
            if (it.eComplexType == EUniqsComplexType_map || it.eComplexType == EUniqsComplexType_array) {
                ofs << indent << "tolua_usertype(tolua_S,\"" << _GetToLuaComplexTypeTemplate(it) << "\");" << endl;
            }
        }
    }
    RIGHT_BRACE;
    ofs << endl;
    ofs << endl;

    // 第四大块，一些全局函数，如果有多个只用实现一个即可

    // 第五大块，各类的 成员变量、成员函数 的实现体
    // 5.1类的成员函数，这里主要就UAlloc
    for (const auto &its : proto.vecStructs) {
        const std::string &lsname = its.strName;
        const std::string &sname = its.strName;
        if (its.bSingleton) {
            string strModuleClassName = proto.m_strName + "_" + lsname;
            // Instance
            ofs << indent << "// method: Instance of class  " << sname << "" << endl;
            ofs << indent << "static int tolua_" << strModuleClassName << "_Instance00(lua_State* tolua_S)" << endl;
            LEFT_BRACE;
            IFNDEFTOLUARELEASE;
            ofs << indent << "tolua_Error tolua_err;" << endl;
            ofs << indent << "if (" << endl;
            ofs << indent << "!tolua_isusertable(tolua_S,1,\"" << sname << "\",0,&tolua_err) ||" << endl;
            ofs << indent << "!tolua_isnoobj(tolua_S,2,&tolua_err)" << endl;
            ofs << indent << ")" << endl;
            ofs << indent << "goto tolua_lerror;" << endl;
            ofs << indent << "else" << endl;
            EIF;
            LEFT_BRACE;
            ofs << indent << "" << sname << "* tolua_ret = (" << sname << "*)" << sname << "::Instance();" << endl;
            ofs << indent << "tolua_pushusertype(tolua_S, (void*)tolua_ret, \"" << sname << "\");" << endl;
            RIGHT_BRACE;
            ofs << indent << "return 1;" << endl;
            IFNDEFTOLUARELEASE;
            ofs << indent << "tolua_lerror :" << endl;
            ofs << indent << "tolua_error(tolua_S, \"#ferror in function 'Instance'.\", &tolua_err);" << endl;
            ofs << indent << "return 0;" << endl;
            EIF;
            RIGHT_BRACE;
            ofs << endl;
            // CreateInstance
            ofs << indent << "// method: CreateInstance of class  " << sname << "" << endl;
            ofs << indent << "static int tolua_" << strModuleClassName << "_CreateInstance00(lua_State* tolua_S)" << endl;
            LEFT_BRACE;
            IFNDEFTOLUARELEASE;
            ofs << indent << "tolua_Error tolua_err;" << endl;
            ofs << indent << "if (" << endl;
            ofs << indent << "!tolua_isusertable(tolua_S,1,\"" << sname << "\",0,&tolua_err) ||" << endl;
            ofs << indent << "!tolua_isnoobj(tolua_S,2,&tolua_err)" << endl;
            ofs << indent << ")" << endl;
            ofs << indent << "goto tolua_lerror;" << endl;
            ofs << indent << "else" << endl;
            EIF;
            LEFT_BRACE;
            ofs << indent << "" << sname << "::CreateInstance();" << endl;
            RIGHT_BRACE;
            ofs << indent << "return 0;" << endl;
            IFNDEFTOLUARELEASE;
            ofs << indent << "tolua_lerror :" << endl;
            ofs << indent << "tolua_error(tolua_S, \"#ferror in function 'CreateInstance'.\", &tolua_err);" << endl;
            ofs << indent << "return 0;" << endl;
            EIF;
            RIGHT_BRACE;
            ofs << endl;
            // ReleaseInstance
            ofs << indent << "// method: ReleaseInstance of class  " << sname << "" << endl;
            ofs << indent << "static int tolua_" << strModuleClassName << "_ReleaseInstance00(lua_State* tolua_S)" << endl;
            LEFT_BRACE;
            IFNDEFTOLUARELEASE;
            ofs << indent << "tolua_Error tolua_err;" << endl;
            ofs << indent << "if (" << endl;
            ofs << indent << "!tolua_isusertable(tolua_S,1,\"" << sname << "\",0,&tolua_err) ||" << endl;
            ofs << indent << "!tolua_isnoobj(tolua_S,2,&tolua_err)" << endl;
            ofs << indent << ")" << endl;
            ofs << indent << "goto tolua_lerror;" << endl;
            ofs << indent << "else" << endl;
            EIF;
            LEFT_BRACE;
            ofs << indent << "" << sname << "::ReleaseInstance();" << endl;
            RIGHT_BRACE;
            ofs << indent << "return 0;" << endl;
            IFNDEFTOLUARELEASE;
            ofs << indent << "tolua_lerror :" << endl;
            ofs << indent << "tolua_error(tolua_S, \"#ferror in function 'ReleaseInstance'.\", &tolua_err);" << endl;
            ofs << indent << "return 0;" << endl;
            EIF;
            RIGHT_BRACE;
            ofs << endl;
        }
        if (!its.bSingleton) {
            // UAlloc
            ofs << indent << "static int tolua_" << proto.m_strName << "_" << sname << "_UAlloc00(lua_State* tolua_S)" << endl;
            LEFT_BRACE;
            IFNDEFTOLUARELEASE;
            ofs << indent << "tolua_Error tolua_err;" << endl;
            ofs << indent << "if (" << endl;
            ofs << indent << "!tolua_isusertable(tolua_S,1,\"" << sname << "\",0,&tolua_err) ||" << endl;
            ofs << indent << "!tolua_isnoobj(tolua_S,2,&tolua_err)" << endl;
            ofs << indent << ")" << endl;
            ofs << indent << "goto tolua_lerror;" << endl;
            ofs << indent << "else" << endl;
            EIF;
            LEFT_BRACE;
            ofs << indent << "" << sname << "* tolua_ret = (" << sname << "*)" << sname << "::UAlloc();" << endl;
            ofs << indent << "tolua_pushusertype(tolua_S, (void*)tolua_ret, \"" << sname << "\");" << endl;
            RIGHT_BRACE;
            ofs << indent << "return 1;" << endl;
            IFNDEFTOLUARELEASE;
            ofs << indent << "tolua_lerror :" << endl;
            ofs << indent << "tolua_error(tolua_S, \"#ferror in function 'UAlloc'.\", &tolua_err);" << endl;
            ofs << indent << "return 0;" << endl;
            EIF;
            RIGHT_BRACE;
            // UFree
            ofs << indent << "static int tolua_" << proto.m_strName << "_" << sname << "_UFree00(lua_State* tolua_S)" << endl;
            LEFT_BRACE;
            IFNDEFTOLUARELEASE;
            ofs << indent << "tolua_Error tolua_err;" << endl;
            ofs << indent << "if (" << endl;
            ofs << indent << "!tolua_isusertable(tolua_S,1,\"" << sname << "\",0,&tolua_err) ||" << endl;
            ofs << indent << "!tolua_isusertable(tolua_S,2,\"" << sname << "\",0,&tolua_err) ||" << endl;
            ofs << indent << "!tolua_isnoobj(tolua_S,3,&tolua_err)" << endl;
            ofs << indent << ")" << endl;
            ofs << indent << "goto tolua_lerror;" << endl;
            ofs << indent << "else" << endl;
            EIF;
            LEFT_BRACE;
            ofs << indent << "" << sname << "* p = (" << sname << "*)tolua_tousertype(tolua_S,2,0);" << endl;
            ofs << indent << "" << sname << "::UFree(p);" << endl;
            RIGHT_BRACE;
            ofs << indent << "return 1;" << endl;
            IFNDEFTOLUARELEASE;
            ofs << indent << "tolua_lerror :" << endl;
            ofs << indent << "tolua_error(tolua_S, \"#ferror in function 'UFree'.\", &tolua_err);" << endl;
            ofs << indent << "return 0;" << endl;
            EIF;
            RIGHT_BRACE;
        }
    }
    // 5.2 类成员get set
    for (const auto &its : proto.vecStructs) {
        const std::string &sname = its.strName;
        // variable
        for (const auto &it : its.vecProperties) {
            // set
#if 1
            const std::string &vname = it.strName;
            string s_vname = sname + "_" + vname;
            ofs << indent << "static int tolua_set_" << s_vname << "(lua_State* tolua_S)" << endl;
            LEFT_BRACE;
            ofs << indent << sname << "* self = (" << sname << "*)tolua_tousertype(tolua_S, 1, 0); " << endl;
            IFNDEFTOLUARELEASE;
            ofs << indent << "tolua_Error tolua_err;" << endl;
            ofs << indent << "if (!self) tolua_error(tolua_S,\"invalid 'self' in accessing variable '" << vname << "' class: " << sname << "\",NULL);" << endl;
            if (!g_pTypeParser->IsComplexType(it.eComplexType)) {
                if (g_pTypeParser->IsRawType(it.eRawType)) {
                    ofs << indent << "if (!tolua_is" << _tolua_typename(it.eRawType) << "(tolua_S,2,0,&tolua_err))" << endl;
                } else {
                    ofs << indent << "if ((tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,\"" << it.strType << "\",0,&tolua_err)))" << endl;
                }
            } else {
                ofs << indent << "if ((tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,\"" << _GetToLuaComplexTypeTemplate(it)
                    << "\",0,&tolua_err)))" << endl;
            }
            ofs << indent << "tolua_error(tolua_S,\"#vinvalid type in variable assignment.\",&tolua_err);" << endl;
            EIF;
            if (!g_pTypeParser->IsComplexType(it.eComplexType)) {
                if (g_pTypeParser->IsRawType(it.eRawType)) {
                    if (it.eRawType == EUniqsRawType_string) {
                        ofs << indent << "self->" << vname << " = tolua_to" << _tolua_typename(it.eRawType) << "(tolua_S,2,0);" << endl;
                    } else {
                        ofs << indent << "self->" << vname << " = (" << g_pTypeParser->GetRawTypeSpecific(it.eRawType) << ")tolua_to" << _tolua_typename(it.eRawType)
                            << "(tolua_S,2,0);" << endl;
                    }
                } else {
                    ofs << indent << "self->" << vname << " = *((" << it.strType << "*)tolua_tousertype(tolua_S, 2, 0));" << endl;
                }
            } else if (it.eComplexType == EUniqsComplexType_map) {
                ofs << indent << "self->" << vname << " = *((" << _GetToLuaComplexTypeTemplate(it) << "*)tolua_tousertype(tolua_S,2,0));" << endl;
            } else if (it.eComplexType == EUniqsComplexType_array) {
                ofs << indent << "self->" << vname << " = *((" << _GetToLuaComplexTypeTemplate(it) << "*)tolua_tousertype(tolua_S,2,0));" << endl;
            } else {
                strError = "tolua generate [5.2 类成员get set][set] not supported type";
                return false;
            }
            ofs << indent << "return 0;" << endl;
            RIGHT_BRACE;
#endif

            // get
#if 1
            ofs << indent << "static int tolua_get_" << s_vname << "(lua_State* tolua_S)" << endl;
            LEFT_BRACE;
            ofs << indent << sname << "* self = (" << sname << "*)tolua_tousertype(tolua_S, 1, 0); " << endl;
            IFNDEFTOLUARELEASE;
            ofs << indent << "if (!self) tolua_error(tolua_S,\"invalid 'self' in accessing variable '" << vname << "' class: " << sname << "\",NULL);" << endl;
            EIF;
            if (!g_pTypeParser->IsComplexType(it.eComplexType)) {
                if (g_pTypeParser->IsRawType(it.eRawType)) {
                    if (it.eRawType == EUniqsRawType_string || it.eRawType == EUniqsRawType_int64 || it.eRawType == EUniqsRawType_uint64) {
                        ofs << indent << "tolua_push" << _tolua_typename(it.eRawType) << "(tolua_S, self->" << vname << ");" << endl;
                    } else {
                        ofs << indent << "tolua_push" << _tolua_typename(it.eRawType) << "(tolua_S, (lua_Number)self->" << vname << ");" << endl;
                    }
                } else {
                    ofs << indent << "tolua_pushusertype(tolua_S,(void*)&self->" << vname << ",\"" << it.strType << "\");" << endl;
                }
            } else {
                if (it.eComplexType == EUniqsComplexType_map || it.eComplexType == EUniqsComplexType_array) {
                    ofs << indent << "tolua_pushusertype(tolua_S,(void*)&self->" << it.strName << ",\"" << _GetToLuaComplexTypeTemplate(it) << "\");" << endl;
                } else {
                    strError = "tolua generate [5.2 类成员get set][get] not supported type";
                    return false;
                }
            }
            ofs << indent << "return 1;" << endl;
            RIGHT_BRACE;
#endif
        }
    }

    // 5.3 STL容器

    // 5.3.1 STL容器.map get_element set_element has_element
    {
        unordered_set<string> ussConflict;
        for (const auto &its : proto.vecStructs) {
            for (const auto &it : its.vecProperties) {
                const std::string &lsname = _GetToLuaComplexTypeExpand(it);
                const std::string &sname = _GetToLuaComplexTypeTemplate(it);
                if (ussConflict.find(sname) != ussConflict.end()) {
                    continue;
                }
                ussConflict.insert(sname);
                // 第一版先只支持map，下同
                if (it.eComplexType == EUniqsComplexType_map) {
                    const char *pszKeyTypeTolua = _tolua_typename(it.eRawType_KeyType);
                    const char *pszKeyType = g_pTypeParser->GetRawTypeSpecific(it.eRawType_KeyType);
                    bool isValueRawType = g_pTypeParser->IsRawType(it.eRawType);
                    const char *pszValueTypeRaw = g_pTypeParser->GetRawTypeSpecific(it.eRawType);

                    // get_element
                    ofs << indent << "static int tolua_" << proto.m_strName << "_" << lsname << "_get_element00(lua_State* tolua_S)" << endl;
                    LEFT_BRACE;
                    IFNDEFTOLUARELEASE;
                    ofs << indent << "tolua_Error tolua_err;" << endl;
                    ofs << indent << "if (" << endl;
                    ofs << indent << "!tolua_isusertype(tolua_S,1,\"" << sname << "\",0,&tolua_err) ||" << endl;
                    ofs << indent << "!tolua_is" << pszKeyTypeTolua << "(tolua_S, 2, 0, &tolua_err) || " << endl;
                    ofs << indent << "!tolua_isnoobj(tolua_S,3,&tolua_err)" << endl;
                    ofs << indent << ")" << endl;
                    ofs << indent << "goto tolua_lerror;" << endl;
                    ofs << indent << "else" << endl;
                    EIF;
                    LEFT_BRACE;
                    ofs << indent << "" << sname << "* self = (" << sname << "*)  tolua_tousertype(tolua_S, 1, 0);" << endl;
                    ofs << indent << "" << pszKeyType << " key = ((" << pszKeyType << ")tolua_to" << pszKeyTypeTolua << "(tolua_S, 2, 0));" << endl;
                    IFNDEFTOLUARELEASE;
                    ofs << indent << "if (!self) tolua_error(tolua_S, \"invalid 'self' in function 'get_element' type:" << sname << " \", NULL);" << endl;
                    EIF;
                    ofs << indent << "auto it = self->find(key);" << endl;
                    ofs << indent << "if (it != self->end())" << endl;
                    LEFT_BRACE;
                    if (isValueRawType) {
                        if (it.eRawType == EUniqsRawType_string || it.eRawType == EUniqsRawType_uint64 || it.eRawType == EUniqsRawType_int64) {
                            ofs << indent << "tolua_push" << _tolua_typename(it.eRawType) << "(tolua_S, it->second);" << endl;
                        } else {
                            ofs << indent << "tolua_push" << _tolua_typename(it.eRawType) << "(tolua_S, (lua_Number)it->second);" << endl;
                        }
                    } else {
                        ofs << indent << "tolua_pushusertype(tolua_S, (void*)it->second, \"" << it.strType << "\");" << endl;
                    }
                    RIGHT_BRACE;
                    ofs << indent << "else" << endl;
                    LEFT_BRACE;
                    if (isValueRawType) {
                        if (it.eRawType == EUniqsRawType_string) {
                            ofs << indent << "tolua_pushcppstring(tolua_S, string());" << endl;
                        } else if (it.eRawType == EUniqsRawType_uint64 || it.eRawType == EUniqsRawType_int64) {
                            ofs << indent << "tolua_push" << _tolua_typename(it.eRawType) << "(tolua_S, 0);" << endl;
                        } else {
                            ofs << indent << "tolua_push" << _tolua_typename(it.eRawType) << "(tolua_S, (lua_Number)0);" << endl;
                        }
                    } else {
                        ofs << indent << "tolua_pushusertype(tolua_S, (void*)NULL, \"" << it.strType << "\");" << endl;
                    }
                    RIGHT_BRACE;

                    RIGHT_BRACE;
                    ofs << indent << "return 1;" << endl;
                    IFNDEFTOLUARELEASE;
                    ofs << "tolua_lerror:" << endl;
                    ofs << indent << "tolua_error(tolua_S, \"#ferror in function 'get_element' type:" << sname << " .\", &tolua_err);" << endl;
                    ofs << indent << "return 0;" << endl;
                    EIF;
                    RIGHT_BRACE;

                    // set_element
                    ofs << indent << "static int tolua_" << proto.m_strName << "_" << lsname << "_set_element00(lua_State* tolua_S)" << endl;
                    LEFT_BRACE;
                    IFNDEFTOLUARELEASE;
                    ofs << indent << "tolua_Error tolua_err;" << endl;
                    ofs << indent << "if (" << endl;
                    ofs << indent << "!tolua_isusertype(tolua_S,1,\"" << sname << "\",0,&tolua_err) ||" << endl;
                    ofs << indent << "!tolua_is" << pszKeyTypeTolua << "(tolua_S, 2, 0, &tolua_err) || " << endl;
                    // set需要判定value的类型
                    if (isValueRawType) {
                        ofs << indent << "!tolua_is" << _tolua_typename(it.eRawType) << "(tolua_S,3,0,&tolua_err) ||" << endl;
                    } else {
                        ofs << indent << "!tolua_isusertype(tolua_S,3,\"" << it.strType << "\",0,&tolua_err) ||" << endl;
                    }
                    ofs << indent << "!tolua_isnoobj(tolua_S,4,&tolua_err)" << endl;
                    ofs << indent << ")" << endl;
                    ofs << indent << "goto tolua_lerror;" << endl;
                    ofs << indent << "else" << endl;
                    EIF;
                    LEFT_BRACE;
                    ofs << indent << "" << sname << "* self = (" << sname << "*)  tolua_tousertype(tolua_S, 1, 0);" << endl;
                    IFNDEFTOLUARELEASE;
                    ofs << indent << "if (!self) tolua_error(tolua_S, \"invalid 'self' in function 'set_element' type:" << sname << " \", NULL);" << endl;
                    EIF;
                    ofs << indent << "" << pszKeyType << " key = ((" << pszKeyType << ")tolua_to" << pszKeyTypeTolua << "(tolua_S, 2, 0));" << endl;
                    if (isValueRawType) {
                        if (it.eRawType == EUniqsRawType_string) {
                            ofs << indent << "char* value = ((char*)tolua_tocppstring(tolua_S,3,0));" << endl;
                        } else if (it.eRawType == EUniqsRawType_uint64 || it.eRawType == EUniqsRawType_int64) {
                            ofs << indent << pszValueTypeRaw << " value = *((" << pszValueTypeRaw << "*)tolua_tousertype(tolua_S, 3, 0)); " << endl;
                        } else {
                            ofs << indent << pszValueTypeRaw << " value = ((" << pszValueTypeRaw << ")tolua_tonumber(tolua_S, 3, 0)); " << endl;
                        }
                    } else {
                        ofs << indent << "" << it.strType << "* value = ((" << it.strType << "*)tolua_tousertype(tolua_S,3,0));" << endl;
                    }
                    ofs << indent << "(*self)[key] = value;" << endl;
                    ofs << indent << "return 1;" << endl;
                    IFNDEFTOLUARELEASE;
                    ofs << "tolua_lerror:" << endl;
                    ofs << indent << "tolua_error(tolua_S, \"#ferror in function 'set_element'. type:" << sname << " \", &tolua_err);" << endl;
                    ofs << indent << "return 0;" << endl;
                    EIF;
                    RIGHT_BRACE;
                    RIGHT_BRACE;

                    // has_element
                    ofs << indent << "static int tolua_" << proto.m_strName << "_" << lsname << "_has_element00(lua_State* tolua_S)" << endl;
                    LEFT_BRACE;
                    IFNDEFTOLUARELEASE;
                    ofs << indent << "tolua_Error tolua_err;" << endl;
                    ofs << indent << "if (" << endl;
                    ofs << indent << "!tolua_isusertype(tolua_S,1,\"" << sname << "\",0,&tolua_err) ||" << endl;
                    ofs << indent << "!tolua_is" << pszKeyTypeTolua << "(tolua_S, 2, 0, &tolua_err) || " << endl;
                    ofs << indent << "!tolua_isnoobj(tolua_S,3,&tolua_err)" << endl;
                    ofs << indent << ")" << endl;
                    ofs << indent << "goto tolua_lerror;" << endl;
                    ofs << indent << "else" << endl;
                    EIF;
                    LEFT_BRACE;
                    ofs << indent << "" << sname << "* self = (" << sname << "*)  tolua_tousertype(tolua_S, 1, 0);" << endl;
                    ofs << indent << "" << pszKeyType << " key = ((" << pszKeyType << ")tolua_to" << pszKeyTypeTolua << "(tolua_S, 2, 0));" << endl;
                    IFNDEFTOLUARELEASE;
                    ofs << indent << "if (!self) tolua_error(tolua_S, \"invalid 'self' in function 'has_element' type:" << sname << " \", NULL);" << endl;
                    EIF;
                    ofs << indent << "bool tolua_ret = self->find(key) != self->end();" << endl;
                    ofs << indent << "tolua_pushboolean(tolua_S, (bool)tolua_ret);" << endl;
                    RIGHT_BRACE;
                    ofs << indent << "return 1;" << endl;
                    IFNDEFTOLUARELEASE;
                    ofs << "tolua_lerror:" << endl;
                    ofs << indent << "tolua_error(tolua_S, \"#ferror in function 'has_element' type:" << sname << " .\", &tolua_err);" << endl;
                    ofs << indent << "return 0;" << endl;
                    EIF;
                    RIGHT_BRACE;

                    // rem_element
                    ofs << indent << "static int tolua_" << proto.m_strName << "_" << lsname << "_rem_element00(lua_State* tolua_S)" << endl;
                    LEFT_BRACE;
                    IFNDEFTOLUARELEASE;
                    ofs << indent << "tolua_Error tolua_err;" << endl;
                    ofs << indent << "if (" << endl;
                    ofs << indent << "!tolua_isusertype(tolua_S,1,\"" << sname << "\",0,&tolua_err) ||" << endl;
                    ofs << indent << "!tolua_is" << pszKeyTypeTolua << "(tolua_S, 2, 0, &tolua_err) || " << endl;
                    ofs << indent << "!tolua_isnoobj(tolua_S,3,&tolua_err)" << endl;
                    ofs << indent << ")" << endl;
                    ofs << indent << "goto tolua_lerror;" << endl;
                    ofs << indent << "else" << endl;
                    EIF;
                    LEFT_BRACE;
                    ofs << indent << "" << sname << "* self = (" << sname << "*)  tolua_tousertype(tolua_S, 1, 0);" << endl;
                    ofs << indent << "" << pszKeyType << " key = ((" << pszKeyType << ")tolua_to" << pszKeyTypeTolua << "(tolua_S, 2, 0));" << endl;
                    IFNDEFTOLUARELEASE;
                    ofs << indent << "if (!self) tolua_error(tolua_S, \"invalid 'self' in function 'rem_element' type:" << sname << " \", NULL);" << endl;
                    EIF;
                    ofs << indent << "self->erase(key); return 1;" << endl;
                    RIGHT_BRACE;
                    IFNDEFTOLUARELEASE;
                    ofs << "tolua_lerror:" << endl;
                    ofs << indent << "tolua_error(tolua_S, \"#ferror in function 'rem_element' type:" << sname << " .\", &tolua_err);" << endl;
                    ofs << indent << "return 0;" << endl;
                    EIF;
                    RIGHT_BRACE;

                    // del_element
                    ofs << indent << "static int tolua_" << proto.m_strName << "_" << lsname << "_del_element00(lua_State* tolua_S)" << endl;
                    LEFT_BRACE;
                    IFNDEFTOLUARELEASE;
                    ofs << indent << "tolua_Error tolua_err;" << endl;
                    ofs << indent << "if (" << endl;
                    ofs << indent << "!tolua_isusertype(tolua_S,1,\"" << sname << "\",0,&tolua_err) ||" << endl;
                    ofs << indent << "!tolua_is" << pszKeyTypeTolua << "(tolua_S, 2, 0, &tolua_err) || " << endl;
                    ofs << indent << "!tolua_isnoobj(tolua_S,3,&tolua_err)" << endl;
                    ofs << indent << ")" << endl;
                    ofs << indent << "goto tolua_lerror;" << endl;
                    ofs << indent << "else" << endl;
                    EIF;
                    LEFT_BRACE;
                    ofs << indent << "" << sname << "* self = (" << sname << "*)  tolua_tousertype(tolua_S, 1, 0);" << endl;
                    ofs << indent << "" << pszKeyType << " key = ((" << pszKeyType << ")tolua_to" << pszKeyTypeTolua << "(tolua_S, 2, 0));" << endl;
                    IFNDEFTOLUARELEASE;
                    ofs << indent << "if (!self) tolua_error(tolua_S, \"invalid 'self' in function 'del_element' type:" << sname << " \", NULL);" << endl;
                    EIF;
                    ofs << indent << "auto it = self->find(key);" << endl;
                    ofs << indent << "if (it != self->end())" << endl;
                    LEFT_BRACE;
                    if (!g_pTypeParser->IsRawType(it.eRawType)) {
                        ofs << indent << "if (it->second) " << it.strType << "::UFree(it->second);" << endl;
                    }
                    ofs << indent << "self->erase(it);" << endl;
                    ofs << indent << "return 1;" << endl;
                    RIGHT_BRACE;
                    ofs << indent << "return 0;" << endl;
                    RIGHT_BRACE;
                    IFNDEFTOLUARELEASE;
                    ofs << "tolua_lerror:" << endl;
                    ofs << indent << "tolua_error(tolua_S, \"#ferror in function 'del_element' type:" << sname << " .\", &tolua_err);" << endl;
                    ofs << indent << "return 0;" << endl;
                    EIF;
                    RIGHT_BRACE;
                }
            }
        }
    }

    // 第六大块，总入口的实现体，也即所有上面函数的总的注册入口
    ofs << indent << "// Open function" << endl;
    ofs << indent << "TOLUA_API int tolua_" << proto.m_strName << "_open (lua_State* tolua_S)" << endl;
    LEFT_BRACE;
    ofs << indent << "tolua_open(tolua_S);" << endl;
    ofs << indent << "tolua_reg_types(tolua_S);" << endl;
    ofs << indent << "tolua_module(tolua_S,NULL,0);" << endl;
    ofs << indent << "tolua_beginmodule(tolua_S,NULL);" << endl;
    LEFT_BRACE;
    ofs << indent << "int ntolua_dobuffer_ret = 0;" << endl;
    // 全局函数
    // ofs << indent << "tolua_function(tolua_S,\"UFree\",tolua_tstl_UFree00);" << endl;
    // 6.1 自定义类
    {
        for (const auto &its : proto.vecStructs) {
            const std::string &lsname = its.strName;
            const std::string &sname = its.strName;
            IFDEFCPLUSPLUS_BEGIN;
            ofs << indent << "tolua_cclass(tolua_S,\"" << lsname << "\",\"" << sname << "\",\"\",tolua_collect_" << lsname << ");" << endl;
            ofs << "#else" << endl;
            ofs << indent << "tolua_cclass(tolua_S,\"" << lsname << "\",\"" << sname << "\",\"\",NULL);" << endl;
            EIF;
            ofs << indent << "tolua_beginmodule(tolua_S,\"" << lsname << "\");" << endl;
            LEFT_BRACE;
            // 6.1.1 function
            if (its.bSingleton) {
                ofs << indent << "tolua_function(tolua_S,\"Instance\",tolua_" << proto.m_strName << "_" << lsname << "_Instance00);" << endl;
                ofs << indent << "tolua_function(tolua_S,\"CreateInstance\",tolua_" << proto.m_strName << "_" << lsname << "_CreateInstance00);" << endl;
                ofs << indent << "tolua_function(tolua_S,\"ReleaseInstance\",tolua_" << proto.m_strName << "_" << lsname << "_ReleaseInstance00);" << endl;
            }
            if (!its.bSingleton) {
                ofs << indent << "tolua_function(tolua_S,\"UAlloc\",tolua_" << proto.m_strName << "_" << lsname << "_UAlloc00);" << endl;
                ofs << indent << "tolua_function(tolua_S,\"UFree\",tolua_" << proto.m_strName << "_" << lsname << "_UFree00);" << endl;
            }
            // 6.1.2 variable
            for (const auto &it : its.vecProperties) {
                const std::string &vname = it.strName;
                string s_vname = sname + "_" + vname;
                ofs << indent << "tolua_variable(tolua_S,\"" << vname << "\",tolua_get_" << s_vname << ",tolua_set_" << s_vname << ");" << endl;
            }
            RIGHT_BRACE;
            ofs << indent << "tolua_endmodule(tolua_S);" << endl;
            ofs << indent << endl;
        }
    }
    // 6.2 模板展开
    {
        unordered_set<string> ussConflict;
        for (const auto &its : proto.vecStructs) {
            for (const auto &it : its.vecProperties) {
                const std::string &lsname = _GetToLuaComplexTypeExpand(it);
                const std::string &sname = _GetToLuaComplexTypeTemplate(it);
                if (ussConflict.find(sname) != ussConflict.end()) {
                    continue;
                }
                ussConflict.insert(sname);

                // 第一版先只支持map，下同
                if (it.eComplexType == EUniqsComplexType_map) {
                    IFDEFCPLUSPLUS_BEGIN;
                    ofs << indent << "tolua_cclass(tolua_S,\"" << lsname << "\",\"" << sname << "\",\"\",tolua_collect_" << lsname << ");" << endl;
                    ofs << "#else" << endl;
                    ofs << indent << "tolua_cclass(tolua_S,\"" << lsname << "\",\"" << sname << "\",\"\",NULL);" << endl;
                    EIF;
                    ofs << indent << "tolua_beginmodule(tolua_S,\"" << lsname << "\");" << endl;
                    LEFT_BRACE;
                    ofs << indent << "tolua_function(tolua_S,\"get_element\",tolua_" << proto.m_strName << "_" << lsname << "_get_element00);" << endl;
                    ofs << indent << "tolua_function(tolua_S,\"set_element\",tolua_" << proto.m_strName << "_" << lsname << "_set_element00);" << endl;
                    ofs << indent << "tolua_function(tolua_S,\"has_element\",tolua_" << proto.m_strName << "_" << lsname << "_has_element00);" << endl;
                    ofs << indent << "tolua_function(tolua_S,\"rem_element\",tolua_" << proto.m_strName << "_" << lsname << "_rem_element00);" << endl;
                    ofs << indent << "tolua_function(tolua_S,\"del_element\",tolua_" << proto.m_strName << "_" << lsname << "_del_element00);" << endl;
                    RIGHT_BRACE;
                    ofs << indent << "tolua_endmodule(tolua_S);" << endl;
                    ofs << indent << endl;
                }
            }
        }
    }
    RIGHT_BRACE;
    ofs << indent << "tolua_endmodule(tolua_S);" << endl;

    ofs << indent << "return 1;" << endl;

    RIGHT_BRACE;

    return true;
}
