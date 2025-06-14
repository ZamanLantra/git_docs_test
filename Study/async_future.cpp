// zamanlantra@ZamansMcBookPro Study % g++ -std=c++17 -o async_future async_future.cpp 
// zamanlantra@ZamansMcBookPro Study % ./async_future 


#include <iostream>
#include <future>
#include <chrono>

using namespace std;

class IResult {
public: 
    virtual ~IResult() = default;
    virtual void fetch() = 0;
};

template <typename T>
class TaskResult : public IResult {
private:
    future<T> m_future;
public:
    TaskResult(future<T>&& fut) : m_future(std::move(fut)) {}
    ~TaskResult() override = default;
    void fetch() override {
        T result = m_future.get();
        cout << "Emitted result is " << result << "\n";
    }
};

template <typename T>
T computeMultiple(T x, T y) {
    this_thread::sleep_for(chrono::seconds(1));
    return x * y;
}

void produce_value(std::promise<int> prom) {
    std::this_thread::sleep_for(std::chrono::seconds(2)); // simulate work
    prom.set_value(42); // send value to future
}

int main() {
    future<int> result = async(launch::async, computeMultiple<int>, 5, 10);

    cout << "Doing other work in main...\n";

    cout << "Waiting for the future...\n";
    int value = result.get(); 
    cout << "Square is: " << value << endl;

    this_thread::sleep_for(chrono::seconds(1));

    vector<shared_ptr<IResult>> results;

    results.emplace_back(make_shared<TaskResult<int>>(async(launch::async, computeMultiple<int>, 15, 10)));
    results.emplace_back(make_shared<TaskResult<double>>(async(launch::async, computeMultiple<double>, 15.005, 20.995)));
    results.emplace_back(make_shared<TaskResult<int>>(async(launch::async, computeMultiple<int>, 15, 30)));

    cout << "Doing other work in main...\n";
    this_thread::sleep_for(chrono::seconds(1));

    for (auto& result : results) {
        result->fetch();
    }

    this_thread::sleep_for(chrono::seconds(1));

    promise<int> prom;
    future<int> fut = prom.get_future();

    thread producer(produce_value, std::move(prom));

    cout << "Waiting for promissed value...\n";
    int valueX = fut.get(); // blocks until promise sets value
    cout << "Received: " << valueX << "\n";

    producer.join();
    return 0;
}
