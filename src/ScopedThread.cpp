#include "cpp_concurrency_in_action/ScopedThread.hpp"
#include <stdexcept>
#include <thread>

namespace concurrency
{

ScopedThread::ScopedThread(std::thread thread) : ownedThread_{std::move(thread)}
{
    if (!ownedThread_.joinable()) {
        throw std::logic_error{"No thread"};
    }
}

ScopedThread::~ScopedThread()
{
    ownedThread_.join();
}

}  // namespace concurrency
