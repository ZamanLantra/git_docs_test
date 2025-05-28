#include <iostream>
#include <concepts>

template <typename T>
requires std::is_arithmetic_v<T>
T square(T value) {
    return value * value;
}

template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
T square_sfinae(T value) {
    return value * value;
}


template <typename T>
requires std::is_floating_point_v<T>
T squareV2(T value) {
    return value * value;
}

int main() {
    std::cout << "Square of 4: " << square(4) << std::endl;
    std::cout << "square_sfinae of 4: " << square_sfinae(4) << std::endl;
    
    std::cout << "Square of (v2) 4.xxxx : " << squareV2(4.5102342556388827666517687) << std::endl;

    // Uncommenting the following line will cause a compilation error
    // because std::string is not an arithmetic type.
    // std::cout << "Square of 'Hello': " << square(std::string("Hello")) << std::endl;

    return 0;
}