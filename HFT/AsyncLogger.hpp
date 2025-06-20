#pragma once

#include <atomic>
#include <thread>
#include <iostream>
#include <stdexcept>
#include "Queue.hpp"
#include "MemoryPool.hpp"

namespace Const {
#ifdef LOG_BUFFER_SIZE
    constexpr size_t LogBufferSize = LOG_BUFFER_SIZE;
#else
    constexpr size_t LogBufferSize = 512;
#endif
};

struct alignas(64) LogMsg {
    char buffer[Const::LogBufferSize];
    size_t len = 0;
};

class AsyncLogger {
public:
    using LogMsgPtr = LogMsg*;
    AsyncLogger(std::ostream& outStream) 
            : outStream_(outStream)
            , runFlag_(true) {
        
        loggerThread_ = std::thread(&AsyncLogger::LoggerThread, this);
    }
    ~AsyncLogger() {
        runFlag_ = false;
        if (loggerThread_.joinable())
            loggerThread_.join();
        outStream_.flush();
    }

    template<typename... Args>
    void log(const char* fmt, Args&&... args) {
        LogMsgPtr msg = pool_.allocate();
        if (!msg) {
            throw std::runtime_error("Logger Pool Exhausted");
        }

        int len = std::snprintf(msg->buffer, sizeof(msg->buffer), fmt, std::forward<Args>(args)...);
        if (len < 0) {
            pool_.deallocate(msg);
            throw std::runtime_error("Encoding error during formatting");
        }
        msg->len = (len < (int)sizeof(msg->buffer)) ? len : (int)sizeof(msg->buffer) - 1;

        queue_.enqueue(msg);
    }

private:
    void LoggerThread() {
        uint32_t spin = 0;
        while (runFlag_.load()) {
            if (LogMsgPtr msg = queue_.dequeue()) {
                outStream_.write(msg->buffer, msg->len);
                pool_.deallocate(msg);
                spin = 0;
            } 
            else if (++spin < 1000) {
                std::this_thread::yield();                                  // Fast retry (cheap when hot)
            } 
            else {
                std::this_thread::sleep_for(std::chrono::microseconds(50)); // Idle fallback
                spin = 0;
            }
        }       
    }

    std::ostream& outStream_;
    std::thread loggerThread_;
    alignas(64) std::atomic<bool> runFlag_;
    Queue<CustomMPMCLockFreeQueue<LogMsgPtr>> queue_;
    MemoryPool<LockFreeThreadSafePool<LogMsg, true>> pool_;
};
