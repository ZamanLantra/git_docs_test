// g++ -std=c++20 challenge6.cpp -o challenge6 -I./RxCpp/Rx/v2/src

#include <rxcpp/rx.hpp>
#include <iostream>
#include <chrono>

namespace rx = rxcpp;

rx::observable<int> generate_values() {
    return rx::observable<>::create<int>([](rx::subscriber<int> s) {
        for (int i = 1; i <= 10; ++i) {
            s.on_next(i);
        }
        s.on_completed();
    });
}

int main() {
    // Create an observable that emits integers from 1 to 5
    auto source = rx::observable<>::range(1, 5);

    // Subscribe to the observable and print each emitted value
    source.subscribe(
        [](int v) { std::cout << "Value: " << v << std::endl; },
        []() { std::cout << "Completed!" << std::endl; }
    );

    auto stream = generate_values()
                    .filter([](int v) { return v % 2 == 0; })
                    .transform([](int v) { return v * 10; });

    stream.subscribe(
        [](int v) { std::cout << "Generated Value: " << v << std::endl; },
        []() { std::cout << "Generation Completed!" << std::endl; }
    );

    auto source1 = rx::observable<>::interval(std::chrono::milliseconds(500)) // nanoseconds(1)
                        // .take(5)
                        .map([](long v) { return "Data Point " + std::to_string(v); });
    
    source1.subscribe(
        [](const std::string& v) { std::cout << "Received: " << v << std::endl; },
        []() { std::cout << "Stream Completed!" << std::endl; }
    );

    std::this_thread::sleep_for(std::chrono::seconds(3)); // Wait for the streams to complete

    return 0;
}