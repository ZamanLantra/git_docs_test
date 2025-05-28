// g++ -std=c++20 challenge1.cpp -o challenge1

#include <iostream>
#include <vector>
#include <numeric>
#include <ranges>

int factorial(int n) {
    int res = 1;
    for (int i = 2; i <= n; ++i) res *= i;
    return res;
}

void fatorial_challenge(const std::vector<int>& nums) {
    std::cout << "Factorial Challenge\n";
    
    // auto fact = nums | std::views::transform([](int n) {
    //       int f = factorial(n);
    //       std::cout << "fatorial_challenge:" << n << " fact:"<< f << std::endl;
    //       return f;
    //     });
    // auto sum_of_factorals = std::accumulate(fact.begin(), fact.end(), 0);
    
    auto sum_of_factorals = std::accumulate(nums.begin(), nums.end(), 0, 
                                [] (auto sum, auto n) { 
                                    int f = factorial(n);
                                    std::cout << "fatorial_challenge:" << n << " fact:"<< f << std::endl;
                                    return sum + f;
                                });
    
    std::cout << "Sum is " << sum_of_factorals << std::endl;
}

// class Tester {
// public:
//     Tester() {
//         std::cout << "Tester created" << std::endl;
//     }
    
//     ~Tester() {
//         std::cout << "Tester destroyed" << std::endl;
//     }

//     void begin() {
//         std::cout << "Begin testing" << std::endl;
//     }

//     void end() {
//         std::cout << "End testing" << std::endl;
//     }
// };

template <typename T>
concept Iterable = requires(T t) {
    { t.begin() } -> std::same_as<decltype(t.end())>;
};

template <Iterable T>
void printIterable(const T& iterable) {
    for (const auto& item : iterable) {
        std::cout << item << " ";
    }
    std::cout << std::endl;
}

int main() {
    
    std::vector<int> nums = {1,2,3,4,5};
    fatorial_challenge(nums);
    
    printIterable(nums);
    printIterable(nums | std::views::transform([](int n) {
                                return factorial(n);
                            })
    );
    
    printIterable(std::string("Hello World"));

    // Tester tester;
    // printIterable(tester);
    
    return 0;
}