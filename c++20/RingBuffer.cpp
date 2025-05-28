// g++ -std=c++20 RingBuffer.cpp -o RingBuffer

#include <iostream>
#include <vector>
#include <tuple>

using namespace std;

class MarketDataRingBuffer {
private:
    vector<tuple<int, double, double>> ring_buffer; // timestamp, bidPrice, askPrice
    int current_index = -1, m_capacity, m_cap_mask;
public:
    MarketDataRingBuffer(int capacity) : m_capacity(capacity), m_cap_mask(capacity-1) {
        assert((capacity & m_cap_mask) == 0 && "Capacity must be power of two");
        ring_buffer.resize(capacity);
    }

    void insert(int timestamp, double bidPrice, double askPrice) {
        ring_buffer[++current_index % m_capacity] = {timestamp, bidPrice, askPrice};
    }

    vector<tuple<int, double, double>> getLastNSnapshots(int n) {   
        n = min(n, current_index + 1);
        
        vector<tuple<int, double, double>> snapshot;
        snapshot.reserve(m_capacity);
        
        int offset = (current_index + 1 - n + m_capacity) & m_cap_mask;
        
        for (int i = 0; i < n; ++i) {
            snapshot.emplace_back(ring_buffer[(i + offset) & m_cap_mask]);
        }

        return snapshot;
    }
};

int main() {

    MarketDataRingBuffer buffer(4);
    buffer.insert(100, 49.5, 50.0);
    buffer.insert(101, 49.7, 50.2);
    buffer.insert(102, 49.6, 50.1);
    buffer.insert(103, 49.8, 50.3);
    
    auto result = buffer.getLastNSnapshots(2);

    for (const auto& [a, b, c] : result) {
        cout << "{" << a << ", " << b << ", " << c << "}\n";
    }

    // Should return:
    // [
    //   {102, 49.6, 50.1},
    //   {103, 49.8, 50.3}
    // ]

    return 0;
}