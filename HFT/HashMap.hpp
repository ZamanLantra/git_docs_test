#pragma once

#include <vector>
#include <list>
#include <array>
#include <stack>
#include <string>
#include <iostream>

namespace Const {
#ifndef HASH_BUCKETS
    constexpr size_t initBuckets = 1 << 8; // 256 - Default bucket count
#else
    constexpr size_t initBuckets = HASH_BUCKETS; // Use user-defined bucket count
#endif
};

template <typename HM>
concept MyHM = requires(HM hm, typename HM::key_type key, typename HM::value_type val) {
    { hm.insert(key, val) } -> std::same_as<void>;
    { hm.contains(key) } -> std::same_as<bool>;
    { hm.remove(key) } -> std::same_as<bool>;
    { hm.find(key) } -> std::same_as<typename HM::value_type*>;
};

/**************************************************************************
Supported HM types include ChainingHashMap, FixedSizedChainingHashMap and 
OpenAddressingHashMap. Check TestHashMap.cpp for usage examples.
**************************************************************************/
template <MyHM HM>
class HashMap {
public:
    HashMap() { }
	HashMap(HashMap const&) = delete;
	HashMap& operator=(HashMap const&) = delete;
    HashMap(HashMap&&) = default;
    HashMap& operator=(HashMap&&) = default;
    void insert(const typename HM::key_type& key, const typename HM::value_type& value) { 
        hashmap_.insert(key, value); }
    bool contains(const typename HM::key_type& key) const { return hashmap_.contains(key); }
    bool remove(const typename HM::key_type& key) { return hashmap_.remove(key); }
    HM::value_type* find(const typename HM::key_type& key) { return hashmap_.find(key); }
private:
    HM hashmap_;
};

/**************************************************************************/
template <typename Key, typename Value>
class ChainingHashMap {
public:
    using key_type = Key;
    using value_type = Value;
    ChainingHashMap() 
            : table_(Const::initBuckets)
            , mask_(Const::initBuckets - 1) {
        std::cout << "ChainingHashMap initialized " << std::endl;
        if (Const::initBuckets == 0 || (Const::initBuckets & (Const::initBuckets - 1)) != 0) {
            throw std::runtime_error("initBuckets must be non-zero and a power of 2");
        }     
    }
    void insert(const Key& key, const Value& value) {
        size_t index = std::hash<Key>()(key) & mask_;
        for (auto& node : table_[index]) {
            if (node.key == key) {
                node.value = value;
                return;
            }
        }
        table_[index].emplace_back(key, value);
    }
    bool contains(const Key& key) const {
        size_t index = std::hash<Key>()(key) & mask_;
        for (const auto& node : table_[index]) {
            if (node.key == key) {
                return true;
            }
        }
        return false;
    }
    bool remove(const Key& key) {
        size_t index = std::hash<Key>()(key) & mask_;
        for (auto& node : table_[index]) {
            if (node.key == key) {
                table_[index].remove(node);
                return true;
            }
        }
        return false;
    }
    Value* find(const Key& key) {
        size_t index = std::hash<Key>()(key) & mask_;
        for (auto& node : table_[index]) {
            if (node.key == key) {
                return &node.value;
            }
        }
        return nullptr;
    }
private:
    struct Node {
        Key key;
        Value value;
        Node(const Key& k, const Value& v) : key(k), value(v) {}
        bool operator==(const Node& other) {
            return key == other.key;
        }
    };
    std::vector<std::list<Node>> table_;
    size_t mask_ = 0;
};

/**************************************************************************/
template <typename Key, typename Value>
class FixedSizedChainingHashMap {
public:
    using key_type = Key;
    using value_type = Value;
    FixedSizedChainingHashMap() 
            : buckets_(Const::initBuckets, nullptr) 
            , nodes_pool_(Const::initBuckets * 16)
            , mask_(Const::initBuckets - 1) {
        std::cout << "FixedSizedChainingHashMap initialized " << std::endl;
        if (Const::initBuckets == 0 || (Const::initBuckets & (Const::initBuckets - 1)) != 0) {
            throw std::runtime_error("initBuckets must be non-zero and a power of 2");
        }
        for (size_t i = 0; i < nodes_pool_.size(); ++i) {
            free_nodes_.push(&nodes_pool_[i]);
        }
    }

