#include "asynclog.h"

/*
class AsyncLogger {
private:
    typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
    typedef BufferVector::value_type BufferPtr;

    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;

    BufferPtr currentBuf_;
    BufferPtr nextBuf_;
    BufferVector buffers_;

    const int flushInterval_;//刷新时间
    const off_t rollSize; //预留日志大小
    const std::string basename_;
    std::atomic<bool> isRuning_;

    int fileFd_;
    
private:
    void AsyncLoggerLoop();

public:
    void append(const std::string * logline,int len);
    void start();
    void stop();
    ~AsyncLogger();
    AsyncLogger(const std::string & basename,off_t rollSize,int flushInterval = 3);

};
*/

AsyncLogger::AsyncLogger(const std::string & basename,off_t rollSize,int flushInterval) :
    flushInterval_(flushInterval),
    basename_(basename),
    rollSize_(rollSize),
    thread_(std::bind(&AsyncLogger::AsyncLoggerLoop,this)),
    mutex_(),
    cond_(),
    currentBuf_(new Buffer),
    nextBuf_(new Buffer),
    buffers_()
{
    //currentBuf_->bzero();
    //nextBuf_->bzero();
    buffers_.reserve(16);
}

void AsyncLogger::append(const std::string * logline,int len)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if(currentBuf_->avail() > len)
    {
        currentBuf_->append(logline->c_str(),len);
    }
    else
    {
        buffers_.push_back(std::move(currentBuf_));
        if(nextBuf_)
        {
            currentBuf_ = std::move(nextBuf_);
        }
        else
        {
            currentBuf_.reset(new Buffer);
        }
        currentBuf_->append(logline->c_str(),len);
        cond_.notify_all();
    }
}

void AsyncLogger::AsyncLoggerLoop()
{
    assert(isRuning_ == true);
    fileFd_ = open(basename_.c_str(),O_RDWR | O_CREAT, 0666);

    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bzero();
    newBuffer2->bzero();
    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);
    while(isRuning_)
    {
        assert(newBuffer1 && newBuffer1->length() == 0);
        assert(newBuffer2 && newBuffer2->length() == 0);
        assert(buffersToWrite.empty());
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if(buffers_.empty())
            {
                cond_.wait_for(lock,std::chrono::seconds(flushInterval_));
            }

            buffers_.push_back(std::move(currentBuf_));
            currentBuf_ = std::move(newBuffer1);
            buffersToWrite.swap(buffers_);
            if(!nextBuf_)
            {
                nextBuf_ = std::move(newBuffer2);
            }

        }
        assert(!buffersToWrite.empty());
        
        if(buffersToWrite.size() > 25)
        {
            char buf[256];
            //snprintf(buf,sizeof(buf));
            buffersToWrite.erase(buffersToWrite.begin()+2, buffersToWrite.end());
        }
        // 将buffersToWrite的数据写入到日志文件中
        for (const auto& buffer : buffersToWrite)
        {
            output.append(buffer->data(), buffer->length());
        }
        // 重新调整buffersToWrite的大小
        if (buffersToWrite.size() > 2)
        {
            buffersToWrite.resize(2);
        }
        // 从buffersToWrite中弹出一个作为newBuffer1 
        if (!newBuffer1)
        {
            assert(!buffersToWrite.empty());
            newBuffer1 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer1->reset();
        }
        // 从buffersToWrite中弹出一个作为newBuffer2
        if (!newBuffer2)
        {
            assert(!buffersToWrite.empty());
            newBuffer2 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer2->reset();
        }
        // 清空buffersToWrite
        buffersToWrite.clear();
        output.flush();
    }
    output.flush();
}

void AsyncLogger::start()
{
    isRuning_ = true;
    
}

void AsyncLogger::stop()
{
    isRuning_ = false;
    cond_.notify_all();
    thread_.join();
}
