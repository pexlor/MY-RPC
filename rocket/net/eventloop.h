#pragma once
#include <pthread.h>
#include <set>
#include <mutex>
#include <atomic>
#include <queue>
#include <functional>
#include <sys/socket.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include "rocket/net/eventloop.h"

namespace rocket {

class EventLoop {
public:
    EventLoop();

    ~EventLoop();

    void loop();

    void wakeup();

    void stop();

private:
    pid_t m_pid;

    int m_epool_fd;

    std::set<int> m_listen_fds;

    std::queue<std::function<void()>> m_pending_tasks;

    std::atomic_bool m_stop_flag;

    std::mutex mutex_;
};
}