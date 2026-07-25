#include "cpp_concurrency_in_action/JoiningThread.hpp"
#include <thread>

namespace concurrency
{

JoiningThread::JoiningThread(std::thread thread) noexcept
    : ownedThread_{std::move(thread)}
{}

JoiningThread::JoiningThread(JoiningThread&& other) noexcept
    : ownedThread_{std::move(other.ownedThread_)}
{}

JoiningThread::~JoiningThread() noexcept
{
    if (joinable()) {
        join();
    }
}

JoiningThread& JoiningThread::operator=(JoiningThread&& other) noexcept
{
    if (joinable()) {
        join();
    }

    ownedThread_ = std::move(other.ownedThread_);
    return *this;
}

JoiningThread& JoiningThread::operator=(std::thread other) noexcept
{
    if (joinable()) {
        join();
    }

    ownedThread_ = std::move(other);
    return *this;
}

void JoiningThread::swap(JoiningThread& other) noexcept
{
    ownedThread_.swap(other.ownedThread_);
}

std::thread::id JoiningThread::get_id() const noexcept
{
    return ownedThread_.get_id();
}

bool JoiningThread::joinable() const noexcept
{
    return ownedThread_.joinable();
}

void JoiningThread::join()
{
    ownedThread_.join();
}

void JoiningThread::detach()
{
    ownedThread_.detach();
}

std::thread& JoiningThread::as_thread() noexcept
{
    return ownedThread_;
}

const std::thread& JoiningThread::as_thread() const noexcept
{
    return ownedThread_;
}

}  // namespace concurrency
