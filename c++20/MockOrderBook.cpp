/*
🚀 Mock HFT Interview Question
🧠 Problem:
You're building a high-performance in-memory order book for a small subset of instruments (say, 10 symbols). Each symbol receives up to 1 million messages per second — inserts, updates, and cancels. Each order has:

a unique 64-bit order_id
a price, quantity, and side (BUY or SELL)

You are to implement a single-threaded order book structure that:

Inserts a new order (by order_id)
Updates an existing order (by order_id)
Deletes an order (by order_id)
Can return the top-of-book (best bid and best ask price and size)

Your goal is to:

Maximize lookup speed by order_id
Keep top-of-book queries in sub-microsecond latency
Avoid dynamic allocations if possible

⚠️ Constraints:

You must process up to 1M ops/sec on a single core
Orders are never modified except via update() or cancel()
You will receive orders sorted by timestamp, but order_ids can be arbitrary

🧩 Hints to Consider:

You likely want a cache-friendly hash table for fast order_id → pointer lookup.
Use price levels (price → quantity map) for top-of-book logic.
Think about using flat arrays instead of maps or trees if prices are bounded (e.g., in cents).
Think about arena allocation or fixed-size pools for the orders.

*/

#include <iostream>
#include <array>
#include <vector>
#include <unordered_map>
#include <map>
#include <cstdint>
#include <stdexcept>
#include <cassert>
#include <random>
#include <chrono>

using namespace std;

constexpr size_t POOL_SIZE = 10'000'000; 
constexpr int NUM_ORDERS = 1'000'000;
constexpr int MAX_PRICE_LEVELS = 100'000;
constexpr double TICK_SIZE = 0.01;
constexpr int TICKS_PER_UNIT = 1 / TICK_SIZE;

struct alignas(64) Order {  //  
    uint64_t order_id;
    double price;
    int quantity;
    bool is_buy;
};

class OrderBook {
public:
    OrderBook() : 
            order_count(0), 
            order_pool(POOL_SIZE), 
            best_bid_index(-1), 
            best_ask_index(MAX_PRICE_LEVELS) {
        
        order_memory.reserve(POOL_SIZE); // reserve space for 1 million orders
        for (auto& order : order_pool) {
            order_memory.emplace_back(&order);
        }
    };
    void insert(const Order& order) {
        if (order_count >= POOL_SIZE) {
            throw runtime_error("Order pool is full");
        }
        ++order_count;
        int idx = price_to_index(order.price);
        if (order.is_buy) {
            bid_levels[idx] += order.quantity;
            if (idx > best_bid_index) best_bid_index = idx;
        } else {
            ask_levels[idx] += order.quantity;
            if (idx < best_ask_index) best_ask_index = idx;
        }
        Order* mem = order_memory.back();
        order_memory.pop_back();
        
        mem->order_id = order.order_id;
        mem->price = order.price;
        mem->quantity = order.quantity;
        mem->is_buy = order.is_buy;

        order_map[order.order_id] = mem;
    }
    
    void update(uint64_t order_id, int new_quantity) {
        if (!order_map.contains(order_id)) {
            throw std::runtime_error("Order not found");
        }
        Order* ord = order_map[order_id];

        if (ord->is_buy) {
            update_price_level<true>(ord->price, (new_quantity - ord->quantity));
        } else {
            update_price_level<false>(ord->price, (new_quantity - ord->quantity));
        }
        ord->quantity = new_quantity;
    }
    
    void cancel(uint64_t order_id) {
        if (!order_map.contains(order_id)) {
            throw std::runtime_error("Order not found");
        }
        Order* ord = order_map[order_id];
        if (ord->is_buy) {
            update_price_level<true>(ord->price, -ord->quantity);
        } else {
            update_price_level<false>(ord->price, -ord->quantity);
        }

        order_memory.emplace_back(order_map[order_id]);
        --order_count;
        order_map.erase(order_id); 
    }
    
    std::pair<double, int> best_bid() const {
        double best_bid_price = index_to_price(best_bid_index);
        return {best_bid_price, bid_levels[best_bid_index]};
    }
    std::pair<double, int> best_ask() const {
        double best_ask_price = index_to_price(best_ask_index);
        return {best_ask_price, ask_levels[best_ask_index]};
    }

private:
    inline int price_to_index(double price) const {
        return static_cast<int>(price * TICKS_PER_UNIT);
    }
    inline double index_to_price(int index) const {
        return index * TICK_SIZE;
    }

