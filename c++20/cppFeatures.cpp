// g++ -std=c++11 -g -O0 test.cpp -o test

/*
🚀 C++11
    auto type deduction:
    Range-based for loops:
    Lambda expressions:
    Move semantics (std::move)
    Smart pointers (std::unique_ptr, std::shared_ptr)
    constexpr

🚀 C++14
    Generic lambdas: with auto
    std::make_unique
    Return type deduction

🚀 C++17
    if constexpr for compile-time branching:
    Structured bindings:
    std::string_view (non-owning strings)
    std::optional, std::variant, std::any

🚀 C++20
    Concepts:
    Ranges library:
    consteval
    std::span (non-owning array views)
    [[likely]], [[unlikely]] attributes
    Calendar and time-zone library

🚀 C++23
    std::print and std::format improvements (modern I/O)
    Deduction guides for std::stack, std::queue, etc.
    if consteval (differentiate between compile-time and runtime)
*/

#include <iostream>
#include <memory>
#include <vector>
#include <ranges>
#include <format>
#include <chrono>
#include <print>
#include <stack>
#include <generator>  // C++23 header

constexpr int compute(int x) {
    if consteval {
        // evaluated at compile time
        return x * x;
    } else {
        // evaluated at runtime
        std::cout << "Runtime computation!\n";
        return x * x;
    }
}

// A coroutine function that generates numbers from 1 to N
std::generator<int> count_to23(int n) {
    for (int i = 1; i <= n; ++i) {
        std::cout << "yield " << i << std::endl;
        co_yield i;  // 'co_yield' suspends execution and yields the value to the caller
    }
}

int main()
{
    std::cout<<"Hello World\n";
    int a = 10;
    int b = std::move(a);
    
    std::vector<int> aa = {1,2,3,4,5,6,7,8,9,10};
    for (auto& x : aa) 
        std::cout<<x << std::endl;

    // c++ 14 lambda example
    auto lambda11 = [](int x, int y) { return x + y; };
    std::cout << "C++ 14 Sum: " << lambda11(5, 10) << std::endl;

    // c++ 14 lambda example
    auto lambda14 = [](auto x, auto y) { return x + y; };
    std::cout << "C++ 14 SumI: " << lambda14(5, 10) << std::endl;
    std::cout << "C++ 14 SumF: " << lambda14(5.1, 10.2) << std::endl;

    // c++ 20 ranges example
    for (auto x : std::views::iota(1, 18) | std::views::filter([](int p) { return (p & (p-1)) == 0; })) {
        std::cout << x << " is a power of 2" << std::endl;
    }

    // c++ 23 features
    int age = 30;
    std::print("I am {} years old.\n", age);

    // Chrono formatting
    auto now = std::chrono::system_clock::now();
    std::cout << std::format("{:%Y-%m-%d %H:%M:%S}\n", now);

    constexpr int a1 = compute(5);    
    std::cout << "Compile-time evaluation " << a1 << std::endl;

    int a2 = compute(5 + std::rand());
    std::cout << "Runtime evaluation " << a2 << std::endl;
    
    std::vector<int> vec{1, 2, 3};
    std::stack my_stack(vec);  // type deduced as std::stack<int, std::vector<int>>

    // Call the coroutine and get the generator
    auto numbers = count_to23(5); // Lazy evaluation
    
    // Iterate over the generated values
    for (int value : numbers) {
        std::cout << "Generated: " << value << std::endl;
    }
    
    return 0;
}
