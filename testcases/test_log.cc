#include "../rocket/common/log.h"
#include "../rocket/common/config.h"

int main()
{
    rocket::Config::SetGlobalConfig("/home/pexlor/Downloads/rpc/conf/rocket.xml");
    DEBUGLOG("test log");
    DEBUGLOG("%s",(rocket::Config::GetGlobalConfig()->m_log_level).c_str());
    return 0;
}