    template <bool IS_BUY>
    void update_price_level(double price, int update_quantity) {
        int idx = price_to_index(price);
        if constexpr (IS_BUY) {
            bid_levels[idx] += update_quantity;
            if (idx == best_bid_index && bid_levels[idx] == 0) {
                for (int i = idx - 1; i >= 0; --i) {
                    if (bid_levels[i] > 0) {
                        best_bid_index = i;
                        return;
                    }
                }      
            }
            best_bid_index = 0;
        } else {
            ask_levels[idx] += update_quantity;
            if (idx == best_ask_index && ask_levels[idx] == 0) {
                for (int i = idx + 1; i < MAX_PRICE_LEVELS; ++i) {
                    if (ask_levels[i] > 0) {
                        best_ask_index = i;
                        return;
                    }
                }
            }
            best_ask_index = MAX_PRICE_LEVELS - 1;
        }
    }

    vector<Order> order_pool;
    vector<Order*> order_memory; // pointers to active orders
    size_t order_count = 0; // current number of orders
    unordered_map<uint64_t, Order*> order_map; // order_id -> pointer to Order
    std::array<int, MAX_PRICE_LEVELS> bid_levels{};
    std::array<int, MAX_PRICE_LEVELS> ask_levels{};
    int best_bid_index;
    int best_ask_index;
};

/*
$ numactl --physcpubind=4 ./MockOrderBook

🚀 Benchmarking OrderBook with 1000000 orders
🟢 Insert Time: 49 ms → 2.04082e+07 ops/sec
🟡 Update Time: 24 ms → 4.16667e+07 ops/sec
🔴 Cancel Time: 35 ms → 2.85714e+07 ops/sec
✅ Top-of-book empty after all cancels.
*/
void benchmark_orderbook() {
    using namespace std::chrono;

    std::cout << "🚀 Benchmarking OrderBook with " << NUM_ORDERS << " orders\n";

    OrderBook book;
    std::vector<Order> orders;
    orders.reserve(NUM_ORDERS);

    // Generate random test orders
    std::mt19937_64 rng(42);
    std::uniform_real_distribution<double> price_dist(99.5, 100.5);
    std::uniform_int_distribution<int> qty_dist(1, 100);
    std::bernoulli_distribution side_dist(0.5);

    for (uint64_t i = 0; i < NUM_ORDERS; ++i) {
        Order o;
        o.order_id = i;
        o.price = price_dist(rng);
        o.quantity = qty_dist(rng);
        o.is_buy = side_dist(rng);
        orders.push_back(o);
    }

    // Benchmark insert
    auto start_insert = high_resolution_clock::now();
    for (const auto& o : orders) {
        book.insert(o);
    }
    auto end_insert = high_resolution_clock::now();

    // Benchmark update
    auto start_update = high_resolution_clock::now();
    for (auto& o : orders) {
        int delta = (qty_dist(rng) % 20) - 10; // +/- up to 10
        int new_qty = std::max(1, o.quantity + delta);
        book.update(o.order_id, new_qty);
    }
    auto end_update = high_resolution_clock::now();

    // Benchmark cancel
    auto start_cancel = high_resolution_clock::now();
    for (const auto& o : orders) {
        book.cancel(o.order_id);
    }
    auto end_cancel = high_resolution_clock::now();

    auto duration_insert = duration_cast<milliseconds>(end_insert - start_insert).count();
    auto duration_update = duration_cast<milliseconds>(end_update - start_update).count();
    auto duration_cancel = duration_cast<milliseconds>(end_cancel - start_cancel).count();

    std::cout << "🟢 Insert Time: " << duration_insert << " ms → "
              << (NUM_ORDERS * 1000.0 / duration_insert) << " ops/sec\n";

    std::cout << "🟡 Update Time: " << duration_update << " ms → "
              << (NUM_ORDERS * 1000.0 / duration_update) << " ops/sec\n";

    std::cout << "🔴 Cancel Time: " << duration_cancel << " ms → "
              << (NUM_ORDERS * 1000.0 / duration_cancel) << " ops/sec\n";

    // Final check: best bid/ask should be (0, 0)
    auto best_bid = book.best_bid();
    auto best_ask = book.best_ask();
    cout << "Best Bid: " << best_bid.first << " Size: " << best_bid.second << "\n";
    cout << "Best Ask: " << best_ask.first << " Size: " << best_ask.second << "\n";
    assert(best_bid.second == 0 && best_ask.second == 0);

    std::cout << "✅ Top-of-book empty after all cancels.\n";
}

int main() {
    {
        std::cout << "Running OrderBook benchmark...\n";
        benchmark_orderbook();
    }

    OrderBook book;
    try {
        book.insert({1, 100.0, 10, true});
        book.insert({2, 101.0, 5, false});
        book.update(1, 15);
        book.cancel(2);
        auto best_bid = book.best_bid();
        auto best_ask = book.best_ask();
        std::cout << "Best Bid: " << best_bid.first << " Size: " << best_bid.second << "\n";
        std::cout << "Best Ask: " << best_ask.first << " Size: " << best_ask.second << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
    return 0;
}
