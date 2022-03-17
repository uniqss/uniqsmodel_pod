#include "uniqsmodel.h"

#include "stl.h"

#include "uniqs_constdefines.h"

#include "typeparser.h"

bool UniqsModel::ReadDef(const std::string& strFrom, std::string& strError) {
    pugi::xml_parse_result result = m_doc.load_file(strFrom.c_str());

    if (!result) {
        strError = result.description();
        strError += " file:";
        strError += strFrom;
        return false;
    }

    pugi::xml_node uniqsproto = m_doc.child(pszUniqsProto);
    if (!uniqsproto) {
        strError = pszUniqsProto;
        strError += " node not found.";
        return false;
    }

    m_strName = uniqsproto.attribute(pszName).as_string();
    if (m_strName.empty()) {
        strError = pszUniqsProto;
        strError += " ";
        strError += pszName;
        strError += " attribute not found.";
        return false;
    }

    m_strProtobufNamespace = uniqsproto.attribute(pszProtobufNamespace).as_string("");
    m_bGenProtobuf = uniqsproto.attribute(pszGenProtobuf).as_bool(true);

    m_strHead = uniqsproto.attribute(pszHead).as_string();

    m_strDescription = uniqsproto.attribute(pszDescription).as_string();

    pugi::xml_node includes = uniqsproto.child(pszIncludes);
    if (includes) {
        if (!ReadDef_includes(includes, strError)) {
            return false;
        }
    }

    pugi::xml_node defines = uniqsproto.child(pszDefines);
    if (defines) {
        if (!ReadDef_defines(defines, strError)) {
            return false;
        }
    }

    pugi::xml_node structs = uniqsproto.child(pszStructs);
    if (structs) {
        if (!ReadDef_structs(structs, strError)) {
            return false;
        }
    }

    m_strPath = strFrom;
    auto pos1 = m_strPath.find_last_of('/');
    auto pos2 = m_strPath.find_last_of('\\');
    // 注意不要把\和/混用，否则可能会找不到正确的路径
    if (pos1 != std::string::npos) {
        m_strPath = m_strPath.substr(0, pos1 + 1);
    } else if (pos2 != std::string::npos) {
        m_strPath = m_strPath.substr(0, pos2 + 1);
    } else {
        // 如果找不到，说明可执行文件和配置文件在同一个目录，直接./即可
        m_strPath = "./";
    }

    return true;
}

bool UniqsModel::ReadDef_includes(pugi::xml_node& node, std::string& strError) {
    const auto& includes = node.children(pszInclude);
    std::string strPrevElement = "";
    int nIdx = 0;
    for (auto it = includes.begin(); it != includes.end(); ++it) {
        Uniqs_Include oInclude;
        oInclude.strName = it->attribute(pszName).as_string();
        if (oInclude.strName.empty()) {
            strError = pszInclude;
            strError += " ";
            strError += pszName;
            strError += " attribute not found.";
            if (!strPrevElement.empty()) {
                strError += "prev element:";
                strError += strPrevElement;
            }
            return false;
        }
        if (mapIncludes.find(oInclude.strName) != mapIncludes.end()) {
            strError = pszInclude;
            strError += " ";
            strError += oInclude.strName;
            strError += " duplicated.";
            if (!strPrevElement.empty()) {
                strError += "prev element:";
                strError += strPrevElement;
            }
            return false;
        }
        mapIncludes[oInclude.strName] = nIdx++;
        strPrevElement = oInclude.strName;

        oInclude.strDescription = it->attribute(pszDescription).as_string();
        vecIncludes.emplace_back(oInclude);
    }

    return true;
}

bool UniqsModel::ReadDef_defines(pugi::xml_node& node, std::string& strError) {
    const auto& defines = node.children(pszDefine);
    std::string strPrevElement = "";
    int nIdx = 0;
    for (auto it = defines.begin(); it != defines.end(); ++it) {
        UniqsProto_Define oDefine;
        oDefine.strName = it->attribute(pszName).as_string();
        if (oDefine.strName.empty()) {
            strError = pszDefine;
            strError += " ";
            strError += pszName;
            strError += " attribute not found.";
            if (!strPrevElement.empty()) {
                strError += " prev element:";
                strError += strPrevElement;
            }
            return false;
        }
        if (mapDefines.find(oDefine.strName) != mapDefines.end()) {
            strError = pszDefine;
            strError += " ";
            strError += oDefine.strName;
            strError += " duplicated.";
            if (!strPrevElement.empty()) {
                strError += " prev element:";
                strError += strPrevElement;
            }
            return false;
        }

        oDefine.nValue = it->attribute(pszValue).as_int();

        mapDefines[oDefine.strName] = nIdx++;
        strPrevElement = oDefine.strName;

        oDefine.strDescription = it->attribute(pszDescription).as_string();
        vecDefines.emplace_back(oDefine);
    }

    return true;
}

bool UniqsModel::ReadDef_structs(pugi::xml_node& node, std::string& strError) {
    const auto& structs = node.children(pszStruct);
    std::string strPrevElement = "";
    int nIdx = 0;
    for (auto it = structs.begin(); it != structs.end(); ++it) {
        Uniqs_Struct oStruct;
        oStruct.strName = it->attribute(pszName).as_string();
        if (oStruct.strName.empty()) {
            strError = pszStruct;
            strError += " ";
            strError += pszName;
            strError += " attribute not found.";
            if (!strPrevElement.empty()) {
                strError += "prev element:";
                strError += strPrevElement;
            }
            return false;
        }
        if (mapStructs.find(oStruct.strName) != mapStructs.end()) {
            strError = pszStruct;
            strError += " ";
            strError += oStruct.strName;
            strError += " duplicated.";
            if (!strPrevElement.empty()) {
                strError += "prev element:";
                strError += strPrevElement;
            }
            return false;
        }

        oStruct.bSingleton = it->attribute(pszSingleton).as_bool(false);

        if (!ReadDef_struct_property(*it, oStruct, strError)) {
            return false;
        }

        mapStructs[oStruct.strName] = nIdx++;
        strPrevElement = oStruct.strName;

        oStruct.strDescription = it->attribute(pszDescription).as_string();
        vecStructs.emplace_back(oStruct);
    }

    return true;
}

