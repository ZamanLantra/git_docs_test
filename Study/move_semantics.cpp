// zamanlantra@ZamansMcBookPro Study % g++ -std=c++17 -o move_semantics move_semantics.cpp 
// zamanlantra@ZamansMcBookPro Study % ./move_semantics 

#include <iostream>
#include <cstring>

using namespace std;

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
    String(String&& other) {                // Move Constructor
        m_size = other.m_size;
        m_data = other.m_data;
        other.m_size = 0;
        other.m_data = nullptr;
        cout << "Move Constructor\n";
    }
    String& operator=(const String& other) { // Copy Assignment
        m_size = other.m_size;
        m_data = new char[m_size+1];
        memcpy(m_data, other.m_data, m_size);
        m_data[m_size] = 0;
        cout << "Copy Assignment\n";
        return *this;
    }
    String& operator=(String&& other) {     // Move Assignment
        if (this != &other) {
            m_size = other.m_size;
            m_data = other.m_data;
            other.m_size = 0;
            other.m_data = nullptr;
        }
        cout << "Move Assignment\n";
        return *this;
    }
    ~String() {                             // Destructor
        cout << "Destroyed\n";
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
    
    String s1("Cherno");
    s1.print("s1->");
    
    cout << "\nStart Copy consructor s2\n";
    String s2 = s1; // Copy consructor
    s2.print("s2->");
    
    cout << "\nStart Copy Assignemnt s3\n";
    String s3("Hello"); 
    s3 = s1;    // Copy assignment
    s3.print("s3->");
    
    cout << "\nStart Move consructor s4\n";
    String s4 = std::move(s1);
    s4.print("s4->");
    s1.print("s1->");
    
    cout << "\nStart Move Assignemnt s5\n";
    String s5("Hello");
    s5 = std::move(s4);
    s4.print("s4->");
    s5.print("s5->");
    
    cout << "\n";
    
    return 0;
}