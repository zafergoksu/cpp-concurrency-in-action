#include <iostream>
#include "cpp_concurrency_in_action/ScopedThread.hpp"

using concurrency::ScopedThread;

struct MyFunc
{
    MyFunc(int& val) : myVal_{val} {};

    void operator()()
    {
        for (int i = 0; i < 100; ++i) {
            myVal_++;
        }
    }

    int& myVal_;
};

int main(int argc, char* argv[])
{
    int localState = 0;
    {
        ScopedThread{std::thread{MyFunc{localState}}};
    }
    std::cout << localState << std::endl;
}