bool UniqsModel::ReadDef_struct_property(pugi::xml_node& node, Uniqs_Struct& rStruct, std::string& strError) {
    const auto& properties = node.children(pszProperty);
    std::string strPrevElement = "";
    int nIdx = 0;
    for (auto it = properties.begin(); it != properties.end(); ++it) {
        Uniqs_StructProperty oProperty;

        oProperty.strName = it->attribute(pszName).as_string();
        if (oProperty.strName.empty()) {
            strError = pszProperty;
            strError += " ";
            strError += pszName;
            strError += " attribute not found.";
            if (!strPrevElement.empty()) {
                strError += " prev element:";
                strError += strPrevElement;
            }
            return false;
        }

        auto it_property = rStruct.mapProperties.find(oProperty.strName);
        if (it_property != rStruct.mapProperties.end()) {
            strError = "struct " + rStruct.strName;
            strError += " property:";
            strError += pszProperty;
            strError += " ";
            strError += oProperty.strName;
            strError += " duplicated.";
            strError += "nIdx:" + std::to_string(nIdx);
            strError += " lastIdx:" + std::to_string(it_property->second);
            if (!strPrevElement.empty()) {
                strError += "prev element:";
                strError += strPrevElement;
            }
            return false;
        }

        oProperty.strValueType = it->attribute(pszType).as_string();

        g_pTypeParser->ParseRawType(oProperty.strValueType, oProperty.eValueType);

        oProperty.strComplexType = it->attribute(pszComplexType).as_string();

        if (!oProperty.strComplexType.empty() && !g_pTypeParser->ParseComplexType(oProperty.strComplexType, oProperty.eComplexType)) {
            strError = pszProperty;
            strError += " ";
            strError += pszComplexType;
            strError += " parse error.";
            strError += " prev element:";
            strError += strPrevElement;
            return false;
        }

        oProperty.strMax = it->attribute(pszMax).as_string();
        if (g_pTypeParser->IsComplexType(oProperty) && oProperty.strMax.empty()) {
            strError = pszProperty;
            strError += " ";
            strError += pszMax;
            strError += " parse error. Node with complextype must have attribute [max]. ";
            strError += " prev element:";
            strError += strPrevElement;
            return false;
        }
        oProperty.strKey = it->attribute(pszKey).as_string();
        oProperty.strKeyType = it->attribute(pszKeyType).as_string();
        if (g_pTypeParser->IsComplexType(oProperty) /*&& !g_pTypeParser->IsRawValueType(oProperty)*/) {
        }

        oProperty.strDescription = it->attribute(pszDescription).as_string();

        rStruct.mapProperties[oProperty.strName] = nIdx++;
        strPrevElement = oProperty.strName;

        rStruct.vecProperties.emplace_back(oProperty);
    }

    return true;
}

bool UniqsModel::Check(std::string& strError) {
    if (!Check_defines(strError)) {
        return false;
    }

    if (!Check_includes(strError)) {
        return false;
    }

    if (!Check_structs(strError)) {
        return false;
    }

    return true;
}

bool UniqsModel::Check_includes(std::string& strError) {
    // for (auto it = vecIncludes.begin(); it != vecIncludes.end(); ++it)
    //{
    //	std::string strInclude = m_strPath + it->strName;
    //	std::ifstream ifs(strInclude.c_str());
    //	if (!ifs)
    //	{
    //		strError = pszInclude;
    //		strError += " ";
    //		strError += strInclude;
    //		strError += " file not found.";
    //		return false;
    //	}
    //	it->ConstructUniqsProto();
    //	if (!it->proto->ReadDef(strInclude, strError))
    //	{
    //		return false;
    //	}
    //	if (!it->proto->Check(strError))
    //	{
    //		return false;
    //	}
    // }

    return true;
}

bool UniqsModel::Check_defines(std::string& strError) {
    //// 检查defines里面是否有重复
    // std::unordered_set<std::string> mapDefinesAll;
    // for (auto it = mapDefines.begin(); it != mapDefines.end(); ++it)
    //{
    //	mapDefinesAll.insert(it->first);
    // }
    // for (auto it = vecIncludes.begin(); it != vecIncludes.end(); ++it)
    //{
    //	if (it->proto != nullptr)
    //	{
    //		for (auto it2 = it->proto->vecDefines.begin(); it2 != it->proto->vecDefines.end(); ++it2)
    //		{
    //			if (mapDefinesAll.find(it2->strName) != mapDefinesAll.end())
    //			{
    //				strError = pszDefine;
    //				strError += " complicated in include file ";
    //				strError += it->strName;
    //				strError += " ";
    //				strError += it->strDescription;
    //				return false;
    //			}
    //			mapDefinesAll.insert(it2->strName);
    //		}
    //	}
    // }

    return true;
}

bool UniqsModel::Check_structs(std::string& strError) {
    // 检查structs里面是否有重复
    // 检查structs里面的property为自定义结构体的，是否已定义
    // 检查structs里面的property为数组或map结构体的，max是否为数值或者在defines中已定义
    // 注意structs的嵌套是无限层，所以要递归或者是非递归循环
    return true;
}
