#include <iostream>
#include <concepts>
#include <array>
#include <ranges>

template <int n>
struct Factorial{
    static constexpr int value = n * Factorial<n - 1>::value;
};

template <>
struct Factorial<0>{
    static constexpr int value = 1;
};

constexpr int factorial_constexpr(int n) {
    if (n < 0) {
        throw std::invalid_argument("Negative input not allowed");
    }
    return n == 0 ? 1 : n * factorial_constexpr(n - 1);
}

constexpr int fibonacci(int n) {
    if (n < 0) {
        throw std::invalid_argument("Negative input not allowed");
    }
    return n <= 1 ? n : fibonacci(n - 1) + fibonacci(n - 2);
}

auto compose1(auto f, auto g) {
    return [f, g](auto x) {
        return std::invoke(f, std::invoke(g, x));
    };
}

auto compose2(auto f, auto g) {
    return [f, g](auto x) {
        return f(g(x));
    };
}

auto compose3(auto f, auto g, auto h) {
    return [=](auto x) {
        return std::invoke(h, std::invoke(g, std::invoke(f, x)));
    };
}

int main() {
    std::cout << "Factorial of 5: " << Factorial<5>::value << std::endl;
    std::cout << "Factorial of 6: " << Factorial<6>::value << std::endl;

    constexpr int fact5 = factorial_constexpr(5);

    std::array<int, factorial_constexpr(4)> arr;
    std::cout << "Size of array with constexpr size: " << arr.size() << std::endl;
    std::cout << "Factorial of 5 using constexpr function: " << fact5 << std::endl;

    std::cout << "Fibonacci of 5: " << fibonacci(5) << std::endl;
    std::cout << "Fibonacci of 6: " << fibonacci(6) << std::endl;

    auto increment = [](int x) {
        return x + 1;
    };
    auto square = [](int x) {
        return x * x;
    };

    auto comp = compose1(increment, square);
    std::cout << "Composed function result (increment after square): " << comp(3) << std::endl;
    auto comp2 = compose1(square, increment);
    std::cout << "Composed function result (square after increment): " << comp2(3) << std::endl;
    
    auto comp3 = compose2(increment, square);
    std::cout << "Composed function result (increment after square using compose2): " << comp3(3) << std::endl;
    
    auto decrement = [](int x) { return x - 1; };
    auto doubleValue = [](int x) { return 2 * x; };
    auto add3 = [](int x) { return x + 3; };

    auto comp4 = compose3(decrement, doubleValue, add3);
    std::cout << "Composed function result (increment, double, add3): " << comp4(3) << std::endl;

    return 0;
}