    void insert(const Key& key, const Value& value) {
        size_t index = std::hash<Key>()(key) & mask_;
        if (free_nodes_.empty()) {
            throw std::runtime_error("No free nodes available in the pool");
        }
        Node* current = free_nodes_.top(); free_nodes_.pop();
        current->key = key;
        current->value = value;
        current->next = nullptr;
        Node* node = buckets_[index];
        if (node == nullptr) {
            buckets_[index] = current;
            return;
        }
        while (node->next != nullptr) {
            node = node->next;
        }
        node->next = current;
    }
    bool contains(const Key& key) const {
        size_t index = std::hash<Key>()(key) & mask_;
        Node* node = buckets_[index];
        while (node != nullptr) {
            if (node->key == key) {
                return true;
            }
            node = node->next;
        }
        return false;
    }
    bool remove(const Key& key) {
        size_t index = std::hash<Key>()(key) & mask_;
        Node* node = buckets_[index];
        Node* prev = nullptr;
        while (node != nullptr) {
            if (node->key == key) {
                if (prev == nullptr) {
                    buckets_[index] = node->next; // Remove head
                } else {
                    prev->next = node->next; // Remove from middle or end
                }
                free_nodes_.push(node); // Return node to free pool
                return true;
            }
            prev = node;
            node = node->next;
        }
        
        return false;
    }
    Value* find(const Key& key) {
        size_t index = std::hash<Key>()(key) & mask_;
        Node* node = buckets_[index];
        while (node != nullptr) {
            if (node->key == key) {
                return &node->value;
            }
            node = node->next;
        }
        return nullptr;
    }
private:
    struct Node {
        Key key;
        Value value;
        Node* next;
        Node() : key(), value(), next(nullptr) {}
        Node(const Key& k, const Value& v) : key(k), value(v), next(nullptr) {}
    };
    std::vector<Node*> buckets_;
    std::vector<Node> nodes_pool_;
    std::stack<Node*> free_nodes_;
    size_t mask_ = 0;
};

/**************************************************************************/
template <typename Key, typename Value>
class OpenAddressingHashMap {
public:
    using key_type = Key;
    using value_type = Value;
    OpenAddressingHashMap() 
            : table_(Const::initBuckets)
            , mask_(Const::initBuckets - 1) {
        std::cout << "OpenAddressingHashMap initialized " << std::endl;
    }
    void insert(const Key& key, const Value& value) {
        if ((size_ + 1) > table_.size() * maxLoadFactor_) {
            reHash();
        }
        size_t index = getHash(key);
        const size_t originalIndex = index;
        while (table_[index].status == Status::OCCUPIED) {
            if (table_[index].key_ == key) {
                table_[index].value_ = value;
                return;
            }
            index = (index + 1) & mask_;
        }

        table_[index].key_ = key;
        table_[index].value_ = value;
        table_[index].status = Status::OCCUPIED;
        ++size_;
    }
    bool contains(const Key& key) const {
        size_t index = getHash(key);
        const size_t originalIndex = index;

        while (table_[index].status != Status::EMPTY) {
            if (table_[index].status == Status::OCCUPIED && table_[index].key_ == key) {
                return true;
            }
            index = (index + 1) & mask_;
            if (index == originalIndex) {
                break;
            }
        }
        return false;
    }
    bool remove(const Key& key) {
        size_t index = getHash(key);
        const size_t originalIndex = index;

        while (table_[index].status != Status::EMPTY) {
            if (table_[index].status == Status::OCCUPIED && table_[index].key_ == key) {
                table_[index].status = Status::DELETED;
                --size_;
                return true;
            }
            index = (index + 1) & mask_;
            if (index == originalIndex) {
                break;
            }
        }

        return false;
    }
    Value* find(const Key& key) {
        size_t index = getHash(key);
        const size_t originalIndex = index;

        while (table_[index].status != Status::EMPTY) {
            if (table_[index].status == Status::OCCUPIED && table_[index].key_ == key) {
                return &table_[index].value_;
            }
            index = (index + 1) & mask_;
            if (index == originalIndex) {
                break;
            }
        }
        return nullptr;
    }
private:
    enum class Status { EMPTY, OCCUPIED, DELETED };
    struct Node {
        Key key_;
        Value value_;
        Status status = Status::EMPTY;
    };
    std::vector<Node> table_;
    size_t size_ = 0;
    size_t mask_ = 0;
    float maxLoadFactor_ = 0.7f;

    size_t getHash(const Key& key) const {
        return std::hash<Key>()(key) & mask_;
    }
    void reHash() {
        std::vector<Node> oldTable_ = std::move(table_);
        size_t newSize = oldTable_.size() * 2;
        mask_ = newSize - 1;
        table_ = std::vector<Node>(newSize);
        size_ = 0;
        for (const auto& node : oldTable_) {
            if (node.status == Status::OCCUPIED) {
                insert(node.key_, node.value_);
            }
        }
    }
};

/**************************************************************************/
