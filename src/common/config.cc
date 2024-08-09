#include "config.h"
#define READ_XML_NODE(name, parent)                               \
    TiXmlElement *name##_node = parent->FirstChildElement(#name); \
    if (!name##_node)                                             \
    {                                                             \
        printf("Start rpc sever error 1");                        \
        exit(0);                                                  \
    }

#define READ_STR_FROM_XML_NODE(name, parent)                      \
    TiXmlElement *name##_node = parent->FirstChildElement(#name); \
    if (!name##_node || !name##_node->GetText())                  \
    {                                                             \
        printf("Start rpc sever error 2");                        \
        exit(0);                                                  \
    }                                                             \
    std::string name##_str = std::string(name##_node->GetText());

static Config *g_config = nullptr;
static std::mutex g_mutex;

Config *Config::GetGlobalConfig()
{
    if(g_config == nullptr)
    {
        std::lock_guard<std::mutex> lock(g_mutex); 
        if(g_config == nullptr)
        {
            g_config = new Config();
        }
    }
    return g_config;
}

void Config::InitConfig(const char *xmlfile)
{
    if (g_config == nullptr)
    {
        std::lock_guard<std::mutex> lock(g_mutex); 
        if(g_config == nullptr)
        {
            g_config = new Config(xmlfile);
        }
    }
}

Config::Config():
    m_io_threads(sysconf(_SC_NPROCESSORS_ONLN)),
    m_log_level("DEBUG"),
    m_log_file_path(""),
    m_log_log_file_name("RpcLog.txt"),
    m_port(5000),
    m_enable_sync_log(false),
    m_log_asyn_flush_time(3.0f)
{

}

Config::Config(const char *xmlfile)
{
    TiXmlDocument *xml_document = new TiXmlDocument();

    bool ret = xml_document->LoadFile(xmlfile);

    if (!ret)
    {
        printf("Start rpc server error ,file:%s\n", xmlfile);
        xml_document->Error();
        exit(0);
    }

    READ_XML_NODE(root, xml_document);
    READ_XML_NODE(log, root_node);
    READ_XML_NODE(server, root_node);

    READ_STR_FROM_XML_NODE(log_level, log_node);
    READ_STR_FROM_XML_NODE(log_file_name, log_node);
    READ_STR_FROM_XML_NODE(log_file_path, log_node);
    READ_STR_FROM_XML_NODE(enable_sync_log, log_node);
    READ_STR_FROM_XML_NODE(log_asyn_flush_time, log_node);

    READ_STR_FROM_XML_NODE(port, server_node);
    READ_STR_FROM_XML_NODE(work_threads, server_node);
    READ_STR_FROM_XML_NODE(io_threads, server_node);
    
    m_port = std::atoi(port_str.c_str());
    m_io_threads = std::atoi(io_threads_str.c_str());
    m_work_threads = std::atoi(work_threads_str.c_str());

    m_log_level = log_level_str;
    m_log_file_path = log_file_path_str;
    m_log_log_file_name = log_file_name_str;
    m_port = std::atoi(port_str.c_str());
    m_enable_sync_log = (enable_sync_log_str == "true") ? true : false;
    m_log_asyn_flush_time = std::atoi(log_asyn_flush_time_str.c_str());

    if(!(m_io_threads &&  m_port && m_log_asyn_flush_time && m_work_threads))
    {
        perror("bad configuration file\n");
        exit(-1);
    }

}
