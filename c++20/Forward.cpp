#include <iostream>
#include <vector>
#include <string>

struct Hello {
    int a; int b; double c;
};

std::ostream& operator<<(std::ostream& os, const Hello& h) {
    return os << h.a << " " << h.b << " " << h.c;
}

class HelloContainer {
public:
    template <typename...Args>
    void emplace(Args&& ...args) {
        lst.emplace_back(std::forward<Args>(args)...);
    }
    void print() {
        for (auto& x : lst) 
            std::cout << x << std::endl;
    }
    std::vector<Hello> lst; 
};

int main() {
    HelloContainer container;
    container.emplace(4,5,6.5);
    container.emplace(14,15,16.5);
    
    container.print();
}
