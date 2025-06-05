#include <coroutine>
#include <iostream>
#include <optional>

template<typename T>
struct Generator {
    struct promise_type {
        T current_value;
        std::suspend_always yield_value(T value) {
            current_value = value;
            return {};
        }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        Generator get_return_object() {
            return Generator{ 
                std::coroutine_handle<promise_type>::from_promise(*this)
            };
        }
        void return_void() {}
        void unhandled_exception() { std::exit(1); }
    };

    std::coroutine_handle<promise_type> corout;

    explicit Generator(std::coroutine_handle<promise_type> h) : corout(h) {}
    ~Generator() { 
        if (corout) corout.destroy(); 
    }

    std::optional<T> next() {
        if (!corout.done()) {
            corout.resume();
            return corout.promise().current_value;
        }
        return {};
    }
};

Generator<int> count_to(int n) {
    for (int i = 1; i <= n; ++i) {
        std::cout << "i" << i << std::endl;
        co_yield i;  // yield each value
    }
}

int main() {
    auto gen = count_to(5); // Lazy evaluation
    while (auto val = gen.next()) {
        std::cout << *val << "\n";  // prints 1 2 3 4 5
    }
}
