// zamanlantra@ZamansMcBookPro Study % g++ -std=c++17 -o smart_ptr smart_ptr.cpp
// zamanlantra@ZamansMcBookPro Study % ./smart_ptr

#include <iostream>

template <typename T>
class my_unique {
public:
    explicit my_unique(T* ptr) : m_ptr(ptr) {
        std::cout << "Created my_unique\n";
    }
    ~my_unique() {
        delete m_ptr;
        std::cout << "destroying my_unique\n";
    }
    my_unique(const my_unique&) = delete;         // copy constructor deleted
    my_unique& operator=(const my_unique&) = delete;    // copy assignment deleted
    
    my_unique(my_unique&& other) : m_ptr(other.m_ptr) { // move constructor
        other.m_ptr = nullptr;
        std::cout << "Move constructor my_unique\n";
    }
    
    my_unique& operator=(my_unique&& other) { // move assignment
        if (this != &other) {
            delete m_ptr;
            m_ptr = other.m_ptr;
            other.m_ptr = nullptr;
        }
        std::cout << "Move assigned my_unique\n";
        return *this;
    }
    
    T* get() {
        return m_ptr;
    }
    T* operator->() {
        return m_ptr;
    }
    T& operator*() const { 
        return *m_ptr;
    }
private:
    T* m_ptr = nullptr;
};

template <typename T>
class my_shared {
public:
    explicit my_shared(T* ptr) : m_ptr(ptr), m_ref_count(new int(1)) {
        std::cout << "Created my_shared, count = " << *m_ref_count << "\n";
    }
    ~my_shared() {
        cleanup();
    }
    my_shared(const my_shared& other) : m_ptr(other.m_ptr), m_ref_count(other.m_ref_count) { // copy constructor
        ++(*m_ref_count);
        std::cout << "Copied my_shared, count = " << *m_ref_count << "\n";
    }

    my_shared& operator=(const my_shared& other) { // copy assignment
        if (this != &other) {
            cleanup(); // why? we need to reduce current pointed values as we are going to copy a new
            m_ptr = other.m_ptr;
            m_ref_count = other.m_ref_count;
            ++(*m_ref_count);
            std::cout << "Copy assigned my_shared, count = " << *m_ref_count << "\n";
        }
        return *this;
    }
    
    my_shared(my_shared&& other) : m_ptr(other.m_ptr) { // move constructor
        other.m_ptr = nullptr;
        std::cout << "Move constructor my_shared\n";
    }
    
    my_shared& operator=(my_shared&& other) { // move assignment
        if (this != &other) {
            delete m_ptr;
            m_ptr = other.m_ptr;
            other.m_ptr = nullptr;
        }
        std::cout << "Move assigned my_shared\n";
        return *this;
    }
    
    T* get() {
        return m_ptr;
    }
    T* operator->() {
        return m_ptr;
    }
    T& operator*() const { 
        return *m_ptr;
    }
private:
    T* m_ptr = nullptr;
    int* m_ref_count = nullptr;
    
    void cleanup() {
        if (m_ref_count && --(*m_ref_count) == 0) {
            delete m_ptr;
            delete m_ref_count;
            std::cout << "Deleting managed object and ref_count\n";
        }
        else if (m_ref_count) {
            std::cout << "Decremented ref_count to " << *m_ref_count << "\n";
        }
    }
};

struct Data {
    int a;
    double b;
};

int main() {
    std::cout << "Try programiz.pro START\n";
    
    std::cout << "Shared Pointer Start\n";
    {
        my_shared<Data> data2(new Data());
        
        {
            my_shared<Data> data(new Data());
            data->a = 10;
            data->b = 20.7;
            
            std::cout << "data->a: " << data->a << " data->b: " << data->b << "\n";
            
            my_shared<Data> datan = data; // Copy constructor
            std::cout << "datan->a: " << datan->a << " datan->b: " << datan->b << "\n";
            
            my_shared<Data> datam(new Data());
            datam = datan;  // Copy assignment
            std::cout << "datam->a: " << datam->a << " datam->b: " << datam->b << "\n";
            
            my_shared<Data> data3 = std::move(data); // Move constructor
            std::cout << "data3->a: " << data3->a << " data3->b: " << data3->b << "\n";
            
            data2 = std::move(data3); // Move assignment
            
            std::cout << "data2->a: " << data2->a << " data2->b: " << data2->b << "\n";
        }
        
        std::cout << "data2->a: " << data2->a << " data2->b: " << data2->b << "\n";
    }
    std::cout << "Shared Pointer End\n\n";
    
    std::cout << "Unique Pointer Start\n";
    {
        
        my_unique<Data> data2(new Data());
    
        {
            my_unique<Data> data(new Data());
            data->a = 10;
            data->b = 20.7;
            
            std::cout << "data->a: " << data->a << " data->b: " << data->b << "\n";
            
            my_unique<Data> data3 = std::move(data); // Move constructor
            std::cout << "data3->a: " << data3->a << " data3->b: " << data3->b << "\n";
            
            data2 = std::move(data3); // Move assignment
            
            std::cout << "data2->a: " << data2->a << " data2->b: " << data2->b << "\n";
        }
        
        std::cout << "data2->a: " << data2->a << " data2->b: " << data2->b << "\n";
    }
    std::cout << "Unique Pointer End\n\n";
    
    std::cout << "Try programiz.pro END\n";
    return 0;
}
