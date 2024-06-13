#include "../rocket/common/log.h"
#include "../rocket/common/config.h"

int main()
{
    rocket::Config::SetGlobalConfig("/home/pexlor/Downloads/rpc/conf/rocket.xml");
    rocket::Logger::SetGetGloballLogger();
    DEBUGLOG("test log");
    //INFOLOG("test log");
    DEBUGLOG("%s",(rocket::Config::GetGlobalConfig()->m_log_level).c_str());
    return 0;
}