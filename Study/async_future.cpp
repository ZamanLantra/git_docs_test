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
    ~IResult() override = default;
    void fetch() override {
        T result = m_future.get();
        cout << "Emitted result is " << result << "\n";
    }
};

int computeSquare(int x, int y) {
    this_thread::sleep_for(chrono::seconds(1));
    return x * y;
}

int main() {
    future<int> result = async(launch::async, computeSquare, 5, 10);

    cout << "Doing other work in main...\n";

    cout << "Waiting for the future...\n";
    int value = result.get(); 
    cout << "Square is: " << value << endl;

    this_thread::sleep_for(chrono::seconds(1));

    vector<shared_ptr<IResult>> results;

    results.emplace_back(make_shared<TaskResult<int>>(async(launch::async, computeSquare, 15, 10)));
    results.emplace_back(make_shared<TaskResult<int>>(async(launch::async, computeSquare, 15, 20)));
    results.emplace_back(make_shared<TaskResult<int>>(async(launch::async, computeSquare, 15, 30)));

    cout << "Doing other work in main...\n";
    this_thread::sleep_for(chrono::seconds(1));

    for (auto& result : results) {
        result->fetch();
    }

    this_thread::sleep_for(chrono::seconds(1));

    return 0;
}
