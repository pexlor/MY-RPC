#include "log.h"
#include "util.h"
#include <sys/time.h>
#include "config.h"



namespace rocket {

std::string LogLevelToString(LogLevel level)
{
    switch (level)
    {
    case Debug:
        return "DEBUG";
    case Info:
        return "INFO";
    case Error:
        return "ERROR";
    default:
        return "UNKNOWN";

    }
}


LogLevel StringToLogLevel(const std::string& log_level)
{
    if(log_level == "DEBUG")
    {
        return Debug;
    }else if(log_level == "INFO")
    {
        return Info;
    }else if(log_level == "ERROR")
    {
        return Error;
    }else
    {
        return Unknown;
    }
}

LogEvent::LogEvent(LogLevel level)
{
    m_level = level;
}
std::string LogEvent::getFileName()
{
    return m_file_name;
}

int32_t LogEvent::getLogLevel()
{
    return m_level;
}

std::string LogEvent::toSting()
{
    struct timeval now_time;

    gettimeofday(&now_time,nullptr);

    struct tm now_time_t;

    localtime_r(&(now_time.tv_sec),&now_time_t);

    char buf[128];
    strftime(&buf[0],128,"%y-%m-%d %H:%M:%S",&now_time_t);
    std::string time_str(buf);

    int ms =now_time.tv_usec / 1000;
    time_str =time_str + "." +std::to_string(ms);

    m_pid = getPid();
    m_thread_id = getThreadId();

    std::stringstream ss;
    ss  << "[" << LogLevelToString(m_level) << "]\t"
        << "[" << time_str << "]\t"
        << "[" << m_pid << ":" << m_thread_id << "]\t"
        << "[" << std::string(__FILE__) <<":"<< __LINE__ << "]\t";

    return ss.str();
}

static Logger* g_logger = nullptr;

Logger::Logger(LogLevel level)
{
    this->m_set_level = level;
}

Logger * Logger::GetGloballLogger()
{
    if(g_logger)
    {
        return g_logger;
    }
    LogLevel global_log_levle = StringToLogLevel(Config::GetGlobalConfig()->m_log_level);
    g_logger = new Logger(global_log_levle);
    return g_logger;
}

void Logger::pushlog(const std::string & msg)
{
    m_buffer.push(msg);
}

void Logger::log()
{
    while(!m_buffer.empty())
    {
        
        std::string msg = m_buffer.front();
        msg += "\n";
        m_buffer.pop();
        printf(msg.c_str());
    }
}

}