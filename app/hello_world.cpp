#include <iostream>
#include <thread>

void hello() {
    std::cout << "Hello world, from a thread" << std::endl;
}

int main(int argc, char* argv[]) {
    std::thread t{hello};
    t.join();
}
