#pragma once
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
namespace rocket{

pid_t getPid();

pid_t getThreadId();

}