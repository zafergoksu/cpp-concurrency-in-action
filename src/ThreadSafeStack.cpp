#include <cpp_concurrency_in_action/ThreadSafeStack.hpp>

#include <exception>
#include <memory>
#include <mutex>
#include <stack>

namespace concurrency {

const char* EmptyStackException::what() const noexcept {
    return "Empty stack";
}

} // namespace concurrency
