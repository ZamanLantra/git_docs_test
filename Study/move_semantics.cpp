// zamanlantra@ZamansMcBookPro Study % g++ -std=c++17 -o move_semantics move_semantics.cpp 
// zamanlantra@ZamansMcBookPro Study % ./move_semantics 

#include <iostream>
#include <cstring>

using namespace std;

void* operator new(size_t bytes) {
    cout << "Allocating " << bytes << " bytes\n";
    void* ptr = std::malloc(bytes);
    if (!ptr) throw std::bad_alloc();
    return ptr;
}
void operator delete(void* ptr) noexcept {
    cout << "Deleting\n";
    std::free(ptr);
}

class String {
public:
    String() = delete;
    String(const char* d) {                 // Constructor
        m_size = strlen(d);
        m_data = new char[m_size+1];
        memcpy(m_data, d, m_size);
        m_data[m_size] = 0;
        cout << "Constructured\n";
    };
    String(const String& other) {           // Copy Constructor
        m_size = other.m_size;
        m_data = new char[m_size+1];
        memcpy(m_data, other.m_data, m_size);
        m_data[m_size] = 0;
        cout << "Copy Constructor\n";
    }
    String(String&& other) noexcept {        // Move Constructor - explicit
        m_size = other.m_size;
        m_data = other.m_data;
        other.m_size = 0;
        other.m_data = nullptr;
        cout << "Move Constructor - explicit\n";
    }
    String(char*&& d) noexcept {             // Move Constructor - implicit
        m_size = strlen(d);
        m_data = d;
        d = nullptr;
        cout << "Move Constructor - implicit\n";
    }
    String& operator=(const String& other) { // Copy Assignment operator
        m_size = other.m_size;
        m_data = new char[m_size+1];
        memcpy(m_data, other.m_data, m_size);
        m_data[m_size] = 0;
        cout << "Copy Assignment\n";
        return *this;
    }
    String& operator=(String&& other) {     // Move Assignment operator
        if (this != &other) {
            m_size = other.m_size;
            m_data = other.m_data;
            other.m_size = 0;
            other.m_data = nullptr;
        }
        cout << "Move Assignment\n";
        return *this;
    }
    String operator+(const String& other) const {   // Concatenate operator +
        const int size = m_size + other.m_size;
        char* c = new char[size + 1];
        memcpy(c, m_data, m_size);
        memcpy(c+m_size, other.m_data, other.m_size);
        c[size] = 0;
        cout << "Concatenate +\n";
        return String(std::move(c));
    }
    String& operator+=(const String& other) {       // Concatenate operator +=
        const int size = m_size + other.m_size;
        char* c = new char[size + 1];
        memcpy(c, m_data, m_size);
        memcpy(c+m_size, other.m_data, other.m_size);
        c[size] = 0;
        m_size = size;
        delete[] m_data;
        m_data = c;
        cout << "Concatenate +=\n";
        return *this;
    }
    ~String() {                             // Destructor
        cout << "Destroyed -- " << m_size << "bytes\n";
        delete[] m_data;
    }
    void print(const char* prefix) const {
        const char* data = (m_data != nullptr) ? m_data : "nullptr";
        cout << prefix << " " << data << "\n";
    }
private:
    char* m_data = nullptr;
    int m_size = 0;
};

int main() {
    std::cout << "Try programiz.pro\n\n";
    
    String s1("Cherno");                    // Uses the constructor
    s1.print("s1->");
    
    String s6("Hello");                     // Uses the constructor
    String s7("World");                     // Uses the constructor
    s6.print("s6->");
    s7.print("s7->");
    const String s6_const("Hello");         // Uses the constructor
    const String s7_const("World");         // Uses the constructor     
    s6_const.print("s6_const->");
    s7_const.print("s7_const->");
    
    cout << "\nStart Copy consructor s2 -------\n";
    String s2 = s1;                         // Uses the Copy consructor
    s2.print("s2->");
    
    cout << "\nStart Copy Assignemnt s3 -------\n";
    String s3("Hello");                     // Uses the constructor
    s3 = s1;                                // Uses the Copy assignment
    s3.print("s3->");
    
    cout << "\nStart Move consructor s4 -------\n";
    String s4 = std::move(s1);              // Uses the Move Constructor - explicit
    s4.print("s4->");
    s1.print("s1->");
    
    cout << "\nStart Move Assignemnt s5 -------\n";
    String s5("Hello");                     // Uses the constructor
    s5 = std::move(s4);                     // Uses the Move Assignment
    s4.print("s4->");
    s5.print("s5->");
    
    cout << "\nConcatenate s8 -------\n";
    String s8 = s6 + s7;                    // Uses the Concat operator and Move Constructor - implicit
    s8.print("s8->");
    
    cout << "\nConcatenate s9 -------\n";
    String s9(s6 + s7);                     // Uses the Concat operator and Move Constructor - implicit
    s9.print("s9->");
    
    cout << "\nConcatenate s10 -------\n";
    String s10 = s6_const + s7_const;       // Uses the Concat operator and Move Constructor - implicit
    s10.print("s10->");
    
    cout << "\nConcatenate s11 -------\n";
    String s11(s6_const + s7_const);        // Uses the Concat operator and Move Constructor - implicit
    s11.print("s11->");
    
    cout << "\nConcatenate s12 -------\n";
    String s12(String("Hi") + String("There")); // Uses the Concat operator and Move Constructor - implicit
    s12.print("s12->");                         // But in this example, we get Hi and There strings constructed first
                                                // As we have Concat, anyway we need to create a bigger string to have both, 
                                                // but if not we could steal the resources of this (i.e. String("Hi")), and give to s12
    
    cout << "\nConcatenate s13 -------\n";
    s6 += s7;                               // Uses the Concat += operator
    s6.print("s6->");
    
    cout << "\n";
                                            // Uses a set of destructors for all String objects -- s1 and s4 would be empty
    return 0;
}