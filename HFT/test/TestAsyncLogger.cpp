// g++ -std=c++20 -O3 TestAsyncLogger.cpp -o TestAsyncLogger

#include "../AsyncLogger.hpp"
#include <thread>
#include <vector>
#include <chrono>
#include <fstream>

int main() {
    { // Using std::cout to write the log
        AsyncLogger logger(std::cout);

        const int numThreads = 10;
        const int messagesPerThread = 20;

        std::vector<std::thread> workers;

        for (int i = 0; i < numThreads; ++i) {
            workers.emplace_back(
                [i, &logger]() {
                    for (int j = 0; j < messagesPerThread; ++j) {
                        logger.log("Thread %d: MessageID = %d\n", i, j);
                        std::this_thread::sleep_for(std::chrono::milliseconds(10 * (i + 1)));
                    }
                }
            );
        }

        for (auto& t : workers) {
            t.join();
        }
    }

    { // Using a file to write the log
        std::ofstream file("log_file.txt");
        AsyncLogger logger(file);

        const int numThreads = 10;
        const int messagesPerThread = 20;

        std::vector<std::thread> workers;

        for (int i = 0; i < numThreads; ++i) {
            workers.emplace_back(
                [i, &logger]() {
                    for (int j = 0; j < messagesPerThread; ++j) {
                        logger.log("Thread %d: MessageID = %d\n", i, j);
                        std::this_thread::sleep_for(std::chrono::milliseconds(10 * (i + 1)));
                    }
                }
            );
        }

        for (auto& t : workers) {
            t.join();
        }
    }
    return 0;
}
