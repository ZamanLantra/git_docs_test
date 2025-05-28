#include <iostream>
#include <vector>
#include <algorithm>
#include <ranges>
#include <numeric>
#include <concepts>
#include <boost/hana.hpp>
#include <map>

int main() {
    std::vector<int> nums = {1, 2, 3, 4, 5};
    for (const auto& num : nums) {
        std::cout << "Number: " << std::tgamma(num+1) << std::endl; // std::tgamma computes the factorial of a number
    }

    auto tuple = boost::hana::make_tuple(5, "Hello", 3.14);

    auto transformed_tupe = boost::hana::transform(tuple, 
        [](auto x) {
            if constexpr (std::is_same_v<decltype(x), const char*>) {
                std::string str(x);
                return str + " World";
            }
            else {
                return x * 2; // For numeric types, just double the value
            }
        }
    );

    std::cout << "Transformed tuple: ";
    boost::hana::for_each(transformed_tupe, [](const auto& x) {
            std::cout << x << " ";
    });
    std::cout << "\n";

    auto numbers = std::views::iota(1, 13) 
                    | std::views::filter([](int n) { return n % 2 != 0; })
                    | std::views::transform([](int n) { return n * n; });
    
    auto sum_of_squares = std::accumulate(numbers.begin(), numbers.end(), 0);
    std::cout << "Sum of squares of even numbers from 1 to 12: " << sum_of_squares << std::endl;

    auto boost_tuple = boost::hana::make_tuple(sum_of_squares, sum_of_squares * 2, "Boost Hana Example");
    std::cout << "Boost Hana Tuple: ";
    boost::hana::for_each(boost_tuple, [](const auto& x) {
        if constexpr (std::is_same_v<decltype(x), const char*>) {
            std::cout << x << " ";
        } else {
            std::cout << x << " ";
        }
    });
    std::cout << "\n";

    std::vector<int> nums2 = {1, 2, 3, 4, 5};
    std::ranges::for_each(nums2, [](int& n) { // Better to use a simple for loop to update instead of doing like this
        std::cout << "Factorial of " << n << " is " << std::tgamma(n + 1) << std::endl;
        n = std::tgamma(n + 1);
    });

    std::ranges::for_each(nums2, [](const int& n) {
        std::cout << n << " ";
    });
    std::cout << "\n";

    return 0;
}
