#pragma once

#include <exception>
#include <memory>
#include <mutex>
#include <stack>

namespace concurrency
{

struct EmptyStackException : public std::exception
{
    [[nodiscard]] const char* what() const noexcept override;
};

template <typename T>
class ThreadSafeStack
{
public:
    ThreadSafeStack() = default;

    ThreadSafeStack(const ThreadSafeStack& other)
    {
        std::scoped_lock lock{other.mutex_};
        data_ = other.data_;
    }

    ThreadSafeStack& operator=(const ThreadSafeStack&) = delete;

    void push(T newValue)
    {
        std::scoped_lock lock{mutex_};
        data_.emplace(std::move(newValue));
    }

    [[nodiscard]] std::shared_ptr<T> pop()
    {
        std::scoped_lock lock{mutex_};
        if (data_.empty()) {
            throw EmptyStackException{};
        }
        const auto result = std::make_shared<T>(data_.top());
        data_.pop();
        return result;
    }

    void pop(T& value)
    {
        std::scoped_lock lock{mutex_};
        if (data_.empty()) {
            throw EmptyStackException();
        }
        value = data_.top();
        data_.pop();
    }

    [[nodiscard]] bool empty() const
    {
        std::scoped_lock lock{mutex_};
        return data_.empty();
    }

private:
    std::stack<T> data_;
    mutable std::mutex mutex_;
};

}  // namespace concurrency
