#include "log.h"


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
        << "[" << m_pid << ":" << m_thread_id << "]\t";

    return ss.str();
}

static std::unique_ptr<Logger> g_logger = nullptr;
static std::mutex g_mutex;

Logger::Logger(LogLevel level):
    asyncLog_(Config::GetGlobalConfig()->m_log_file_path+Config::GetGlobalConfig()->m_log_log_file_name,Config::GetGlobalConfig()->m_log_asyn_flush_time),
    m_set_level(level)
{
    std::cout<< Config::GetGlobalConfig()->m_log_file_path+Config::GetGlobalConfig()->m_log_log_file_name << std::endl;
}

void Logger::Init()
{   
    LogLevel global_log_levle = StringToLogLevel(Config::GetGlobalConfig()->m_log_level);
    
    g_logger.reset(new Logger(global_log_levle));

    g_logger->asyncLog_.start();
}

Logger * Logger::GetGloballLogger()
{
    if(g_logger == nullptr)
    {
        std::lock_guard<std::mutex> lcok(g_mutex);
        if(g_logger == nullptr)
        {
            Init();
        }
    }
    return g_logger.get();   
}

LogLevel Logger::getLogLevel()
{
    return m_set_level;
}

void Logger::pushlog(const std::string & msg)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if(Config::GetGlobalConfig()->m_enable_sync_log) m_buffer.push(msg);
    asyncLog_.append(msg,msg.size());
}

Logger::~Logger()
{
    printf("Logger over\n");
}

void Logger::log()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    while (!m_buffer.empty()) {
        std::string msg = m_buffer.front();
        m_buffer.pop();
        std::cout<< msg; 
    }
}