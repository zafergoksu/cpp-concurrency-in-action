#pragma once

#include <thread>
#include <utility>

namespace concurrency
{

class JoiningThread
{
public:
    JoiningThread() noexcept = default;

    template <typename Callable, typename... Args>
    explicit JoiningThread(Callable&& func, Args&&... args)
        : ownedThread_{std::forward<Callable>(func),
                       std::forward<Args>(args)...}
    {}

    explicit JoiningThread(std::thread thread) noexcept;
    JoiningThread(JoiningThread&& other) noexcept;
    JoiningThread& operator=(JoiningThread&& other) noexcept;
    JoiningThread(const JoiningThread&) = delete;
    JoiningThread& operator=(const JoiningThread&) = delete;

    JoiningThread& operator=(std::thread other) noexcept;
    ~JoiningThread() noexcept;

    void swap(JoiningThread& other) noexcept;
    [[nodiscard]] std::thread::id get_id() const noexcept;
    [[nodiscard]] bool joinable() const noexcept;
    void join();
    void detach();
    std::thread& as_thread() noexcept;
    [[nodiscard]] const std::thread& as_thread() const noexcept;

private:
    std::thread ownedThread_;
};

}  // namespace concurrency
