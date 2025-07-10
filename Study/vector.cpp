#include <iostream>
#include <utility>

namespace zam {

template <typename T>
class vector {
public:
    vector() {
        std::cout << "vector default constructor called\n";
    }
    vector(size_t size) 
            : capacity_(size)
            , size_(0) {
        std::cout << "vector 1 constructor called\n";
        data_ = static_cast<T*>(operator new[](capacity_ * sizeof(T)));
    }
    vector(size_t size, const T& val) 
            : capacity_(size)
            , size_(size) {
        std::cout << "vector 2 constructor called\n";
        data_ = static_cast<T*>(operator new[](capacity_ * sizeof(T)));
        std::fill(data_, data_+size_, val);
    }
    vector(const vector& other) noexcept {
        std::cout << "vector copy constructor called\n";
        capacity_ = 0;
        size_ = 0;
        data_ = nullptr;
        reserve(other.capacity());
        for (size_t i = 0; i < other.size(); ++i) {
            emplace_back(other[i]);
        }
    }
    vector(vector&& other) noexcept {
        std::cout << "vector move constructor called\n";
        reset(other.data_, other.size_, other.capacity_);
        other.reset(nullptr, 0, 0);
    }
    vector& operator=(const vector& other) noexcept {
        std::cout << "vector copy assignment called\n";
        if (this != &other) {
            for (size_t i = 0; i < size_; ++i) {
                data_[i].~T();
            }
            size_ = 0;
            reserve(other.capacity());
            for (size_t i = 0; i < other.size(); ++i) {
                emplace_back(other[i]);
            }
        }
        return *this;
    }
    vector& operator=(vector&& other) noexcept {
        std::cout << "vector move assignment called\n";
        if (this != &other) {
            if (data_) operator delete[](data_);
            reset(other.data_, other.size_, other.capacity_);
            other.reset(nullptr, 0, 0);
        }
        return *this;
    }
    ~vector() {
        if (data_) {
            for (size_t i = 0; i < size_; ++i) {
                data_[i].~T();
            }
            operator delete[](data_);
        }
        capacity_ = 0;
        size_ = 0;
    }
    void push_back(const T& val) {
        std::cout << "vector push_back called with value " << val << "\n";
        if (size_ == capacity_) {
            reallocate();
        }
        new (data_ + size_++) T(val);
    }
    template <typename... UArgs>
    void emplace_back(UArgs&&... args) {
        std::cout << "vector emplace_back called\n";
        if (size_ == capacity_) {
            reallocate();
        }
        new (data_+size_) T(std::forward<UArgs>(args)...);
        ++size_;
    }
    T pop_back() {
        if (size_ == 0) throw std::runtime_error("Cannot pop_back as size is 0");
        --size_;
        T val = std::move(data_[size_]);
        data_[size_].~T();
        return val;
    }
    T& operator[](size_t index) {
        std::cout << "At operator[] size_:" << size_ << " capacity_:" << capacity_ << std::endl; 
        if (index >= size_) 
            throw std::runtime_error("Index " + std::to_string(index) + " out of bounds");
        return data_[index];
    }
    const T& operator[](size_t index) const {
        std::cout << "At operator[] size_:" << size_ << " capacity_:" << capacity_ << std::endl; 
        if (index >= size_) 
            throw std::runtime_error("Index " + std::to_string(index) + " out of bounds");
        return data_[index];
    }
    T* data() {
        return data_;
    }
    const T* data() const {
        return data_;
    }
    size_t size() const {
        return size_;
    }
    size_t capacity() const {
        return capacity_;
    }
    void reserve(size_t capacity) {
        if (capacity > capacity_) {
            reallocate(capacity);
        }
    }
private:
    size_t size_ = 0;
    size_t capacity_ = 0;
    T* data_ = nullptr;
    
    void reallocate(size_t new_capacity = 0) {
        std::cout << "reallocate called start size_ " << size_ << "\n";
        T* old_data = data_;
        size_t old_size = size_;

        if (new_capacity > 0) {
            capacity_ = new_capacity;
        } else {
            capacity_ = (capacity_ == 0) ? 1 : capacity_ * 2;
        }
        data_ = static_cast<T*>(operator new[](capacity_ * sizeof(T)));
        for (size_t i = 0; i < old_size; ++i) {
            std::cout << "try-move\n";
            new (data_ + i) T(std::move(old_data[i]));
        }
        if (old_data) {
            for (size_t i = 0; i < old_size; ++i) {
                old_data[i].~T();
            }
            operator delete[](old_data);
        }

        size_ = old_size;
        std::cout << "reallocate called done\n";
    }
    
    void reset(T* data, size_t size, size_t capacity) {
        data_ = data;
        size_ = size;
        capacity_ = capacity;
    }
};
}

// --------------------------tests---------------------------------------------------------
#include <iostream>
#include <string>
#include <cassert>
#include <stdexcept>

struct TestType {
    int x;
    static int constructions;
    static int destructions;

