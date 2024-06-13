#pragma once

#include <string>
#include <queue>
#include <memory>
#include <sstream>


namespace rocket {

template<typename... Args>
std::string formatString(const char* str,Args&&... args)
{ 
    int size = snprintf(nullptr,0,str,args...);

    std::string result;
    if(size > 0)
    {
        result.resize(size);
        snprintf(&result[0],size+1,str,args...);
    }
    return result;
}


#define DEBUGLOG(str, ... ) \
        {std::string debugmsg = (rocket::LogEvent(rocket::LogLevel::Debug)).toSting() + rocket::formatString(str,##__VA_ARGS__);\
        rocket::Logger::GetGloballLogger()->pushlog(debugmsg); \
        rocket::Logger::GetGloballLogger()->log();}
    

#define INFOLOG(str, ... ) \
    {std::string infomsg = (new rocket::LogEvent(rocket::LogLevel::Info))->toSting() + rocket::formatString(str,##__VA_ARGS__);\
    rocket::Logger::GetGloballLogger()->pushlog(infomsg); \
    rocket::Logger::GetGloballLogger()->log();}

#define ERRORLOG(str, ... ) \
    {std::string errormsg = (new rocket::LogEvent(rocket::LogLevel::Error))->toSting() + rocket::formatString(str,##__VA_ARGS__);\
    rocket::Logger::GetGloballLogger()->pushlog(errormsg); \
    rocket::Logger::GetGloballLogger()->log();}

enum LogLevel {
    Unknown = 0,
    Debug = 1,
    Info = 2,
    Error = 3
};


class Logger {
public:
    typedef std::shared_ptr<Logger> s_ptr;
    Logger(LogLevel level);
    void pushlog(const std::string & msg);
    static Logger * GetGloballLogger();
    void log();
private:
    LogLevel m_set_level;
    std::queue<std::string> m_buffer;
};

std::string LogLevelToString(LogLevel level);
LogLevel StringToLogLevel(const std::string& log_level);


class LogEvent {
public:
    LogEvent(LogLevel level);
    std::string getFileName();
    int32_t getLogLevel();
    std::string  toSting();
private:
    std::string m_file_name;
    int32_t m_file_line;
    int32_t m_pid;
    int32_t m_thread_id;
    LogLevel m_level;
};


}
