#pragma once

#include <vector>
#include <atomic>
#include <iostream>
#include <concepts>

template <typename T>
concept MsgPtr = std::is_pointer_v<T>;

template <typename Q>
concept MyQ = requires(Q q, typename Q::value_type ptr) {
    { q.enqueue(ptr) } -> std::convertible_to<bool>;
    { q.dequeue() } -> std::convertible_to<typename Q::value_type>;
};

template <MyQ Q>
class Queue {
public:
    Queue(size_t capacity) : queue_(capacity) { }
    bool enqueue(Q::value_type ptr) { return queue_.enqueue(ptr); }
    Q::value_type dequeue() { return queue_.dequeue(); }
private:
    Q queue_;
};

template <MsgPtr T>
class CustomSPMCLockFreeQueue {
public:
    using value_type = T;
    CustomSPMCLockFreeQueue(size_t capacity) 
            : buffer_(capacity)
            , capacity_(capacity)
            , mask_(capacity - 1) {
        if (capacity_ == 0 || (capacity_ & mask_) != 0) {
            throw std::invalid_argument("Capacity must be a power of two and greater than zero.");
        }
    }

    inline bool enqueue(T ptr) {
        const size_t tail = tail_.load(std::memory_order_relaxed);
        const size_t next_tail = (tail + 1);
        if (next_tail - head_.load(std::memory_order_acquire) > capacity_) {
            return false;
        }
        buffer_[tail & mask_] = std::move(ptr);
        tail_.store(next_tail, std::memory_order_release);
        return true;
    }

    inline T dequeue() {
        if (head_.load(std::memory_order_relaxed) == tail_.load(std::memory_order_relaxed)) {
            return nullptr; 
        }
        const size_t head = head_.fetch_add(1, std::memory_order_acq_rel);
        return buffer_[head & mask_];
    }

private:
    std::vector<T> buffer_;
    alignas(64) std::atomic<size_t> head_{ 0 };
    alignas(64) std::atomic<size_t> tail_{ 0 };
    size_t capacity_{ 0 };
    size_t mask_{ 0 };
};