    TestType(int val = 0) : x(val) { std::cout << "ctor " << x << std::endl; ++constructions; }
    TestType(const TestType& other) : x(other.x) { std::cout << "cpy-ctor " << x << std::endl; ++constructions; }
    TestType(TestType&& other) noexcept : x(other.x) { std::cout << "mv-ctor " << x << std::endl; ++constructions; other.x = 0; }
    TestType& operator=(const TestType& other) { x = other.x; return *this; }
    TestType& operator=(TestType&& other) noexcept { x = other.x; other.x = 0; return *this; }
    ~TestType() { ++destructions; }

    bool operator==(const TestType& other) const { return x == other.x; }
};
int TestType::constructions = 0;
int TestType::destructions = 0;

std::ostream& operator<<(std::ostream& stream, const TestType& obj) {
    stream << "TestType [" << obj.x << "]\n";
    return stream;
}

void test_default_constructor() {
    zam::vector<int> v;
    assert(v.size() == 0);
    assert(v.capacity() == 0);
}

void test_size_constructor() {
    zam::vector<int> v(5);
    assert(v.size() == 0);
    assert(v.capacity() >= 5);
}

void test_size_value_constructor() {
    zam::vector<int> v(5, 42);
    assert(v.size() == 5);
    for (size_t i = 0; i < v.size(); ++i)
        assert(v[i] == 42);
}

void test_push_back_and_indexing() {
    zam::vector<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    assert(v.size() == 3);
    assert(v[0] == 1);
    assert(v[1] == 2);
    assert(v[2] == 3);

    bool caught = false;
    try { v[3]; }
    catch (const std::runtime_error&) { caught = true; }
    assert(caught);
}

void test_emplace_back() {
    std::cout << "test_emplace_back ---------------\n";
    zam::vector<std::string> v;
    std::cout << "test_emplace_back ---------------1\n";
    v.emplace_back("hello");
    std::cout << "test_emplace_back ---------------2\n";
    v.emplace_back(5, 'x');  // string(size_t, char) ctor
    std::cout << "test_emplace_back ---------------3\n";
    assert(v.size() == 2);
    assert(v[0] == "hello");
    assert(v[1] == "xxxxx");
}

void test_pop_back() {
    zam::vector<int> v;
    v.push_back(10);
    v.push_back(20);
    int val = v.pop_back();
    assert(val == 20);
    assert(v.size() == 1);

    v.pop_back();
    bool caught = false;
    try { v.pop_back(); }
    catch (const std::runtime_error&) { caught = true; }
    assert(caught);
}

void test_copy_constructor_and_assignment() {
    zam::vector<int> v1;
    v1.push_back(1);
    v1.push_back(2);
    zam::vector<int> v2 = v1; // copy ctor
    assert(v2.size() == 2);
    assert(v2[0] == 1 && v2[1] == 2);

    zam::vector<int> v3;
    v3 = v2; // copy assignment
    assert(v3.size() == 2);
    assert(v3[0] == 1 && v3[1] == 2);
}

void test_move_constructor_and_assignment() {
    zam::vector<int> v1;
    v1.push_back(1);
    v1.push_back(2);
    zam::vector<int> v2 = std::move(v1);
    assert(v2.size() == 2);
    assert(v2[0] == 1 && v2[1] == 2);
    assert(v1.size() == 0); // moved-from vector should be empty

    zam::vector<int> v3;
    v3 = std::move(v2);
    assert(v3.size() == 2);
    assert(v3[0] == 1 && v3[1] == 2);
    assert(v2.size() == 0);
}

void test_reserve_and_capacity() {
    zam::vector<int> v;
    v.reserve(10);
    assert(v.capacity() >= 10);
    assert(v.size() == 0);
    for (int i = 0; i < 10; ++i) v.push_back(i);
    assert(v.size() == 10);
    assert(v.capacity() >= 10);
}

void test_destruction_count() {
    std::cout << "test_destruction_count ---------------\n";
    TestType::constructions = 0;
    TestType::destructions = 0;
    {
        zam::vector<TestType> v;
        v.reserve(2);
        std::cout << "test_destruction_count ---------------1\n";
        v.emplace_back(1);
        std::cout << "test_destruction_count ---------------2\n";
        v.emplace_back(2);
        std::cout << "test_destruction_count ---------------3\n";
        assert(TestType::constructions == 2);
    }
    assert(TestType::destructions == 2);
}

void test_emplace_and_push_with_nontrivial() {
    zam::vector<TestType> v;
    v.push_back(TestType(5));
    v.emplace_back(10);
    assert(v.size() == 2);
    assert(v[0].x == 5);
    assert(v[1].x == 10);
}

void test_reallocate_grows_capacity() {
    zam::vector<int> v(2, 0);
    size_t old_capacity = v.capacity();
    v.push_back(1);
    if (v.capacity() == old_capacity) {
        v.push_back(2);
    }
    assert(v.capacity() > old_capacity);
}

int main() {
    test_default_constructor();
    test_size_constructor();
    test_size_value_constructor();
    test_push_back_and_indexing();
    test_emplace_back();
    test_pop_back();
    test_copy_constructor_and_assignment();
    test_move_constructor_and_assignment();
    test_reserve_and_capacity();
    test_destruction_count();
    test_emplace_and_push_with_nontrivial();
    test_reallocate_grows_capacity();

    std::cout << "All tests passed!\n";
    return 0;
}
