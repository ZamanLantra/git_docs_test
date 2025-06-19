#pragma once

#include <iostream>
#include <array>
#include <vector>
#include "HashMap.hpp"
#include <cstdint>
#include <stdexcept>
#include <cassert>
#include <random>
#include <chrono>

namespace Const {
    constexpr size_t PoolSize = 10'000'000; 
    constexpr size_t NumOrders = 1'000'000; 
    constexpr size_t MaxPriceLevels = 100'000; 
    constexpr double TickSize = 0.01; 
    constexpr size_t TicksPerUnit = static_cast<size_t>(1 / TickSize);
}

struct alignas(64) Order {
    uint64_t order_id;
    double price;
    int quantity;
    bool is_buy;
};

template <bool RequireStorage>
class OrderBook {
public:
    using OrderPtr = Order*;
    OrderBook() 
            : bestBidIndex_(-1)
            , bestAskIndex_(Const::MaxPriceLevels) {
        
        if constexpr (RequireStorage) {
            freeMsgPtrs_.reserve(Const::PoolSize); // reserve space for Const::PoolSize orders
            for (auto& order : orderPool_) {
                freeMsgPtrs_.emplace_back(&order);
            }
        }
    };
    void insert(OrderPtr order) {
        OrderPtr mem = nullptr;
        if constexpr (RequireStorage) {
            if (orderCount_ >= Const::PoolSize) {
                throw std::runtime_error("Order pool is full");
            }
            ++orderCount_;
            mem = freeMsgPtrs_.back();
            freeMsgPtrs_.pop_back();
            
            mem->order_id = order->order_id;
            mem->price = order->price;
            mem->quantity = order->quantity;
            mem->is_buy = order->is_buy;
        }
        else {
            mem = order;
        }
        orderMap_[order->order_id] = mem;

        // Update price levels
        int idx = priceToIndex(order->price);
        if (order->is_buy) {
            bidLevels_[idx] += order->quantity;
            if (idx > bestBidIndex_) 
                bestBidIndex_ = idx;
        } 
        else {
            askLevels_[idx] += order->quantity;
            if (idx < bestAskIndex_) 
                bestAskIndex_ = idx;
        }
    }
    
    void update(uint64_t order_id, int new_quantity) {
        if (!orderMap_.contains(order_id)) {
            throw std::runtime_error("Order not found");
        }
        OrderPtr ord = orderMap_[order_id];

        if (ord->is_buy) {
            updatePriceLevel<true>(ord->price, (new_quantity - ord->quantity));
        } 
        else {
            updatePriceLevel<false>(ord->price, (new_quantity - ord->quantity));
        }
        ord->quantity = new_quantity;
    }
    
    void cancel(uint64_t order_id) {
        if (!orderMap_.contains(order_id)) {
            throw std::runtime_error("Order not found");
        }
        OrderPtr ord = orderMap_[order_id];
        if (ord->is_buy) {
            updatePriceLevel<true>(ord->price, -ord->quantity);
        } 
        else {
            updatePriceLevel<false>(ord->price, -ord->quantity);
        }
        if constexpr (RequireStorage) {
            freeMsgPtrs_.emplace_back(orderMap_[order_id]);
            --orderCount_;
        }
        orderMap_.erase(order_id); 
    }
    
    std::pair<double, int> bestBid() const {
        double bestBidPrice = indexToPrice(bestBidIndex_);
        return { bestBidPrice, bidLevels_[bestBidIndex_] };
    }
    std::pair<double, int> bestAsk() const {
        double bestAskPrice = indexToPrice(bestAskIndex_);
        return { bestAskPrice, askLevels_[bestAskIndex_] };
    }

private:
    inline int priceToIndex(double price) const {
        return static_cast<int>(price * Const::TicksPerUnit);
    }
    inline double indexToPrice(int index) const {
        return index * Const::TickSize;
    }

    template <bool IS_BUY>
    void updatePriceLevel(double price, int updateQuantity) {
        const int idx = priceToIndex(price);
        if constexpr (IS_BUY) {
            bidLevels_[idx] += updateQuantity;
            if (idx == bestBidIndex_ && bidLevels_[idx] == 0) {
                for (int i = idx - 1; i >= 0; --i) {
                    if (bidLevels_[i] > 0) {
                        bestBidIndex_ = i;
                        return;
                    }
                }      
            }
            bestBidIndex_ = 0;
        } else {
            askLevels_[idx] += updateQuantity;
            if (idx == bestAskIndex_ && askLevels_[idx] == 0) {
                for (int i = idx + 1; i < Const::MaxPriceLevels; ++i) {
                    if (askLevels_[i] > 0) {
                        bestAskIndex_ = i;
                        return;
                    }
                }
            }
            bestAskIndex_ = Const::MaxPriceLevels - 1;
        }
    }

    std::vector<Order> orderPool_;              // used only if RequireStorage is true
    std::vector<OrderPtr> freeMsgPtrs_;         // used only if RequireStorage is true
    size_t orderCount_ = 0;                     // used only if RequireStorage is true  
    HashMap<FixedSizedChainingHashMap<uint64_t, OrderPtr>> orderMap_; // order_id -> pointer to Order
    std::array<int, Const::MaxPriceLevels> bidLevels_{};
    std::array<int, Const::MaxPriceLevels> askLevels_{};
    int bestBidIndex_;
    int bestAskIndex_;
};

// HashMap types : ChainingHashMap, FixedSizedChainingHashMap, OpenAddressingHashMap, STLHashMap