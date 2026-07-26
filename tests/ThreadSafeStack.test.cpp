#include "cpp_concurrency_in_action/ThreadSafeStack.hpp"
#include <gtest/gtest.h>
#include <algorithm>
#include <atomic>
#include <cstddef>
#include <latch>
#include <memory>
#include <string>
#include <thread>
#include <vector>

using concurrency::EmptyStackException;
using concurrency::ThreadSafeStack;

TEST(ThreadSafeStack, IsEmpty)
{
    ThreadSafeStack<int> stack{};

    EXPECT_TRUE(stack.empty());
}

TEST(ThreadSafeStack, InitializeOneValueRef)
{
    ThreadSafeStack<int> stack{};
    stack.push(1);

    EXPECT_FALSE(stack.empty());
    int myValue;
    stack.pop(myValue);
    EXPECT_EQ(myValue, 1);
}

TEST(ThreadSafeStack, InitializeOneValueSharedPtr)
{
    ThreadSafeStack<int> stack{};
    stack.push(1);

    EXPECT_FALSE(stack.empty());
    std::shared_ptr<int> myValue = stack.pop();
    ASSERT_NE(myValue, nullptr);
    EXPECT_EQ(*myValue, 1);
}

TEST(ThreadSafeStack, PopThrowsOnEmptyStack)
{
    ThreadSafeStack<int> stack{};

    EXPECT_TRUE(stack.empty());
    EXPECT_THROW((void)stack.pop(), EmptyStackException);
}

TEST(ThreadSafeStack, PopsInOrder)
{
    ThreadSafeStack<int> stack{};
    stack.push(1);
    stack.push(2);
    stack.push(3);

    ASSERT_FALSE(stack.empty());

    int result;
    stack.pop(result);
    EXPECT_EQ(result, 3);
    stack.pop(result);
    EXPECT_EQ(result, 2);
    stack.pop(result);
    EXPECT_EQ(result, 1);

    ASSERT_TRUE(stack.empty());
}

TEST(ThreadSafeStack, PopByRefThrowsOnEmptyStack)
{
    ThreadSafeStack<int> stack{};

    int value{};
    EXPECT_THROW(stack.pop(value), EmptyStackException);
}

TEST(ThreadSafeStack, EmptyStackExceptionMessage)
{
    ThreadSafeStack<int> stack{};

    try {
        (void)stack.pop();
        FAIL() << "Expected EmptyStackException";
    } catch (const EmptyStackException& e) {
        EXPECT_STREQ(e.what(), "Empty stack");
    }
}

TEST(ThreadSafeStack, PopThrowLeavesStackUsable)
{
    ThreadSafeStack<int> stack{};
    EXPECT_THROW((void)stack.pop(), EmptyStackException);

    constexpr int sentinel = 42;
    stack.push(sentinel);
    int value{};
    stack.pop(value);
    EXPECT_EQ(value, sentinel);
}

TEST(ThreadSafeStack, CopyConstructorCopiesContents)
{
    ThreadSafeStack<int> stack{};
    stack.push(1);
    stack.push(2);

    ThreadSafeStack<int> copy{stack};

    int value{};
    copy.pop(value);
    EXPECT_EQ(value, 2);
    copy.pop(value);
    EXPECT_EQ(value, 1);
    EXPECT_TRUE(copy.empty());

    // The original must be untouched by draining the copy.
    stack.pop(value);
    EXPECT_EQ(value, 2);
    stack.pop(value);
    EXPECT_EQ(value, 1);
    EXPECT_TRUE(stack.empty());
}

TEST(ThreadSafeStack, HoldsNonTrivialTypes)
{
    constexpr std::size_t longLength = 1000;
    ThreadSafeStack<std::string> stack{};
    stack.push("hello");
    stack.push(std::string(longLength, 'x'));

    std::shared_ptr<std::string> top = stack.pop();
    ASSERT_NE(top, nullptr);
    EXPECT_EQ(*top, std::string(longLength, 'x'));

    std::string value;
    stack.pop(value);
    EXPECT_EQ(value, "hello");
}

TEST(ThreadSafeStack, ConcurrentPushesLoseNothing)
{
    constexpr std::size_t numThreads = 4;
    constexpr int pushesPerThread = 1000;

    ThreadSafeStack<int> stack{};
    std::latch startSignal{numThreads};
    {
        std::vector<std::jthread> producers;
        producers.reserve(numThreads);
        for (std::size_t threadId = 0; threadId < numThreads; ++threadId) {
            producers.emplace_back([&stack, &startSignal, threadId]() -> void {
                // wait until all threads are initialized
                startSignal.arrive_and_wait();
                for (int i = 0; i < pushesPerThread; ++i) {
                    stack.push((static_cast<int>(threadId) * pushesPerThread) +
                               i);
                }
            });
        }
    }

    std::vector<int> popped;
    popped.reserve(numThreads * pushesPerThread);
    int value{};
    while (!stack.empty()) {
        stack.pop(value);
        popped.emplace_back(value);
    }

    ASSERT_EQ(popped.size(), numThreads * pushesPerThread);
    std::ranges::sort(popped);
    for (std::size_t i = 0; i < popped.size(); ++i) {
        EXPECT_EQ(popped[i], static_cast<int>(i));
    }
}

TEST(ThreadSafeStack, ConcurrentPushAndPop)
{
    constexpr std::size_t numProducers = 2;
    constexpr std::size_t numConsumers = 2;
    constexpr int pushesPerProducer = 1000;
    constexpr std::size_t totalItems = numProducers * pushesPerProducer;

    ThreadSafeStack<int> stack{};
    std::latch startSignal{numProducers + numConsumers};
    std::atomic<std::size_t> consumedCount{0};
    std::atomic<int> consumedSum{0};
    {
        // thread pool that we wait for at the end of scope
        std::vector<std::jthread> workers;
        workers.reserve(numProducers + numConsumers);

        for (std::size_t threadId = 0; threadId < numProducers; ++threadId) {
            workers.emplace_back([&stack, &startSignal, threadId]() -> void {
                startSignal.arrive_and_wait();
                for (int i = 0; i < pushesPerProducer; ++i) {
                    stack.push(
                        (static_cast<int>(threadId) * pushesPerProducer) + i);
                }
            });
        }

        for (std::size_t threadId = 0; threadId < numConsumers; ++threadId) {
            workers.emplace_back([&stack, &startSignal, &consumedCount,
                                  &consumedSum]() -> void {
                startSignal.arrive_and_wait();
                while (consumedCount.load(std::memory_order_relaxed) <
                       totalItems) {
                    try {
                        int value{};
                        stack.pop(value);
                        // Relaxed is enough: these are statistics counters, and
                        // the jthread join provides the final synchronization.
                        consumedSum.fetch_add(value, std::memory_order_relaxed);
                        consumedCount.fetch_add(1, std::memory_order_relaxed);
                    } catch (const EmptyStackException&) {
                        std::this_thread::yield();
                    }
                }
            });
        }
    }

    constexpr int expectedSum =
        static_cast<int>(totalItems) * (totalItems - 1) / 2;
    EXPECT_EQ(consumedCount.load(std::memory_order_relaxed), totalItems);
    EXPECT_EQ(consumedSum.load(std::memory_order_relaxed), expectedSum);
    EXPECT_TRUE(stack.empty());
}
