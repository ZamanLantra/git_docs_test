#pragma once

#include <vector>
#include <list>
#include <array>
#include <stack>
#include <string>
#include <iostream>

using namespace std;

template <typename Key, typename Value>
class ChainingHashTable {
private:
    struct Node {
        Key key;
        Value value;
        Node(const Key& k, const Value& v) : key(k), value(v) {}
        bool operator==(const Node& other) {
            return key == other.key;
        }
    };
    vector<list<Node>> table_;
public:
    ChainingHashTable(size_t size = 16) : table_(size) {
        cout << "ChainingHashTable initialized " << endl;
    }
    void insert(const Key& key, const Value& value) {
        size_t index = std::hash<Key>()(key) % table_.size();
        for (auto& node : table_[index]) {
            if (node.key == key) {
                node.value = value;
                return;
            }
        }
        table_[index].emplace_back(key, value);
    }
    bool contains(const Key& key) const {
        size_t index = std::hash<Key>()(key) % table_.size();
        for (const auto& node : table_[index]) {
            if (node.key == key) {
                return true;
            }
        }
        return false;
    }
    bool remove(const Key& key) {
        size_t index = std::hash<Key>()(key) % table_.size();
        for (auto& node : table_[index]) {
            if (node.key == key) {
                table_[index].remove(node);
                return true;
            }
        }
        return false;
    }
    Value* find(const Key& key) {
        size_t index = std::hash<Key>()(key) % table_.size();
        for (auto& node : table_[index]) {
            if (node.key == key) {
                return &node.value;
            }
        }
        return nullptr;
    }
};

template <typename Key, typename Value>
class FixedSizedChainingHashTable {
private:
    struct Node {
        Key key;
        Value value;
        Node* next;
        Node() : key(), value(), next(nullptr) {}
        Node(const Key& k, const Value& v) : key(k), value(v), next(nullptr) {}
    };
    vector<Node*> table_;
    array<Node, 1024> nodes_pool_;
    stack<Node*> free_nodes_;
public:
    FixedSizedChainingHashTable(size_t size = 2) : table_(size, nullptr) {
        cout << "FixedSizedChainingHashTable initialized " << endl;
        for (size_t i = 0; i < nodes_pool_.size(); ++i) {
            free_nodes_.push(&nodes_pool_[i]);
        }
    }

    void insert(const Key& key, const Value& value) {
        size_t index = std::hash<Key>()(key) % table_.size();
        if (free_nodes_.empty()) {
            throw std::runtime_error("No free nodes available in the pool");
        }

        Node* current = free_nodes_.top(); free_nodes_.pop();
        current->key = key;
        current->value = value;
        current->next = nullptr;
        
        Node* node = table_[index];
        if (node == nullptr) {
            table_[index] = current;
            return;
        }
        while (node->next != nullptr) {
            node = node->next;
        }
        node->next = current;
    }
    bool contains(const Key& key) const {
        size_t index = std::hash<Key>()(key) % table_.size();
        Node* node = table_[index];
        while (node != nullptr) {
            if (node->key == key) {
                return true;
            }
            node = node->next;
        }
        return false;
    }
    bool remove(const Key& key) {
        size_t index = std::hash<Key>()(key) % table_.size();
        Node* node = table_[index];
        Node* prev = nullptr;
        while (node != nullptr) {
            if (node->key == key) {
                if (prev == nullptr) {
                    table_[index] = node->next; // Remove head
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
        size_t index = std::hash<Key>()(key) % table_.size();
        Node* node = table_[index];
        while (node != nullptr) {
            if (node->key == key) {
                return &node->value;
            }
            node = node->next;
        }
        return nullptr;
    }
};

template <typename Key, typename Value>
class OpenAddressingHashTable {
private:
    enum class Status { EMPTY, OCCUPIED, DELETED };
    struct Node {
        Key key_;
        Value value_;
        Status status = Status::EMPTY;
    };
    vector<Node> table_;
    size_t size_ = 0;
    float maxLoadFactor_ = 0.7f;

    size_t getHash(const Key& key) const {
        return std::hash<Key>()(key) % table_.size();
    }

    void reHash() {
        vector<Node> oldTable_ = std::move(table_);
        size_t newSize = oldTable_.size() * 2;
        table_ = vector<Node>(newSize);
        size_ = 0;
        for (const auto& node : oldTable_) {
            if (node.status == Status::OCCUPIED) {
                insert(node.key_, node.value_);
            }
        }
    }
public:
    OpenAddressingHashTable(size_t size = 16) : table_(size){
        cout << "OpenAddressingHashTable initialized " << endl;
    }

    void insert(const Key& key, const Value& value) {
        if (size_+1 > table_.size() * maxLoadFactor_) {
            reHash();
        }
        size_t index = getHash(key);
        size_t originalIndex = index;
        while (table_[index].status == Status::OCCUPIED) {
            if (table_[index].key_ == key) {
                table_[index].value_ = value;
                return;
            }
            index = (index + 1) % table_.size();
        }

        table_[index].key_ = key;
        table_[index].value_ = value;
        table_[index].status = Status::OCCUPIED;
        ++size_;
    }

    bool contains(const Key& key) const {
        size_t index = getHash(key);
        size_t originalIndex = index;

        while (table_[index].status != Status::EMPTY) {
            if (table_[index].status == Status::OCCUPIED && table_[index].key_ == key) {
                return true;
            }
            index = (index + 1) % table_.size();
            if (index == originalIndex) {
                break;
            }
        }
        return false;
    }
    bool remove(const Key& key) {
        size_t index = getHash(key);
        size_t originalIndex = index;

        while (table_[index].status != Status::EMPTY) {
            if (table_[index].status == Status::OCCUPIED && table_[index].key_ == key) {
                table_[index].status = Status::DELETED;
                --size_;
                return true;
            }
            index = (index + 1) % table_.size();
            if (index == originalIndex) {
                break;
            }
        }

        return false;
    }
    Value* find(const Key& key) {
        size_t index = getHash(key);
        size_t originalIndex = index;

        while (table_[index].status != Status::EMPTY) {
            if (table_[index].status == Status::OCCUPIED && table_[index].key_ == key) {
                return &table_[index].value_;
            }
            index = (index + 1) % table_.size();
            if (index == originalIndex) {
                break;
            }
        }
        return nullptr;
    }
};

int main() {
    // using HashTable = ChainingHashTable<int, string>;
    // using HashTable = FixedSizedChainingHashTable<int, string>;
    using HashTable = OpenAddressingHashTable<int, string>;

    HashTable hash_table;

    hash_table.insert(1, "one");
    hash_table.insert(2, "two");
    hash_table.insert(3, "three");

    if (hash_table.contains(2)) {
        cout << "Key 2 exists in the hash table." << endl;
    } else {
        cout << "Key 2 does not exist in the hash table." << endl;
    }

    hash_table.remove(2);
    
    if (!hash_table.contains(2)) {
        cout << "Key 2 has been removed from the hash table." << endl;
    }

    string* value3 = hash_table.find(3);
    if (value3) {
        cout << "Found key 3 with value: " << *value3 << endl;
    } else {
        cout << "Key 3 not found in the hash table." << endl;
    }
    string* value4 = hash_table.find(4);
    if (value4) {
        cout << "Found key 4 with value: " << *value4 << endl;
    } else {
        cout << "Key 4 not found in the hash table." << endl;
    }

    return 0;
}
