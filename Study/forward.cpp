// zamanlantra@ZamansMcBookPro Study % g++ -std=c++17 -o forward forward.cpp 
// zamanlantra@ZamansMcBookPro Study % ./forward 

#include <iostream>
#include <vector>
#include <memory>

using namespace std;

enum OppType {
    INT32 = 0,
    INT64, 
    DOUBLE,
    FLOAT,
    CONTAINER,
};

class Container;

template<typename T>
constexpr OppType OppTypeFrom() {
    if constexpr (std::is_same_v<T, int>) return OppType::INT32;
    else if constexpr (std::is_same_v<T, long long>) return OppType::INT64;
    else if constexpr (std::is_same_v<T, double>) return OppType::DOUBLE;
    else if constexpr (std::is_same_v<T, float>) return OppType::FLOAT;
    else if constexpr (std::is_same_v<T, Container>) return OppType::CONTAINER;
    else static_assert(!sizeof(T), "Unsupported type for OppDat");
}

class IDat {
public:
    IDat(OppType type) : m_type(type) { }
    virtual ~IDat() {};
    const OppType m_type = OppType::INT32;
};

template <typename T>
class OppDat : public IDat {
public:
    OppDat(size_t size) : IDat(OppTypeFrom<T>()), m_size(size) {
        data = new T[size];
    };
    ~OppDat() { delete[] data; }
private:
    T* data = nullptr;
    size_t m_size = 0;
};

class Container {
public:
    vector<shared_ptr<IDat>> dats;

    void printTypes() const {
        for (auto& a : dats) {
            if (dynamic_cast<OppDat<int>*>(a.get())) { cout << a->m_type << " Type: int\n"; }
            else if (dynamic_cast<OppDat<double>*>(a.get())) { cout << a->m_type << " Type: double\n"; }
            else { cout << a->m_type << " Some other type\n"; }
            
            if (a->m_type == INT32) { cout << "XType: int\n"; }
            else if (a->m_type == DOUBLE) { cout << "XType: double\n"; }
            else if (a->m_type == CONTAINER) { cout << "XType: container\n";  }
            else { cout << "XSome other type\n"; }
        }
    }
    
    template<typename T, typename...Args>
    void add(Args&&... args) {
        dats.emplace_back(make_shared<OppDat<T>>(std::forward<Args>(args)...));
    }
};

int main() {
    Container c;
    c.dats.emplace_back(make_shared<OppDat<int>>(5));
    c.dats.emplace_back(make_shared<OppDat<double>>(20));
    c.dats.emplace_back(make_shared<OppDat<Container>>(200));
    
    c.add<int64_t>(1000);
    
    c.printTypes();
    int64_t a = 0;

    return 0;
}
