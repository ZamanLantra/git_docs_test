// g++ -std=c++20 StockFeedManager.cpp -o StockFeedManager

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <queue>

using namespace std;

// class StockFeedManager {
// private:
//     int timestamp = 0;
//     unordered_map<int, unordered_set<string>> user_stocks;
//     unordered_map<string, vector<pair<int, int>>> events; // pair -> timestamp, eventID
// public:
//     void addEvent(const string& stockSymbol, int eventId) {
//         events[stockSymbol].emplace_back(timestamp++, eventId);
//     }
//     void subscribe(int userId, const string& stockSymbol) {
//         user_stocks[userId].insert(stockSymbol);
//     }
//     void unsubscribe(int userId, const string& stockSymbol) {
//         user_stocks[userId].erase(stockSymbol);
//     }
//     vector<int> getUserFeed(int userId, int maxEvents) {
//         if (!user_stocks.contains(userId)) {
//             cout << "User Not Subscribed to Any Stocks\n";
//             return {};
//         }
        
//         using rev_iter = vector<pair<int, int>>::reverse_iterator;

//         priority_queue<tuple<int, int, rev_iter, rev_iter>> q; // timestamp, eventID, rev_iter, rev_iter

//         for (auto& symbol : user_stocks[userId]) {
//             auto& vec = events[symbol];
//             if (vec.empty()) continue; // No events for this stock
            
//             auto it = vec.rbegin();
//             q.emplace(it->first, it->second, it, vec.rend());
//         }
        
//         int count = 0; 
//         vector<int> result;
//         result.reserve(maxEvents);

//         while (!q.empty() && count < maxEvents) {
//             auto [ts, eventId, it, end] = q.top();
//             q.pop();
//             result.emplace_back(eventId);
            
//             ++count;
//             ++it;
//             if (it != end) {
//                 q.emplace(it->first, it->second, it, end);
//             }
//         }

//         return result;
//     }
// };


class StockFeedManager {
private:
    int timestamp = 0;
    unordered_map<int, unordered_set<string>> user_stocks;
    unordered_map<string, deque<pair<int, int>>> events; // pair -> timestamp, eventID 
public:
    void addEvent(const string& stockSymbol, int eventId) {
        events[stockSymbol].emplace_back(timestamp++, eventId);
    }
    void subscribe(int userId, const string& stockSymbol) {
        user_stocks[userId].insert(stockSymbol);
    }
    void unsubscribe(int userId, const string& stockSymbol) {
        user_stocks[userId].erase(stockSymbol);
    }
    vector<int> getUserFeed(int userId, int maxEvents) {
        if (!user_stocks.contains(userId)) {
            cout << "User Not Subscribed to Any Stocks\n";
            return {};
        }
        
        using iter = deque<pair<int, int>>::reverse_iterator;
        priority_queue<tuple<int, int, iter, iter>> q; // timestamp, eventID, iter, iter

        for (auto& symbol : user_stocks[userId]) {
            auto& structure = events[symbol];
            if (structure.empty()) continue; // No events for this stock     
            auto it = structure.rbegin(); // Start from most recent
            q.emplace(it->first, it->second, it, structure.rend());
        }

        vector<int> result;
        result.reserve(maxEvents);

        while (!q.empty() && result.size() < maxEvents) {
            auto [ts, eventId, it, end] = q.top();
            q.pop();
            result.emplace_back(eventId);

            if (++it != end) {
                q.emplace(it->first, it->second, it, end);
            }
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
