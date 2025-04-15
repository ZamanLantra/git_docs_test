// zamanlantra@ZamansMcBookPro Study % g++ -std=c++17 -o inheritance inheritance.cpp 
// zamanlantra@ZamansMcBookPro Study % ./inheritance 

// Implement a type-erased heterogeneous container that can hold objects of any type implementing a specific interface, without using inheritance in the stored types.

// The interface must include at least:
//      std::string name() const
//      void act() const

// Constraints:
//      You cannot require Fireball, Robot, or Goblin to inherit from a common base class.
//      You must use templates, type erasure, and polymorphism to achieve this.
//      Bonus: Make the container iterable with begin()/end().

// What you need to use:
//      Type erasure via std::unique_ptr to abstract base.
//      Templates to wrap any compatible object.
//      Polymorphism to allow calling the interface methods uniformly.
//      Possibly std::function, lambdas, or even CRTP if you go wild.

#include <iostream>
#include <vector>
#include <memory>

using namespace std;

class Base {
protected:
    string m_name = "";
    Base(string name) : m_name(name) { 
        cout << "\tBase Constructor " << m_name << "\n";
    }
public:
    Base() = delete;
    virtual ~Base() { 
        cout << "\tBase Destructor " << m_name << "\n";
    }
    virtual std::string name() const {
        return m_name;
    }
    virtual void act() const {
        cout << "\tBase Act of " << m_name << "\n";
    }
};

class Fireball : public Base {
private:
    int m_value = 0;
public:
    Fireball(string name) : Base(name) { cout << "Fireball Constructor " << m_name << "\n"; }
    ~Fireball() override { cout << "Fireball Destructor " << m_name << "\n"; }
    std::string name() const override { return string("Fireball#") + m_name; }
    void act() const override { cout << "Fireball Act of " << m_name << "\n"; }
};

class Robot : public Base {
private:
    int m_value = 0;
public:
    Robot(string name) : Base(name) { cout << "Robot Constructor " << m_name << "\n"; }
    ~Robot() override { cout << "Robot Destructor " << m_name << "\n"; }
    std::string name() const override { return string("Robot#") + m_name; }
    void act() const override { cout << "Robot Act of " << m_name << "\n"; }
};

class Goblin : public Base {
private:
    int m_value = 0;
public:
    Goblin(string name) : Base(name) { cout << "Goblin Constructor " << m_name << "\n"; }
    ~Goblin() override { cout << "Goblin Destructor " << m_name << "\n"; }
    std::string name() const override { return string("Goblin#") + m_name; }
    void act() const override { cout << "Goblin Act of " << m_name << "\n"; }
};

class ActorContainer {
private:
    vector<unique_ptr<Base>> data;
public:
    ActorContainer() {}
    void add(unique_ptr<Base> ptr) {
        data.push_back(std::move(ptr));
    }

    class iterator {
public:
        iterator(vector<unique_ptr<Base>>* data, int idx, bool forward) : m_data(data), m_index(idx), m_forward(forward) { }
        Base* operator->() { return (*m_data)[m_index].get(); }
        Base& operator*() { return *((*m_data)[m_index].get()); }
        iterator& operator++() { m_forward ? ++m_index : --m_index; return *this; } // this will support only prefix (++it)
        iterator operator++(int) { iterator temp = *this; ++(*this); return temp; }// this will support postfix (it++)
        bool operator!=(const iterator& other) { return m_index != other.m_index; }
private:
        vector<unique_ptr<Base>>* m_data = nullptr;
        int m_index = 0;
        bool m_forward = true;
    };
    
    iterator begin() { return iterator(&data, 0, true); }
    iterator end() { return iterator(&data, data.size(), true); }
    iterator rbegin() { return iterator(&data, data.size()-1, false); }
    iterator rend() { return iterator(&data, -1, false); }
};

int main() {
    ActorContainer c;
    c.add(make_unique<Fireball>("🔥"));
    c.add(make_unique<Fireball>("🔥🔥"));
    c.add(make_unique<Robot>("🤖"));
    c.add(make_unique<Goblin>("👺"));
    
    cout << "\nPrinting forward START\n";
    for (const auto& actor : c) {
        std::cout << actor.name() << " performs action\n";
        actor.act();
    }
    cout << "Printing forward END\n";
    
    cout << "\nPrinting forward iterator START\n";
    for (auto it = c.begin(); it != c.end(); ++it) {
        std::cout << it->name() << " performs action\n";
        it->act();
    }
    cout << "Printing forward iterator END\n\n";
    
    cout << "\nPrinting reverse START\n";
    for (auto it = c.rbegin(); it != c.rend(); it++) {
        std::cout << it->name() << " performs action\n";
        it->act();
    }
    cout << "Printing reverse END\n\n";
    
    return 0;
}
