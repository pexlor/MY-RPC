#pragma once 
#include <tinyxml/tinyxml.h>
#include <map>
#include <string>
namespace rocket {

class Config {
public:
    Config(const char * xmlfile);

private:
    std::map<std::string,std::string> m_config_values;
};


}