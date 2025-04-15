// zamanlantra@ZamansMcBookPro Study % g++ -std=c++17 -o inheritance_template inheritance_template.cpp
// zamanlantra@ZamansMcBookPro Study % ./inheritance_template

// without inheritance using type erasure and templates

#include <iostream>
#include <vector>
#include <memory>
#include <string>

using namespace std;

class Fireball { // Concrete actor classes with no inheritance
private:
    string m_name;
public:
    Fireball(string name) : m_name(std::move(name)) {
        cout << "Fireball Constructor " << m_name << "\n";
    }
    ~Fireball() {
        cout << "Fireball Destructor " << m_name << "\n";
    }
    string name() const { return string("Fireball#") + m_name; }
    void act() const { cout << "Fireball Act of " << m_name << "\n"; }
};

class Robot { // Concrete actor classes with no inheritance
private:
    string m_name;
public:
    Robot(string name) : m_name(std::move(name)) {
        cout << "Robot Constructor " << m_name << "\n";
    }
    ~Robot() {
        cout << "Robot Destructor " << m_name << "\n";
    }
    string name() const { return string("Robot#") + m_name; }
    void act() const { cout << "Robot Act of " << m_name << "\n"; }
};

class Goblin { // Concrete actor classes with no inheritance
private:
    string m_name;
public:
    Goblin(string name) : m_name(std::move(name)) {
        cout << "Goblin Constructor " << m_name << "\n";
    }
    ~Goblin() {
        cout << "Goblin Destructor " << m_name << "\n";
    }
    string name() const { return string("Goblin#") + m_name; }
    void act() const { cout << "Goblin Act of " << m_name << "\n"; }
};

class IActor { // Type-erased interface
public:
    virtual ~IActor() {}
    virtual string name() const = 0;
    virtual void act() const = 0;
    virtual unique_ptr<IActor> clone() const = 0;
};

template<typename T>
class ActorModel : public IActor { // Template model to wrap any actor
public:
    ActorModel(T actor) : m_actor(std::move(actor)) {}
    string name() const override { return m_actor.name(); }
    void act() const override { m_actor.act(); }
    unique_ptr<IActor> clone() const override {
        return make_unique<ActorModel<T>>(m_actor);
    }
private:
    T m_actor;
};

class Actor { // Value-semantic wrapper
public:
    template<typename T>
    Actor(T actor) : m_ptr(make_unique<ActorModel<T>>(std::move(actor))) {}

    Actor(const Actor& other) : m_ptr(other.m_ptr->clone()) {}
    Actor& operator=(const Actor& other) {
        if (this != &other)
            m_ptr = other.m_ptr->clone();
        return *this;
    }

    Actor(Actor&&) noexcept = default;
    Actor& operator=(Actor&&) noexcept = default;

    string name() const { return m_ptr->name(); }
    void act() const { m_ptr->act(); }

private:
    unique_ptr<IActor> m_ptr;
};

class ActorContainer { // Container that holds different actors
private:
    vector<Actor> data;
public:
    template<typename T>
    void add(T actor) {
        data.emplace_back(std::move(actor));
    }

    auto begin() { return data.begin(); }
    auto end() { return data.end(); }
    auto begin() const { return data.begin(); }
    auto end() const { return data.end(); }

    auto rbegin() { return data.rbegin(); }
    auto rend() { return data.rend(); }
};

int main() {
    ActorContainer c;

    c.add(Fireball("🔥"));
    c.add(Fireball("🔥🔥"));
    c.add(Robot("🤖"));
    c.add(Goblin("👺"));

    cout << "\n-- Forward Iteration --\n";
    for (const auto& actor : c) {
        cout << actor.name() << " performs action\n";
        actor.act();
    }

    cout << "\n-- Reverse Iteration --\n";
    for (auto it = c.rbegin(); it != c.rend(); ++it) {
        cout << it->name() << " performs action\n";
        it->act();
    }
    cout << "\n";
    
    return 0;
}
