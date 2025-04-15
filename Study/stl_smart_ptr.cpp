// zamanlantra@ZamansMcBookPro Study % g++ -std=c++17 -o stl_smart_ptr stl_smart_ptr.cpp
// zamanlantra@ZamansMcBookPro Study % ./stl_smart_ptr

// You're implementing a simple social network in C++. Each User has:
// A name.
// A list of friends.
// An optional profile object that contains details (bio, age, etc).

// Requirements:
// Use std::shared_ptr<User> to allow multiple users to have shared ownership of their friends.
// Use std::unique_ptr<Profile> to ensure each user owns their profile exclusively.
// Use std::weak_ptr<User> to avoid circular references in mutual friendships.

#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std;

class Profile {
private:
    string m_bio = "";
    int m_age = 0;
public:
    Profile() = delete;
    Profile(const string& b, int a) : m_bio(std::move(b)), m_age(a) {  }
    ~Profile() {  }
    void Print() const {
        cout << "'" << m_bio << "' age: " << m_age << "\n";
    }
};

class User {
private:
    string m_name = "";
    unique_ptr<Profile> m_profile;
    vector<weak_ptr<User>> friends;
public:
    User(const string& name) : m_name(name) { }
    void AddFriend(weak_ptr<User> fr) {
        friends.emplace_back(fr);
    }
    const string& GetName() const {
        return m_name;
    }
    const string PrintFriends() const {
        std::string s = "";
        for (const auto& weakFriend : friends) {
            if (auto f = weakFriend.lock()) {
                s += (string(" ") + f->m_name);
            } else {
                s += " [expired user]";
            }
        }
        return s;
    }
    void AddProfile(unique_ptr<Profile> prof) {
        m_profile = std::move(prof);
    }
    void PrintProfile() const {
        m_profile->Print();
    }
};

int main() {
    std::cout << "Try programiz.pro\n"; 
    
    shared_ptr<User> user1 = make_shared<User>("Zaman");
    unique_ptr<Profile> profile1 = make_unique<Profile>("Cool", 10);
    user1->AddProfile(std::move(profile1));                                             // why move? copying unique_ptr is deleted by design
    
    shared_ptr<User> user2 = make_shared<User>("Shaffika"); 
    {
        shared_ptr<User> user3 = make_shared<User>("Imalsha");          
        user1->AddFriend(user2);
        user1->AddFriend(user3);
    }                                                                                   // Imalsha will get deleted as it goes out of scope
    
    std::cout << user1->GetName() << " has friends: " << user1->PrintFriends() << "\n";  // PrintFriends wont print Imalsha as it is deleted
    user1->PrintProfile();
    std::cout << "But profile unique_ptr is : " << profile1.get() << " as it is moved\n"; 
    if (profile1.get())
        profile1->Print();
    
    return 0;
}