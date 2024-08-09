#pragma once 
#include <tinyxml/tinyxml.h>
#include <map>
#include <string>
#include <unistd.h>
#include <mutex>
class Config {
public:
    std::string m_log_level;
    std::string m_log_log_file_name;
    std::string m_log_file_path;
    int m_port {0}; 
    int m_io_threads {0};
    int m_work_threads {0};
    bool m_enable_sync_log {false};
    int m_log_asyn_flush_time;
public:
    static Config * GetGlobalConfig();
    static void  InitConfig(const char* xmlfile);
private:
    Config(const char * xmlfile);
    Config();
private:
    std::map<std::string,std::string> m_config_values;
};


