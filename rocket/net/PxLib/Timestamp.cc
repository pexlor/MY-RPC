#include "Timestamp.h"

/*

class Timestamp
{
private:
    time_t secsinceepoch_;
public:
    Timestamp();
    Timestamp(int64_t secsinceepoch);
    ~Timestamp();

    static Timestamp now();

    time_t toint() const;
    std::string tostring() const;
};
*/
Timestamp::Timestamp()
{
    secsinceepoch_ = time(0);
}
Timestamp::Timestamp(int64_t secsinceepoch)
            :secsinceepoch_(secsinceepoch)
{

}
Timestamp::~Timestamp()
{

}

Timestamp Timestamp::now()
{
    return Timestamp();
}

time_t Timestamp::toint() const
{
    return secsinceepoch_;
}
std::string Timestamp::tostring() const
{
    char buf[32] = {0};
    tm * tm_time = localtime(&secsinceepoch_);
    snprintf(buf,32,"%4d-%02d",
            tm_time->tm_year+1900,tm_time->tm_mon+1);

    return buf;
}
