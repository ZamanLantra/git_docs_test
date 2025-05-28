
// g++ -std=c++20 BoundedBlockingQueue.cpp -o BoundedBlockingQueue

#include <iostream>
#include <queue>
#include <thread>
#include <mutex>

using namespace std;

class BoundedBlockingQueue {
private:
    int capacity;
    queue<int> q;
    mutex mtx;
    condition_variable cv_enqueue;
    condition_variable cv_dequeue;
public:
    BoundedBlockingQueue(int capacity) : capacity(capacity) {

    }
    void enqueue(int element) {
        unique_lock<mutex> lock(mtx);
        cv_enqueue.wait(lock, [this] { return q.size() < capacity; });
        q.push(element);
        cout << "Enqueued: " << element << endl;
        cv_dequeue.notify_one();
    }
    int dequeue() {
        unique_lock<mutex> lock(mtx);
        cv_dequeue.wait(lock, [this] { return !q.empty(); });
        int front = q.front();
        q.pop();
        cout << "Dequeued: " << front << endl;
        cv_enqueue.notify_one();
        return front;
    }
    int size() {
        lock_guard<mutex> lock(mtx);
        return q.size();
    }
};

int main() {
    std::cout << "This is a placeholder for BoundedBlockingQueue.cpp" << std::endl;
    BoundedBlockingQueue q(2);

    std::thread producer([&] {
        q.enqueue(1);
        q.enqueue(2);
        q.enqueue(3); // blocks until dequeue frees space
    });

    std::thread consumer([&] {
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        std::cout << q.dequeue() << std::endl; // prints 1
        std::cout << q.dequeue() << std::endl; // prints 2
        std::cout << q.dequeue() << std::endl; // prints 3
    });

    producer.join();
    consumer.join();

    return 0;
}