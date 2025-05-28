// g++ -std=c++20 StockFeedManager.cpp -o StockFeedManager

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <queue>

using namespace std;

class StockFeedManager {
private:
    int timestamp = 0;
    unordered_map<int, deque<int>> user_feed; // userId -> deque of eventIds
    unordered_map<int, unordered_set<string>> user_stocks; // userId -> set of stock symbols
    unordered_map<string, deque<pair<int, int>>> events; // stockSymbol -> deque of pairs (timestamp, eventId)
public:
    void addEvent(const string& stockSymbol, int eventId) {
        events[stockSymbol].emplace_back(timestamp++, eventId); // O(1)
        for (const auto& [userId, stocks] : user_stocks) {      // O(n) where n is number of users    
            if (stocks.find(stockSymbol) != stocks.end()) {
                user_feed[userId].emplace_front(eventId);       // O(1)
            }
        }
    }
    void subscribe(int userId, const string& stockSymbol) {
        user_stocks[userId].insert(stockSymbol);                // O(1)
        if (events.contains(stockSymbol)) {                     // O(1)
            for (const auto& event : events[stockSymbol]) {     // O(m) where m is number of events for the stock
                user_feed[userId].emplace_front(event.second);  // O(1)
            }
        }
    }
    void unsubscribe(int userId, const string& stockSymbol) {
        if (!user_stocks.contains(userId)) return;
        else if (user_stocks[userId].find(stockSymbol) == user_stocks[userId].end()) return;
        user_stocks[userId].erase(stockSymbol);                                 // O(1)
        auto& feed = user_feed[userId];
        for (const auto& [tm, event] : events[stockSymbol]) {                   // O(m)
            feed.erase(remove(feed.begin(), feed.end(), event), feed.end());    // O(f) where f is the size of user feed
        }
    }
    vector<int> getUserFeed(int userId, int maxEvents) {
        if (!user_feed.contains(userId)) return {};
        auto& feed = user_feed[userId];
        vector<int> result;
        result.reserve(maxEvents);
        for (int i = 0; i < maxEvents && i < feed.size(); ++i) { // O(maxEvents)
            result.push_back(feed[i]);                           // O(1)
        }
        return result;
    }
};

int main() {
    StockFeedManager s;

    s.addEvent("AAPL", 101);     // timestamp = 0
    s.addEvent("TSLA", 102);     // timestamp = 1
    s.subscribe(1, "AAPL");
    s.subscribe(1, "TSLA");
    s.addEvent("TSLA", 103);     // timestamp = 2
    s.addEvent("AAPL", 104);     // timestamp = 3

    auto feed = s.getUserFeed(1, 3); // Expected: [104, 103, 102]

    cout << "User Feed: ";
    for (int eventId : feed) {
        cout << " " << eventId;
    }
    cout << endl;

    s.unsubscribe(1, "AAPL");

    s.addEvent("TSLA", 105);
    s.addEvent("TSLA", 106);

    feed = s.getUserFeed(1, 3); // Expected: [103, 102]

    cout << "User Feed after Unsubscribe: ";
    for (int eventId : feed) {
        cout << " " << eventId;
    }
    cout << endl;
}

/*
Problem: Stock Event Feed System
You’re tasked with building a stock event feed system that monitors and processes events for multiple users. 
Each user subscribes to specific stocks, and the system must deliver a personalized event feed with the most recent updates.

Each event consists of:

stockSymbol (string)
eventId (unique int)
timestamp (automatically assigned)

You must implement a class StockFeedManager with the following operations:

🔧 Method Signatures

class StockFeedManager {
public:
    void addEvent(const string& stockSymbol, int eventId);
    void subscribe(int userId, const string& stockSymbol);
    void unsubscribe(int userId, const string& stockSymbol);
    vector<int> getUserFeed(int userId, int maxEvents);
};

📘 Description

addEvent(stockSymbol, eventId)
Adds an event for the given stock. Each new event has a globally increasing timestamp.

subscribe(userId, stockSymbol)
User subscribes to a stock.

unsubscribe(userId, stockSymbol)
User unsubscribes from a stock. Has no effect if not already subscribed.

getUserFeed(userId, maxEvents)
Returns up to maxEvents most recent eventIds (not timestamps!) for all stocks the user is subscribed to. 
Events must be sorted by descending timestamp.

💡 Constraints

All eventIds are unique and positive integers.
Up to 10⁵ users and 10⁵ stock symbols.
Up to 10⁶ total events.
Each stock keeps at most the latest 100 events.
Each user can subscribe to up to 1000 stocks.
Feed must be generated efficiently (avoid copying all events).
No two events will share the same timestamp.
*/