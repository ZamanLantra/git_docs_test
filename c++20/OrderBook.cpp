// g++ -std=c++20 OrderBook.cpp -o OrderBook
#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

class OrderBook {
private:
    size_t sequence = 0;
    map<int, int> buy_side, sell_side;   // price → quantity
    vector<string> done_trades;
public:
    void addBuy(int price, int quantity) {
        if (!sell_side.empty()) {
            auto [sell_price, sell_quantity] = *sell_side.begin();
            if (sell_price <= price) {
                cout << "Trade Happens at addBuy | Buy:" << quantity << "@" << 
                    price << " Sell:" << sell_quantity << "@" << sell_price <<  endl;
                sell_side.erase(sell_side.begin());
                done_trades.emplace_back(string("Buy ") + to_string(quantity) + "@" + to_string(sell_price));
                done_trades.emplace_back(string("Sell ") + to_string(sell_quantity) + "@" + to_string(sell_price));
                return;
            }
        }
        buy_side.emplace(-price, quantity);
    }
    void addSell(int price, int quantity) {
        if (!buy_side.empty()) {
            auto [key, buy_quantity] = *buy_side.begin();
            int buy_price = -1 * key;
            if (buy_price >= price) {
                cout << "Trade Happens at addSell | Sell:" << quantity << "@" << 
                    price << " Buy:" << buy_quantity << "@" << buy_price <<  endl;
                buy_side.erase(buy_side.begin());
                done_trades.emplace_back(string("Sell ") + to_string(quantity) + "@" + to_string(buy_price));
                done_trades.emplace_back(string("Buy ") + to_string(buy_quantity) + "@" + to_string(buy_price));
                return;
            }
        }
        sell_side.emplace(-price, quantity);
    }
    vector<string> getTrades() { // returns list of trades: "Buy 100@50" or "Sell 200@51"
        return done_trades;
    }            
};

int main() {

    OrderBook ob;
    ob.addBuy(50, 100);   // No match, added to book
    ob.addSell(49, 100);  // Matches with Buy(50,100) → Trade happens
    auto trades = ob.getTrades(); 
    for (const auto& t : trades) {
        cout << t << endl;
    }
    // Output: ["Buy 100@50", "Sell 100@49"]
}

/*
Prices: 1 to 10^6
Quantities: 1 to 10^4
Up to 10^5 total orders
Must be efficient: O(log n) per order (use appropriate containers)
*/