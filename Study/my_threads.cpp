// g++ -std=c++20 -O3 -pthread -o my_threads my_threads.cpp 

#include <iostream>
#include <thread>
#include <chrono>
#include <queue>
#include <condition_variable>

// --------- Test 1 --------------------------------------------
void count(std::stop_token stoken) {
    int i = 0;
    while (!stoken.stop_requested()) {
        std::cout << "Count: " << i++ << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    std::cout << "Counter stopped.\n";
}

// --------- Test 2 --------------------------------------------
void worker(std::stop_token stoken, int id) {
    while (!stoken.stop_requested()) {
        std::cout << "Thread " << id << " working...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    std::cout << "Thread " << id << " stopping.\n";
}

// --------- Test 3 --------------------------------------------
std::mutex mtx1;
std::condition_variable cv1;
std::queue<int> q1;

void producer() {
    for (int i = 0; i < 5; ++i) {
        {
            std::lock_guard<std::mutex> lock(mtx1);
            q1.push(i);
            std::cout << "Produced " << i << "\n";
        }
        cv1.notify_one();  // Wake up one waiting thread
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void consumer() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx1);
        cv1.wait(lock, [] { return !q1.empty(); });  // Spurious wakeup safe!
        int val = q1.front();
        q1.pop();
        lock.unlock();
        std::cout << "Consumed " << val << "\n";
        if (val == 4) break;
    }
}

// --------- Test 4 --------------------------------------------
std::mutex mtx2;
void worker_polling(std::stop_token stoken, bool& data_ready, std::condition_variable& cv2) {
    while (!stoken.stop_requested()) {
        std::unique_lock<std::mutex> lock(mtx2);
        cv2.wait_for(lock, std::chrono::milliseconds(500), [&] { return data_ready; });
        if (data_ready) {
            std::cout << "Data is ready! Processing data...\n";
            data_ready = false;
        } else {
            std::cout << "Woke up after timeout. But data is not ready...\n";
        }
    }
    std::cout << "Worker exiting gracefully.\n";
}

int main() {
    std::cout << "--------- Test 1 --------------------------------------------" << std::endl;
    std::jthread counterThread1(count);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    counterThread1.request_stop();  // Graceful shutdown
    
    std::cout << "--------- Test 2 --------------------------------------------" << std::endl;
    std::stop_source ssrc;
    std::vector<std::jthread> threads;
    for (int i = 0; i < 4; ++i)
        threads.emplace_back(worker, ssrc.get_token(), i);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    ssrc.request_stop();  // Stop all threads at once
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    std::cout << "--------- Test 3 --------------------------------------------" << std::endl;
    std::thread t1(producer);
    std::thread t2(consumer);
    t1.join();
    t2.join();
    
    std::cout << "--------- Test 4 --------------------------------------------" << std::endl;
    bool data_ready = false;
    std::condition_variable cv2;
    std::jthread pollingWorker(worker_polling, std::ref(data_ready), std::ref(cv2));
    std::this_thread::sleep_for(std::chrono::seconds(1));
    data_ready = true;
    cv1.notify_one();  // Wake up one waiting thread
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    data_ready = true;
    cv1.notify_one();  // Wake up one waiting thread
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    pollingWorker.request_stop();  // Graceful shutdown
}
