#pragma once

#include "stl.h"
#include "uniqs_include.h"
#include "uniqs_define.h"
#include "uniqs_struct.h"

#include "pugiconfig.hpp"
#include "pugixml.hpp"

class UniqsModel {
   public:
    // 从配置的xml里面读取各部分的内容
    bool ReadProto(const std::string& strFrom, std::string& strError);

   private:
    bool ReadProto_includes(pugi::xml_node& node, std::string& strError);
    bool ReadProto_defines(pugi::xml_node& node, std::string& strError);
    bool ReadProto_structs(pugi::xml_node& node, std::string& strError);
    bool ReadProto_struct_property(pugi::xml_node& node, Uniqs_Struct& rStruct, std::string& strError);

   public:
    // 判定配置是否正确，比如包含的结构体是否有定义、包含的define是否有定义等逻辑
    bool Check(std::string& strError);

   private:
    bool Check_includes(std::string& strError);
    bool Check_defines(std::string& strError);
    bool Check_structs(std::string& strError);

   public:
   public:
    // std::vector<包含文件名, 描述>
    std::vector<Uniqs_Include> vecIncludes;
    // 防止重复包含
    std::unordered_map<std::string, int> mapIncludes;

    // std::vector<std::pair<定义名, std::pair<定义值, 描述> > >
    std::vector<UniqsProto_Define> vecDefines;
    // 防止重复包含
    std::unordered_map<std::string, int> mapDefines;

    // std::vector<std::pair<结构名, std::pair<结构内容, 描述> > >
    std::vector<Uniqs_Struct> vecStructs;
    // 防止重复包含
    std::unordered_map<std::string, int> mapStructs;

   public:
    pugi::xml_document m_doc;
    std::string m_strName;
    std::string m_strProtobufNamespace;
    bool m_bGenProtobuf;
    std::string m_strHead;
    std::string m_strDescription;
    std::string m_strPath;
};
