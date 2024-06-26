#include "Http.h"

Http::Http(std::string &Message):Message_(Message),MessageCopy_(Message)
{
    
}

Http::~Http()
{
    munmap(FileAddress,FileSize_);
}

/*
    功能：解析请求行
    参数：一行请求行字符串
    返回值：匹配成功返回true否则false
*/
bool Http::ParseRequestLine(const std::string & line)
{
    std::regex patten("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
    std::smatch subMatch;
   // std::cout<<"Line:::"+line<<std::endl;
    if(std::regex_match(line,subMatch,patten))
    {
        RequstMethod_   = subMatch[1];
        RequstUrl_      = subMatch[2];
        RequstHttpEdition_ = subMatch[3];
        state_ = HEADERS;
        //std::cout <<RequstMethod_ <<std::endl;
        //printf("GETLINE:%s %s %s\n",RequstMethod_.c_str(),RequstUrl_.c_str(),RequstHttpEdition_.c_str());
        return true;
    }
    return false;
}

/*
    功能：解析请求头
    参数：一行请求头字符串
    返回值：
*/

bool Http::ParseHeader(const std::string & line)
{
    std::regex patten("^([^:]*): ?(.*)$");
    std::smatch subMatch;
    if(std::regex_match(line,subMatch,patten))
    {
       // std::cout << " HEAD::::" << subMatch[1] << " " << subMatch[2] << std::endl;
        RequestHead_[subMatch[1]] = subMatch[2];
        return true;
    }else
    {
        state_ = BODY;
        return false;
    }
}


/*
    功能：解析请求请求数据
    参数：一行请求数据字符串
    返回值：
*/
void Http::ParseBody(const std::string & line)//请求解析体
{
    state_ = FINISH;
}

std::string Http::GetLine()
{
    int index = MessageCopy_.find("\r\n");
    std::string line = MessageCopy_.substr(0,index);
    MessageCopy_.erase(0,index+2);
    return line;
}


void Http::ParsePath()
{
    if(RequstUrl_ == "/")
    {
        FilePath_ = "res/demo.html";
        FileType_ = "text/html";
    }else
    {
        FilePath_ = "res"+RequstUrl_;
        FileType_ = "image/jpg";
    }
    std::cout << FilePath_ <<std::endl;
}
bool Http::Parse()
{
    if(Message_.size()<=0)
    {
        return false;
    }
    while(MessageCopy_.size() > 0 && state_ != FINISH)
    {
        std::string line = GetLine();
        switch (state_)
        {
        case REQUEST_LINE:
            if(!ParseRequestLine(line))
            { 
                std::cout<<"Line Erroe\n";
                return false;
            }
            ParsePath();
            break;
        case HEADERS:
            ParseHeader(line);
            if(MessageCopy_.size()<=2)
            {
                state_ = FINISH;
            }
            break;
        case BODY:
            ParseBody(line);
            break;
        default:
            break;
        }
    }
    return true;
}


/*
char head_buf[] = "HTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\nContent-Type: text/html;charset=UTF-8\r\nContent-Length: 1033\r\nConnection: close\r\n\r\n";
char Phead_buf[] = "HTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\nContent-Type: image/jpg;charset=UTF-8\r\nContent-Length: 35825\r\nConnection: close\r\n\r\n";
*/
std::string Http::Respond()
{
    std::string RespondMessage;
    STATUS_CODE StatusCode;
    int filefd;
    struct stat file_st;
    char FileBuf[1024];
    char buf[1024];
    if(!Parse())
    {
        printf("Parse ERROE\n");
        return "";
    }
    filefd = ::open(FilePath_.c_str(),O_RDONLY);
    if(filefd!=-1)
    {
        StatusCode = SUCCESS;
        fstat(filefd,&file_st);
        FileSize_ = file_st.st_size;
        sprintf(FileBuf,"HTTP/1.1 %d OK\r\nAccept-Ranges: bytes\r\nContent-Type: %s;charset=UTF-8\r\nContent-Length: %d\r\nConnection: close\r\n\r\n",StatusCode,FileType_.c_str(),FileSize_);
        RespondMessage.append(FileBuf);
        FileAddress =(char *)mmap(NULL,FileSize_,PROT_READ,MAP_PRIVATE,filefd,0);
        if(FileAddress==(char *)-1)
        {

            printf("mmap ERROR \n");
        }
        RespondMessage.append(FileAddress,FileSize_);
        close(filefd);
    }else
    {
        StatusCode = ERROR;
        
            printf("open Faile \n");
        sprintf(FileBuf,"HTTP/1.1 %d ERROR\r\nConnection: close\r\n\r\n",StatusCode);
        RespondMessage.append(FileBuf);
    }
    printf("ok \n");
    return RespondMessage;
}