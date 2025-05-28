#include <iostream>
#include <ranges>
#include <concepts>

struct Generator {
    int current = 0, previous = 0;
    int next() {
        if (current == 0) { // Handle the first call
            current = 1;
            return 0;
        }
        
        int next_value = current + previous;
        previous = current;
        current = next_value;
        
        return current;
    }
};

Generator fibonacci(int n) {
    return Generator();
}

int main() {
    std::cout << "Hello, C++20!" << std::endl;

    auto numbers = {1, 2, 3, 4, 5, 6, 7, 8};
    auto squared_numbers = numbers | std::views::filter([](int n) { return n % 2 == 0; }) | std::views::transform([](int n) { return n * n; }) ;

    for (const auto& num : squared_numbers) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    auto numbers2 = std::views::iota(1, 20) | std::views::filter([](int n) { return n % 3 == 0;}) | std::views::take(5);

    for (const auto& num : numbers2) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    auto gen = fibonacci(10);

    for (int i = 0; i < 10; ++i) {
        std::cout << gen.next() << " ";
    }
    std::cout << std::endl;

    auto results = std::views::iota(1, 101) 
                    | std::views::transform([](int x) { return x * x;}) 
                    | std::views::filter([](int x) { return x % 5 == 0;}) 
                    | std::views::take(10);
    
    for (const auto& a : results) {
        std::cout << a << " ";
    }
    std::cout << std::endl;

    return 0;
}