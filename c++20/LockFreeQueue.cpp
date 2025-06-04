#include <atomic>
#include <vector>
#include <cstddef>

template<typename T>
class LockFreePool {
public:
    explicit LockFreePool(size_t capacity)
        : capacity_(capacity),
          storage_(capacity),
          freelist_(capacity),
          head_(0),
          tail_(0)
    {
        // Initialize freelist to contain all slots initially
        for (size_t i = 0; i < capacity_; ++i) {
            freelist_[i] = &storage_[i];
        }
        head_.store(capacity_);
        tail_.store(0);
    }

    // Acquire an object from the pool
    T* acquire() {
        size_t tail = tail_.load(std::memory_order_acquire);
        size_t head = head_.load(std::memory_order_acquire);

        if (tail == head) {
            // Pool exhausted
            return nullptr;
        }

        T* obj = freelist_[tail % capacity_];
        tail_.store((tail + 1) % (2 * capacity_), std::memory_order_release);
        return obj;
    }

    // Release an object back to the pool
    void release(T* obj) {
        size_t head = head_.load(std::memory_order_acquire);

        if ((head - tail_.load(std::memory_order_relaxed)) == capacity_) {
            // Pool overflow (should never happen if users respect pool size)
            throw std::runtime_error("Pool overflow");
        }

        freelist_[head % capacity_] = obj;
        head_.store((head + 1) % (2 * capacity_), std::memory_order_release);
    }

private:
    const size_t capacity_;
    std::vector<T> storage_;           // Actual object storage (cache-friendly)
    std::vector<T*> freelist_;         // Ring buffer of free slots
    std::atomic<size_t> head_;         // Next slot to produce (release)
    std::atomic<size_t> tail_;         // Next slot to consume (acquire)
};

struct MarketDataMsg {
    char msg_type;
    uint64_t timestamp;
    double price;
    uint32_t size;
};

int main() {
    constexpr size_t capacity = 10;
  
    LockFreePool<MarketDataMsg> pool(capacity);


    
    return 0;
}
