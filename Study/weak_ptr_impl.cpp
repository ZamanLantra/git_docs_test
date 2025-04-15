// zamanlantra@ZamansMcBookPro Study % g++ -std=c++17 -o weak_ptr_impl weak_ptr_impl.cpp
// zamanlantra@ZamansMcBookPro Study % ./weak_ptr_impl

#include <iostream>

// ===== ControlBlock =====
template<typename T>
struct ControlBlock {
    T* ptr;
    int shared_count;
    int weak_count;

    ControlBlock(T* p) : ptr(p), shared_count(1), weak_count(1) {}
};

// ===== Forward Declarations =====
template<typename T>
class SharedPtr;

template<typename T>
class WeakPtr;

// ===== SharedPtr Implementation =====
template<typename T>
class SharedPtr {
public:
    SharedPtr() : control(nullptr) {}
    explicit SharedPtr(T* ptr) : control(new ControlBlock<T>(ptr)) {}

    SharedPtr(const SharedPtr& other) {
        control = other.control;
        if (control) control->shared_count++;
    }

    SharedPtr(SharedPtr&& other) noexcept {
        control = other.control;
        other.control = nullptr;
    }

    SharedPtr& operator=(const SharedPtr& other) {
        if (this != &other) {
            release();
            control = other.control;
            if (control) control->shared_count++;
        }
        return *this;
    }

    SharedPtr& operator=(SharedPtr&& other) noexcept {
        if (this != &other) {
            release();
            control = other.control;
            other.control = nullptr;
        }
        return *this;
    }

    ~SharedPtr() {
        release();
    }

    T* get() const {
        return control ? control->ptr : nullptr;
    }

    T& operator*() const { return *control->ptr; }
    T* operator->() const { return control->ptr; }

    bool unique() const { return control && control->shared_count == 1; }

private:
    ControlBlock<T>* control;

    void release() {
        if (control) {
            control->shared_count--;
            if (control->shared_count == 0) {
                delete control->ptr;
                control->ptr = nullptr;
                if (--control->weak_count == 0) {
                    delete control;
                }
            }
            control = nullptr;
        }
    }

    SharedPtr(const WeakPtr<T>& weak) {  // used by WeakPtr::lock
        control = weak.control;
        if (control && control->shared_count > 0) {
            control->shared_count++;
        } else {
            control = nullptr;
        }
    }

    friend class WeakPtr<T>;
};

// ===== WeakPtr Implementation =====
template<typename T>
class WeakPtr {
public:
    WeakPtr() : control(nullptr) {}

    WeakPtr(const SharedPtr<T>& shared) {
        control = shared.control;
        if (control) control->weak_count++;
    }

    WeakPtr(const WeakPtr& other) {
        control = other.control;
        if (control) control->weak_count++;
    }

    WeakPtr& operator=(const WeakPtr& other) {
        if (this != &other) {
            release();
            control = other.control;
            if (control) control->weak_count++;
        }
        return *this;
    }

    ~WeakPtr() {
        release();
    }

    bool expired() const {
        return !control || control->shared_count == 0;
    }

    SharedPtr<T> lock() const {
        if (expired()) return SharedPtr<T>();
        return SharedPtr<T>(*this);  // calls private constructor
    }

private:
    ControlBlock<T>* control;

    void release() {
        if (control) {
            control->weak_count--;
            if (control->shared_count == 0 && control->weak_count == 0) {
                delete control;
            }
            control = nullptr;
        }
    }

    friend class SharedPtr<T>;
};

// ===== Sample Class =====
class Person {
public:
    Person(const std::string& n) : name(n) {
        std::cout << "Person " << name << " created.\n";
    }
    ~Person() {
        std::cout << "Person " << name << " destroyed.\n";
    }
    void greet() const {
        std::cout << "Hi, I'm " << name << ".\n";
    }

private:
    std::string name;
};

// ===== Main =====
int main() {
    WeakPtr<Person> weak;

    {
        SharedPtr<Person> p1(new Person("Alice"));
        weak = WeakPtr<Person>(p1);

        {
            SharedPtr<Person> p2 = weak.lock();
            if (p2.get()) {
                p2->greet();
            }
        }

        std::cout << "Exiting inner scope...\n";
    }

    std::cout << "Back in main. Is object expired? " << (weak.expired() ? "Yes" : "No") << "\n";
    return 0;
}
