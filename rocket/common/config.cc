#include "config.h"
#define READ_XML_NODE(name,parent) \
    TiXmlElement * name##_node = parent->FirstChildElement(#name); \
    if(!name##_node){\
        printf("Start rpc sever error");\
        exit(0); \
    }

#define READ_STR_FROM_XML_NODE(name,parent) \
    TiXmlElement * name##_node = parent->FirstChildElement(#name); \
    if(!name##_node || !name##_node->GetText()){\
        printf("Start rpc sever error");\
        exit(0); \
    } \
    std::string name## = std::string(name##_node->GetText());\


namespace rocket {

    static Config* g_config = nullptr;

    Config * Config::GetGlobalConfig()
    {
        return g_config;
    }

    void Config::SetGlobalConfig(const char* xmlfile)
    {
        if(g_config == nullptr)
        {
            g_config = new Config(xmlfile);
        }
        
    }
   Config::Config(const char * xmlfile)
   {
        TiXmlDocument * xml_document = new TiXmlDocument();

        bool ret  =xml_document->LoadFile(xmlfile);

        if(!ret)
        {
            printf("Start rpc server error\n");
            exit(0);
        }

        READ_XML_NODE(root,xml_document);
        READ_XML_NODE(log,root_node);
        READ_STR_FROM_XML_NODE(log_level,log_node);

        m_log_level = log_level;
   }




}
