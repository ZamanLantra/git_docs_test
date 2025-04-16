// zamanlantra@ZamansMcBookPro Study % g++ -std=c++17 -o async_future async_future.cpp 
// zamanlantra@ZamansMcBookPro Study % ./async_future 


#include <iostream>
#include <future>
#include <chrono>

int computeSquare(int x, int y) {
    std::this_thread::sleep_for(std::chrono::seconds(3));
    return x * y;
}

int main() {
    std::future<int> result = std::async(std::launch::async, computeSquare, 5, 10);

    std::cout << "Doing other work in main...\n";

    std::cout << "Waiting for the future...\n";
    int value = result.get(); 
    std::cout << "Square is: " << value << std::endl;

    return 0;
}
