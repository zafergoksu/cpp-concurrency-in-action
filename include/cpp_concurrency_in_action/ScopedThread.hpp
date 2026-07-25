#pragma once

#include <thread>

namespace concurrency
{

class ScopedThread
{
public:
    explicit ScopedThread(std::thread thread);
    ~ScopedThread();

    ScopedThread(const ScopedThread&) = delete;
    ScopedThread& operator=(ScopedThread&&) = delete;

private:
    std::thread ownedThread_;
};

}  // namespace concurrency
