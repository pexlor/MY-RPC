#include "asynclog.h"

AsyncLogger::AsyncLogger(const std::string & basename,off_t rollSize,int flushInterval) :
    flushInterval_(flushInterval),
    basename_(basename),
    rollSize_(rollSize),
    mutex_(),
    cond_(),
    currentBuf_(new Buffer(maxBuffSize)),
    nextBuf_(new Buffer(maxBuffSize)),
    buffers_(),
    isRuning_(false)
{
    buffers_.reserve(16);
}

AsyncLogger::~AsyncLogger()
{
    printf("~AsyncLogger");
    if (isRuning_)
    {
      stop();
    }
}

void AsyncLogger::append(const std::string & logline,int len)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if(currentBuf_->avail() > len)
    {
        currentBuf_->append(logline.c_str(),len);
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
            currentBuf_.reset(new Buffer(maxBuffSize));
        }
        currentBuf_->append(logline.c_str(),len);
        cond_.notify_all();
    }
}

void AsyncLogger::AsyncLoggerLoop()
{
    assert(isRuning_);
    fileFd_ = open(basename_.c_str(),O_RDWR | O_CREAT, 0666);
    BufferPtr newBuffer1(new Buffer(maxBuffSize));
    BufferPtr newBuffer2(new Buffer(maxBuffSize));
    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);
    std::string output;
    runCond_.notify_all();//通知主线程准备完毕
    while(isRuning_)
    {
        assert(newBuffer1 && newBuffer1->size() == 0);
        assert(newBuffer2 && newBuffer2->size() == 0);
        assert(buffersToWrite.empty());
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if(buffers_.empty())
            {
                printf("i im wait\n");
                cond_.wait_for(lock,std::chrono::seconds(flushInterval_));
                printf("i im wait2\n");
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
            buffersToWrite.erase(buffersToWrite.begin()+2, buffersToWrite.end());
        }
        // 将buffersToWrite的数据写入到日志文件中
        for (const auto& buffer : buffersToWrite)
        {
            output.append(buffer->data(), buffer->size());
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
        printf("asyncLogger is write\n");
        write(fileFd_,output.c_str(),output.size());
        output = "";
    }
    write(fileFd_,output.c_str(),output.size());
    output = "";
    close(fileFd_);
}

void AsyncLogger::start()
{
    if(isRuning_){
        printf("asyncLogger is run\n");
        return;
    }
    isRuning_ = true;
    thread_.reset(new std::thread(std::bind(&AsyncLogger::AsyncLoggerLoop,this)));
    std::unique_lock<std::mutex> lock(runMutex_);
    runCond_.wait(lock);
}

void AsyncLogger::stop()
{
    printf("wait stop");
    isRuning_ = false;
    cond_.notify_all();
    thread_->join();
}
