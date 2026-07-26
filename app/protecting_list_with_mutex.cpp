#include <algorithm>
#include <iostream>
#include <thread>
#include <list>
#include <mutex>
#include <vector>

std::list<int> someList;
std::mutex someMutex;

void addToList(int newValue) {
    // std::lock_guard<std::mutex> guard(someMutex);
    std::scoped_lock guard(someMutex);
    someList.emplace_back(newValue);
}

bool listContains(int valueToFind) {
    // std::lock_guard guard(someMutex);
    std::scoped_lock guard(someMutex);
    // return std::find(someList.begin(), someList.end(), valueToFind) != someList.end();
    return std::ranges::find(someList, valueToFind) != someList.end();
}

int main (int argc, char *argv[]) {
    std::vector<std::thread> threads;
    for (int i = 0; i < 20; ++i) {
        threads.emplace_back(addToList, i);
    }

    std::cout << "Contains: " << std::boolalpha << listContains(3) << std::endl;

    std::cout << "List contains: ";
    for (const auto& entry : someList) {
        std::lock_guard guard(someMutex);
        std::cout << entry << ", ";
    }
    std::cout << std::endl;

    for (auto& t : threads) {
        t.join();
    }
    return 0;
}
