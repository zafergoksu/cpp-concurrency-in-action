#include <iostream>
#include "cpp_concurrency_in_action/JoiningThread.hpp"

using concurrency::JoiningThread;

void hello()
{
    std::cout << "Hello" << std::endl;
}

void world()
{
    std::cout << "World" << std::endl;
}

void add(int a, int b)
{
    std::cout << "sum: " << a + b << std::endl;
}

struct Fib
{
    int prev = 0;
    int curr = 1;

    void work(int max)
    {
        if (max > 1) {
            for (int i = 2; i <= max; ++i) {
                int temp = curr;
                curr = prev + curr;
                prev = temp;
            }
        }

        std::cout << "fib: " << curr << std::endl;
    }
};

int main(int argc, char* argv[])
{
    JoiningThread thread{std::thread(hello)};
    std::thread::id id1 = thread.get_id();
    JoiningThread other{std::thread(world)};
    std::thread::id id2 = other.get_id();

    std::cout << "jthread 1: " << id1 << " jthread 2: " << id2 << "\n";

    thread.swap(other);
    std::thread::id newId1 = thread.get_id();
    std::thread::id newId2 = other.get_id();
    std::cout << "jthread 1: " << newId1 << " jthread 2: " << newId2 << "\n";
    std::cout << "threads swapped: " << std::boolalpha
              << (id1 != newId1 && id2 != newId2) << std::endl;

    JoiningThread summed{add, 1, 2};

    Fib myFib;
    JoiningThread fibb{&Fib::work, &myFib, 20};

    return 0;
}
