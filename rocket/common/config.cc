#include "config.h"
namespace rocket {

   Config::Config(const char * xmlfile)
   {
        TiXmlDocument * xml_document = new TiXmlDocument();

        bool ret  =xml_document->LoadFile(xmlfile);

        if(!ret)
        {
            printf("Start rpc server error\n");
            exit(0);
        }

        TiXmlElement *root_node = xml_document->FirstChildElement("root")

   }




}

