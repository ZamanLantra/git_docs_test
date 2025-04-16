// zamanlantra@ZamansMcBookPro Study % g++ -std=c++17 -o callbacks callbacks.cpp 
// zamanlantra@ZamansMcBookPro Study % ./callbacks 


#include <functional>
#include <iostream>
#include <thread>

using namespace std;

// =============== Passing Function Pointers ===============
void perform(std::function<void(int)> cb) {
    cb(99);
}

void func1(int x) { cout << "Inside Func1 got: " << x << "\n"; }
void func2(int x) { cout << "Inside Func2 got: " << x << "\n"; }

class ICallback {
public:
    virtual ~ICallback() = default;
    virtual void onDone() = 0;
};

// =============== Using interfaces 1 ===============
class Worker
{
public:
    Worker(ICallback& callback) : m_callback(callback) {}
    ~Worker() { }
    void DoSomething() {
        cout << "Worker did work\n";
        m_callback.onDone();
    }
private:
    ICallback& m_callback;
};

class Boss : public ICallback {
public:
    Boss() = default;
    ~Boss() = default;

    void onDone() override {
        cout << "Boss signs\n";
    }
};

// =============== Using interfaces 2 ===============
class IBroadcast {
public:
    virtual ~IBroadcast() = default;
    virtual void onEvent(int code, const std::string& message) = 0;
};

class Logger : public IBroadcast {
    string m_name = "";
public:
    Logger(string name) : m_name(name) {}
    void onEvent(int code, const std::string& message) override {
        std::cout << "[Logger: " << m_name << "] Event code: " << code << ", message: " << message << std::endl;
    }
};

class EventHandler {
private:
    vector<shared_ptr<IBroadcast>> loggers;
public:
    EventHandler() = default;
    void RegisterLoggers(shared_ptr<IBroadcast> logger) {
        loggers.emplace_back(std::move(logger));
    }
    void Broadcast(int code, const std::string& message) {
        for (auto& logger : loggers) {
            logger->onEvent(code, message);
        }
    }
};

// =============== Using interfaces 3 - But with threads - fire and forget ===============
class EventEmitter {
private:
    std::vector<std::shared_ptr<IBroadcast>> listeners;
public:
    EventEmitter() = default;
    void RegisterLoggers(std::shared_ptr<IBroadcast> cb) {
        listeners.emplace_back(std::move(cb));
    }
    void Broadcast(int code, const std::string& msg) {
        for (auto& cb : listeners) {
            std::thread([cb, code, msg]() {
                cb->onEvent(code, msg);
            }).detach();
        }
    }
};

// =============== Main ===============
int main() {
    perform([](int x) {
        cout << "Lambda got: " << x << "\n";
    });
    perform(func1);
    perform(func2);

    Boss boss;
    Worker worker(boss);
    worker.DoSomething();

    EventHandler events;
    events.RegisterLoggers(make_shared<Logger>("1"));
    events.RegisterLoggers(make_shared<Logger>("2"));
    events.RegisterLoggers(make_shared<Logger>("3"));
    events.Broadcast(1000, "This is where kingdoms are made");

    EventEmitter emitter;
    emitter.RegisterLoggers(make_shared<Logger>("Thread1"));
    emitter.RegisterLoggers(make_shared<Logger>("Thread2"));
    emitter.RegisterLoggers(make_shared<Logger>("Thread3"));
    emitter.Broadcast(1001, "Come on");

    std::this_thread::sleep_for(std::chrono::seconds(2));
}

// Lambda got: 99
// Inside Func1 got: 99
// Inside Func2 got: 99
// Worker did work
// Boss signs
// [Logger: 1] Event code: 1000, message: This is where kingdoms are made
// [Logger: 2] Event code: 1000, message: This is where kingdoms are made
// [Logger: 3] Event code: 1000, message: This is where kingdoms are made
// [Logger: Thread1] Event code: 1001, message: Come on
// [Logger: Thread2] Event code: 1001, message: Come on
// [Logger: Thread3] Event code: 1001, message: Come on