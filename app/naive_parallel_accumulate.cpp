#include <algorithm>
#include <cstddef>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <thread>
#include <vector>

template <typename Iterator, typename T>
struct AccumulateBlock
{
    void operator()(Iterator first, Iterator last, T& result)
    {
        result = std::accumulate(first, last, result);
    }
};

template <typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init)
{
    const std::size_t length = std::distance(first, last);
    if (!length) {
        return init;
    }

    const std::size_t minPerThread = 25;
    const std::size_t maxThreads = (length + minPerThread - 1) / minPerThread;
    const std::size_t hardwareThreads = std::thread::hardware_concurrency();
    const std::size_t numThreads =
        std::min(hardwareThreads != 0 ? hardwareThreads : 2, maxThreads);
    const std::size_t blockSize = length / numThreads;
    std::vector<T> results(numThreads);
    std::vector<std::thread> threads(numThreads - 1);
    Iterator blockStart = first;

    for (std::size_t i = 0; i < (numThreads - 1); ++i) {
        Iterator blockEnd = blockStart;
        std::advance(blockEnd, blockSize);

        threads[i] = std::thread{AccumulateBlock<Iterator, T>(), blockStart,
                                 blockEnd, std::ref(results[i])};

        blockStart = blockEnd;
    }

    AccumulateBlock<Iterator, T>()(blockStart, last, results[numThreads - 1]);

    for (auto& entry : threads) {
        entry.join();
    }

    return std::accumulate(results.begin(), results.end(), init);
}

int main(int argc, char* argv[])
{
    std::vector<std::size_t> myVec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::size_t expected = (myVec.size() * (myVec.size() + 1)) / 2;
    std::size_t returnedVal =
        parallel_accumulate(myVec.begin(), myVec.end(), 0);

    std::cout << "Expected: " << expected << " Returned: " << returnedVal
              << std::endl;

    return 0;
}
